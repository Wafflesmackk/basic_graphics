#version 430

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 inTex;


uniform mat4 MVP;
uniform mat4 M;       
uniform mat3 normalMat; 

out vec3 vNormal;
out vec3 vWorldPos;
out vec2 textCoor;

void main()
{
    vec4 worldPos = M * vec4(aPos, 1.0);
    vWorldPos = worldPos.xyz;
    vNormal   = normalize(normalMat * aNormal);
    textCoor = inTex;

    gl_Position = MVP * vec4(aPos, 1.0);
}
