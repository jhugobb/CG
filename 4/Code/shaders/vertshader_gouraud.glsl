#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the input locations of attributes
layout (location = 0) in vec3 vertCoordinates_in;
layout (location = 1) in vec3 vertNormal_in;

// Specify the Uniforms of the vertex shader
uniform mat4 modelTransform;
uniform mat4 projTransform;
uniform mat3 normalTransform;


// Specify the output of the vertex stage
out vec3 vertColor;

void main()
{
    // gl_Position is the output (a vec4) of the vertex shader
    // Currently without any transformation
    gl_Position = projTransform * modelTransform * vec4(vertCoordinates_in, 1.0);

    vec3 lightPosition = vec3(modelTransform * vec4(100.0, 100.0, 150.0, 1.0));
    float material[4] = float[4](0.5, 0.5, 0.5, 20);

    vec3 N = normalize(normalTransform * vertNormal_in);
    vec3 materialColor, lightColor;
    materialColor = vec3(1, 1, 1);
    lightColor = vec3(1, 1, 1);

    vec3 IA, ID, IS;
    vec3 L, R;
    L = normalize(lightPosition - vertCoordinates_in);
    R = normalize(2 * dot(L, N) * N - L);
    IA = materialColor * material[0]; //material color defined as (0.5 , 0.5 , 0.5)
    ID = (max(0.0, dot(L, N)) * lightColor) * materialColor * material[1];
    IS = (pow(max(0.0, dot(R, 0 - vertCoordinates_in)), material[3]) * lightColor) * material[2]; //0 - vertCoor because we are in the origin of the reference

    vertColor = IA + ID + IS;
}
