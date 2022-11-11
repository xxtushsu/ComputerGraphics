#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the input locations of attributes
layout (location = 0) in vec3 vertCoordinates_in;
layout (location = 1) in vec3 vertNormal_in;
layout (location = 2) in vec2 textureCoord_in;

// Light Constants
const vec3 light_position = vec3(100.0, 100.0, 100.0);
const vec3 light_color = vec3(1.0, 1.0, 1.0);

// Material Constants
const vec3 material_color = vec3(0.2, 0.2, 0.2);
const float material_ka = 0.3;
const float material_kd = 0.8;
const float material_ks = 0.9;
const int material_n = 3;

// Specify the Uniforms of the vertex shader
uniform mat4 modelViewTransform;
uniform mat4 projectionTransform;
uniform mat3 normalTransform;

// Specify the output of the vertex stage
out float I;
out vec2 textCoord;

void main()
{
    vec4 P = modelViewTransform * vec4(vertCoordinates_in, 1.0);
    vec3 N = normalize(normalTransform * vertNormal_in);
    vec3 V = normalize(P.xyz * -1);
    vec3 L = normalize(light_position - P.xyz);
    vec3 R = reflect(-L, N);

    float IA = material_ka;
    float ID = material_kd * max(0, dot(N, L));
    float IS = (dot(N, L) > 0) ? max(0.0, pow(dot(R, V), material_n)) : 0.0;

    I = IA + ID + IS;
    textCoord = textureCoord_in;
    gl_Position = projectionTransform * P;
}
