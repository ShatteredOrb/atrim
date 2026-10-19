////////////////////////////////////////////////////////
// Deferred G-Buffer Solid - Fragment Shader
//
//
////////////////////////////////////////////////////////
#version 120
#extension GL_ARB_draw_buffers : enable


////////////////////
//Textures
uniform samplerCube aDiffuseMap;
@define sampler_aDiffuseMap 0

//------------------------------------

///////////////////////////////
// Main program
void main()
{
	gl_FragData[0].xyz = textureCube(aDiffuseMap, gl_TexCoord[0].xyz).xyz;
	gl_FragData[1].xyz = vec3(1);
}