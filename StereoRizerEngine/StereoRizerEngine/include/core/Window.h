#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <unknwn.h>

#include <glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define XR_USE_GRAPHICS_API_OPENGL
#define XR_USE_PLATFORM_WIN32
#include "openxr/openxr.h"
#include "openxr/openxr_platform.h"

#include "graphics/Renderer.h"
#include "graphics/GfxAPIUtils.h"
#include <graphics/AssimpModel.h>
#include "xr/OpenXRSupport.h"
#include <vector>
#include <algorithm>
#include <memory>
#include <functional>
#include <vector>
#include <algorithm>

using namespace stereorizer::graphics;

namespace stereorizer::core
{ 
	class Window
	{
	public:
		Window(int width, int height, const char* title);
		~Window();

		void Destroy();
		void PollEvents();
		void SwapBuffers();
		void Run();

		// Manage scene models owned by the application (Window stores non-owning pointers)
		void AddModel(AssimpModel& model);
		void RemoveModel(AssimpModel* model);

		int GetWidth() const;
		int GetHeight() const;

	private:
		int _width;
		int _height;
		const char* _title;
		// GLFW windows must be destroyed with glfwDestroyWindow; use unique_ptr with custom deleter
		// use std::function deleter so unique_ptr is default-constructible
		std::unique_ptr<GLFWwindow, std::function<void(GLFWwindow*)>> _window;
		std::unique_ptr<stereorizer::graphics::Renderer> _leftRenderer;
		std::unique_ptr<stereorizer::graphics::Renderer> _rightRenderer;
		std::vector<std::shared_ptr<stereorizer::graphics::AssimpModel>> _models;
		void UpdateXRViews();
		void RenderModelsLeft();
		void RenderModelsRight();
		void Create();
		bool _xrInitialized = false;

		stereorizer::xr::OpenXRSupport _xrSupport;
		GraphicsAPI_Type m_apiType = GraphicsAPI_Type::OpenGL;
	};
}
