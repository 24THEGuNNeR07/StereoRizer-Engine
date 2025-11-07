#shader vertex
#version 450 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec3 Normal;
out vec3 FragPos;
out vec3 WorldNormal;

void main()
{
    FragPos = vec3(modelMatrix * vec4(position, 1.0));
    WorldNormal = mat3(transpose(inverse(modelMatrix))) * normal;
    Normal = WorldNormal;
    
    gl_Position = projectionMatrix * viewMatrix * vec4(FragPos, 1.0);
}

#shader fragment
#version 450 core

layout(location = 0) out vec4 color;

// Light structure
struct Light {
    int type;              // 0 = Directional, 1 = Point, 2 = Spot
    vec3 position;         // World space position (for point/spot lights)
    vec3 direction;        // World space direction (for directional/spot lights)
    vec3 color;            // Light color
    float intensity;       // Light intensity
    vec3 attenuation;      // x = constant, y = linear, z = quadratic
    float innerConeAngle;  // Inner cone angle for spot lights (radians)
    float outerConeAngle;  // Outer cone angle for spot lights (radians)
};

uniform Light light;
uniform vec3 viewPos;      // Camera position in world space
uniform vec3 materialColor; // Material diffuse color

in vec3 Normal;
in vec3 FragPos;
in vec3 WorldNormal;

vec3 calculateDirectionalLight(Light light, vec3 normal, vec3 fragPos) {
    // Calculate light direction (pointing towards the light)
    vec3 lightDir = normalize(-light.direction);
    
    // Diffuse calculation
    float diff = max(dot(normal, lightDir), 0.0);
    
    return light.color * light.intensity * diff;
}

vec3 calculatePointLight(Light light, vec3 normal, vec3 fragPos) {
    // Calculate light direction and distance
    vec3 lightDir = normalize(light.position - fragPos);
    float distance = length(light.position - fragPos);
    
    // Diffuse calculation
    float diff = max(dot(normal, lightDir), 0.0);
    
    // Attenuation calculation
    float attenuation = 1.0 / (light.attenuation.x + light.attenuation.y * distance + light.attenuation.z * distance * distance);
    
    return light.color * light.intensity * diff * attenuation;
}

vec3 calculateSpotLight(Light light, vec3 normal, vec3 fragPos) {
    // Calculate light direction and distance
    vec3 lightDir = normalize(light.position - fragPos);
    float distance = length(light.position - fragPos);
    
    // Diffuse calculation
    float diff = max(dot(normal, lightDir), 0.0);
    
    // Spotlight cone calculation
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = cos(light.innerConeAngle) - cos(light.outerConeAngle);
    float intensity = clamp((theta - cos(light.outerConeAngle)) / epsilon, 0.0, 1.0);
    
    // Attenuation calculation
    float attenuation = 1.0 / (light.attenuation.x + light.attenuation.y * distance + light.attenuation.z * distance * distance);
    
    return light.color * light.intensity * diff * intensity * attenuation;
}

void main()
{
    vec3 norm = normalize(Normal);
    vec3 lightContribution;
    
    // Calculate lighting based on light type
    if (light.type == 0) {
        // Directional light
        lightContribution = calculateDirectionalLight(light, norm, FragPos);
    } else if (light.type == 1) {
        // Point light
        lightContribution = calculatePointLight(light, norm, FragPos);
    } else if (light.type == 2) {
        // Spot light
        lightContribution = calculateSpotLight(light, norm, FragPos);
    } else {
        // Fallback - no lighting
        lightContribution = vec3(1.0);
    }
    
    // Add a small ambient component to prevent complete darkness
    vec3 ambient = vec3(0.1);
    
    // Combine with material color
    vec3 finalColor = (ambient + lightContribution) * materialColor;
    
    color = vec4(finalColor, 1.0);
}