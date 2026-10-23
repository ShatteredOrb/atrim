#ifndef LUX_PLAYER_STATE_DEFAULT_BASE_H
#define LUX_PLAYER_STATE_DEFAULT_BASE_H

//----------------------------------------------

#include "LuxPlayerState.h"

//----------------------------------------------

class cLuxPlayer;

//------------------------------------

class iLuxPlayerState_DefaultBase_SaveData : public iLuxPlayerState_SaveData
{
    kSerializableClassInit(iLuxPlayerState_DefaultBase_SaveData)
public:
    virtual ~iLuxPlayerState_DefaultBase_SaveData() {}
};


//----------------------------------------------

class iLuxPlayerState_DefaultBase : public iLuxPlayerState
{
    typedef iLuxPlayerState super_class;
public:
    iLuxPlayerState_DefaultBase(cLuxPlayer *apPlayer, eLuxPlayerState aType);
    virtual ~iLuxPlayerState_DefaultBase();

    void OnEnterState(eLuxPlayerState aPrevState);
    void OnLeaveState(eLuxPlayerState aNewState);

    void OnMapEnter(cLuxMap *apMap);
    void OnMapLeave(cLuxMap *apMap);

    void Update(double adFixedDelta);
    void PostUpdate(double adFixedDelta);
    virtual void OnDraw(cGuiSet *apGuiSet, double adFrameTime);

    virtual cGuiGfxElement *GetCrosshair();

    bool OnDoAction(eLuxPlayerAction aAction, bool abPressed);

    void OnSaveBody(iPhysicsBody *apBody, float &afMass, bool &abCollideCharacter) {}

    float DrawDebug(cGuiSet *apSet, iFontData *apFont, float afStartY);

    /////////////////////////////////
    //Save data stuff
    virtual bool IsSaved()
    {
        return true;
    }

    void SaveToSaveData(iLuxPlayerState_SaveData* apSaveData);
    void LoadFromSaveDataBeforeEnter(cLuxMap *apMap, iLuxPlayerState_SaveData* apSaveData);
    void LoadFromSaveDataAfterEnter(cLuxMap *apMap, iLuxPlayerState_SaveData* apSaveData);

protected:
    virtual bool ShowOutlineOnEntity(iLuxEntity *apEntity, iPhysicsBody *apBody, const cVector3f &avFocusPos);

    virtual void ImplementedUpdate(double adFixedDelta) {}
    virtual bool ImplementedDoAction(eLuxPlayerAction aAction, bool abPressed)
    {
        return true;
    }
    virtual void ImplementedOnEnterState(eLuxPlayerState aPrevState) {}
    virtual void ImplementedOnLeaveState(eLuxPlayerState aNewState) {}

    bool CanInteractWithEntity();

    void AddOutlineObjects(iPhysicsBody *apBody, iLuxEntity *apEntity, const cVector3f &avFocusPos);
    void GetAttachedBodies(iPhysicsBody *apBody, tPhysicsBodyList &alstBodies);

    iPhysicsBody *mpBodyInFocus;
    iLuxEntity *mpEntityInFocus;
    float mfFocusDistance;
    cVector3f mvFocusPos;
    bool mbCurrentEntityHasOutline;
};

//----------------------------------------------


#endif // LUX_PLAYER_STATE_DEFAULT_BASE_H
