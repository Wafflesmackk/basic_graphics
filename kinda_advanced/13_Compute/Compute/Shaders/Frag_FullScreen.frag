#version 430

in vec2 textureCoords;
out vec4 outputColor;

layout(binding = 0) uniform sampler2D imgage;

void main()
{
	outputColor = texture(imgage, textureCoords);
}