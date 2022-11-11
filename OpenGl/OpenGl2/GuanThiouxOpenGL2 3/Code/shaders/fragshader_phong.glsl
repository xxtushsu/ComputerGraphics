#version 330 core

// Define constants
#define M_PI 3.141593

uniform sampler2D textureMap;

// Inputs
in vec3 N;
in vec3 L;
in vec3 V;
in vec2 textCoord;

const float material_ka = 0.3;
const float material_kd = 0.8;
const float material_ks = 0.9;
const int material_n = 3;


// Outputs
out vec4 fColor;

void main()
{
    vec4 textureColor = texture(textureMap, textCoord);

    vec3 R = reflect(-L, N);

    float IA = material_ka;
    float ID = material_kd * max(0, dot(N, L));
    float IS = (dot(N, L) > 0) ? max(0.0, pow(dot(R, V), material_n)) : 0.0;
    float I = IA + ID + IS;

    fColor = vec4(textureColor.bgr * I, 1);
}
