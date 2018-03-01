#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the inputs to the fragment shader
// These must have the same type and name!
in vec3 vertNormal;
in vec3 vertCoor;
in vec3 lightPosition;
in vec2 textureCoords;

// Specify the Uniforms of the fragment shaders
uniform mat4 modelTransfor;
uniform mat3 normalTransform;
uniform sampler2D samplerUniform;


// Specify the output of the fragment shader
// Usually a vec4 describing a color (Red, Green, Blue, Alpha/Transparency)
out vec4 fColor;

void main()
{

    float material[4] = float[4](0.2, 0.8, 0.0, 1);

    vec3 N = normalize(normalTransform * vertNormal);
    vec3 materialColor, lightColor;
    lightColor = vec3(1, 1, 1);
    vec4 textureColor = texture2D(samplerUniform, textureCoords);
    materialColor = vec3(textureColor/textureColor.w);
    vec3 IA, ID, IS;
    vec3 L, R;
    L = normalize(lightPosition - vertCoor);
    R = reflect(-L, N);
    IA = materialColor * material[0]; //material color defined as (0.5 , 0.5 , 0.5)
    ID = (max(0.0, dot(L, N)) * lightColor) * materialColor * material[1];
    IS = (pow(max(0.0, dot(R, 0 - vertCoor)), material[3]) * lightColor) * material[2]; //0 - vertCoor because we are in the origin of the reference

    fColor = vec4((IA + ID + IS), 1.0);

}
