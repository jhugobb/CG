#version 330 core

// Constants

// Inputs to the fragment shader
in vec3 vertNormal;
in vec3 vertCoor;
in vec3 lightPosition;
in vec2 textureCoords;

//  Uniforms of the fragment shaders
uniform mat4 modelTransfor;
uniform mat3 normalTransform;
uniform sampler2D samplerUniform;

// Output of the fragment shader
out vec4 fColor;

void main()
{
    vec3 materialColor, lightColor;
    vec3 IA, ID, IS;
    vec3 L, R;
    
    // This is an arbitrary material with a small specular component due to cats not reflecting light
    float material[4] = float[4](0.2, 0.8, 0.0, 1);

    vec3 N = normalize(normalTransform * vertNormal);
    lightColor = vec3(1, 1, 1);

    vec4 textureColor = texture2D(samplerUniform, textureCoords);
    
    materialColor = vec3(textureColor/textureColor.w);

    L = normalize(lightPosition - vertCoor);
    R = reflect(-L, N);
    IA = materialColor * material[0];
    ID = (max(0.0, dot(L, N)) * lightColor) * materialColor * material[1];
    
    //0 - vertCoor because we are in the origin of the reference    
    IS = (pow(max(0.0, dot(R, 0 - vertCoor)), material[3]) * lightColor) * material[2]; 

    fColor = vec4((IA + ID + IS), 1.0);
}
