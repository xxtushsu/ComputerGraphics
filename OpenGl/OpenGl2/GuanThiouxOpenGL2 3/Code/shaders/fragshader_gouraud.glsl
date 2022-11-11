#version 330 core

// Define constants
#define M_PI 3.141593

uniform sampler2D textureMap;

in float I;
in vec2 textCoord;

out vec4 fColor;

void main()
{
    vec4 textureColor = texture(textureMap, textCoord);
    fColor = vec4(textureColor.bgr * I, 1);
}
