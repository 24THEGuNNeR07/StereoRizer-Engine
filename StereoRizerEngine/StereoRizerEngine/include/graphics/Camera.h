#pragma once
#include <glm/glm.hpp>

namespace stereorizer::graphics
{
	class FreeCamera
	{

	private:
		glm::vec3 cameraPos, cameraUp;
		glm::vec3 dir;
		float pitch, yaw;
		float fov;
		float aspectRatio, nearPlane, farPlane;
		bool changedView, changedProjection, vpUpdated;
		glm::mat4 view, projection, vp;
	public:
		float maxPitch = 90.0f, minPitch = -90.0f, maxFov = 45.0f, minFov = 1.0f;

		FreeCamera(const glm::vec3& cameraPos, const float aspectRatio, float yaw = -90.0f, float pitch = 0.0f, float fov = 45.0f, float nearPlane = 0.1f, float farPlane = 100.0f);

		const glm::vec3& getCameraPos();
		const glm::vec3& getCameraDir();
		glm::vec3 getCameraRight();
		const glm::mat4& getViewMatrix();
		const glm::mat4& getProjectionMatrix();
		const glm::mat4& getVPMatrix();
		const float getPitch();
		const float getYaw();
		const float getFOV();
		const float getAspectRatio();
		const float getNearPlane();
		const float getFarPlane();


		void setCameraPos(const glm::vec3& camPos);
		void setPitch(float pitch);
		void setYaw(float yaw);
		void setFOV(float fov);
		void setAspectRatio(float aspectRatio);
		void setNearPlane(float nearPlane);
		void setFarPlane(float farPlane);

		void validateMaxMins();
		void updateCameraDir(bool forced = false);
		const glm::mat4& updateMatrices(bool forced = false);
		const bool isVPUpdated();
		void clearFlags();
	};
}