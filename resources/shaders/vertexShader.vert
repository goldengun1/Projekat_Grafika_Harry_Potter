#version 330 core

layout (location = 0) in vec3 aFragPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

void main(){
    gl_Position = Projection * View * Model * vec4(aFragPos,1.0f);
    FragPos = vec3(Model * vec4(aFragPos, 1.0));
    TexCoords = vec2(aTexCoords.x,aTexCoords.y);
    Normal = mat3(transpose(inverse(Model))) * aNormal;
}