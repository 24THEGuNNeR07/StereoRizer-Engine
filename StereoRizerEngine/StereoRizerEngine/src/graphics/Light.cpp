#include "graphics/Light.h"
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

using namespace stereorizer::graphics;

Light::Light(LightType type)
    : _type(type)
    , _position(0.0f, 0.0f, 0.0f)
    , _direction(0.0f, -1.0f, 0.0f) // Default downward direction
    , _color(1.0f, 1.0f, 1.0f)     // White light
    , _intensity(1.0f)
    , _attenuation(1.0f, 0.0f, 0.0f) // No attenuation by default
    , _innerConeAngle(0.5f)         // ~28.6 degrees
    , _outerConeAngle(0.785f)       // 45 degrees
{
}

void Light::SetPosition(const glm::vec3& position) {
    _position = position;
}

void Light::SetDirection(const glm::vec3& direction) {
    _direction = glm::normalize(direction);
}

void Light::SetColor(const glm::vec3& color) {
    _color = color;
}

void Light::SetIntensity(float intensity) {
    _intensity = intensity;
}

void Light::SetAttenuation(float constant, float linear, float quadratic) {
    _attenuation = glm::vec3(constant, linear, quadratic);
}

void Light::SetSpotAngles(float innerConeAngle, float outerConeAngle) {
    _innerConeAngle = innerConeAngle;
    _outerConeAngle = outerConeAngle;
}

void Light::UploadToShader(unsigned int shaderID, const std::string& uniformPrefix) const {
    // Type (0 = Directional, 1 = Point, 2 = Spot)
    GLint typeLocation = glGetUniformLocation(shaderID, (uniformPrefix + ".type").c_str());
    if (typeLocation != -1) {
        glUniform1i(typeLocation, static_cast<int>(_type));
    }
    
    // Position
    GLint positionLocation = glGetUniformLocation(shaderID, (uniformPrefix + ".position").c_str());
    if (positionLocation != -1) {
        glUniform3fv(positionLocation, 1, glm::value_ptr(_position));
    }
    
    // Direction
    GLint directionLocation = glGetUniformLocation(shaderID, (uniformPrefix + ".direction").c_str());
    if (directionLocation != -1) {
        glUniform3fv(directionLocation, 1, glm::value_ptr(_direction));
    }
    
    // Color
    GLint colorLocation = glGetUniformLocation(shaderID, (uniformPrefix + ".color").c_str());
    if (colorLocation != -1) {
        glUniform3fv(colorLocation, 1, glm::value_ptr(_color));
    }
    
    // Intensity
    GLint intensityLocation = glGetUniformLocation(shaderID, (uniformPrefix + ".intensity").c_str());
    if (intensityLocation != -1) {
        glUniform1f(intensityLocation, _intensity);
    }
    
    // Attenuation (for point and spot lights)
    GLint attenuationLocation = glGetUniformLocation(shaderID, (uniformPrefix + ".attenuation").c_str());
    if (attenuationLocation != -1) {
        glUniform3fv(attenuationLocation, 1, glm::value_ptr(_attenuation));
    }
    
    // Spot light angles
    GLint innerAngleLocation = glGetUniformLocation(shaderID, (uniformPrefix + ".innerConeAngle").c_str());
    if (innerAngleLocation != -1) {
        glUniform1f(innerAngleLocation, _innerConeAngle);
    }
    
    GLint outerAngleLocation = glGetUniformLocation(shaderID, (uniformPrefix + ".outerConeAngle").c_str());
    if (outerAngleLocation != -1) {
        glUniform1f(outerAngleLocation, _outerConeAngle);
    }
}