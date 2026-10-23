#ifndef LUX_INPUT_HANDLER_H
#define LUX_INPUT_HANDLER_H

//----------------------------------------------

#include "LuxBase.h"
#include "LuxTypes.h"

//----------------------------------------------

class cLuxAction
{
public:
    cLuxAction() : msName("") {}
    cLuxAction(const tString& asName,
               int alId,
               bool abConfigurable,
               eLuxActionCategory aCat) : msName(asName), mlId(alId), mbConfigurable(abConfigurable), mCat(aCat) {}

    tString msName;
    int mlId;
    bool mbConfigurable;
    eLuxActionCategory mCat;
};

typedef std::vector<cLuxAction *> tLuxActionVec;
typedef tLuxActionVec::iterator tLuxActionVecIt;

//----------------------------------------------

class cLuxInput
{
public:
    cLuxInput() : msInputType("") {}
    cLuxInput(const tString& asInputType, int alValue, int alActionId) :
        msInputType(asInputType), mlValue(alValue), mlActionId(alActionId) {}


    tString msInputType;
    int mlValue;
    int mlActionId;
};

typedef std::vector<cLuxInput *> tLuxInputVec;
typedef tLuxInputVec::iterator    tLuxInputVecIt;

//----------------------------------------------

class cLuxPlayer;

//----------------------------------------------


class cLuxInputHandler : public iLuxUpdateable
{
public:
    cLuxInputHandler();
    ~cLuxInputHandler();

    void LoadUserConfig();
    void SaveUserConfig();

    void OnStart();
    void Update(double adFixedDelta);
    void OnFrameInput(double adFrameTime);
    void Reset();

    tWString GetInputName(const tString& asActionName);

    void ChangeState(eLuxInputState aState);
    eLuxInputState GetState()
    {
        return mState;
    }

    bool GetInvertMouse()
    {
        return mbInvertMouse;
    }
    void SetInvertMouse(bool abX)
    {
        mbInvertMouse = abX;
    }

    bool GetSmoothMouse()
    {
        return mbSmoothMouse;
    }
    void SetSmoothMouse(bool abX)
    {
        mbSmoothMouse = abX;
    }

    float GetMouseSensitivity()
    {
        return mfMouseSensitivity;
    }
    void SetMouseSensitivity(float afX);

#if USE_GAMEPAD
    bool GetInvertGamepadLook()
    {
        return mbGamepadLookInvert;
    }
    void SetInvertGamepadLook(bool abX)
    {
        mbGamepadLookInvert = abX;
    }

    float GetGamepadLookSensitivity()
    {
        return mfGamepadLookSensitivity;
    }
    void SetGamepadLookSensitivity(float afX);

    iGamepad *GetGamepad()
    {
        return mpPad;
    }
#endif

    cLuxAction   *GetActionByName(const tString& asName);
    cLuxAction      *GetActionById(int alId);
    tLuxActionVec GetActionsByCategory(eLuxActionCategory aCat);

    tLuxInputVec GetDefaultInputsByActionId(int alId);

    void ResetSmoothMousePos();
    cVector2f GetSmoothMousePos(const cVector2f& avRelPosMouse);

    bool FrameMouseLookActive();

#if USE_GAMEPAD
    bool IsGamepadPresent();

    void AppDeviceWasPlugged();
    void AppDeviceWasRemoved();
#endif

private:
    void UpdateGlobalInput();
#if USE_GAMEPAD
    bool UpdateGamepadUIInput();
#endif

    void UpdateGameInput();
    void UpdateGamePlayerInput();
    void UpdateGameMessageInput();
    void UpdateGameEffectInput();

    void UpdatePreMenuInput();
    void UpdateMainMenuInput();
    void UpdateInventoryInput();
    void UpdateJournalInput();
    void UpdateDebugInput();
    void UpdateCreditsInput();
    void UpdateLoadScreenInput();

    bool CurrentStateSendsInputToGui();

    cVector2f GetFrameSmoothMouseDelta(const cVector2f& avDelta, float afFrameTime);
    void ResetFrameSmoothMouse();

    void CreateActions();

    void CreateSubAction(cAction *apAction, const tStringVec& avType, int alValue);

    tStringVec GetInputValueStrings(const tString& asX);

    bool CreateSubActionFromInputString(cAction* apAction, const tString& asInputString);

    bool ShowMouseOnMouseInput();

#if USE_GAMEPAD
    void SetUpGamepad();
#endif

    cGraphics *mpGraphics;
    cInput *mpInput;

    cLuxPlayer *mpPlayer;

    eLuxInputState mState;

    bool mbSmoothMouse;
    bool mbInvertMouse;

    double mfMouseActiveAt;

    float mfMouseSensitivity;

    int mlMaxSmoothMousePos;
    float mfPrevSmoothMousePosMul;
    tVector2fList mlstSmoothMousePos;

    cVector2l mvLastAbsMousePos;

    class cFrameMouseSample
    {
    public:
        cVector2f mvVelocity;
        float mfDuration;
    };
    std::list<cFrameMouseSample> mlstFrameMouseSamples;
    bool mbFrameLookWasActive;

    cVector2l mvLastFrameGuiMousePos;

#if USE_GAMEPAD
    float mfGamepadWalkSensitivity;
    float mfGamepadLookSensitivity;
    bool mbGamepadLookInvert;
    iGamepad *mpPad;
    bool mbGamepadUIInput;
#endif
};

//----------------------------------------------


#endif // LUX_INPUT_HANDLER_H
