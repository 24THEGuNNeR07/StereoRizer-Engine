#include "OpenXRSupport.h"
#include "Common.h"

OpenXRSupport::OpenXRSupport()
{
}

OpenXRSupport::~OpenXRSupport()
{
}

bool OpenXRSupport::Init(GraphicsAPI_Type apiType)
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
		LOG_ERROR("Failed to enumerate API layer properties.");
		return false;
	}
	apiLayerProperties.resize(apiLayerCount, { XR_TYPE_API_LAYER_PROPERTIES });
	result = xrEnumerateApiLayerProperties(apiLayerCount, &apiLayerCount, apiLayerProperties.data());
	if (result != XR_SUCCESS) {
		LOG_ERROR("Failed to enumerate API layer properties.");
		return false;
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
			LOG_ERROR(std::string("Failed to find OpenXR instance extension: ") + requestedInstanceExtension);
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
		LOG_ERROR("Failed to create XR instance.");
		LOG_ERROR(std::string("Error code: ") + std::to_string((int)result));
		return false;
	}
	else {
		LOG_INFO("XR Instance created successfully.");
	}

	// Get system ID (headset)
	XrSystemGetInfo systemInfo{ XR_TYPE_SYSTEM_GET_INFO };
	systemInfo.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
	result = xrGetSystem(xrInstance, &systemInfo, &xrSystemId);
	if (XR_FAILED(result)) {
		LOG_ERROR("Failed to get OpenXR system");
		return false;
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
		LOG_ERROR("Failed to get OpenGL graphics requirements");
		return false;
	}

	// Create session (using OpenGL graphics binding)
	XrGraphicsBindingOpenGLWin32KHR graphicsBinding{ XR_TYPE_GRAPHICS_BINDING_OPENGL_WIN32_KHR };
	graphicsBinding.hDC = wglGetCurrentDC();
	graphicsBinding.hGLRC = wglGetCurrentContext();

	xrSessionGLRC = graphicsBinding.hGLRC;
	xrSessionDC = graphicsBinding.hDC;

	XrSessionCreateInfo sessionCreateInfo{ XR_TYPE_SESSION_CREATE_INFO };
	sessionCreateInfo.next = &graphicsBinding;
	sessionCreateInfo.systemId = xrSystemId;

	result = xrCreateSession(xrInstance, &sessionCreateInfo, &xrSession);
	if (XR_FAILED(result)) {
		LOG_ERROR("Failed to create OpenXR session");
		return false;
	}

	XrReferenceSpaceCreateInfo spaceInfo{ XR_TYPE_REFERENCE_SPACE_CREATE_INFO };
	spaceInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_STAGE;
	spaceInfo.poseInReferenceSpace = { {0,0,0,1}, {0,0,0} };

	result = xrCreateReferenceSpace(xrSession, &spaceInfo, &xrAppSpace);
	if (XR_FAILED(result)) {
		LOG_ERROR("Failed to create reference space");
		return false;
	}

	CreateXRSwapchains();

	return true;
}

void OpenXRSupport::PollEvents()
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
				LOG_INFO("XR session started!");
			}
			else if (currentState == XR_SESSION_STATE_STOPPING) {
				xrEndSession(xrSession);
				LOG_INFO("XR session stopping.");
			}
			else if (currentState == XR_SESSION_STATE_EXITING || currentState == XR_SESSION_STATE_LOSS_PENDING) {
				LOG_INFO("XR session exiting or lost.");
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

glm::mat4 OpenXRSupport::ConvertXrPoseToMat4(int eyeIndex)
{
	// Convert orientation and position to GLM types
	glm::quat orientation(views[eyeIndex].pose.orientation.w, 
						  views[eyeIndex].pose.orientation.x, 
						  views[eyeIndex].pose.orientation.y, 
						  views[eyeIndex].pose.orientation.z);
	glm::vec3 position(views[eyeIndex].pose.position.x, views[eyeIndex].pose.position.y, views[eyeIndex].pose.position.z);

	// Build the transform (world-space pose)
	glm::mat4 rotation = glm::mat4_cast(orientation);
	glm::mat4 translation = glm::translate(glm::mat4(1.0f), position);
	glm::mat4 worldFromPose = translation * rotation;

	// Return inverse for view matrix (world → view)
	return glm::inverse(worldFromPose);
}

glm::mat4 OpenXRSupport::ConvertXrFovToProj(int eyeIndex, float nearZ, float farZ)
{
	float tanLeft = tan(views[eyeIndex].fov.angleLeft);
	float tanRight = tan(views[eyeIndex].fov.angleRight);
	float tanUp = tan(views[eyeIndex].fov.angleUp);
	float tanDown = tan(views[eyeIndex].fov.angleDown);

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

bool OpenXRSupport::CopyFramebufferToSwapchain(GLuint srcFbo,
											   GLint srcX, 
											   GLint srcY, 
											   GLsizei srcW, 
											   GLsizei srcH, 
											   XrSwapchainData& swapchain, 
											   uint32_t imageIndex, 
											   GLuint dstFboReuse)
{
	// Get the swapchain texture
	GLuint dstTex = swapchain.images[imageIndex].image;
	if (dstTex == 0) {
		LOG_ERROR("Swapchain image is invalid");
		return false;
	}
	// Create a temporary FBO on the correct context
	GLuint dstFbo = 0;
	glGenFramebuffers(1, &dstFbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dstFbo);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dstTex, 0);

	if (glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		LOG_ERROR("Swapchain FBO incomplete");
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glDeleteFramebuffers(1, &dstFbo);
		return false;
	}

	// Bind source FBO for reading
	glBindFramebuffer(GL_READ_FRAMEBUFFER, srcFbo);
	if (srcFbo == 0) {
		glReadBuffer(GL_BACK); // default framebuffer
	}

	// Blit from src to swapchain
	glBlitFramebuffer(
		srcX, srcY, srcX + srcW, srcY + srcH,    // src rect
		0, 0, swapchain.width, swapchain.height, // dst rect
		GL_COLOR_BUFFER_BIT,
		GL_LINEAR);

	// 6Clean up
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &dstFbo);

	glFlush(); // ensure GPU starts processing before xrReleaseSwapchainImage


	return true;
}

void OpenXRSupport::InitLoop(int width, int height)
{
	// persistent dstFbo (create once)
	xrDstFbo = 0;
	glGenFramebuffers(1, &xrDstFbo);

	srcFbo = 0;
	int srcWidth = width;
	int srcHeight = height;
}

void OpenXRSupport::WaitFrame()
{
	XrResult res;
	
	res = xrWaitFrame(xrSession, nullptr, &frameState);
	if (XR_FAILED(res)) {
		LOG_ERROR(std::string("xrWaitFrame failed: ") + std::to_string((int)res));
	}
}

void OpenXRSupport::BeginFrame()
{
	XrResult res;
	res = xrBeginFrame(xrSession, nullptr);
	if (XR_FAILED(res)) {
		LOG_ERROR(std::string("xrBeginFrame failed: ") + std::to_string((int)res));
	}
}

void OpenXRSupport::CreateXRSwapchains()
{
	uint32_t viewCountOutput;

	// First call — query how many views
	XrResult result = xrEnumerateViewConfigurationViews(
		xrInstance,
		xrSystemId,
		XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO,
		0,
		&viewCountOutput,
		nullptr);

	if (XR_FAILED(result)) {
		LOG_ERROR(std::string("First xrEnumerateViewConfigurationViews failed: ") + std::to_string((int)result));
		return;
	}

	std::vector<XrViewConfigurationView> configViews(
		viewCountOutput,
		{ XR_TYPE_VIEW_CONFIGURATION_VIEW });

	result = xrEnumerateViewConfigurationViews(
		xrInstance,
		xrSystemId,
		XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO,
		viewCountOutput,
		&viewCountOutput,
		configViews.data());

	if (XR_FAILED(result)) {
		LOG_ERROR(std::string("Second xrEnumerateViewConfigurationViews failed: ") + std::to_string((int)result));
		return;
	}

	for (uint32_t i = 0; i < viewCountOutput; i++) {
		XrViewConfigurationView viewConfig = configViews[i];
		XrSwapchainCreateInfo swapchainInfo{ XR_TYPE_SWAPCHAIN_CREATE_INFO };
		swapchainInfo.arraySize = 1;
		swapchainInfo.format = GL_SRGB8_ALPHA8; // or whatever your renderer uses
		swapchainInfo.width = viewConfig.recommendedImageRectWidth;
		swapchainInfo.height = viewConfig.recommendedImageRectHeight;
		swapchainInfo.mipCount = 1;
		swapchainInfo.faceCount = 1;
		swapchainInfo.sampleCount = 1;
		swapchainInfo.usageFlags = XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT;

		XrSwapchain swapchain;
		xrCreateSwapchain(xrSession, &swapchainInfo, &swapchain);

		_swapchains[i].handle = swapchain;
		_swapchains[i].width = swapchainInfo.width;
		_swapchains[i].height = swapchainInfo.height;

		// Get OpenGL images
		uint32_t imageCount = 0;
		xrEnumerateSwapchainImages(swapchain, 0, &imageCount, nullptr);
		_swapchains[i].images.resize(imageCount, { XR_TYPE_SWAPCHAIN_IMAGE_OPENGL_KHR });
		xrEnumerateSwapchainImages(swapchain, imageCount, &imageCount,
			reinterpret_cast<XrSwapchainImageBaseHeader*>(_swapchains[i].images.data()));
	}

	LOG_INFO("Swapchains created for both eyes.");
}

void OpenXRSupport::LocateViews()
{
	XrResult res;

	locateInfo.viewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
	locateInfo.displayTime = frameState.predictedDisplayTime;
	locateInfo.space = xrAppSpace;

	uint32_t viewCountOutput = 0;
	res = xrLocateViews(xrSession, &locateInfo, &viewState, 2, &viewCountOutput, views);
	if (XR_FAILED(res) || viewCountOutput < 2) {
		LOG_ERROR(std::string("xrLocateViews failed or returned <2 views: ") + std::to_string((int)res) + ", count=" + std::to_string((int)viewCountOutput));
	}
}

void OpenXRSupport::SetFrameSize(int width, int height)
{
	srcWidth = width;
	srcHeight = height;
}

void OpenXRSupport::CopyFrameBuffer()
{
	XrResult res;

	// Prepare layerViews
	std::vector<XrCompositionLayerProjectionView> layerViews(2, { XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW });

	// For each eye: acquire swapchain image, blit the correct half, release, and fill layerViews
	for (uint32_t eye = 0; eye < 2; ++eye)
	{
		XrSwapchainData& sc = _swapchains[eye];
		if (sc.handle == XR_NULL_HANDLE || sc.images.empty()) {
			LOG_ERROR(std::string("Swapchain for eye ") + std::to_string((int)eye) + " invalid");
			continue;
		}

		// Acquire image
		uint32_t imageIndex = 0;
		XrSwapchainImageAcquireInfo acquireInfo{ XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO };
		res = xrAcquireSwapchainImage(sc.handle, &acquireInfo, &imageIndex);
		if (XR_FAILED(res)) {
			LOG_ERROR(std::string("xrAcquireSwapchainImage failed for eye ") + std::to_string((int)eye) + ": " + std::to_string((int)res));
			continue;
		}

		// Wait for image ready
		XrSwapchainImageWaitInfo waitInfo{ XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO };
		waitInfo.timeout = XR_INFINITE_DURATION;
		res = xrWaitSwapchainImage(sc.handle, &waitInfo);
		if (XR_FAILED(res)) {
			LOG_ERROR(std::string("xrWaitSwapchainImage failed for eye ") + std::to_string((int)eye) + ": " + std::to_string((int)res));
			// best effort release
			XrSwapchainImageReleaseInfo releaseInfo{ XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO };
			xrReleaseSwapchainImage(sc.handle, &releaseInfo);
			continue;
		}

		// Determine source rectangle in the window/backbuffer for this eye
		// We rendered left eye into left half (0 .. _width/2) and right eye into right half (_width/2 .. _width)
		GLint srcX = (eye == 0) ? 0 : (srcWidth / 2);
		GLint srcY = 0;
		GLsizei srcW = srcWidth / 2;
		GLsizei srcH = srcHeight;

		// Blit from window backbuffer (READ) to swapchain texture (DRAW)
		bool ok = CopyFramebufferToSwapchain(
			srcFbo, srcX, srcY, srcW, srcH,
			sc, imageIndex, xrDstFbo);

		if (!ok) {
			LOG_ERROR(std::string("CopyFramebufferToSwapchainByBlit failed for eye ") + std::to_string((int)eye));
			// fallback: you could render a fullscreen quad into swapchain texture using src as texture
		}

		// Release swapchain image
		XrSwapchainImageReleaseInfo releaseInfo{ XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO };
		res = xrReleaseSwapchainImage(sc.handle, &releaseInfo);
		if (XR_FAILED(res)) {
			LOG_ERROR(std::string("xrReleaseSwapchainImage failed for eye ") + std::to_string((int)eye) + ": " + std::to_string((int)res));
		}

		// Fill composition layer view
		layerViews[eye].pose = views[eye].pose;
		layerViews[eye].fov = views[eye].fov;
		layerViews[eye].subImage.swapchain = sc.handle;
		layerViews[eye].subImage.imageRect.offset = { 0, 0 };
		layerViews[eye].subImage.imageRect.extent = { sc.width, sc.height };
	} // end for eyes

	// Submit frame to XR runtime
	XrCompositionLayerProjection layer{ XR_TYPE_COMPOSITION_LAYER_PROJECTION };
	layer.space = xrAppSpace;
	layer.viewCount = (uint32_t)layerViews.size();
	layer.views = layerViews.data();

	const XrCompositionLayerBaseHeader* layers[] = {
		reinterpret_cast<const XrCompositionLayerBaseHeader*>(&layer)
	};

	XrFrameEndInfo endInfo{ XR_TYPE_FRAME_END_INFO };
	endInfo.displayTime = frameState.predictedDisplayTime;
	endInfo.environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE;
	endInfo.layerCount = 1;
	endInfo.layers = layers;

	res = xrEndFrame(xrSession, &endInfo);
	if (XR_FAILED(res)) {
		LOG_ERROR(std::string("xrEndFrame failed: ") + std::to_string((int)res));
	}
}

void OpenXRSupport::EndLoop()
{
	glDeleteFramebuffers(1, &xrDstFbo);
}
