#version 430

// pipeline-ból bejövő per-fragment attribútumok 
in vec4 color;
in vec2 ScreenPosition;

// kimenő érték - a fragment színe 
out vec4 outputColor;

// !!!!! VARÁZSLAT !!!!
// Erről bővebben később...
uniform float ElapsedTimeInSec = 0.0;
// !!!!!!!!!!!!

void main()
{
	//outputColor = vec4(1.0 -color.rgb, 1.0);
	//outputColor = color;
	//outputColor = vec4(1.0);
	//outputColor = color.bgra;
	outputColor = vec4(ScreenPosition, 0.0, 1.0);

	const float Radius = 0.5;
	const float RadiusSqr = Radius * Radius;
	const float lineWidth = 0.025;
	//float lengthNormPos =  length(ScreenPosition);//sqrt((ScreenPosition.x * ScreenPosition.x ) + (ScreenPosition.y * ScreenPosition.y));


	if(dot(ScreenPosition,ScreenPosition) > RadiusSqr + lineWidth ||
	dot(ScreenPosition,ScreenPosition) < RadiusSqr - lineWidth ) discard; //outputColor.rgb = vec3(1.0,0.0,1.0);
}
