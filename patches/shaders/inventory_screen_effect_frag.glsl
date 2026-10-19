#version 120
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect diffuseMap;
@define sampler_diffuseMap 0

void main()
{
	vec3 vCol = texture2DRect(diffuseMap, gl_TexCoord[0].xy).xyz;
	
	vec3 vColToR = vec3(0.39,  0.769, 0.189);
	vec3 vColToG = vec3(0.349, 0.686, 0.168);
	vec3 vColToB = vec3(0.272, 0.534, 0.131);
	
	float fIntensity = 0.35;
	
	vec3 vFinalCol;
	vFinalCol.x = dot(vCol, vColToR) * fIntensity;
	vFinalCol.y = dot(vCol, vColToG) * fIntensity;
	vFinalCol.z = dot(vCol, vColToB) * fIntensity;
	
	gl_FragColor.xyz = pow(vFinalCol.xyz, vec3(1.5));
	
	gl_FragColor.w = 1.0;
}