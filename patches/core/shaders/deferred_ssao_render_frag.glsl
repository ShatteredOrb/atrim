#version 120

#extension GL_ARB_texture_rectangle : enable

uniform float afFarPlane;

uniform float afScatterLengthMul;
uniform vec2 avScatterLengthLimits;
uniform float afDepthDiffMul;
uniform float afSkipEdgeLimit;

uniform vec2 avScreenSize;

uniform sampler2DRect depthMap;
@define sampler_depthMap 0

uniform sampler2D scatterDisk;
@define sampler_scatterDisk 1

void main()
{
    float fCoreDepth = texture2DRect(depthMap, gl_TexCoord[0].xy).x;

    float fScatterLength = clamp(afScatterLengthMul / (fCoreDepth * afFarPlane), avScatterLengthLimits.x, avScatterLengthLimits.y) * avScreenSize.y;    
    float fScatterDiskZ =0.0;

    vec2 vScreenScatterCoord = (gl_FragCoord.xy) * 1.0 / 4.0;
    vScreenScatterCoord.y = fract(vScreenScatterCoord.y);
    vScreenScatterCoord.y *= 1.0 / $SampleNumDiv2;

    float fOccSum =0.0;
    float fFullGap = 1.0 / afDepthDiffMul;
    float fFadeEnd = max(afSkipEdgeLimit, fFullGap + 0.01);

    for(int i=0; i<$SampleNumDiv2 / 2; i++)
    {
        vec2 vScatterLookupCoord1 = vec2(vScreenScatterCoord.x, vScreenScatterCoord.y + fScatterDiskZ);

        vec4 vOffset1 = (texture2D(scatterDisk, vScatterLookupCoord1) *2.0 - 1.0)  * fScatterLength;

        vec4 vDepth = vec4(    texture2DRect(depthMap, gl_TexCoord[0].xy + vOffset1.xy).x,
                    texture2DRect(depthMap, gl_TexCoord[0].xy + vOffset1.zw).x,
                    texture2DRect(depthMap, gl_TexCoord[0].xy - vOffset1.xy).x,
                    texture2DRect(depthMap, gl_TexCoord[0].xy - vOffset1.zw).x);

        vec4 vGap = (fCoreDepth - vDepth) * afFarPlane;
        vec4 vPairGap = vGap + vGap.zwxy;
        vec4 vAmount = clamp(vPairGap * afDepthDiffMul, 0.0, 1.0);
        vec4 vRange = vec4(1.0) - smoothstep(fFullGap, fFadeEnd, vPairGap);

        vAmount *= vRange;

        vec4 vOcc = (vec4(1.0) - vAmount) * (vec4(1.0) - vAmount);
        fOccSum += dot(vOcc,vec4(1.0));

        fScatterDiskZ += (1.0 / $SampleNumDiv2) * 2.0;
    }

    float fOcc = fOccSum / (2.0 * $SampleNumDiv2);

    if(!(fOcc >= 0.0))
    {
       fOcc = 1.0;
    }
    gl_FragColor.xyz = vec3(max(fOcc * fOcc, 0.25));
}
