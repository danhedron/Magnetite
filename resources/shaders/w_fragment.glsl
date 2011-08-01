uniform sampler2D worldDiffuse;
varying vec4 vCoord;
varying vec4 vColour;

void main (void)  
{     
	gl_FragColor = texture2D( worldDiffuse, vCoord.xy ) * vColour;
} 