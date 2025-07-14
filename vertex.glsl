#version 330 core

in vec2 a_vertex;
in vec2 a_texture_coordinate;

out vec2 texture_coordinate;

void main()
{
	gl_Position = vec4(a_vertex, 1.0f, 1.0f);

	texture_coordinate = a_texture_coordinate;
}