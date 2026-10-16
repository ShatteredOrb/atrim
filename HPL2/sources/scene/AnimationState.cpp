#include "scene/AnimationState.h"

#include "math/Math.h"
#include "graphics/Animation.h"
#include "resources/AnimationManager.h"

#include "system/LowLevelSystem.h"
#include "scene/MeshEntity.h"
#include "graphics/Mesh.h"
#include "graphics/AnimationTrack.h"
#include "graphics/Skeleton.h"
#include "graphics/Bone.h"

#include "scene/Node3D.h"
#include "graphics/BoneState.h"


namespace hpl
{

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cAnimationState::cAnimationState(cAnimation* apAnimation, const tString &asName,
                                 cAnimationManager *apAnimationManager)
{
    mpAnimation = apAnimation;

    mpAnimationManager = apAnimationManager;

    mfLength = mpAnimation->GetLength();
    msName = asName;

    mbActive = false;

    mfTimePos = 0;
    mfWeight = 1;
    mfSpeed = 1.0f;
    mfBaseSpeed = 1.0f;
    mfTimePos = 0;
    mfPrevTimePos = 0;

    mbLoop = false;
    mbPaused = false;

    mfSpecialEventTime = 0;

    mfFadeStep = 0;

    mfFadeSpeed = 0;

    mbCanBlend = true;
    mvSkeletonBounds.clear();
}

//-----------------------------------------------------------------------

cAnimationState::~cAnimationState()
{
    STLDeleteAll(mvEvents);

    //if no animation manger, then it means that the animation should not be destroyed by the state.
    if(mpAnimationManager)
    {
        mpAnimationManager->Destroy(mpAnimation);
    }
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cAnimationState::Update(double adFixedDelta)
{
    //Update animation
    AddTimePosition((float)adFixedDelta);

    //Fading
    if(mfFadeStep != 0)
    {
        mfWeight += mfFadeStep * (float)adFixedDelta;

        if(mfFadeStep < 0 && mfWeight <= 0)
        {
            mfWeight = 0;
            mbActive = false;
            mfFadeStep = 0;
        }
        else if(mfFadeStep > 0 && mfWeight >= 1)
        {
            mfWeight = 1;
            mfFadeStep = 0;
        }
    }

    if(mfFadeSpeed != 0)
    {
        mfSpeed += mfFadeSpeed * (float)adFixedDelta;

        if(mfSpeed < 0)
        {
            mfSpeed = 0;
            mfFadeSpeed = 0;
        }
        else if(mfSpeed > 1.0f)
        {
            mfSpeed = 1;
            mfFadeSpeed = 0;
        }
    }
}

//-----------------------------------------------------------------------

bool cAnimationState::IsFading()
{
    return mfFadeStep != 0;
}


//-----------------------------------------------------------------------

bool cAnimationState::IsOver()
{
    if(mbLoop)
    {
        return false;
    }

    return mfTimePos >= mfLength;
}

//-----------------------------------------------------------------------

void cAnimationState::FadeIn(float afTime)
{
    mfFadeStep = 1.0f / std::abs(afTime);
}

void cAnimationState::FadeOut(float afTime)
{
    mfFadeStep = -1.0f / std::abs(afTime);
}

//-----------------------------------------------------------------------

void cAnimationState::FadeInSpeed(float afTime)
{
    if(afTime == 0.0f)
    {
        mfSpeed = 1;
        mfFadeSpeed = 0;
    }
    else
    {
        mfFadeSpeed = 1.0f / cMath::Abs(afTime);
    }
}

void cAnimationState::FadeOutSpeed(float afTime)
{
    if(afTime == 0.0f)
    {
        mfSpeed = 0;
        mfFadeSpeed = 0;
    }
    else
    {
        mfFadeSpeed = -1.0f / cMath::Abs(afTime);
    }
}

//-----------------------------------------------------------------------

void cAnimationState::SetLength(float afLength)
{
    mfLength = afLength;
}

float cAnimationState::GetLength()
{
    return mfLength;
}

//-----------------------------------------------------------------------


void cAnimationState::SetWeight(float afWeight)
{
    mfWeight = afWeight;
}
float cAnimationState::GetWeight()
{
    return mfWeight;
}

//-----------------------------------------------------------------------

void cAnimationState::SetSpeed(float afSpeed)
{
    mfSpeed = afSpeed;
}
float cAnimationState::GetSpeed()
{
    return mfSpeed;
}

//-----------------------------------------------------------------------

void cAnimationState::SetBaseSpeed(float afSpeed)
{
    mfBaseSpeed = afSpeed;
}
float cAnimationState::GetBaseSpeed()
{
    return mfBaseSpeed;
}

//-----------------------------------------------------------------------

void cAnimationState::SetTimePosition(float afPosition)
{
    if(mbLoop)
    {
        mfTimePos = cMath::Wrap(afPosition, 0, mfLength);
    }
    else
    {
        mfTimePos = cMath::Clamp(afPosition, 0, mfLength);
    }

    mfPrevTimePos = mfTimePos;
}

float cAnimationState::GetTimePosition()
{
    return mfTimePos;
}

float cAnimationState::GetPreviousTimePosition()
{
    return mfPrevTimePos;
}

//-----------------------------------------------------------------------

void cAnimationState::SetRelativeTimePosition(float afPosition)
{
    SetTimePosition(afPosition * mfLength);
}

float cAnimationState::GetRelativeTimePosition()
{
    return mfTimePos / mfLength;
}

//-----------------------------------------------------------------------

bool cAnimationState::IsActive()
{
    return mbActive;
}
void cAnimationState::SetActive(bool abActive)
{
    if(mbActive == abActive)
    {
        return;
    }

    mbActive = abActive;

    //Should this really be here?
    mbPaused = false;
    mfFadeStep = 0;
}

//-----------------------------------------------------------------------

bool cAnimationState::IsLooping()
{
    return mbLoop;
}
void cAnimationState::SetLoop(bool abLoop)
{
    mbLoop = abLoop;
}

//-----------------------------------------------------------------------

bool cAnimationState::IsPaused()
{
    return mbPaused;
}

void cAnimationState::SetPaused(bool abPaused)
{
    mbPaused = abPaused;
}

//-----------------------------------------------------------------------

bool cAnimationState::IsAfterSpecialEvent()
{
    return mfTimePos > mfSpecialEventTime;
}

bool cAnimationState::IsBeforeSpecialEvent()
{
    return mfTimePos < mfSpecialEventTime;
}

//-----------------------------------------------------------------------

void cAnimationState::AddTimePosition(float afAdd)
{
    if(mbPaused)
    {
        return;
    }

    mfPrevTimePos = mfTimePos;

    mfTimePos += afAdd * mfSpeed * mfBaseSpeed;

    if(mbLoop)
    {
        mfTimePos = cMath::Wrap(mfTimePos, 0, mfLength);
    }
    else
    {
        mfTimePos = cMath::Clamp(mfTimePos, 0, mfLength);
    }
}

//-----------------------------------------------------------------------

cAnimation *cAnimationState::GetAnimation()
{
    return mpAnimation;
}

//-----------------------------------------------------------------------

void cAnimationState::CreateSkeletonBoundsFromMesh(cMeshEntity * apMesh, tBoneStateVec * apvBoneStates)
{
    if(mpAnimation == NULL || apMesh == NULL || apvBoneStates == NULL)
    {
        return;
    }

    if(mvSkeletonBounds.size() != 0)
    {
        return;
    }

    if(apvBoneStates->empty())
    {
        return;
    }

    float fDeltaTime = 1.0f / 30.0f;

    // Find the smallest delta time
    for(int i = 0; i < mpAnimation->GetTrackNum(); i++)
    {
        cAnimationTrack *pTrack = mpAnimation->GetTrack(i);
        if(pTrack == NULL || pTrack->GetKeyFrameNum() < 2)
        {
            continue;
        }

        cKeyFrame* pKeyframe = pTrack->GetKeyFrame(0);
        if(pKeyframe == NULL)
        {
            continue;
        }

        for(int k = 1; k < pTrack->GetKeyFrameNum(); k++)
        {
            cKeyFrame* pNextKeyframe = pTrack->GetKeyFrame(k);
            if(pNextKeyframe == NULL)
            {
                continue;
            }

            float fDiff = pNextKeyframe->time - pKeyframe->time;
            if(fDiff > 0.0f)
            {
                fDeltaTime = cMath::Min(fDeltaTime, fDiff);
            }

            pKeyframe = pNextKeyframe;
        }
    }

    fDeltaTime = cMath::Max(1.0f / 100.0f, fDeltaTime);

    cMesh * pMesh = apMesh->GetMesh();
    if(pMesh == NULL || pMesh->GetSkeleton() == NULL)
    {
        return;
    }

    cMatrixf mtxWorldMatrix = apMesh->GetWorldMatrix();
    apMesh->SetWorldMatrix(cMatrixf::Identity);

    const size_t lBoneSize = apvBoneStates->size();

    for(float fTimePosition = 0.0f; fTimePosition <= GetLength() + 0.0001f; fTimePosition += fDeltaTime)
    {
        // Reset bone states
        for(size_t i = 0; i < lBoneSize; i++)
        {
            cNode3D *pState = (*apvBoneStates)[i];
            cBone* pBone = pMesh->GetSkeleton()->GetBoneByIndex((int)i);
            if(pState && pBone && pState->IsActive())
            {
                pState->SetMatrix(pBone->GetLocalTransform(), false);
            }
        }

        // Apply tracks ONLY if the node index is valid
        for(int i = 0; i < mpAnimation->GetTrackNum(); i++)
        {
            cAnimationTrack *pTrack = mpAnimation->GetTrack(i);
            if(pTrack == NULL)
            {
                continue;
            }

            int lNodeIdx = pTrack->GetNodeIndex();
            if(lNodeIdx < 0 || (size_t)lNodeIdx >= lBoneSize)
            {
                continue;
            }

            cNode3D* pState = (*apvBoneStates)[lNodeIdx];
            if(pState)
            {
                pTrack->ApplyToNode(pState, fTimePosition, 1.0f, true);
            }
        }

        for(size_t i = 0; i < lBoneSize; i++)
        {
            (*apvBoneStates)[i]->UpdateMatrix(false);
        }

        int lSkeletonBoundIdx = int(fTimePosition * 4.0f);
        if(lSkeletonBoundIdx < 0)
        {
            lSkeletonBoundIdx = 0;
        }

        if((int)mvSkeletonBounds.size() <= lSkeletonBoundIdx)
        {
            cVector3f vPos = (*apvBoneStates)[0]->GetWorldPosition();
            cSkeletonAABB bounds = cSkeletonAABB(vPos, vPos);
            bounds.SetTime(fTimePosition);
            mvSkeletonBounds.push_back(bounds);
        }

        // Expand AABB
        for(size_t i = 0; i < lBoneSize; i++)
        {
            float fBoundingRadius = pMesh->GetBoneBoundingRadius((int)i);
            cVector3f vPos = (*apvBoneStates)[i]->GetWorldPosition();
            cVector3f vMaxPos = vPos + cVector3f(fBoundingRadius);
            cVector3f vMinPos = vPos - cVector3f(fBoundingRadius);
            mvSkeletonBounds[lSkeletonBoundIdx].Expand(vMinPos, vMaxPos);
        }
    }

    apMesh->SetWorldMatrix(mtxWorldMatrix);
}

bool cAnimationState::TryGetBoundingVolumeAtTime(float afTime, cVector3f & avMin, cVector3f & avMax)
{
    int lIdx = int(afTime * 4.0f);

    //////////////////////////
    // returns the precalculated bounding volume at this time
    if((int)mvSkeletonBounds.size() > lIdx)
    {
        avMax = mvSkeletonBounds[lIdx].mvMax;
        avMin = mvSkeletonBounds[lIdx].mvMin;

        return true;
    }

    return false;
}

//-----------------------------------------------------------------------

cAnimationEvent *cAnimationState::CreateEvent()
{
    cAnimationEvent *pEvent = hplNew( cAnimationEvent, () );
    pEvent->mfTime = 0;
    pEvent->mType = eAnimationEventType_LastEnum;
    pEvent->msValue = "";
    mvEvents.push_back(pEvent);

    return pEvent;
}

cAnimationEvent *cAnimationState::GetEvent(int alIdx)
{
    return mvEvents[alIdx];
}

int cAnimationState::GetEventNum()
{
    return (int)mvEvents.size();
}

//-----------------------------------------------------------------------

void cAnimationState::AddTransition(int alAnimId, int alPreviousAnimId, float afMinTime, float afMaxTime)
{
    mvTransitions.push_back(cAnimationTransition(alAnimId, alPreviousAnimId, afMinTime, afMaxTime));
}

cAnimationTransition *cAnimationState::GetTransitionFromPrevAnim(int alPreviousAnimId, float afPreviousTimePos)
{
    cAnimationTransition *pTransOut = NULL;

    for(size_t i = 0; i < mvTransitions.size(); ++i)
    {
        cAnimationTransition &trans = mvTransitions[i];

        if(trans.mlPreviousAnimId == alPreviousAnimId || trans.mlPreviousAnimId < 0)
        {
            //Check if within time limits
            if(trans.mfMinTime >= 0 && trans.mfMaxTime >= 0 && afPreviousTimePos >= 0)
            {
                if(afPreviousTimePos < trans.mfMinTime || afPreviousTimePos > trans.mfMaxTime)
                {
                    continue;
                }
            }

            pTransOut = &trans;
            if(trans.mlPreviousAnimId >= 0 || alPreviousAnimId < 0)
            {
                break;    //if not a default transition, we know we got the right one.
            }
        }
    }

    return pTransOut;
}

cAnimationTransition *cAnimationState::GetTransition(int alIdx)
{
    return &mvTransitions[alIdx];
}

int cAnimationState::GetTransitionNum()
{
    return (int)mvTransitions.size();
}

//-----------------------------------------------------------------------
}
