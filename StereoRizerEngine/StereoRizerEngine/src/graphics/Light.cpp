#include <graphics/Light.h>
#pragma once

using namespace stereorizer::graphics;

Light::Light(uint32_t id, const std::string &arrayPrefix, const std::vector<Shader *> *const litShaders) : id(id), arrayPrefix(arrayPrefix), litShaders(litShaders)
{

}

void Light::setup(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
{
	active = true;
	setAmbient(ambient);
	setDiffuse(diffuse);
	setSpecular(specular);
	setPosition(glm::vec3(0.0f));
	setScale(glm::vec3(1.0f));
	Transform::setRotation(glm::vec3(-90.0f, 0.0f, 0.0f));
	updateTransforms();
}

uint32_t Light::getId() const
{
	return id;
}

const std::string Light::getNameFor(const std::string &name) const
{
	return arrayPrefix + "[" + std::to_string(id) + "]." + name;
}
void Light::updateTransforms()
{
	if(!isModelChanged())
		return;
	update("lightPos", getPosition());


	update("lightDir", getDirection());
	update("lightScale", getScale());

	setChangedModel(false);
}
const glm::vec3 &Light::getAmbient() const
{
	return ambient;
}
const glm::vec3 &Light::getDiffuse() const
{
	return diffuse;
}
const glm::vec3 &Light::getSpecular() const
{
	return specular;
}
float Light::getYaw() const
{
	return Transform::getRotation().x;
}
float Light::getPitch() const
{
	return Transform::getRotation().y;
}
const glm::vec3 Light::getDirection() const
{
	float cosYaw = glm::cos(glm::radians(getYaw()));
	float sinYaw = glm::sin(glm::radians(getYaw()));
	float cosPitch = glm::cos(glm::radians(getPitch()));
	float sinPitch = glm::sin(glm::radians(getPitch()));
	return glm::vec3(cosPitch * cosYaw, sinPitch, cosPitch * sinYaw);
}
bool Light::isActive() const
{
	return active;
}

void Light::activate()
{
	active = true;
}
void Light::setAmbient(const glm::vec3 &ambient)
{
	this->ambient = ambient;
	update<glm::vec3>("ambient", ambient);
}
void Light::setDiffuse(const glm::vec3 &diffuse)
{
	this->diffuse = diffuse;
	update<glm::vec3>("diffuse", diffuse);
}
void Light::setSpecular(const glm::vec3 &specular)
{
	this->specular = specular;
	update<glm::vec3>("specular", specular);
}

void Light::setYaw(const float yaw)
{
	Transform::setRotation(glm::vec3(yaw, getPitch(), 0.0f));
	update("lightDir", getDirection());
}
void Light::setPitch(const float pitch)
{
	Transform::setRotation(glm::vec3(getYaw(), pitch, 0.0f));
	update("lightDir", getDirection());
}

void Light::free()
{
	active = false;
	glm::vec3 zeroVec = glm::vec3(0.0f);
	setAmbient(zeroVec);
	setDiffuse(zeroVec);
	setSpecular(zeroVec);
	setPosition(zeroVec);
	Transform::setRotation(glm::vec3(-90.0f, 0.0f, 0.0f));
}


PointLight::PointLight(uint32_t id, const std::string &arrayPrefix, const std::vector<Shader *> *const litShaders) : Light(id, arrayPrefix, litShaders)
{
}
void PointLight::setup(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float constant, float linear, float quadratic)
{
	setConstant(constant);
	setLinear(linear);
	setQuadratic(quadratic);
	Light::setup(ambient, diffuse, specular);
}
const float PointLight::getConstant() const
{
	return constant;
}
const float PointLight::getLinear() const
{
	return linear;
}
const float PointLight::getQuadratic() const
{
	return quadratic;
}

void PointLight::setConstant(float constant)
{
	this->constant = constant;
	update("constant", constant);
}
void PointLight::setLinear(float linear)
{
	this->linear = linear;
	update("linear", linear);
}
void PointLight::setQuadratic(float quadratic)
{
	this->quadratic = quadratic;
	update("quadratic", quadratic);
}
void PointLight::free()
{
	setConstant(1.0f);
	setLinear(0.0f);
	setQuadratic(0.0f);
	Light::free();
}


DirectionalLight::DirectionalLight(uint32_t id, const std::string &arrayPrefix, const std::vector<Shader *> *const litShaders) : Light(id, arrayPrefix, litShaders)
{

}

SpotLight::SpotLight(uint32_t id, const std::string &arrayPrefix, const std::vector<Shader *> *const litShaders) : PointLight(id, arrayPrefix, litShaders)
{
}
void SpotLight::setup(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float constant, float linear, float quadratic, float cutoff, float outerCutoff)
{
	setCutoff(cutoff);
	setOuterCutoff(outerCutoff);
	PointLight::setup(ambient, diffuse, specular);
}
const float SpotLight::getCutoff() const
{
	return cutoff;
}
const float SpotLight::getOuterCutoff() const
{
	return outerCutoff;
}

void SpotLight::setCutoff(const float cutoff)
{
	this->cutoff = cutoff;
	update("cutoff", glm::cos(glm::radians(cutoff)));
}
void SpotLight::setOuterCutoff(const float outerCutoff)
{
	this->outerCutoff = outerCutoff;
	update("outerCutoff", glm::cos(glm::radians(outerCutoff)));
}
void SpotLight::free()
{
	setCutoff(0.0f);
	setOuterCutoff(0.0f);
	PointLight::free();
}
