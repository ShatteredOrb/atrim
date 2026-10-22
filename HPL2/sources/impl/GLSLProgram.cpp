#include "impl/GLSLProgram.h"

#include "system/LowLevelSystem.h"
#include "system/String.h"

#include "impl/SDLTexture.h"
#include "impl/GLSLShader.h"
#include "impl/LowLevelGraphicsSDL.h"



namespace hpl
{

//-----------------------------------------------------------------------

int cGLSLProgram::mlCurrentProgram = 0;

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cGLSLProgram::cGLSLProgram(const tString& asName) : iGpuProgram(asName, eGpuProgramFormat_GLSL)
{
    mlHandle = glCreateProgram();

}

//-----------------------------------------------------------------------

cGLSLProgram::~cGLSLProgram()
{
    for(int i = 0; i < 2; ++i)
    {
        cGLSLShader* pGLSLShader = static_cast<cGLSLShader *>(mpShader[i]);
        if(pGLSLShader)
        {
            glDetachShader(mlHandle, pGLSLShader->GetHandle());
        }
    }
    glDeleteProgram(mlHandle);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cGLSLProgram::Link()
{
    ///////////////////////////////////////
    //Attach shaders
    for(int i = 0; i < 2; ++i)
    {
        cGLSLShader* pGLSLShader = static_cast<cGLSLShader *>(mpShader[i]);
        if(pGLSLShader)
        {
            glAttachShader(mlHandle, pGLSLShader->GetHandle());
        }
    }

    ///////////////////////////////////////
    //Link
    glLinkProgram(mlHandle);

    ///////////////////////////////////////
    //Check for errors
    GLint lStatus;
    glGetProgramiv(mlHandle, GL_LINK_STATUS, &lStatus);
    if(lStatus == GL_FALSE)
    {
        Error("Failed to link GLSL program %s\n", msName.c_str());
        LogProgramInfoLog();
        return false;
    }

    for(size_t i = 0; i < mvParameters.size(); ++i)
    {
        mvParameters[i].ClearCache();
    }

    ///////////////////////////////////////
    //Set up sampler units
    iGpuShader* pFragShader = mpShader[eGpuShaderType_Fragment];
    if(pFragShader && pFragShader->GetSamplerUnitNum() > 0)
    {

        //Log("Setting up samplers in '%s'\n", msName.c_str());
        glUseProgram(mlHandle);

        for(int i = 0; i < pFragShader->GetSamplerUnitNum(); ++i)
        {
            tString &sSamplerName = pFragShader->GetSamplerUnit(i)->msName;
            int lUnit = pFragShader->GetSamplerUnit(i)->mlUnit;

            //Log("Setting %s to %d\n",sSamplerName.c_str(), lUnit);
            GLint lVarHandle = glGetUniformLocation(mlHandle, sSamplerName.c_str());
            if(lVarHandle < 0)
            {
                Error("Sampler %s does not exist, could not bind it to unit %d\n", sSamplerName.c_str(), lUnit);
                continue;
            }

            glUniform1i(lVarHandle, lUnit);
        }

        glUseProgram(0);
    }

    return true;
}

//-----------------------------------------------------------------------

void cGLSLProgram::Bind()
{
    if(mlCurrentProgram == mlHandle)
    {
        return;
    }

    mlCurrentProgram = mlHandle;
    glUseProgram(mlHandle);
}

//-----------------------------------------------------------------------

void cGLSLProgram::UnBind()
{
    if(mlCurrentProgram == 0)
    {
        return;
    }

    mlCurrentProgram = 0;
    glUseProgram(0);
}


//-----------------------------------------------------------------------

bool cGLSLProgram::SetSamplerToUnit(const tString& asSamplerName, int alUnit)
{
    GLint lVarHandle = glGetUniformLocation(mlHandle, asSamplerName.c_str());
    if(lVarHandle < 0)
    {
        return false;
    }

    Bind();
    glUniform1i(lVarHandle, alUnit);
    UnBind();

    for(size_t i = 0; i < mvParameters.size(); ++i)
    {
        if(mvParameters[i].mlId == lVarHandle)
        {
            mvParameters[i].ClearCache();
        }
    }

    return true;
}

//-----------------------------------------------------------------------

int cGLSLProgram::GetVariableId(const tString& asName)
{
    for(size_t i = 0; i < mvParameters.size(); ++i)
    {
        if(mvParameters[i].msName == asName)
        {
            return (int)i;
        }
    }

    GLint lId = glGetUniformLocation(mlHandle, asName.c_str());
    if(lId < 0)
    {
        return -1;
    }

    mvParameters.push_back(cGLSLParam(lId, asName));

    return (int)mvParameters.size() - 1;
}

//-----------------------------------------------------------------------

bool cGLSLProgram::GetVariableAsId(const tString& asName, int alId)
{
    if(alId < 0)
    {
        return false;
    }

    ////////////////////////
    // Check if id is already set to variable
    for(size_t i = 0; i < mvParameters.size(); ++i)
    {
        if(mvParameters[i].msName == asName)
        {
            if(i == alId)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }
    ////////////////////////
    // Check if variable exits
    GLint lHandle = glGetUniformLocation(mlHandle, asName.c_str());
    if(lHandle < 0)
    {
        return false;
    }

    ////////////////////////
    // Check if the id is taken
    if(alId < (int)mvParameters.size())
    {
        if(mvParameters[alId].mlId >= 0)
        {
            if(mvParameters[alId].mlId == lHandle)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }
    ////////////////////////
    // Resize vector so id fits
    else
    {
        mvParameters.resize(alId + 1);
    }

    ////////////////////////
    // Vector is big enough and id is not taken or already added,
    // add it.
    mvParameters[alId].mlId = lHandle;
    mvParameters[alId].msName = asName;
    mvParameters[alId].ClearCache();

    return true;
}

//-----------------------------------------------------------------------

bool cGLSLProgram::SetInt(int alVarId, int alX)
{
    if(alVarId < 0 || alVarId >= (int)mvParameters.size())
    {
        return false;
    }

    if(mlCurrentProgram != mlHandle)
    {
        Bind();
    }

    cGLSLParam &param = mvParameters[alVarId];
    if(param.mCacheType == eGLSLParamCache_Int && param.mlCacheInt == alX)
    {
        return true;
    }
    param.mCacheType = eGLSLParamCache_Int;
    param.mlCacheInt = alX;

    glUniform1i(param.mlId, alX);

    return true;
}

//-----------------------------------------------------------------------


bool  cGLSLProgram::SetFloat(int alVarId, float afX)
{
    if(alVarId < 0 || alVarId >= (int)mvParameters.size())
    {
        return false;
    }

    if(mlCurrentProgram != mlHandle)
    {
        Bind();
    }

    cGLSLParam &param = mvParameters[alVarId];
    if(param.mCacheType == eGLSLParamCache_Float && param.mfCache[0] == afX)
    {
        return true;
    }
    param.mCacheType = eGLSLParamCache_Float;
    param.mfCache[0] = afX;

    glUniform1f(param.mlId, afX);

    return true;
}

//-----------------------------------------------------------------------

bool  cGLSLProgram::SetVec2f(int alVarId, float afX, float afY)
{
    if(alVarId < 0 || alVarId >= (int)mvParameters.size())
    {
        return false;
    }

    if(mlCurrentProgram != mlHandle)
    {
        Bind();
    }

    cGLSLParam &param = mvParameters[alVarId];
    if(param.mCacheType == eGLSLParamCache_Vec2 && param.mfCache[0] == afX && param.mfCache[1] == afY)
    {
        return true;
    }
    param.mCacheType = eGLSLParamCache_Vec2;
    param.mfCache[0] = afX;
    param.mfCache[1] = afY;

    glUniform2f(param.mlId, afX, afY);

    return true;
}

//-----------------------------------------------------------------------

bool  cGLSLProgram::SetVec3f(int alVarId, float afX, float afY, float afZ)
{
    if(alVarId < 0 || alVarId >= (int)mvParameters.size())
    {
        return false;
    }

    if(mlCurrentProgram != mlHandle)
    {
        Bind();
    }

    cGLSLParam &param = mvParameters[alVarId];
    if(param.mCacheType == eGLSLParamCache_Vec3 &&
       param.mfCache[0] == afX && param.mfCache[1] == afY && param.mfCache[2] == afZ)
    {
        return true;
    }
    param.mCacheType = eGLSLParamCache_Vec3;
    param.mfCache[0] = afX;
    param.mfCache[1] = afY;
    param.mfCache[2] = afZ;

    glUniform3f(param.mlId, afX, afY, afZ);

    return true;
}

//-----------------------------------------------------------------------

bool  cGLSLProgram::SetVec4f(int alVarId, float afX, float afY, float afZ, float afW)
{
    if(alVarId < 0 || alVarId >= (int)mvParameters.size())
    {
        return false;
    }

    if(mlCurrentProgram != mlHandle)
    {
        Bind();
    }

    cGLSLParam &param = mvParameters[alVarId];
    if(param.mCacheType == eGLSLParamCache_Vec4 &&
       param.mfCache[0] == afX && param.mfCache[1] == afY &&
       param.mfCache[2] == afZ && param.mfCache[3] == afW)
    {
        return true;
    }
    param.mCacheType = eGLSLParamCache_Vec4;
    param.mfCache[0] = afX;
    param.mfCache[1] = afY;
    param.mfCache[2] = afZ;
    param.mfCache[3] = afW;

    glUniform4f(param.mlId, afX, afY, afZ, afW);

    return true;
}

//-----------------------------------------------------------------------

bool cGLSLProgram::SetMatrixf(int alVarId, const cMatrixf& aMtx)
{
    if(alVarId < 0 || alVarId >= (int)mvParameters.size())
    {
        return false;
    }

    if(mlCurrentProgram != mlHandle)
    {
        Bind();
    }

    cGLSLParam &param = mvParameters[alVarId];
    if(param.mCacheType == eGLSLParamCache_Matrix && memcmp(param.mfCache, aMtx.v, sizeof(float) * 16) == 0)
    {
        return true;
    }
    param.mCacheType = eGLSLParamCache_Matrix;
    memcpy(param.mfCache, aMtx.v, sizeof(float) * 16);

    glUniformMatrix4fv(param.mlId, 1, true, aMtx.v);

    return true;
}

//-----------------------------------------------------------------------

bool cGLSLProgram::SetMatrixf(int alVarId, eGpuShaderMatrix aType,
                              eGpuShaderMatrixOp aOp)
{
    //Not needed for GLSL
    return false;
}


//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PROTECTED METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cGLSLProgram::LogProgramInfoLog()
{
    GLint infologLength = 0;
    GLsizei charsWritten  = 0;
    char *infoLog;

    glGetProgramiv(mlHandle, GL_INFO_LOG_LENGTH, &infologLength);

    if (infologLength > 0)
    {
        infoLog = (char *)hplMalloc(infologLength);
        glGetProgramInfoLog(mlHandle, infologLength, &charsWritten, infoLog);
        Log("-------------\n");
        Log("%s\n", infoLog);
        Log("-------------\n");
        hplFree(infoLog);
    }
}

//-----------------------------------------------------------------------

}
