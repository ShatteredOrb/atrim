#ifndef HPL_GLSL_PROGRAM_H
#define HPL_GLSL_PROGRAM_H

//#include <windows.h>

#include "system/SystemTypes.h"
#include "math/MathTypes.h"
#include "graphics/GPUShader.h"

#include "graphics/GPUProgram.h"

#include <GL/glew.h>
#include <string.h>

namespace hpl
{

//-----------------------------------------------------

enum eGLSLParamCache
{
    eGLSLParamCache_None,
    eGLSLParamCache_Int,
    eGLSLParamCache_Float,
    eGLSLParamCache_Vec2,
    eGLSLParamCache_Vec3,
    eGLSLParamCache_Vec4,
    eGLSLParamCache_Matrix
};

class cGLSLParam
{
public:
    cGLSLParam() : msName(""), mlId(-1), mCacheType(eGLSLParamCache_None), mlCacheInt(0)
    {
        memset(mfCache, 0, sizeof(mfCache));
    }
    cGLSLParam(GLint alId, const tString& asName) : msName(asName), mlId(alId), mCacheType(eGLSLParamCache_None), mlCacheInt(0)
    {
        memset(mfCache, 0, sizeof(mfCache));
    }

    void ClearCache()
    {
        mCacheType = eGLSLParamCache_None;
    }

    tString msName;
    GLint mlId;

    eGLSLParamCache mCacheType;
    int mlCacheInt;
    float mfCache[16];
};

//-----------------------------------------------------

class cGLSLProgram : public iGpuProgram
{
public:
    cGLSLProgram(const tString& asName);
    ~cGLSLProgram();

    bool Link();

    void Bind();
    void UnBind();

    bool CanAccessAPIMatrix()
    {
        return true;
    }
    bool SamplerNeedsTextureUnitSetup()
    {
        return true;
    }

    /**
     * Binds and unbinds program so must not be done during rendering.
     */
    bool SetSamplerToUnit(const tString& asSamplerName, int alUnit);

    int GetVariableId(const tString& asName);
    bool GetVariableAsId(const tString& asName, int alId);

    bool SetInt(int alVarId, int alX);
    bool SetFloat(int alVarId, float afX);
    bool SetVec2f(int alVarId, float afX, float afY);
    bool SetVec3f(int alVarId, float afX, float afY, float afZ);
    bool SetVec4f(int alVarId, float afX, float afY, float afZ, float afW);

    bool SetMatrixf(int alVarId, const cMatrixf& aMtx);
    bool SetMatrixf(int alVarId, eGpuShaderMatrix aType, eGpuShaderMatrixOp aOp);

private:
    void LogProgramInfoLog();

    GLuint mlHandle;
    static int mlCurrentProgram;

    std::vector<cGLSLParam> mvParameters;
};
};
#endif // HPL_GLSL_PROGRAM_H
