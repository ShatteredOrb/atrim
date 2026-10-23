#ifndef LUX_AREA_LIQUID_H
#define LUX_AREA_LIQUID_H

//----------------------------------------------

#include "LuxArea.h"

//----------------------------------------------

class cLuxArea_Liquid_SaveData : public iLuxArea_SaveData
{
    kSerializableClassInit(cLuxArea_Liquid_SaveData)
public:
    iLuxArea *CreateArea(cLuxMap *apMap);

    float mfDensity;
    float mfLinearViscosity;
    float mfAngularViscosity;

    tString msPhysicsMaterial;

    bool mbHasWaves;
    float mfWaveAmp;
    float mfWaveFreq;

    float mfPlayerSpeedMul;
    float mfMaxWaveDistanceSqr;

    float mfTimeCount;

    cPlanef mSurfacePlane;

    cVector3f mvPosition;
};

//----------------------------------------------

class cLuxArea_Liquid : public iLuxArea
{
    typedef iLuxArea super_class;
    friend class cLuxAreaLoader_Liquid;
public:
    cLuxArea_Liquid(const tString &asName, int alID, cLuxMap *apMap);
    virtual ~cLuxArea_Liquid();

    //////////////////////
    //General
    void SetupAfterLoad(cWorld *apWorld);
    void OnUpdate(double adFixedDelta);

    //////////////////////
    //Connection callbacks
    void OnConnectionStateChange(iLuxEntity *apEntity, int alState) {}

    //////////////////////
    //Save data stuff
    iLuxEntity_SaveData *CreateSaveData();
    virtual void SaveToSaveData(iLuxEntity_SaveData* apSaveData);
    virtual void LoadFromSaveData(iLuxEntity_SaveData* apSaveData);
    virtual void SetupSaveData(iLuxEntity_SaveData *apSaveData);

private:
    void DoBuoyancyOnBody(iPhysicsBody *apBody, float afSurfaceY, bool abInsideWater);
    void DoBuoyancyOnCharBody(iCharacterBody *apCharBody, float afSurfaceY, bool abInsideWater);

    void SplashEffect(iPhysicsBody *apBody, float afSurfaceY);

    /////////////////////////
    // Data
    float mfDensity;
    float mfLinearViscosity;
    float mfAngularViscosity;

    iPhysicsMaterial *mpPhysicsMaterial;

    bool mbHasWaves;
    float mfWaveAmp;
    float mfWaveFreq;

    float mfPlayerSpeedMul;
    float mfMaxWaveDistanceSqr;

    /////////////////////////
    // Variables
    float mfTimeCount;

    cPlanef mSurfacePlane;
};

//----------------------------------------------

class cLuxAreaLoader_Liquid : public iLuxAreaLoader
{
public:
    cLuxAreaLoader_Liquid(const tString& asName);
    ~cLuxAreaLoader_Liquid();

    iLuxArea *CreateArea(const tString& asName, int alID, cLuxMap *apMap);

    void LoadVariables(iLuxArea *apArea, cWorld *apWorld);
    void SetupArea(iLuxArea *apArea, cWorld *apWorld);

};

//----------------------------------------------


#endif // LUX_AREA_LIQUID_H
