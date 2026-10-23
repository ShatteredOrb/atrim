#ifndef LUX_ENEMY_CHILD_H
#define LUX_ENEMY_CHILD_H

//----------------------------------------------

#include "LuxEnemy.h"

//----------------------------------------------

class cLuxEnemy_Child_SaveData : public iLuxEnemy_SaveData
{
    kSerializableClassInit(cLuxEnemy_Child_SaveData)
public:
    ////////////////
    //Properties
    int mPatrolMoveSpeed;
    float mfWaitTime;
};

//----------------------------------------------

class cLuxEnemy_Child : public iLuxEnemy
{
    friend class cLuxEnemyLoader_Child;
    typedef iLuxEnemy super_class;
public:
    cLuxEnemy_Child(const tString &asName, int alID, cLuxMap *apMap);
    virtual ~cLuxEnemy_Child();

    //////////////////////
    //General
    void OnSetupAfterLoad(cWorld *apWorld);
    void OnAfterWorldLoad();
    void UpdateEnemySpecific(double adFixedDelta);

    //////////////////////
    //Actions
    bool StateEventImplement(int alState, eLuxEnemyStateEvent aEvent, cLuxStateMessage *apMessage);


    //////////////////////
    //Debug
    void OnRenderSolidImplemented(cRendererCallbackFunctions* apFunctions);


    //////////////////////
    //Save data stuff
    iLuxEntity_SaveData* CreateSaveData();
    void SaveToSaveData(iLuxEntity_SaveData* apSaveData);
    void LoadFromSaveData(iLuxEntity_SaveData* apSaveData);
    void SetupSaveData(iLuxEntity_SaveData *apSaveData);

private:
    //////////////////////
    // Callbacks
    bool PlayerIsDetected();

    void OnDisableTriggers();

    float GetDamageMul(float afAmount, int alStrength);

    //////////////////////
    // Variables
    eLuxEnemyMoveSpeed mPatrolMoveSpeed;
    float mfWaitTime;
};

//----------------------------------------------

class cLuxEnemyLoader_Child : public iLuxEnemyLoader
{
public:
    cLuxEnemyLoader_Child(const tString& asName);
    virtual ~cLuxEnemyLoader_Child() {}

    iLuxEnemy *CreateEnemy(const tString& asName, int alID, cLuxMap *apMap);
    void LoadVariables(iLuxEnemy *apEnemy, cXmlElement *apRootElem);
    void LoadInstanceVariables(iLuxEnemy *apEnemy, cResourceVarsObject *apInstanceVars);
};

//----------------------------------------------


#endif // LUX_ENEMY_CHILD_H
