////////////////////////////////////////////////////////
// PostEffect Bloom Blur - Fragment Shader
//
// Blur effect for the bloom post effect
////////////////////////////////////////////////////////

#version 120
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect diffuseMap;
@define sampler_diffuseMap 0

uniform float afBlurSize;

// Initialize these down in MAIN due to Mac OS X OpenGL Driver
@ifdef FeatureNotSupported_ConstArray
	float vMul[9];
	float fOffset[9];
@else
	const float vMul[5] = float[5]   ( 0.25,  0.3, 0.5, 0.3, 0.25);
	const float fOffset[5] = float[5]( -2.5, -0.75, 0.0, 0.75,  2.5);
@endif
const float fMulSum = 0.25+0.3+0.5+0.3+0.25;

void main()
{
	@ifdef FeatureNotSupported_ConstArray
		vMul[0] = 0.25; vMul[1] = 0.3; vMul[2] = 0.5; vMul[3] = 0.3; vMul[4] = 0.25;
		fOffset[0] = -2.5; fOffset[1] = -0.75; fOffset[2] = 0.0; fOffset[3] = 0.75; fOffset[4] = 2.5;
	@endif

	vec3 vAmount =vec3(0);
	
	@ifdef BlurHorisontal
		vec2 vOffsetMul = vec2(1.0, 0.0)*afBlurSize;
	@else
		vec2 vOffsetMul = vec2(0.0, 1.0)*afBlurSize;
	@endif
	
	for(int i=0; i<5; i+=1)
	{	
		vec2 vOffset = vec2(fOffset[i])*vOffsetMul;
		vec3 vColor = texture2DRect(diffuseMap, gl_TexCoord[0].xy + vOffset).xyz;
		vAmount += vColor * vMul[i];
	}
	
	vAmount /= fMulSum;
	
	gl_FragColor.xyz = vAmount;
	gl_FragColor.w = 1.0;
}