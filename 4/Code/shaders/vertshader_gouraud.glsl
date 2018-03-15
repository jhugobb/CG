#version 330 core

// Constants

// Input locations of attributes
layout (location = 0) in vec3 vertCoordinates_in;
layout (location = 1) in vec3 vertNormal_in;
layout (location = 2) in vec2 textureCoords_in;

// Uniforms of the vertex shader
uniform mat4 modelTransform;
uniform mat4 projTransform;
uniform mat3 normalTransform;


// Output of the vertex stage
out vec3 vertColor;
out vec2 textureCoords;

void main()
{
    vec3 materialColor, lightColor;
    vec3 IA, ID, IS;
    vec3 L, R;

    // gl_Position is the output (a vec4) of the vertex shader
    gl_Position = projTransform * modelTransform * vec4(vertCoordinates_in, 1.0);

    // We chose the light position to be at (100, 100, 150)
    vec4 light = modelTransform * vec4(100.0, 100.0, 150.0, 1.0);

    vec3 lightPosition = vec3(light/light.w);
    // This is an arbitrary material with a small specular component due to cats not reflecting light    
    float material[4] = float[4](0.2, 0.8, 0.0, 1);

    vec3 N = normalize(normalTransform * vertNormal_in);
    materialColor = vec3(1, 1, 1);
    lightColor = vec3(1, 1, 1);

    L = normalize(lightPosition - vertCoordinates_in);
    R = normalize(2 * dot(L, N) * N - L);
    IA = materialColor * material[0]; //material color defined as (0.5 , 0.5 , 0.5)
    ID = (max(0.0, dot(L, N)) * lightColor) * materialColor * material[1];
    IS = (pow(max(0.0, dot(R, 0 - vertCoordinates_in)), material[3]) * lightColor) * material[2]; //0 - vertCoor because we are in the origin of the reference

    vertColor = IA + ID + IS;

    textureCoords = textureCoords_in;
}
