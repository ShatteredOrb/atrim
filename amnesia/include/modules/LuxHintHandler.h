#ifndef LUX_HINT_HANDLER_H
#define LUX_HINT_HANDLER_H

//----------------------------------------------

#include "LuxBase.h"

//---------------------------------------

#if USE_GAMEPAD

class cLuxHintIcon
{
public:
    cGuiGfxElement *mpGuiIcon;
    cVector3f mvPosition;
    cVector2f mvSize;
    int mlCharacterPosition;
};

#endif

//----------------------------------------

class cLuxHintHandler : public iLuxUpdateable
{
    friend class cLuxHintHandler_SaveData;
public:
    cLuxHintHandler();
    ~cLuxHintHandler();

    void LoadFonts();
    void OnStart();
    void Update(double adFixedDelta);
    void LoadUserConfig();
    void SaveUserConfig();
    void Reset();
    void OnDraw(double adFrameTime);

    void DrawHintText(double adFrameTime, cGuiSet *apGuiSet);
    void UpdateHintText(double adFixedDelta);

    void SetActive(bool abX);
    bool IsActive()
    {
        return mbActive;
    }

    /**
    * if time shown is <=0 then the life time is calculated based on string length.
    */
    bool Add(const tString &asName, const tWString &asMessage, float afTimeShown);
    bool Remove(const tString &asName);

    void Block(const tString &asName);
    void UnBlock(const tString &asName);

private:
#if USE_GAMEPAD
    void ParseStringForGamepadIcons();
    tWString AddGamepadIconAtPosition(const tWString& asCommand, int alPosition);
#endif

    //////////////////
    // Data
    iFontData *mpFont;

    float mfYPos;
    cVector2f mvFontSize;

    float mfTextDurationMul;

    //////////////////
    // Variables
    bool mbActive;

    tStringSet m_setHintsGiven;
    tStringSet m_setHintsBlocked;

    cLinearOscillation mHintOscill;

    cInterpolatedFloat mfAlpha;
    float mfShowTextCount;
    tWString msCurrentText;

#if USE_GAMEPAD
    cGuiGfxElement *mpGamepadButtonIcon[eGamepadButton_LastEnum];
    cGuiGfxElement *mpGamepadHatIcon[4];
    cGuiGfxElement *mpGampadAxisIcon[eGamepadAxis_LastEnum * 2];

    std::vector<cLuxHintIcon> mvHintIcons;
#endif
};

//----------------------------------------------


#endif // LUX_HINT_HANDLER_H
