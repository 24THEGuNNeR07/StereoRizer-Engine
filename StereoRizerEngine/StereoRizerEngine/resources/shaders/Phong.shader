#shader vertex
#version 450 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec3 Normal;
out vec3 FragPos;

void main()
{
	Normal = mat3(transpose(inverse(modelMatrix))) * normal;
	FragPos = vec3(modelMatrix * vec4(position, 1.0));
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);
};

#shader fragment
#version 450 core

layout(location = 0) out vec4 color;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;

in vec3 Normal;
in vec3 FragPos;

void main()
{
	vec3 N = normalize(Normal);
	//vec3 cameraPos = vec3(viewMatrix[3]);
	//vec3 viewDir = normalize(cameraPos - FragPos);
	//float diff = max(dot(N, viewDir), 0.0);
	vec4 output_color = vec4(N, 1.0);
	//output_color *= diff;
	color = output_color;
};