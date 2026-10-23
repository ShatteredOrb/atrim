#ifndef LUX_ENEMY_WATER_LURKER_H
#define LUX_ENEMY_WATER_LURKER_H

//----------------------------------------------

#include "LuxEnemy.h"

//----------------------------------------------

class cLuxEnemy_WaterLurker_SaveData : public iLuxEnemy_SaveData
{
    kSerializableClassInit(cLuxEnemy_WaterLurker_SaveData)
public:
    ////////////////
    //Properties
    float mfPlayerDetectionHeight;
    float mfRunSpeedMul;
};

//----------------------------------------------

enum eWaterLurkerSplash
{
    eWaterLurkerSplash_Walk,
    eWaterLurkerSplash_Run,
    eWaterLurkerSplash_Eat,
    eWaterLurkerSplash_Attack
};

//----------------------------------------------

class cLuxEnemy_WaterLurker : public iLuxEnemy
{
    friend class cLuxEnemyLoader_WaterLurker;
    typedef iLuxEnemy super_class;
public:
    cLuxEnemy_WaterLurker(const tString &asName, int alID, cLuxMap *apMap);
    virtual ~cLuxEnemy_WaterLurker();

    //////////////////////
    //General
    void OnSetupAfterLoad(cWorld *apWorld);
    void OnAfterWorldLoad();
    void UpdateEnemySpecific(double adFixedDelta);

    //////////////////////
    //Actions
    bool StateEventImplement(int alState, eLuxEnemyStateEvent aEvent, cLuxStateMessage *apMessage);

    //////////////////////
    //Callbacks
    bool InRangeOfFood(iPhysicsBody *apFoodBody);

    //////////////////////
    //Debug
    void OnRenderSolidImplemented(cRendererCallbackFunctions* apFunctions);


    //////////////////////
    //Save data stuff
    iLuxEntity_SaveData *CreateSaveData();
    void SaveToSaveData(iLuxEntity_SaveData* apSaveData);
    void LoadFromSaveData(iLuxEntity_SaveData* apSaveData);
    void SetupSaveData(iLuxEntity_SaveData *apSaveData);

private:
    //////////////////////
    // Actions
    void SplashWater(eWaterLurkerSplash aType);

    //////////////////////
    // Callbacks
    bool PlayerIsDetected();
    void OnSetActiveEnemySpecific(bool abX);

    float GetDamageMul(float afAmount, int alStrength);

    //////////////////////
    // State stuff
    void PatrolUpdateGoal();
    void PatrolEndOfPath();

    //////////////
    //Data
    float mfPlayerDetectionHeight;
    float mfRunSpeedMul;

    float mfEatDamage;
    float mfAfterAttackPauseTime;

    tString msSplashPS_Walk;
    tString msSplashPS_Run;
    tString msSplashPS_Eat;
    tString msSplashPS_Attack;

    tString msSplashSound_Walk;
    tString msSplashSound_Run;
    tString msSplashSound_Eat;
    tString msSplashSound_Attack;
};

//----------------------------------------------

class cLuxEnemyLoader_WaterLurker : public iLuxEnemyLoader
{
public:
    cLuxEnemyLoader_WaterLurker(const tString& asName);
    virtual ~cLuxEnemyLoader_WaterLurker() {}

    iLuxEnemy *CreateEnemy(const tString& asName, int alID, cLuxMap *apMap);
    void LoadVariables(iLuxEnemy *apEnemy, cXmlElement *apRootElem);
    void LoadInstanceVariables(iLuxEnemy *apEnemy, cResourceVarsObject *apInstanceVars);
};

//----------------------------------------------


#endif // LUX_ENEMY_WATER_LURKER_H
