#include "Window.h"

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"

Window::Window(int width, int height, const char* title)
{
	_width = width;
	_height = height;
	_title = title;
	_window = nullptr;
	Create();
	_leftRenderer = Renderer();
	_rightRenderer = Renderer();
}

Window::~Window()
{
	Destroy();
}

void Window::Destroy()
{
	if (_window)
	{
		glfwDestroyWindow(_window);
		glfwTerminate();
		_window = nullptr;
	}
}

void Window::PollEvents()
{
	glfwPollEvents();
}

void Window::SwapBuffers()
{
	glfwSwapBuffers(_window);
}

void Window::Run(Model& model)
{
	while (!glfwWindowShouldClose(_window))
	{
		glfwGetFramebufferSize(_window, &_width, &_height);

		glClear(GL_COLOR_BUFFER_BIT);

		PollXrEvents();

		XrResult res;
		// 1. Get OpenXR frame data
		XrFrameState frameState{ XR_TYPE_FRAME_STATE };
		res = xrWaitFrame(xrSession, nullptr, &frameState);
		if (XR_FAILED(res)) {
			std::cerr << "Failed to wait for frame\n";
			return;
		}
		res = xrBeginFrame(xrSession, nullptr);

		if (XR_FAILED(res)) {
			std::cerr << "Failed to begin frame\n";
			return;
		}

		// Example: get views for left/right eye
		uint32_t viewCount = 2;
		XrView views[2]{ {XR_TYPE_VIEW}, {XR_TYPE_VIEW} };
		XrViewLocateInfo locateInfo{ XR_TYPE_VIEW_LOCATE_INFO };
		locateInfo.viewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
		locateInfo.displayTime = frameState.predictedDisplayTime;
		locateInfo.space = xrAppSpace;

		XrViewState viewState{ XR_TYPE_VIEW_STATE };
		uint32_t viewCountOutput;
		res = xrLocateViews(xrSession, &locateInfo, &viewState, viewCount, &viewCountOutput, views);

		if (XR_FAILED(res)) {
			std::cerr << "Failed to locate views\n";
			return;
		}

		glm::mat4 leftView = ConvertXrPoseToMat4(views[0].pose);
		glm::mat4 leftProj = ConvertXrFovToProj(views[0].fov, 0.1f, 100.0f);
		_leftRenderer.SetCamera(leftView, leftProj);

		glm::mat4 rightView = ConvertXrPoseToMat4(views[1].pose);
		glm::mat4 rightProj = ConvertXrFovToProj(views[1].fov, 0.1f, 100.0f);
		_rightRenderer.SetCamera(rightView, rightProj);

		glViewport(0, 0, _width / 2, _height);
		_leftRenderer.Draw(model);

		glViewport(_width/ 2, 0, _width / 2, _height);
		_rightRenderer.Draw(model);

		// Swap front and back buffers
		SwapBuffers();

		// Poll for and process events
		PollEvents();

		//xrEndFrame(xrSession, nullptr);
	}

}

int Window::GetWidth() const
{
	return _width;
}

int Window::GetHeight() const
{
	return _height;
}

void Window::Create()
{
	/* Initialize the library */
	if (!glfwInit()) {
		std::cout << "Error initializing GLFW" << std::endl;
		return;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	_window = glfwCreateWindow(_width, _height, _title, NULL, NULL);
	if (!_window)
	{
		glfwTerminate();
		return;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(_window);

	//glfwSetFramebufferSizeCallback(_window, Window::framebuffer_size_callback);

	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		std::cerr << "GLEW Error: " << glewGetErrorString(err) << std::endl;
		return;
	}

	InitOpenXR();
}

void Window::InitOpenXR()
{
	XrApplicationInfo AI;
	strncpy(AI.applicationName, "OpenXR Tutorial Chapter 2", XR_MAX_APPLICATION_NAME_SIZE);
	AI.applicationVersion = 1;
	strncpy(AI.engineName, "OpenXR Engine", XR_MAX_ENGINE_NAME_SIZE);
	AI.engineVersion = 1;
	AI.apiVersion = XR_CURRENT_API_VERSION;

	m_instanceExtensions.push_back(XR_EXT_DEBUG_UTILS_EXTENSION_NAME);
	// Ensure m_apiType is already defined when we call this line.
	m_instanceExtensions.push_back(GetGraphicsAPIInstanceExtensionString(m_apiType));

	XrResult result;
	// Get all the API Layers from the OpenXR runtime.
	uint32_t apiLayerCount = 0;
	std::vector<XrApiLayerProperties> apiLayerProperties;
	result = xrEnumerateApiLayerProperties(0, &apiLayerCount, nullptr);
	if (result != XR_SUCCESS) {
		std::cerr << "Failed to enumerate API layer properties." << std::endl;
		return;
	}
	apiLayerProperties.resize(apiLayerCount, { XR_TYPE_API_LAYER_PROPERTIES });
	result = xrEnumerateApiLayerProperties(apiLayerCount, &apiLayerCount, apiLayerProperties.data());
	if (result != XR_SUCCESS) {
		std::cerr << "Failed to enumerate API layer properties." << std::endl;
		return;
	}

	// Check the requested API layers against the ones from the OpenXR. If found add it to the Active API Layers.
	for (auto& requestLayer : m_apiLayers) {
		for (auto& layerProperty : apiLayerProperties) {
			// strcmp returns 0 if the strings match.
			if (strcmp(requestLayer.c_str(), layerProperty.layerName) != 0) {
				continue;
			}
			else {
				m_activeAPILayers.push_back(requestLayer.c_str());
				break;
			}
		}
	}

	// Get all the Instance Extensions from the OpenXR instance.
	uint32_t extensionCount = 0;
	std::vector<XrExtensionProperties> extensionProperties;
	xrEnumerateInstanceExtensionProperties(nullptr, 0, &extensionCount, nullptr);
	extensionProperties.resize(extensionCount, { XR_TYPE_EXTENSION_PROPERTIES });
	xrEnumerateInstanceExtensionProperties(nullptr, extensionCount, &extensionCount, extensionProperties.data());

	// Check the requested Instance Extensions against the ones from the OpenXR runtime.
	// If an extension is found add it to Active Instance Extensions.
	// Log error if the Instance Extension is not found.
	for (auto& requestedInstanceExtension : m_instanceExtensions) {
		bool found = false;
		for (auto& extensionProperty : extensionProperties) {
			// strcmp returns 0 if the strings match.
			if (strcmp(requestedInstanceExtension.c_str(), extensionProperty.extensionName) != 0) {
				continue;
			}
			else {
				m_activeInstanceExtensions.push_back(requestedInstanceExtension.c_str());
				found = true;
				break;
			}
		}
		if (!found) {
			std::cerr<< "Failed to find OpenXR instance extension: " << requestedInstanceExtension << std::endl;
		}
	}

	XrInstanceCreateInfo instanceCI{ XR_TYPE_INSTANCE_CREATE_INFO };
	instanceCI.createFlags = 0;
	instanceCI.applicationInfo = AI;
	instanceCI.enabledApiLayerCount = static_cast<uint32_t>(m_activeAPILayers.size());
	instanceCI.enabledApiLayerNames = m_activeAPILayers.data();
	instanceCI.enabledExtensionCount = static_cast<uint32_t>(m_activeInstanceExtensions.size());
	instanceCI.enabledExtensionNames = m_activeInstanceExtensions.data();
	result = xrCreateInstance(&instanceCI, &xrInstance);

	if (result != XR_SUCCESS) {
		std::cerr << "Failed to create XR instance." << std::endl;
		std::cerr << "Error code: " << result << std::endl;
		return;
	}
	else {
		std::cout << "XR Instance created successfully." << std::endl;
	}

	// Get system ID (headset)
	XrSystemGetInfo systemInfo{ XR_TYPE_SYSTEM_GET_INFO };
	systemInfo.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
	result = xrGetSystem(xrInstance, &systemInfo, &xrSystemId);
	if (XR_FAILED(result)) {
		std::cerr << "Failed to get OpenXR system\n";
		return;
	}

	// Load graphics requirements function
	PFN_xrGetOpenGLGraphicsRequirementsKHR pfnGetOpenGLGraphicsRequirementsKHR = nullptr;
	xrGetInstanceProcAddr(
		xrInstance,
		"xrGetOpenGLGraphicsRequirementsKHR",
		reinterpret_cast<PFN_xrVoidFunction*>(&pfnGetOpenGLGraphicsRequirementsKHR)
	);

	// Query requirements
	XrGraphicsRequirementsOpenGLKHR graphicsRequirements{ XR_TYPE_GRAPHICS_REQUIREMENTS_OPENGL_KHR };
	result = pfnGetOpenGLGraphicsRequirementsKHR(xrInstance, xrSystemId, &graphicsRequirements);
	if (XR_FAILED(result)) {
		std::cerr << "Failed to get OpenGL graphics requirements\n";
		return;
	}

	// Create session (using OpenGL graphics binding)
	XrGraphicsBindingOpenGLWin32KHR graphicsBinding{ XR_TYPE_GRAPHICS_BINDING_OPENGL_WIN32_KHR };
	graphicsBinding.hDC = wglGetCurrentDC();
	graphicsBinding.hGLRC = wglGetCurrentContext();

	XrSessionCreateInfo sessionCreateInfo{ XR_TYPE_SESSION_CREATE_INFO };
	sessionCreateInfo.next = &graphicsBinding;
	sessionCreateInfo.systemId = xrSystemId;

	result = xrCreateSession(xrInstance, &sessionCreateInfo, &xrSession);
	if (XR_FAILED(result)) {
		std::cerr << "Failed to create OpenXR session\n";
		return;
	}

	XrReferenceSpaceCreateInfo spaceInfo{ XR_TYPE_REFERENCE_SPACE_CREATE_INFO };
	spaceInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_STAGE;
	spaceInfo.poseInReferenceSpace = { {0,0,0,1}, {0,0,0} };

	result = xrCreateReferenceSpace(xrSession, &spaceInfo, &xrAppSpace);
	if (XR_FAILED(result)) {
		std::cerr << "Failed to create reference space\n";
		return;
	}
}

glm::mat4 Window::ConvertXrPoseToMat4(const XrPosef& pose)
{
	// Convert orientation and position to GLM types
	glm::quat orientation(pose.orientation.w, pose.orientation.x, pose.orientation.y, pose.orientation.z);
	glm::vec3 position(pose.position.x, pose.position.y, pose.position.z);

	// Build the transform (world-space pose)
	glm::mat4 rotation = glm::mat4_cast(orientation);
	glm::mat4 translation = glm::translate(glm::mat4(1.0f), position);
	glm::mat4 worldFromPose = translation * rotation;

	// Return inverse for view matrix (world → view)
	return glm::inverse(worldFromPose);
}

glm::mat4 Window::ConvertXrFovToProj(const XrFovf& fov, float nearZ, float farZ)
{
	float tanLeft = tan(fov.angleLeft);
	float tanRight = tan(fov.angleRight);
	float tanUp = tan(fov.angleUp);
	float tanDown = tan(fov.angleDown);

	float width = tanRight - tanLeft;
	float height = tanUp - tanDown;

	glm::mat4 proj(0.0f);
	proj[0][0] = 2.0f / width;
	proj[1][1] = 2.0f / height;
	proj[2][0] = (tanRight + tanLeft) / width;
	proj[2][1] = (tanUp + tanDown) / height;
	proj[2][2] = -(farZ + nearZ) / (farZ - nearZ);
	proj[2][3] = -1.0f;
	proj[3][2] = -(2.0f * farZ * nearZ) / (farZ - nearZ);
	return proj;
}

void Window::PollXrEvents()
{
	XrEventDataBuffer eventData{ XR_TYPE_EVENT_DATA_BUFFER };
	while (xrPollEvent(xrInstance, &eventData) == XR_SUCCESS) {
		switch (eventData.type) {
		case XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED: {
			auto* sessionStateChanged = reinterpret_cast<XrEventDataSessionStateChanged*>(&eventData);
			currentState = sessionStateChanged->state;

			if (currentState == XR_SESSION_STATE_READY) {
				XrSessionBeginInfo beginInfo{ XR_TYPE_SESSION_BEGIN_INFO };
				beginInfo.primaryViewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
				xrBeginSession(xrSession, &beginInfo);
				std::cout << "XR session started!" << std::endl;
			}
			else if (currentState == XR_SESSION_STATE_STOPPING) {
				xrEndSession(xrSession);
				std::cout << "XR session stopping." << std::endl;
			}
			else if (currentState == XR_SESSION_STATE_EXITING || currentState == XR_SESSION_STATE_LOSS_PENDING) {
				std::cout << "XR session exiting or lost." << std::endl;
			}
			break;
		}
		default:
			break;
		}

		// Reset structure for next poll
		eventData = { XR_TYPE_EVENT_DATA_BUFFER };
	}
}
