#version 430

// pipeline-ból bejövő per-fragment attribútumok 
in vec3 worldPosition;
in vec3 worldNormal;
in vec2 textureCoords;

// kimenő érték - a fragment színe 
out vec4 outputColor;

// textúra mintavételező objektum 
uniform sampler2D textureImage;

uniform vec3 cameraPosition;

// fényforrás tulajdonságok 
uniform vec4 lightPosition = vec4( 0.0, 1.0, 0.0, 0.0);

uniform vec3 La = vec3(0.0, 0.0, 0.0 );
uniform vec3 Ld = vec3(1.0, 1.0, 1.0 );
uniform vec3 Ls = vec3(1.0, 1.0, 1.0 );

uniform float lightConstantAttenuation    = 1.0;
uniform float lightLinearAttenuation      = 1.0;
uniform float lightQuadraticAttenuation   = 1.0;

// anyag tulajdonságok 

uniform vec3 Ka = vec3( 1.0 );
uniform vec3 Kd = vec3( 1.0 );
uniform vec3 Ks = vec3( 1.0 );

uniform float Shininess = 20.0;

/* segítség:  normalizálás:  http://www.opengl.org/sdk/docs/manglsl/xhtml/normalize.xml
	- skaláris szorzat:   http://www.opengl.org/sdk/docs/manglsl/xhtml/dot.xml
	- clamp: http://www.opengl.org/sdk/docs/manglsl/xhtml/clamp.xml
	- reflect: http://www.opengl.org/sdk/docs/manglsl/xhtml/reflect.xml
			 reflect(beérkező_vektor, normálvektor);  pow(alap, kitevő); 
*/

struct LightProperties
{
	vec4 pos;
	vec3 La;
	vec3 Ld;
	vec3 Ls;
	float constantAttenuation;
	float linearAttenuation;
	float quadraticAttenuation;
};

struct MaterialProperties
{
	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
	float Shininess;
};

vec3 lighting(LightProperties light, vec3 position, vec3 normal, MaterialProperties material)
{
	vec3 Ambient = light.La * material.Ka;

	vec3 ToLight;

	float LightDistance = 0.0f;
	
	if(light.pos.w == 0.0){
		ToLight = light.pos.xyz;
	}
	else{
		ToLight = light.pos.xyz - position;
		LightDistance = length(ToLight);
	}

	ToLight = normalize(ToLight);

	float Attenuation = 1.0 / (light.constantAttenuation + light.linearAttenuation * LightDistance + light.quadraticAttenuation * LightDistance * LightDistance);

	float DiffuseFactor = max(dot(ToLight,normal),0.0) * Attenuation;

	vec3 Diffuse = light.Ld * material.Kd * DiffuseFactor;

	vec3 reflectDir = reflect(-ToLight, normal);

	vec3 viewDir = normalize( cameraPosition - position);

	float SpecualFactor = pow(max(dot(viewDir, reflectDir),0.0),material.Shininess) * Attenuation;

	vec3 SpecularColor = SpecualFactor * light.Ls * material.Ks;

	return SpecularColor +  Diffuse + Ambient;
}

void main()
{
	vec3 normal =  normalize(worldNormal);

	LightProperties light;
	light.pos = lightPosition;
	light.La = La;
	light.Ld = Ld;
	light.Ls = Ls;
	light.constantAttenuation = lightConstantAttenuation;
	light.linearAttenuation = lightLinearAttenuation;
	light.quadraticAttenuation = lightQuadraticAttenuation;

	MaterialProperties material;
	material.Ka = Ka;
	material.Kd = Kd;
	material.Ks = Ks;
	material.Shininess = Shininess;

	vec3 shadedColor = lighting(light, worldPosition, normal, material);
	outputColor = vec4(shadedColor, 1) * texture(textureImage, textureCoords);
	//outputColor = vec4(normal * 0.5 + 0.5,1.0); //adhoc debug
}
