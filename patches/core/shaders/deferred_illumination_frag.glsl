////////////////////////////////////////////////////////
// Deferred Illumination - Fragment Shader
//
// Used in a sepperate pass to render illuminating parts of a material.
////////////////////////////////////////////////////////
#version 120

uniform sampler2D aDiffuse;
@define sampler_aDiffuse 0

uniform float afColorMul;

void main()
{
	gl_FragColor = texture2D(aDiffuse, gl_TexCoord[0].xy) * afColorMul;
}