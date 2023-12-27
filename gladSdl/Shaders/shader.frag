#version 330


in vec4 vCol;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

in vec3 worldPos0;

out vec4 colour;
out vec4 FragColor;
const int MAX_POINT_LIGHTS = 3;
const int MAX_SPOT_LIGHTS = 3;

struct Light
{
	vec3 colour;
	float ambientIntensity;
	//vec3 direction;
	float diffuseIntensity;
};


struct DirectionalLight
{
	Light base;
	vec3 direction;
};

struct PointLight
{
	Light base;
	vec3 position;
	float constant;
	float linear;
	float exponent;	
};

struct SpotLight
{	
	PointLight base;
	vec3 direction;
	float edge;
};

 
struct Material
{
	float specularIntensity;
	float shininess;	
};

uniform int pointLightCount;
uniform int spotLightCount;

uniform DirectionalLight directionalLight; //henamana sarawa to look more clear
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

uniform sampler2D theTexture;
uniform Material sht;
uniform vec3 eyePosition;
//Fog
uniform vec3 gCameraWorldPos;
uniform vec4 gColorMod = vec4(1);
uniform vec4 gColorAdd = vec4(0);

uniform float gFogStart = -1.0;
uniform float gFogEnd = -1.0;
uniform vec3 gFogColor = vec3(1.0,1.0,1.0);
uniform float gExpFogDensity = 1.0;
uniform PointLight gPointLights[MAX_POINT_LIGHTS];
/// create new function
//Note directional light always calculated by direction 'PintLight is partially calculated by direction
vec4 CalcLightByDirection(Light light, vec3 direction)  //am functiona har void mainakay xwarawaya copy pasteman krdwa bas directionalLightman goriwa ba light & light.direction ba direction
{

  vec4 ambientColour = vec4(light.colour,1.0f) * light.ambientIntensity;
    float diffuseFactor = max(dot(normalize(Normal), normalize(direction)),0.0f);
	// A.B = |A||B| cos(angle)
        //A.B = 1*1* cos(angle)   
        //A.B == cos(angle)
	vec4 diffuseColour = vec4(light.colour, 1.0f) * light.diffuseIntensity * diffuseFactor;
	 //////////////////////////////////////////////////////////////////////
	vec4 specularColour = vec4(0,0,0,0);
	if(diffuseFactor > 0.0f)
	{
		vec3 fragToEye = normalize(eyePosition - FragPos);
		vec3 reflectedVertex = normalize(reflect(direction,normalize(Normal)));
		float specularFactor = dot(fragToEye,reflectedVertex);
	if(specularFactor > 0.0f)
	{
		specularFactor = pow(specularFactor, sht.shininess);
		specularColour = vec4(light.colour * sht.specularIntensity * specularFactor, 1.0f);
	}
//vec4 ambientColour = vec4(light.colour, 1.0f) * Light.ambientIntensity;
	}
    return (ambientColour + diffuseColour + specularColour);
	

}

vec4 CalcDirectionalLight()
{
	return CalcLightByDirection(directionalLight.base,directionalLight.direction);
}

vec4 CalcPointLight(PointLight pLight)
{
	vec3 direction = FragPos - pLight.position;
		float distance = length(direction);
		direction = normalize(direction);
		
		vec4 colour = CalcLightByDirection(pLight.base,direction);
		//how far away so how 'runaky shawq' calculated
		float attenuation = pLight.exponent * distance * distance +
							pLight.linear * distance +
							pLight.constant;
							
	 return (colour/attenuation);
	}
	

//CALCSPOTLIGHT SINGULAR NO SINGULAR
vec4 CalcSpotLight(SpotLight sLight)
{
	vec3 rayDirection = normalize(FragPos - sLight.base.position);
	float slFactor = dot(rayDirection,sLight.direction);
	
	if(slFactor > sLight.edge) 
	{
		vec4 colour = CalcPointLight(sLight.base);
		return colour * (1.0f - (1.0f - slFactor)*(1.0f/(1.0f - sLight.edge))); //TORCH in the game will be nicer more realistic of the edge area
	} 
	else 
	{
	return vec4(0,0,0,0); 
    }
}
 
vec4 CalcPointLights()

{
	vec4 totalColour = vec4(0,0,0,0);
	for(int i = 0; i < pointLightCount; i++)
	{
		
	 totalColour += CalcPointLight(pointLights[i]);
	}
	return totalColour;
}

//CALCSPOTLIGHTS PLURAL

vec4 CalcSpotLights()
{
	vec4 totalColour = vec4(0,0,0,0);
	for(int i = 0; i < spotLightCount; i++)
	{
		
	 totalColour += CalcSpotLight(spotLights[i]);
	}
	return totalColour;
}



float CalcExpFogFactor()
{
float CameraToPixelDist = length(worldPos0 - eyePosition);
float DistRatio = 0.05 * CameraToPixelDist/gFogEnd;
//float FogFactor = exp(-DistRatio * gExpFogDensity);
//using technique by multiply it by itself better fog

float FogFactor = exp(-DistRatio * gExpFogDensity * DistRatio * gExpFogDensity);
FogFactor = clamp(FogFactor,0.88,1.0);  //added this like // FogFactor,0.8,0.9) 0.8 wata chan tamaka ladwrawa chrw xast be 0.9ka la nzikawa xoy chan be 
return FogFactor;
}



float CalcLinearFogFactor()
{
float CameraToPixelDist = length(worldPos0 - eyePosition);
float fogRange =gFogEnd - gFogStart;
float fogDist =gFogEnd - CameraToPixelDist;
float FogFactor = fogDist/fogRange;
FogFactor = clamp(FogFactor,0.0,1.0);
return FogFactor;

}
float  CalcFogFactor() 
{
float FogFactor;
//FogFactor = CalcLinearFogFactor();   //<<<<<<<<<<<<<<<<<<<<
FogFactor =CalcExpFogFactor();     //<<<<<<<<<<<<<<<<<<<<
//if(gFogStart>=0.0)
//{
//FogFactor = CalcExpFogFactor();

//}


return FogFactor;

}
//////////////////
vec4 CalcPhongLighting()
{

    if (gFogColor != vec3(0)) {
        float FogFactor = CalcFogFactor();

        colour = mix(vec4(gFogColor, 1.0), colour, FogFactor);
    }
    vec4 totalColour =  colour * gColorMod + gColorAdd;

    return totalColour;
}
/////////////////

void main()
{
//vec3 Normal = normalize(Normal);

FragColor = vec4(1.0, 0.0, 0.0, 0.0);
 vec4 finalColour = CalcDirectionalLight();
   //finalColour += CalcPointLights();
   //finalColour += CalcSpotLights();
	
    Light In;
	In.ambientIntensity;
	In.diffuseIntensity;
	In.colour;
	// CalcDirectionalLight(In);

	 colour = texture(theTexture,TexCoord.xy) * finalColour;
	
	float fogFactor = CalcFogFactor();
	colour = mix(vec4(gFogColor,1.0),colour,CalcFogFactor());
	
	 colour =  colour * gColorMod + gColorAdd; //to enhance like Ogldev said
	//FragColor = colour * CalcPhongLighting();
		//FragColor = exp(-colour /finalColour.y / finalColour.x);  COOL EFFECTS 
		FragColor = CalcPhongLighting() ; 

		
}
