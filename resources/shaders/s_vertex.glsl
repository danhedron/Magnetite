#version 130 
uniform sampler2D skyDiffuse;

attribute vec3 in_vertex;

varying vec3 v_pos;
varying float time;

void main(void)
{	
	time = 0.5;
	v_pos = in_vertex;
	gl_Position = gl_ModelViewProjectionMatrix * vec4(in_vertex * 250,1);
}
