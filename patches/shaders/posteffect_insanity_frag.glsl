////////////////////////////////////////////////////////
// PostEffect Insanity - Fragment Shader
//
// Effect when player goes insane
////////////////////////////////////////////////////////

#version 120
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect diffuseMap;
@define sampler_diffuseMap 0

uniform sampler2D ampMap0;
@define sampler_ampMap0 1

uniform sampler2D ampMap1;
@define sampler_ampMap1 2

uniform sampler2D zoomMap;
@define sampler_zoomMap 3


uniform float afAlpha;
uniform float afT;
uniform vec2 avScreenSize;
uniform float afAmpT;
uniform float afWaveAlpha;
uniform float afZoomAlpha;

void main()
{
	vec2 vTexUV = gl_TexCoord[0].xy/avScreenSize;
	vec3 vAmp = texture2D(ampMap0, vTexUV).xyz*(1.0-afAmpT) + texture2D(ampMap1, vTexUV).xyz*afAmpT;
	vAmp *= afWaveAlpha * 0.04 * avScreenSize.y;
	
	vec3 vZoom = texture2D(zoomMap, vTexUV).xyz;
	
	
	vec2 vUV = gl_FragCoord.xy;
	
	vUV += (vZoom.xy-vec2(0.5))*2.0* 0.6 * vZoom.z * avScreenSize.y * afZoomAlpha;
	
	vec2 vSinUv = (vUV / avScreenSize.y) * 0.6;
	vUV.x += sin(afT + vSinUv.y) * vAmp.x;
	vUV.y += sin(afT + vSinUv.x*1.83) * vAmp.y;
	
	vec3 vDiffuseColor = texture2DRect(diffuseMap, vUV).xyz;
	
	
	gl_FragColor.xyz = vDiffuseColor;
}