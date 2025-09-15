#version 430

// pipeline-ból bejövő per-fragment attribútumok 
in vec3 worldPosition;
in vec3 worldNormal;
in vec2 textureCoords;

// kimenő érték - a fragment színe 
out vec4 outputColor;


uniform int objectTypeID; // 0: default, 1: asztallap, 2: robot

// textúra mintavételező objektum 
uniform sampler2D textureImage;
uniform sampler2D shineTexture;

uniform vec3 cameraPosition;

// fényforrás tulajdonságok 
uniform vec4 lightPosition = vec4( 1/3.0, 2/3.0, 2/3.0, 0.0);

uniform vec3 La = vec3(0.1, 0.1, 0.1 );
uniform vec3 Ld = vec3(1.0, 1.0, 1.0 );
uniform vec3 Ls = vec3(1.0, 1.0, 1.0 );

uniform float lightConstantAttenuation    = 1.0;
uniform float lightLinearAttenuation      = 0.0;
uniform float lightQuadraticAttenuation   = 0.025;

// anyag tulajdonságok 

uniform vec3 Ka = vec3( 1.0 );
uniform vec3 Kd = vec3( 1.0 );
uniform vec3 Ks = vec3( 1.0 );

uniform float Shininess = 100.0;

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
	vec3 toLight;
	float lightDist = 0;
	if(light.pos.w == 0){ // irányfény
		toLight = light.pos.xyz;
	}else{ // pontfény
		toLight = light.pos.xyz - position;
		lightDist = length(toLight); // distance(light.pos.xyz, position)
	}
	toLight = normalize(toLight);
	float attenuation = 1.0 / (
		light.constantAttenuation +
		light.linearAttenuation * lightDist +
		light.quadraticAttenuation * lightDist * lightDist
	);

	// ambiens
	vec3 ambient = light.La * material.Ka;
	// diffúz
	float diffuseFacor = max(0, dot(toLight, normal)) * attenuation;
	vec3 diffuse = diffuseFacor * light.Ld * material.Kd;
	// spekuláris csillanás
	vec3 viewDir = normalize(cameraPosition - position);
	vec3 reflectDir = reflect(-toLight, normal);
	float specularFactor = pow(max(0,dot(viewDir,reflectDir)), material.Shininess) * attenuation;
	vec3 specular = specularFactor * light.Ls * material.Ks;

	return ambient + diffuse + specular;
}

void main()
{
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

	if(objectTypeID == 2){
		float shineVal = texture(shineTexture, textureCoords).r;
		if(shineVal == 0){
			material.Ks = vec3(0);
		}else{
			material.Shininess = 16.0 * shineVal;
		}
	}


	vec3 normal = normalize(worldNormal);
	vec3 shadedColor = lighting(light, worldPosition, normal, material);


	vec4 tex = texture(textureImage, textureCoords);

	if(objectTypeID == 1){ // asztallap
		float u = textureCoords.x, v = textureCoords.y;
		int val = int(floor(8*u) + floor(8*v));
		if(val % 2 == 1){
			tex *= 0.8;
		}
	}
	outputColor = vec4(shadedColor, 1) * tex;


	// outputColor = vec4(normal, 1);
}
