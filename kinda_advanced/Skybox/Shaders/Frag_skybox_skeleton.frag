#version 430

// pipeline-ból bejövő per-fragment attribútumok
in vec3 worldPosition;

out vec4 outputColor;

// skybox textúra
uniform samplerCube skyboxTexture;

void main()
{
	//outputColor = vec4( 1.0,0.0,1.0,1.0);
	//outputColor = vec4(worldPosition,1);
	vec3 W = normalize(worldPosition);
	/*if(worldPosition.y >= 0)
		outputColor = mix(vec4(0,1,1,1),vec4(1,1,1,1), W.y);
	else
		outputColor = vec4(0.1);*/
	vec3 L = normalize( vec3(1,0,1));

	outputColor = texture(skyboxTexture, worldPosition);

	float cosAlpha = cos(3.1415/24);
	float cosBeta = cos(3.1415/48);
	/*if(dot(W,L) >= cosAlpha){
		outputColor = vec4(1,1,0,1);
	}*/
	outputColor = mix(outputColor, vec4(1,1,0.5,1), smoothstep(cosAlpha, cosBeta,dot(W,L)));
}