#version 330

layout (location = 0) in vec3 pos; //
layout (location = 1) in vec2 tex;
layout (location = 2) in vec3 norm;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 gWorld; //world Matrix 'here'

out vec4 vCol;
out vec2 TexCoord; 
out vec3 Normal;
out vec3 FragPos;
out vec3 worldPos0; // for this we need to provide gWorld 'look up' 


void main()
{
vec4 Pos4 =vec4(pos,1.0);
    	
	//vCol = vec4(clamp(pos,0.0f,1.0f), 1.0f);

	TexCoord = tex;
	            vCol = vec4(1.0f, 0.5f, 0.2f, 1.0f);

	Normal = mat3(transpose(inverse(model))) * norm; // for non uniform you dont need the "tanspose invert" part
	FragPos = (model * vec4(pos, 1.0)).xyz;
	worldPos0 = (gWorld * Pos4).xyz;
	gl_Position = projection * view * model * vec4(pos, 1.0);
/* EXAMPLE:
vec3 a = vec3(1,2,3);
a.x = 1
a.xy = 1,2
a.xyz = 1,2,3
a.xyy = 1,2,2
a.zxy = 3,1,2
*/

}