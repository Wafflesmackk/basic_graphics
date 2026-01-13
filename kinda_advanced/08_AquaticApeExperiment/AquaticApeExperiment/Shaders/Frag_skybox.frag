#version 430

// pipeline-ból bejövő per-fragment attribútumok
in vec3 worldPosition;

out vec4 outputColor;

// skybox textúra
uniform samplerCube skyboxTexture;

void main()
{
	// skybox textúra
	outputColor = texture( skyboxTexture, worldPosition );
}