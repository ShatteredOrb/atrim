#ifndef LUX_PROP_CRITTER_BASE_H
#define LUX_PROP_CRITTER_BASE_H

//----------------------------------------------

#include "LuxProp.h"

//----------------------------------------------

class iLuxProp_CritterBase;
class cLuxCritterRayCallback : public iPhysicsRayCallback
{
public:
    cLuxCritterRayCallback(iLuxProp_CritterBase *apCritterBase) : mpCritterBase(apCritterBase) {}

    void Reset();

    bool BeforeIntersect(iPhysicsBody *pBody);
    bool OnIntersect(iPhysicsBody *pBody, cPhysicsRayParams *apParams);

    bool GetIntersected()
    {
        return mbIntersected;
    }
    const cVector3f &GetNormal()
    {
        return mvNormal;
    }
    const cVector3f &GetPos()
    {
        return mvPos;
    }
    iPhysicsBody *GetBody()
    {
        return mpBody;
    }
    float GetT()
    {
        return mfClosestT;
    }
private:
    float mfClosestT;
    bool mbIntersected;
    cVector3f mvNormal;
    cVector3f mvPos;
    iPhysicsBody *mpBody;

    iLuxProp_CritterBase *mpCritterBase;
};

//----------------------------------------------

class iLuxProp_CritterBase_SaveData : public iLuxProp_SaveData
{
    kSerializableClassInit(iLuxProp_CritterBase_SaveData)
public:
    virtual ~iLuxProp_CritterBase_SaveData() {}

    cVector3f mvVel;
    cVector3f mvGravityVel;
    cVector3f mvSwarmPoint;

    int mlAnimState;
    cVector3f mvGroundNormal;
    bool mbColliding;
    bool mbUpdateAnimation;
};

//----------------------------------------------

enum eLuxCritterState
{
    eLuxCritterState_Idle,
    eLuxCritterState_Move,
    eLuxCritterState_Flee,
    eLuxCritterState_Hunt,
    eLuxCritterState_Hit,
    eLuxCritterState_Attack_1,
    eLuxCritterState_Attack_2,
    eLuxCritterState_LastEnum
};

//----------------------------------------------

class iLuxProp_CritterBase : public iLuxProp
{
    typedef iLuxProp super_class;
    friend class cLuxCritterRayCallback;
    friend class iLuxPropLoader_Critter;
public:
    iLuxProp_CritterBase(const tString &asName, int alID, cLuxMap *apMap);
    virtual ~iLuxProp_CritterBase();

    //////////////////////
    //General
    bool CanInteract(iPhysicsBody *apBody);
    bool OnInteract(iPhysicsBody *apBody, const cVector3f &avPos);

    void OnSetupAfterLoad(cWorld *apWorld);

    void OnResetProperties();

    void UpdatePropSpecific(double adFixedDelta);

    void BeforePropDestruction();

    eLuxFocusCrosshair GetFocusCrosshair(iPhysicsBody *apBody, const cVector3f &avPos);

    void OnRenderSolid(cRendererCallbackFunctions* apFunctions);

    void OnHealthChange();
    void OnDamage(float afAmount, int alStrength);

    bool CausesSanityDecrease()
    {
        return mbCausesSanityDecrease;
    }

    //////////////////////
    //Properties

    //////////////////////
    //Connection callbacks
    void OnConnectionStateChange(iLuxEntity *apEntity, int alState) {}

    //////////////////////
    //Save data stuff
    virtual void SaveToSaveData(iLuxEntity_SaveData* apSaveData);
    virtual void LoadFromSaveData(iLuxEntity_SaveData* apSaveData);
    virtual void SetupSaveData(iLuxEntity_SaveData *apSaveData);


protected:
    virtual void OnDamageCritter(float afAmount) {}
    virtual void OnKillCritter() {}

    virtual void UpdateCritterSpecific(double adFixedDelta) = 0;
    virtual void OnShapeCollision(const cVector3f& avPushVec, double adFixedDelta) {}

    void CreateOrthoVectors(cVector3f &avRight, cVector3f& avUp, cVector3f &avForward);
    float GetDistanceToPlayer();
    float GetDistanceToPlayer2D();

    cMatrixf GetAttackMatrix(const cVector3f& avDir);
    bool Attack(const cVector3f& avDir);

    cVector3f GetWanderAdd(float afLength, float afRadius, double adFixedDelta);
    cVector3f GetTowardPlayerAdd(bool abDependOnDistance, double adFixedDelta);

    void UpdateMesh(double adFixedDelta);
    void UpdateMovement(double adFixedDelta);

    void CheckRayCollision(const cVector3f& avVelAdd, const cVector3f& avGravityAdd, double adFixedDelta);
    void CheckShapeCollision(const cVector3f& avVelAdd, const cVector3f& avGravityAdd, double adFixedDelta);

    //Data
    bool mbUseRayCollision;

    tString msDeathEntity;
    tString msDeathPS;
    tString msDeathSound;

    float mfAttackSizeRadius;
    cVector3f mvAttackOffset;
    cVector2f mvAttackDamageMinMax;
    cVector2f mvAttackForce;
    int mlAttackStrength;
    tString msAttackHitSound;

    cMatrixf m_mtxMeshOffset;

    iCollideShape *mpDamageShape;

    bool mbCausesSanityDecrease;

    //Vars
    int mlAnimState;
    cVector3f mvVel;
    cVector3f mvGravityVel;
    cVector3f mvGroundNormal;
    bool mbColliding;
    bool mbUpdateAnimation;


    cVector3f mvSwarmPoint;

    std::vector<cMatrixf> mvBaseMatrices;
    tVector3fList mlstFwdDirs;
    tVector3fList mlstUpDirs;

    iPhysicsBody *mpBody;

    cLuxCritterRayCallback *mpRayCallback;
};

//----------------------------------------------

class iLuxPropLoader_Critter : public iLuxPropLoader
{
public:
    iLuxPropLoader_Critter(const tString asName);
    virtual ~iLuxPropLoader_Critter() {}

    void LoadVariables(iLuxProp *apProp, cXmlElement *apRootElem);
    void LoadInstanceVariables(iLuxProp *apProp, cResourceVarsObject *apInstanceVars);

    virtual void LoadCritterVariables(iLuxProp *apProp, cXmlElement *apRootElem) = 0;
    virtual void LoadCritterInstanceVariables(iLuxProp *apProp, cResourceVarsObject *apInstanceVars) = 0;
};

//----------------------------------------------


#endif // LUX_PROP_CRITTER_BASE_H
