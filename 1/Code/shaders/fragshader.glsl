#version 330 core

void main()
{
    in vec3 interpolatedColor;

    out vec4 outcolor;

    outcolor = vec4(interpolatedColor, 1);
}
