#pragma once
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace stereorizer::graphics
{
	class Camera {
	public:
		Camera(float fov, float aspectRatio, float nearPlane, float farPlane);
		~Camera() = default;

		// Position/orientation setters
		void SetPosition(const glm::vec3& pos);
		void SetFront(const glm::vec3& front);
		void SetUp(const glm::vec3& up);

		glm::vec3 GetPosition() const noexcept;
		glm::vec3 GetFront() const noexcept;
		glm::vec3 GetCameraRight() const noexcept {
			return glm::normalize(glm::cross(_cameraFront, _cameraUp));
		}

		// Orientation control
		void SetYaw(float yaw);
		void SetPitch(float pitch);

		float GetYaw() const noexcept { return _yaw; }
		float GetPitch() const noexcept { return _pitch; }
		float GetNearPlane() const noexcept { return _NearPlane; }
		float GetFarPlane() const noexcept { return _FarPlane; }

		void SetViewMatrix(const glm::mat4& view);
		void SetProjectionMatrix(const glm::mat4& proj);

		// Matrix getters
		const glm::mat4& GetViewMatrix() const noexcept;
		const glm::mat4& GetProjectionMatrix() const noexcept;

		// Shader uniform upload
		void UploadToShader(std::shared_ptr<Shader> shader) const;
		void UploadToReprojectionShader(std::shared_ptr<Shader> shader) const;

	private:
		float _FOV;
		float _AspectRatio;
		float _NearPlane;
		float _FarPlane;

		// Euler angles
		float _yaw;
		float _pitch;

		glm::vec3 _cameraPos;
		glm::vec3 _cameraFront;
		glm::vec3 _cameraUp;

		glm::mat4 _projectionMatrix;
		glm::mat4 _viewMatrix;

		// Matrix update
		void UpdateViewMatrix();
		void UpdateProjectionMatrix();
		void UpdateCameraVectors();
	};
}