#version 330 core

// Constants
#define M_PI 3.1415926535897932384626433832795
#define WAVENR 8
// Input locations of attributes
layout (location = 0) in vec3 vertCoordinates_in;
layout (location = 1) in vec3 vertNormal_in;
layout (location = 2) in vec2 uvCoor_in;

// Uniforms of the vertex shader
uniform mat4 modelTransform;
uniform mat4 projTransform;
uniform mat3 normalTransform;
uniform float amp[WAVENR];
uniform float freq[WAVENR];
uniform float phase[WAVENR];
//uniform mat3 normalTransform;


// Output of the vertex stage
out vec3 N;
out vec3 materialColor;
out vec3 vertCoorOut;
out vec3 eyePosition;

float waveHeight(int waveIdx, float u)
{
    return amp[waveIdx] * sin(2.0 * M_PI * (freq[waveIdx] * u + phase[waveIdx] /*+ time*/));
}

float waveDU(int waveIdx, float u)
{
    return amp[waveIdx] * 2.0 * M_PI * freq[waveIdx] * cos(2.0 * M_PI * (freq[waveIdx] * u + phase[waveIdx]  /*+time*/));
}

void main()
{
    float z = 0;
    float dU = 0;
    float maxAmp = -1.0 / 0.0;



    for (int waveIdx = 0 ; waveIdx < WAVENR ; waveIdx++)
    {
        z += waveHeight(waveIdx, uvCoor_in.x);
        dU += waveDU(waveIdx, uvCoor_in.x);
        if (amp[waveIdx] > maxAmp)
            maxAmp = amp[waveIdx];
    }
    // gl_Position is the output (a vec4) of the vertex shader
    vec3 vertCoor = vec3(vertCoordinates_in.x, vertCoordinates_in.y, z);
    gl_Position = projTransform * modelTransform * vec4(vertCoor, 1.0);


    //eye position
    mat4 viewModel = inverse(projTransform);
    eyePosition = vec3(viewModel[3] / viewModel[3].w);

    float dV = 0;
    vec3 normal = normalize(vec3(-dU, -dV, 1.0));
    N = (normalTransform * normal);

    //do the smoothstep and mix thing
    vec3 maxColor = vec3(0, 0.6, 1); //light blue
    vec3 minColor = vec3(0, 0, 0.2);
    float interp = smoothstep(-maxAmp, maxAmp, z); //calculates the interpolation of z between the two most possible heights that could exist in the water


    materialColor = mix(minColor, maxColor, interp); //mixes the minimum and maximum color
    vertCoorOut = vertCoor;

}
