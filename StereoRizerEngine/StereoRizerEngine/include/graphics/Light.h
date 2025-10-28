#pragma once
#include <graphics/Transform.h>
#include <graphics/Shader.h>
#include <string>
#include <vector>
#include <glm/glm.hpp>

namespace stereorizer::graphics
{
	class Light : public Transform
	{
	private:
		const uint32_t id;
		glm::vec3 ambient = glm::vec3(0.0f), diffuse = glm::vec3(0.0f), specular = glm::vec3(0.0f);
		const std::string arrayPrefix;
		bool active = false;
	public:
		const std::vector<Shader*>* const litShaders;
		Light(uint32_t id, const std::string& arrayPrefix, const std::vector<Shader*>* const litShaders);
		void setup(glm::vec3 ambient = glm::vec3(0.05f), glm::vec3 diffuse = glm::vec3(1.0f), glm::vec3 specular = glm::vec3(1.0f));
		uint32_t getId() const;
		const std::string getNameFor(const std::string& name) const;
		template<typename Type>
		void update(const std::string& name, const Type& val) const
		{
			for (auto& shader : *litShaders)
			{
				shader->setUniform<Type>(getNameFor(name), val);
			}
		}
		virtual void updateTransforms();
		const glm::vec3& getAmbient() const;
		const glm::vec3& getDiffuse() const;
		const glm::vec3& getSpecular() const;
		float getYaw() const;
		float getPitch() const;
		const glm::vec3 getDirection() const;
		bool isActive() const;

		void activate();
		void setAmbient(const glm::vec3& ambient);
		void setDiffuse(const glm::vec3& diffuse);
		void setSpecular(const glm::vec3& specular);
		void setYaw(const float yaw);
		void setPitch(const float pitch);

		virtual void free();

		/*
		Use getDirection() to get direction of the light,
		Use Transform::getRotation() if it is a must
		*/
		glm::vec3 getRotation() = delete;
		/*
		Use setYaw and setPitch to change direction of the light,
		Use Transform::setRotation() if it is a must
		*/
		void setRotation(const glm::vec3& rot) = delete;
	};

	class PointLight : public Light
	{
	private:
		float constant = 1.0f, linear = 0.0f, quadratic = 0.0f;
	public:
		PointLight(uint32_t id, const std::string& arrayPrefix, const std::vector<Shader*>* const litShaders);
		void setup(glm::vec3 ambient = glm::vec3(0.05f), glm::vec3 diffuse = glm::vec3(1.0f), glm::vec3 specular = glm::vec3(1.0f), float constant = 1.0f, float linear = 0.09f, float quadratic = 0.032f);
		const float getConstant() const;
		const float getLinear() const;
		const float getQuadratic() const;

		void setConstant(float constant);
		void setLinear(float linear);
		void setQuadratic(float quadratic);
		virtual void free() override;
	};

	class DirectionalLight : public Light
	{
	public:
		DirectionalLight(uint32_t id, const std::string& arrayPrefix, const std::vector<Shader*>* const litShaders);
	};

	class SpotLight : public PointLight
	{
	private:
		float cutoff = 0.0f, outerCutoff = 0.0f;
	public:
		SpotLight(uint32_t id, const std::string& arrayPrefix, const std::vector<Shader*>* const litShaders);
		void setup(glm::vec3 ambient = glm::vec3(0.05f), glm::vec3 diffuse = glm::vec3(1.0f), glm::vec3 specular = glm::vec3(1.0f), float constant = 1.0f, float linear = 0.09f, float quadratic = 0.032f, float cutoff = 20.0f, float outerCutoff = 25.0f);
		const float getCutoff() const;
		const float getOuterCutoff() const;

		void setCutoff(const float cutoff);
		void setOuterCutoff(const float outerCutoff);
		virtual void free() override;
	};
}