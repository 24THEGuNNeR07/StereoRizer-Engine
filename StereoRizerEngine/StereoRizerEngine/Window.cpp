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
	_renderer = Renderer();
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

void Window::Run(Mesh& mesh, Shader& shader)
{
	while (!glfwWindowShouldClose(_window))
	{
		_renderer.Clear();

		_renderer.Draw(mesh, shader);

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

	_window = glfwCreateWindow(_width, _height, _title, NULL, NULL);
	if (!_window)
	{
		glfwTerminate();
		return;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(_window);

	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		std::cerr << "GLEW Error: " << glewGetErrorString(err) << std::endl;
		return;
	}
}
