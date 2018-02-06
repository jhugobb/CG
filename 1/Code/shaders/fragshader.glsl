#version 330 core

in vec3 interpolatedColor;

out vec4 finalColor;

void main()
{
    finalColor = vec4(interpolatedColor, 1);
}
