#ifndef HPL_RENDERER_DEFERRED_H
#define HPL_RENDERER_DEFERRED_H

#include "graphics/Renderer.h"

namespace hpl
{

//---------------------------------------------

//////////////////////////////////////////////////////////////////////////////
// GENERAL RENDERER INFO - DEFERRRED RENDERER
//
// Important to note is that this renderer does not require depth or stencil buffer
// for the render target, but always use internal stuff for rendering.
//
//
//////////////////////////////////////////////////////////////////////////////

//---------------------------------------------

class iFrameBuffer;
class iDepthStencilBuffer;
class iTexture;
class iLight;
class cSubMeshEntity;

//---------------------------------------------

enum eDeferredLightList
{
    eDeferredLightList_StencilBack_ScreenQuad,        //First draw back to stencil, then draw light as full screen quad
    eDeferredLightList_StencilFront_RenderBack,        //First draw front to stencil, then draw back facing as light.
    eDeferredLightList_RenderBack,                    //Draw back facing as light.
    eDeferredLightList_Batches,                        //Draw many lights as batch. Spotlights not allowed!

    eDeferredLightList_Box_StencilFront_RenderBack,
    eDeferredLightList_Box_RenderBack,

    eDeferredLightList_LastEnum
};

//---------------------------------------------

enum eDeferredShapeQuality
{
    eDeferredShapeQuality_Low,
    eDeferredShapeQuality_Medium,
    eDeferredShapeQuality_High,
    eDeferredShapeQuality_LastEnum
};

//---------------------------------------------

enum eDeferredGBuffer
{
    eDeferredGBuffer_32Bit,
    eDeferredGBuffer_64Bit,
    eDeferredGBuffer_LastEnum
};

//---------------------------------------------

enum eDeferredSSAO
{
    eDeferredSSAO_InBoxLight,
    eDeferredSSAO_OnColorBuffer,
    eDeferredSSAO_LastEnum
};

//---------------------------------------------

enum eGBufferComponents
{
    eGBufferComponents_Full,
    eGBufferComponents_ColorAndDepth,
    eGBufferComponents_Color,
    eGBufferComponents_Depth,
    eGBufferComponents_Normals,
    eGBufferComponents_LinearDepth,
    eGBufferComponents_LastEnum
};

//---------------------------------------------

class cDeferredLight
{
public:
    cDeferredLight() : mpShadowTexture(NULL), mbCastShadows(false) {}

    iLight *mpLight;
    cRect2l mClipRect;
    int mlArea;
    cMatrixf m_mtxViewSpaceRender;
    cMatrixf m_mtxViewSpaceTransform;
    bool mbInsideNearPlane;
    iOcclusionQuery *mpQuery;

    iTexture *mpShadowTexture;
    bool mbCastShadows;
    eShadowMapResolution mShadowResolution;
    int mlShadowCasterNum;
};

//---------------------------------------------

class cRendererDeferred : public  iRenderer
{
public:
    cRendererDeferred(cGraphics *apGraphics, cResources* apResources);
    ~cRendererDeferred();

    bool LoadData();
    void DestroyData();

    iTexture *GetPostEffectTexture();

    iTexture *GetGbufferTexture(int alIdx);
    iFrameBuffer *GetGBufferFrameBuffer(eGBufferComponents aComponents);
    //iTexture *GetShadowTexture(eShadowMapResolution aQuality){ return mpShadowTexture[aQuality]; }

    iDepthStencilBuffer *GetDepthStencilBuffer()
    {
        return mpDepthStencil[0];
    }

    iFrameBuffer *GetAccumBuffer()
    {
        return mpAccumBuffer;
    }
    iFrameBuffer *GetAccumLightBuffer()
    {
        return mpAccumLightBuffer;
    }

    iTexture *GetRefractionTexture()
    {
        return mpRefractionTexture;
    }
    iTexture *GetReflectionTexture()
    {
        return mpReflectionTexture;
    }

    cDeferredLight *GetDeferredLight(int alID);
    size_t GetDeferredLightNum();
    float GetLightComplexity(cDeferredLight* apLightData);

    //Static properties. Must be set before renderer data load.
    static void SetGBufferType(eDeferredGBuffer aType)
    {
        mGBufferType = aType;
    }
    static eDeferredGBuffer GetGBufferType()
    {
        return mGBufferType;
    }

    static void SetNumOfGBufferTextures(int alNum)
    {
        mlNumOfGBufferTextures = alNum;
    }
    static int GetNumOfGBufferTextures()
    {
        return mlNumOfGBufferTextures;
    }

    static void SetDepthCullLights(bool abX)
    {
        mbDepthCullLights = abX;
    }
    static int GetDepthCullLights()
    {
        return mbDepthCullLights;
    }

    static void SetSSAOLoaded(bool abX)
    {
        mbSSAOLoaded = abX;
    }
    static void SetSSAONumOfSamples(int alX)
    {
        mlSSAONumOfSamples = alX;
    }
    static void SetSSAOBufferSizeDiv(int alX)
    {
        mlSSAOBufferSizeDiv = alX;
    }
    static void SetSSAOScatterLengthMul(float afX)
    {
        mfSSAOScatterLengthMul = afX;
    }
    static void SetSSAOScatterLengthMin(float afX)
    {
        mfSSAOScatterLengthMin = afX;
    }
    static void SetSSAOScatterLengthMax(float afX)
    {
        mfSSAOScatterLengthMax = afX;
    }
    static void SetSSAOType(eDeferredSSAO aType)
    {
        mSSAOType = aType;
    }
    static void SetSSAODepthDiffMul(float afX)
    {
        mfSSAODepthDiffMul = afX;
    }
    static void SetSSAOSkipEdgeLimit(float afX)
    {
        mfSSAOSkipEdgeLimit = afX;
    }

    static bool GetSSAOLoaded()
    {
        return mbSSAOLoaded;
    }
    static int GetSSAONumOfSamples()
    {
        return mlSSAONumOfSamples;
    }
    static int GetSSAOBufferSizeDiv()
    {
        return mlSSAOBufferSizeDiv;
    }
    static float GetSSAOScatterLengthMul()
    {
        return mfSSAOScatterLengthMul;
    }
    static float GetSSAOScatterLengthMin()
    {
        return mfSSAOScatterLengthMin;
    }
    static float GetSSAOScatterLengthMax()
    {
        return mfSSAOScatterLengthMax;
    }
    static eDeferredSSAO GetSSAOType()
    {
        return mSSAOType;
    }
    static float GetSSAODepthDiffMul()
    {
        return mfSSAODepthDiffMul;
    }
    static float GetSSAOSkipEdgeLimit()
    {
        return mfSSAOSkipEdgeLimit;
    }

    static void SetDebugRenderLightComplexity(bool abX)
    {
        mbDebugRenderLightComplexity = abX;
    }
    static bool GetDebugRenderLightComplexity()
    {
        return mbDebugRenderLightComplexity;
    }

    static void SetDebugRenderOverdraw(bool abX)
    {
        mbDebugRenderOverdraw = abX;
    }
    static bool GetDebugRenderOverdraw()
    {
        return mbDebugRenderOverdraw;
    }

    static void SetDebugPrevFrameOcclusion(bool abX)
    {
        mbDebugPrevFrameOcclusion = abX;
    }
    static bool GetDebugPrevFrameOcclusion()
    {
        return mbDebugPrevFrameOcclusion;
    }

    static void SetOcclusionTestLargeLights(bool abX)
    {
        mbOcclusionTestLargeLights = abX;
    }
    static bool GetOcclusionTestLargeLights()
    {
        return mbOcclusionTestLargeLights;
    }

    static void SetDebugRenderFrameBuffers(bool abX)
    {
        mbDebugRenderFrameBuffers = abX;
    }
    static bool GetDebugRenderFrameBuffers()
    {
        return mbDebugRenderFrameBuffers;
    }

    static void SetDebugRenderLightBuffer(bool abX)
    {
        mbDebugRenderLightBuffer = abX;
    }
    static bool GetDebugRenderLightBuffer()
    {
        return mbDebugRenderLightBuffer;
    }

    static void SetModulateFog(bool abX)
    {
        mbModulateFog = abX;
    }
    static bool GetModulateFog()
    {
        return mbModulateFog;
    }

    static void EnableFog(bool abX)
    {
        mbFogEnabled = abX;
    }

private:
    void CopyToFrameBuffer();
    void SetupRenderList();
    void RenderObjects();

    void SetupGBuffer();

    void SetupRenderVariables();

    void RenderZ();
    void RenderZDissolve();
    void RenderDynamicZTemp();
    void RenderGbuffer();
    void RenderSSAO();
    void RenderDeferredSkyBox();

    void SetupLightsAndRenderQueries();
    void InitLightRendering();
    void RenderLights();
    void RenderLights_StencilBack_ScreenQuad();
    void RenderLights_StencilFront_RenderBack();
    void RenderLights_RenderBack();
    void RenderLights_Batches();
    void RenderLights_Box_StencilFront_RenderBack();
    void RenderLights_Box_RenderBack();

    void RenderIllumination();

    void RenderReflection(iRenderable *apObject);
    void RenderSubMeshEntityReflection(cSubMeshEntity *pReflectionObject);

    void RenderDecals();
    void RenderFullScreenFog();
    void RenderFog();
    void RenderTranslucent();

    void SetAccumulationBuffer();
    void SetAccumulationLightBuffer();
    void SetGBuffer(eGBufferComponents aComponents);
    iTexture *GetBufferTexture(int alIdx);


    void RenderGbufferContent();
    void RenderLightBufferContent();
    void RenderReflectionContent();
    void RenderLightComplexity();
    void RenderOverdraw();

    ////////////////
    //Draw helpers
    void RenderBoxLight(cDeferredLight* apLightData);

    ////////////////
    //Misc Helpers
    void RenderLightShadowMap(cDeferredLight* apLightData);
    void SetupLightProgramVariables(iGpuProgram *apProgram, cDeferredLight* apLightData);
    iGpuProgram *SetupProgramAndTextures(cDeferredLight* apLightData, tFlag alExtraFlags);
    iVertexBuffer *GetLightShape(iLight *apLight, eDeferredShapeQuality aQuality);


    iVertexBuffer *mpShapeSphere[eDeferredShapeQuality_LastEnum];
    iVertexBuffer *mpShapePyramid;

    iVertexBuffer *mpBatchBuffer;
    int mlMaxBatchLights;
    int mlMaxBatchVertices;
    int mlMaxBatchIndices;

    iVertexBuffer *mpFullscreenLightQuad;
    float mfLastFrustumFOV;
    float mfLastFrustumFarPlane;

    float mfFarPlane;
    float mfFarBottom;
    float mfFarTop;
    float mfFarLeft;
    float mfFarRight;

    cMatrixf m_mtxInvView;

    float mfMinLargeLightNormalizedArea;
    int mlMinLargeLightArea;

    float mfMinRenderReflectionNormilzedLength;

    float mfShadowDistanceHigh;
    float mfShadowDistanceMedium;
    float mfShadowDistanceLow;
    float mfShadowDistanceNone;

    bool mbStencilNeedClearing;
    cRect2l mStencilDirtyRect;

    iFrameBuffer *mpGBuffer[2][eGBufferComponents_LastEnum]; //[2] = reflection or not

    iFrameBuffer *mpAccumBuffer;
    iFrameBuffer *mpAccumLightBuffer;
    iFrameBuffer *mpReflectionBuffer;

    iTexture *mpGBufferTexture[2][4];    //[2] = reflection or not
    iTexture *mpAccumBufferTexture;
    iTexture *mpLightBufferTexture;
    iTexture *mpRefractionTexture;
    iTexture *mpReflectionTexture;
    iDepthStencilBuffer *mpDepthStencil[2];    //[2] = reflection or not


    bool mbReflectionTextureCleared;

    /*iTexture *mpShadowTempDiffTexture[eShadowMapResolution_LastEnum];
    iTexture *mpShadowTexture[eShadowMapResolution_LastEnum];
    iFrameBuffer *mpShadowBuffer[eShadowMapResolution_LastEnum];
    cShadowMapLightCache mShadowMapCacheData[eShadowMapResolution_LastEnum];*/
    iTexture *mpShadowJitterTexture;
    int mlShadowJitterSize;
    int mlShadowJitterSamples;

    iTexture *mpLinearDepthTexture;
    iTexture *mpSSAOTexture;
    iTexture *mpSSAOBlurTexture;
    iTexture *mpSSAOScatterDisk;

    iFrameBuffer *mpLinearDepthBuffer;
    iFrameBuffer *mpSSAOBuffer;
    iFrameBuffer *mpSSAOBlurBuffer;

    iGpuProgram *mpUnpackDepthProgram;
    iGpuProgram *mpSSAOBlurProgram[2];
    iGpuProgram *mpSSAORenderProgram;

    iGpuProgram *mpOverdrawProgram;
    iGpuProgram *mpHeatMapProgram;

    std::vector<cDeferredLight *> mvTempDeferredLights;
    std::vector<cDeferredLight *> mvSortedLights[eDeferredLightList_LastEnum];

    iGpuProgram *mpSkyBoxProgram;
    iGpuProgram *mpLightStencilProgram;
    iGpuProgram *mpLightBoxProgram[3];//0=no SSAO, 1=SSAO used, 2=light debug

    cProgramComboManager *mpFogProgramManager;

    cMatrixf m_mtxTempLight;

    //Static setting variables
    static eDeferredGBuffer mGBufferType;
    static int mlNumOfGBufferTextures;
    static bool mbDepthCullLights;

    static bool mbSSAOLoaded;
    static int mlSSAONumOfSamples;
    static float mfSSAOScatterLengthMul;
    static float mfSSAOScatterLengthMin;
    static float mfSSAOScatterLengthMax;
    static float mfSSAODepthDiffMul;
    static float mfSSAOSkipEdgeLimit;
    static eDeferredSSAO mSSAOType;
    static int mlSSAOBufferSizeDiv;

    static bool mbEnableParallax;

    static bool mbDebugRenderFrameBuffers;
    static bool mbDebugRenderLightBuffer;

    static bool mbModulateFog;
    static bool mbFogEnabled;

    static bool mbOcclusionTestLargeLights;
    static bool mbDebugPrevFrameOcclusion;
    static bool mbDebugRenderLightComplexity;
    static bool mbDebugRenderOverdraw;

};

//---------------------------------------------

};
#endif // HPL_RENDERER_DEFERRED_H
