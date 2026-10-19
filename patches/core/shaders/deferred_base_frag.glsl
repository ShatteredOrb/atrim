////////////////////////////////////////////////////////
// Deferred Base - Fragment Shader
//
// A basic fragment with little fancy stuff.
////////////////////////////////////////////////////////
#version 120

#extension GL_ARB_texture_rectangle : enable
#extension GL_ARB_draw_buffers : enable


@ifdef UseColor
	varying vec4 gvColor;
@endif

@ifdef UseDiffuse || UseAlphaMap
	uniform sampler2D aDiffuseMap;
	@define sampler_aDiffuseMap 0
@endif


@ifdef UseDissolve || UseAlphaUseDissolveFilter
	uniform sampler2D aDissolveMap;
	@define sampler_aDissolveMap 1
@endif

@ifdef UseDissolve	
	uniform float afDissolveAmount;
@endif

@ifdef UseDissolveAlphaMap
	uniform sampler2D aDissolveAlphaMap;
	@define sampler_aDissolveAlphaMap 2
@endif

void main()
{
	vec4 vFinalColor;
	
	////////////////////
	//Diffuse 
	@ifdef UseDiffuse || UseAlphaMap
	 	vFinalColor = texture2D(aDiffuseMap, gl_TexCoord[0].xy);
	@else	
		vFinalColor = vec4(1.0);
	@endif
	
	////////////////////
	//Vertex colors
	@ifdef UseColor
		vFinalColor *= gvColor;
	@endif
	
	////////////////////
	//Dissolve
	@ifdef UseDissolve || UseAlphaUseDissolveFilter
		vec2 vDissolveCoords = gl_FragCoord.xy * (1.0/128.0);//128 = size of dissolve texture.
		float fDissolve = texture2D(aDissolveMap, vDissolveCoords).w;
		
		@ifdef UseDissolveAlphaMap
			//Get in 0.75 - 1 range
			fDissolve = fDissolve*0.25 + 0.75;
			
			float fDissolveAlpha = texture2D(aDissolveAlphaMap, gl_TexCoord[0].xy).w;
			fDissolve -= (0.25 - fDissolveAlpha*0.25);
		@else
			//Get in 0.5 - 1 range.
			fDissolve = fDissolve*0.5 + 0.5;
		@endif 
		
		@ifdef UseDissolve
			vFinalColor.w = fDissolve - (1.0-afDissolveAmount*vFinalColor.w)*0.5;
		@else
			vFinalColor.w = fDissolve - (1.0-vFinalColor.w)*0.5;
		@endif
	@endif 
		 
	gl_FragColor = vFinalColor;
}