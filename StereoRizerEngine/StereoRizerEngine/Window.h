#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <unknwn.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define XR_USE_GRAPHICS_API_OPENGL
#define XR_USE_PLATFORM_WIN32
#include "openxr/openxr.h"
#include "openxr/openxr_platform.h"

#include "Renderer.h"
#include "GfxAPIUtils.h"
#include "OpenXRSupport.h"

class Window
{
public:
	Window(int width, int height, const char* title);
	~Window();

	void Destroy();
	void PollEvents();
	void SwapBuffers();
	void UploadModel(Model& model);
	void Run();

	int GetWidth() const;
	int GetHeight() const;

	int _width;
	int _height;
	const char* _title;
	GLFWwindow* _window;
	Renderer _leftRenderer;
	Renderer _rightRenderer;
	Model* _model = nullptr;
	void Create();
	bool _xrInitialized = false;

	OpenXRSupport _xrSupport;
	GraphicsAPI_Type m_apiType = GraphicsAPI_Type::OpenGL;
	void HandleXRFrame();
};

