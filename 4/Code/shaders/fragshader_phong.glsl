#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the inputs to the fragment shader
// These must have the same type and name!
in vec3 vertNormal;
in vec3 vertCoor;

// Specify the Uniforms of the fragment shaders
uniform mat4 modelTransform; // for example
uniform mat3 normalTransform; // for example


// Specify the output of the fragment shader
// Usually a vec4 describing a color (Red, Green, Blue, Alpha/Transparency)
out vec4 fNormal;

void main()
{

    vec3 lightPosition = vec3(modelTransform * vec4(100.0, 100.0, 150.0, 1.0));
    float material[4] = float[4](0.5, 0.5, 0.5, 20);

    vec3 N = normalize(normalTransform * vertNormal);
    vec3 materialColor, lightColor;
    materialColor = vec3(1, 1, 1);
    lightColor = vec3(1, 1, 1);

    vec3 IA, ID, IS;
    vec3 L, R;
    L = normalize(lightPosition - vertCoor);
    R = normalize(2 * dot(L, N) * N - L);
    IA = materialColor * material[0]; //material color defined as (0.5 , 0.5 , 0.5)
    ID = (max(0.0, dot(L, N)) * lightColor) * materialColor * material[1];
    IS = (pow(max(0.0, dot(R, 0 - vertCoor)), material[3]) * lightColor) * material[2]; //0 - vertCoor because we are in the origin of the reference

    fNormal = vec4(IA + ID + IS, 1.0);
}
