#pragma once
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
private:
	float _FOV;
	float _AspectRatio;
	float _NearPlane;
	float _FarPlane;

	glm::vec3 _cameraPos;
	glm::vec3 _cameraFront;
	glm::vec3 _cameraUp;

	glm::mat4 _projectionMatrix;
	glm::mat4 _viewMatrix;

public:
	Camera(float fov, float aspectRatio, float nearPlane, float farPlane);
	~Camera() = default;
	void SetShaderUnifroms(Shader& shader);
	void SetViewMatrix(glm::mat4 view);
	void SetProjectionMatrix(glm::mat4 proj);
};

