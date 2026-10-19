// Pack a float in a 2d vector
vec2 PackFloatInVec2(float afX)
{
	vec2 vRet;
	afX *= 255.0;
	vRet.x = floor(afX);
	vRet.y = (afX - vRet.x);
	vRet.x *= (1.0 / 255.0);
	return vRet;	
}

//------------------------------------

// Pack a float in a 3d vector
vec3 PackFloatInVec3(float afX)
{
	vec3 vRet;
	afX *= 255.0;
	vRet.x = floor(afX);
	afX = (afX - vRet.x) * 255.0;
	vRet.y = floor(afX);
	vRet.z = afX - vRet.y;
	vRet.xy *= (1.0 / 255.0);
	
	return vRet;	
}

//------------------------------------

// Unpack a 2d vector to a float
float UnpackVec2ToFloat(vec2 avVal)
{
	return dot(avVal, vec2(1.0, 1.0/255.0) ); 	
} 

//------------------------------------

// Unpack a 3d vector to a float
float UnpackVec3ToFloat(vec3 avVal)
{
	return dot(avVal, vec3(1.0, 1.0/255.0, 1.0 / (255.0*255.0 )) ); 	
}
