#version 130 
uniform sampler2D skyDiffuse;

varying vec3 v_pos;
varying float time;

void main (void)  
{
	vec4 atmCol = texture2D( skyDiffuse, vec2(time, -v_pos.y+0.5));
	
	gl_FragColor = atmCol;
} 
