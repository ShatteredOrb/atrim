#include "LuxHandObject_LightSource.h"

#include "LuxMap.h"
#include "LuxPlayer.h"
#include "LuxPlayerHands.h"
#include "LuxEnemy.h"
#include "LuxMapHelper.h"
#include "LuxMapHandler.h"
#include "LuxHelpFuncs.h"
#include "LuxDebugHandler.h"
#include <limits>

//-----------------------------------------------------------------------

// When the player is right handed the hands entity is mirrored on X (see cLuxPlayerHands::UpdatePlayerHandsPos).
// Lights parented to it must be mirrored too, otherwise their world matrix gets a negative determinant
// and the spotlight frustum / culling breaks (lantern light disappears).
static cMatrixf GetHandOrientedLightMatrix(const cMatrixf& a_mtxLocal)
{
    if(gpBase->mpPlayer->GetHandOrientation() == eLuxHandOrientation_Right)
    {
        return cMath::MatrixMul(a_mtxLocal, cMath::MatrixScale(cVector3f(-1.0f, 1.0f, 1.0f)));
    }
    return a_mtxLocal;
}

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxHandObject_LightSource::cLuxHandObject_LightSource(const tString& asName, cLuxPlayerHands *apHands) : iLuxHandObject(asName, apHands)
{
    mfSwayAngle = 0;
    mfSwayVel = 0;

    mbFlickering = false;
    mfFlickeringSpeed = 1.0f;
    mlFlickeringState = 1;

    mfFlickerAmount = 1;
    mfFlickerTime = 0;
    mfFlickerPauseTime = 0;
    mfLastAppliedFlicker = 1.0f;

    mfStrobeDroneTime=0;
    mpDroneSound = NULL;
    mlDroneSoundId = -1;
}

cLuxHandObject_LightSource::~cLuxHandObject_LightSource()
{
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxHandObject_LightSource::LoadImplementedVars(cXmlElement *apVarsElem)
{
    float fFadeInTime = apVarsElem->GetAttributeFloat("FadeInTime", 0);
    float fFadeOutTime = apVarsElem->GetAttributeFloat("FadeOutTime", 0);
    if(fFadeInTime == 0)
    {
        fFadeInTime = 0.001f;
    }
    if(fFadeOutTime == 0)
    {
        fFadeOutTime = 0.001f;
    }
    mfFadeInSpeed = 1.0f / fFadeInTime;
    mfFadeOutSpeed = 1.0f / fFadeOutTime;

    mbHasSwayPhysics = apVarsElem->GetAttributeBool("HasSwayPhysics", false);
    mfMaxSwayVel = apVarsElem->GetAttributeFloat("MaxSwayVel", 0);
    mvSwayAngleLimits = cMath::Vector2ToRad(apVarsElem->GetAttributeVector2f("SwayAngleLimits", 0));
    mvSwayDownAngleLimits = cMath::Vector2ToRad(apVarsElem->GetAttributeVector2f("SwayDownAngleLimits", 0));

    mfSwayGravity = apVarsElem->GetAttributeFloat("SwayGravity", 0);
    mfSwayFriction = apVarsElem->GetAttributeFloat("SwayFriction", 0);
    mvSwayPinDir = apVarsElem->GetAttributeVector3f("SwayPinDir", 1);
    mvSwayPinDir.Normalize();
    mfSwayPlayerSpeedMul = apVarsElem->GetAttributeFloat("SwayPlayerSpeedMul", 0);
    mfSwayCameraRollMul = apVarsElem->GetAttributeFloat("SwayCameraRollMul", 0);

    msSkipSwaySubMesh = apVarsElem->GetAttributeString("SkipSwaySubMesh", "");
}

//-----------------------------------------------------------------------

void cLuxHandObject_LightSource::ImplementedCreateEntity(cLuxMap *apMap)
{
    mvDefaultLightColors.resize(mvLights.size());
    mvDefaultLightFlicker.resize(mvLights.size());
    mvLightFadeOutColor.resize(mvLights.size());

    if(mvDefaultLightMatrix.empty())
    {
        mvDefaultLightMatrix.resize(mvLights.size());

        for(size_t i = 0; i < mvLights.size(); ++i)
        {
            mvDefaultLightMatrix[i] = mvLights[i]->GetLocalMatrix();
        }
    }

    for(size_t i = 0; i < mvLights.size(); ++i)
    {
        mvDefaultLightColors[i]  = mvLights[i]->GetDiffuseColor();
        mvDefaultLightFlicker[i] = mvLights[i]->GetFlickerActive();
        mvLights[i]->SetFlickerActive(false);
    }

    mvDefaultSubMeshMatrix.resize(mpMeshEntity->GetSubMeshEntityNum());
    for(int i = 0; i < mpMeshEntity->GetSubMeshEntityNum(); ++i)
    {
        cSubMeshEntity *pSubEnt = mpMeshEntity->GetSubMeshEntity((unsigned int)i);
        mvDefaultSubMeshMatrix[i] = pSubEnt->GetLocalMatrix();
    }

    mvDefaultBillboardMatrix.resize(mvBillboards.size());
    for(size_t i = 0; i < mvBillboards.size() && i < mvDefaultBillboardMatrix.size(); ++i)
    {
        mvDefaultBillboardMatrix[i] = mvBillboards[i]->GetLocalMatrix();
    }
}

//-----------------------------------------------------------------------

void cLuxHandObject_LightSource::ImplementedDestroyEntity(cLuxMap *apMap)
{
    StopDroneSound();
}

//-----------------------------------------------------------------------

void cLuxHandObject_LightSource::ImplementedReset()
{
    mfSwayAngle = 0;
    mfSwayVel = 0;

    mfStrobeDroneTime = 0;
    mfFlickerAmount = 1;
    mfLastAppliedFlicker = 1.0f;
    StopDroneSound();
}

//-----------------------------------------------------------------------

void cLuxHandObject_LightSource::StopDroneSound()
{
    if(mpDroneSound)
    {
        cSoundHandler *pSoundHandler = gpBase->mpEngine->GetSound()->GetSoundHandler();
        if(pSoundHandler->IsValid(mpDroneSound, mlDroneSoundId))
        {
            mpDroneSound->Stop();
        }
        mpDroneSound = NULL;
    }
    mlDroneSoundId = -1;
}

//-----------------------------------------------------------------------

void cLuxHandObject_LightSource::Update(double adFixedDelta)
{
    float fAlpha = 1.0f;

    bool bUpdate = false;
    bool bUpdateDone = false;

    for(size_t i = 0; i < mvLights.size() && i < mvDefaultLightMatrix.size(); ++i)
    {
        mvLights[i]->SetMatrix(GetHandOrientedLightMatrix(mvDefaultLightMatrix[i]));
    }

    ///////////////////
    // Sway Physics
    if(mbHasSwayPhysics)
    {
        UpdateSwayPhysics(adFixedDelta);
    }

    ///////////////////
    // Fade out
    if(mpHands->GetState() == eLuxHandsState_Holster)
    {
        for(size_t i = 0; i < mvLights.size(); ++i)
        {
            if(mvDefaultLightFlicker[i])
            {
                mvLights[i]->SetFlickerActive(false);
                mvLights[i]->StopFading();
            }
        }

        mpHands->mfHandObjectAlpha -= mfFadeOutSpeed * (float)adFixedDelta;
        if(mpHands->mfHandObjectAlpha < 0.0f)
        {
            mpHands->mfHandObjectAlpha = 0.0f;
        }

        if(mpDroneSound)
        {
            cSoundHandler *pSoundHandler = gpBase->mpEngine->GetSound()->GetSoundHandler();

            if(pSoundHandler->IsValid(mpDroneSound, mlDroneSoundId))
            {
                mpDroneSound->FadeOut(0.5f);
            }
            mpDroneSound = NULL;
        }

        bUpdate = true;
    }
    ///////////////////
    // Fade in
    else if(mpHands->mfHandObjectAlpha < 1.0f)
    {
        mpHands->mfHandObjectAlpha += mfFadeInSpeed * (float)adFixedDelta;
        if(mpHands->mfHandObjectAlpha > 1.0f)
        {
            mpHands->mfHandObjectAlpha = 1.0f;
            bUpdateDone = true;
        }

        bUpdate = true;
    }

    ///////////////////
    // Flickering
    float fFlicker = UpdateFlickering(adFixedDelta);

    ///////////////////
    // Set alpha
    // Flicker must also be applied after the fade in is done, otherwise the
    // strobe / script flicker is never visible. Only touch colors when something
    // changed so the engine's own light flicker (mvDefaultLightFlicker) is left alone.
    fAlpha = mpHands->mfHandObjectAlpha * fFlicker;
    if(fFlicker != mfLastAppliedFlicker)
    {
        bUpdate = true;
        mfLastAppliedFlicker = fFlicker;
    }

    ///////////////////
    // Calculate fade out color
    if(mpHands->GetState() != eLuxHandsState_Holster)
    {
        for(size_t i = 0; i < mvLights.size(); ++i)
        {
            mvLightFadeOutColor[i] = mvLights[i]->GetDiffuseColor();
        }
    }


    ///////////////////
    // Set alpha
    if(bUpdate)
    {
        if(mpMeshEntity)
        {
            mpMeshEntity->SetIlluminationAmount(fAlpha);
        }

        for(size_t i = 0; i < mvBillboards.size() && i < mvDefaultBillboardMatrix.size(); ++i)
        {
            cColor col = mvBillboards[i]->GetColor();
            col.a = fAlpha;
            mvBillboards[i]->SetColor(col);
        }

        for(size_t i = 0; i < mvParticleSystems.size(); ++i)
        {
            cColor col = mvParticleSystems[i]->GetColor();
            col.a = fAlpha;
            mvParticleSystems[i]->SetColor(col);
        }

        for(size_t i = 0; i < mvLights.size(); ++i)
        {
            if(mpHands->GetState() == eLuxHandsState_Holster)
            {
                mvLights[i]->SetDiffuseColor(mvLightFadeOutColor[i] * fAlpha);
            }
            else
            {
                mvLights[i]->SetDiffuseColor(mvDefaultLightColors[i] * fAlpha);
            }
        }
    }

    if(bUpdateDone)
    {
        for(size_t i = 0; i < mvLights.size(); ++i)
        {
            if(mvDefaultLightFlicker[i])
            {
                mvLights[i]->SetFlickerActive(true);
            }
        }
    }
}

//-----------------------------------------------------------------------

bool cLuxHandObject_LightSource::DoAction(eLuxPlayerAction aAction, bool abPressed)
{
    return false;
}

//-----------------------------------------------------------------------

bool cLuxHandObject_LightSource::AnimationIsOver()
{
    return true;
}

//-----------------------------------------------------------------------

void cLuxHandObject_LightSource::SetFlickering(bool abX)
{
    mbFlickering = abX;

    if(mbFlickering)
    {
        mfFlickerAmount = 1;
        mfFlickerTime = 0.05f;
        mlFlickeringState = 1;
        mfFlickerPauseTime=0;
    }
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxHandObject_LightSource::UpdateSwayPhysics(double adFixedDelta)
{
    /////////////////////////////
    // Update vel
    {
        /////////////////////////////
        // Gravity
        cCamera *pCam = gpBase->mpPlayer->GetCamera();
        float fDownAngle = pCam->GetPitch() * -1.0f;
        fDownAngle += pCam->GetRoll() * mfSwayCameraRollMul;
        fDownAngle = cMath::Clamp(fDownAngle, mvSwayDownAngleLimits.x, mvSwayDownAngleLimits.y);

        float fAngle = mfSwayAngle - fDownAngle;

        float fForce = -sin(fAngle) * mfSwayGravity;
        mfSwayVel += fForce * (float)adFixedDelta;

        /////////////////////////////
        // Player velocity
        iCharacterBody *pCharBody = gpBase->mpPlayer->GetCharacterBody();
        float fPlayerSpeed = pCharBody->GetVelocity(gpBase->mpEngine->GetFixedDelta()).Length();
        if(pCharBody->GetMoveSpeed(eCharDir_Forward) < 0)
        {
            fPlayerSpeed = -fPlayerSpeed;
        }

        mfSwayVel += -fPlayerSpeed * mfSwayPlayerSpeedMul;

        /////////////////////////////
        // Cap and Friction
        if(mfSwayVel > mfMaxSwayVel)
        {
            mfSwayVel = mfMaxSwayVel;
        }
        if(mfSwayVel < -mfMaxSwayVel)
        {
            mfSwayVel = -mfMaxSwayVel;
        }

        mfSwayVel -= mfSwayVel * mfSwayFriction * (float)adFixedDelta;
    }

    /////////////////////////////
    // Update Angle
    mfSwayAngle += mfSwayVel * (float)adFixedDelta;

    //Min
    if(mfSwayAngle < mvSwayAngleLimits.x)
    {
        mfSwayVel = 0;
        mfSwayAngle = mvSwayAngleLimits.x;
    }

    //Max
    if(mfSwayAngle > mvSwayAngleLimits.y)
    {
        mfSwayVel = 0;
        mfSwayAngle = mvSwayAngleLimits.y;
    }

    /////////////////////////////
    // Update Model matrix
    cMatrixf mtxSway = cMath::MatrixRotate(mvSwayPinDir * mfSwayAngle, eEulerRotationOrder_XYZ);
    cMatrixf mtxSwayLight = cMath::MatrixRotate(mvSwayPinDir * mfSwayAngle * 0.125f, eEulerRotationOrder_XYZ);

    if ( mpMeshEntity )
    {
        for(int i=0; i<mpMeshEntity->GetSubMeshEntityNum(); ++i)
        {
            cSubMeshEntity *pSubEnt = mpMeshEntity->GetSubMeshEntity(i);
            if(pSubEnt->GetSubMesh()->GetName() == msSkipSwaySubMesh)
            {
                continue;
            }
            //Log("'%s'\n",pSubEnt->GetSubMesh()->GetName().c_str());

            pSubEnt->SetMatrix(cMath::MatrixMul(mtxSway, mvDefaultSubMeshMatrix[i]) );
        }
        mpMeshEntity->SetMatrix(m_mtxOffset);
    }

    for(size_t i=0; i<mvLights.size() && i<mvDefaultLightMatrix.size(); ++i)
    {
        //Must keep the right hand mirroring, otherwise the light breaks when hand orientation is Right.
        mvLights[i]->SetMatrix(GetHandOrientedLightMatrix(cMath::MatrixMul(mtxSwayLight, mvDefaultLightMatrix[i])) );
    }

    for(size_t i = 0; i < mvBillboards.size() && i < mvDefaultBillboardMatrix.size(); ++i)
    {
        mvBillboards[i]->SetMatrix(cMath::MatrixMul(mtxSway, mvDefaultBillboardMatrix[i]) );
    }
    // Update
}

//----------------------------------------------------------------------

float cLuxHandObject_LightSource::UpdateFlickering(double adFixedDelta)
{
    if(mpHands->GetState() == eLuxHandsState_Holster)
    {
        return 1.0f;
    }

    ////////////////////////////
    // See if an enemy is in the spotlight.
    bool bEnemyInSpotLight = false;
    cLuxMap* pMap = gpBase->mpMapHandler->GetCurrentMap();
    if(pMap == NULL)
    {
        StopDroneSound();
        return 1.0f;
    }
    cLuxEnemyIterator it = pMap->GetEnemyIterator();
    while(it.HasNext())
    {
        iLuxEnemy* pEnemy = it.Next();
        if(pEnemy->IsActive()==false)
        {
            continue;
        }
        if(pEnemy->GetInLanternLightCount()>0)
        {
            bEnemyInSpotLight=true;
            break;
        }
    }

    ////////////////////////////
    // Strobe drone
    cSoundHandler *pSoundHandler = gpBase->mpEngine->GetSound()->GetSoundHandler();

    if(bEnemyInSpotLight)
    {
        mfStrobeDroneTime = 1.0f;
    }
    if(mfStrobeDroneTime>0)
    {
        mfStrobeDroneTime -= (float)adFixedDelta;

        if(mpDroneSound==NULL || pSoundHandler->IsValid(mpDroneSound, mlDroneSoundId)==false)
        {
            mpDroneSound = pSoundHandler->PlayGui("ui_lanterndrone.ogg", true, 1);
            if(mpDroneSound)
            {
                mlDroneSoundId = mpDroneSound->GetId();
                mpDroneSound->FadeIn(1.0f, 0.5f);
            }
        }
    }
    else
    {
        if(mpDroneSound)
        {
            if(pSoundHandler->IsValid(mpDroneSound, mlDroneSoundId))
            {
                mpDroneSound->FadeOut(0.5f);
            }
            mpDroneSound = NULL;
        }
    }

    ////////////////////////////
    // If In spotlight, do a strobing effect
    if(bEnemyInSpotLight)
    {
        //Make sure within bounds
        if(mfFlickerTime<0.01)
        {
            mfFlickerTime = 0.025f;
        }
        if(mfFlickerTime>0.1)
        {
            mfFlickerTime = 0.1f;
        }

        //Puase
        if(mfFlickerPauseTime>0)
        {
            mfFlickerPauseTime -= (float)adFixedDelta;
            return mfFlickerAmount;
        }

        ////////////////////////////
        // Fade in
        if(mlFlickeringState==1)
        {
            mfFlickerAmount += (float)adFixedDelta*(1.0f/mfFlickerTime);
            if(mfFlickerAmount>=1)
            {
                mfFlickerAmount=1;
                mfFlickerTime = cMath::RandRectf(0.1f, 1.0f);
                mlFlickeringState=0;
                gpBase->mpHelpFuncs->PlayGuiSoundData("lantern_flicker_strobe", eSoundEntryType_World);
            }
        }
        ////////////////////////////
        // Fade out
        else if(mlFlickeringState==0)
        {
            mfFlickerAmount -= (float)adFixedDelta*(1.0f/mfFlickerTime);
            if(mfFlickerAmount<=0)
            {
                if(cMath::RandRectl(0, 5)==0)
                {
                    mfFlickerPauseTime = cMath::RandRectf(0, 1);
                }

                mfFlickerAmount=0;
                mfFlickerTime = cMath::RandRectf(0.025f, 0.2f);
                mlFlickeringState=1;
                gpBase->mpHelpFuncs->PlayGuiSoundData("lantern_flicker_strobe", eSoundEntryType_World);
            }
        }

        return mfFlickerAmount;
    }

    ////////////////////////////
    // Check if active
    if(mbFlickering==false)
    {
        mfFlickerAmount += (float)adFixedDelta*12;
        if(mfFlickerAmount>=1)
        {
            mfFlickerAmount=1;
        }
        return mfFlickerAmount;
    }

    ////////////////////////////
    // Fade in
    if(mlFlickeringState==1 && mfFlickerAmount<1)
    {
        mfFlickerAmount += (float)adFixedDelta*35;
        if(mfFlickerAmount>=1)
        {
            mfFlickerAmount=1;
        }
    }
    ////////////////////////////
    // Fade out
    else if(mlFlickeringState==0 && mfFlickerAmount>0)
    {
        mfFlickerAmount -= (float)adFixedDelta*35;
        if(mfFlickerAmount<=0)
        {
            mfFlickerAmount=0;
        }
    }
    ////////////////////////////
    // Check of time for state switch
    else
    {
        mfFlickerTime -= (float)adFixedDelta*mfFlickeringSpeed;
        if(mfFlickerTime<=0)
        {
            ///////////////////////////
            // Off -> On
            if(mlFlickeringState == 1)
            {
                mfFlickerTime = cMath::RandRectf(0.0f, 0.15f);
                mlFlickeringState =0;
                gpBase->mpHelpFuncs->PlayGuiSoundData("lantern_flicker", eSoundEntryType_World);
            }
            ///////////////////////////
            // On -> Off
            else
            {
                mfFlickerTime = cMath::RandRectf(0.1f, 0.8f);
                mlFlickeringState =1;
                gpBase->mpHelpFuncs->PlayGuiSoundData("lantern_flicker", eSoundEntryType_World);
            }
        }
    }

    return mfFlickerAmount;
}

//----------------------------------------------------------------------
