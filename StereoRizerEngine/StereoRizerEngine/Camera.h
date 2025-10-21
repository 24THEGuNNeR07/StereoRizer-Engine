#pragma once
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera {
public:
	Camera(float fov, float aspectRatio, float nearPlane, float farPlane);
	~Camera() = default;

	// Position/orientation setters
	void SetPosition(const glm::vec3& pos);
	void SetFront(const glm::vec3& front);
	void SetUp(const glm::vec3& up);

	void SetViewMatrix(const glm::mat4& view);
	void SetProjectionMatrix(const glm::mat4& proj);

	// Matrix getters
	const glm::mat4& GetViewMatrix() const noexcept;
	const glm::mat4& GetProjectionMatrix() const noexcept;

	// Shader uniform upload
	void UploadToShader(const Shader& shader) const;

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

	// Matrix update
	void UpdateViewMatrix();
	void UpdateProjectionMatrix();
};