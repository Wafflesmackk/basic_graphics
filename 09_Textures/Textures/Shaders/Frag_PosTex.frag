#version 430

// pipeline-ból bejövő per-fragment attribútumok
in vec2 textureCoords;
in vec3 worldPosition;

// kimenő érték - a fragment színe
out vec4 outputColor;

// textúra mintavételező objektum
uniform sampler2D textureImage;
uniform sampler2D textureMask;

uniform float indicatorElevation = 0.0;
uniform float indicatorWidth = 0.025;

void main()
{
	float Mask =  texture( textureMask, textureCoords ).r;
	if(Mask < 0.1)discard;

	float indicatorMask = 1.0 - smoothstep( abs(worldPosition.y - indicatorElevation), 0.0, indicatorWidth);

	vec4 indicatorColor = vec4(1,0,0,1);

	vec4 texColor = texture( textureImage, textureCoords );

	outputColor = mix(texColor, indicatorColor, indicatorMask);
}
