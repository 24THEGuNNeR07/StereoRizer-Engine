#pragma once

#include <glm/glm.hpp>
#include <string>

namespace stereorizer::graphics
{
    enum class LightType {
        Directional,
        Point,
        Spot
    };

    class Light {
    public:
        Light(LightType type = LightType::Directional);
        
        // Common light properties
        void SetPosition(const glm::vec3& position);
        void SetDirection(const glm::vec3& direction);
        void SetColor(const glm::vec3& color);
        void SetIntensity(float intensity);
        
        // Point/Spot light attenuation
        void SetAttenuation(float constant, float linear, float quadratic);
        
        // Spot light specific
        void SetSpotAngles(float innerConeAngle, float outerConeAngle);
        
        // Getters
        LightType GetType() const { return _type; }
        const glm::vec3& GetPosition() const { return _position; }
        const glm::vec3& GetDirection() const { return _direction; }
        const glm::vec3& GetColor() const { return _color; }
        float GetIntensity() const { return _intensity; }
        
        const glm::vec3& GetAttenuation() const { return _attenuation; } // constant, linear, quadratic
        float GetInnerConeAngle() const { return _innerConeAngle; }
        float GetOuterConeAngle() const { return _outerConeAngle; }
        
        // Upload to shader uniforms
        void UploadToShader(unsigned int shaderID, const std::string& uniformPrefix = "light") const;

    private:
        LightType _type;
        glm::vec3 _position;
        glm::vec3 _direction;
        glm::vec3 _color;
        float _intensity;
        
        // Attenuation (constant, linear, quadratic)
        glm::vec3 _attenuation;
        
        // Spot light angles (in radians)
        float _innerConeAngle;
        float _outerConeAngle;
    };
}