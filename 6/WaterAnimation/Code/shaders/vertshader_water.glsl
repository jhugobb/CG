#version 330 core

// Constants

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

void main()
{
    vec3 vertCoordinates = vertCoordinates_in;
    // gl_Position is the output (a vec4) of the vertex shader
    gl_Position = projTransform * modelTransform * vec4(vertCoordinates, 1.0);
    uvCoor = uvCoor_in;
}
