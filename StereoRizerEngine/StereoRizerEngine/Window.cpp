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
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		glViewport(0, 0, _width / 2, _height);
		_leftRenderer.Draw(model);

		glViewport(_width/ 2, 0, _width / 2, _height);
		_rightRenderer.Draw(model);

		// Swap front and back buffers
		SwapBuffers();

		// Poll for and process events
		PollEvents();
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
}
