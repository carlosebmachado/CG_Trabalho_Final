#version 330 core

layout (location = 0) in vec3 entrada_pos;
layout (location = 1) in vec2 entrada_pos_text;
layout (location = 2) in vec3 entrada_normal;

out vec2 pos_text;
out vec3 normal;
out vec3 posicao;
uniform float scale;
uniform float angulo_x;
uniform float angulo_y;
uniform float angulo_z;
uniform float pos_x;
uniform float pos_y;
uniform float pos_z;

void main()
{
    pos_text = entrada_pos_text;
    normal = entrada_normal;
    posicao = entrada_pos * scale;
    

	// escala
    gl_Position = vec4(entrada_pos * scale, 1.0);


    // rotação
    float temp_x = (cos(angulo_z) * cos(angulo_y)) * gl_Position.x + ((cos(angulo_z) * sin(angulo_y) * sin(angulo_x)) - (sin(angulo_z) * cos(angulo_x))) * gl_Position.y + ((cos(angulo_z) * sin(angulo_y) * cos(angulo_x)) + (sin(angulo_z) * sin(angulo_x))) * gl_Position.z;
    float temp_y = (sin(angulo_z) * cos(angulo_y)) * gl_Position.x + ((sin(angulo_z) * sin(angulo_y) * sin(angulo_x)) + (cos(angulo_z) * cos(angulo_x))) * gl_Position.y + ((sin(angulo_z) * sin(angulo_y) * cos(angulo_x)) - (cos(angulo_z) * sin(angulo_x))) * gl_Position.z;

    gl_Position.z = (-sin(angulo_y)) * gl_Position.x + (cos(angulo_y) * sin(angulo_x)) * gl_Position.y + (cos(angulo_y) * cos(angulo_x)) * gl_Position.z;
    gl_Position.y = temp_y;
    gl_Position.x = temp_x;
    

	// traslação
    gl_Position.x += pos_x;
    gl_Position.y += pos_y;
    gl_Position.z += pos_z;

}
