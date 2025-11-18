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
out vec4 ClipSpacePos;

void main()
{
    FragPos = vec3(modelMatrix * vec4(position, 1.0));
    WorldNormal = mat3(transpose(inverse(modelMatrix))) * normal;
    Normal = WorldNormal;
    
    gl_Position = projectionMatrix * viewMatrix * vec4(FragPos, 1.0);

    ClipSpacePos = gl_Position;
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
in vec4 ClipSpacePos;

#ifdef USE_REPROJECTION
uniform sampler2D leftDepthTexture;    // Depth map from left renderer
uniform sampler2D leftColorTexture;    // Color map from left renderer  

// Camera matrices for left renderer
uniform mat4 leftViewMatrix;           // Left camera view matrix
uniform mat4 leftProjectionMatrix;     // Left camera projection matrix

uniform mat4 viewMatrix;               // Right camera view matrix
uniform mat4 projectionMatrix;         // Right camera projection matrix
uniform mat4 modelMatrix;
#endif

const vec3 MISMATCH_COLOR = vec3(1.0, 0.078, 0.576); // Pink color for mismatches

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

vec2 ProjectToScreen(vec3 worldPos, mat4 viewMatrix, mat4 projectionMatrix)
{
    // Transform to view space
    vec4 viewSpacePos = viewMatrix * vec4(worldPos, 1.0);
    
    // Transform to clip space
    vec4 clipSpacePos = projectionMatrix * viewSpacePos;
    
    // Perspective divide
    vec3 ndcPos = clipSpacePos.xyz / clipSpacePos.w;
    
    // Convert to screen coordinates
    return ndcPos.xy * 0.5 + 0.5;
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
    
#ifdef USE_REPROJECTION
    vec4 clipPos = ClipSpacePos;
    
    // Step 2: Convert to NDC by perspective divide
    vec3 ndcPos = clipPos.xyz / clipPos.w;
    
    // Step 3: Convert NDC to screen coordinates [0,1]
    vec2 rightScreenCoord = ndcPos.xy * 0.5 + 0.5;
    float rightDepthValue = ndcPos.z * 0.5 + 0.5; // Convert Z from [-1,1] to [0,1]
    
    // Step 4: Reconstruct world position using inverse matrices
    mat4 invViewMatrix = inverse(viewMatrix);
    mat4 invProjectionMatrix = inverse(projectionMatrix);
    // Fragment shader
    vec4 viewPos = inverse(projectionMatrix) * clipPos;
    viewPos /= viewPos.w; // divide by w to go from clip to view space

    vec4 worldPos = inverse(viewMatrix) * viewPos;
    vec3 worldPosition = worldPos.xyz / worldPos.w;
    
    // Step 5: Project world position to LEFT camera screen coordinates
    vec2 leftScreenCoord = ProjectToScreen(worldPosition, leftViewMatrix, leftProjectionMatrix);

    float leftDepthValue = texture(leftDepthTexture, leftScreenCoord).r;
    
    // Skip if no valid depth in left camera
    if (leftDepthValue >= 1.0) {
        color = vec4(MISMATCH_COLOR, 1.0);
        return;
    }

    if (abs(leftDepthValue - rightDepthValue) <= 0.002) {
        vec4 leftColorValue = texture(leftColorTexture, leftScreenCoord);
        color = leftColorValue;
    } else {
        // Depth mismatch - render pinkrightScreenCoord
        color = vec4(MISMATCH_COLOR, 1.0);
    }
#else
    color = vec4(finalColor, 1.0);
#endif
}