#version 430

// pipeline-ból bejövő per-fragment attribútumok 
in vec4 color;
in vec2 ScreenPosition;

// kimenő érték - a fragment színe 
out vec4 outputColor;

// !!!!! VARÁZSLAT !!!!
// Erről bővebben később...
uniform float ElapsedTimeInSec = 0.0;
// !!!!!!!!!!!!

const float PI = 3.14159265359;
const float angle_degree = 30.0;
const vec4 angles_degree = vec4(30.0,150.0,240.0,360.0);

float myFunc( const float t ){
	return pow(abs(cos(2 * PI * t + ElapsedTimeInSec)),8) /*+ abs(sin( ElapsedTimeInSec))*/ ;
};

vec4 colorize( float between0and1 ){

vec4 color1 = vec4(0.0, 0.0, 0.0, 0.0);
vec4 color2 = vec4(1.0, 1.0, 0.5, 0.0);


return mix(color1, color2,between0and1);
};


void main()
{
	//outputColor = vec4(1.0 -color.rgb, 1.0);
	//outputColor = color;
	//outputColor = vec4(1.0);
	//outputColor = color.bgra;
	outputColor = vec4(ScreenPosition, 0.0, 1.0);

	const float Radius = 0.5;
	const float RadiusSqr = Radius * Radius;
	const float lineWidth = 0.025;
	//float lengthNormPos =  length(ScreenPosition);//sqrt((ScreenPosition.x * ScreenPosition.x ) + (ScreenPosition.y * ScreenPosition.y));


	//5.fel
	float radianAngle = radians(angle_degree);

	vec2 rotatedAngleVec = vec2(cos(radianAngle), sin(radianAngle));
	
	float projection = dot(rotatedAngleVec,ScreenPosition);

	float fVal = myFunc(projection);
	
	outputColor =vec4(vec3(fVal),1.0);

	//6.fel
	outputColor = colorize(fVal);

	float avgAngle = 0.0;
	//7.fel
	for(int i = 0; i< 4; i++){
		float radianAngle = radians(angles_degree[i]);

		vec2 rotatedAngleVec = vec2(cos(radianAngle), sin(radianAngle));

		float projection = dot(rotatedAngleVec,ScreenPosition);

		avgAngle += myFunc(projection);
	};
	outputColor = colorize(avgAngle/4.0);




	/*if(dot(ScreenPosition,ScreenPosition) > RadiusSqr + lineWidth ||
	dot(ScreenPosition,ScreenPosition) < RadiusSqr - lineWidth ) discard; //outputColor.rgb = vec3(1.0,0.0,1.0);*/
}
