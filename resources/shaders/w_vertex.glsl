#version 120 
uniform sampler2D worldDiffuse;

attribute vec4 in_p;

varying float f_light;
varying vec2 f_coords;

void main(void)
{
	f_light = in_p.z;
	f_coords = in_p.xy;
	
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
