#version 410 core


in vec2 TexCoord; // Input texture coordinate from vertex shader

out vec4 FragColor; // Output color

uniform sampler2D terrainTexture; // Texture sampler

void main() {
    FragColor = texture(terrainTexture, TexCoord);
}
