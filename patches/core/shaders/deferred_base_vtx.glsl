////////////////////////////////////////////////////////
// Deferred Base - Vertex Shader
//
//
////////////////////////////////////////////////////////
#version 120

//---------------------------------------------

@ifdef UseNormals
	varying vec3 gvNormal;
@endif
@ifdef UseNormalMapping
	varying vec3 gvTangent;
	varying vec3 gvBinormal;
@endif

@ifdef UseColor
	varying vec4 gvColor;
@endif

//32 bit G-Buffer
@ifdef Deferred_32bit
	varying float gfLinearDepth;
	
	uniform float afInvFarPlane;
//64 bit G-Buffer
@endif
	
@ifdef Deferred_64bit || UseVertexPosition || UseEnvMap || UseFog || UseRefraction
	varying vec3 gvVertexPos;	
@endif

@ifdef DeferredLight
	
	//Only need to cacluate position for 32 bit version
	@ifdef Deferred_32bit
		uniform float afNegFarPlane;
		varying vec3 gvFarPlanePos;
	@endif
	
	@ifdef UseBatching
		varying vec3 gvLightPosition;		
		varying vec4 gvLightColor;
		varying float gfLightRadius;
	@endif
@endif

@ifdef UseParallax
	varying vec3 gvTangentEyePos;
@endif

@ifdef UseUvAnimation
	uniform mat4 a_mtxUV;
@endif

@ifdef UseBoxFogNormalizedRayDir
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
	
	
	//////////////////////
	// Color
	@ifdef UseColor
		gvColor = gl_Color;
	@endif
	
	//////////////////////
	// Uv
	@ifdef UseUv
		@ifdef UseUvAnimation
			gl_TexCoord[0] = a_mtxUV * gl_MultiTexCoord0;
		@else
			gl_TexCoord[0] = gl_MultiTexCoord0;
		@endif
		@ifdef UseUvCoord1
			gl_TexCoord[1] = gl_MultiTexCoord1;
		@endif
	@endif

	//////////////////////
	// Normals
	@ifdef UseNormals
		gvNormal = normalize(gl_NormalMatrix * gl_Normal);
	@endif

	//////////////////////
	// Normalmapping
	@ifdef UseNormalMapping
		//To consider: Is gl_NormalMatrix correct here?
		gvTangent = normalize(gl_NormalMatrix * gl_MultiTexCoord1.xyz);
		
		//Need to do it in model space (and not view) because reflection normal mapping will fail otherwise!
		gvBinormal = normalize(gl_NormalMatrix * cross(gl_Normal,gl_MultiTexCoord1.xyz) * gl_MultiTexCoord1.w); 
	@endif
	
	//////////////////////
	// Parallax
	@ifdef UseParallax
		vec3 vViewEyeVec =  (gl_ModelViewMatrix * gl_Vertex).xyz;
		
		gvTangentEyePos.x = dot(vViewEyeVec, gvTangent);
		gvTangentEyePos.y = dot(vViewEyeVec, gvBinormal);
		gvTangentEyePos.z = dot(-vViewEyeVec, gvNormal);
		
		//Do not normalize yet! Do that in the fragment shader.		
	@endif

	//////////////////////
	// Deferring (G-Buffer)
	@ifdef Deferred_32bit
		gfLinearDepth = -(gl_ModelViewMatrix * gl_Vertex).z * afInvFarPlane; //Do not use near plane! Doing like this will make the calcs simpler in the light shader.
	@endif
	
	@ifdef Deferred_64bit || UseVertexPosition || UseEnvMap || UseFog || UseRefraction
		gvVertexPos = (gl_ModelViewMatrix * gl_Vertex).xyz;
	@endif
	
	//////////////////////
	// Deferring (Lights)
	@ifdef DeferredLight
	
		@ifdef Deferred_32bit
			////////////////////////////
			//Light are rendered as shapes
			@ifdef UseDeferredLightShapes
				//Project the position to the farplane
				vec3 vPos = (gl_ModelViewMatrix * gl_Vertex).xyz;
				
				//Spotlight will divided in fragment shader
				@ifdef DivideInFrag
					gvFarPlanePos = vPos;
					gvFarPlanePos.xy *= afNegFarPlane;
				//Point light does division now
				@else
					vec2 vTanXY = vPos.xy / vPos.z;
					
					gvFarPlanePos.xy = vTanXY * afNegFarPlane;
					gvFarPlanePos.z = afNegFarPlane;
				@endif
			////////////////////////////
			//Light are 2D quads
			@else
				//No need to project, postion is already at far plane.
				gvFarPlanePos = gl_Vertex.xyz;
			@endif
		@endif
		
		////////////////////////////
		//Batching is used
		@ifdef UseBatching
			gvLightPosition = gl_MultiTexCoord0.xyz;
			gvLightColor = 	gl_Color;
			gfLightRadius = gl_MultiTexCoord1.x;			
		@endif
	@endif
}