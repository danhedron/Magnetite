#version 150 core 
uniform sampler2D worldDiffuse;

in vec4 in_p;

smooth out float f_light;
smooth out vec2 f_coords;

void main(void)
{
	f_light = in_p.z;
	f_coords = in_p.xy;
	
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}