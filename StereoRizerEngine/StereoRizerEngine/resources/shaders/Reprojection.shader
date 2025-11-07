#shader vertex
#version 450 core

layout(location = 0) in vec4 position;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec4 ClipSpacePos;  // Save the position assigned to gl_Position

void main()
{
    // Transform vertex to world space then to clip space
    vec4 worldPos = modelMatrix * position;
    gl_Position = projectionMatrix * viewMatrix * worldPos;
    
    // Pass the clip space position (same as gl_Position) to fragment shader
    ClipSpacePos = gl_Position;
}

#shader fragment
#version 450 core

layout(location = 0) out vec4 color;

in vec4 ClipSpacePos;  // Clip space position from vertex shader (same as gl_Position)

// Input textures
uniform sampler2D leftDepthTexture;    // Depth map from left renderer
uniform sampler2D leftColorTexture;    // Color map from left renderer  

// Camera matrices for left renderer
uniform mat4 leftViewMatrix;           // Left camera view matrix
uniform mat4 leftProjectionMatrix;     // Left camera projection matrix

// Camera matrices for right renderer (current)
uniform mat4 viewMatrix;               // Right camera view matrix
uniform mat4 projectionMatrix;         // Right camera projection matrix
uniform mat4 modelMatrix;

// Camera parameters
uniform float nearPlane;
uniform float farPlane;

// Depth comparison margin
const float DEPTH_MARGIN = 0.00001;      // Hardcoded margin for depth comparison
const vec3 MISMATCH_COLOR = vec3(1.0, 0.078, 0.576); // Pink color for mismatches

// Linearize depth value
float LinearizeDepth(float depth, float near, float far)
{
    float z = depth * 2.0 - 1.0; // Convert to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}

// Reconstruct world position from screen coordinates and depth using inverse matrices
vec3 ReconstructWorldPosition(vec2 screenCoord, float depth, mat4 invViewMatrix, mat4 invProjectionMatrix)
{
    // Convert screen coordinates to NDC
    vec4 clipSpacePos = vec4(screenCoord * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
    
    // Transform to view space using inverse projection
    vec4 viewSpacePos = invProjectionMatrix * clipSpacePos;
    viewSpacePos /= viewSpacePos.w;  // Perspective divide
    
    // Transform to world space using inverse view
    vec4 worldSpacePos = invViewMatrix * viewSpacePos;
    return worldSpacePos.xyz;
}

// Project world position to screen space for given camera
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
    // Step 1: Use the saved clip space position (same as gl_Position from vertex shader)
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
    
    // Step 6: Check if projection is within left camera's view bounds [0,1]
    if (leftScreenCoord.x < 0.0 || leftScreenCoord.x > 1.0 || 
        leftScreenCoord.y < 0.0 || leftScreenCoord.y > 1.0) {
        // Outside left camera view - render pink
        color = vec4(MISMATCH_COLOR, 1.0);
        return;
    }
    
    // Step 7: Sample depth from left camera at projected coordinates
    float leftDepthValue = texture(leftDepthTexture, leftScreenCoord).r;
    
    // Skip if no valid depth in left camera
    if (leftDepthValue >= 1.0) {
        color = vec4(MISMATCH_COLOR, 1.0);
        return;
    }
    
    // Step 8: Linearize both depths for proper comparison
    float leftLinearDepth = LinearizeDepth(leftDepthValue, nearPlane, farPlane);
    float rightLinearDepth = LinearizeDepth(rightDepthValue, nearPlane, farPlane);
    
    // Step 9: Compare depths - if close, sample left color, else mismatch color
    //if (abs(leftLinearDepth - rightLinearDepth) <= DEPTH_MARGIN) {
    //    vec4 leftColorValue = texture(leftColorTexture, leftScreenCoord);
    //    color = leftColorValue;
    //} else {
    //    // Depth mismatch - render pink
    //    color = vec4(MISMATCH_COLOR, 1.0);
    //}

    vec4 leftColorValue = texture(leftColorTexture, leftScreenCoord);
        color = leftColorValue;
}