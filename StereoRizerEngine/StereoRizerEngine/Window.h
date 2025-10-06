#pragma once
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Renderer.h"

class Window
{
public:
	Window(int width, int height, const char* title);
	~Window();

	void Destroy();
	void PollEvents();
	void SwapBuffers();
	void Run(Mesh& mesh, Shader& shader);

	int GetWidth() const;
	int GetHeight() const;

private:
	int _width;
	int _height;
	const char* _title;
	GLFWwindow* _window;
	void Create();
	Renderer _renderer;
};

