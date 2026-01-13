#version 430

in vec3 vNormal;
in vec3 vWorldPos;
in vec2 textCoor;
uniform int uPolygonN;

out vec4 outputColor;

uniform vec3 baseColorIn = vec3(0.8, 0.2, 0.2); 
uniform vec3 baseColorOut = vec3(0.2, 0.4, 0.8);

/*uniform vec3 baseColorIn = vec3(1.0, 1.0, 1.0); 
uniform vec3 baseColorOut = vec3(1.0, 1.0, 1.0);*/

uniform vec3 lightDir1 = normalize(vec3( 1.0,  1.0,  1.0));
uniform vec3 lightDir2 = normalize(vec3(-1.0,  0.5, -0.2));

uniform sampler2D textureOutside;
uniform sampler2D textureInside;
const float pi = 3.14159265359;

float polygonEdgeDistance(vec2 p, int N, out float insideMask)
{
    float inradius = cos(pi / float(N));
    float minD = 1e9;
    insideMask = 1.0;

    for (int i = 0; i < N; ++i)
    {
        float ang = (float(i) + 0.5) * (2.0 * pi / float(N));
        vec2 n = vec2(cos(ang), sin(ang));      

        float d = inradius - dot(p, n);      
        minD = min(minD, d);

        if (d < 0.0) insideMask = 0.0;        
    }

    return minD;
}


void main()
{

    vec3 n = normalize(vNormal);

    float d1 = max(dot(n, -lightDir1), 0.0);
    float d2 = max(dot(n, -lightDir2), 0.0);

    float ambient = 0.25;
    float diffuse = d1 + 0.7 * d2;
    float lighting = ambient + diffuse;

    vec4 tex =
        gl_FrontFacing
        ? texture(textureInside,  textCoor)
        : texture(textureOutside, textCoor);

    vec3 baseColor =
        gl_FrontFacing
        ? baseColorIn
        : baseColorOut;

    vec3 surfaceColor = tex.rgb * baseColor * lighting;

vec2 p = textCoor * 2.0 - 1.0;

float inside;
float dist = polygonEdgeDistance(p, uPolygonN, inside);

if (inside < 0.5)
    discard;

float edgeWidth = 0.03; 
float edge = 1.0 - smoothstep(0.0, edgeWidth, dist);

vec3 edgeColor = vec3(0.0);
vec3 finalColor = mix(surfaceColor, edgeColor, edge);

outputColor = vec4(finalColor, tex.a);

}
