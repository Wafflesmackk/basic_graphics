#version 430

// pipeline-ból bejövő per-fragment attribútumok 
in vec3 worldPosition;
in vec3 worldNormal;
in vec2 textureCoords;

// kimenő érték - a fragment színe 
out vec4 outputColor;

// textúra mintavételező objektum 
uniform sampler2D textureImage;
uniform sampler2D textureShining;

uniform vec3 cameraPosition;

// fényforrás tulajdonságok 
uniform vec4 lightPosition = vec4( 0.0, 1.0, 0.0, 0.0);
uniform vec4 lightPosition2 = vec4( 0.0, 1.0, 0.0, 0.0);

uniform vec3 La = vec3(0.0, 0.0, 0.0 );
uniform vec3 Ld = vec3(1.0, 1.0, 1.0 );
uniform vec3 Ls = vec3(1.0, 1.0, 1.0 );

uniform vec3 La2 = vec3(0.0, 0.0, 0.0 );
uniform vec3 Ld2 = vec3(1.0, 1.0, 1.0 );
uniform vec3 Ls2 = vec3(1.0, 1.0, 1.0 );

uniform float lightConstantAttenuation    = 1.0;
uniform float lightLinearAttenuation      = 0.0;
uniform float lightQuadraticAttenuation   = 0.0;

// anyag tulajdonságok 

uniform vec3 Ka = vec3( 1.0 );
uniform vec3 Kd = vec3( 1.0 );
uniform vec3 Ks = vec3( 1.0 );

uniform float Shininess = 20.0;

const int SHADER_STATE_DEFAULT = 0;
const int SHADER_STATE_ROBOT = 1;
const int SHADER_STATE_TABLE = 2;
uniform int state;

uniform float indicatorFactor = 0.0;
uniform vec3 indicatorColor = vec3(1.0,0.0,1.0);
uniform bool isIndicatorLocal = false;
uniform vec2 indicatorUV = vec2(0.5,0.5);
uniform float indicatorR = 0.25;

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
	
	vec3 ToLight; // A fényforrásBA mutató vektor 
	float LightDistance = 0.0; // A fényforrástól vett távolság 
	
	if ( light.pos.w == 0.0 ) // irány fényforrás (directional light) 
	{
		// Irányfényforrás esetén minden pont ugyan abból az irányból van megvilágítva
		ToLight	= light.pos.xyz;
		// A távolságot 0-n hagyjuk, hogy az attenuáció ne változtassa a fényt
	}
	else				  // pont fényforrás (positional light) 
	{
		// Pontfényforrás esetén kiszámoljuk a fragment pontból a fényforrásba mutató vektort, ...
		ToLight	= light.pos.xyz - position;
		// ...  és a távolságot a fényforrástól 
		LightDistance = length(ToLight);
	}
	// Normalizáljuk a fényforrásba mutató vektort 
	ToLight = normalize(ToLight);
	
	// Attenuáció (fényelhalás) kiszámítása 
	float Attenuation = 1.0 / ( light.constantAttenuation + light.linearAttenuation * LightDistance + light.quadraticAttenuation * LightDistance * LightDistance);
	
	// Ambiens komponens 
	// Ambiens fény mindenhol ugyanakkora 
	vec3 Ambient = light.La * material.Ka;

	// Diffúz komponens 
	// A diffúz fényforrásból érkező fény mennyisége arányos a fényforrásba mutató vektor és a normálvektor skaláris szorzatával
	// és az attenuációval
	float DiffuseFactor = max(dot(ToLight,normal), 0.0) * Attenuation;
	vec3 Diffuse = DiffuseFactor * light.Ld * material.Kd;
	
	// Spekuláris komponens 
	vec3 viewDir = normalize( cameraPosition - position ); // A fragmentből a kamerába mutató vektor 
	vec3 reflectDir = reflect( -ToLight, normal ); // Tökéletes visszaverődés vektora 
	
	// A spekuláris komponens a tökéletes visszaverődés iránya és a kamera irányától függ.
	// A koncentráltsága cos()^s alakban számoljuk, ahol s a fényességet meghatározó paraméter.
	// Szintén függ az attenuációtól.
	float SpecularFactor = pow(max( dot( viewDir, reflectDir) ,0.0), material.Shininess) * Attenuation;
	vec3 Specular = SpecularFactor * light.Ls * material.Ks;

	return Ambient + Diffuse + Specular;
}


void main()
{
	vec3 normal = normalize( worldNormal );

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

	if(state == SHADER_STATE_ROBOT){

		float FragShininess = texture(textureShining, textureCoords).r;
		if(FragShininess > 1e-6){
			material.Shininess = 16 * FragShininess;
		}
		else{
			material.Ks = vec3(0.0f);
		}
	}

	vec3 shadedColor = lighting(light, worldPosition, normal, material);

	if(state == SHADER_STATE_ROBOT || state == SHADER_STATE_TABLE){
		LightProperties light2 = light;
		light2.pos = lightPosition2;
		light2.La = La2;
		light2.Ld = Ld2;
		light2.Ls = Ls2;
		shadedColor += lighting(light2,worldPosition,normal,material);
	}

	vec4 texColor = texture(textureImage, textureCoords);

	if(state == SHADER_STATE_TABLE){
		uint val = uint(floor(8.0*textureCoords.x) + floor(8.0*textureCoords.y));

		if(val %2 == 1){
			texColor *= vec4(0.8f, 0.8,0.8,1.0);
		}

	}

	outputColor =	vec4(shadedColor, 1) * texColor;
}
