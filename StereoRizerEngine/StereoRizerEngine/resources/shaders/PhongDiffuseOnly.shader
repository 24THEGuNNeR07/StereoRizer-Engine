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

#ifdef USE_REPROJECTION
out vec4 ClipSpacePos;
#endif

void main()
{
    FragPos = vec3(modelMatrix * vec4(position, 1.0));
    WorldNormal = mat3(transpose(inverse(modelMatrix))) * normal;
    Normal = WorldNormal;
    
    gl_Position = projectionMatrix * viewMatrix * vec4(FragPos, 1.0);

#ifdef USE_REPROJECTION
    ClipSpacePos = gl_Position;
#endif
}

#shader fragment
#version 450 core

layout(location = 0) out vec4 color;

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


#ifdef USE_REPROJECTION
in vec4 ClipSpacePos;

uniform sampler2D leftDepthTexture;    // Depth map from left renderer
uniform sampler2D leftColorTexture;    // Color map from left renderer  

uniform mat4 leftViewMatrix;           // Left camera view matrix
uniform mat4 leftProjectionMatrix;     // Left camera projection matrix

uniform mat4 viewMatrix;               // Right camera view matrix
uniform mat4 projectionMatrix;         // Right camera projection matrix
uniform mat4 modelMatrix;
#endif

#ifdef SHOW_REPROJECTION_MASK
const vec3 MISMATCH_COLOR = vec3(1.0, 0.078, 0.576);
#endif

vec3 CalculateDirectionalLight(Light light, vec3 normal, vec3 fragPos) {
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    return light.color * light.intensity * diff;
}

vec3 CalculatePointLight(Light light, vec3 normal, vec3 fragPos) {
    vec3 lightDir = normalize(light.position - fragPos);
    float distance = length(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    float attenuation = 1.0 / (light.attenuation.x + light.attenuation.y * distance + light.attenuation.z * distance * distance);
    return light.color * light.intensity * diff * attenuation;
}

vec3 CalculateSpotLight(Light light, vec3 normal, vec3 fragPos) {
    vec3 lightDir = normalize(light.position - fragPos);
    float distance = length(light.position - fragPos);

    float diff = max(dot(normal, lightDir), 0.0);
    
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = cos(light.innerConeAngle) - cos(light.outerConeAngle);
    float intensity = clamp((theta - cos(light.outerConeAngle)) / epsilon, 0.0, 1.0);

    float attenuation = 1.0 / (light.attenuation.x + light.attenuation.y * distance + light.attenuation.z * distance * distance);
    
    return light.color * light.intensity * diff * intensity * attenuation;
}

#ifdef USE_REPROJECTION
vec2 ProjectToScreen(vec3 worldPos, mat4 viewMatrix, mat4 projectionMatrix)
{
    vec4 viewSpacePos = viewMatrix * vec4(worldPos, 1.0);
    vec4 clipSpacePos = projectionMatrix * viewSpacePos;
    vec3 ndcPos = clipSpacePos.xyz / clipSpacePos.w;
    return ndcPos.xy * 0.5 + 0.5;
}
#endif

void main()
{
#ifdef USE_REPROJECTION
    vec4 clipPos = ClipSpacePos;
    vec3 ndcPos = clipPos.xyz / clipPos.w;
    
    vec2 rightScreenCoord = ndcPos.xy * 0.5 + 0.5;
    float rightDepthValue = ndcPos.z * 0.5 + 0.5;
    
    mat4 invViewMatrix = inverse(viewMatrix);
    mat4 invProjectionMatrix = inverse(projectionMatrix);

    vec4 viewPos = inverse(projectionMatrix) * clipPos;
    viewPos /= viewPos.w;

    vec4 worldPos = inverse(viewMatrix) * viewPos;
    vec3 worldPosition = worldPos.xyz / worldPos.w;
    
    vec2 leftScreenCoord = ProjectToScreen(worldPosition, leftViewMatrix, leftProjectionMatrix);

    float leftDepthValue = texture(leftDepthTexture, leftScreenCoord).r;

    if (abs(leftDepthValue - rightDepthValue) <= 0.002 && leftDepthValue < 1.0) {
        vec4 leftColorValue = texture(leftColorTexture, leftScreenCoord);
        color = leftColorValue;
    } else {
#ifdef SHOW_REPROJECTION_MASK
        color = vec4(MISMATCH_COLOR, 1.0);
        return;
#endif
#endif
    vec3 norm = normalize(Normal);
    vec3 lightContribution;
    
    if (light.type == 0) 
        lightContribution = CalculateDirectionalLight(light, norm, FragPos);
    else if (light.type == 1)
        lightContribution = CalculatePointLight(light, norm, FragPos);
    else if (light.type == 2)
        lightContribution = CalculateSpotLight(light, norm, FragPos);
    else
        lightContribution = vec3(1.0);

    vec3 ambient = vec3(0.1);
    
    vec3 finalColor = (ambient + lightContribution) * materialColor;
    
    color = vec4(finalColor, 1.0);
#ifdef USE_REPROJECTION
    }
#endif
}