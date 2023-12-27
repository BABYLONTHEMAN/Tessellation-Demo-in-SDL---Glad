//#version 330 core
//
//layout (location = 0) in vec3 aPos; // Vertex position attribute
//
//uniform mat4 model;
//uniform mat4 projection;
//uniform mat4 view;
//
//void main() {
//    gl_Position = projection * view * model * vec4(aPos, 1.0);
//}

#version 410 core

layout (location = 0) in vec3 aPos;      // Vertex position attribute
layout (location = 1) in vec2 aTexCoord; // Texture coordinate attribute

out vec2 TexCoord; // Output texture coordinate for fragment shader

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

void main() {
    gl_Position =  projection * model * view * vec4(aPos, 1.0);
    TexCoord = aTexCoord; // Pass texture coordinates to the fragment shader
}
