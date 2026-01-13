#version 430

layout(location = 0)  in vec2 velocity;
layout(location = 1)  in vec2 worldPosition;
layout(location = 0) out vec4 outputColor;

void main()
{
	vec2 uv = gl_PointCoord*2-1;
	outputColor = (1 + clamp(velocity.xyyx, 0, 1))/2;
	if(length(uv)>1.) discard;
}