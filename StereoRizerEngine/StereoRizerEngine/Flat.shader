#shader vertex
#version 330 core

layout(location = 0) in vec4 position;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * position;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

void main()
{
	color = vec4(1.0, 0.0, 1.0, 1.0);
};