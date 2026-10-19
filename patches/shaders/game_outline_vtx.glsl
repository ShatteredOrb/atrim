#version 120

void main()
{	
	//TODO: Skeletal animation!
	
	vec4 vVertex = gl_Vertex;
	//vVertex.xyz += vVertex.xyz*0.1;
	
	gl_TexCoord[0] = gl_MultiTexCoord0;
		
	vec4 vPosition = gl_ModelViewProjectionMatrix * vVertex;
		
	//vec3 vNormal = normalize(gl_NormalMatrix * gl_Normal).xyz;
	//vPosition.xyz += 0;//vNormal*0.1;
	
	gl_Position =vPosition;
}