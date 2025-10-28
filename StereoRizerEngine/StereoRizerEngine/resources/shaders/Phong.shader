#shader vertex
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 Normal;
out vec3 FragPos;

uniform mat4 MVP;
uniform mat4 M;
uniform mat4 TIM;

void main()
{
    vec4 pos = vec4(aPos, 1.0);
    pos.x *=2;
	gl_Position = MVP * vec4(aPos, 1.0);
    FragPos = vec3(M * vec4(aPos, 1.0));
    Normal = mat3(TIM ) * aNormal;
};

#shader fragment
#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

void main()
{
	//vec3 N = normalize(vertex.Normal);
	//vec3 cameraPos = vec3(viewMatrix[3]);
	//vec3 modelPos = vec3(modelMatrix[3]);
	//vec3 viewDir = normalize(cameraPos - modelPos);
	//float diff = max(dot(N, viewDir), 0.0);
	vec4 output_color = vec4(1.0, 0.0, 0.0, 1.0);
	//output_color *= diff;
	FragColor = output_color;
};