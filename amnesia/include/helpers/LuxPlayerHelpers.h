#ifndef LUX_PLAYER_HELPERS_H
#define LUX_PLAYER_HELPERS_H

//----------------------------------------------

#include "LuxBase.h"

//----------------------------------------------

class cLuxPlayer;

//----------------------------------------------

class cLuxPlayerVoiceFlashback : public iLuxPlayerHelper
{
    friend class cLuxPlayer_SaveData;
public:
    cLuxPlayerVoiceFlashback(cLuxPlayer *apPlayer);
    ~cLuxPlayerVoiceFlashback();

    void Update(double adFixedDelta);

    bool AllowsJump()
    {
        return !(mbIsPlaying && (mbObstructMovement));
    }

    void StopPlaying();
    void Start(
        const tString & asVoiceFile,
        const tString & asTextEntry1, float afDelay1,
        const tString & asTextEntry2, float afDelay2,
        const tString & asTextEntry3, float afDelay3,
        const tString & asTextEntry4, float afDelay4,
        const tString & asTextEntry5, float afDelay5,
        const tString & asTextEntry6, float afDelay6,
        const tString & asTextEntry7, float afDelay7,
        bool abUseEffects, bool abObstructMovement );
    void StartEffects();
    void StopEffects();
    void StopMovementObstruction();
    void StartMovementObstruction();
    void SetStopSound( const tString& asStopSound )
    {
        m_sStopSound = asStopSound;
    }
    bool GetIsPlaying()
    {
        return mbIsPlaying;
    }

    void SetMoveSpeedMultiplier( float afMoveSpeedMultiplier )
    {
        mfMoveSpeedMultiplier = afMoveSpeedMultiplier;
    }
    void SetMouseSensitivityModifier( float afMouseSensitivityModifier )
    {
        mfMouseSensitivityModifier = afMouseSensitivityModifier;
    }

private:
    bool mbIsDelaying;
    bool mbIsPlaying;
    bool mbUseEffects;
    bool mbObstructMovement;

    float mfEffectFadeInTime;
    float mfEffectFadeOutTime;
    float mfSepiaAmount;
    float mfLightFadeAmount;
    float mfImageTrailAmount;
    float mfBlurStartDistance;
    float mfBlurAmount;
    float mfFovMultiplier;
    float mfMoveSpeedMultiplier;
    float mfMouseSensitivityModifier;
    tString m_sStopSound;
};

//----------------------------------------------

class cLuxPlayerInsanityCollapse : public iLuxPlayerHelper
{
    friend class cLuxPlayer_SaveData;
public:
    cLuxPlayerInsanityCollapse(cLuxPlayer *apPlayer);
    ~cLuxPlayerInsanityCollapse();

    void Reset();

    void Start();
    void Stop();
    bool IsActive()
    {
        return mbActive;
    }

    void Update(double adFixedDelta);

private:
    bool mbActive;

    int mlState;

    float mfHeightAddCollapseSpeed;
    float mfHeightAddAwakeSpeed;
    float mfRollCollapseSpeed;
    float mfRollAwakeSpeed;

    float mfHeightAddGoal;

    float mfAwakenSanity;

    float mfSleepTime;

    float mfSleepSpeedMul;
    float mfWakeUpSpeedMul;

    tString msStartSound;
    tString msAwakenSound;
    tString msSleepLoopSound;
    float mfSleepLoopSoundVolume;

    tString msSleepRandomSound;
    float mfSleepRandomMinTime;
    float mfSleepRandomMaxTime;

    float mfHeightAdd;
    float mfRoll;

    float mfT;
    float mfRandomCount;

    cSoundEntry *mpLoopSound;
    int mlLoopSoundID;
};

//----------------------------------------------

class cLuxPlayerCamDirEffects : public iLuxPlayerHelper
{
public:
    cLuxPlayerCamDirEffects(cLuxPlayer *apPlayer);
    ~cLuxPlayerCamDirEffects();

    void Reset();

    void Update(double adFixedDelta);

    float AddAndGetYawAdd(float afX, bool abFrameInput = false);
    float AddAndGetPitchAdd(float afX, bool abFrameInput = false);

private:
    void SetSwayActive(bool abX);
    void UpdateSway(double adFixedDelta);

    float mfStartSwayMaxSanity;

    bool mbSwayActive;

    tVector2fList mlstPrevAdd;
    cVector2f mvSwayAdd;
    int mlMaxPositions;

    float mfSwayAlpha;

    cVector2f mvNextAdd;
};

//----------------------------------------------

class cLuxPlayerSpawnPS_SpawnPos
{
public:
    cParticleSystem *mpPS;
    cVector3f mvPos;
    cVector3f mvLastLocalPos;
};

class cLuxPlayerSpawnPS : public iLuxPlayerHelper
{
public:
    cLuxPlayerSpawnPS (cLuxPlayer *apPlayer);
    ~cLuxPlayerSpawnPS();

    void Reset();

    void Start(const tString& asFileName);
    void Stop();

    void RespawnAll();

    bool IsActive()
    {
        return mbActive;
    }
    const tString &GetFileName()
    {
        return msFileName;
    }

    void Update(double adFixedDelta);
private:
    void GenerateAllSpawnPos();
    cParticleSystem *CreatePS(cLuxPlayerSpawnPS_SpawnPos *apSpawnPos);

    void DestroyAllSpawnPoints();

    bool LoadSpawnPSFile(const tString& asFileName);

    bool mbActive;
    tString msFileName;

    tString msParticleSystem;
    float mfHeightFromFeet;
    float mfHeightAddMin;
    float mfHeightAddMax;
    float mfDensity;
    float mfRadius;
    cColor mPSColor;
    bool mbFadePS;
    float mfPSMinFadeStart;
    float mfPSMinFadeEnd;
    float mfPSMaxFadeStart;
    float mfPSMaxFadeEnd;

    std::vector<cLuxPlayerSpawnPS_SpawnPos> mvSpawnPos;
};

//----------------------------------------------

class cLuxPlayerHurt : public iLuxPlayerHelper
{
public:
    cLuxPlayerHurt (cLuxPlayer *apPlayer);
    ~cLuxPlayerHurt();

    void Reset();

    void Update(double adFixedDelta);

    void OnDraw(double adFrameTime);

private:
    std::vector<cGuiGfxElement *> mvNoiseGfx;
    float mfEffectStartHealth;
    float mfMinMoveMul;
    float mfMaxPantCount;
    float mfPantSpeed;
    float mfPantSize;

    int mlCurrentNoise;
    float mfNoiseUpdateCount;

    float mfNoiseAlpha;
    float mfNoiseFreq;
    cColor mNoiseColor;

    float mfPantCount;
    float mfPantPosAdd;
    float mfPantPosAddVel;
    float mfPantPosAddDir;
    float mfAlpha;

    float mfHealthRegainSpeed;
    float mfHealthRegainLimit;
};

//----------------------------------------------

class cLuxFlashbackData
{
public:
    cLuxFlashbackData() {}
    cLuxFlashbackData(const tString &asFile, const tString &asCallback) : msFile(asFile), msCallback(asCallback) {}

    tString msFile;
    tString msCallback;
};

typedef std::list<cLuxFlashbackData> tLuxFlashbackDataList;
typedef tLuxFlashbackDataList::iterator tLuxFlashbackDataListIt;

//----------------------------------------------

class cLuxPlayerFlashback : public iLuxPlayerHelper
{
    friend class cLuxPlayer_SaveData;
public:
    cLuxPlayerFlashback (cLuxPlayer *apPlayer);
    ~cLuxPlayerFlashback ();

    void Reset();

    void Start(const tString &asFlashbackFile, const tString &asCallback);

    void Update(double adFixedDelta);

    void OnDraw(double adFrameTime);

    bool IsActive()
    {
        return mbActive;
    }

private:
    void LoadAndPlayFlashbackFile(const tString& asFlashbackFile);

    //Data
    float mfRadialBlurSize;
    float mfRadialBlurStartDist;
    float mfWorldSoundVolume;
    float mfMoveSpeedMul;
    float mfRunSpeedMul;

    //Variables
    float mfFlashDelay;
    bool mbActive;
    float mfFlashbackStartCount;
    tString msFlashbackFile;
    tString msCallback;
    tLuxFlashbackDataList mlstFlashbackQueue;

};

//---------------------------------------------

class cLuxPlayerLookAt : public iLuxPlayerHelper
{
    friend class cLuxPlayer_SaveData;
public:
    cLuxPlayerLookAt(cLuxPlayer *apPlayer);
    ~cLuxPlayerLookAt();

    void Update(double adFixedDelta);

    void Reset();

    void SetTarget(const cVector3f &avTargetPos, float afSpeedMul, float afMaxSpeed, const tString& asAtTargetCallback);

    void SetActive(bool abX);
    bool IsActive()
    {
        return mbActive;
    }

private:
    bool mbActive;
    cVector3f mvTargetPos;

    cVector3f mvCurrentSpeed;
    float mfSpeedMul;
    float mfMaxSpeed;

    tString msAtTargetCallback;

    float mfDestFovMul;
    float mfFov;
    float mfFovSpeed;
    float mfFovMaxSpeed;
};

//----------------------------------------------

class cLuxPlayerSanity : public iLuxPlayerHelper
{
    friend class cLuxPlayer_SaveData;
public:
    cLuxPlayerSanity(cLuxPlayer *apPlayer);
    ~cLuxPlayerSanity();

    void Reset();

    void StartHit();
    void SetSanityLost();

    void Update(double adFixedDelta);

    void OnDraw(double adFrameTime);

    float GetAtLowSanityCount()
    {
        return mfAtLowSanityCount;
    }

private:
    float GetCurrentSizeMul();

    void UpdateInsaneEffects(double adFixedDelta);
    void UpdateCheckEnemySeen(double adFixedDelta);
    void UpdateHit(double adFixedDelta);

    void UpdateInsanityVisuals(double adFixedDelta);
    void UpdateEnemySeenEffect(double adFixedDelta);
    void UpdateLosingSanity(double adFixedDelta);
    void UpdateLowSanity(double adFixedDelta);

    float mfHitAlpha;
    bool mbHitActive;
    float mfSanityLostCount;

    float mfAtLowSanityCount;

    bool mbHitIsUpdated;
    bool mbSanityLostIsUpdated;

    float mfPantCount;

    float mfCheckEnemySeenCount;

    bool mbSanityEffectUpdated;

    float mfT;
    float mfInsaneWaveAlpha;

    float mfSanityDrainCount;
    float mfSanityDrainVolume;
    float mfSanityHeartbeatCount;

    float mfSeenEnemyCount;
    bool mbEnemyIsSeen;

    float mfShowHintTimer;

    //////////////
    // Data
    float mfHitZoomInSpeed;
    float mfHitZoomOutSpeed;
    float mfHitZoomInFOVMul;
    float mfHitZoomInAspectMul;

    float mfSanityRegainSpeed;
    float mfSanityRegainLimit;
    float mfSanityVeryLowLimit;
    float mfSanityEffectsStart;

    float mfSanityWaveAlphaMul;
    float mfSanityWaveSpeedMul;

    float mfSanityLowLimit;
    float mfSanityLowLimitMaxTime;
    float mfSanityLowNewSanityAmount;

    float mfCheckNearEnemyInterval;
    float mfNearEnemyDecrease;
    float mfNearCritterDecrease;
};

//----------------------------------------------

class cLuxPlayerLantern : public iLuxPlayerHelper
{
public:
    cLuxPlayerLantern(cLuxPlayer *apPlayer);
    ~cLuxPlayerLantern();

    void OnStart();
    void Update(double adFixedDelta);
    void Reset();

    void OnMapEnter(cLuxMap *apMap);
    void OnMapLeave(cLuxMap *apMap);

    void CreateWorldEntities(cLuxMap *apMap);
    void DestroyWorldEntities(cLuxMap *apMap);

    void SetActive(bool abX, bool abUseEffects, bool abCheckForOilAndItems = true, bool abCheckIfAllowed = true);
    bool IsActive()
    {
        return mbActive;
    }

    void SetDisabled(bool abX);
    bool GetDisabled()
    {
        return mbDisabled;
    }

    iLight *GetLight()
    {
        return mpLight;
    }

private:
    cColor mDefaultColor;
    float mfRadius;
    tString msGobo;
    bool mbCastShadows;
    cVector3f mvLocalOffset;
    tString msTurnOnSound;
    tString msTurnOffSound;
    tString msOutOfOilSound;
    tString msDisabledSound;
    float mfLowerOilSpeed;
    float mfFadeLightOilAmount;

    bool mbDisabled;
    bool mbActive;
    float mfAlpha;
    cLightPoint *mpLight;
};

//----------------------------------------------

class cLuxPlayerDeath : public iLuxPlayerHelper
{
public:
    cLuxPlayerDeath(cLuxPlayer *apPlayer);
    ~cLuxPlayerDeath();

    void LoadFonts();
    void LoadUserConfig();
    void SaveUserConfig();

    void Reset();

    void Start();

    void Update(double adFixedDelta);
    void PostUpdate(double adFixedDelta);

    void OnDraw(double adFrameTime);

    void OnPressButton();

    float GetFadeAlpha()
    {
        return mfFadeAlpha;
    }

    void DisableStartSound()
    {
        mbSkipStartSound = true;
    }

    bool ShowHint()
    {
        return mbShowHint;
    }
    void SetShowHint(bool abX)
    {
        mbShowHint = abX;
    }

    void ReleasePlayerFromLimbo()
    {
        mbHoldsPlayerInLimbo = false;
    }

    void SetHint(const tString& asCat, const tString& asEntry);
    const tString &GetHintCat()
    {
        return msHintCat;
    }
    const tString &GetHintEntry()
    {
        return msHintEntry;
    }

    void SetKeepPlayerInLimbo( bool abKeepPlayerInLimbo )
    {
        mbKeepPlayerInLimbo = abKeepPlayerInLimbo;
    }

private:
    void ResetGame();

    bool mbToMainMenu;
    bool mbActive;

    bool mbKeepPlayerInLimbo;
    bool mbHoldsPlayerInLimbo;
    bool mbHasRunCheckPointCallbackScript;

    bool mbShowHint;

    int mlState;

    tString msStartSound;
    tString msAwakenSound;

    bool mbSkipStartSound;

    float mfHeightAddSpeed;
    float mfRollSpeed;

    float mfHeightAddGoal;
    float mfHeightAddGoalCrouch;
    float mfFadeOutTime;

    float mfMaxSanityGain;
    float mfMaxHealthGain;
    float mfMaxOilGain;
    float mfMinSanityGain;
    float mfMinHealthGain;
    float mfMinOilGain;

    float mfHeightAdd;
    float mfRoll;

    float mfT;

    float mfMinHeightAdd;

    tString msHintCat;
    tString msHintEntry;

    tWString msCurrentHintText;

    cGuiGfxElement *mpWhiteModGfx;

    iFontData *mpFont;

    float mfFadeAlpha;
    float mfTextAlpha1;
    float mfTextAlpha2;
    float mfTextOnScreenCount;
    float mfWhiteCount;

    cLinearOscillation mFlashOscill;

    cSoundEntry *mpVoiceEntry;
    int mlVoiceEntryId;
};


//----------------------------------------------

class cLuxPlayerLean : public iLuxPlayerHelper
{
public:
    cLuxPlayerLean(cLuxPlayer *apPlayer);
    ~cLuxPlayerLean();

    void CreateWorldEntities(cLuxMap *apMap);
    void DestroyWorldEntities(cLuxMap *apMap);

    void Update(double adFixedDelta);

    void SetLean(float afMul);
    void AddLean(float afAdd);

    void Reset();
private:
    iCollideShape *mpHeadShape;

    float mfDir;
    float mfDirAdd;
    float mfMovement;
    float mfRotation;

    float mfMaxMovement;
    float mfMaxRotation;
    float mfMaxTime;

    float mfMoveSpeed;

    bool mbIntersect;

    bool mbPressed;
};

//----------------------------------------------

class cLuxPlayerDamageData
{
public:
    std::vector<cGuiGfxElement *> mvImages;
};

class cLuxPlayerHudEffect_Splash
{
public:
    cGuiGfxElement *mpImage;
    cVector3f mvPos;
    cVector2f mvSize;
    float mfAlpha;
    float mfAlphaMul;

    cVector3f mvPosVel;
    cVector2f mvSizeVel;
    float mfAlphaVel;
    float mfAlphaMoveStart;
};

typedef std::list<cLuxPlayerHudEffect_Splash> cLuxPlayerHudEffect_SplashList;
typedef cLuxPlayerHudEffect_SplashList::iterator cLuxPlayerHudEffect_SplashListIt;

class cLuxPlayerHudEffect : public iLuxPlayerHelper
{
public:
    cLuxPlayerHudEffect(cLuxPlayer *apPlayer);
    ~cLuxPlayerHudEffect();

    void AddDamageSplash(eLuxDamageType aType);
    void Flash(const cColor& aColor, eGuiMaterial aFlashMaterial, float afInTime, float afOutTime);

    void OnDraw(double adFrameTime);
    void Update(double adFixedDelta);
    void Reset();

private:
    void DrawSplashes(double adFrameTime);
    void UpdateSplashes(double adFixedDelta);

    void DrawFlash(double adFrameTime);
    void UpdateFlash(double adFixedDelta);

    void LoadDamageData(cLuxPlayerDamageData *apData, const tString& asName);

    std::vector<cLuxPlayerDamageData> mvDamageTypes;
    cLuxPlayerHudEffect_SplashList mlstSplashes;

    cGuiGfxElement *mpFlashGfx;
    cColor mFlashColor;
    eGuiMaterial mFlashMaterial;
    float mfFlashAlphaSpeed;
    float mfFlashAlphaOutSpeed;
    float mfFlashAlpha;
    bool mbFlashActive;
};


//----------------------------------------------

class cLuxPlayerLightLevel : public iLuxPlayerHelper
{
public:
    cLuxPlayerLightLevel(cLuxPlayer *apPlayer);
    ~cLuxPlayerLightLevel();

    void OnStart();
    void Update(double adFixedDelta);
    void Reset();

    void OnMapEnter(cLuxMap *apMap);

    float GetExtendedLightLevel()
    {
        return mfExtendedLightLevel;
    }
    float GetNormalLightLevel()
    {
        return mfNormalLightLevel;
    }

private:
    float mfExtendedLightLevel;    //Uses longer range on point lights
    float mfNormalLightLevel;    //Uses normal radius
    float mfUpdateCount;

    float mfRadiusAdd;
};

//----------------------------------------------

class cLuxPlayerInDarkness : public iLuxPlayerHelper
{
    friend class cLuxPlayer_SaveData;
public:
    cLuxPlayerInDarkness(cLuxPlayer *apPlayer);
    ~cLuxPlayerInDarkness();

    void OnStart();
    void Update(double adFixedDelta);
    void Reset();

    void OnMapEnter(cLuxMap *apMap);
    void OnMapLeave(cLuxMap *apMap);

    void CreateWorldEntities(cLuxMap *apMap);
    void DestroyWorldEntities(cLuxMap *apMap);

    cLightPoint *GetAmbientLight()
    {
        return mpAmbientLight;
    }

    bool InDarkness();

    void SetActive(bool abX);

private:
    cSoundHandler *mpSoundHandler;

    float mfMinDarknessLightLevel;

    float mfAmbientLightMinLightLevel;
    float mfAmbientLightRadius;
    float mfAmbientLightIntensity;
    float mfAmbientLightFadeInTime;
    float mfAmbientLightFadeOutTime;
    cColor mAmbientLightColor;

    tString msLoopSoundFile;
    float mfLoopSoundVolume;
    float mfLoopSoundStartupTime;
    float mfLoopSoundFadeInSpeed;
    float mfLoopSoundFadeOutSpeed;

    float mfSanityLossPerSecond;
    float mfSanityLossMul;

    bool mbInDarkness;
    float mfShowHintTimer;

    cSoundEntry *mpLoopSound;
    float mfLoopSoundCount;

    cLightPoint *mpAmbientLight;
    bool mbAmbientLightIsOn;

    bool mbActive;
};

//----------------------------------------------


#endif // LUX_MAP_HANDLER_H
