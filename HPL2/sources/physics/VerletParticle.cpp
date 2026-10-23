#include "physics/VerletParticle.h"

#include "physics/PhysicsWorld.h"
#include "physics/PhysicsBody.h"

#include "system/LowLevelSystem.h"

#include "math/Math.h"

namespace hpl
{

//////////////////////////////////////////////////////////////////////////
// PARTICLE RAY CALLBACK
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cVerletParticleRayCallback::cVerletParticleRayCallback(iVerletParticleContainer *apContainer)
{
    mpContainer = apContainer;

    mvIntersectPos = 0;
    mvIntersectNormal = 0;
}

//-----------------------------------------------------------------------

void cVerletParticleRayCallback::Reset()
{
    mfClosestDist = 9999999.0f;
    mbIntersected = false;
}

//-----------------------------------------------------------------------

bool cVerletParticleRayCallback::BeforeIntersect(iPhysicsBody *pBody)
{
    if(pBody->GetCollide() == false)
    {
        return false;
    }

    return mpContainer->CheckParticleBodyCollision(pBody);
}

//-----------------------------------------------------------------------

bool cVerletParticleRayCallback::OnIntersect(iPhysicsBody *pBody, cPhysicsRayParams *apParams)
{
    mbIntersected = true;

    if(apParams->mfDist < mfClosestDist)
    {
        mfClosestDist = apParams->mfDist;
        mvIntersectPos = apParams->mvPoint;
        mvIntersectNormal = apParams->mvNormal;
    }
    return true;
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PARTICLE
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cVerletParticle::cVerletParticle(iVerletParticleContainer *apContainer)
{
    mpContainer = apContainer;

    mvPosition = 0;
    mvPrevPosition = 0;
    mvSmoothPosition = 0;
    mvRenderPrevSmoothPosition = 0;
    mbSmoothPositionValid = false;
    mfInvMass = 1;
}

cVerletParticle::cVerletParticle(iVerletParticleContainer *apContainer, const cVector3f& avPos, float afInvMass)
{
    mpContainer = apContainer;

    mvPosition = avPos;
    mvPrevPosition = avPos;
    mvSmoothPosition = avPos;
    mvRenderPrevSmoothPosition = avPos;
    mbSmoothPositionValid = false;
    mfInvMass = afInvMass;
}

cVerletParticle::~cVerletParticle()
{

}

//-----------------------------------------------------------------------

void cVerletParticle::SetPosition(const cVector3f& avPos, bool abSetPrevPos)
{
    mvPosition = avPos;
    if(abSetPrevPos)
    {
        mvPrevPosition = mvPosition;
    }

    mpContainer->SetSleeping(false);
}

//-----------------------------------------------------------------------

void cVerletParticle::UpdateMovement(double adFixedDelta)
{
    cVector3f vAcc = mfInvMass == 0 ? 0 : mpContainer->mvGravityForce;

    cVector3f vTemp = mvPosition;
    mvPosition += (mvPosition * mpContainer->mfDampingMul - mvPrevPosition * mpContainer->mfDampingMul) + vAcc * (float)adFixedDelta * (float)adFixedDelta;

    mvPrevPosition = vTemp;
}


//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PARTICLE CONTAINER
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

iVerletParticleContainer::iVerletParticleContainer(const tString &asName, iPhysicsWorld *apWorld)
{
    msName = asName;

    mpWorld = apWorld;

    mlUniqueID = -1;

    mpRayParticleCallback = hplNew( cVerletParticleRayCallback, (this));

    mvGravityForce = mpWorld->GetGravity();

    mbCollide = true;

    mfDamping = 0.001f;
    mfDampingMul = 1 - mfDamping;

    mfSlideAmount = 0.3f;
    mfParticleRadius = 0.05f;

    mbSleeping = false;
    mfSleepCheckCount = 0;
    mfSleepCheckTime = 1.0f / 3.0f;
    mlSleepCount = 0;
    mlSleepMaxCount = 3;
    mfSleepCheckSqrLimit = 0.0015f * 0.0015f;

    mlUpdateCount = 0;
}

iVerletParticleContainer::~iVerletParticleContainer()
{

}

//-----------------------------------------------------------------------

void iVerletParticleContainer::SetDamping(float afX)
{
    afX = cMath::Clamp(afX, 0, 1);

    mfDamping = afX;
    mfDampingMul = 1 - mfDamping;
}


//-----------------------------------------------------------------------

void iVerletParticleContainer::SetParticleRadius(float afX)
{
    if(mfParticleRadius == afX)
    {
        return;
    }

    mfParticleRadius = afX;

    SetSleeping(false);
}

//-----------------------------------------------------------------------

void iVerletParticleContainer::SetSleeping(bool abX)
{
    if(mbSleeping == abX)
    {
        return;
    }

    mbSleeping = abX;
    mlSleepCount = 0;
    mfSleepCheckCount = 0;

    SetSpecificDataSleeping(abX);
}

//-----------------------------------------------------------------------

void iVerletParticleContainer::PreUpdate(double adFixedDelta)
{
    ///////////////////////////
    //Sleeping
    if(mbSleeping)
    {
        if(CheckSpecificDataAwake())
        {
            SetSleeping(false);
        }
    }
    ///////////////////////////
    //NOT Sleeping
    if(mbSleeping == false)
    {
        mlUpdateCount++;

        //////////////////////////
        //Update counter and see if time for check
        mfSleepCheckCount += (float)adFixedDelta;
        if(mfSleepCheckCount < mfSleepCheckTime)
        {
            return;
        }

        //////////////////////////
        //Iterate particles and see if all are sleeping
        bool bAllSleeping = true;
        for(tVerletParticleListIt it = mlstParticles.begin(); it != mlstParticles.end(); ++it)
        {
            cVerletParticle *pPart = *it;

            float fSqrSpeed = cMath::Vector3DistSqr(pPart->GetPosition(), pPart->GetPrevPosition());
            if(fSqrSpeed > mfSleepCheckSqrLimit)
            {
                bAllSleeping = false;
                break;
            }
        }

        //////////////////////////
        //Check implementation specific sleeping.
        if(bAllSleeping)
        {
            bAllSleeping = CheckSpecificDataSleeping();
        }

        //////////////////////////
        //Update counter and see if sleeping should start
        if(bAllSleeping)
        {
            mlSleepCount++;
            if(mlSleepMaxCount <= mlSleepCount)
            {
                SetSleeping(true);
            }
        }
        else
        {
            mlSleepCount = 0;
        }
    }
}

//-----------------------------------------------------------------------

void iVerletParticleContainer::UpdateLengthConstraint(cVerletParticle *apP1, cVerletParticle *apP2, float afLength)
{
    cVector3f vDelta =  apP2->mvPosition - apP1->mvPosition;
    float fDist = vDelta.Length();

    float fDiff = (fDist - afLength) / (fDist * (apP1->mfInvMass + apP2->mfInvMass));

    apP1->mvPosition += vDelta * fDiff * apP1->mfInvMass;
    apP2->mvPosition -= vDelta * fDiff * apP2->mfInvMass;
}

//-----------------------------------------------------------------------

void iVerletParticleContainer::UpdateParticleCollisionConstraint(cVerletParticle *apPart, const cVector3f &avPrevPos, float afRadius)
{
    if(mbCollide == false)
    {
        return;
    }

    cVector3f vDiff = apPart->mvPosition - avPrevPos;
    if(vDiff == cVector3f(0, 0, 0))
    {
        return;
    }

    float fLength = vDiff.Length();
    cVector3f vDir = vDiff / fLength;

    mpRayParticleCallback->Reset();

    cVector3f vStart = avPrevPos - vDir * afRadius;
    cVector3f vEnd = apPart->mvPosition + vDir * afRadius;

    mpWorld->CastRay(mpRayParticleCallback, vStart, vEnd, true, true, true, true);

    if(mpRayParticleCallback->mbIntersected)
    {
        apPart->mvPosition = mpRayParticleCallback->mvIntersectPos - vDir * afRadius;
        apPart->mvPrevPosition = apPart->mvPosition + mpRayParticleCallback->mvIntersectNormal * (fLength * mfSlideAmount);
    }
}

//-----------------------------------------------------------------------

}
