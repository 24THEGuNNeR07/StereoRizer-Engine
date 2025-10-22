#pragma once
#define _CRT_SECURE_NO_WARNINGS

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <unknwn.h>

#define XR_USE_GRAPHICS_API_OPENGL
#define XR_USE_PLATFORM_WIN32
#include "openxr/openxr.h"
#include "openxr/openxr_platform.h"

#include <vector>
#include <iostream>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "graphics/GfxAPIUtils.h"

struct XrSwapchainData {
    XrSwapchain handle = XR_NULL_HANDLE;
    std::vector<XrSwapchainImageOpenGLKHR> images;
    int32_t width = 0;
    int32_t height = 0;
};

class OpenXRSupport
{
public:
    OpenXRSupport();
    ~OpenXRSupport();

    // initialize OpenXR with a chosen graphics API type
    bool Init(GraphicsAPI_Type apiType);

    // poll runtime events (session state changes etc.)
    void PollEvents();

    // helpers used by Window for rendering
    glm::mat4 ConvertXrPoseToMat4(int eyeIndex);
    glm::mat4 ConvertXrFovToProj(int eyeIndex, float nearZ, float farZ);

    bool CopyFramebufferToSwapchain(GLuint srcFbo,
        GLint srcX, GLint srcY, GLsizei srcW, GLsizei srcH,
        XrSwapchainData& swapchain, uint32_t imageIndex,
        GLuint dstFboReuse);

    // accessors
    XrSession GetSession() const { return xrSession; }
    XrSpace GetAppSpace() const { return xrAppSpace; }
    XrSwapchainData* GetSwapchains() { return _swapchains; }

	void InitLoop(int width, int height);

	void WaitFrame();
	void BeginFrame();
    void LocateViews();

    void SetFrameSize(int width, int height);

    void CopyFrameBuffer();

	void EndLoop();

private:
    // OpenXR state
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

    // openxr rendering
    XrSwapchainData _swapchains[2];

    HGLRC xrSessionGLRC = nullptr;
    HDC xrSessionDC = nullptr;

    // internal helpers
    void CreateXRSwapchains();

    XrFrameState frameState{ XR_TYPE_FRAME_STATE };

    XrView views[2] = { {XR_TYPE_VIEW}, {XR_TYPE_VIEW} };
    XrViewState viewState{ XR_TYPE_VIEW_STATE };
    XrViewLocateInfo locateInfo{ XR_TYPE_VIEW_LOCATE_INFO };

    // persistent dstFbo (create once)
    GLuint xrDstFbo;
    GLuint srcFbo;
    int srcWidth;
    int srcHeight;
};

