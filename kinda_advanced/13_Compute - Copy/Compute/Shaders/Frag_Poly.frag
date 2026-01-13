#version 430

in vec3 vNormal;
in vec3 vWorldPos;

out vec4 FragColor;

uniform vec3 lightDir = normalize(vec3(1, 1, 1));
uniform vec3 baseColor = vec3(0.8, 0.6, 0.3);

void main()
{
	vec3 n = normalize(vNormal);
	float diff = max(dot(n, -lightDir), 0.0);

	vec3 color = baseColor * (0.2 + diff);
	FragColor = vec4(color, 1.0);
}
