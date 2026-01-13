#version 430

// pipeline-ból bejövő per-fragment attribútumok 
in vec3 color;

// kimenő érték - a fragment színe 
out vec4 outputColor;

uniform float multiplier = 1.0;
void main()
{
	outputColor = vec4( color  * multiplier, 1.0 );
}