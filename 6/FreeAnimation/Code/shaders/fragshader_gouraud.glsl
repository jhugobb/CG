#version 330 core

// Constants

// Inputs to the fragment shader
in vec3 vertColor;
in vec2 textureCoords;

//  Uniforms of the fragment shaders
uniform sampler2D samplerUniform;

// Output of the fragment shader
out vec4 fColor;

void main()
{
    vec4 color = vec4(vertColor, 1.0);
    vec4 textureColor = texture(samplerUniform, textureCoords);
    fColor = color * textureColor;
}
