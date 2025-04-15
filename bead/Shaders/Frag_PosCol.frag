#version 430

// pipeline-ból bejövő per-fragment attribútumok 
in vec3 color;

// kimenő érték - a fragment színe 
out vec4 outputColor;

uniform float EmissiveFactor = 0.0;
uniform vec3 EmissiveColor = vec3(1.0);

void main()
{
	outputColor = vec4( color + EmissiveColor, 1.0 );
}
