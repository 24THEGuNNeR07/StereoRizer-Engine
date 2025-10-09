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
	// Create OpenXR instance
	XrInstanceCreateInfo createInfo{ XR_TYPE_INSTANCE_CREATE_INFO };
	strcpy(createInfo.applicationInfo.applicationName, "StereoRizer");
	createInfo.applicationInfo.applicationVersion = 1;
	strcpy(createInfo.applicationInfo.engineName, "StereoRizerEngine");
	createInfo.applicationInfo.engineVersion = 1;
	createInfo.applicationInfo.apiVersion = XR_CURRENT_API_VERSION;

	XrResult result = xrCreateInstance(&createInfo, &xrInstance);
	if (XR_FAILED(result)) {
		std::cerr << "Failed to create OpenXR instance\n";
		return;
	}

	// Get system ID (headset)
	XrSystemGetInfo systemInfo{ XR_TYPE_SYSTEM_GET_INFO };
	systemInfo.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
	result = xrGetSystem(xrInstance, &systemInfo, &xrSystemId);
	if (XR_FAILED(result)) {
		std::cerr << "Failed to get OpenXR system\n";
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

	// Create app space
	XrReferenceSpaceCreateInfo spaceCreateInfo{ XR_TYPE_REFERENCE_SPACE_CREATE_INFO };
	spaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_STAGE;
	spaceCreateInfo.poseInReferenceSpace = { {0,0,0,1}, {0,0,0} };

	xrCreateReferenceSpace(xrSession, &spaceCreateInfo, &xrAppSpace);

	_xrInitialized = true;
}
