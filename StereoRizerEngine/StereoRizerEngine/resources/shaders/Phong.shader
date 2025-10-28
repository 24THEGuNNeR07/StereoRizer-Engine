#shader vertex
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 Normal;
out vec3 FragPos;

struct Model {
    mat4 MVP;
    mat4 M;
    mat4 V;
    mat4 P;
    mat4 VP;
    mat4 MV;
    mat4 TIM;
};

uniform Model model;

void main()
{
	gl_Position = model.MVP * vec4(aPos, 1.0);
    FragPos = vec3(model.M * vec4(aPos, 1.0));
    Normal = mat3(model.TIM ) * aNormal;
};

#shader fragment
#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

struct Model {
    mat4 MVP;
    mat4 M;
    mat4 V;
    mat4 P;
    mat4 VP;
    mat4 MV;
    mat4 TIM;
};

struct Camera {
    vec3 viewPos;
};

uniform Model model;
uniform Camera camera;

void main()
{
	//vec3 N = normalize(vertex.Normal);
	//vec3 cameraPos = vec3(viewMatrix[3]);
	//vec3 modelPos = vec3(modelMatrix[3]);
	//vec3 viewDir = normalize(cameraPos - modelPos);
	//float diff = max(dot(N, viewDir), 0.0);
	vec4 output_color = vec4(Normal, 1.0);
	//output_color *= diff;
	FragColor = output_color;
};