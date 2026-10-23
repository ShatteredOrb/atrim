#ifndef LUX_PRE_MENU_H
#define LUX_PRE_MENU_H

//----------------------------------------------

#include "LuxBase.h"

enum eLuxPreMenuState
{
    eLuxPreMenuState_Initial,
    eLuxPreMenuState_FadeIn,
    eLuxPreMenuState_FadeOut,
    eLuxPreMenuState_FastFadeOut,
    eLuxPreMenuState_ShowPremenuSection,
    eLuxPreMenuState_Final,

    eLuxPreMenuState_LastEnum
};

//----------------------------------------------

class cLuxPreMenuTextElement
{
public:
    bool Load(cXmlElement* apElement, const cVector2f& avGuiSetSize);

    cWidgetLabel *CreateLabel(cGuiSet* apSet);

public:
    cVector2f mvFrameSize;

    tWString msText;
    tString msFontFile;
    cVector2f mvFontSize;
    cVector3f mvPos;
    cColor mColor;
    eFontAlign mAlign;

    float mfTime;
};

//----------------------------------------------

typedef std::list<cLuxPreMenuTextElement *>    tPreMenuTextList;
typedef tPreMenuTextList::iterator            tPreMenuTextListIt;

//----------------------------------------------

class cLuxPreMenuSection
{
public:
    cLuxPreMenuSection();
    ~cLuxPreMenuSection();

    bool Load(cXmlElement* apElement, const cVector2f& avGuiSetSize);

    void AddTextElement(cLuxPreMenuTextElement* apText);
    bool HasTextElements();

    const tPreMenuTextList &GetTextElements()
    {
        return mlstTextElements;
    }

    cGuiGfxElement *CreateBackground(cGui* apGui, cTextureManager* apTexMgr);
    float GetTime()
    {
        return mfTime;
    }

    bool ShowFirstStartOnly()
    {
        return mbShowFirstStartOnly;
    }

    bool HasGammaSettings()
    {
        return mbHasGammaSettings;
    }

public:
    tPreMenuTextList mlstTextElements;

    cColor mBackgroundColor;
    tString msBackgroundFile;

    bool mbHideGuiSet;
    bool mbHasGammaSettings;

    tString msMusic;
    float mfMusicVolume;
    float mfMusicFadeTime;

    float mfTime;

    bool mbAllowSkipping;
    bool mbShowFirstStartOnly;
};

//----------------------------------------------

class cLuxPreMenu : public iLuxUpdateable
{
public:
    cLuxPreMenu();
    ~cLuxPreMenu();

    void Update(double adFixedDelta);

    void OnEnterContainer(const tString& asOldContainer);
    void OnLeaveContainer(const tString& asNewContainer);

    void OnDraw(double adFrameTime);

    void ButtonPressed();

    void AppLostInputFocus();
    void AppGotInputFocus();
    cGuiSet *GetSet()
    {
        return mpGuiSet;
    }
    eLuxPreMenuState GetState()
    {
        return mCurrentState;
    }
    bool IsContinueButtonVisible()
    {
        return mpBContinue->IsVisible();
    }

private:
    void UpdateActions(double adFixedDelta);
    void UpdateState();

    void LoadPreMenuSections();

    void SetGammaValueToInput(float afGamma, bool abGenCallback = false);

    bool Gamma_ChangeValue(iWidget* apWidget, const cGuiMessageData& aData);
    kGuiCallbackDeclarationEnd(Gamma_ChangeValue);

    bool Continue_Pressed(iWidget* apWidget, const cGuiMessageData& aData);
    kGuiCallbackDeclarationEnd(Continue_Pressed);

    bool Continue_UIPressed(iWidget* apWidget, const cGuiMessageData& aData);
    kGuiCallbackDeclarationEnd(Continue_UIPressed);

    bool Gamma_UIArrowPressed(iWidget* apWidget, const cGuiMessageData& aData);
    kGuiCallbackDeclarationEnd(Gamma_UIArrowPressed);
    ///////////////////////
    // Settings

    ///////////////////////
    // Variables
    cGraphics *mpGraphics;

    cGui *mpGui;
    cGuiSet *mpGuiSet;

    cViewport *mpViewport;

    cVector2f mvScreenSize;
    cVector2f mvGuiSetSize;
    cVector2f mvGuiSetCenterSize;
    cVector2f mvGuiSetOffset;
    cVector3f mvGuiSetStartPos;

    eLuxPreMenuState mCurrentState;
    int mlCurrentSectionIdx;
    float mfTimer;
    cInterpolatedFloat mfAlphaFade;

    bool mbExitPreMenu;

    bool mbQuickTransition;

    cGuiGfxElement *mpBlackFade;

    std::vector<cLuxPreMenuSection *> mvSections;

    cLuxPreMenuSection *mpCurrentSection;
    cGuiGfxElement *mpCurrentBackground;
    std::vector<cWidgetLabel *> mvCurrentLabels;

    cWidgetButton *mpBContinue;

    cWidgetDummy *mpGGamma;
    cWidgetImage *mpIGammaPreview;
    cWidgetLabel *mpLGamma;
    cWidgetSlider *mpSGamma;
    float mfGammaMinValue;
    float mfGammaMaxValue;
    float mfGammaStep;
};

//----------------------------------------------


#endif // LUX_MAIN_MENU_H
