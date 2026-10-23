#ifndef HPL_PHYSICS_ROPE_H
#define HPL_PHYSICS_ROPE_H

#include "physics/PhysicsTypes.h"
#include "system/SystemTypes.h"
#include "math/MathTypes.h"
#include "math/PidController.h"

#include "physics/VerletParticle.h"

namespace hpl
{

class iLowLevelGraphics;

class iPhysicsWorld;
class iPhysicsBody;
class iCollideShape;
class cVerletParticle;

class iPhysicsRope;

class cSoundEntity;

//------------------------------------------

class cPhysicsRopeAttachment
{
public:
    iPhysicsBody *mpBody;
    cVerletParticle *mpParticle;
    cVector3f mvBodyLocalPos;
};

//------------------------------------------

class iPhysicsRope : public iVerletParticleContainer
{
public:
    iPhysicsRope(const tString &asName, iPhysicsWorld *apWorld, const cVector3f &avStartPos, const cVector3f &avEndPos);
    virtual ~iPhysicsRope();

    /////////////////////////////
    // General
    void UpdateBeforeSimulate(double adFixedDelta);
    void UpdateAfterSimulate(double adFixedDelta);

    void RemoveAttachedBody(iPhysicsBody *apBody, bool abRemoveContainerFromBody = true);


    /////////////////////////////
    // Motor
    void SetMotorActive(bool abX);
    bool GetMotorActive()
    {
        return mbMotorActive;
    }

    void SetMotorWantedLength(float afX)
    {
        mfMotorWantedLength = afX;
    }
    void SetMotorSpeedMul(float afX)
    {
        mfMotorSpeedMul = afX;
    }
    void SetMotorMaxSpeed(float afX)
    {
        mfMotorMaxSpeed = afX;
    }
    void SetMotorMinSpeed(float afX)
    {
        mfMotorMinSpeed = afX;
    }

    float GetMotorWantedLength()
    {
        return mfMotorWantedLength;
    }
    float GetMotorSpeedMul()
    {
        return mfMotorSpeedMul;
    }
    float GetMotorMaxSpeed()
    {
        return mfMotorMaxSpeed;
    }
    float GetMotorMinSpeed()
    {
        return mfMotorMinSpeed;
    }

    void SetMotorSound(const tString& asSound)
    {
        msMotorSound = asSound;
    }
    void SetMotorSoundStartSpeed(float afX)
    {
        mfMotorSoundStartSpeed = afX;
    }
    void SetMotorSoundStopSpeed(float afX)
    {
        mfMotorSoundStopSpeed = afX;
    }

    const tString &GetMotorSound()
    {
        return msMotorSound;
    }
    float GetMotorSoundStartSpeed()
    {
        return mfMotorSoundStartSpeed;
    }
    float GetMotorSoundStopSpeed()
    {
        return mfMotorSoundStopSpeed;
    }


    /////////////////////////////
    // Auto Move
    void SetAutoMoveActive(bool abX);
    void SetAutoMoveAcc(float afX)
    {
        mfAutoMoveAcc = afX;
    }
    void SetAutoMoveMaxSpeed(float afX)
    {
        mfAutoMoveMaxSpeed = afX;
    }
    void SetAutoMoveSpeed(float afX)
    {
        mfAutoMoveSpeed = afX;
    }

    bool GetAutoMoveActive();
    float GetAutoMoveAcc()
    {
        return mfAutoMoveAcc;
    }
    float GetAutoMoveMaxSpeed()
    {
        return mfAutoMoveMaxSpeed;
    }
    float GetAutoMoveSpeed()
    {
        return mfAutoMoveSpeed;
    }

    /////////////////////////////
    // Properties
    cVerletParticle *GetStartParticle()
    {
        return mlstParticles.front();
    }
    cVerletParticle *GetEndParticle()
    {
        return mlstParticles.back();
    }

    void SetAttachedStartBody(iPhysicsBody *apBody);
    void SetAttachedEndBody(iPhysicsBody *apBody);

    iPhysicsBody *GetAttachedStartBody()
    {
        return mvAttachedBody[0].mpBody;
    }
    iPhysicsBody *GetAttachedEndBody()
    {
        return mvAttachedBody[1].mpBody;
    }

    cPhysicsRopeAttachment *GetAttachment(int alIdx)
    {
        return &mvAttachedBody[alIdx];
    }

    void SetTotalLength(float afX);
    float GetTotalLength()
    {
        return mfTotalLength;
    }

    void SetMinTotalLength(float afX);
    void SetMaxTotalLength(float afX);
    float GetMinTotalLength()
    {
        return mfMinTotalLength;
    }
    float GetMaxTotalLength()
    {
        return mfMaxTotalLength;
    }

    void SetSegmentLength(float afX);
    float GetSegmentLength()
    {
        return mfSegmentLength;
    }

    void SetMaxIterations(int alX)
    {
        mlMaxIterations = alX;
    }
    int GetMaxIterations()
    {
        return mlMaxIterations;
    }

    void SetStrength(float afX)
    {
        mfStrength  = afX;
    }
    float GetStrength()
    {
        return mfStrength;
    }

    void SetStiffness(float afX)
    {
        mfStiffness  = afX;
    }
    float GetStiffness()
    {
        return mfStiffness;
    }

    void SetCollideAttachments(bool abX)
    {
        mbCollideAttachments  = abX;
    }
    bool GetCollideAttachments()
    {
        return mbCollideAttachments;
    }

    /////////////////////////////
    // Debug
    void RenderDebug(iLowLevelGraphics *apLowLevel);

protected:
    bool CheckParticleBodyCollision(iPhysicsBody *apBody);
    bool CheckSpecificDataSleeping();
    bool CheckSpecificDataAwake();
    void SetSpecificDataSleeping(bool abSleeping);

    void UpdateMovement(double adFixedDelta);
    void UpdateMotor(double adFixedDelta);
    void UpdateMotorAndAutoMove(double adFixedDelta);

    void UpdateAttachedParticlePositions(double adFixedDelta);
    void UpdateAttachedBodies(double adFixedDelta);
    void UpdateConstraints(double adFixedDelta);
    void CalculateSmoothPositions(double adFixedDelta);

    void BuildRopeParticles();

    void SetAttachedBody(int alIdx, cVerletParticle *apParticle, iPhysicsBody *apBody);

    cVector3f GetStartDirection();

    cPidControllerVec3 mForcePid[2];

    cPhysicsRopeAttachment mvAttachedBody[2];

    bool mbCollideAttachments;

    int mlUniqueID;

    bool mbMotorActive;
    float mfMotorWantedLength;
    float mfMotorSpeedMul;
    float mfMotorMinSpeed;
    float mfMotorMaxSpeed;

    tString msMotorSound;
    float mfMotorSoundStartSpeed;
    float mfMotorSoundStopSpeed;
    cSoundEntity *mpMotorSoundEntity;
    int mlMotorSoundEntityID;

    bool mbAutoMoveActive;
    float mfAutoMoveAcc;
    float mfAutoMoveMaxSpeed;
    float mfAutoMoveSpeed;

    float mfTotalLength;
    float mfMinTotalLength;
    float mfMaxTotalLength;
    float mfSegmentLength;

    float mfFirstSegmentLength;

    int mlMaxIterations;

    float mfStrength;
    float mfStiffness;

    bool mbHasUpdated;

public:
    /** Tick in which the smooth positions were last calculated (for render interpolation). */
    int GetRenderInterpTick()
    {
        return mlRenderInterpTick;
    }
private:
    int mlRenderInterpTick;
};
};
#endif // HPL_PHYSICS_ROPE_H
