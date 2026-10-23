#ifndef LUX_HAND_OBJECT_LIGHT_SOURCE_H
#define LUX_HAND_OBJECT_LIGHT_SOURCE_H

//----------------------------------------------

#include "LuxHandObject.h"

//----------------------------------------------

enum eLampState
{
    eLampState_Normal,
    eLampState_FailureFlickering,
    eLampState_FailureDead
};

//----------------------------------------------

class cLuxHandObject_LightSource : public iLuxHandObject
{
public:
    cLuxHandObject_LightSource(const tString& asName, cLuxPlayerHands *apHands);
    ~cLuxHandObject_LightSource();

    void LoadImplementedVars(cXmlElement *apVarsElem);

    void ImplementedCreateEntity(cLuxMap *apMap);
    void ImplementedDestroyEntity(cLuxMap *apMap);

    void ImplementedReset();

    void Update(double adFixedDelta);

    bool DoAction(eLuxPlayerAction aAction, bool abPressed);
    bool AnimationIsOver();

    void SetFlickering(bool abX);
    bool GetFlickering()
    {
        return mbFlickering;
    }

    void SetFlickeringSpeed(float afX)
    {
        mfFlickeringSpeed = afX;
    }
    float GetFlickeringSpeed()
    {
        return mfFlickeringSpeed;
    }

private:
    void UpdateSwayPhysics(double adFixedDelta);
    float UpdateFlickering(double adFixedDelta);
    void StopDroneSound();

    bool mbFlickering;
    float mfFlickeringSpeed;
    int mlFlickeringState;

    float mfFlickerAmount;
    float mfFlickerTime;
    float mfFlickerPauseTime;
    float mfLastAppliedFlicker;

    float mfFadeInSpeed;
    float mfFadeOutSpeed;

    float mfStrobeDroneTime;
    cSoundEntry *mpDroneSound;
    int mlDroneSoundId;

    bool mbHasSwayPhysics;
    float mfMaxSwayVel;
    cVector2f mvSwayAngleLimits;
    cVector2f mvSwayDownAngleLimits;
    cVector3f mvSwayPinDir;
    float mfSwayGravity;
    float mfSwayFriction;
    float mfSwayPlayerSpeedMul;
    float mfSwayCameraRollMul;
    tString msSkipSwaySubMesh;

    float mfSwayAngle;
    float mfSwayVel;

    std::vector<cColor> mvLightFadeOutColor;
    std::vector<cColor> mvDefaultLightColors;
    std::vector<bool> mvDefaultLightFlicker;
    std::vector<cMatrixf> mvDefaultLightMatrix;
    std::vector<cMatrixf> mvDefaultSubMeshMatrix;
    std::vector<cMatrixf> mvDefaultBillboardMatrix;
};

//----------------------------------------------



#endif // LUX_HAND_OBJECT_LIGHT_SOURCE_H
