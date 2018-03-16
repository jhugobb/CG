#version 330 core

// Constants

// Inputs to the fragment shader
in vec3 vertNormal;

// Uniforms of the fragment shaders

// Output of the fragment shader
out vec4 fColor;

void main()
{
    fColor = vec4((vertNormal.x + 1.0) / 2.0, (vertNormal.y + 1.0) / 2.0, (vertNormal.z + 1.0) / 2, 1.0); //(coor + 1) / 2 is used in order to transform the values that com in a [-1, 1] domain to the [0, 1], domain.
}
