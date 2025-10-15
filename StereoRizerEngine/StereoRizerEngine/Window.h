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

struct XrSwapchainData {
	XrSwapchain handle = XR_NULL_HANDLE;
	std::vector<XrSwapchainImageOpenGLKHR> images;
	int32_t width = 0;
	int32_t height = 0;
};

class Window
{
public:
	Window(int width, int height, const char* title);
	~Window();

	void Destroy();
	void PollEvents();
	void SwapBuffers();
	void Run(Model& model);

	int GetWidth() const;
	int GetHeight() const;

private:
	int _width;
	int _height;
	const char* _title;
	GLFWwindow* _window;
	Renderer _leftRenderer;
	Renderer _rightRenderer;
	void Create();
	bool _xrInitialized = false;
	void InitOpenXR();
	glm::mat4 ConvertXrPoseToMat4(const XrPosef& pose);
	glm::mat4 ConvertXrFovToProj(const XrFovf& fov, float nearZ, float farZ);
	void PollXrEvents();

	// OpenXR
	XrInstance xrInstance{ XR_NULL_HANDLE };
	XrSystemId xrSystemId{ XR_NULL_SYSTEM_ID };
	XrSession xrSession{ XR_NULL_HANDLE };
	XrSpace xrAppSpace{ XR_NULL_HANDLE };
	XrSessionState currentState = XR_SESSION_STATE_UNKNOWN;

	std::vector<const char*> m_activeAPILayers = {};
	std::vector<const char*> m_activeInstanceExtensions = {};
	std::vector<std::string> m_apiLayers = {};
	std::vector<std::string> m_instanceExtensions = {};

	XrDebugUtilsMessengerEXT m_debugUtilsMessenger = {};

	XrFormFactor m_formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
	XrSystemProperties m_systemProperties = { XR_TYPE_SYSTEM_PROPERTIES };

	GraphicsAPI_Type m_apiType = GraphicsAPI_Type::OpenGL;

	//openxr rendering
	XrSwapchainData _swapchains[2];
	void CreateXRSwapchains();
	bool CopyFramebufferToSwapchainByBlit_ReadRect(GLuint srcFbo,
		GLint srcX, GLint srcY, GLsizei srcW, GLsizei srcH,
		XrSwapchainData& swapchain, uint32_t imageIndex,
		GLuint dstFboReuse);

	HGLRC xrSessionGLRC;
	HDC xrSessionDC;
};

