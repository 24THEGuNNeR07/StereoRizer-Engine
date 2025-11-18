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

#include "graphics/Renderer.h"
#include "graphics/Light.h"
#include "graphics/GfxAPIUtils.h"
#include "graphics/Shader.h"
#include "xr/OpenXRSupport.h"
#include <vector>
#include <algorithm>
#include <memory>
#include <functional>
#include <vector>
#include <algorithm>

// ImGui includes
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace stereorizer::core
{
	enum class ViewDisplayMode {
		Color,
		Depth,
		ReprojectionMask
	};

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
		void AddModel(std::shared_ptr<stereorizer::graphics::Model> model);
		void RemoveModel(std::shared_ptr<stereorizer::graphics::Model> model);
		
		// Light management
		void SetLight(std::shared_ptr<stereorizer::graphics::Light> light);
		std::shared_ptr<stereorizer::graphics::Light> GetLight() const;

		int GetWidth() const;
		int GetHeight() const;

		// IPD accessors
		float GetIPD() const;
		void SetIPD(float ipd);

		// Depth texture control
		void SetLeftViewDisplayMode(ViewDisplayMode mode);
		ViewDisplayMode GetLeftViewDisplayMode() const;
		GLuint GetLeftViewDepthTexture() const;
		GLuint GetLeftViewColorTexture() const;

		void SetRightViewDisplayMode(ViewDisplayMode mode);
		ViewDisplayMode GetRightViewDisplayMode() const;
		GLuint GetRightViewDepthTexture() const;
		GLuint GetRightViewColorTexture() const;

		// FPS control
		void SetTargetFPS(float targetFPS);
		float GetTargetFPS() const;
		float GetCurrentFPS() const;

	private:
		int _width;
		int _height;
		const char* _title;
		// GLFW windows must be destroyed with glfwDestroyWindow; use unique_ptr with custom deleter
		// use std::function deleter so unique_ptr is default-constructible
		std::unique_ptr<GLFWwindow, std::function<void(GLFWwindow*)>> _window;
		std::unique_ptr<stereorizer::graphics::Renderer> _leftRenderer;
		std::unique_ptr<stereorizer::graphics::Renderer> _rightRenderer;
		std::vector<std::shared_ptr<stereorizer::graphics::Model>> _models;
		std::shared_ptr<stereorizer::graphics::Light> _sceneLight;
		bool UpdateXRViews();
		void RenderModelsLeft();
		void RenderModelsRight();
		void InitResources();
		void RenderImGui();
		void handleMouseInput();

		bool _xrInitialized = false;
		// Inter-pupillary distance in meters (default 64mm)
		float _ipd = 0.064f;

		float deltaTime = 0.0f;
		float currentFrame = 0.0f;
		float lastFrame = 0.0f;
		
		// FPS control
		float _targetFPS = 60.0f;
		float _currentFPS = 0.0f;
		float _frameTimeAccumulator = 0.0f;
		int _frameCount = 0;
		float _lastFPSUpdate = 0.0f;
		
		void processInput(GLFWwindow* window);
		void OnMouseMove(double xpos, double ypos);
		static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
		bool firstMouse = true;
		float lastX = 0, lastY = 0;

		stereorizer::xr::OpenXRSupport _xrSupport;
		GraphicsAPI_Type m_apiType = GraphicsAPI_Type::OpenGL;
		
		// Depth texture state
		ViewDisplayMode _leftViewDisplayMode = ViewDisplayMode::Color;
		ViewDisplayMode _rightViewDisplayMode = ViewDisplayMode::Color;

		std::shared_ptr<stereorizer::graphics::Shader> _reprojectionShader = nullptr;
		std::shared_ptr<stereorizer::graphics::Shader> _standardShader = nullptr;
	};
}
