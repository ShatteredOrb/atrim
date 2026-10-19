////////////////////////////////////////////////////////
// PostEffect Bloom Blur - Vertex Shader
//
// Blur effect for the bloom post effect
////////////////////////////////////////////////////////
#version 120

void main()
{	
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_TexCoord[0] = gl_MultiTexCoord0;
}