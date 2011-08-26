#version 150 core 
uniform sampler2D worldDiffuse;

smooth in float f_light;
smooth in vec2 f_coords;

void main (void)  
{
	vec4 col = texture2D( worldDiffuse, f_coords * 0.25 ) * f_light / 255;
	col.a = 1;
	gl_FragColor = col;
} 