#ifndef LUX_PLAYER_STATE_INTERACT_H
#define LUX_PLAYER_STATE_INTERACT_H

//----------------------------------------------

#include "LuxPlayerState.h"

//----------------------------------------------

class iLuxProp;

//----------------------------------------------

class iLuxPlayerState_Interact_SaveData : public iLuxPlayerState_SaveData
{
    kSerializableClassInit(iLuxPlayerState_Interact_SaveData)
public:
    virtual ~iLuxPlayerState_Interact_SaveData() {}
    int mlCurrentPropId;
    int mlCurrentBodyId;
    cVector3f mvCurrentFocusPos;
};

//----------------------------------------------

class iLuxPlayerState_Interact : public iLuxPlayerState
{
    typedef iLuxPlayerState super_class;
public:
    iLuxPlayerState_Interact(cLuxPlayer *apPlayer, eLuxPlayerState aType);
    virtual ~iLuxPlayerState_Interact();

    void OnDestroyEntity(iLuxEntity *apEntity);
    void OnAttachBodyToStickyArea(iPhysicsBody *apBody);

    /////////////////////////////////
    //Save data stuff
    virtual void SaveToSaveData(iLuxPlayerState_SaveData* apSaveData);
    virtual void LoadFromSaveDataBeforeEnter(cLuxMap *apMap, iLuxPlayerState_SaveData* apSaveData);
    virtual void LoadFromSaveDataAfterEnter(cLuxMap *apMap, iLuxPlayerState_SaveData* apSaveData);

protected:
    void SetupInteractVars();
    void ResetInteractVars();

    iPhysicsBody *mpCurrentBody;
    iLuxProp *mpCurrentProp;
    cVector3f mvCurrentFocusPos;
};

//----------------------------------------------


#endif // LUX_PLAYER_STATE_INTERACT_H
