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

uniform sampler2D colorTexture;

void main()
{
    // Sample the color texture directly
    color = texture(colorTexture, TexCoords);
}