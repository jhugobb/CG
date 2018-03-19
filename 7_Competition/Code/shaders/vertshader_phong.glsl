#version 330 core

// Constants

// Input locations of attributes
layout (location = 0) in vec3 vertCoordinates_in;
layout (location = 1) in vec3 vertNormal_in;
layout (location = 2) in vec2 textureCoords_in;

// Uniforms of the vertex shader
uniform mat4 modelTransform;
uniform mat4 projTransform;
uniform mat4 viewTransform;
uniform mat3 normalTransform;
uniform vec3 lightPosition;


// Output of the vertex stage
out vec3 vertNormal;
out vec3 vertCoor;
out vec3 lightPositionOut;
out vec2 textureCoords;
out vec3 eyePosition;

void main()
{
    // gl_Position is the output (a vec4) of the vertex shader
    gl_Position = projTransform * viewTransform * modelTransform * vec4(vertCoordinates_in, 1.0);

    mat4 viewModel = inverse(viewTransform);
    eyePosition = vec3(viewModel[3] / viewModel[3].w);

    vertCoor = vertCoordinates_in;

    vertNormal = vertNormal_in;

    vec4 light = modelTransform * vec4(lightPosition, 1.0);
    lightPositionOut = lightPosition;

    textureCoords = textureCoords_in;
}
