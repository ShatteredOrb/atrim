////////////////////////////////////////////////////////
// _TEST_ ARRAY SUPPORT FRAG
//
// A shader that test if the const arrays are supported.
////////////////////////////////////////////////////////

#version 120

const float vMul[5] = float[5]   ( 0.25,  0.3, 0.5, 0.3, 0.25);

void main()
{
	float fTotal =0;
	for(int i=0; i<5; i+=1) fTotal += vMul[i]*gl_TexCoord[0].x;	
	
	gl_FragColor = vec4(fTotal);
}