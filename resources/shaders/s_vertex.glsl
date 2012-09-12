#version 130 
uniform sampler2D skyDiffuse;

attribute vec3 vertex_position;

varying vec3 v_pos;
varying float time;

void main(void)
{	
	time = 0.5;
	v_pos = vertex_position;
	gl_Position = gl_ModelViewProjectionMatrix * vec4(vertex_position * 250,1);
}
