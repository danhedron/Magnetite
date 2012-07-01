#version 130 
uniform sampler2D skyDiffuse;

attribute vec3 in_vertex;

void main(void)
{	
	gl_Position = gl_ModelViewProjectionMatrix * vec4(in_vertex,1);
}
