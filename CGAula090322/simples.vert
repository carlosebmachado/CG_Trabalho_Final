#version 330 core

layout (location = 0) in vec3 entrada_pos;
layout (location = 1) in vec2 entrada_pos_text;
layout (location = 2) in vec3 entrada_normal;

out vec2 pos_text;
out vec3 normal;
out vec3 posicao;

void main()
{
    pos_text = entrada_pos_text;
    normal = entrada_normal;
    posicao = entrada_pos * 2;
    gl_Position = vec4(entrada_pos * 2, 1.0);
} 