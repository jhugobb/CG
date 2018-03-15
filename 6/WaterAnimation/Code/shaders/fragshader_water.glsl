#version 330 core

// Constants

// Inputs to the fragment shader
in vec2 uvCoor;
in vec3 vertNormal;

// Uniforms of the fragment shaders

// Output of the fragment shader
out vec4 fColor;

void main()
{
    fColor = vec4(uvCoor.x, uvCoor.y, 0.0, 1.0);
}
