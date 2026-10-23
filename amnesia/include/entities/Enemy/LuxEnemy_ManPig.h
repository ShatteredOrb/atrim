#ifndef LUX_ENEMY_MAN_PIG_H
#define LUX_ENEMY_MAN_PIG_H

//----------------------------------------------

#include "LuxEnemy.h"
#include "LuxIdleBehavior_ManPig.h"

//----------------------------------------------

class cLuxEnemy_ManPig_SaveData : public iLuxEnemy_SaveData
{
    kSerializableClassInit(cLuxEnemy_ManPig_SaveData)
public:
    ////////////////
    //Properties
    bool mbThreatenOnAlert;
    bool mbFleeFromPlayer;
    bool mbAutoDisableAfterFlee;
    bool mbPlayActivateSound;
    float mfLanternSensitivity;

    bool mbAllowZeroWaitTime;

    int mIdleBehavior;
    int mPatrolMoveSpeed;

    float mfInLanternLightCount;
    float mfHuntPauseTimeMul;

    float mfDamageMul;
    float mfRunSpeedMul;

    bool mbTeslaTerror;
    bool mbTeslaFadeDisabled;
    bool mbTeslaSoundDisabled;
    bool mbTeslaEasyEscapeDisabled;
};

//----------------------------------------------

class cLuxEnemy_ManPig : public iLuxEnemy
{
    friend class cLuxEnemyLoader_ManPig;
    typedef iLuxEnemy super_class;
public:
    cLuxEnemy_ManPig(const tString &asName, int alID, cLuxMap *apMap);
    virtual ~cLuxEnemy_ManPig();

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
    iLuxEntity_SaveData *CreateSaveData();
    void SaveToSaveData(iLuxEntity_SaveData* apSaveData);
    void LoadFromSaveData(iLuxEntity_SaveData* apSaveData);
    void SetupSaveData(iLuxEntity_SaveData *apSaveData);

    //////////////////////
    //Tesla
    void SetTeslaFadeDisabled(bool abX);
    void SetTeslaSoundDisabled(bool abX);
    void SetTeslaEasyEscapeDisabled(bool abX);
    void ForceTeslaSighting();
private:
    //////////////////////
    // Callbacks
    bool PlayerIsDetected();

    void OnDisableTriggers();

    float GetDamageMul(float afAmount, int alStrength);

    void OnSetActiveEnemySpecific(bool abX);

    //////////////////////
    // State stuff
    bool CheckEnemyAutoRemoval(float afDistance);
    void PatrolUpdateGoal();
    bool FleeTryToFindSafeNode();
    bool StalkFindNode();
    bool TrackFindNode();
    bool TrackTeleportBehindPlayer();

    //////////////////////
    // Helper
    void PlayHuntSound();
    bool InsidePlayerView();
    void UpdateCheckInLantern(double adFixedDelta);

    void SetTeslaEffectsAmount(float afX);
    void ResetMindFuckEffects();
    void UpdateTesla(double adFixedDelta);

    //////////////
    //Data
    tString msNoticeSound;
    tString msGiveUpNoticeSound;
    tString msEnabledSound;
    tString msChaseSound;
    tString msTeslaMindFuckLoop;

    float mfGroggyDamageCount;
    float mfAlertToHuntDistance;
    float mfAlertToInstantHuntDistance;
    float mfHuntPauseMinTime;
    float mfHuntPauseMaxTime;
    float mfIncreaseAlertSpeedDistance;
    float mfIncreasedAlertSpeedMul;
    float mfAlertRunTowardsToHuntLimit;
    float mfAlertRunTowardsCheckDistance;
    bool mbIsTelsa;


    std::vector<cColor> mvDefaultLightColors;

    //////////////
    //Variables
    float mfWaitTime;
    float mfAlertRunTowardsCount;
    bool mbPlayActivateSound;
    float mfLanternSensitivity;

    bool mbLastShortAttackWasMiss;
    bool mbForceChargeAttack;

    cSoundEntry *mpMindFuckSound;
    int mlMindFuckSoundId;

    float mfDamageMul;
    float mfRunSpeedMul;

    bool mbThreatenOnAlert;
    bool mbFleeFromPlayer;
    bool mbAutoDisableAfterFlee;

    float mfFleeCheckIfInvisbleCount;

    float mfHuntPauseTimeMul;

    float mfTeslaFlickerTimer;
    int mlTeslaFlickerState;
    float mfTeslaEffectAmount;
    float mfTeslaSpecialNoticeCount;
    bool mbTeslaMindFuckActive;
    float mfTeslaMindFuckPulse;
    float mfTeslaMindFuckPulseAdd;
    float mfBlackOutCount;
    float mfBlackOutDurationCount;
    bool mbTeslaTerror;
    float mfMindFuckBlinkCount;
    float mfMindFuckBlinkAmount;
    int mlMindFuckBlinkState;
    bool mbTeslaForceSighting;
    bool mbTeslaFadeDisabled; //Used to avoid tesla fades to override script fades
    bool mbTeslaSoundDisabled;
    bool mbTeslaEasyEscapeDisabled; //Allows for chase sequences with the tesla

    eLuxIdleBehavior mIdleBehavior;
    eLuxEnemyMoveSpeed mPatrolMoveSpeed;
    bool mbAllowZeroWaitTime;

    float mfCheckFlashLightShining;

    cSoundEntity *mpHuntSound;
    int mlHuntSoundId;
};

//----------------------------------------------

class cLuxEnemyLoader_ManPig : public iLuxEnemyLoader
{
public:
    cLuxEnemyLoader_ManPig(const tString& asName);
    virtual ~cLuxEnemyLoader_ManPig() {}

    iLuxEnemy *CreateEnemy(const tString& asName, int alID, cLuxMap *apMap);
    void LoadVariables(iLuxEnemy *apEnemy, cXmlElement *apRootElem);
    void LoadInstanceVariables(iLuxEnemy *apEnemy, cResourceVarsObject *apInstanceVars);
};

//----------------------------------------------

#endif // LUX_ENEMY_MAN_PIG_H
