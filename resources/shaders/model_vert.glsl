#version 130

uniform mat4 matrix_view;
uniform mat4 matrix_projection;
uniform mat4 matrix_worldspace;

attribute vec3 vertex_position;

void main()
{
	mat4 mvp = (matrix_projection * matrix_view * matrix_worldspace);
	gl_Position = mvp * vec4( vertex_position, 1.0 );
}
