#include "graphics/Renderable.h"

#include "math/Math.h"
#include "math/Frustum.h"
#include "system/LowLevelSystem.h"

namespace hpl
{

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

iRenderable::iRenderable(const tString &asName) : iEntity3D(asName)
{
    mlLastMatrixCount = -1;

    mbStatic = false;

    mlRenderFlags = eRenderableFlag_VisibleInReflection | eRenderableFlag_VisibleInNonReflection;

    mfIlluminationAmount = 1.0f;
    mfCoverageAmount = 1.0f;

    mlRenderFrameCount = -1;

    mlCalcScaleMatrixCount = -1;
    mvCalcScale = cVector3f(1, 1, 1);

    mbForceShadow = false;

    mbIsOneSided = false;
    mvOneSidedNormal = 0;

    mpModelMatrix = NULL;

    mfViewSpaceZ = 0;

    mbIsVisible = true;

    mlLargePlaneSurfacePlacement = 0;

    mpRenderCallback = NULL;

    mpRenderContainerNode = NULL;

    mpRenderableUserData = NULL;

    mfShaderTimer = 0.0f;
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void iRenderable::SetRenderFlagBit(tRenderableFlag alFlagBit, bool abSet)
{
    if(abSet)
    {
        mlRenderFlags |= alFlagBit;
    }
    else
    {
        mlRenderFlags &= (~alFlagBit);
    }

    if(mpRenderCallback)
    {
        mpRenderCallback->OnRenderFlagsChange(this);
    }
}

//-----------------------------------------------------------------------

void iRenderable::SetVisible(bool abVisible)
{
    mbIsVisible = abVisible;

    OnChangeVisible();

    if(mpRenderCallback)
    {
        mpRenderCallback->OnVisibleChange(this);
    }
}

//-----------------------------------------------------------------------

cMatrixf *iRenderable::GetInvModelMatrix()
{
    cMatrixf *pModelMatrix = GetModelMatrix(NULL);
    if(pModelMatrix == NULL)
    {
        return NULL;
    }

    if(mlLastMatrixCount != GetMatrixUpdateCount())
    {
        mlLastMatrixCount = GetMatrixUpdateCount();

        m_mtxInvModel = cMath::MatrixInverse(*pModelMatrix);
    }

    return &m_mtxInvModel;
}

//-----------------------------------------------------------------------

void iRenderable::SetCoverageAmount(float afX)
{
    afX = cMath::Clamp(afX, 0.0f, 1.0f);

    if(mfCoverageAmount == afX)
    {
        return;
    }

    mfCoverageAmount = afX;

    //A little hack so that shadows are updated
    SetTransformUpdated(false);
}

//-----------------------------------------------------------------------

const cVector3f &iRenderable::GetCalcScale()
{
    cMatrixf *pModelMatrix = GetModelMatrix(NULL);

    if(pModelMatrix != NULL && mlCalcScaleMatrixCount != GetMatrixUpdateCount())
    {
        mlCalcScaleMatrixCount = GetMatrixUpdateCount();
        mvCalcScale.x = pModelMatrix->GetRight().Length();
        mvCalcScale.y = pModelMatrix->GetUp().Length();
        mvCalcScale.z = pModelMatrix->GetForward().Length();
    }

    return mvCalcScale;
}

//-----------------------------------------------------------------------

bool iRenderable::CollidesWithBV(cBoundingVolume *apBV)
{
    return cMath::CheckBVIntersection(*GetBoundingVolume(), *apBV);
}

//-----------------------------------------------------------------------

bool iRenderable::CollidesWithFrustum(cFrustum *apFrustum)
{
    return apFrustum->CollideBoundingVolume(GetBoundingVolume()) != eCollision_Outside;
}

//-----------------------------------------------------------------------
}
