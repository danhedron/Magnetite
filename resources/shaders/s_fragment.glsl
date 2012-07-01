#version 130 
uniform sampler2D skyDiffuse;

varying vec2 f_coords;

void main (void)  
{
	vec4 atmCol = texture2D( skyDiffuse, vec2(0.5,0.5));
	
	gl_FragColor = atmCol;
} 
