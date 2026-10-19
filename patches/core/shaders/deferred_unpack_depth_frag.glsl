////////////////////////////////////////////////////////
// Deferred Depth unpack - Fragment Shader
//
// Unpacks the depth map of the gbuffer into the red channel.
////////////////////////////////////////////////////////
#version 120

#extension GL_ARB_texture_rectangle : enable

@include helper_float_packing.glsl

//---------------------------------------------------------

@ifdef Deferred_64bit
	uniform float afNegInvFarPlane;
@endif	

uniform sampler2DRect depthTexture;
@define sampler_depthTexture 0

void main()
{
	//32 bit G-Buffer
	@ifdef Deferred_32bit
		float fDepth = UnpackVec3ToFloat(texture2DRect(depthTexture, gl_TexCoord[0].xy).xyz);
	//64 bit G-Buffer
	@elseif Deferred_64bit
		float fDepth = texture2DRect(depthTexture, gl_TexCoord[0].xy).z * afNegInvFarPlane;
	@endif
	
	
	gl_FragColor.x = fDepth;
}

