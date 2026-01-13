#version 430

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

out vec3 vNormal;
out vec3 vWorldPos;

uniform mat4 MVP = mat4(1);

void main()
{
	gl_Position = MVP * vec4(aPos, 1.0);
	vNormal = aNormal;
	vWorldPos = aPos;
}
