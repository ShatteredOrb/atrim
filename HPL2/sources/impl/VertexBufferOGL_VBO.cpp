#include "impl/VertexBufferOGL_VBO.h"

#include "system/LowLevelSystem.h"
#include "math/Math.h"

#include "impl/LowLevelGraphicsSDL.h"

#include <memory.h>

#include <GL/glew.h>


namespace hpl
{

#define BUFFER_OFFSET(i) ((void*)(i*sizeof(float)))

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cVertexBufferOGL_VBO::cVertexBufferOGL_VBO(iLowLevelGraphics* apLowLevelGraphics,
        eVertexBufferDrawType aDrawType, eVertexBufferUsageType aUsageType,
        int alReserveVtxSize, int alReserveIdxSize) :
    iVertexBufferOpenGL(apLowLevelGraphics, eVertexBufferType_Hardware,  aDrawType, aUsageType, alReserveVtxSize, alReserveIdxSize)
{
    mlElementHandle = 0;

    mlInterleavedHandle[0] = 0;
    mlInterleavedHandle[1] = 0;

    mlCurrentInterleaved = 0;
    mlInterleavedStride = 0;

    mlLastUploadedBytes[0] = 0;
    mlLastUploadedBytes[1] = 0;

    mbUse16BitIndices = false;
    mlIndexBufferSize = 0;
}

//-----------------------------------------------------------------------

cVertexBufferOGL_VBO::~cVertexBufferOGL_VBO()
{
    if(mlInterleavedHandle[0])
    {
        glDeleteBuffersARB(1, &mlInterleavedHandle[0]);
    }
    if(mlInterleavedHandle[1])
    {
        glDeleteBuffersARB(1, &mlInterleavedHandle[1]);
    }

    if(mlElementHandle)
    {
        glDeleteBuffersARB(1, (GLuint*)&mlElementHandle);
    }
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cVertexBufferOGL_VBO::UpdateData(tVertexElementFlag aTypes, bool abIndices)
{
    GLenum usageType = GL_STATIC_DRAW_ARB;

    if(mUsageType == eVertexBufferUsageType_Dynamic)
    {
        usageType = GL_DYNAMIC_DRAW_ARB;
    }
    else if(mUsageType == eVertexBufferUsageType_Stream)
    {
        usageType = GL_STREAM_DRAW_ARB;
    }

    if(aTypes != 0 && mlInterleavedStride > 0)
    {
        const int lVertexCount = GetVertexNum();
        const size_t lTotalBytes = (size_t)lVertexCount * (size_t)mlInterleavedStride;

        if(lTotalBytes > 0)
        {
            int lUpdateIdx = 1 - mlCurrentInterleaved;
            unsigned int lHandle = mlInterleavedHandle[lUpdateIdx];

            static std::vector<char> temp; // Temporary buffer
            if(temp.size() < lTotalBytes)
            {
                temp.resize(lTotalBytes);
            }

            for(size_t i = 0; i < mvElementArrays.size(); ++i)
            {
                cVtxBufferGLElementArray *pElement = mvElementArrays[i];
                const size_t lElementBytes = pElement->mlElementNum * GetVertexFormatByteSize(pElement->mFormat);
                const char *src = (const char*)pElement->GetArrayPtr();
                if(src == NULL)
                {
                    continue; // empty element
                }

                for(int v = 0; v < lVertexCount; ++v)
                {
                    char *dst = &temp[(size_t)v * mlInterleavedStride + mvElementOffsets[i]];
                    memcpy(dst, src + (size_t)v * lElementBytes, lElementBytes);
                }
            }

            glBindBufferARB(GL_ARRAY_BUFFER_ARB, lHandle);

            if(lTotalBytes != mlLastUploadedBytes[lUpdateIdx])
            {
                glBufferDataARB(GL_ARRAY_BUFFER_ARB, (GLsizeiptr)lTotalBytes, NULL, usageType);
                mlLastUploadedBytes[lUpdateIdx] = lTotalBytes;
            }

            glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, (GLsizeiptr)lTotalBytes, &temp[0]);
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

            mlCurrentInterleaved = lUpdateIdx;
        }
    }

    //Create the VBO index array
    if(abIndices && mlElementHandle != 0)
    {
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, mlElementHandle);

        const int lIndexCount = GetIndexNum();
        if(lIndexCount <= 0 || mvIndexArray.empty())
        {
            mlIndexBufferSize = 0;
            glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0, NULL, usageType);
        }
        else if(mbUse16BitIndices)
        {
            //16bit conversion
            std::vector<unsigned short> shortIndices(lIndexCount);

            for(int i = 0; i < lIndexCount; ++i)
            {
#ifdef _DEBUG
                if(mvIndexArray[i] > 65535)
                {
                    Error("Index %u exceeds range in 16-bit marked buffer!\n", mvIndexArray[i]);
                }
#endif
                shortIndices[i] = (unsigned short)mvIndexArray[i];
            }

            mlIndexBufferSize = lIndexCount * (int)sizeof(unsigned short);

            glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB,
                            mlIndexBufferSize, &shortIndices[0], usageType);
        }
        else
        {
            //Use 32bit
            mlIndexBufferSize = lIndexCount * (int)sizeof(unsigned int);

            glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB,
                            mlIndexBufferSize, &mvIndexArray[0], usageType);
        }

        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
    }
}

//-----------------------------------------------------------------------

void cVertexBufferOGL_VBO::Draw(eVertexBufferDrawType aDrawType)
{
    eVertexBufferDrawType drawType = aDrawType == eVertexBufferDrawType_LastEnum ? mDrawType : aDrawType;

    ///////////////////////////////
    //Get the draw type
    GLenum mode = GetDrawModeFromDrawType(drawType);

    //////////////////////////////////
    //Bind and draw the buffer

    int lSize = mlElementNum;
    if(mlElementNum < 0)
    {
        lSize = GetIndexNum();
    }

    if(lSize <= 0 || mlElementHandle == 0)
    {
        return; // nothing to draw (empty/broken mesh)
    }

    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, mlElementHandle);

    GLenum indexType = mbUse16BitIndices ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;

    glDrawElements(mode, lSize, indexType, (char*)NULL);
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
}

//-----------------------------------------------------------------------

void cVertexBufferOGL_VBO::DrawIndices(unsigned int *apIndices, int alCount, eVertexBufferDrawType aDrawType)
{
    eVertexBufferDrawType drawType = aDrawType == eVertexBufferDrawType_LastEnum ? mDrawType : aDrawType;

    ///////////////////////////////
    //Get the draw type
    GLenum mode = GetDrawModeFromDrawType(drawType);

    //////////////////////////////////
    //Bind and draw the buffer
    glDrawElements(mode, alCount, GL_UNSIGNED_INT, apIndices);
}


//-----------------------------------------------------------------------

void cVertexBufferOGL_VBO::Bind()
{
    SetVertexStates();
}

//-----------------------------------------------------------------------

void cVertexBufferOGL_VBO::UnBind()
{
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

    for(size_t i = 0; i < mvElementArrays.size(); ++i)
    {
        cVtxBufferGLElementArray *pElement = mvElementArrays[i];

        //Log("Unbinding %d handle %d, type: %d\n",i,pElement->mlGLHandle, pElement->mType);

        int lTextureUnit = GetVertexElementTextureUnit(pElement->mType);
        if(lTextureUnit >= 0)
        {
            glClientActiveTextureARB(GL_TEXTURE0_ARB + lTextureUnit);
        }

        glDisableClientState( GetGLArrayFromVertexElement(pElement->mType) );
    }
    glClientActiveTextureARB(GL_TEXTURE0_ARB);
}

//-----------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
/////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cVertexBufferOGL_VBO::CompileSpecific()
{
    GLenum usageType = GL_STATIC_DRAW_ARB;

    if(mUsageType == eVertexBufferUsageType_Dynamic)
    {
        usageType = GL_DYNAMIC_DRAW_ARB;
    }
    else if(mUsageType == eVertexBufferUsageType_Stream)
    {
        usageType = GL_STREAM_DRAW_ARB;
    }

    //Create the VBO vertex arrays
    mlInterleavedStride = 0;
    mvElementOffsets.resize(mvElementArrays.size());

    for(size_t i = 0; i < mvElementArrays.size(); ++i)
    {
        cVtxBufferGLElementArray *pElement = mvElementArrays[i];
        mvElementOffsets[i] = mlInterleavedStride;

        mlInterleavedStride += pElement->mlElementNum * GetVertexFormatByteSize(pElement->mFormat);
    }

    const int lVertexCount = GetVertexNum();
    const size_t lTotalBytes = (size_t)lVertexCount * (size_t)mlInterleavedStride;

    for(int b = 0; b < 2; ++b)
    {
        if(mlInterleavedHandle[b] == 0)
        {
            glGenBuffersARB(1, &mlInterleavedHandle[b]);
        }
    }

    // Pack into temp buffer (skip when there is no vertex data)
    if(lTotalBytes > 0 && mlInterleavedStride > 0)
    {
        std::vector<char> temp(lTotalBytes);

        for(size_t i = 0; i < mvElementArrays.size(); ++i)
        {
            cVtxBufferGLElementArray *pElement = mvElementArrays[i];

            const size_t lElementBytes = pElement->mlElementNum * GetVertexFormatByteSize(pElement->mFormat);
            const char *src = (const char*)pElement->GetArrayPtr();
            if(src == NULL)
            {
                continue; // empty element
            }

            for(int v = 0; v < lVertexCount; ++v)
            {
                char *dst = &temp[(size_t)v * mlInterleavedStride + mvElementOffsets[i]];
                memcpy(dst, src + (size_t)v * lElementBytes, lElementBytes);
            }
        }

        for(int b = 0; b < 2; ++b)
        {
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, mlInterleavedHandle[b]);
            glBufferDataARB(GL_ARRAY_BUFFER_ARB, (GLsizeiptr)lTotalBytes, &temp[0], usageType);
            mlLastUploadedBytes[b] = lTotalBytes;
        }
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
    }
    else
    {
        // No usable vertex data, allocate empty VBOs so later binds stay valid
        for(int b = 0; b < 2; ++b)
        {
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, mlInterleavedHandle[b]);
            glBufferDataARB(GL_ARRAY_BUFFER_ARB, 0, NULL, usageType);
            mlLastUploadedBytes[b] = 0;
        }
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
    }

    //Start with buffer 0 or the primary buffer
    mlCurrentInterleaved = 0;

    //Create the VBO index array
    const int lIndexCount = GetIndexNum();
    mbUse16BitIndices = (GetVertexNum() <= 65535);

    if(mlElementHandle == 0)
    {
        glGenBuffersARB(1, (GLuint*)&mlElementHandle);
    }

    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, mlElementHandle);

    if(lIndexCount <= 0 || mvIndexArray.empty())
    {
        mlIndexBufferSize = 0;
        glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0, NULL, usageType);
    }
    else if(mbUse16BitIndices)
    {
        //16bit conversion
        std::vector<unsigned short> shortIndices(lIndexCount);

        for(int i = 0; i < lIndexCount; ++i)
        {
#ifdef _DEBUG
            if(mvIndexArray[i] > 65535)
            {
                Error("Index %u exceeds range in 16-bit marked buffer!\n", mvIndexArray[i]);
            }
#endif
            shortIndices[i] = (unsigned short)mvIndexArray[i];
        }

        mlIndexBufferSize = lIndexCount * (int)sizeof(unsigned short);

        glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, mlIndexBufferSize,
                        &shortIndices[0], usageType);
    }
    else
    {
        //Use 32bit
        mlIndexBufferSize = lIndexCount * (int)sizeof(unsigned int);

        glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, mlIndexBufferSize,
                        &mvIndexArray[0], usageType);
    }

    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
}

//-----------------------------------------------------------------------

iVertexBufferOpenGL *cVertexBufferOGL_VBO::CreateDataCopy(tVertexElementFlag aFlags, eVertexBufferDrawType aDrawType,
        eVertexBufferUsageType aUsageType,
        int alReserveVtxSize, int alReserveIdxSize)
{
    return hplNew(cVertexBufferOGL_VBO, (mpLowLevelGraphics, aDrawType, aUsageType, alReserveVtxSize, alReserveIdxSize));
}

//-----------------------------------------------------------------------

void cVertexBufferOGL_VBO::SetVertexStates()
{
    ////////////////////////////////////////
    // Bind the interleaved buffer designated for drawing
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, mlInterleavedHandle[mlCurrentInterleaved]);

    ////////////////////////////////////////
    // Set all vertices except position
    bool bHadExtraTextureUnit = false;

    for(size_t i = 0; i < mvElementArrays.size(); ++i)
    {
        cVtxBufferGLElementArray *pElement = mvElementArrays[i];
        if(pElement->mType == eVertexBufferElement_Position)
        {
            continue;    //Make sure position is called last...
        }

        GLenum GLType = GetGLTypeFromVertexFormat(pElement->mFormat);
        int lSize = pElement->mlElementNum;

        int lTextureUnit = GetVertexElementTextureUnit(pElement->mType);
        if(lTextureUnit >= 0)
        {
            if(lTextureUnit > 0)
            {
                bHadExtraTextureUnit = true;
            }
            glClientActiveTextureARB(GL_TEXTURE0_ARB + lTextureUnit);
        }

        glEnableClientState( GetGLArrayFromVertexElement(pElement->mType) );

        const char *offset = (const char*)NULL + mvElementOffsets[i];

        switch(pElement->mType)
        {
        case eVertexBufferElement_Normal:
            glNormalPointer(GLType, (GLsizei)mlInterleavedStride, offset);
            break;

        case eVertexBufferElement_Color0:
            glColorPointer(lSize, GLType, (GLsizei)mlInterleavedStride, offset);
            break;

        case eVertexBufferElement_Color1:
            glSecondaryColorPointerEXT(lSize, GLType, (GLsizei)mlInterleavedStride, offset);
            break;

        case eVertexBufferElement_Texture1Tangent:
        case eVertexBufferElement_Texture0:
        case eVertexBufferElement_Texture1:
        case eVertexBufferElement_Texture2:
        case eVertexBufferElement_Texture3:
        case eVertexBufferElement_Texture4:
            glTexCoordPointer(lSize, GLType, (GLsizei)mlInterleavedStride, offset);
            break;
        }
    }
    if(bHadExtraTextureUnit)
    {
        glClientActiveTextureARB(GL_TEXTURE0_ARB);
    }

    ////////////////////////////////////////
    // Set position vertex, so it is set last.
    for(size_t i = 0; i < mvElementArrays.size(); ++i)
    {
        cVtxBufferGLElementArray *pElement = mvElementArrays[i];
        if(pElement->mType != eVertexBufferElement_Position)
        {
            continue;    //Only set position
        }

        GLenum GLType = GetGLTypeFromVertexFormat(pElement->mFormat);
        int lSize = pElement->mlElementNum;

        glEnableClientState( GetGLArrayFromVertexElement(pElement->mType) );

        const char *offset = (const char*)NULL + mvElementOffsets[i];
        glVertexPointer(lSize, GLType, (GLsizei)mlInterleavedStride, offset);
    }

    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
}

//-----------------------------------------------------------------------

}
