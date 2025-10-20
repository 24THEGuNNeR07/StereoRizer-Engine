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
    case GraphicsAPI_Type::OpenGL:
#if defined(_WIN32)
        return XR_KHR_OPENGL_ENABLE_EXTENSION_NAME;
#endif
    default:
        throw std::runtime_error("Unknown or unsupported graphics API type.");
    }
}
