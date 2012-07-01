#version 120 
uniform sampler2D worldDiffuse;

varying float f_light;
varying vec2 f_coords;

void main (void)  
{
	vec4 col = texture2D( worldDiffuse, f_coords * 0.25);
	vec3 rgb = col.rgb * (f_light / 255);
	if( col.a < 0.5 ) discard;
	gl_FragColor = vec4(rgb, col.a);
} 
