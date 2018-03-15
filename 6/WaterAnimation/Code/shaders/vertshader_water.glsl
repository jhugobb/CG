#version 330 core

// Constants
#define M_PI 3.1415926535897932384626433832795
// Input locations of attributes
layout (location = 0) in vec3 vertCoordinates_in;
layout (location = 1) in vec3 vertNormal_in;
layout (location = 2) in vec2 uvCoor_in;

// Uniforms of the vertex shader
uniform mat4 modelTransform;
uniform mat4 projTransform;
//uniform mat3 normalTransform;


// Output of the vertex stage
out vec2 uvCoor;
out vec3 vertNormal;

void main()
{
    float freq = 2.0;
    float amp = 0.1;
    float phase = 0;
    float time = 0;
    vec3 vertCoor = vertCoordinates_in;
    // gl_Position is the output (a vec4) of the vertex shader
    gl_Position = projTransform * modelTransform * vec4(vertCoor.x, vertCoor.y, amp * sin(2.0 * M_PI * (freq * vertCoor.x + phase + time)), 1.0);
    uvCoor = uvCoor_in;

    float dU = amp * 2.0 * M_PI * freq * cos(2.0 * M_PI * (freq * uvCoor_in.x + phase + time));
    float dV = 0;
    vec3 normal = normalize(vec3(-dU, -dV, 1.0));
    vertNormal = normal;
}