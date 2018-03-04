#version 330 core

// Constants

// Inputs to the fragment shader
in vec3 vertNormal;

// Uniforms of the fragment shaders

// Output of the fragment shader
out vec4 fNormal;

void main()
{
    fNormal = vec4(normalize(vertNormal), 1.0);
}
