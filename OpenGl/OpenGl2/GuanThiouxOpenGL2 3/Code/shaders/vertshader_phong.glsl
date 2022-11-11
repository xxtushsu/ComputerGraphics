#version 330 core

// Define constants
#define M_PI 3.141593

// Specify the input locations of attributes
layout (location = 0) in vec3 vertCoordinates_in;
layout (location = 1) in vec3 vertNormal_in;
layout (location = 2) in vec2 textureCoord_in;

// Specify the Uniforms of the vertex shader
uniform mat4 modelViewTransform;
uniform mat4 projectionTransform;
uniform mat3 normalTransform;
// uniform sampler2D textureMap;

// Light Constants
const vec3 light_position = vec3(100.0, 100.0, 100.0);

// Specify the output of the vertex stage
out vec3 N;
out vec3 L;
out vec3 V;
out vec2 textCoord;

void main()
{
    vec4 P = modelViewTransform * vec4(vertCoordinates_in, 1);
    N = normalize(normalTransform * vertNormal_in);
    V = normalize(P.xyz * -1);
    L = normalize(light_position - P.xyz);
    gl_Position = projectionTransform * P;
    textCoord = textureCoord_in;
}
