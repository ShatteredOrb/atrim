#ifndef LUX_PROP_BUTTON_H
#define LUX_PROP_BUTTON_H

//----------------------------------------------

#include "LuxProp.h"

//----------------------------------------------

class cLuxProp_Button_SaveData : public iLuxProp_SaveData
{
    kSerializableClassInit(cLuxProp_Button_SaveData)
public:
    bool mbSwitchedOn;
    bool mbCanBeSwitchedOn;
    bool mbCanBeSwitchedOff;
};


//----------------------------------------------

class cLuxProp_Button : public iLuxProp
{
    typedef iLuxProp super_class;
    friend class cLuxPropLoader_Button;
public:
    cLuxProp_Button(const tString &asName, int alID, cLuxMap *apMap);
    virtual ~cLuxProp_Button();

    //////////////////////
    //General
    bool CanInteract(iPhysicsBody *apBody);
    bool OnInteract(iPhysicsBody *apBody, const cVector3f &avPos);

    void OnSetupAfterLoad(cWorld *apWorld);

    void OnResetProperties();

    void UpdatePropSpecific(double adFixedDelta);

    void BeforePropDestruction();

    eLuxFocusCrosshair GetFocusCrosshair(iPhysicsBody *apBody, const cVector3f &avPos);

    //////////////////////
    //Properties
    void SetSwitchedOn(bool abX, bool abUseEffects);
    void SetCanBeSwitchedOn(bool abX);
    void SetCanBeSwitchedOff(bool abX);

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
    //Vars
    bool mbSwitchedOn;
    bool mbCanBeSwitchedOn;
    bool mbCanBeSwitchedOff;
};

//----------------------------------------------

class cLuxPropLoader_Button : public iLuxPropLoader
{
public:
    cLuxPropLoader_Button(const tString& asName);
    virtual ~cLuxPropLoader_Button() {}

    iLuxProp *CreateProp(const tString& asName, int alID, cLuxMap *apMap);
    void LoadVariables(iLuxProp *apProp, cXmlElement *apRootElem);
    void LoadInstanceVariables(iLuxProp *apProp, cResourceVarsObject *apInstanceVars);

private:
};

//----------------------------------------------


#endif // LUX_PROP_BUTTON_H
