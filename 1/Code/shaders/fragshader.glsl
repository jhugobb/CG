#version 330 core

in vec3 interpolatedColor;

out vec4 outcolor;

void main()
{
    outcolor = vec4(interpolatedColor, 1);
}
