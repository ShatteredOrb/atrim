#ifndef LUX_PROP_SWING_DOOR_H
#define LUX_PROP_SWING_DOOR_H

//----------------------------------------------

#include "LuxProp.h"

//----------------------------------------------

class cLuxProp_SwingDoor_SaveData : public iLuxProp_SaveData
{
    kSerializableClassInit(cLuxProp_SwingDoor_SaveData)
public:
    bool mbLocked;
    bool mbBroken;
    bool mbClosed;

    int mlBrokenEntityID;

    bool mbDisableBreakable;
    bool mbDisableAutoClose;

    int mlCurrentMeshEntity;

    float mfGoalOpenAmount;
    bool mbOpeningTowardsMaxAngle;
    float mfCurrentOpenAmount;
    float mfOpenSpeed;
    bool mbIsUpdatingOpenAmount;

    cEngineMeshEntity_SaveData mDamageMesh1;
    cEngineMeshEntity_SaveData mDamageMesh2;
};

//----------------------------------------------

class cLuxSwingDoorJointData
{
public:
    iPhysicsJointHinge *mpHingeJoint;
    iPhysicsBody *mpChildBody;

    float mfMinAngle;
    float mfMaxAngle;
};


//----------------------------------------------


class cLuxProp_SwingDoor : public iLuxProp
{
    typedef iLuxProp super_class;
    friend class cLuxPropLoader_SwingDoor;
public:
    cLuxProp_SwingDoor(const tString &asName, int alID, cLuxMap *apMap);
    virtual ~cLuxProp_SwingDoor();

    //////////////////////
    //Genera
    bool CanInteract(iPhysicsBody *apBody);
    bool OnInteract(iPhysicsBody *apBody, const cVector3f &avPos);

    void OnSetupAfterLoad(cWorld *apWorld);

    void OnResetProperties();

    void UpdatePropSpecific(double adFixedDelta);
    void UpdateOpening(double adFixedDelta);

    void BeforePropDestruction();

    eLuxFocusCrosshair GetFocusCrosshair(iPhysicsBody *apBody, const cVector3f &avPos);

    void ImplementedOnSetActive(bool abX);

    void OnHealthChange();
    void OnDamage(float afAmount, int alStrength);

    //////////////////////
    //Properties
    iLuxInteractData_RotateBase *GetMoveBaseData()
    {
        return &mSwingDoorData;
    }

    void SetClosed(bool abClosed, bool abEffects);
    bool GetClosed()
    {
        return mbClosed;
    }

    /**
     * -1=closed 0=between 1=open
     */
    int GetDoorState();

    void SetLocked(bool abLocked, bool abEffects);
    bool GetLocked()
    {
        return mbLocked;
    }

    void SetDisableAutoClose(bool abX)
    {
        mbDisableAutoClose = abX;
    }
    bool GetDisableAutoClose()
    {
        return mbDisableAutoClose;
    }

    void SetOpenAmount(float afOpenAmount, float afDuration, bool abGoTowardsMaxAngle);

    void SetCurrentDamageLevel(int alX);

    cMeshEntity *GetEffectMeshEntity();

    bool IsBroken()
    {
        return mbBroken;
    }
    /** True if damage can break the door (breakable and not disabled by script). */
    bool CanBeBroken()
    {
        return mbBreakable && mbDisableBreakable == false;
    }

    //////////////////////
    //Connection callbacks
    void OnConnectionStateChange(iLuxEntity *apEntity, int alState);


    //////////////////////
    //Save data stuff
    iLuxEntity_SaveData *CreateSaveData();
    void SaveToSaveData(iLuxEntity_SaveData* apSaveData);
    void LoadFromSaveData(iLuxEntity_SaveData* apSaveData);
    void SetupSaveData(iLuxEntity_SaveData *apSaveData);

private:
    void SetupDoorPhysics(float afOpenAmount, bool abOpenTowardsMaxAngle);

    cLuxSwingDoorJointData *GetJointDataFromBody(iPhysicsBody *apBody);
    cLuxSwingDoorJointData *GetJointDataFromJoint(iPhysicsJoint *apJoint);

    //////////////////////
    // Data
    std::vector<cLuxSwingDoorJointData> mvJointData;

    cLuxInteractData_SwingDoor mSwingDoorData;

    bool mbIsUpdatingOpenAmount;
    bool mbOpeningTowardsMaxAngle;
    float mfGoalOpenAmount;
    float mfOpenSpeed;
    float mfCurrentOpenAmount;

    bool mbCanInteractWithStaticBody;
    tString msCloseOnSound;
    tString msCloseOffSound;
    tString msLockOnSound;
    tString msLockOffSound;
    tString msInteractLockedSound;

    bool mbDisableBreakable;
    bool mbBreakable;

    tString msDamageMesh[2];
    tString msBrokenEntity;

    tString msDamageSound;
    tString msDamagePS;

    tString msBreakSound;
    tString msBreakPS;
    float mfBreakImpulse;

    float mfHealthDamage[2];

    cMeshEntity *mpDamageMeshEntity[3];

    //////////////////////
    // Variables
    bool mbLocked;
    bool mbClosed;

    int mlBrokenEntityID;

    bool mbDisableAutoClose;

    int mlCurrentMeshEntity;
    bool mbBroken;

    float mfInteractSoundCount;


};

//----------------------------------------------

class cLuxPropLoader_SwingDoor : public iLuxPropLoader
{
public:
    cLuxPropLoader_SwingDoor(const tString& asName);
    virtual ~cLuxPropLoader_SwingDoor() {}

    iLuxProp *CreateProp(const tString& asName, int alID, cLuxMap *apMap);
    void LoadVariables(iLuxProp *apProp, cXmlElement *apRootElem);
    void LoadInstanceVariables(iLuxProp *apProp, cResourceVarsObject *apInstanceVars);

private:
};

//----------------------------------------------


#endif // LUX_PROP_SWING_DOOR_H
