#version 330 core

// Constants

// Inputs to the fragment shader
in vec3 N;

// Uniforms of the fragment shaders
uniform vec3 lightColor;
uniform vec4 material;
uniform vec3 lightPosition;

// Output of the fragment shader
out vec4 fColor;

void main()
{
    fColor = vec4((N.x + 1.0) / 2.0, (N.y + 1.0) / 2.0, (N.z + 1.0) / 2.0, 1.0); //(coor + 1) / 2 is used in order to transform the values that com in a [-1, 1] domain to the [0, 1], domain.
}
