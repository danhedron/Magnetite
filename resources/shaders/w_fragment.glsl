#version 120 
uniform sampler2D worldDiffuse;

varying float f_light;
varying vec2 f_coords;

void main (void)  
{
	vec4 col = texture2D( worldDiffuse, f_coords * 0.25);
	col.rgb *= f_light / 255;
	gl_FragColor = col;
} 
