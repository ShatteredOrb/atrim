#ifndef LUX_PROP_PHONEBOX_H
#define LUX_PROP_PHONEBOX_H

//----------------------------------------------

#include "LuxProp.h"

//----------------------------------------------

class cLuxProp_PhoneBox_SaveData : public iLuxProp_SaveData
{
    kSerializableClassInit(cLuxProp_PhoneBox_SaveData)
public:

    tString msVoiceFile;
    tString msTextEntry1;
    tString msTextEntry2;
    tString msTextEntry3;
    tString msTextEntry4;
    tString msTextEntry5;
    tString msTextEntry6;
    tString msTextEntry7;
    float mfDelay1;
    float mfDelay2;
    float mfDelay3;
    float mfDelay4;
    float mfDelay5;
    float mfDelay6;
    float mfDelay7;
    bool mbPlayerHasInteracted;
    bool mbIsRinging;
    bool mbIsPlayingVoice;
};


//----------------------------------------------

class cLuxProp_PhoneBox : public iLuxProp
{
    typedef iLuxProp super_class;
    friend class cLuxPropLoader_PhoneBox;
public:
    cLuxProp_PhoneBox(const tString &asName, int alID, cLuxMap *apMap);
    virtual ~cLuxProp_PhoneBox();

    //////////////////////
    //General
    bool CanInteract(iPhysicsBody *apBody);
    bool OnInteract(iPhysicsBody *apBody, const cVector3f &avPos);

    void OnSetupAfterLoad(cWorld *apWorld);

    void OnResetProperties();

    void UpdatePropSpecific(double adFixedDelta);

    void BeforePropDestruction();

    eLuxFocusCrosshair GetFocusCrosshair(iPhysicsBody *apBody, const cVector3f &avPos);

    void StartRinging();
    void StopRinging();
    void HangUp();
    void VoiceStopped();

    //////////////////////
    //Properties
    int GetSpawnContainerID()
    {
        return mlSpawnContainerID;
    }
    void SetSpawnContainerID(int alX)
    {
        mlSpawnContainerID = alX;
    }

    //////////////////////
    //Connection callbacks
    void OnConnectionStateChange(iLuxEntity *apEntity, int alState);

    //////////////////////
    //Save data stuff
    iLuxEntity_SaveData* CreateSaveData();
    void SaveToSaveData(iLuxEntity_SaveData* apSaveData);
    void LoadFromSaveData(iLuxEntity_SaveData* apSaveData);
    void SetupSaveData(iLuxEntity_SaveData *apSaveData);


private:
    ////////////////
    //Variables
    tString msVoiceFile;
    tString msTextEntry1;
    tString msTextEntry2;
    tString msTextEntry3;
    tString msTextEntry4;
    tString msTextEntry5;
    tString msTextEntry6;
    tString msTextEntry7;
    float mfDelay1;
    float mfDelay2;
    float mfDelay3;
    float mfDelay4;
    float mfDelay5;
    float mfDelay6;
    float mfDelay7;

    int mlSpawnContainerID;

    float mfFlashAlpha;

    float mfMoveSpeedMultiplier;
    float mfMouseSensitivityModifier;

    bool mbPlayerHasInteracted;
    bool mbIsRinging;
    bool mbIsPlayingVoice;

    cSoundEntity* mpRingingSound;
};

//----------------------------------------------

class cLuxPropLoader_PhoneBox : public iLuxPropLoader
{
public:
    cLuxPropLoader_PhoneBox(const tString& asName);
    virtual ~cLuxPropLoader_PhoneBox() {}

    iLuxProp *CreateProp(const tString& asName, int alID, cLuxMap *apMap);
    void LoadVariables(iLuxProp *apProp, cXmlElement *apRootElem);
    void LoadInstanceVariables(iLuxProp *apProp, cResourceVarsObject *apInstanceVars);

private:
};

//----------------------------------------------


#endif // LUX_PROP_PHONEBOX_H
