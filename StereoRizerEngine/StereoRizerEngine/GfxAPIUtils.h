#pragma once

#include <string>
#include <stdexcept>
#include <openxr/openxr_platform.h>

enum class GraphicsAPI_Type
{
    UNKNOWN,
    Vulkan,
    OpenGL,
#ifdef _WIN32
    D3D11,
    D3D12
#endif
};

inline const char* GetGraphicsAPIInstanceExtensionString(GraphicsAPI_Type apiType)
{
    switch (apiType)
    {
#if defined(XR_USE_GRAPHICS_API_VULKAN)
    case GraphicsAPI_Type::Vulkan:
        return XR_KHR_VULKAN_ENABLE_EXTENSION_NAME;
#endif
    case GraphicsAPI_Type::OpenGL:
#if defined(_WIN32)
        return XR_KHR_OPENGL_ENABLE_EXTENSION_NAME;
#elif defined(__linux__)
        return XR_KHR_OPENGL_ENABLE_X11_EXTENSION_NAME; // Or XR_KHR_OPENGL_ENABLE_XCB_EXTENSION_NAME depending on window system
#else
        throw std::runtime_error("OpenGL extension not supported on this platform.");
#endif

    default:
        throw std::runtime_error("Unknown or unsupported graphics API type.");
    }
}
