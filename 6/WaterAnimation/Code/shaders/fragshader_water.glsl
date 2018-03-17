#version 330 core

// Constants

// Inputs to the fragment shader
in vec3 N;
in vec3 materialColor;
in vec3 vertCoorOut;
in vec3 eyePosition;

// Uniforms of the fragment shaders
uniform vec3 lightColor;
uniform vec4 material;
uniform vec3 lightPosition;

// Output of the fragment shader
out vec4 fColor;

void main()
{
    vec3 L = normalize(lightPosition - vertCoorOut);
    vec3 R = reflect(-L, N);
    vec3 IA = materialColor * material.x;
    vec3 ID = (max(0.0, dot(L, N)) * lightColor) * materialColor * material.y;

    vec3 IS = (pow(max(0.0, dot(R, eyePosition - vertCoorOut)), material.w) * lightColor) * material.z;

    fColor = vec4((IA + ID + IS), 1.0);

    //fColor = vec4((N.x + 1.0) / 2.0, (N.y + 1.0) / 2.0, (N.z + 1.0) / 2.0, 1.0); //(coor + 1) / 2 is used in order to transform the values that com in a [-1, 1] domain to the [0, 1], domain.
}
