#include "Window.h"
#include "Shader.h"
#include "Common.h"

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

void Window::UploadModel(Model& model)
{
	if (_model) delete _model;
	_model = new Model(std::move(model));
}

void Window::Run()
{
	if (_xrInitialized)
		_xrSupport.InitLoop(_width, _height);

	while (!glfwWindowShouldClose(_window))
	{
		glfwMakeContextCurrent(_window); // force same context before every frame

		glfwGetFramebufferSize(_window, &_width, &_height);

		if (_xrInitialized)
			_xrSupport.SetFrameSize(_width, _height);

		// Clear window backbuffer (we render the app into backbuffer halves)
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, _width, _height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (_xrInitialized)
			HandleXRFrame();

		if (_model) {
			glViewport(0, 0, _width / 2, _height);
			_leftRenderer.Draw(*_model);
			glViewport(_width / 2, 0, _width / 2, _height);
			_rightRenderer.Draw(*_model);
		}

		// Ensure all draws are finished into backbuffer before we read from it
		glFlush();

		if (_xrInitialized)
			_xrSupport.CopyFrameBuffer();

		SwapBuffers();
		PollEvents();
	}

	// cleanup dstFbo
	if (_xrInitialized)
		_xrSupport.EndLoop();
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
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	_window = glfwCreateWindow(_width, _height, _title, NULL, NULL);
	glfwMakeContextCurrent(_window);

	glewInit();

	// DEBUG: Check GPU
	LOG_INFO(std::string("GL Renderer: ") + reinterpret_cast<const char*>(glGetString(GL_RENDERER)));

	// Now create OpenXR session, passing this context in graphics binding
	_xrInitialized = _xrSupport.Init(m_apiType);
}

// Helper for XR frame lifecycle and camera setup
void Window::HandleXRFrame() {
	_xrSupport.PollEvents();
	_xrSupport.WaitFrame();
	_xrSupport.BeginFrame();
	_xrSupport.LocateViews();

	// Set left camera
	glm::mat4 leftView = _xrSupport.ConvertXrPoseToMat4(0);
	glm::mat4 leftProj = _xrSupport.ConvertXrFovToProj(0, 0.1f, 100.0f);
	_leftRenderer.GetCamera().SetViewMatrix(leftView);
	_leftRenderer.GetCamera().SetProjectionMatrix(leftProj);

	// Set right camera
	glm::mat4 rightView = _xrSupport.ConvertXrPoseToMat4(1);
	glm::mat4 rightProj = _xrSupport.ConvertXrFovToProj(1, 0.1f, 100.0f);
	_rightRenderer.GetCamera().SetViewMatrix(rightView);
	_rightRenderer.GetCamera().SetProjectionMatrix(rightProj);
}
