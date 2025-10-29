#include "graphics/Camera.h"
#include "graphics/Shader.h"
#include <glm/gtc/matrix_transform.hpp>

using namespace stereorizer::graphics;

Camera::Camera(float fov, float aspectRatio, float nearPlane, float farPlane)
	: _FOV(fov), _AspectRatio(aspectRatio), _NearPlane(nearPlane), _FarPlane(farPlane), _yaw(-90.0f), _pitch(0.0f)
{
	_cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
	_cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	_cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	UpdateCameraVectors();
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

glm::vec3 stereorizer::graphics::Camera::GetPosition() const noexcept
{
	return _cameraPos;
}

glm::vec3 stereorizer::graphics::Camera::GetFront() const noexcept
{
	return _cameraFront;
}

void Camera::UpdateViewMatrix() {
	_viewMatrix = glm::lookAt(_cameraPos, _cameraPos + _cameraFront, _cameraUp);
}

void Camera::UpdateProjectionMatrix() {
	_projectionMatrix = glm::perspective(glm::radians(_FOV), _AspectRatio, _NearPlane, _FarPlane);
}

void stereorizer::graphics::Camera::UpdateCameraVectors()
{
	glm::vec3 front;
	front.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
	front.y = sin(glm::radians(_pitch));
	front.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
	_cameraFront = glm::normalize(front);

	// Recalculate right and up vectors to maintain orthogonality
	glm::vec3 right = glm::normalize(glm::cross(_cameraFront, glm::vec3(0.0f, 1.0f, 0.0f)));
	_cameraUp = glm::normalize(glm::cross(right, _cameraFront));
}

void stereorizer::graphics::Camera::SetYaw(float yaw)
{
	_yaw = yaw;
	UpdateCameraVectors();
	UpdateViewMatrix();
}

void stereorizer::graphics::Camera::SetPitch(float pitch)
{
	// Clamp pitch to avoid gimbal lock
	if (pitch > 89.0f) pitch = 89.0f;
	if (pitch < -89.0f) pitch = -89.0f;

	_pitch = pitch;
	UpdateCameraVectors();
	UpdateViewMatrix();
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