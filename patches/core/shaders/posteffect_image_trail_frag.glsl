////////////////////////////////////////////////////////
// PostEffect Image Trail - Fragment Shader
//
// Just set up the texture for a dest blend
////////////////////////////////////////////////////////
#version 120

#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect diffuseMap;
@define sampler_diffuseMap 0

uniform float afAlpha;

void main()
{
	vec3 vDiffuseColor = texture2DRect(diffuseMap, gl_TexCoord[0].xy).xyz;
	
	gl_FragColor.xyz = vDiffuseColor;
	gl_FragColor.w = afAlpha;
}