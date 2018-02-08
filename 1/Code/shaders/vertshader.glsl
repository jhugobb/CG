#version 330 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec3 color;

void main()
{
    out vec3 interpolatedColor;

    interpolatedColor = color;
    gl_Position = vec4(color, 0, 1);
}
