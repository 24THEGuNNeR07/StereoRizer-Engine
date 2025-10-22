#include "core/Window.h"
#include "graphics/Shader.h"
#include "graphics/Model.h"
#include "core/Common.h"
#include "graphics/Renderer.h"
#include "xr/OpenXRSupport.h"

Window::Window(int width, int height, const char* title)
{
	_width = width;
	_height = height;
	_title = title;
	// initialize unique_ptr with nullptr and custom deleter
	_window = std::unique_ptr<GLFWwindow, std::function<void(GLFWwindow*)>>(nullptr, [](GLFWwindow* w){ if (w) glfwDestroyWindow(w); });
	Create();
	_leftRenderer = std::make_unique<Renderer>();
	_rightRenderer = std::make_unique<Renderer>();
}

Window::~Window()
{
	Destroy();
}

void Window::Destroy()
{
	if (_window)
	{
		// unique_ptr custom deleter will call glfwDestroyWindow
		_window.reset();
		glfwTerminate();
	}
}

void Window::PollEvents()
{
	glfwPollEvents();
}

void Window::SwapBuffers()
{
	glfwSwapBuffers(_window.get());
}

void Window::AddModel(std::shared_ptr<Model> model)
{
	if (!model) return;
	if (std::find(_models.begin(), _models.end(), model) == _models.end())
		_models.push_back(model);
}

void Window::RemoveModel(std::shared_ptr<Model> model)
{
	if (!model) return;
	auto it = std::find(_models.begin(), _models.end(), model);
	if (it != _models.end())
		_models.erase(it);
}

void Window::UpdateXRViews()
{
	if (!_xrInitialized)
		return;

	_xrSupport.PollEvents();
	_xrSupport.WaitFrame();
	_xrSupport.BeginFrame();
	_xrSupport.LocateViews();

	// Update left camera
	glm::mat4 leftView = _xrSupport.ConvertXrPoseToMat4(0);
	glm::mat4 leftProj = _xrSupport.ConvertXrFovToProj(0, 0.1f, 100.0f);
	_leftRenderer->GetCamera()->SetViewMatrix(leftView);
	_leftRenderer->GetCamera()->SetProjectionMatrix(leftProj);

	// Update right camera
	glm::mat4 rightView = _xrSupport.ConvertXrPoseToMat4(1);
	glm::mat4 rightProj = _xrSupport.ConvertXrFovToProj(1, 0.1f, 100.0f);
	_rightRenderer->GetCamera()->SetViewMatrix(rightView);
	_rightRenderer->GetCamera()->SetProjectionMatrix(rightProj);
}

void Window::RenderModelsLeft()
{
	if (!_leftRenderer) return;
	for (auto& m : _models)
	{
		if (m)
			_leftRenderer->Draw(m);
	}
}

void Window::RenderModelsRight()
{
	if (!_rightRenderer) return;
	for (auto& m : _models)
	{
		if (m)
			_rightRenderer->Draw(m);
	}
}

void Window::Run()
{
	if (_xrInitialized)
		_xrSupport.InitLoop(_width, _height);

	while (!glfwWindowShouldClose(_window.get()))
	{
		glfwMakeContextCurrent(_window.get()); // force same context before every frame

		glfwGetFramebufferSize(_window.get(), &_width, &_height);
        
		if (_xrInitialized)
			_xrSupport.SetFrameSize(_width, _height);

		// Clear window backbuffer (we render the app into backbuffer halves)
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, _width, _height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (_xrInitialized)
			UpdateXRViews();

	glViewport(0, 0, _width / 2, _height);
	RenderModelsLeft();

	glViewport(_width / 2, 0, _width / 2, _height);
	RenderModelsRight();

		// Ensure all draws are finished into backbuffer before we read from it
		// (glFlush should be sufficient usually; use glFinish for debugging)
		glFlush();

		if (_xrInitialized)
			_xrSupport.CopyFrameBuffer();
        
		SwapBuffers();

		PollEvents();
	} // end while

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

	_window = std::unique_ptr<GLFWwindow, void(*)(GLFWwindow*)>(nullptr, [](GLFWwindow* w){ if (w) glfwDestroyWindow(w); });
	GLFWwindow* raw = glfwCreateWindow(_width, _height, _title, NULL, NULL);
	_window.reset(raw);
	glfwMakeContextCurrent(_window.get());
	glewInit();

	// DEBUG: Check GPU
	LOG_INFO(std::string("GL Renderer: ") + reinterpret_cast<const char*>(glGetString(GL_RENDERER)));

	// Now create OpenXR session, passing this context in graphics binding
	_xrInitialized = _xrSupport.Init(m_apiType);
}
