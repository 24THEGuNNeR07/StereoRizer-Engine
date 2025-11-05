#shader vertex
#version 450 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texCoords;

out vec2 TexCoords;

void main()
{
    TexCoords = texCoords;
    gl_Position = vec4(position, 0.0, 1.0);
}

#shader fragment
#version 450 core

layout(location = 0) out vec4 color;

in vec2 TexCoords;

uniform sampler2D depthTexture;
uniform float nearPlane;
uniform float farPlane;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * nearPlane * farPlane) / (farPlane + nearPlane - z * (farPlane - nearPlane));
}

void main()
{
    float depthValue = texture(depthTexture, TexCoords).r;
    
    // Linearize depth and normalize
    float linearDepth = LinearizeDepth(depthValue);
    float normalizedDepth = linearDepth / farPlane;
    
    // Clamp and invert so closer objects are darker
    normalizedDepth = clamp(normalizedDepth, 0.0, 1.0);
    float grayscale = 1.0 - normalizedDepth;
    
    color = vec4(vec3(grayscale), 1.0);
}