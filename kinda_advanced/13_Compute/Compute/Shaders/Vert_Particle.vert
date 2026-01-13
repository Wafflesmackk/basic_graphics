#version 430

layout(location = 0) in vec2 inputObjectSpacePosition;
layout(location = 1) in vec2 vs_in_vel;

// Variables going forward through the pipeline
layout(location = 0) out vec2 vs_out_vel;
layout(location = 1) out vec2 worldPosition;

void main()
{
	gl_Position = vec4( inputObjectSpacePosition, -1, 1 );
	vs_out_vel = vs_in_vel;
	worldPosition = inputObjectSpacePosition;
}