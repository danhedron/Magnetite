#version 120 
uniform sampler2D worldDiffuse;

attribute vec3 in_params;
attribute vec3 in_vertex;

varying float f_light;
varying vec2 f_coords;

void main(void)
{
	f_light = in_params.z;
	f_coords = in_params.xy;
	
	gl_Position = gl_ModelViewProjectionMatrix * vec4(in_vertex,1.0);
}
