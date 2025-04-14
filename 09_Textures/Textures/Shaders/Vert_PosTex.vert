#version 430

// VBO-ból érkező változók
layout( location = 0 ) in vec3 inputObjectSpacePosition;
layout( location = 1 ) in vec2 inputTextureCoords;

// a pipeline-ban tovább adandó értékek
out vec2 textureCoords;
out vec3 worldPosition;

// shader külső paraméterei

// transzformációs mátrixok
uniform mat4 world;
uniform mat4 viewProj; // Egyben adjuk át, előre össze szorozva a view és projection mátrixokat.

void main()
{
	gl_Position  = viewProj * world * vec4( inputObjectSpacePosition, 1.0 );
	worldPosition = (world * vec4( inputObjectSpacePosition, 1.0 )).xyz;
	textureCoords = inputTextureCoords;
}
