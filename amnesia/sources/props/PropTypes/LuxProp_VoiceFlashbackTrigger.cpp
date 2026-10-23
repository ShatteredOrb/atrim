#include "LuxProp_VoiceFlashbackTrigger.h"

#include "LuxPlayer.h"
#include "LuxMap.h"
#include "LuxHelpFuncs.h"
#include "LuxInventory.h"
#include "LuxItemType.h"
#include "LuxMessageHandler.h"
#include "LuxEffectRenderer.h"
#include "LuxPlayerHelpers.h"

//////////////////////////////////////////////////////////////////////////
// LOADER
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxPropLoader_VoiceFlashbackTrigger::cLuxPropLoader_VoiceFlashbackTrigger(const tString& asName) : iLuxPropLoader(asName)
{
    mfDefaultMaxFocusDistance = gpBase->mpGameCfg->GetFloat("Player_Interaction","Item_DefaultMaxFocusDist",0);
}

//-----------------------------------------------------------------------

iLuxProp *cLuxPropLoader_VoiceFlashbackTrigger::CreateProp(const tString& asName, int alID, cLuxMap *apMap)
{
    return hplNew(cLuxProp_VoiceFlashbackTrigger, (asName, alID,apMap) );
}

//-----------------------------------------------------------------------

void cLuxPropLoader_VoiceFlashbackTrigger::LoadVariables(iLuxProp *apProp, cXmlElement *apRootElem)
{
}

//-----------------------------------------------------------------------

void cLuxPropLoader_VoiceFlashbackTrigger::LoadInstanceVariables(iLuxProp *apProp, cResourceVarsObject *apInstanceVars)
{
    cLuxProp_VoiceFlashbackTrigger  *pVoiceFlashbackTrigger = static_cast<cLuxProp_VoiceFlashbackTrigger*>(apProp);

    pVoiceFlashbackTrigger->msVoiceFile = apInstanceVars->GetVarString("VoiceFile", "");
    pVoiceFlashbackTrigger->msTextEntry = apInstanceVars->GetVarString("TextEntry", "");
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

//Must NOT be eLuxPropType_Item: lots of code static_casts every Item prop to cLuxProp_Item.
cLuxProp_VoiceFlashbackTrigger::cLuxProp_VoiceFlashbackTrigger(const tString &asName,int alID, cLuxMap *apMap) : iLuxProp(asName,alID,apMap, eLuxPropType_VoiceFlashbackTrigger)
{
    mlSpawnContainerID =-1;
    mfFlashAlpha =0;
    mbPlayerHasInteracted = false;

    mfMoveSpeedMultiplier = gpBase->mpGameCfg->GetFloat("VoiceFlashback","VoiceTriggerMoveSpeedMultiplier",0);
    mfMouseSensitivityModifier = gpBase->mpGameCfg->GetFloat("VoiceFlashback","VoiceTriggerMouseSensitivityModifier",0);
}

//-----------------------------------------------------------------------

cLuxProp_VoiceFlashbackTrigger::~cLuxProp_VoiceFlashbackTrigger()
{
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cLuxProp_VoiceFlashbackTrigger::CanInteract(iPhysicsBody *apBody)
{
    return !mbPlayerHasInteracted;
}

bool cLuxProp_VoiceFlashbackTrigger::OnInteract(iPhysicsBody *apBody, const cVector3f &avPos)
{
    cLuxPlayerVoiceFlashback * pVoiceFlashback = gpBase->mpPlayer->GetHelperVoiceFlashback();

    pVoiceFlashback->StopPlaying();

    pVoiceFlashback->SetMoveSpeedMultiplier( mfMoveSpeedMultiplier );
    pVoiceFlashback->SetMouseSensitivityModifier( mfMouseSensitivityModifier );

    pVoiceFlashback->Start( msVoiceFile,
                            msTextEntry, 0.0f,
                            "", 0.0f,
                            "", 0.0f,
                            "", 0.0f,
                            "", 0.0f,
                            "", 0.0f,
                            "", 0.0f,
                            true, true
                          );

    mbPlayerHasInteracted = true;

    return true;
}

//-----------------------------------------------------------------------

void cLuxProp_VoiceFlashbackTrigger::OnResetProperties()
{

}

//-----------------------------------------------------------------------

void cLuxProp_VoiceFlashbackTrigger::OnSetupAfterLoad(cWorld *apWorld)
{

}

//-----------------------------------------------------------------------

void cLuxProp_VoiceFlashbackTrigger::UpdatePropSpecific(double adFixedDelta)
{
    if(mbPlayerHasInteracted)
    {
        return;
    }

    FlashIfNearPlayer(adFixedDelta);
}

//-----------------------------------------------------------------------

void cLuxProp_VoiceFlashbackTrigger::BeforePropDestruction()
{
}

//-----------------------------------------------------------------------

eLuxFocusCrosshair cLuxProp_VoiceFlashbackTrigger::GetFocusCrosshair(iPhysicsBody *apBody, const cVector3f &avPos)
{
    return eLuxFocusCrosshair_VoiceOver;
}

//-----------------------------------------------------------------------

void cLuxProp_VoiceFlashbackTrigger::OnConnectionStateChange(iLuxEntity *apEntity, int alState)
{
    //TODO!
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SAVE DATA STUFF
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

kBeginSerialize(cLuxProp_VoiceFlashbackTrigger_SaveData, iLuxProp_SaveData)
kSerializeVar(msVoiceFile, eSerializeType_String)
kSerializeVar(msTextEntry, eSerializeType_String)
kSerializeVar(mlSpawnContainerID, eSerializeType_Int32)
kEndSerialize()

//-----------------------------------------------------------------------

iLuxEntity_SaveData* cLuxProp_VoiceFlashbackTrigger::CreateSaveData()
{
    return hplNew(cLuxProp_VoiceFlashbackTrigger_SaveData, ());
}

//-----------------------------------------------------------------------

void cLuxProp_VoiceFlashbackTrigger::SaveToSaveData(iLuxEntity_SaveData* apSaveData)
{
    //////////////////
    //Init
    super_class::SaveToSaveData(apSaveData);
    cLuxProp_VoiceFlashbackTrigger_SaveData *pData = static_cast<cLuxProp_VoiceFlashbackTrigger_SaveData*>(apSaveData);

    //////////////////
    //Set variables
    kCopyToVar(pData,msVoiceFile);
    kCopyToVar(pData,msTextEntry);
    kCopyToVar(pData,mlSpawnContainerID);
}

//-----------------------------------------------------------------------

void cLuxProp_VoiceFlashbackTrigger::LoadFromSaveData(iLuxEntity_SaveData* apSaveData)
{
    //////////////////
    //Init
    super_class::LoadFromSaveData(apSaveData);
    cLuxProp_VoiceFlashbackTrigger_SaveData *pData = static_cast<cLuxProp_VoiceFlashbackTrigger_SaveData*>(apSaveData);

    //////////////////
    //Set variables
    kCopyFromVar(pData,msVoiceFile);
    kCopyFromVar(pData,msTextEntry);
    kCopyFromVar(pData,mlSpawnContainerID);
}

//-----------------------------------------------------------------------

void cLuxProp_VoiceFlashbackTrigger::SetupSaveData(iLuxEntity_SaveData *apSaveData)
{
    super_class::SetupSaveData(apSaveData);
}

//-----------------------------------------------------------------------
