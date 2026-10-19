////////////////////////////////////////////////////////
// Deferred Fog - Vertex Shader
//
//
////////////////////////////////////////////////////////
#version 120

//---------------------------------------------

varying vec3 gvVertexPos;	

//---------------------------------------------

@ifdef OutsideBox && UseBackside
	varying vec3 gvLocalBoxRay;
	
	uniform mat4 a_mtxBoxInvViewModelRotation;
@endif

//---------------------------------------------

///////////////////////////////
// Main program
void main()
{	
	//////////////////////
	// Position
	gl_Position = ftransform();
	
	
	gvVertexPos = (gl_ModelViewMatrix * gl_Vertex).xyz;
	
	@ifdef OutsideBox && UseBackside
		gvLocalBoxRay = (a_mtxBoxInvViewModelRotation * vec4(gvVertexPos,1)).xyz;
	@endif
}