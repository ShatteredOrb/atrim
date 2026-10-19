////////////////////////////////////////////////////////
// PostEffect Bloom Blur - Fragment Shader
//
// Blur effect for the bloom post effect
////////////////////////////////////////////////////////
#version 120

#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect diffuseMap;
@define sampler_diffuseMap 0

uniform sampler1D convMap;
@define sampler_convMap 1

@ifdef UseFadeAlpha
	uniform float afFadeAlpha;
@endif

void main()
{
	vec3 vDiffuseColor = texture2DRect(diffuseMap, gl_TexCoord[0].xy).xyz;
	
	vec3 vOutput =  vec3	(texture1D(convMap, vDiffuseColor.x).x,
			  	 texture1D(convMap, vDiffuseColor.y).y,
				 texture1D(convMap, vDiffuseColor.z).z);
	
	@ifdef UseFadeAlpha
		gl_FragColor.xyz = vOutput*afFadeAlpha + vDiffuseColor*(1-afFadeAlpha);
	@else
		gl_FragColor.xyz = vOutput;	
	@endif	
}