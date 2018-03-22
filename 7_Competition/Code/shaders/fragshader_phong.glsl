#version 330 core

// Constants

// Inputs to the fragment shader
in vec3 vertNormal;
in vec3 vertCoor;
in vec3 lightPositionOut;
in vec2 textureCoords;
in vec3 eyePosition;

//  Uniforms of the fragment shaders
uniform mat4 modelTransfor;
uniform mat3 normalTransform;
uniform sampler2D samplerUniform;
uniform vec3 lightColor;
uniform vec4 material;
uniform bool placedBox;

// Output of the fragment shader
out vec4 fColor;

void main()
{
    vec3 IA, ID, IS;
    vec3 L, R;
    
    // This is an arbitrary material with a small specular component due to cats not reflecting light
    //float material[4] = float[4](0.2, 0.8, 0.0, 1);

    vec3 N = normalize(normalTransform * vertNormal);

    vec4 textureColor = texture(samplerUniform, textureCoords);
    
    vec3 materialColor = vec3(textureColor / textureColor.w);

    if (placedBox)
        materialColor += vec3(0.4, 0, 0);

    L = normalize(lightPositionOut - vertCoor);
    R = reflect(-L, N);
    IA = materialColor * material.x;
    ID = (max(0.0, dot(L, N)) * lightColor) * materialColor * material.y;
    
    //0 - vertCoor because we are in the origin of the reference    
    IS = (pow(max(0.0, dot(R, eyePosition - vertCoor)), material.w) * lightColor) * material.z;

    fColor = vec4((IA + ID + IS), 1.0);
}
