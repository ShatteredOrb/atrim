#include "LuxPlayerState_DefaultBase.h"

#include "LuxPlayer.h"
#include "LuxMapHelper.h"
#include "LuxProp.h"
#include "LuxMoveState.h"
#include "LuxEffectRenderer.h"
#include "LuxPlayerHands.h"



//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

iLuxPlayerState_DefaultBase::iLuxPlayerState_DefaultBase(cLuxPlayer *apPlayer, eLuxPlayerState aType) : iLuxPlayerState(apPlayer, aType)
{
    mbCurrentEntityHasOutline = false;

    /////////////////////////////
    //Load crosshair gfx
    cGui *pGui = gpBase->mpEngine->GetGui();

    mvFocusCrosshairGfx.resize(eLuxFocusCrosshair_LastEnum, NULL);
    mvFocusCrosshairGfx[eLuxFocusCrosshair_Grab] = pGui->CreateGfxImage("hud_crosshair_over_grab.tga", eGuiMaterial_Alpha);
    mvFocusCrosshairGfx[eLuxFocusCrosshair_Push] = pGui->CreateGfxImage("hud_crosshair_over_push.tga", eGuiMaterial_Alpha);
    mvFocusCrosshairGfx[eLuxFocusCrosshair_Ignite] = pGui->CreateGfxImage("hud_crosshair_over_ignite.tga", eGuiMaterial_Alpha);
    mvFocusCrosshairGfx[eLuxFocusCrosshair_Pick] = pGui->CreateGfxImage("hud_crosshair_over_pick.tga", eGuiMaterial_Alpha);
    mvFocusCrosshairGfx[eLuxFocusCrosshair_LevelDoor] = pGui->CreateGfxImage("hud_crosshair_over_leveldoor.tga", eGuiMaterial_Alpha);
    mvFocusCrosshairGfx[eLuxFocusCrosshair_Ladder] = pGui->CreateGfxImage("hud_crosshair_over_ladder.tga", eGuiMaterial_Alpha);
    mvFocusCrosshairGfx[eLuxFocusCrosshair_VoiceOver] = pGui->CreateGfxImage("hud_crosshair_over_voice.tga",eGuiMaterial_Alpha);
    mvFocusCrosshairGfx[eLuxFocusCrosshair_PhoneBox] = pGui->CreateGfxImage("hud_crosshair_over_phone.tga",eGuiMaterial_Alpha);
    mvFocusCrosshairGfx[eLuxFocusCrosshair_Note] = pGui->CreateGfxImage("hud_crosshair_over_note.tga",eGuiMaterial_Alpha);
}

//-----------------------------------------------------------------------

iLuxPlayerState_DefaultBase::~iLuxPlayerState_DefaultBase()
{
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void iLuxPlayerState_DefaultBase::OnEnterState(eLuxPlayerState aPrevState)
{
    mpBodyInFocus = NULL;
    mpEntityInFocus = NULL;

    ImplementedOnEnterState(aPrevState);
}

//-----------------------------------------------------------------------

void iLuxPlayerState_DefaultBase::OnLeaveState(eLuxPlayerState aNewState)
{
    gpBase->mpEffectRenderer->ClearOutlineObjects();

    ImplementedOnLeaveState(aNewState);
}

//-----------------------------------------------------------------------

void iLuxPlayerState_DefaultBase::OnMapEnter(cLuxMap *apMap)
{

}

//-----------------------------------------------------------------------

void iLuxPlayerState_DefaultBase::OnMapLeave(cLuxMap *apMap)
{
    mpBodyInFocus = NULL;
    mpEntityInFocus = NULL;
}

//-----------------------------------------------------------------------

void iLuxPlayerState_DefaultBase::Update(double adFixedDelta)
{
    /////////////////////////////////////////
    // Get the entity currently in focus
    cCamera *pCam = mpPlayer->GetCamera();

    gpBase->mpMapHelper->GetClosestEntity(    pCam->GetPosition(), pCam->GetForward(), 20.0f,
            &mfFocusDistance, &mpBodyInFocus, &mpEntityInFocus);
    mvFocusPos = pCam->GetPosition() + pCam->GetForward() * mfFocusDistance;

    AddOutlineObjects(mpBodyInFocus, mpEntityInFocus, mvFocusPos);

    //For other classes to use focused
    mpPlayer->SetEntityInFocus(mpEntityInFocus);
    mpPlayer->SetBodyInFocus(mpBodyInFocus);
    mpPlayer->SetCurrentFocusDistance(mfFocusDistance);

    if(mpEntityInFocus && CanInteractWithEntity())
    {
        mpPlayer->SetFocusText(mpEntityInFocus->GetFocusText());
    }
    else
    {
        mpPlayer->SetFocusText(_W(""));
    }

    //update implemented class
    ImplementedUpdate(adFixedDelta);
}

void iLuxPlayerState_DefaultBase::PostUpdate(double adFixedDelta)
{

}

//-----------------------------------------------------------------------

void iLuxPlayerState_DefaultBase::OnDraw(cGuiSet *apGuiSet, double adFrameTime)
{
    if(mpEntityInFocus && CanInteractWithEntity())
    {
        mpEntityInFocus->InFocusDraw(apGuiSet, adFrameTime);
    }
}

//-----------------------------------------------------------------------

bool iLuxPlayerState_DefaultBase::OnDoAction(eLuxPlayerAction aAction, bool abPressed)
{
    if(mpPlayer->GetCurrentMoveStateData()->GetAllowsInteraction() == false)
    {
        return true;
    }

    ////////////////////////////
    // Implemented
    if(ImplementedDoAction(aAction, abPressed) == false)
    {
        return false;
    }

    ////////////////////////////
    // Interact
    if(aAction == eLuxPlayerAction_Interact)
    {
        // Pressed
        if(abPressed)
        {
            if(CanInteractWithEntity())
            {
                mpEntityInFocus->OnInteract(mpBodyInFocus, mvFocusPos);
                mpEntityInFocus->RunInteractCallbackFunc();
            }
            return false;
        }
    }


    return true;
}

//-----------------------------------------------------------------------

cGuiGfxElement *iLuxPlayerState_DefaultBase::GetCrosshair()
{
    cGuiGfxElement* pCrosshair = mpDefaultCrosshairGfx;

    if(mpEntityInFocus && mfFocusDistance < mpEntityInFocus->GetMaxFocusDistance())
    {
        eLuxFocusCrosshair crossHair = mpEntityInFocus->GetFocusCrosshair(mpBodyInFocus, mvFocusPos);
        if(mpEntityInFocus->GetInteractionDisabled())
        {
            crossHair = eLuxFocusCrosshair_Default;
        }

        if(crossHair != eLuxFocusCrosshair_LastEnum && crossHair != eLuxFocusCrosshair_Default)
        {
            if(mpPlayer->GetFocusIconStyle() == eLuxFocusIconStyle_Default)
            {
                pCrosshair = mvFocusCrosshairGfx[crossHair];
            }
            else
            {
                pCrosshair = mpSimpleInteractCrosshairGfx;
            }
        }
    }

    //MFP crosshairs (voice, phone, note) do not exist in every mod's resources.
    if(pCrosshair == NULL)
    {
        pCrosshair = mpDefaultCrosshairGfx;
    }

    if(pCrosshair)
    {
        bool bPlayerIsRightHanded = (mpPlayer->GetHandOrientation() == eLuxHandOrientation_Right);
        pCrosshair->SetFlipUvXAxis(bPlayerIsRightHanded);
    }

    return pCrosshair;
}

//-----------------------------------------------------------------------

float iLuxPlayerState_DefaultBase::DrawDebug(cGuiSet *apSet, iFontData *apFont, float afStartY)
{
    tWString sBodyName = mpBodyInFocus ? cString::To16Char(mpBodyInFocus->GetName()) : _W("None");
    apSet->DrawFont(apFont, cVector3f(5, afStartY, 10), 14, cColor(1, 1), _W("Focus Body: %ls Dist: %f"), sBodyName.c_str(), mfFocusDistance);

    afStartY += 15;

    return afStartY;
}

//-----------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool iLuxPlayerState_DefaultBase::ShowOutlineOnEntity(iLuxEntity *apEntity, iPhysicsBody *apBody, const cVector3f &avFocusPos)
{
    if(apEntity->GetEntityType() != eLuxEntityType_Prop)
    {
        return false;
    }

    iLuxProp *pProp = static_cast<iLuxProp *>(apEntity);

    eLuxPropType propType = pProp->GetPropType();
    bool bOutlineType = (propType == eLuxPropType_Item ||
                         propType == eLuxPropType_PhoneBox ||
                         propType == eLuxPropType_VoiceFlashbackTrigger);

    return bOutlineType && apEntity->CanInteract(apBody) && apEntity->GetInteractionDisabled() == false;
}

//-----------------------------------------------------------------------

bool iLuxPlayerState_DefaultBase::CanInteractWithEntity()
{
    if(    mpEntityInFocus && mfFocusDistance < mpEntityInFocus->GetMaxFocusDistance() &&
            mpEntityInFocus->CanInteract(mpBodyInFocus) && mpEntityInFocus->GetInteractionDisabled() == false)
    {
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------

static bool BodyIsAdded(iPhysicsBody *apBody, tPhysicsBodyList &alstBodies)
{
    for(tPhysicsBodyListIt it = alstBodies.begin(); it != alstBodies.end(); ++it)
    {
        if(apBody == *it)
        {
            return true;
        }
    }
    return false;
}

void iLuxPlayerState_DefaultBase::AddOutlineObjects(iPhysicsBody *apBody, iLuxEntity *apEntity, const cVector3f &avFocusPos)
{
    gpBase->mpEffectRenderer->ClearOutlineObjects();
    if(    apEntity == NULL ||
            apEntity->GetEntityType() != eLuxEntityType_Prop ||
            mfFocusDistance > apEntity->GetMaxFocusDistance() ||
            ShowOutlineOnEntity(apEntity, apBody, avFocusPos) == false)
    {
        mbCurrentEntityHasOutline = false;
        return;
    }

    iLuxProp *pProp = static_cast<iLuxProp *>(apEntity);

    if(!pProp->GetGlowEnabled() )
    {
        mbCurrentEntityHasOutline = false;
        return;
    }

    mbCurrentEntityHasOutline = true;

    /////////////////////////////
    //Show all connected bodies
    if(pProp->ShowOutlinesOnConnectedBodies())
    {
        tPhysicsBodyList lstAttachedBodies;
        GetAttachedBodies(apBody, lstAttachedBodies);

        cMeshEntity *pMeshEntity = pProp->GetEffectMeshEntity();
        if(pMeshEntity == NULL)
        {
            return;
        }
        for(int i = 0; i < pMeshEntity->GetSubMeshEntityNum(); ++i)
        {
            cSubMeshEntity *pSubEnt = pMeshEntity->GetSubMeshEntity(i);

            if(BodyIsAdded(static_cast<iPhysicsBody * >(pSubEnt->GetEntityParent()), lstAttachedBodies))
            {
                gpBase->mpEffectRenderer->AddOutlineObject(pSubEnt, pProp->GetGlowOutlineColor());
            }
        }
    }
    /////////////////////////////
    //Show only focused body.
    else
    {
        cMeshEntity *pMeshEntity = pProp->GetEffectMeshEntity();
        if(pMeshEntity == NULL)
        {
            return;
        }
        for(int i = 0; i < pMeshEntity->GetSubMeshEntityNum(); ++i)
        {
            cSubMeshEntity *pSubEnt = pMeshEntity->GetSubMeshEntity(i);

            if(static_cast<iPhysicsBody * >(pSubEnt->GetEntityParent()) == apBody)
            {
                gpBase->mpEffectRenderer->AddOutlineObject(pSubEnt, pProp->GetGlowOutlineColor());
            }
        }
    }
}

//-----------------------------------------------------------------------

void iLuxPlayerState_DefaultBase::GetAttachedBodies(iPhysicsBody *apBody, tPhysicsBodyList &alstBodies)
{
    alstBodies.push_back(apBody);

    for(int i = 0; i < apBody->GetJointNum(); ++i)
    {
        iPhysicsJoint *pJoint = apBody->GetJoint(i);
        iPhysicsBody *pChild = pJoint->GetChildBody();
        iPhysicsBody *pParent = pJoint->GetParentBody();
        if(pChild && BodyIsAdded(pChild, alstBodies) == false)
        {
            GetAttachedBodies(pChild, alstBodies);
        }
        if(pParent && BodyIsAdded(pParent, alstBodies) == false)
        {
            GetAttachedBodies(pParent, alstBodies);
        }
    }
}


//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SAVE DATA STUFF
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerializeVirtual(iLuxPlayerState_DefaultBase_SaveData, iLuxPlayerState_SaveData)
kEndSerialize()

//-----------------------------------------------------------------------


void iLuxPlayerState_DefaultBase::SaveToSaveData(iLuxPlayerState_SaveData* apSaveData)
{
    ///////////////////////
    // Init
    super_class::SaveToSaveData(apSaveData);
    iLuxPlayerState_DefaultBase_SaveData *pData = static_cast<iLuxPlayerState_DefaultBase_SaveData *>(apSaveData);


    ///////////////////////
    // Save vars
}

//-----------------------------------------------------------------------

void iLuxPlayerState_DefaultBase::LoadFromSaveDataBeforeEnter(cLuxMap *apMap, iLuxPlayerState_SaveData* apSaveData)
{
    ///////////////////////
    // Init
    super_class::LoadFromSaveDataBeforeEnter(apMap, apSaveData);
    iLuxPlayerState_DefaultBase_SaveData *pData = static_cast<iLuxPlayerState_DefaultBase_SaveData *>(apSaveData);

    ///////////////////////
    // Load vars
}

//-----------------------------------------------------------------------

void iLuxPlayerState_DefaultBase::LoadFromSaveDataAfterEnter(cLuxMap *apMap, iLuxPlayerState_SaveData* apSaveData)
{
    ///////////////////////
    // Init
    super_class::LoadFromSaveDataAfterEnter(apMap, apSaveData);
    iLuxPlayerState_DefaultBase_SaveData *pData = static_cast<iLuxPlayerState_DefaultBase_SaveData *>(apSaveData);

    ///////////////////////
    // Load vars
}

//-----------------------------------------------------------------------


