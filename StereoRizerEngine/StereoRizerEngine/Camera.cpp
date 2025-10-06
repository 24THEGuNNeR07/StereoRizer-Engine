#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(float fov, float aspectRatio, float nearPlane, float farPlane) :
	 _FOV(fov), _AspectRatio(aspectRatio), _NearPlane(nearPlane), _FarPlane(farPlane)
{
	_projectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
	_cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
	_cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	_cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	_viewMatrix = glm::lookAt(_cameraPos, _cameraPos + _cameraFront, _cameraUp);
}
