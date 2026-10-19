////////////////////////////////////////////////////////
// Deferred Depth unpack - Fragment Shader
//
// Unpacks the depth map of the gbuffer into the red channel.
////////////////////////////////////////////////////////
#version 120

#extension GL_ARB_texture_rectangle : enable

//---------------------------------------------------------

uniform float afFarPlane;

uniform sampler2DRect screenTexture;
@define sampler_screenTexture 0

uniform sampler2DRect depthTexture;
@define sampler_depthTexture 1

uniform sampler2DRect normalTexture;
@define sampler_normalTexture 2

void main()
{
	float fD = 1.0;
	vec2 vScreenPos = gl_FragCoord.xy;
		
	//////////////////
	// Check normal discontinuity
	vec3 vCoreNormal = texture2DRect(normalTexture, gl_FragCoord.xy).xyz*2.0-1.0;
	vec4 vNormalDotCore;
	vNormalDotCore.x = dot( vCoreNormal, texture2DRect(normalTexture, vScreenPos + vec2(0.0,fD) ).xyz*2.0-1.0 );
	vNormalDotCore.y = dot( vCoreNormal, texture2DRect(normalTexture, vScreenPos + vec2(0.0,-fD) ).xyz*2.0-1.0 );
	vNormalDotCore.z = dot( vCoreNormal, texture2DRect(normalTexture, vScreenPos + vec2(fD,0.0) ).xyz*2.0-1.0 );
	vNormalDotCore.w = dot( vCoreNormal, texture2DRect(normalTexture, vScreenPos + vec2(-fD,0.0) ).xyz*2.0-1.0 );
	vNormalDotCore = step(0.0, vNormalDotCore - vec4(0.8)); //Negate the limit for minimum dot product.
	
	float fNormalT = 1.0 - max( dot( vNormalDotCore, vec4(0.25) ), 0.0);
	
	//////////////////
	// Check depth discontinuity
	float fCoreDepth = texture2DRect(depthTexture, gl_FragCoord.xy).x;
	vec4 vDepthDiff;
	vDepthDiff.x  = texture2DRect(depthTexture, vScreenPos + vec2(0.0,fD) ).x + texture2DRect(depthTexture, vScreenPos + vec2(0.0,-fD)).x;
	vDepthDiff.y  = texture2DRect(depthTexture, vScreenPos + vec2(fD,0.0) ).x + texture2DRect(depthTexture, vScreenPos + vec2(-fD,0.0)).x;
	vDepthDiff.z  = texture2DRect(depthTexture, vScreenPos + vec2(fD,fD) ).x + texture2DRect(depthTexture, vScreenPos + vec2(-fD,-fD)).x;
	vDepthDiff.w  = texture2DRect(depthTexture, vScreenPos + vec2(fD,-fD) ).x + texture2DRect(depthTexture, vScreenPos + vec2(fD,-fD)).x;
	vDepthDiff = abs( vec4(2*fCoreDepth) - vDepthDiff )*afFarPlane - vec4(0.5); //negate limit value for minum depth.
	vDepthDiff = step(0.0, vDepthDiff);
	
	float fDepthT =  max(dot( vDepthDiff, vec4(0.25) ), 0.0);
	
	//////////////////
	// Smooth at screen pos
	float fT = fDepthT*0.5 + fNormalT*0.5;
	
	vec3 vColor = texture2DRect(screenTexture, vScreenPos).xyz * (1.0 - fT);
	vColor += texture2DRect(screenTexture, vScreenPos + vec2(0.0,fD) ).xyz * fT;
	vColor += texture2DRect(screenTexture, vScreenPos + vec2(0.0,-fD) ).xyz * fT;
	vColor += texture2DRect(screenTexture, vScreenPos + vec2(fD,0.0) ).xyz * fT;
	vColor += texture2DRect(screenTexture, vScreenPos + vec2(-fD,0.0) ).xyz * fT;
	
	vColor *= 1.0 / ((1.0 - fT) + fT*4.0);	
	
	//////////////////
	// Get new screen color
	gl_FragColor.xyz = vColor; //vec3(fT);
	gl_FragColor.w = 1.0;
}

