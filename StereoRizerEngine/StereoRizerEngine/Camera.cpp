#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>


Camera::Camera(float fov, float aspectRatio, float nearPlane, float farPlane)
	: _FOV(fov), _AspectRatio(aspectRatio), _NearPlane(nearPlane), _FarPlane(farPlane)
{
	_cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
	_cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	_cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	UpdateProjectionMatrix();
	UpdateViewMatrix();
}

void Camera::SetPosition(const glm::vec3& pos) {
	_cameraPos = pos;
	UpdateViewMatrix();
}

void Camera::SetFront(const glm::vec3& front) {
	_cameraFront = front;
	UpdateViewMatrix();
}

void Camera::SetUp(const glm::vec3& up) {
	_cameraUp = up;
	UpdateViewMatrix();
}

void Camera::UpdateViewMatrix() {
	_viewMatrix = glm::lookAt(_cameraPos, _cameraPos + _cameraFront, _cameraUp);
}

void Camera::UpdateProjectionMatrix() {
	_projectionMatrix = glm::perspective(glm::radians(_FOV), _AspectRatio, _NearPlane, _FarPlane);
}

void Camera::SetViewMatrix(const glm::mat4& view) {
	_viewMatrix = view;
}

void Camera::SetProjectionMatrix(const glm::mat4& proj) {
	_projectionMatrix = proj;
}

const glm::mat4& Camera::GetViewMatrix() const noexcept {
	return _viewMatrix;
}

const glm::mat4& Camera::GetProjectionMatrix() const noexcept {
	return _projectionMatrix;
}

void Camera::UploadToShader(std::shared_ptr<Shader> shader) const {
	unsigned int projLoc = glGetUniformLocation(shader->GetID(), "projectionMatrix");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(_projectionMatrix));
	unsigned int viewLoc = glGetUniformLocation(shader->GetID(), "viewMatrix");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(_viewMatrix));
}