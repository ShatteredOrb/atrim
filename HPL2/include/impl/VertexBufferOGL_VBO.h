#ifndef HPL_VERTEXBUFFER_OGL_VBO_H
#define HPL_VERTEXBUFFER_OGL_VBO_H

#include "impl/VertexBufferOpenGL.h"


namespace hpl
{

class cVertexBufferOGL_VBO : public iVertexBufferOpenGL
{
public:
    cVertexBufferOGL_VBO(    iLowLevelGraphics* apLowLevelGraphics,
                             eVertexBufferDrawType aDrawType, eVertexBufferUsageType aUsageType,
                             int alReserveVtxSize, int alReserveIdxSize);
    ~cVertexBufferOGL_VBO();

    void UpdateData(tVertexElementFlag aTypes, bool abIndices);

    void Draw(eVertexBufferDrawType aDrawType = eVertexBufferDrawType_LastEnum);
    void DrawIndices(unsigned int *apIndices, int alCount,
                     eVertexBufferDrawType aDrawType = eVertexBufferDrawType_LastEnum);

    void Bind();
    void UnBind();

private:
    void CompileSpecific();
    iVertexBufferOpenGL *CreateDataCopy(tVertexElementFlag aFlags, eVertexBufferDrawType aDrawType,
                                        eVertexBufferUsageType aUsageType,
                                        int alReserveVtxSize, int alReserveIdxSize);

    void SetVertexStates();

    unsigned int mlElementHandle;

    unsigned int mlInterleavedHandle[2];
    int          mlCurrentInterleaved;
    size_t       mlInterleavedStride;

    std::vector<size_t> mvElementOffsets;

    size_t mlLastUploadedBytes[2];

    std::vector<char> mvInterleavedShadow;

    bool mbUse16BitIndices;
    unsigned int mlIndexBufferSize;
};

};
#endif // HPL_RENDERER3D_OGL_VBO_H
