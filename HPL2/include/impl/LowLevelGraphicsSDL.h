#ifndef HPL_LOWLEVELGRAPHICS_SDL_H
#define HPL_LOWLEVELGRAPHICS_SDL_H

#include <GL/glew.h>
#ifdef _WIN32
#include <GL/wglew.h>
#endif

#include "SDL/SDL.h"

// Include these AFTER SDL
#ifdef __linux__
#include <GL/glx.h>
#endif

#include "graphics/LowLevelGraphics.h"
#include "math/MathTypes.h"

namespace hpl
{
#ifdef __linux__
typedef int (*GLXSWAPINTERVALPROC)(int interval);
#endif
//-------------------------------------------------

//////////////////////////////////////////
// Global Functions
//////////////////////////////////////////

GLenum GetGLDepthTestFuncEnum(eDepthTestFunc aType);
GLenum GetGLAlphaTestFuncEnum(eAlphaTestFunc aType);

GLenum GetGLStencilFuncEnum(eStencilFunc aType);
GLenum GetGLStencilOpEnum(eStencilOp aType);

GLenum GetGLTextureParamEnum(eTextureParam aType);
GLenum GetGLTextureOpEnum(eTextureOp aType);
GLenum GetGLTextureFuncEnum(eTextureFunc aType);
GLenum GetGLTextureSourceEnum(eTextureSource aType);

GLenum GetGLBlendEnum(eBlendFunc aType);

GLenum PixelFormatToGLFormat(ePixelFormat aFormat);
GLenum PixelFormatToGLInternalFormat(ePixelFormat aFormat);
GLenum GetGLCompressionFormatFromPixelFormat(ePixelFormat aFormat);

GLenum TextureTypeToGLTarget(eTextureType aType);

GLenum GetGLTextureTargetEnum(eTextureType aType);

GLenum GetGLTextureCompareMode(eTextureCompareMode aMode);
GLenum GetGLTextureCompareFunc(eTextureCompareFunc aFunc);

GLenum GetGLWrapEnum(eTextureWrap aMode);

//-------------------------------------------------

class cColorWriteDL
{
public:
    bool r;
    bool g;
    bool b;
    bool a;
};

//-------------------------------------------------

class cLowLevelGraphicsSDL : public iLowLevelGraphics
{
public:
    cLowLevelGraphicsSDL();
    ~cLowLevelGraphicsSDL();

    /////////////////////////////////////////////////////
    /////////////// GENERAL SETUP ///////////////////////
    /////////////////////////////////////////////////////

    bool Init(    int alWidth, int alHeight, int alBpp, bool abFullscreen,
                  eGpuProgramFormat aGpuProgramFormat, const tString& asWindowCaption,
                  const cVector2l &avWindowPos);

    eGpuProgramFormat GetGpuProgramFormat()
    {
        return mGpuProgramFormat;
    }

    int GetCaps(eGraphicCaps aType);

    void ShowCursor(bool abX);

    void SetWindowGrab(bool abX);

    void SetRelativeMouse(bool abX);

    void SetWindowCaption(const tString &asName);

    void SetWindowIcon();

    bool GetWindowMouseFocus();

    bool GetWindowInputFocus();

    bool GetWindowIsVisible();

    bool GetFullscreenModeActive()
    {
        return mbFullscreen;
    }

    void SetVsyncActive(bool abX);

    void SetGammaCorrection(float afX);
    float GetGammaCorrection();

    cVector2f GetScreenSizeFloat();
    const cVector2l &GetScreenSizeInt();

    /////////////////////////////////////////////////////
    /////////////// DATA CREATION //////////////////////
    /////////////////////////////////////////////////////

    iFontData *CreateFontData(const tString &asName);

    iTexture *CreateTexture(const tString &asName, eTextureType aType, eTextureUsage aUsage);

    iVertexBuffer *CreateVertexBuffer(    eVertexBufferType aType,
                                          eVertexBufferDrawType aDrawType,
                                          eVertexBufferUsageType aUsageType,
                                          int alReserveVtxSize = 0, int alReserveIdxSize = 0);

    iGpuProgram *CreateGpuProgram(const tString& asName);
    iGpuShader *CreateGpuShader(const tString& asName, eGpuShaderType aType);

    iFrameBuffer *CreateFrameBuffer(const tString& asName);
    iDepthStencilBuffer *CreateDepthStencilBuffer(const cVector2l& avSize, int alDepthBits, int alStencilBits);

    iOcclusionQuery *CreateOcclusionQuery();

    /////////////////////////////////////////////////////
    /////////// FRAME BUFFER OPERATIONS ///////
    /////////////////////////////////////////////////////

    void ClearFrameBuffer(tClearFrameBufferFlag aFlags);

    void SetClearColor(const cColor& aCol);
    void SetClearDepth(float afDepth);
    void SetClearStencil(int alVal);

    void CopyFrameBufferToTexure(    iTexture* apTex, const cVector2l &avPos,
                                     const cVector2l &avSize, const cVector2l &avTexOffset = 0);
    cBitmap *CopyFrameBufferToBitmap(const cVector2l &avScreenPos = 0, const cVector2l &avScreenSize = -1);

    void FlushRendering();
    void SwapBuffers();

    void SetCurrentFrameBuffer(iFrameBuffer* apFrameBuffer, const cVector2l &avPos = 0, const cVector2l& avSize = -1);
    iFrameBuffer *GetCurrentFrameBuffer()
    {
        return mpFrameBuffer;
    }

    void SetFrameBufferDrawTargets(int *apTargets, int alNumOfTargets);

    /////////////////////////////////////////////////////
    /////////// RENDER STATE ////////////////////////////
    /////////////////////////////////////////////////////

    void SetColorWriteActive(bool abR, bool abG, bool abB, bool abA);
    void SetDepthWriteActive(bool abX);

    void SetCullActive(bool abX);
    void SetCullMode(eCullMode aMode);

    void SetDepthTestActive(bool abX);
    void SetDepthTestFunc(eDepthTestFunc aFunc);

    void SetAlphaTestActive(bool abX);
    void SetAlphaTestFunc(eAlphaTestFunc aFunc, float afRef);

    void SetStencilActive(bool abX);
    void SetStencilWriteMask(unsigned int alMask);
    void SetStencil(eStencilFunc aFunc, int alRef, unsigned int aMask,
                    eStencilOp aFailOp, eStencilOp aZFailOp, eStencilOp aZPassOp);
    void SetStencilTwoSide(    eStencilFunc aFrontFunc, eStencilFunc aBackFunc,
                               int alRef, unsigned int aMask,
                               eStencilOp aFrontFailOp, eStencilOp aFrontZFailOp, eStencilOp aFrontZPassOp,
                               eStencilOp aBackFailOp, eStencilOp aBackZFailOp, eStencilOp aBackZPassOp);

    void SetScissorActive(bool abX);
    void SetScissorRect(const cVector2l& avPos, const cVector2l& avSize);

    void SetClipPlane(int alIdx, const cPlanef& aPlane);
    cPlanef GetClipPlane(int alIdx);
    void SetClipPlaneActive(int alIdx, bool abX);

    void SetColor(const cColor &aColor);

    void SetBlendActive(bool abX);
    void SetBlendFunc(eBlendFunc aSrcFactor, eBlendFunc aDestFactor);
    void SetBlendFuncSeparate(    eBlendFunc aSrcFactorColor, eBlendFunc aDestFactorColor,
                                  eBlendFunc aSrcFactorAlpha, eBlendFunc aDestFactorAlpha);

    void SetPolygonOffsetActive(bool abX);
    void SetPolygonOffset(float afBias, float afSlopeScaleBias);

    /////////////////////////////////////////////////////
    /////////// MATRIX //////////////////////////////////
    /////////////////////////////////////////////////////

    void PushMatrix(eMatrix aMtxType);
    void PopMatrix(eMatrix aMtxType);
    void SetIdentityMatrix(eMatrix aMtxType);

    void SetMatrix(eMatrix aMtxType, const cMatrixf& a_mtxA);

    void SetOrthoProjection(const cVector2f& avSize, float afMin, float afMax);
    void SetOrthoProjection(const cVector3f& avMin, const cVector3f& avMax);

    /////////////////////////////////////////////////////
    /////////// TEXTURE OPERATIONS ///////////////////////
    /////////////////////////////////////////////////////

    void SetTexture(unsigned int alUnit, iTexture* apTex);
    void ClearTextureHandle(GLuint aHandle);

    void SetActiveTextureUnit(unsigned int alUnit);
    void SetTextureEnv(eTextureParam aParam, int alVal);
    void SetTextureConstantColor(const cColor &aColor);


    /////////////////////////////////////////////////////
    /////////// DRAWING ///////////////////////////////
    /////////////////////////////////////////////////////

    void DrawTriangle(tVertexVec& avVtx);

    void DrawQuad(    const cVector3f &avPos, const cVector2f &avSize, const cColor& aColor = cColor(1, 1));
    void DrawQuad(    const cVector3f &avPos, const cVector2f &avSize,
                      const cVector2f &avMinTexCoord, const cVector2f &avMaxTexCoord,
                      const cColor& aColor = cColor(1, 1));
    void DrawQuad(    const cVector3f &avPos, const cVector2f &avSize,
                      const cVector2f &avMinTexCoord0, const cVector2f &avMaxTexCoord0,
                      const cVector2f &avMinTexCoord1, const cVector2f &avMaxTexCoord1,
                      const cColor& aColor = cColor(1, 1));

    void DrawQuad(const tVertexVec &avVtx);
    void DrawQuad(const tVertexVec &avVtx, const cColor aCol);
    void DrawQuad(const tVertexVec &avVtx, const float afZ);
    void DrawQuad(const tVertexVec &avVtx, const float afZ, const cColor &aCol);
    void DrawQuadMultiTex(const tVertexVec &avVtx, const tVector3fVec &avExtraUvs);

    void DrawLine(const cVector3f& avBegin, const cVector3f& avEnd, cColor aCol);
    void DrawLine(const cVector3f& avBegin, const cColor& aBeginCol, const cVector3f& avEnd, const cColor& aEndCol);

    void DrawBoxMinMax(const cVector3f& avMin, const cVector3f& avMax, cColor aCol);
    void DrawSphere(const cVector3f& avPos, float afRadius, cColor aCol);
    void DrawSphere(const cVector3f& avPos, float afRadius, cColor aColX, cColor aColY, cColor aColZ);

    void DrawLineQuad(const cRect2f& aRect, float afZ, cColor aCol);
    void DrawLineQuad(const cVector3f &avPos, const cVector2f &avSize, cColor aCol);

    /////////////////////////////////////////////////////
    /////////// VERTEX BATCHING /////////////////////////
    /////////////////////////////////////////////////////

    void AddVertexToBatch(const cVertex *apVtx);
    void AddVertexToBatch(const cVertex *apVtx, const cVector3f* avTransform);
    void AddVertexToBatch(const cVertex *apVtx, const cMatrixf* aMtx);

    void AddVertexToBatch_Size2D(const cVertex *apVtx, const cVector3f* avTransform,
                                 const cColor* apCol, const float &mfW, const float &mfH);

    void AddVertexToBatch_Raw(    const cVector3f& avPos, const cColor &aColor,
                                  const cVector3f& avTex);


    void AddTexCoordToBatch(unsigned int alUnit, const cVector3f *apCoord);
    void SetBatchTextureUnitActive(unsigned int alUnit, bool abActive);

    void AddIndexToBatch(int alIndex);

    void FlushTriBatch(tVtxBatchFlag aTypeFlags, bool abAutoClear = true);
    void FlushQuadBatch(tVtxBatchFlag aTypeFlags, bool abAutoClear = true);
    void ClearBatch();

    /////////////////////////////////////////////////////
    /////////// IMPLEMENTION SPECIFICS /////////////////
    /////////////////////////////////////////////////////

    void SetupGL();

private:
    cVector2l mvScreenSize;
    int mlBpp;
    bool mbFullscreen;
    eGpuProgramFormat mGpuProgramFormat;

    //////////////////////////////////////
    //Windows stuff
#if defined(_WIN32)
    HGLRC mGLContext;
    HDC   mDeviceContext;
    HINSTANCE hInstance;
#endif

    bool mbInitHasBeenRun;

    float mfPolygonOffsetBias;
    float mfPolygonOffsetSlope;
    bool mbPolygonOffsetActive;

    unsigned int mlStencilWriteMask;

    //////////////////////////////////////
    //Render state settings
    cColorWriteDL mColorWrite;
    bool mbDepthWrite;

    bool mbCullActive;
    eCullMode mCullMode;

    bool mbDepthTestActive;
    eDepthTestFunc mDepthTestFunc;

    bool mbAlphaTestActive;
    eAlphaTestFunc mAlphaTestFunc;
    float mfAlphaTestFuncRef;

    bool mbScissorActive;
    cVector2l mvScissorPos;
    cVector2l mvScissorSize;
    int mlScissorFrameBufferHeight;

    bool mbBlendActive;
    eBlendFunc mBlendSrcFactor;
    eBlendFunc mBlendDestFactor;
    eBlendFunc mBlendSrcFactorAlpha;
    eBlendFunc mBlendDestFactorAlpha;

    bool mbBlendFuncSeparate;

    iFrameBuffer *mpFrameBuffer;
    cVector2l mvFrameBufferPos;
    cVector2l mvFrameBufferSize;
    cVector2l mvFrameBufferTotalSize;

    //////////////////////////////////////
    //Gamma
    Uint16 mvStartGammaArray[3][256];
    float mfGammaCorrection;

    //////////////////////////////////////
    //Clipping
    cPlanef mvClipPlanes[kMaxClipPlanes];


    //////////////////////////////////////
    //SDL Variables
    SDL_Surface *mpScreen;
    bool mbGrab;

    //////////////////////////////////////
    //Vertex Array variables
    //The vertex arrays used:
    float *mpVertexArray;
    unsigned int mlVertexCount;
    unsigned int *mpIndexArray;
    unsigned int mlIndexCount;

    unsigned int mlBatchStride;

    float *mpTexCoordArray[kMaxTextureUnits];
    bool mbTexCoordArrayActive[kMaxTextureUnits];
    unsigned int mlTexCoordArrayCount[kMaxTextureUnits];

    unsigned int mlBatchArraySize;

    //////////////////////////////////////
    //Texture
    GLenum mvCurrentTextureTarget[kMaxTextureUnits];
    GLenum mvCurrentTextureHandle[kMaxTextureUnits];

    //////////////////////////////////////
    //Double sided stencil
    bool mbDoubleSidedStencilIsSet;

    //////////////////////////////////////
    //Matrix helper
    void SetMatrixMode(eMatrix mType);

    //////////////////////////////////////
    //Batch helper
    void SetUpBatchArrays();

    //////////////////////////////////////
    //Vtx helper
    void SetVtxBatchStates(tVtxBatchFlag aFlags);
};
};
#endif // HPL_LOWLEVELGRAPHICS_SDL_H
