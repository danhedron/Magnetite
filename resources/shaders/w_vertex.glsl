uniform sampler2D worldDiffuse;
varying vec4 vCoord;
varying vec4 vColour;

void main(void)
{
	vec4 a = gl_Vertex;
	vCoord = gl_MultiTexCoord0;
	vColour = gl_Color;
	gl_Position = gl_ModelViewProjectionMatrix * a;
}    