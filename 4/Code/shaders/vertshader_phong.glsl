#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the input locations of attributes
layout (location = 0) in vec3 vertCoordinates_in;
layout (location = 1) in vec3 vertNormal_in;
layout (location = 2) in vec2 textureCoords_in;

// Specify the Uniforms of the vertex shader
uniform mat4 modelTransform;
uniform mat4 projTransform;
uniform mat3 normalTransform;


// Specify the output of the vertex stage
out vec3 vertNormal;
out vec3 vertCoor;
out vec3 lightPosition;
out vec2 textureCoords;

void main()
{
    // gl_Position is the output (a vec4) of the vertex shader
    gl_Position = projTransform * modelTransform * vec4(vertCoordinates_in, 1.0);

    vertCoor = vertCoordinates_in;

    vertNormal = normalTransform * vertNormal_in;

    vec4 light = modelTransform * vec4(100.0, 100.0, 150.0, 1.0);
    lightPosition = vec3(light/light.w);

    textureCoords = textureCoords_in;
}
