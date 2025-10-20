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
#include <string>
#include <iostream>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "GfxAPIUtils.h"

// Small POD for a swapchain and its OpenGL images
struct XrSwapchainData {
    XrSwapchain handle = XR_NULL_HANDLE;
    std::vector<XrSwapchainImageOpenGLKHR> images;
    int32_t width = 0;
    int32_t height = 0;
};

// Thin helper to manage OpenXR integration for rendering loop.
// Responsibilities:
// - create instance, session and swapchains
// - provide per-frame helpers: Wait/Begin/Locate/Copy/End
// - keep OpenGL objects minimal and reuse FBOs where possible
class OpenXRSupport
{
public:
    OpenXRSupport();
    ~OpenXRSupport();

    // Initialize OpenXR. apiType currently only influences instance extensions.
    bool Init(GraphicsAPI_Type apiType);

    // Poll runtime events (session state changes etc.)
    void PollEvents();

    // Helpers used by the Window/Renderer
    glm::mat4 ConvertXrPoseToMat4(int eyeIndex) const;
    glm::mat4 ConvertXrFovToProj(int eyeIndex, float nearZ, float farZ) const;

    // Copy a rectangle from a source FBO into the swapchain image via blit.
    bool CopyFramebufferToSwapchainByBlit_ReadRect(GLuint srcFbo,
        GLint srcX, GLint srcY, GLsizei srcW, GLsizei srcH,
        XrSwapchainData& swapchain, uint32_t imageIndex,
        GLuint dstFboReuse);

    // Accessors
    XrSession GetSession() const { return xrSession; }
    XrSpace GetAppSpace() const { return xrAppSpace; }
    XrSwapchainData* GetSwapchains() { return _swapchains; }

    // Frame loop helpers
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

    std::vector<const char*> m_activeAPILayers;
    std::vector<const char*> m_activeInstanceExtensions;
    std::vector<std::string> m_apiLayers;
    std::vector<std::string> m_instanceExtensions;

    XrDebugUtilsMessengerEXT m_debugUtilsMessenger = {};

    XrFormFactor m_formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
    XrSystemProperties m_systemProperties = { XR_TYPE_SYSTEM_PROPERTIES };

    GraphicsAPI_Type m_apiType = GraphicsAPI_Type::OpenGL;

    // rendering resources
    XrSwapchainData _swapchains[2];

    HGLRC xrSessionGLRC = nullptr;
    HDC xrSessionDC = nullptr;

    // internal helpers
    void CreateXRSwapchains();

    XrFrameState frameState{ XR_TYPE_FRAME_STATE };

    XrView views[2] = { {XR_TYPE_VIEW}, {XR_TYPE_VIEW} };
    XrViewState viewState{ XR_TYPE_VIEW_STATE };
    XrViewLocateInfo locateInfo{ XR_TYPE_VIEW_LOCATE_INFO };

    // persistent dst FBO (created once in InitLoop)
    GLuint xrDstFbo = 0;
    GLuint srcFbo = 0;
    int srcWidth = 0;
    int srcHeight = 0;
};

