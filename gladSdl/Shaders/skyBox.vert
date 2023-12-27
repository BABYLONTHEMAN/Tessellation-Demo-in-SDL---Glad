#version 410 core


//uniform mat4 viewMatrix;
//uniform mat4 projectionMatrix;

//layout (location = 0) in vec3 pos; 
//layout (location = 1) in vec2 tex;
//layout (location = 2) in vec3 norm;

//out vec4 vCol;
out vec3 TexCoord;
////// FOG 
out float Visibility;
/////
//out vec3 Normal;
//out vec3 FragPos;

//uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

/////
const float density =0.0007;
 const float gradient =0.05;


 
//<><><><><><><><><><><>>>>//////////////////////////////////////////
//*************************************************************************
uniform struct
{
	mat4 projectionMatrix;
	mat4 viewMatrix;
	mat4 modelMatrix;
	mat3 normalMatrix;
} matrices;

layout (location = 0) in vec3 pos; 
layout (location = 1) in vec2 tex;
layout (location = 2) in vec3 norm;

//*************************************************************************
//<><><><><><><><><><><>>>>//////////////////////////////////////////
void main()
{
	TexCoord = pos;
	vec4 posrelativetocam = view * vec4(pos, 1.0);
	float dury = length(posrelativetocam.xyz); //dury la camera 
    //gl_Position = projection * view * vec4(pos, 1.0);
	Visibility = exp(-pow((dury*density),gradient));
	Visibility =clamp (Visibility,0.0,0.1);
	gl_Position = projection * posrelativetocam * Visibility;
	//Visibility to stay between 0 and 1 using clamp
}