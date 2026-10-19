#version 120
uniform vec3 gvColor;


@ifdef UseAlpha
	uniform sampler2D alphaMap;
	@define sampler_alphaMap 0
@endif

void main()
{
	@ifdef UseAlpha
		gl_FragColor.xyz = gvColor * texture2D(alphaMap, gl_TexCoord[0].xy).w;	
	@else
		gl_FragColor.xyz = gvColor;
	@endif
	
	gl_FragColor.w = 1.0;
}