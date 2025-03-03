#version 430

// pipeline-ból bejövő per-fragment attribútumok 
in vec4 color;

// kimenő érték - a fragment színe 
out vec4 outputColor;

// !!!!! VARÁZSLAT !!!!
// Erről bővebben később...
uniform float ElapsedTimeInSec = 0.0;
// !!!!!!!!!!!!

void main()
{
	outputColor = color;
}
