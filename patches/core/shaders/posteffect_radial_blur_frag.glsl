////////////////////////////////////////////////////////
// PostEffect Radial Blur - Fragment Shader
//
// Do a blur based on the dist from center
////////////////////////////////////////////////////////
#version 120

#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect diffuseMap;
@define sampler_diffuseMap 0

uniform float afSize;
uniform float afBlurStartDist;
uniform vec2 avHalfScreenSize;


@ifdef FeatureNotSupported_ConstArray
	float vColorMul[5];
	float vSizeMul[5];
@else
	float vColorMul[5] = float[5]   ( 0.1,  0.2, 0.5, 0.2, 0.1);
	float vSizeMul[5] = float[5]   ( -1.0,  -0.5, 0.0, 0.5, 1.0);
@endif
const float fTotalMul = 1.1f;


void main()
{
	@ifdef FeatureNotSupported_ConstArray
		vColorMul[0] = 0.1; vColorMul[1] = 0.2; vColorMul[2] = 0.5; vColorMul[3] = 0.2; vColorMul[4] = 0.1;
		vSizeMul[0] = -1.0; vSizeMul[1] = -0.5; vSizeMul[2] = 0.0; vSizeMul[3] = 0.5; vSizeMul[4] = 1.0;
	@endif
	vec2 vScreenCoord = gl_FragCoord.xy;
	
	vec2 vDir = avHalfScreenSize - vScreenCoord;
	float fDist = length(vDir) / avHalfScreenSize.x;
	vDir = normalize(vDir);
	
	fDist = max(0.0, fDist-afBlurStartDist);
	
	vDir *= fDist * afSize;
			
	vec3 vDiffuseColor = vec3(0.0);
	
	for(int i=0; i<5; ++i)
	{
		vDiffuseColor += texture2DRect(diffuseMap, vScreenCoord+vDir*vSizeMul[i]).xyz * vColorMul[i];
	}
	
	vDiffuseColor /= fTotalMul;
	
	gl_FragColor.xyz = vDiffuseColor;
	gl_FragColor.w = 1.0;
}