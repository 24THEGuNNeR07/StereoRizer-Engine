#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out Vertex
{
	vec3 Position;
	vec3 Normal;
} vertex;

void main()
{
	vertex.Normal = mat3(modelMatrix) * normal;
	vertex.Position = vec3(modelMatrix * vec4(position, 1.0));
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;

in Vertex
{
	vec3 Position;
	vec3 Normal;
} vertex;

void main()
{
	vec3 N = normalize(vertex.Normal);
	vec3 cameraPos = vec3(viewMatrix[3]);
	vec3 modelPos = vec3(modelMatrix[3]);
	vec3 viewDir = normalize(cameraPos - modelPos);
	float diff = max(dot(N, viewDir), 0.0);
	vec4 output_color = vec4(0.5, 0.5, 0.5, 1.0);
	output_color *= diff;
	color = output_color;
};