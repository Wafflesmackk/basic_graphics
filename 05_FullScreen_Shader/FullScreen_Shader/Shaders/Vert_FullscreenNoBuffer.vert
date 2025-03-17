#version 430

// lokális változók: két tömb 
vec4 positions[6] = vec4[6](
	vec4(-1, -1, 0, 1),
	vec4( 1, -1, 0, 1),
	vec4(-1,  1, 0, 1),

	vec4(-1,  1, 0, 1),
	vec4( 1, -1, 0, 1),
	vec4( 1,  1, 0, 1)
);

vec4 colors[6] = vec4[6](
	vec4(1, 0, 0, 1),
	vec4(0, 1, 0, 1),
	vec4(0, 0, 1, 1),

	vec4(0, 0, 1, 1),
	vec4(0, 1, 0, 1),
	vec4(1, 1, 1, 1)
);


// a pipeline-ban tovább adandó értékek 
out vec4 color;
out vec2 ScreenPosition;

uniform float ElapsedTimeInSec = 0.0;

mat4 scaleMatrix(float sx, float sy, float sz){

	mat4 ret = mat4(
		sx, 0.0, 0.0, 0.0,
		0.0, sy, 0.0, 0.0,
		0.0, 0.0, sx, 0.0,
		0.0, 0.0, 0.0, 1.0
	);
	return ret;
};

mat4 rotateMatrix(float angle){
	
	float cosAngle = cos(angle);
	float sinAngle = sin(angle);
	mat4 ret = mat4(
		cosAngle, sinAngle, 0.0, 0.0,
		-sinAngle, cosAngle, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0
	);
	return ret;
};

mat4 translateMatrix(float tx, float ty, float tz){
	
	mat4 ret = mat4(
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		tx, ty, tz, 1.0
	);
	return ret;
};


void main()
{
	// gl_VertexID: https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/gl_VertexID.xhtml
	gl_Position  =/* translateMatrix(sin(ElapsedTimeInSec * 0.8) * 0.7, cos(ElapsedTimeInSec * 0.8) * 0.5, 0.2) * rotateMatrix(0.25 * ElapsedTimeInSec) * scaleMatrix(0.15,0.2,0.05) * */ positions[gl_VertexID];
	ScreenPosition = positions[gl_VertexID].xy;

	color = colors[gl_VertexID];
}
