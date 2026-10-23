#ifndef LUX_ENEMY_MOVER_H
#define LUX_ENEMY_MOVER_H

//----------------------------------------------

#include "LuxBase.h"

//----------------------------------------------

class iLuxEnemy;

//----------------------------------------------

enum eLuxEnemyMoveState
{
    eLuxEnemyMoveState_Backward,
    eLuxEnemyMoveState_Stopped,
    eLuxEnemyMoveState_Walking,
    eLuxEnemyMoveState_Jogging,
    eLuxEnemyMoveState_Running,

    eLuxEnemyMoveState_LastEnum
};

//----------------------------------------------

class cLuxEnemyMover
{
    friend class cLuxEnemyMover_SaveData;
    friend class iLuxEnemy;
public:
    cLuxEnemyMover(iLuxEnemy *apEnemy, iCharacterBody *apCharBody);
    virtual ~cLuxEnemyMover();

    //////////////////////
    //General
    void SetupAfterLoad(cWorld *apWorld);

    void OnUpdate(double adFixedDelta);

    //////////////////////
    //Actions
    void MoveToPos(const cVector3f& avFeetPos);

    void TurnToPos(const cVector3f& avFeetPos);
    void TurnToAngle(float afAngle);

    void MoveBackwardsToPos(const cVector3f& avFeetPos);
    void TurnAwayFromPos(const cVector3f& avFeetPos);

    void UseMoveStateAnimations();

    void ForceMoveState(eLuxEnemyMoveState aMoveState);

    //////////////////////
    //Properties

    //This is used by enemy when caclulting final max speed.
    float CalculateSpeedMul(double adFixedDelta);

    //This get the speed along the movement direction (so it does not accound for falling speeds, climbing, etc
    float GetMoveSpeed();

    //This gets speed / wanted_speed
    float GetWantedSpeedAmount();

    void SetOverideMoveState(bool abX);
    bool GetOverideMoveState()
    {
        return mbOverideMoveState;
    }

    float GetStuckCounter()
    {
        return mfStuckCounter;
    }
    float GetMaxStuckCounter()
    {
        return mfMaxStuckCounter;
    }
    bool GetStuckCounterIsAtMax()
    {
        return mfStuckCounter >= mfMaxStuckCounter;
    }
    void ResetStuckCounter()
    {
        mfStuckCounter = 0;
    }

    void StopTurning()
    {
        mbTurning = false;
    }

    void SetWallAvoidanceActive(bool abX)
    {
        mbWallAvoidActive = abX;
    }

    void SetupWallAvoidance(float afRadius, float afSteerAmount, int alSamples);

    //////////////////////
    //Update
    void UpdateWallAvoidance(double adFixedDelta);

    //////////////////////
    //Debug
    void OnRenderSolid(cRendererCallbackFunctions* apFunctions);

private:
    void UpdateStuckCounter(double adFixedDelta);
    void UpdateTurning(double adFixedDelta);
    void UpdateMoveAnimation(double adFixedDelta);
    void UpdateStepEffects(double adFixedDelta);
    void ConvertLocalDirTo2D(cVector3f& avLocalDir);
    cMatrixf GetMovementDirectionMatrix();

    ///////////////////
    // Data
    iLuxEnemy *mpEnemy;
    iCharacterBody *mpCharBody;

    float mfStuckLimit;
    float mfMaxStuckCounter;

    float mfWallAvoidRadius;
    float mfWallAvoidSteerAmount;

    static std::vector<cVector3f> mvPrecalcSampleDirs;

    ///////////////////
    // Variables
    bool mbTurning;
    float mfTurnGoalAngle;
    float mfTurnSpeed;
    float mfTurnBreakAcc;

    cVector3f mvCurrentGoal;

    float mfStuckCounter;

    cVector3f mvSteeringVec;

    eLuxEnemyMoveState mMoveState;
    bool mbOverideMoveState;

    bool mbWallAvoidActive;
    float mfWallAvoidCount;
    std::vector<cVector3f> mvSampleRays;
    std::vector<cVector3f> mvSampleRayBaseDir;
    std::vector<bool> mvSampleRaysCollide;
    std::vector<float> mvSampleRaysAmount;
    std::vector<int> mvSamplePartitionUsed;
    int mlSampleMaxPartCount;
};

//----------------------------------------------


#endif // LUX_ENEMY_MOVER_H
