#include "LuxMainMenu_Options.h"

#include "LuxMap.h"
#include "LuxMapHandler.h"
#include "LuxEffectHandler.h"
#include "LuxInputHandler.h"
#include "LuxHintHandler.h"
#include "LuxConfigHandler.h"
#include "LuxPostEffects.h"
#include "LuxMessageHandler.h"
#include "LuxPlayerHelpers.h"
#include "LuxPlayer.h"
#include "LuxHelpFuncs.h"

//////////////////////////////////////////////////////////////////////////
// HELPERS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

static int GetIndexFromAnisotropy(float afX)
{
    if(afX <= 1)
    {
        return 0;
    }
    if(afX <= 2)
    {
        return 1;
    }
    if(afX <= 4)
    {
        return 2;
    }
    if(afX <= 8)
    {
        return 3;
    }
    if(afX <= 16)
    {
        return 4;
    }
    return 0;
}

static float GetAnisotropyFromIndex(int alX)
{
    switch(alX)
    {
    case 0:
        return 1.0f;
    case 1:
        return 2.0f;
    case 2:
        return 4.0f;
    case 3:
        return 8.0f;
    case 4:
        return 16.0f;
    }
    return 1.0f;
}

//-----------------------------------------------------------------------

static int GetIndexFromSSAOSamples(int alX)
{
    if(alX <= 4)
    {
        return 0;
    }
    if(alX <= 8)
    {
        return 1;
    }
    if(alX <= 16)
    {
        return 2;
    }
    if(alX <= 32)
    {
        return 3;
    }
    //if(alX <= 64) return 4;
    //if(alX <= 128) return 5;
    return 0;
}

static int GetSSAOSamplesFromIndex(int alX)
{
    switch(alX)
    {
    case 0:
        return 4;
    case 1:
        return 8;
    case 2:
        return 16;
    case 3:
        return 32;
        //case 4: return 64;
        //case 5: return 128;
    }
    return 8;
}

//-----------------------------------------------------------------------

cResourceVarsObject cLuxMainMenu_Options::mInitialValues = cResourceVarsObject();
cResourceVarsObject cLuxMainMenu_Options::mCurrentValues = cResourceVarsObject();

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxMainMenu_Options::cLuxMainMenu_Options(cGuiSet *apGuiSet, cGuiSkin *apGuiSkin) : iLuxMainMenuWindow(apGuiSet, apGuiSkin)
{
    mvWindowSize = cVector2f(620, 460);

    mbTipFadeRestart = false;
    mbTipWidgetUpdated = true;
    mbTipTextReset = false;
    mpCurrentTipWidget = NULL;

    mfFlashMin = 0.0f;
    mfFlashStep = 0.05f;
    mfFlashMax = 1.0f;

    mfGammaMin = 0.3f;
    mfGammaStep = 0.05f;
    mfGammaMax = 2.0f;

    mfMouseSensitivityMin = 0.2f;
    mfMouseSensitivityStep = 0.1f;
    mfMouseSensitivityMax = 5.0f;

#if USE_GAMEPAD
    mfGamepadLookSensitivityMin = 0.5f;
    mfGamepadLookSensitivityStep = 0.05f;
    mfGamepadLookSensitivityMax = 3.0f;
#endif

    mfVolumeMin = 0.0f;
    mfVolumeStep = 0.01f;
    mfVolumeMax = 1.0f;

    mbShowCommentary = gpBase->mpMenuCfg->GetBool("Options", "ShowCommentary", false);

    mbSettingInitialValues = false;

    mbKeyConfigOpen = false;
}

//-----------------------------------------------------------------------

cLuxMainMenu_Options::~cLuxMainMenu_Options()
{
    STLDeleteAll(mvOptionData);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////


//-----------------------------------------------------------------------

void cLuxMainMenu_Options::CreateGui()
{
    mInitialValues = cResourceVarsObject();
    mCurrentValues = cResourceVarsObject();

    CreateMainGui();
}

//-----------------------------------------------------------------------

void cLuxMainMenu_Options::ExitPressed()
{
    if(mpGuiSet->PopUpIsActive())
    {
        return;
    }

    gpBase->mpMainMenu->SetWindowActive(eLuxMainMenuWindow_LastEnum);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxMainMenu_Options::CreateMainGui()
{
    float fLeftBorderSize = 30;
    float fUpperBorderSize = 10;

    cVector3f vPos(fLeftBorderSize, 35 + fUpperBorderSize, 1);

    //////////////////////////
    //Window
    mpWindow = mpGuiSet->CreateWidgetWindow(eWidgetWindowButtonFlag_None, cVector3f(0, 0, 5), mvWindowSize, kTranslate("MainMenu", "Options"));
    mpWindow->AddCallback(eGuiMessage_OnUpdate, this, kGuiCallback(Window_OnUpdate));
    mpWindow->SetStatic(true);

    //////////////////////////
    //Buttons
    float fButtonWidth = 80;
    float fButtonSepp = 3;
    vPos.x = mpWindow->GetSize().x - fButtonWidth * 2 - fButtonSepp - 5;
    vPos.y = mpWindow->GetSize().y - 25 - 10;

    mpBOK = mpGuiSet->CreateWidgetButton(vPos, cVector2f(fButtonWidth, 30), kTranslate("MainMenu", "OK"), mpWindow);
    mpBOK->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(PressOK));

    vPos.x += fButtonWidth + fButtonSepp;
    mpBCancel = mpGuiSet->CreateWidgetButton(vPos, cVector2f(fButtonWidth, 30), kTranslate("MainMenu", "Cancel"), mpWindow);
    mpBCancel->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(PressCancel));
    mpBCancel->AddCallback(eGuiMessage_UIButtonPress, this, kGuiCallback(UIPressCancel));
    mpBCancel->SetGlobalUIInputListener(true);

    mpBOK->SetFocusNavigation(eUIArrow_Right, mpBCancel);
    mpBCancel->SetFocusNavigation(eUIArrow_Left, mpBOK);

    vPos = cVector3f(fLeftBorderSize, 35 + fUpperBorderSize, 1);

    //////////////////////////
    //Tabs
    cVector2f vTabFrameSize(mvWindowSize.x - fLeftBorderSize * 2, mvWindowSize.y - fUpperBorderSize * 3 - 30 - 25 - 20);
    cWidgetTabFrame *pTabFrame = mpGuiSet->CreateWidgetTabFrame(vPos, vTabFrameSize, _W(""), mpWindow, false, true);
    pTabFrame->SetGlobalUIInputListener(true);


    pTabFrame->AddCallback(eGuiMessage_SelectionChange, this, kGuiCallback(TabFrame_OnPageChange));

    mpTabGame = pTabFrame->AddTab(kTranslate("MainMenu", "OptionsGame"));
    mpTabGraphics = pTabFrame->AddTab(kTranslate("MainMenu", "OptionsGraphics"));
    mpTabInput = pTabFrame->AddTab(kTranslate("MainMenu", "OptionsInput"));
    mpTabSound = pTabFrame->AddTab(kTranslate("MainMenu", "OptionsSound"));

#if USE_GAMEPAD
    ///////////////////
    // Add help icons for tabs
    cVector2f vImageSize = fUpperBorderSize * 2;
    vImageSize.x *= 1.5f;
    vPos.y -= 1;
    vPos.x -= fUpperBorderSize * 1.3f;

    mpShoulderHint[0] = mpGuiSet->CreateWidgetImage("gamepad_lb.tga", vPos, vImageSize, eGuiMaterial_Alpha, false, mpWindow, "LB Tip");

    vPos.x = mpWindow->GetSize().x - fUpperBorderSize * 5.0f;

    mpShoulderHint[1] = mpGuiSet->CreateWidgetImage("gamepad_rb.tga", vPos, vImageSize, eGuiMaterial_Alpha, false, mpWindow, "RB Tip");
#endif

    for(int i = 0; i < pTabFrame->GetTabNum(); ++i)
    {
        cWidgetTab* pTab = pTabFrame->GetTab(i);
        int lPrev = i - 1;
        int lNext = i + 1;

        if(lPrev >= 0)
        {
            pTab->GetTabLabel()->SetFocusNavigation(eUIArrow_Left, pTabFrame->GetTab(lPrev)->GetTabLabel());
        }
        if(lNext < pTabFrame->GetTabNum())
        {
            pTab->GetTabLabel()->SetFocusNavigation(eUIArrow_Right, pTabFrame->GetTab(lNext)->GetTabLabel());
        }
    }

    AddGraphicsOptions(mpTabGraphics);
    AddSoundOptions(mpTabSound);
    AddGameOptions(mpTabGame);
    AddInputOptions(mpTabInput);

    //////////////////////////
    //Tip Label
    vPos.x = fLeftBorderSize;
    vPos.y = mpWindow->GetSize().y - 25 - 10;
    mpLTip = mpGuiSet->CreateWidgetLabel(vPos + cVector3f(10, 0, 0), cVector2f(400, 30), _W(""), mpWindow);
    mpLTip->SetDefaultFontColor(cColor(1, 1));
    mpLTip->SetWordWrap(true);
    mpLTip->SetClipActive(true);
    mpLTip->SetScrollSpeedMul(4.0f);
    mpLTip->SetDrawBackGround(true);
    mpLTip->SetBackGroundColor(cColor(0, 0.5f));

    mpLastFocusedWidget = mpCBLanguage;
}

//-----------------------------------------------------------------------

void cLuxMainMenu_Options::AddGameOptions(cWidgetTab* apTab)
{
    float fBorderSize = 15;
    cVector3f vPos(fBorderSize, 6 + fBorderSize + 10, 0.1f);

    ///////////////////////////////////////////////
    // Language Combobox
    cWidgetLabel* pLabel = mpGuiSet->CreateWidgetLabel(vPos, -1, kTranslate("OptionsMenu", "Language"), apTab);
    mpCBLanguage = mpGuiSet->CreateWidgetComboBox(vPos + cVector3f(pLabel->GetSize().x + 5, -2, 0), cVector2f(150, 25), _W(""), apTab);
    //Language is only loaded at startup, so a restart is needed for it to apply everywhere.
    SetUpInput(pLabel, mpCBLanguage, true, kTranslate("OptionsMenu", "LanguageTip"));
    mpCBLanguage->AddCallback(eGuiMessage_SelectionChange, this, kGuiCallback(ChangeLanguage));

    vPos.y += mpCBLanguage->GetSize().y + 15;

    ///////////////////////////////////////////////
    // Subtitles Checkbox
    mpChBShowSubtitles = mpGuiSet->CreateWidgetCheckBox(vPos, 0, kTranslate("OptionsMenu", "ShowSubtitles"), apTab);
    SetUpInput(NULL, mpChBShowSubtitles, false, kTranslate("OptionsMenu", "ShowSubtitlesTip"));

    ///////////////////////////////////////////////
    // Effect subtitles Checkbox
    mpChBShowEffectSubtitles = mpGuiSet->CreateWidgetCheckBox(vPos + cVector3f(mpChBShowSubtitles->GetSize().x + 15, 0, 0), 0, kTranslate("OptionsMenu", "ShowEffectSubtitles"), apTab);
    SetUpInput(NULL, mpChBShowEffectSubtitles, false, kTranslate("OptionsMenu", "ShowEffectSubtitlesTip"));

    vPos.y += mpChBShowSubtitles->GetSize().y + 15;

    ///////////////////////////////////////////////
    // Hints Checkbox
    mpChBShowHints = mpGuiSet->CreateWidgetCheckBox(vPos, 0, kTranslate("OptionsMenu", "ShowHints"), apTab);
    SetUpInput(NULL, mpChBShowHints, false, kTranslate("OptionsMenu", "ShowHintsTip"));

    vPos.y += mpChBShowHints->GetSize().y + 15;

    ///////////////////////////////////////////////
    // Death Hints subtitles Checkbox
    mpChBShowDeathHints = mpGuiSet->CreateWidgetCheckBox(vPos, 0, kTranslate("OptionsMenu", "ShowDeathHints"), apTab);
    SetUpInput(NULL, mpChBShowDeathHints, false, kTranslate("OptionsMenu", "ShowDeathHintsTip"));

    vPos.y += mpChBShowDeathHints->GetSize().y + 15;

    ///////////////////////////////////////////////
    // Crosshair Checkbox
    mpChBShowCrosshair = mpGuiSet->CreateWidgetCheckBox(vPos, 0, kTranslate("OptionsMenu", "ShowCrosshair"), apTab);
    SetUpInput(NULL, mpChBShowCrosshair, false, kTranslate("OptionsMenu", "ShowCrosshairTip"));
    vPos.y += mpChBShowCrosshair->GetSize().y + 15;

    ///////////////////////////////////////////////
    // Focus Icon style Combobox
    pLabel = mpGuiSet->CreateWidgetLabel(vPos, -1, kTranslate("OptionsMenu", "FocusIconStyle"), apTab);
    mpCBFocusIconStyle = mpGuiSet->CreateWidgetComboBox(vPos + cVector3f(pLabel->GetSize().x + 5, -2, 0), cVector2f(150, 25), _W(""), apTab);
    SetUpInput(pLabel, mpCBFocusIconStyle, false, kTranslate("OptionsMenu", "FocusIconStyleTip"));
    mpCBFocusIconStyle->AddItem(kTranslate("OptionsMenu", "FocusIconStyleDefault"));
    mpCBFocusIconStyle->AddItem(kTranslate("OptionsMenu", "FocusIconStyleSimple"));
    vPos.y += pLabel->GetSize().y + 15;

    ///////////////////////////////////////////////
    // Hand orientation Combobox
    pLabel = mpGuiSet->CreateWidgetLabel(vPos, -1, kTranslate("OptionsMenu", "HandOrientation"), apTab);
    mpCBHandOrientation = mpGuiSet->CreateWidgetComboBox(vPos + cVector3f(pLabel->GetSize().x + 5, -2, 0), cVector2f(150, 25), _W(""), apTab);
    SetUpInput(pLabel, mpCBHandOrientation, false, kTranslate("OptionsMenu", "HandOrientationTip"));
    mpCBHandOrientation->AddItem(kTranslate("ButtonNames", "Left"));
    mpCBHandOrientation->AddItem(kTranslate("ButtonNames", "Right"));
    vPos.y += pLabel->GetSize().y + 15;

    ///////////////////////////////////////////////
    // Flashback Intensity
    pLabel = mpGuiSet->CreateWidgetLabel(vPos, -1, kTranslate("OptionsMenu", "FlashIntensity"), apTab);
    mpSFlash = mpGuiSet->CreateWidgetSlider(eWidgetSliderOrientation_Horizontal, cVector3f(0, pLabel->GetSize().y + 5, 0), cVector2f(100, 20), 0, pLabel);
    SetUpInput(pLabel, mpSFlash, false, kTranslate("OptionsMenu", "FlashIntensityTip"));
    SetUpSlider(mpSFlash, mfFlashMin, mfFlashMax, mfFlashStep, kGuiCallback(FlashSlider_OnMove), &mpLFlash);

    vPos.y += mpSFlash->GetLocalPosition().y + mpSFlash->GetSize().y + 15;

    //////////////////////////////////
    // Commentary
    if(mbShowCommentary)
    {
        mpChBShowCommentary = mpGuiSet->CreateWidgetCheckBox(vPos, 0, kTranslate("OptionsMenu", "Commentary"), apTab);
        SetUpInput(NULL, mpChBShowCommentary, false, kTranslate("OptionsMenu", "CommentaryTip"));
        vPos.y += mpChBShowCommentary->GetSize().y + 15;
    }

    // Populate languages
    PopulateLanguageList();

    //////////////////////////////////////////////////////////////////////////////////
    // Set up navigation
    iWidget* pLastWidget = NULL;

    // Down
    mpCBLanguage->SetFocusNavigation(eUIArrow_Down, mpChBShowSubtitles);
    mpChBShowSubtitles->SetFocusNavigation(eUIArrow_Down, mpChBShowHints);
    mpChBShowEffectSubtitles->SetFocusNavigation(eUIArrow_Down, mpChBShowHints);
    mpChBShowHints->SetFocusNavigation(eUIArrow_Down, mpChBShowDeathHints);
    mpChBShowDeathHints->SetFocusNavigation(eUIArrow_Down, mpChBShowCrosshair);
    mpChBShowCrosshair->SetFocusNavigation(eUIArrow_Down, mpCBFocusIconStyle);
    mpCBFocusIconStyle->SetFocusNavigation(eUIArrow_Down, mpCBHandOrientation);
    if(mbShowCommentary)
    {
        mpCBHandOrientation->SetFocusNavigation(eUIArrow_Down, mpChBShowCommentary);
        mpChBShowCommentary->SetFocusNavigation(eUIArrow_Down, mpBOK);

        pLastWidget = mpChBShowCommentary;
    }
    else
    {
        mpCBHandOrientation->SetFocusNavigation(eUIArrow_Down, mpBOK);

        pLastWidget = mpCBHandOrientation;
    }

    apTab->SetUserData(pLastWidget);
    apTab->GetTabLabel()->SetUserData(mpCBLanguage);

    // Up
    mpChBShowSubtitles->SetFocusNavigation(eUIArrow_Up, mpCBLanguage);
    mpChBShowEffectSubtitles->SetFocusNavigation(eUIArrow_Up, mpCBLanguage);
    mpChBShowHints->SetFocusNavigation(eUIArrow_Up, mpChBShowSubtitles);
    mpChBShowDeathHints->SetFocusNavigation(eUIArrow_Up, mpChBShowHints);
    mpChBShowCrosshair->SetFocusNavigation(eUIArrow_Up, mpChBShowDeathHints);
    mpCBFocusIconStyle->SetFocusNavigation(eUIArrow_Up, mpChBShowCrosshair);
    mpCBHandOrientation->SetFocusNavigation(eUIArrow_Up, mpCBFocusIconStyle);
    if(mbShowCommentary)
    {
        mpChBShowCommentary->SetFocusNavigation(eUIArrow_Up, mpCBHandOrientation);
    }

    // Left/Right
    mpChBShowSubtitles->SetFocusNavigation(eUIArrow_Right, mpChBShowEffectSubtitles);
    mpChBShowEffectSubtitles->SetFocusNavigation(eUIArrow_Left, mpChBShowSubtitles);
}

//-----------------------------------------------------------------------

void cLuxMainMenu_Options::AddGraphicsOptions(cWidgetTab* apTab)
{
    float fBorderSize = 15;
    cVector3f vPos(fBorderSize - 5, 6 + fBorderSize, 0.1f);

    /////////////////////////////////
    // Basic options
    mpDBasicGfxOptions = mpGuiSet->CreateWidgetDummy(vPos, apTab);
    mpDBasicGfxOptions->SetVisible(true);
    mpDBasicGfxOptions->SetEnabled(true);
    AddBasicGfxOptions(mpDBasicGfxOptions);

    /////////////////////////////////
    // Avanced options
    mpDAdvancedGfxOptions = mpGuiSet->CreateWidgetDummy(vPos, apTab);
    mpDAdvancedGfxOptions->SetVisible(false);
    mpDAdvancedGfxOptions->SetEnabled(false);
    AddAdvancedGfxOptions(mpDAdvancedGfxOptions);

    // Basic/Advanced toggle button
    tWStringVec vToggleButtonStrings;
    vToggleButtonStrings.push_back(kTranslate("OptionsMenu", "AdvancedOptions"));
    vToggleButtonStrings.push_back(kTranslate("OptionsMenu", "BasicOptions"));

    float fButtonWidth = 0;
    mpBToggleShowGfxOptions = mpGuiSet->CreateWidgetButton(0,
                              cVector2f(0, 25),
                              vToggleButtonStrings[0],
                              apTab);
    mpBToggleShowGfxOptions->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(PressToggleShowGfxOptions));
    for(int i = 0; i < (int)vToggleButtonStrings.size(); ++i)
    {
        float fStringLength = mpBToggleShowGfxOptions->GetDefaultFontType()->GetLength(mpBToggleShowGfxOptions->GetDefaultFontSize(),
                              mpBToggleShowGfxOptions->GetText().c_str()) + 10;

        if(fButtonWidth < fStringLength)
        {
            fButtonWidth = fStringLength;
        }
    }


    mpBToggleShowGfxOptions->SetSize(cVector2f(fButtonWidth, mpBToggleShowGfxOptions->GetSize().y));
    mpBToggleShowGfxOptions->SetPosition(cVector3f(apTab->GetSize()) - cVector3f(fButtonWidth, 50, -2));

    SetUpInput(NULL, mpBToggleShowGfxOptions, true, _W(""));

    mpBToggleShowGfxOptions->SetFocusNavigation(eUIArrow_Down, mpBOK);
    mpBToggleShowGfxOptions->SetFocusNavigation(eUIArrow_Up, mpSGamma);

    mpSGamma->SetFocusNavigation(eUIArrow_Down, mpBToggleShowGfxOptions);
    mpChBRefraction->SetFocusNavigation(eUIArrow_Down, mpBToggleShowGfxOptions);

    apTab->SetUserData(mpBToggleShowGfxOptions);
    apTab->GetTabLabel()->SetUserData(mpCBResolution);
}

//-----------------------------------------------------------------------

void cLuxMainMenu_Options::AddBasicGfxOptions(cWidgetDummy* apDummy)
{
    float fBorderSize = 0;
    cVector3f vPos(fBorderSize, 6 + fBorderSize, 0.1f);

    cWidgetLabel* pLabel = NULL;

    /////////////////////////////////
    // Screen group
    cWidgetGroup *pGroup = mpGuiSet->CreateWidgetGroup(vPos, 0, kTranslate("OptionsMenu", "Screen"), apDummy);
    {
        float fBorderSize = 15;
        pGroup->SetSize(cVector2f(apDummy->GetParent()->GetSize().x - fBorderSize - fBorderSize, 70));
        cVector3f vPosInGroup = cVector3f(fBorderSize, fBorderSize, 0.1f);

        /////////////////////////////////
        // Resolution
        pLabel = mpGuiSet->CreateWidgetLabel(vPosInGroup, -1, kTranslate("OptionsMenu", "Resolution"), pGroup);
        mpCBResolution = mpGuiSet->CreateWidgetComboBox(pLabel->GetLocalPosition() + cVector3f(0, pLabel->GetSize().y + 5, 0), cVector2f(175, 25), _W(""), pGroup);
        SetUpInput(pLabel, mpCBResolution, true, kTranslate("OptionsMenu", "ResolutionTip"));

        vPosInGroup.x += mpCBResolution->GetSize().x + 100;

        /////////////////////////////////
        // Full screen and Vsync
        mpChBFullScreen = mpGuiSet->CreateWidgetCheckBox(vPosInGroup + cVector3f(0, 2, 0), -1, kTranslate("OptionsMenu", "FullScreen"), pGroup);
        SetUpInput(NULL, mpChBFullScreen, true, kTranslate("OptionsMenu", "FullScreenTip"));

        mpChBVSync = mpGuiSet->CreateWidgetCheckBox(vPosInGroup + cVector3f(0, mpChBFullScreen->GetSize().y + 10, 0), 0, kTranslate("OptionsMenu", "VSync"), pGroup);
        SetUpInput(NULL, mpChBVSync, false, kTranslate("OptionsMenu", "VSyncTip"));
    }

    vPos.y += pGroup->GetSize().y + 15;

    /////////////////////////////////
    // Texture Quality
    pLabel = mpGuiSet->CreateWidgetLabel(vPos, -1, kTranslate("OptionsMenu", "TexQuality"), apDummy);
    mpCBTextureSizeLevel = mpGuiSet->CreateWidgetComboBox(pLabel->GetLocalPosition() + cVector3f(0, pLabel->GetSize().y + 5, 0), cVector2f(100, 25), _W(""), apDummy);
    SetUpInput(pLabel, mpCBTextureSizeLevel, true, kTranslate("OptionsMenu", "TexQualityTip"));

    cMaterialManager* pMatMgr = gpBase->mpEngine->GetResources()->GetMaterialManager();
    tWStringVec vTexQualityStrings;
    vTexQualityStrings.push_back(kTranslate("OptionsMenu", "UltraLow"));
    vTexQualityStrings.push_back(kTranslate("OptionsMenu", "Low"));
    vTexQualityStrings.push_back(kTranslate("OptionsMenu", "Medium"));
    vTexQualityStrings.push_back(kTranslate("OptionsMenu", "High"));

    mpCBTextureSizeLevel->ClearItems();
    for(int i = 0; i < (int)vTexQualityStrings.size(); ++i)
    {
        mpCBTextureSizeLevel->AddItem(vTexQualityStrings[i]);
    }

    /////////////////////////////////
    // Gamma
    vPos.x += 140;

    pLabel = mpGuiSet->CreateWidgetLabel(vPos, -1, kTranslate("OptionsMenu", "Gamma"), apDummy);
    {
        cVector3f vLabelPos(0, pLabel->GetSize().y + 5, 0);

        cWidgetImage *pImg = mpGuiSet->CreateWidgetImage("menu_gamma.tga", vLabelPos, -1, eGuiMaterial_Alpha, false, pLabel);
        SetUpInput(NULL, pImg, false, kTranslate("OptionsMenu", "GammaInstructions"));
        vLabelPos.y += pImg->GetSize().y + 5.0f;

        mpSGamma = mpGuiSet->CreateWidgetSlider(eWidgetSliderOrientation_Horizontal, vLabelPos, cVector2f(pImg->GetSize().x, 20), 0, pLabel);
        SetUpInput(pLabel, mpSGamma, false, kTranslate("OptionsMenu", "GammaInstructions"));
        SetUpSlider(mpSGamma, mfGammaMin, mfGammaMax, mfGammaStep, kGuiCallback(GammaSlider_OnMove), &mpLGamma);
        mpSGamma->SetBarValueSize(4);

        vLabelPos.y += mpSGamma->GetSize().y + 4.0f;
    }

    mpCBResolution->SetFocusNavigation(eUIArrow_Down, mpCBTextureSizeLevel);
    mpCBResolution->SetFocusNavigation(eUIArrow_Right, mpChBFullScreen);

    mpChBFullScreen->SetFocusNavigation(eUIArrow_Left, mpCBResolution);
    mpChBFullScreen->SetFocusNavigation(eUIArrow_Down, mpChBVSync);

    mpChBVSync->SetFocusNavigation(eUIArrow_Left, mpCBResolution);
    mpChBVSync->SetFocusNavigation(eUIArrow_Up, mpChBFullScreen);
    mpChBVSync->SetFocusNavigation(eUIArrow_Down, mpCBTextureSizeLevel);

    mpCBTextureSizeLevel->SetFocusNavigation(eUIArrow_Up, mpCBResolution);
    mpCBTextureSizeLevel->SetFocusNavigation(eUIArrow_Down, mpSGamma);

    mpSGamma->SetFocusNavigation(eUIArrow_Up, mpCBTextureSizeLevel);

}

//-----------------------------------------------------------------------

void cLuxMainMenu_Options::AddAdvancedGfxOptions(cWidgetDummy* apDummy)
{
    float fBorderSize = 0;
    cVector3f vPos(fBorderSize, 6 + fBorderSize, 0.1f);
    float fItemSep = 180;

    cWidgetFrame* pMainFrame = mpGuiSet->CreateWidgetFrame(cVector3f(0, 0, 1), cVector2f(550, 275), false, apDummy, false, true);
    pMainFrame->SetDrawBackground(false);

    cWidgetLabel* pLabel = NULL;

    cVector2f vGroupSize = cVector2f(520, 70);

    ////////////////////////////
    // Texture
    cWidgetGroup *pGroup = mpGuiSet->CreateWidgetGroup(vPos, vGroupSize, kTranslate("OptionsMenu", "Material"), pMainFrame);
    {
        float fBorderSize = 15;
        cVector3f vPosInGroup = cVector3f(fBorderSize, fBorderSize, 0.1f);

        ////////////////////////////
        // Texture filter
        pLabel = mpGuiSet->CreateWidgetLabel(vPosInGroup, -1, kTranslate("OptionsMenu", "TexFilter"), pGroup);
        mpCBTextureFilter = mpGuiSet->CreateWidgetComboBox(cVector3f(0, pLabel->GetSize().y + 5, 0), cVector2f(150, 25), _W(""), pLabel);
        SetUpInput(pLabel, mpCBTextureFilter, false, kTranslate("OptionsMenu", "TexFilterTip"));

        //vPosInGroup.x += mpCBTextureFilter->GetSize().x + 20;

        vPosInGroup.x += fItemSep;

        /////////////////////////////
        // Anisotropy
        pLabel = mpGuiSet->CreateWidgetLabel(vPosInGroup, -1, kTranslate("OptionsMenu", "Anisotropy"), pGroup);
        mpCBAnisotropy = mpGuiSet->CreateWidgetComboBox(cVector3f(0, pLabel->GetSize().y + 5, 0), cVector2f(100, 25), _W(""), pLabel);
        SetUpInput(pLabel, mpCBAnisotropy, false, kTranslate("OptionsMenu", "AnisotropyTip"));

        //vPosInGroup.x += mpCBAnisotropy->GetSize().x + 20;

        vPosInGroup.x += fItemSep;

        /////////////////////////////
        // Parallax Quality
        mpChBParallax = mpGuiSet->CreateWidgetCheckBox(vPosInGroup, 0, kTranslate("OptionsMenu", "Parallax"), pGroup);
        SetUpInput(NULL, mpChBParallax, true, kTranslate("OptionsMenu", "ParallaxEnabledTip"));

        mpCBParallaxQuality = mpGuiSet->CreateWidgetComboBox(cVector3f(mpCBAnisotropy->GetSize().x + 80, mpChBParallax->GetSize().y + 5, 0), cVector2f(100, 25), _W(""), pLabel);
        SetUpInput(NULL, mpCBParallaxQuality, true, kTranslate("OptionsMenu", "ParallaxQualityTipNew"));

        mpCBParallaxQuality->ClearItems();
        mpCBParallaxQuality->AddItem(kTranslate("OptionsMenu", "ParallaxSimple"));
        mpCBParallaxQuality->AddItem(kTranslate("OptionsMenu", "ParallaxRelief"));
    }

    vPos.y += pGroup->GetSize().y + 10;

    /////////////////////////////
    // Shadows Group
    pGroup = mpGuiSet->CreateWidgetGroup(vPos, vGroupSize, kTranslate("OptionsMenu", "Shadows"), pMainFrame);
    {
        float fBorderSize = 15;
        cVector3f vPosInGroup = cVector3f(fBorderSize, fBorderSize, 0.1f);

        /////////////////////////////
        // Shadows Active
        mpChBShadows = mpGuiSet->CreateWidgetCheckBox(vPosInGroup, 0, kTranslate("OptionsMenu", "Shadows"), pGroup);
        SetUpInput(NULL, mpChBShadows, false, kTranslate("OptionsMenu", "ShadowsTip"));

        //vPosInGroup.x += mpChBShadows->GetSize().x + 15;
        vPosInGroup.x += fItemSep;

        /////////////////////////////
        // Shadow Quality
        pLabel = mpGuiSet->CreateWidgetLabel(vPosInGroup, -1, kTranslate("OptionsMenu", "ShadowQuality"), pGroup);
        mpCBShadowQuality = mpGuiSet->CreateWidgetComboBox(cVector3f(0, pLabel->GetSize().y + 5, 0), cVector2f(100, 25), _W(""), pLabel);
        SetUpInput(pLabel, mpCBShadowQuality, true, kTranslate("OptionsMenu", "ShadowQualityTip"));

        //vPosInGroup.x += mpCBShadowQuality->GetSize().x + 15;
        vPosInGroup.x += fItemSep;

        /////////////////////////////
        // Shadow Resolution
        pLabel = mpGuiSet->CreateWidgetLabel(vPosInGroup, -1, kTranslate("OptionsMenu", "ShadowRes"), pGroup);
        mpCBShadowRes = mpGuiSet->CreateWidgetComboBox(cVector3f(0, pLabel->GetSize().y + 5, 0), cVector2f(100, 25), _W(""), pLabel);
        SetUpInput(pLabel, mpCBShadowRes, true, kTranslate("OptionsMenu", "ShadowResTip"));

        // Set up values
        tWStringVec vOptionStrings;
        vOptionStrings.push_back(kTranslate("OptionsMenu", "Low"));
        vOptionStrings.push_back(kTranslate("OptionsMenu", "Medium"));
        vOptionStrings.push_back(kTranslate("OptionsMenu", "High"));
        vOptionStrings.push_back(kTranslate("OptionsMenu", "Ultra"));

        mpCBShadowQuality->ClearItems();
        mpCBShadowRes->ClearItems();
        for(int i = 0; i < (int)vOptionStrings.size(); ++i)
        {
            mpCBShadowQuality->AddItem(vOptionStrings[i]);
            mpCBShadowRes->AddItem(vOptionStrings[i]);
        }
    }

    vPos.y += pGroup->GetSize().y + 10;

    ////////////////////////////
    // Post Effects
    pGroup = mpGuiSet->CreateWidgetGroup(vPos, vGroupSize, kTranslate("OptionsMenu", "PostEffects"), pMainFrame);
    {
        float fBorderSize = 15;
        float fInputSep = 10;
        cVector3f vPosInGroup = cVector3f(fBorderSize, fBorderSize, 0.1f);
        float fMaxWidth = 0;

        // Bloom
        mpChBBloom = mpGuiSet->CreateWidgetCheckBox(vPosInGroup, 0, kTranslate("OptionsMenu", "Bloom"), pGroup);
        SetUpInput(NULL, mpChBBloom, false, kTranslate("OptionsMenu", "BloomTip"));

        if(fMaxWidth < mpChBBloom->GetSize().x)
        {
            fMaxWidth = mpChBBloom->GetSize().x;
        }

        vPosInGroup.y += mpChBBloom->GetSize().y + fInputSep;

        // ImageTrail
        mpChBImageTrail = mpGuiSet->CreateWidgetCheckBox(vPosInGroup, 0, kTranslate("OptionsMenu", "ImageTrail"), pGroup);
        SetUpInput(NULL, mpChBImageTrail, false, kTranslate("OptionsMenu", "ImageTrailTip"));

        if(fMaxWidth < mpChBImageTrail->GetSize().x)
        {
            fMaxWidth = mpChBImageTrail->GetSize().x;
        }

        vPosInGroup.y = fBorderSize;
        //vPosInGroup.x += fMaxWidth + fInputSep;
        //fMaxWidth = 0;

        vPosInGroup.x += fItemSep;

        // Sepia
        mpChBSepia = mpGuiSet->CreateWidgetCheckBox(vPosInGroup, 0, kTranslate("OptionsMenu", "Sepia"), pGroup);
        SetUpInput(NULL, mpChBSepia, false, kTranslate("OptionsMenu", "SepiaTip"));

        if(fMaxWidth < mpChBBloom->GetSize().x)
        {
            fMaxWidth = mpChBBloom->GetSize().x;
        }

        vPosInGroup.y += mpChBSepia->GetSize().y + fInputSep;

        // RadialBlur
        mpChBRadialBlur = mpGuiSet->CreateWidgetCheckBox(vPosInGroup, 0, kTranslate("OptionsMenu", "RadialBlur"), pGroup);
        SetUpInput(NULL, mpChBRadialBlur, false, kTranslate("OptionsMenu", "RadialBlurTip"));

        if(fMaxWidth < mpChBRadialBlur->GetSize().x)
        {
            fMaxWidth = mpChBRadialBlur->GetSize().x;
        }

        vPosInGroup.y = fBorderSize;
        //vPosInGroup.x += fMaxWidth + fInputSep;
        vPosInGroup.x += fItemSep;

        // Insanity
        mpChBInsanity = mpGuiSet->CreateWidgetCheckBox(vPosInGroup, 0, kTranslate("OptionsMenu", "Insanity"), pGroup);
        SetUpInput(NULL, mpChBInsanity, false, kTranslate("OptionsMenu", "InsanityTip"));
    }

    vPos.y += pGroup->GetSize().y + 10;
    ////////////////////////////
    // SSAO Group
    pGroup = mpGuiSet->CreateWidgetGroup(vPos, vGroupSize, kTranslate("OptionsMenu", "SSAO"), pMainFrame);
    {
        float fBorderSize = 15;
        cVector3f vPosInGroup = cVector3f(fBorderSize, fBorderSize, 0.1f);

        ///////////////////////////
        // SSAO Active
        mpChBSSAO = mpGuiSet->CreateWidgetCheckBox(vPosInGroup, 0, kTranslate("OptionsMenu", "SSAO"), pGroup);
        SetUpInput(NULL, mpChBSSAO, true, kTranslate("OptionsMenu", "SSAOTip"));

        //vPosInGroup.x += mpChBSSAO->GetSize().x + 20;
        vPosInGroup.x += fItemSep;

        ///////////////////////////
        // SSAO Samples
        pLabel = mpGuiSet->CreateWidgetLabel(vPosInGroup, -1, kTranslate("OptionsMenu", "SSAOSamples"), pGroup);
        mpCBSSAOSamples = mpGuiSet->CreateWidgetComboBox(cVector3f(0, pLabel->GetSize().y + 5, 0), cVector2f(60, 25), _W(""), pLabel);
        SetUpInput(pLabel, mpCBSSAOSamples, true, kTranslate("OptionsMenu", "SSAOSamplesTip"));

        for(int i = 0; i < 4; ++i)
        {
            int lSamples = GetSSAOSamplesFromIndex(i);
            mpCBSSAOSamples->AddItem(cString::ToStringW(lSamples));
        }

        //vPosInGroup.x += mpCBSSAOSamples->GetSize().x + 20;
        vPosInGroup.x += fItemSep;

        ///////////////////////////
        // SSAO Resolution
        pLabel = mpGuiSet->CreateWidgetLabel(vPosInGroup, -1, kTranslate("OptionsMenu", "SSAOResolution"), pGroup);
        mpCBSSAOResolution = mpGuiSet->CreateWidgetComboBox(cVector3f(0, pLabel->GetSize().y + 5, 0), cVector2f(80, 25), _W(""), pLabel);
        SetUpInput(pLabel, mpCBSSAOResolution, true, kTranslate("OptionsMenu", "SSAOResolutionTip"));

        mpCBSSAOResolution->AddItem(kTranslate("OptionsMenu", "Medium"));
        mpCBSSAOResolution->AddItem(kTranslate("OptionsMenu", "High"));
    }

    vPos.y += pGroup->GetSize().y + 10;

    ////////////////////////////
    // Water Group
    //pGroup = mpGuiSet->CreateWidgetGroup(vPos, vGroupSize, _W("Water"), pMainFrame);
    //{
    //    float fBorderSize = 15;
    //    cVector3f vPosInGroup = cVector3f(fBorderSize, fBorderSize, 0.1f);
    //}

    //vPos.x = fBorderSize;
    //vPos.y += pGroup->GetSize().y + 10;

    ////////////////////////////
    // Misc
    pGroup = mpGuiSet->CreateWidgetGroup(vPos, vGroupSize, kTranslate("KeyConfig", "Misc"), pMainFrame);
    {
        float fBorderSize = 15;
        cVector3f vPosInGroup = cVector3f(fBorderSize, fBorderSize, 0.1f);

        mpChBRefraction = mpGuiSet->CreateWidgetCheckBox(vPosInGroup, 0, kTranslate("OptionsMenu", "Refraction"), pGroup);
        SetUpInput(NULL, mpChBRefraction, true, kTranslate("OptionsMenu", "RefractionTip"));

        vPosInGroup.x += fItemSep;
        //vPosInGroup.y = fBorderSize;

        ///////////////////////////
        // World Reflection Active
        mpChBWorldReflection = mpGuiSet->CreateWidgetCheckBox(vPosInGroup, 0, kTranslate("OptionsMenu", "WorldReflection") + _W(" (") + kTranslate("OptionsMenu", "Water") + _W(")"), pGroup);
        SetUpInput(NULL, mpChBWorldReflection, false, kTranslate("OptionsMenu", "WorldReflectionTip"));
    }


    //////////////
    // Setup gamepad navigation
    mpCBTextureFilter;
    mpCBAnisotropy;
    mpCBParallaxQuality;

    mpChBShadows;
    mpCBShadowQuality;
    mpCBShadowRes;

    mpChBBloom;
    mpChBSepia;
    mpChBInsanity;
    mpChBImageTrail;
    mpChBRadialBlur;

    mpChBSSAO;
    mpCBSSAOSamples;
    mpCBSSAOResolution;

    mpChBWorldReflection;
    mpChBRefraction;

    {
        mpCBTextureFilter->SetFocusNavigation(eUIArrow_Right, mpCBAnisotropy);
        mpCBAnisotropy->SetFocusNavigation(eUIArrow_Right, mpCBParallaxQuality);

        mpCBAnisotropy->SetFocusNavigation(eUIArrow_Left, mpCBTextureFilter);
        mpCBParallaxQuality->SetFocusNavigation(eUIArrow_Left, mpCBAnisotropy);

        mpCBTextureFilter->SetFocusNavigation(eUIArrow_Down, mpChBShadows);
        mpCBAnisotropy->SetFocusNavigation(eUIArrow_Down, mpCBShadowQuality);
        mpCBParallaxQuality->SetFocusNavigation(eUIArrow_Down, mpCBShadowRes);
    }

    {
        mpChBShadows->SetFocusNavigation(eUIArrow_Up, mpCBTextureFilter);
        mpCBShadowQuality->SetFocusNavigation(eUIArrow_Up, mpCBAnisotropy);
        mpCBShadowRes->SetFocusNavigation(eUIArrow_Up, mpCBParallaxQuality);

        mpChBShadows->SetFocusNavigation(eUIArrow_Right, mpCBShadowQuality);
        mpCBShadowQuality->SetFocusNavigation(eUIArrow_Right, mpCBShadowRes);

        mpCBShadowQuality->SetFocusNavigation(eUIArrow_Left, mpChBShadows);
        mpCBShadowRes->SetFocusNavigation(eUIArrow_Left, mpCBShadowQuality);

        mpChBShadows->SetFocusNavigation(eUIArrow_Down, mpChBBloom);
        mpCBShadowQuality->SetFocusNavigation(eUIArrow_Down, mpChBSepia);
        mpCBShadowRes->SetFocusNavigation(eUIArrow_Down, mpChBInsanity);
    }

    {
        mpChBBloom->SetFocusNavigation(eUIArrow_Up, mpChBShadows);
        mpChBSepia->SetFocusNavigation(eUIArrow_Up, mpCBShadowQuality);
        mpChBInsanity->SetFocusNavigation(eUIArrow_Up, mpCBShadowRes);

        mpChBBloom->SetFocusNavigation(eUIArrow_Right, mpChBSepia);
        mpChBSepia->SetFocusNavigation(eUIArrow_Right, mpChBInsanity);

        mpChBSepia->SetFocusNavigation(eUIArrow_Left, mpChBBloom);
        mpChBInsanity->SetFocusNavigation(eUIArrow_Left, mpChBSepia);

        mpChBBloom->SetFocusNavigation(eUIArrow_Down, mpChBImageTrail);
        mpChBSepia->SetFocusNavigation(eUIArrow_Down, mpChBRadialBlur);
        mpChBInsanity->SetFocusNavigation(eUIArrow_Down, mpChBRadialBlur);

        mpChBImageTrail->SetFocusNavigation(eUIArrow_Up, mpChBBloom);
        mpChBRadialBlur->SetFocusNavigation(eUIArrow_Up, mpChBSepia);

        mpChBImageTrail->SetFocusNavigation(eUIArrow_Right, mpChBRadialBlur);
        mpChBRadialBlur->SetFocusNavigation(eUIArrow_Left, mpChBImageTrail);

        mpChBImageTrail->SetFocusNavigation(eUIArrow_Down, mpChBSSAO);
        mpChBRadialBlur->SetFocusNavigation(eUIArrow_Down, mpCBSSAOSamples);
    }

    {
        mpChBSSAO->SetFocusNavigation(eUIArrow_Up, mpChBImageTrail);
        mpCBSSAOSamples->SetFocusNavigation(eUIArrow_Up, mpChBRadialBlur);
        mpCBSSAOResolution->SetFocusNavigation(eUIArrow_Up, mpChBRadialBlur);

        mpChBSSAO->SetFocusNavigation(eUIArrow_Right, mpCBSSAOSamples);
        mpCBSSAOSamples->SetFocusNavigation(eUIArrow_Right, mpCBSSAOResolution);

        mpCBSSAOSamples->SetFocusNavigation(eUIArrow_Left, mpChBSSAO);
        mpCBSSAOResolution->SetFocusNavigation(eUIArrow_Left, mpCBSSAOSamples);

        mpChBSSAO->SetFocusNavigation(eUIArrow_Down, mpChBWorldReflection);
        mpCBSSAOSamples->SetFocusNavigation(eUIArrow_Down, mpChBWorldReflection);
        mpCBSSAOResolution->SetFocusNavigation(eUIArrow_Down, mpChBWorldReflection);
    }

    {
        mpChBWorldReflection->SetFocusNavigation(eUIArrow_Up, mpCBSSAOSamples);
        mpChBWorldReflection->SetFocusNavigation(eUIArrow_Left, mpCBSSAOSamples);
        mpChBWorldReflection->SetFocusNavigation(eUIArrow_Down, mpChBRefraction);

        mpChBRefraction->SetFocusNavigation(eUIArrow_Up, mpChBRefraction);
    }
}

//-----------------------------------------------------------------------

void cLuxMainMenu_Options::AddInputOptions(cWidgetTab* apTab)
{
    float fBorderSize = 15;
    cVector3f vPos(fBorderSize, 6 + fBorderSize + 10, 0.1f);

    /////////////////////////////
    // Invert Mouse
    mpChBInvertMouse = mpGuiSet->CreateWidgetCheckBox(vPos, 0, kTranslate("OptionsMenu", "InvertMouse"), apTab);
    SetUpInput(NULL, mpChBInvertMouse, false, kTranslate("OptionsMenu", "InvertMouseTip"));

    vPos.y += mpChBInvertMouse->GetSize().y + 15;

    /////////////////////////////
    // Smooth Mouse
    mpChBSmoothMouse = mpGuiSet->CreateWidgetCheckBox(vPos, 0, kTranslate("OptionsMenu", "SmoothMouse"), apTab);
    SetUpInput(NULL, mpChBSmoothMouse, false, kTranslate("OptionsMenu", "SmoothMouseTip"));

    vPos.y += mpChBSmoothMouse->GetSize().y + 15;

    /////////////////////////////
    // Mouse Sensitivity
    cWidgetLabel* pLabel = mpGuiSet->CreateWidgetLabel(vPos, -1, kTranslate("OptionsMenu", "MouseSensitivity"), apTab);
    mpSMouseSensitivity = mpGuiSet->CreateWidgetSlider(eWidgetSliderOrientation_Horizontal, cVector3f(0, pLabel->GetSize().y + 5, 0), cVector2f(100, 20), 0, pLabel);
    SetUpInput(pLabel, mpSMouseSensitivity, false, kTranslate("OptionsMenu", "MouseSensitivityTip"));
    SetUpSlider(mpSMouseSensitivity, mfMouseSensitivityMin, mfMouseSensitivityMax, mfMouseSensitivityStep, kGuiCallback(MouseSensitivitySlider_OnMove), &mpLMouseSensitivity);

    vPos.y += mpSMouseSensitivity->GetLocalPosition().y + mpSMouseSensitivity->GetSize().y + 15;

#if USE_GAMEPAD
    //////////////////////////////////////////////////
    // Gamepad stuff
    mpChBGamepadInvertLook = NULL;
    mpLGamepadLookSensitivity = NULL;
    mpSGamepadLookSensitivity = NULL;

    /////////////////////////////
    // Invert Gamepad look
    mpChBGamepadInvertLook = mpGuiSet->CreateWidgetCheckBox(vPos, 0, kTranslate("OptionsMenu", "InvertGamepadLook"), apTab);
    SetUpInput(NULL, mpChBGamepadInvertLook, false, kTranslate("OptionsMenu", "InvertGamepadLookTip"));

    vPos.y += mpChBGamepadInvertLook->GetSize().y + 15;

    /////////////////////////////
    // Gamepad Sensitivity
    pLabel = mpGuiSet->CreateWidgetLabel(vPos, -1, kTranslate("OptionsMenu", "GamepadLookSensitivity"), apTab);
    mpSGamepadLookSensitivity = mpGuiSet->CreateWidgetSlider(eWidgetSliderOrientation_Horizontal, cVector3f(0, pLabel->GetSize().y + 5, 0), cVector2f(100, 20), 0, pLabel);
    SetUpInput(pLabel, mpSMouseSensitivity, false, kTranslate("OptionsMenu", "GamepadLookSensitivityTip"));
    SetUpSlider(mpSGamepadLookSensitivity, mfGamepadLookSensitivityMin, mfGamepadLookSensitivityMax, mfGamepadLookSensitivityStep, kGuiCallback(GamepadLookSensitivitySlider_OnMove), &mpLGamepadLookSensitivity);

    vPos.y += mpSGamepadLookSensitivity->GetLocalPosition().y + mpSGamepadLookSensitivity->GetSize().y + 15;
#endif

    //////////////////////////////
    // Key Config Button
    mpBKeyConfig = mpGuiSet->CreateWidgetButton(vPos, cVector2f(0, 25), kTranslate("OptionsMenu", "KeyConfigButton"), apTab);
    float fButtonWidth = mpBKeyConfig->GetDefaultFontType()->GetLength(mpBKeyConfig->GetDefaultFontSize(), mpBKeyConfig->GetText().c_str());
    mpBKeyConfig->SetSize(cVector2f(fButtonWidth + 20, mpBKeyConfig->GetSize().y));
    SetUpInput(NULL, mpBKeyConfig, false, kTranslate("OptionsMenu", "KeyConfigButtonTip"));
    mpBKeyConfig->AddCallback(eGuiMessage_ButtonPressed, this, kGuiCallback(PressKeyConfig));



    //////////////////////////////////////////////////////////////////////////////////
    // Set up navigation

    apTab->SetUserData(mpBKeyConfig);
    apTab->GetTabLabel()->SetUserData(mpChBInvertMouse);

    // Down
    mpChBInvertMouse->SetFocusNavigation(eUIArrow_Down, mpChBSmoothMouse);
    mpChBSmoothMouse->SetFocusNavigation(eUIArrow_Down, mpSMouseSensitivity);
    mpSMouseSensitivity->SetFocusNavigation(eUIArrow_Down, mpBKeyConfig);
#if USE_GAMEPAD
    mpSMouseSensitivity->SetFocusNavigation(eUIArrow_Down, mpChBGamepadInvertLook);
    mpChBGamepadInvertLook->SetFocusNavigation(eUIArrow_Down, mpSGamepadLookSensitivity);
    mpSGamepadLookSensitivity->SetFocusNavigation(eUIArrow_Down, mpBKeyConfig);
#endif
    mpBKeyConfig->SetFocusNavigation(eUIArrow_Down, mpBOK);

    // Up
    mpChBSmoothMouse->SetFocusNavigation(eUIArrow_Up, mpChBInvertMouse);
    mpSMouseSensitivity->SetFocusNavigation(eUIArrow_Up, mpChBSmoothMouse);
    mpBKeyConfig->SetFocusNavigation(eUIArrow_Up, mpSMouseSensitivity);
#if USE_GAMEPAD
    mpChBGamepadInvertLook->SetFocusNavigation(eUIArrow_Up, mpSMouseSensitivity);
    mpSGamepadLookSensitivity->SetFocusNavigation(eUIArrow_Up, mpChBGamepadInvertLook);
    mpBKeyConfig->SetFocusNavigation(eUIArrow_Up, mpSGamepadLookSensitivity);
#endif

}

//-----------------------------------------------------------------------

void cLuxMainMenu_Options::AddSoundOptions(cWidgetTab* apTab)
{
    float fBorderSize = 15;
    cVector3f vPos(fBorderSize, 6 + fBorderSize + 10, 0.1f);

    /////////////////////////////
    // Sound Device selector
    cWidgetLabel* pLabel = mpGuiSet->CreateWidgetLabel(vPos, -1, kTranslate("OptionsMenu", "SoundDevice"), apTab);
    vPos.y += pLabel->GetSize().y + 5;

    mpCBSndDevice = mpGuiSet->CreateWidgetComboBox(vPos, cVector2f(400, 25), _W(""), apTab);
    SetUpInput(pLabel, mpCBSndDevice, true, kTranslate("OptionsMenu", "SoundDeviceTip"));

    vPos.y += mpCBSndDevice->GetSize().y + 15;

    /////////////////////////////
    // Sound Master Volume
    pLabel = mpGuiSet->CreateWidgetLabel(vPos, -1, kTranslate("OptionsMenu", "Volume"), apTab);
    mpSVolume = mpGuiSet->CreateWidgetSlider(eWidgetSliderOrientation_Horizontal, vPos + cVector3f(0, pLabel->GetSize().y + 5, 0), cVector2f(100, 20), 0, apTab);
    SetUpInput(pLabel, mpSVolume, false, kTranslate("OptionsMenu", "VolumeTip"));
    SetUpSlider(mpSVolume, mfVolumeMin, mfVolumeMax, mfVolumeStep, kGuiCallback(SoundSlider_OnMove), &mpLVolume);

    mpCBSndDevice->SetFocusNavigation(eUIArrow_Down, mpSVolume);

    mpSVolume->SetFocusNavigation(eUIArrow_Up, mpCBSndDevice);
    mpSVolume->SetFocusNavigation(eUIArrow_Down, mpBOK);

    apTab->SetUserData(mpSVolume);
    apTab->GetTabLabel()->SetUserData(mpCBSndDevice);

    PopulateSoundDevices();
}

//-----------------------------------------------------------------------

void cLuxMainMenu_Options::SetInputValues(cResourceVarsObject& aObj)
{
    mbSettingInitialValues = true;
    ////////////////////////////////
    // Game options
    {
        // Show hints
        mpChBShowSubtitles->SetChecked(aObj.GetVarBool("ShowSubtitles"), false);
        mpChBShowEffectSubtitles->SetChecked(aObj.GetVarBool("ShowEffectSubtitles"), false);
        mpChBShowHints->SetChecked(aObj.GetVarBool("ShowHints"), false);
        mpChBShowDeathHints->SetChecked(aObj.GetVarBool("ShowDeathHints"), false);

        mpChBShowCrosshair->SetChecked(aObj.GetVarBool("ShowCrosshair"), false);

        mpCBFocusIconStyle->SetSelectedItem(aObj.GetVarInt("FocusIconStyle"), false, false);
        if(mpCBFocusIconStyle->GetSelectedItem() == -1)
        {
            mpCBFocusIconStyle->SetSelectedItem(0, false, true);
        }

        mpCBHandOrientation->SetSelectedItem(aObj.GetVarInt("HandOrientation"), false, false);
        if(mpCBHandOrientation->GetSelectedItem() == -1)
        {
            mpCBHandOrientation->SetSelectedItem(0, false, true);
        }

        if(mbShowCommentary)
        {
            mpChBShowCommentary->SetChecked(aObj.GetVarBool("ShowCommentary"), false);
        }

        float mfFlashIntensity = aObj.GetVarFloat("FlashIntensity");
        SetSliderValue(mpSFlash, mfFlashIntensity, false, mfFlashMin, mfFlashMax);
        SetFlashLabelString(mfFlashIntensity);

        // Language
        {
            tString sLang = aObj.GetVarString("Language", gpBase->msDefaultGameLanguage);
            int lLangIndex = 0;
            for(int i = 0; i < (int)mvLangFiles.size(); ++i)
            {
                if(sLang == cString::To8Char(mvLangFiles[i]))
                {
                    lLangIndex = i;
                    break;
                }
            }
            mpCBLanguage->SetSelectedItem(lLangIndex, false, false);
        }
    }
    ////////////////////////////////
    // Graphics options
    {

        /////////////////////////
        // Resolution
        {
            int lCurrentMode = -1;
            bool bGenerateCallback = false;
            tVideoModeVec vVidModes;
            cPlatform::GetAvailableVideoModes(vVidModes, 32);

            cVector2f vCurrentResf = aObj.GetVarVector2f("Resolution");
            cVideoMode vCurrentRes = cVideoMode(cVector2l((int)vCurrentResf.x, (int)vCurrentResf.y), -1, -1);

            /////////////////
            // Remove duplicates
            for(size_t i = 0; i < vVidModes.size(); ++i)
            {
                int lRemove = 0;
                int lRefreshRate = vVidModes[i].mlRefreshRate;

                //////////////
                // Move forward until there are no more matches
                for(size_t j = i + 1; j < vVidModes.size(); ++j)
                {
                    if(vVidModes[i].mvScreenSize == vVidModes[j].mvScreenSize)
                    {
                        lRemove++;
                        lRefreshRate = cMath::Max(lRefreshRate, vVidModes[i].mlRefreshRate);
                    }
                    else
                    {
                        break;
                    }
                }

                //////////////
                // Remove dupilcates and get the one with the highest refreshrate
                for(size_t j = i + lRemove; j < vVidModes.size(); j++)
                {
                    vVidModes[j - lRemove] = vVidModes[j];
                }
                vVidModes[i].mlRefreshRate = lRefreshRate;

                while(lRemove--)
                {
                    vVidModes.pop_back();
                }
            }

            mpCBResolution->ClearItems();
            for(size_t i = 0; i < vVidModes.size(); ++i)
            {
                const cVideoMode& mode = vVidModes[i];
                tWString sRes;

                sRes = cString::ToStringW(mode.mvScreenSize.x) + _W("x") + cString::ToStringW(mode.mvScreenSize.y);

                mpCBResolution->AddItem(sRes);
                mvScreenSizes.push_back(mode);

                if(mode == vCurrentRes)
                {
                    lCurrentMode = mpCBResolution->GetItemNum() - 1;
                }
            }
            if(lCurrentMode == -1)
            {
                tWString sRes = cString::ToStringW(vCurrentRes.mvScreenSize.x) + _W("x") + cString::ToStringW(vCurrentRes.mvScreenSize.y) + _W(" (Custom)");

                mpCBResolution->AddItem(sRes);
                mvScreenSizes.push_back(vCurrentRes);
                lCurrentMode = mpCBResolution->GetItemNum() - 1;

                //lCurrentMode=0;
                //bGenerateCallback = true;
            }
            mpCBResolution->SetSelectedItem(lCurrentMode, true, false);
        }

        /////////////////////////
        // Fullscreen & vsync
        mpChBFullScreen->SetChecked(aObj.GetVarBool("FullScreen"), false);
        mpChBVSync->SetChecked(aObj.GetVarBool("VSync"), false);

        /////////////////////////
        // Texture quality and filtering
        {
            /////////////////////////////////
            // Texture Quality
            mpCBTextureSizeLevel->SetSelectedItem((mpCBTextureSizeLevel->GetItemNum() - 1) - aObj.GetVarInt("TextureQuality"), true, false);

            /////////////////////////////////
            // Texture filtering
            tWStringVec vTexFilterStrings;
            vTexFilterStrings.push_back(kTranslate("OptionsMenu", "Nearest"));
            vTexFilterStrings.push_back(kTranslate("OptionsMenu", "Bilinear"));
            vTexFilterStrings.push_back(kTranslate("OptionsMenu", "Trilinear"));

            mpCBTextureFilter->ClearItems();
            for(int i = 0; i < (int)vTexFilterStrings.size(); ++i)
            {
                mpCBTextureFilter->AddItem(vTexFilterStrings[i]);
            }

            mpCBTextureFilter->SetSelectedItem(aObj.GetVarInt("TextureFilter"), true, false);

            /////////////////////////////////
            // Anisotropy
            mpCBAnisotropy->ClearItems();
            mpCBAnisotropy->AddItem(kTranslate("OptionsMenu", "Off"));
            int lAnisoPow = 2;
            for(int i = 0; i < 4; ++i)
            {
                cWidgetItem *pItem = mpCBAnisotropy->AddItem(cString::ToStringW(lAnisoPow) + _W("X"));
                lAnisoPow *= 2;
            }
            mpCBAnisotropy->SetSelectedItem(GetIndexFromAnisotropy(aObj.GetVarFloat("TextureAnisotropy")));
        }

        /////////////////////////
        // Shadows & Parallax
        {
            mpChBShadows->SetChecked(aObj.GetVarBool("ShadowsActive"), false);

            tWStringVec vOptionStrings;
            vOptionStrings.push_back(kTranslate("OptionsMenu", "Low"));
            vOptionStrings.push_back(kTranslate("OptionsMenu", "Medium"));
            vOptionStrings.push_back(kTranslate("OptionsMenu", "High"));
            vOptionStrings.push_back(kTranslate("OptionsMenu", "Ultra"));

            mpCBShadowQuality->ClearItems();
            mpCBShadowRes->ClearItems();
            mpCBParallaxQuality->ClearItems();
            for(int i = 0; i < (int)vOptionStrings.size(); ++i)
            {
                mpCBShadowQuality->AddItem(vOptionStrings[i]);
                mpCBShadowRes->AddItem(vOptionStrings[i]);
            }

            mpCBShadowQuality->SetSelectedItem(aObj.GetVarInt("ShadowQuality"), true, false);
            mpCBShadowRes->SetSelectedItem(aObj.GetVarInt("ShadowResolution"), true, false);

            mpChBParallax->SetChecked(aObj.GetVarBool("ParallaxEnabled"), false);

            mpCBParallaxQuality->AddItem(kTranslate("OptionsMenu", "ParallaxSimple"));
            mpCBParallaxQuality->AddItem(kTranslate("OptionsMenu", "ParallaxRelief"));

            mpCBParallaxQuality->SetSelectedItem(aObj.GetVarInt("ParallaxQuality"), true, false);
        }

        /////////////////////////
        // Water
        {
            mpChBWorldReflection->SetChecked(aObj.GetVarBool("WorldReflection"), false);
            mpChBRefraction->SetChecked(aObj.GetVarBool("Refraction"), false);
        }

        /////////////////////////
        // SSAO
        {

            /////////////////////////////
            // Active
            mpChBSSAO->SetChecked(aObj.GetVarBool("SSAOActive"), false);

            /////////////////////////////
            // Samples
            int lCurrentSamples = aObj.GetVarInt("SSAONumOfSamples");
            mpCBSSAOSamples->SetSelectedItem(GetIndexFromSSAOSamples(lCurrentSamples), true, false);
            /*
            for(int i=0;i<(int)mpCBSSAOSamples->GetItemNum();++i)
            {
                cWidgetItem* pItem = mpCBSSAOSamples->GetItem(i);
                int lSamples = cString::ToInt(cString::To8Char(pItem->GetText()).c_str(), 0);
                if(lCurrentSamples == lSamples)
                {
                    lCurrentSamplesIdx = i;
                    break;
                }
            }
            */

            /////////////////////////////
            // Resolution
            mpCBSSAOResolution->SetSelectedItem(aObj.GetVarInt("SSAOResolution"));
        }

        /////////////////
        // PostEffects
        {
            cLuxMapHandler* pMapHdlr = gpBase->mpMapHandler;
            cLuxPostEffectHandler *pPostEffects = gpBase->mpPostEffectHandler;
            // Bloom
            mpChBBloom->SetChecked(aObj.GetVarBool("BloomActive"), false);
            // ImageTrail
            mpChBImageTrail->SetChecked(aObj.GetVarBool("ImageTrailActive"), false);
            // Sepia
            mpChBSepia->SetChecked(aObj.GetVarBool("SepiaActive"), false);
            // RadialBlur
            mpChBRadialBlur->SetChecked(aObj.GetVarBool("RadialBlurActive"), false);
            //Insanity
            mpChBInsanity->SetChecked(aObj.GetVarBool("InsanityActive"), false);
        }

        // Gamma
        float fGamma = aObj.GetVarFloat("Gamma");
        SetSliderValue(mpSGamma, fGamma, false, mfGammaMin, mfGammaMax);
        SetGammaLabelString(fGamma);
    }
    ////////////////////////////////
    // Input
    mpChBInvertMouse->SetChecked(aObj.GetVarBool("InvertMouse"), false);
    mpChBSmoothMouse->SetChecked(aObj.GetVarBool("SmoothMouse"), false);

    float fSensitivity = aObj.GetVarFloat("MouseSensitivity");
    SetSliderValue(mpSMouseSensitivity, fSensitivity, false, mfMouseSensitivityMin, mfMouseSensitivityMax);
    SetSensitivityLabelString(fSensitivity);

#if USE_GAMEPAD
    mpChBGamepadInvertLook->SetChecked(aObj.GetVarBool("GamepadInvertLook"), false);

    fSensitivity = aObj.GetVarFloat("GamepadLookSensitivity");
    SetSliderValue(mpSGamepadLookSensitivity, fSensitivity, false, mfGamepadLookSensitivityMin, mfGamepadLookSensitivityMax);
    SetGamepadLookSensitivityLabelString(fSensitivity);
#endif

    ////////////////////////////////
    // Sound
    float fVolume = aObj.GetVarFloat("SoundVolume");
    SetSliderValue(mpSVolume, fVolume, false, mfVolumeMin, mfVolumeMax);
    SetVolumeLabelString(fVolume);

    int lSndDev = aObj.GetVarInt("SoundDeviceID");
    int lSndDevIdx = -1;
    for(size_t i = 0; i < mvSoundDevices.size(); ++i)
    {
        if(lSndDev == mvSoundDevices[i]->GetID())
        {
            lSndDevIdx = (int)i;
            break;
        }
    }

    if(lSndDevIdx == -1)
    {
        mpCBSndDevice->SetSelectedItem(mpCBSndDevice->GetItemNum() - 1, true, false);
    }
    else
    {
        mpCBSndDevice->SetSelectedItem(lSndDevIdx, true, false);
    }

    mbSettingInitialValues = false;
}

//-----------------------------------------------------------------------

void cLuxMainMenu_Options::SetUpInput(cWidgetLabel* apLabel, iWidget* apInput, bool abNeedsRestart, const tWString& asMessage)
{
    cLuxOption_ExtData* pData = AddOptionData(abNeedsRestart, asMessage);
    if(apLabel)
    {
        apLabel->SetUserData(pData);
        apLabel->AddCallback(eGuiMessage_MouseMove, this, kGuiCallback(Option_OnMouseOver));
        apLabel->AddCallback(eGuiMessage_GetUINavFocus, this, kGuiCallback(Option_OnMouseOver));
    }
    apInput->SetUserData(pData);
    apInput->AddCallback(eGuiMessage_MouseMove, this, kGuiCallback(Option_OnMouseOver));
    apInput->AddCallback(eGuiMessage_GetUINavFocus, this, kGuiCallback(Option_OnMouseOver));

    eGuiMessage message = eGuiMessage_LastEnum;
    switch(apInput->GetType())
    {
    case eWidgetType_CheckBox:
        message = eGuiMessage_CheckChange;
        break;
    case eWidgetType_ComboBox:
        message = eGuiMessage_SelectionChange;
        break;
    case eWidgetType_Slider:
        message = eGuiMessage_SliderMove;
        break;
    case eWidgetType_Button:
        message = eGuiMessage_ButtonPressed;
        break;
    }
    if(message != eGuiMessage_LastEnum)
    {
        apInput->AddCallback(message, this, kGuiCallback(Option_OnChangeValue));
    }
}

//-----------------------------------------------------------------------

cLuxOption_ExtData *cLuxMainMenu_Options::AddOptionData(bool abNeedsRestart, const tWString& asMessage)
{
    mvOptionData.push_back(hplNew(cLuxOption_ExtData, (abNeedsRestart, asMessage)));
    return mvOptionData.back();
}

//-----------------------------------------------------------------------

void cLuxMainMenu_Options::SetCurrentTipWidget(iWidget* apWidget)
{
    if(mpCurrentTipWidget == apWidget)
    {
        mbTipFadeRestart = true;
        return;
    }

    mbTipWidgetUpdated = true;
    mpCurrentTipWidget = apWidget;
}

//-----------------------------------------------------------------------


void cLuxMainMenu_Options::ApplyChanges()
{
    // Reset initial values on next enter
    mInitialValues.AddVarBool("InitialValuesSet", false);

    cLuxConfigHandler* pCfgHdr = gpBase->mpConfigHandler;
    ///////////////////////////
    // Game
    {
        pCfgHdr->msLangFile = cString::To8Char(mvLangFiles[mpCBLanguage->GetSelectedItem()]);
        gpBase->mpMessageHandler->SetShowSubtitles(mpChBShowSubtitles->IsChecked());
        gpBase->mpMessageHandler->SetShowEffectSubtitles(mpChBShowEffectSubtitles->IsChecked());
        gpBase->mpHintHandler->SetActive(mpChBShowHints->IsChecked());
        gpBase->mpPlayer->GetHelperDeath()->SetShowHint(mpChBShowDeathHints->IsChecked());

        if(mbShowCommentary)
        {
            gpBase->mpMapHandler->SetShowCommentary(mpChBShowCommentary->IsChecked());
        }

        gpBase->mpPlayer->SetShowCrosshair(mpChBShowCrosshair->IsChecked());

        gpBase->mpPlayer->SetFocusIconStyle((eLuxFocusIconStyle)mpCBFocusIconStyle->GetSelectedItem());

        gpBase->mpPlayer->SetHandOrientation((eLuxHandOrientation)mpCBHandOrientation->GetSelectedItem());

        gpBase->mpEffectHandler->SetFlashIntensity(GetFlash());
    }


    ///////////////////////////
    // Graphics
    {
        cGraphics* pGfx = gpBase->mpEngine->GetGraphics();
        cRenderSettings* pRenderSettings = gpBase->mpMapHandler->GetViewport()->GetRenderSettings();
        cMaterialManager* pMatMgr = gpBase->mpEngine->GetResources()->GetMaterialManager();

        const cVideoMode vidMode = mvScreenSizes[mpCBResolution->GetSelectedItem()];
        pCfgHdr->mvScreenSize = vidMode.mvScreenSize;
        pCfgHdr->mbFullscreen = mpChBFullScreen->IsChecked();
        pCfgHdr->mbVSync = mpChBVSync->IsChecked();
        pGfx->GetLowLevel()->SetVsyncActive(pCfgHdr->mbVSync);
        pGfx->GetLowLevel()->SetGammaCorrection(GetGamma());

        // Texture
        pCfgHdr->mlTextureQuality = (mpCBTextureSizeLevel->GetItemNum() - 1) - mpCBTextureSizeLevel->GetSelectedItem();
        pCfgHdr->mlTextureFilter = mpCBTextureFilter->GetSelectedItem();
        pCfgHdr->mfTextureAnisotropy = GetAnisotropyFromIndex(mpCBAnisotropy->GetSelectedItem());

        pMatMgr->SetTextureAnisotropy(pCfgHdr->mfTextureAnisotropy);
        pMatMgr->SetTextureFilter((eTextureFilter)pCfgHdr->mlTextureFilter);

        // Parallax
        pCfgHdr->mbParallaxEnabled = mpChBParallax->IsChecked();
        pCfgHdr->mlParallaxQuality = mpCBParallaxQuality->GetSelectedItem();

        // Shadows
        pCfgHdr->mbShadowsActive = mpChBShadows->IsChecked();
        pCfgHdr->mlShadowQuality = mpCBShadowQuality->GetSelectedItem();
        pCfgHdr->mlShadowRes = mpCBShadowRes->GetSelectedItem();

        // Water
        pCfgHdr->mbWorldReflection = mpChBWorldReflection->IsChecked();
        pCfgHdr->mbRefraction = mpChBRefraction->IsChecked();

        //Update the viewport stuff
        gpBase->mpMapHandler->UpdateViewportRenderProperties();

        /////////////////////
        // PostEffects
        {
            cLuxMapHandler* pMapHdlr = gpBase->mpMapHandler;
            cLuxPostEffectHandler *pPostEffects = gpBase->mpPostEffectHandler;

            // Bloom
            pMapHdlr->GetPostEffect_Bloom()->SetDisabled(mpChBBloom->IsChecked() == false);
            // ImageTrail
            pMapHdlr->GetPostEffect_ImageTrail()->SetDisabled(mpChBImageTrail->IsChecked() == false);
            // Sepia
            pMapHdlr->GetPostEffect_Sepia()->SetDisabled(mpChBSepia->IsChecked() == false);
            // RadialBlur
            pMapHdlr->GetPostEffect_RadialBlur()->SetDisabled(mpChBRadialBlur->IsChecked() == false);
            //Insanity
            pPostEffects->GetInsanity()->SetDisabled(mpChBInsanity->IsChecked() == false);
        }

        // SSAO
        pCfgHdr->mbSSAOActive = mpChBSSAO->IsChecked();
        pCfgHdr->mlSSAOSamples = GetSSAOSamplesFromIndex(mpCBSSAOSamples->GetSelectedItem());
        pCfgHdr->mlSSAOResolution = mpCBSSAOResolution->GetSelectedItem();
    }


    //////////////////////////////
    // Input
    gpBase->mpInputHandler->SetInvertMouse(mpChBInvertMouse->IsChecked());
    gpBase->mpInputHandler->SetSmoothMouse(mpChBSmoothMouse->IsChecked());

    gpBase->mpInputHandler->SetMouseSensitivity(GetSensitivity());

#if USE_GAMEPAD
    gpBase->mpInputHandler->SetGamepadLookSensitivity(GetGamepadLookSensitivity());
    gpBase->mpInputHandler->SetInvertGamepadLook(mpChBGamepadInvertLook->IsChecked());
#endif

    /*
    /////////////////////////////
    // Sound
    gpBase->mpEngine->GetSound()->GetLowLevel()->SetVolume(GetVolume());
    cWidgetItem* pItem = mpCBSndDevice->GetItem(mpCBSndDevice->GetSelectedItem());
    if(pItem)
    {
        iSoundDeviceIdentifier* pSndDev = (iSoundDeviceIdentifier*)pItem->GetUserData();
        gpBase->mpConfigHandler->mlSoundDevID = pSndDev->GetID();
    }
    */

    //////////////////////////////////////////////////////////////
    //--- REMOVED - Language is now loaded only at startup
    //if(gpBase->LoadLanguage(cString::To8Char(mvLangFiles[mpCBLanguage->GetSelectedItem()])))
    //    gpBase->mpMainMenu->RecreateGui();
}

//-----------------------------------------------------------------------

void cLuxMainMenu_Options::SetFlashLabelString(float afX)
{
    SetSliderLabelString(mpLFlash, afX, mfFlashMin, mfFlashMax);
}

//-----------------------------------------------------------------------

void cLuxMainMenu_Options::SetGammaLabelString(float afX)
{
    SetSliderLabelString(mpLGamma, afX, mfGammaMin, mfGammaMax);
}

//-----------------------------------------------------------------------

void cLuxMainMenu_Options::SetSensitivityLabelString(float afX)
{
    SetSliderLabelString(mpLMouseSensitivity, afX, mfMouseSensitivityMin, mfMouseSensitivityMax);
}

//-----------------------------------------------------------------------

#if USE_GAMEPAD
void cLuxMainMenu_Options::SetGamepadLookSensitivityLabelString(float afX)
{
    SetSliderLabelString(mpLGamepadLookSensitivity, afX, mfGamepadLookSensitivityMin, mfGamepadLookSensitivityMax);
}
#endif

//-----------------------------------------------------------------------

void cLuxMainMenu_Options::SetVolumeLabelString(float afX)
{
    SetSliderLabelString(mpLVolume, afX * 10.0f, mfVolumeMin * 10.0f, mfVolumeMax * 10.0f);
}

//-----------------------------------------------------------------------

bool cLuxMainMenu_Options::ChangeLanguage(iWidget* apWidget, const cGuiMessageData& aData)
{
    ///////////////////////////////////////////////////////
    // --- REMOVED - Language is now loaded only at startup

    //mInitialValues.AddVarBool("SettingLanguage", true);

    //DumpCurrentValues(mCurrentValues);

    //gpBase->LoadLanguage(cString::To8Char(mvLangFiles[mpCBLanguage->GetSelectedItem()]));
    //gpBase->mpMainMenu->RecreateGui();

    return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_Options, ChangeLanguage);

//-----------------------------------------------------------------------

void cLuxMainMenu_Options::PopulateLanguageList()
{
    mpCBLanguage->ClearItems();
    mvLangFiles.clear();

    tWStringList lstLangs;
    tWString sPath = cString::To16Char(gpBase->msGameLanguageFolder);
    cPlatform::FindFilesInDir(lstLangs, sPath, _W("*.lang"));

    tWStringListIt it = lstLangs.begin();
    for(; it != lstLangs.end(); ++it)
    {
        tWString sLang = *it;

        tWString sLangEntry = cString::ToLowerCaseW(cString::SubW(sLang, 0,
                              cString::GetLastStringPosW(sLang, _W("."))));

        mpCBLanguage->AddItem(kTranslate("Languages", cString::To8Char(sLangEntry)));
        mvLangFiles.push_back(sLang);
    }
}

//-----------------------------------------------------------------------

void cLuxMainMenu_Options::PopulateSoundDevices()
{
    iLowLevelSound* pLowLevelSound = gpBase->mpEngine->GetSound()->GetLowLevel();
    iSoundDeviceIdentifier* pCurSndDev = pLowLevelSound->GetCurrentSoundDevice();

    mpCBSndDevice->ClearItems();
    mvSoundDevices.clear();

    mvSoundDevices = pLowLevelSound->GetFilteredSoundDevices();
    if(mvSoundDevices.empty())
    {
        cWidgetItem* pItem = mpCBSndDevice->AddItem(pCurSndDev->GetName());
        pItem->SetUserData(pCurSndDev);
    }
    else
    {
        bool bCurrentDevFound = false;
        for(int i = 0; i < (int)mvSoundDevices.size(); ++i)
        {
            iSoundDeviceIdentifier* pSndDev = mvSoundDevices[i];
            if(bCurrentDevFound == false && pSndDev == pCurSndDev)
            {
                bCurrentDevFound = true;
            }

            cWidgetItem* pItem = mpCBSndDevice->AddItem(pSndDev->GetName());
            pItem->SetUserData(pSndDev);
        }
        if(bCurrentDevFound == false)
        {
            cWidgetItem* pItem = mpCBSndDevice->AddItem("(Unsupported) " + pCurSndDev->GetName());
            pItem->SetUserData(pCurSndDev);
        }
    }
}

//-----------------------------------------------------------------------

void cLuxMainMenu_Options::SetUpSlider(cWidgetSlider* apSlider, float afMinValue, float afMaxValue, float afStepValue, tGuiCallbackFunc apCallback, cWidgetLabel** apValueDisplay)
{
    int lMaxValue = cMath::RoundToInt((afMaxValue - afMinValue) / afStepValue);
    apSlider->SetMaxValue(lMaxValue);
    apSlider->SetBarValueSize(cMath::RoundToInt(0.25f * (float)lMaxValue));

    if(apCallback)
    {
        apSlider->AddCallback(eGuiMessage_SliderMove, this, apCallback);
    }

    if(apValueDisplay)
    {
        cWidgetLabel* pLabel = mpGuiSet->CreateWidgetLabel(cVector3f(apSlider->GetSize().x * 0.5f, 2, 1), -1, _W(""), apSlider);
        pLabel->SetTextAlign(eFontAlign_Center);

        *apValueDisplay = pLabel;
    }
}

//-----------------------------------------------------------------------

void cLuxMainMenu_Options::SetSliderValue(cWidgetSlider* apSlider, float afValue, bool abGenCallback, float afMinValue, float afMaxValue)
{
    afValue = cMath::Clamp(afValue, afMinValue, afMaxValue);

    float fMaxSliderValue = (float) apSlider->GetMaxValue();
    float fRange = afMaxValue - afMinValue;

    int lValue = cMath::RoundToInt((afValue - afMinValue) * fMaxSliderValue / fRange);

    apSlider->SetValue(lValue, abGenCallback);
}

//-----------------------------------------------------------------------

float cLuxMainMenu_Options::GetSliderValue(cWidgetSlider* apSlider, float afMinValue, float afMaxValue)
{
    float fSliderRelValue = ((float)apSlider->GetValue()) / (float)apSlider->GetMaxValue();
    float fRange = afMaxValue - afMinValue;

    return afMinValue + fRange * fSliderRelValue;
}

//-----------------------------------------------------------------------

void cLuxMainMenu_Options::SetSliderLabelString(cWidgetLabel* apLabel, float afValue,
        float afMinValue, float afMaxValue,
        const tWString& asMin, const tWString& asMax)
{
    tWString sText;

    if(afValue <= afMinValue)
    {
        if(asMin.empty())
        {
            afValue = afMinValue;
        }
        else
        {
            sText = asMin;
        }
    }
    else if(afValue >= afMaxValue)
    {
        if(asMax.empty())
        {
            afValue = afMaxValue;
        }
        else
        {
            sText = asMax;
        }
    }

    if(sText.empty())
    {
        sText = cString::ToStringW(afValue, 3, true);
    }

    apLabel->SetText(sText);
}

//-----------------------------------------------------------------------

void cLuxMainMenu_Options::DumpInitialValues(cResourceVarsObject &aObj)
{
    ////////////////////////////////
    // Game options
    {
        // Show hints
        aObj.AddVarBool("ShowHints", gpBase->mpHintHandler->IsActive());
        aObj.AddVarBool("ShowDeathHints", gpBase->mpPlayer->GetHelperDeath()->ShowHint());
        aObj.AddVarBool("ShowSubtitles", gpBase->mpMessageHandler->ShowSubtitles());
        aObj.AddVarBool("ShowEffectSubtitles", gpBase->mpMessageHandler->ShowEffectSubtitles());

        aObj.AddVarBool("ShowCrosshair", gpBase->mpPlayer->GetShowCrosshair());
        aObj.AddVarInt("FocusIconStyle", gpBase->mpPlayer->GetFocusIconStyle());
        aObj.AddVarInt("HandOrientation", gpBase->mpPlayer->GetHandOrientation());
        aObj.AddVarBool("ShowCommentary", gpBase->mpMapHandler->GetShowCommentary());
        aObj.AddVarFloat("FlashIntensity", gpBase->mpEffectHandler->GetFlashIntensity());

        // Language
        aObj.AddVarString("Language", gpBase->mpConfigHandler->msLangFile);
    }
    ////////////////////////////////
    // Graphics options
    {
        /////////////////////////
        // Resolution
        const cVector2l& vResolution = gpBase->mpConfigHandler->mvScreenSize;
        cVector2f vResolutionf = cVector2f((float)vResolution.x, (float)vResolution.y);
        aObj.AddVarVector2f("Resolution", vResolutionf);

        /////////////////////////
        // Fullscreen & vsync
        aObj.AddVarBool("FullScreen", gpBase->mpConfigHandler->mbFullscreen);
        aObj.AddVarBool("VSync", gpBase->mpConfigHandler->mbVSync);

        /////////////////////////
        // Texture quality and filtering
        aObj.AddVarInt("TextureQuality", gpBase->mpConfigHandler->mlTextureQuality);
        aObj.AddVarInt("TextureFilter", gpBase->mpConfigHandler->mlTextureFilter);
        aObj.AddVarFloat("TextureAnisotropy", gpBase->mpConfigHandler->mfTextureAnisotropy);

        /////////////////////////
        // Shadows & Parallax
        aObj.AddVarBool("ShadowsActive", gpBase->mpConfigHandler->mbShadowsActive);
        aObj.AddVarInt("ShadowQuality", gpBase->mpConfigHandler->mlShadowQuality);
        aObj.AddVarInt("ShadowResolution", gpBase->mpConfigHandler->mlShadowRes);
        aObj.AddVarBool("ParallaxEnabled", gpBase->mpConfigHandler->mbParallaxEnabled);
        aObj.AddVarInt("ParallaxQuality", gpBase->mpConfigHandler->mlParallaxQuality);

        /////////////////////////
        // Water
        aObj.AddVarBool("WorldReflection", gpBase->mpConfigHandler->mbWorldReflection);
        aObj.AddVarBool("Refraction", gpBase->mpConfigHandler->mbRefraction);

        /////////////////////////
        // SSAO
        aObj.AddVarBool("SSAOActive", gpBase->mpConfigHandler->mbSSAOActive);
        aObj.AddVarInt("SSAONumOfSamples", gpBase->mpConfigHandler->mlSSAOSamples);
        aObj.AddVarInt("SSAOResolution", gpBase->mpConfigHandler->mlSSAOResolution);

        /////////////////
        // PostEffects
        cLuxMapHandler* pMapHdlr = gpBase->mpMapHandler;
        cLuxPostEffectHandler *pPostEffects = gpBase->mpPostEffectHandler;
        aObj.AddVarBool("BloomActive", pMapHdlr->GetPostEffect_Bloom()->IsDisabled() == false);
        aObj.AddVarBool("ImageTrailActive", pMapHdlr->GetPostEffect_ImageTrail()->IsDisabled() == false);
        aObj.AddVarBool("SepiaActive", pMapHdlr->GetPostEffect_Sepia()->IsDisabled() == false);
        aObj.AddVarBool("RadialBlurActive", pMapHdlr->GetPostEffect_RadialBlur()->IsDisabled() == false);
        aObj.AddVarBool("InsanityActive", pPostEffects->GetInsanity()->IsDisabled() == false);


        ///////////////////
        // Gamma
        aObj.AddVarFloat("Gamma", gpBase->mpEngine->GetGraphics()->GetLowLevel()->GetGammaCorrection());
    }

    ////////////////////////////////
    // Input
    aObj.AddVarBool("InvertMouse", gpBase->mpInputHandler->GetInvertMouse());
    aObj.AddVarBool("SmoothMouse", gpBase->mpInputHandler->GetSmoothMouse());
    aObj.AddVarFloat("MouseSensitivity", gpBase->mpInputHandler->GetMouseSensitivity());

#if USE_GAMEPAD
    aObj.AddVarBool("GamepadInvertLook", gpBase->mpInputHandler->GetInvertGamepadLook());
    aObj.AddVarFloat("GamepadLookSensitivity", gpBase->mpInputHandler->GetGamepadLookSensitivity());
#endif

    ////////////////////////////////
    // Sound
    aObj.AddVarFloat("SoundVolume", gpBase->mpEngine->GetSound()->GetLowLevel()->GetVolume());
    aObj.AddVarInt("SoundDeviceID", gpBase->mpConfigHandler->mlSoundDevID);
}

//-----------------------------------------------------------------------

void cLuxMainMenu_Options::DumpCurrentValues(cResourceVarsObject &aObj)
{
    ////////////////////////////////
    // Game options
    {
        aObj.AddVarBool("ShowHints",        mpChBShowHints->IsChecked());
        aObj.AddVarBool("ShowDeathHints",    mpChBShowDeathHints->IsChecked());
        aObj.AddVarBool("ShowSubtitles",    mpChBShowSubtitles->IsChecked());
        aObj.AddVarBool("ShowEffectSubtitles", mpChBShowEffectSubtitles->IsChecked());

        aObj.AddVarBool("ShowCrosshair",    mpChBShowCrosshair->IsChecked());
        aObj.AddVarInt("FocusIconStyle",    mpCBFocusIconStyle->GetSelectedItem());
        aObj.AddVarInt("HandOrientation",    mpCBHandOrientation->GetSelectedItem());
        aObj.AddVarBool("ShowCommentary",   mpChBShowCommentary->IsChecked());
        aObj.AddVarFloat("FlashIntensity",  GetFlash());

        aObj.AddVarString("Language",        cString::To8Char(mvLangFiles[mpCBLanguage->GetSelectedItem()]));
    }

    ////////////////////////////////
    // Graphics options
    {
        /////////////////////////
        // Resolution
        const cVideoMode& vResolution = mvScreenSizes[mpCBResolution->GetSelectedItem()];
        cVector2f vResolutionf = cVector2f((float)vResolution.mvScreenSize.x, (float)vResolution.mvScreenSize.y);
        aObj.AddVarVector2f("Resolution", vResolutionf);

        /////////////////////////
        // Fullscreen & vsync
        aObj.AddVarBool("FullScreen",    mpChBFullScreen->IsChecked());
        aObj.AddVarBool("VSync",        mpChBVSync->IsChecked());

        /////////////////////////
        // Texture quality and filtering
        aObj.AddVarInt("TextureQuality", (mpCBTextureSizeLevel->GetItemNum() - 1) - mpCBTextureSizeLevel->GetSelectedItem());
        aObj.AddVarInt("TextureFilter", mpCBTextureFilter->GetSelectedItem());
        aObj.AddVarFloat("TextureAnisotropy", GetAnisotropyFromIndex(mpCBAnisotropy->GetSelectedItem()));

        /////////////////////////
        // Shadows & Parallax
        aObj.AddVarBool("ShadowsActive", mpChBShadows->IsChecked());
        aObj.AddVarInt("ShadowQuality",  mpCBShadowQuality->GetSelectedItem());
        aObj.AddVarInt("ShadowResolution", mpCBShadowRes->GetSelectedItem());

        aObj.AddVarBool("ParallaxEnabled", mpChBParallax->IsChecked());
        aObj.AddVarInt("ParallaxQuality",  mpCBParallaxQuality->GetSelectedItem());

        /////////////////////////
        // Water
        aObj.AddVarBool("WorldReflection", mpChBWorldReflection->IsChecked());
        aObj.AddVarBool("Refraction", mpChBRefraction->IsChecked());

        /////////////////////////
        // SSAO
        aObj.AddVarBool("SSAOActive", mpChBSSAO->IsChecked());
        aObj.AddVarInt("SSAONumOfSamples", GetSSAOSamplesFromIndex(mpCBSSAOSamples->GetSelectedItem()));
        aObj.AddVarInt("SSAOResolution", mpCBSSAOResolution->GetSelectedItem());

        /////////////////
        // PostEffects
        aObj.AddVarBool("BloomActive", mpChBBloom->IsChecked());
        aObj.AddVarBool("ImageTrailActive", mpChBImageTrail->IsChecked());
        aObj.AddVarBool("SepiaActive", mpChBSepia->IsChecked());
        aObj.AddVarBool("RadialBlurActive", mpChBRadialBlur->IsChecked());
        aObj.AddVarBool("InsanityActive", mpChBInsanity->IsChecked());

        ///////////////////
        // Gamma
        aObj.AddVarFloat("Gamma", GetGamma());
    }

    ////////////////////////////////
    // Input
    aObj.AddVarBool("InvertMouse", mpChBInvertMouse->IsChecked());
    aObj.AddVarBool("SmoothMouse", mpChBSmoothMouse->IsChecked());
    aObj.AddVarFloat("MouseSensitivity", GetSensitivity());

#if USE_GAMEPAD
    aObj.AddVarBool("GamepadInvertLook", mpChBGamepadInvertLook->IsChecked());
    aObj.AddVarFloat("GamepadLookSensitivity", GetGamepadLookSensitivity());
#endif

    ////////////////////////////////
    // Sound
    aObj.AddVarFloat("SoundVolume", GetVolume());
    cWidgetItem* pItem = mpCBSndDevice->GetItem(mpCBSndDevice->GetSelectedItem());
    int lDevID = -1;
    if(pItem)
    {
        iSoundDeviceIdentifier* pSndDev = (iSoundDeviceIdentifier*)pItem->GetUserData();
        lDevID = pSndDev->GetID();
    }

    aObj.AddVarInt("SoundDeviceID", lDevID);
}


//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CALLBACKS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cLuxMainMenu_Options::Window_OnUpdate(iWidget* apWidget, const cGuiMessageData& aData)
{
    ///////////////////////////////////////////////////
    // If there is a popup active, dont update tips
    if(mpGuiSet->PopUpIsActive())
    {
        return true;
    }

    ////////////////////////////////////////////////////
    // Update Tip label fade
    cColor labelCol = mpLTip->GetDefaultFontColor();
    if(mbTipFadeRestart)
    {
        if(mbTipTextReset)
        {
            mbTipTextReset = false;
            mpLTip->SetScrollOffset(0);
        }
        labelCol = labelCol + cColor(0, aData.mfVal * 3);
        if(labelCol.a >= 1.0f)
        {
            labelCol.a = 1.0f;
            mbTipFadeRestart = false;
        }
        mpLTip->SetDefaultFontColor(labelCol);
    }
    else
    {
        if(labelCol.a > 0.0f)
        {
            mpLTip->SetDefaultFontColor(labelCol - cColor(0, aData.mfVal * 0.8f));
        }
        else if(mbTipTextReset == false)
        {
            mbTipTextReset = true;
        }
    }

    ///////////////////////////////////////////////////
    // Update Tip label text

    if(mbTipWidgetUpdated)
    {
        mbTipWidgetUpdated = false;

        if(mpCurrentTipWidget && mpCurrentTipWidget->GetUserData())
        {
            cLuxOption_ExtData* pData = (cLuxOption_ExtData*)mpCurrentTipWidget->GetUserData();
            tWString sRestart = pData->mbNeedsRestart ? _W(" (") + kTranslate("OptionsMenu", "ReqRestart") + _W(")") : _W("");

            mpLTip->SetText(pData->msMessage + sRestart);
        }
    }

    return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_Options, Window_OnUpdate);

//-----------------------------------------------------------------------

bool cLuxMainMenu_Options::Option_OnMouseOver(iWidget* apWidget, const cGuiMessageData& aData)
{
    if(apWidget->GetUserData() == NULL)
    {
        return true;
    }

    SetCurrentTipWidget(apWidget);

    /////////////
    // Make sure the focused widget is visible
    if(aData.mMessage == eGuiMessage_GetUINavFocus)
    {
        iWidget* pParent = apWidget->GetParent();

        if(pParent)
        {
            iWidget* pParent2 = pParent->GetParent();
            cWidgetFrame* pFrame = dynamic_cast<cWidgetFrame *>(pParent2);

            if(pFrame == NULL
                    && pParent2 != NULL)
            {
                pFrame = dynamic_cast<cWidgetFrame *>(pParent2->GetParent());
            }

            if(pFrame)
            {
                cVector3f vFramePos = pFrame->GetGlobalPosition();
                cVector3f vWidgedPos = apWidget->GetGlobalPosition();
                cVector3f vScrollAmount = pFrame->GetScrollAmount();
                cVector3f vScrollPos = vWidgedPos + vScrollAmount - vFramePos;
                vScrollPos.y -= 150;
                pFrame->ScrollToPosition(cVector2f(vScrollPos.x, vScrollPos.y));
            }
        }
    }
    return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_Options, Option_OnMouseOver);

//-----------------------------------------------------------------------

bool cLuxMainMenu_Options::Option_OnChangeValue(iWidget* apWidget, const cGuiMessageData& aData)
{
    if(mbSettingInitialValues)
    {
        return true;
    }

    if(apWidget->GetUserData())
    {
        cLuxOption_ExtData* pData = (cLuxOption_ExtData*)apWidget->GetUserData();
        if(pData->mbNeedsRestart)
        {
            gpBase->mpConfigHandler->SetGameNeedsRestart();
        }
    }
    return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_Options, Option_OnChangeValue);

//-----------------------------------------------------------------------

bool cLuxMainMenu_Options::FlashSlider_OnMove(iWidget* apWidget, const cGuiMessageData& aData)
{
    float mfFlashIntensity = GetFlash();
    SetSliderLabelString(mpLFlash, mfFlashIntensity, mfFlashMin, mfFlashMax);

    gpBase->mpEffectHandler->SetFlashIntensity(mfFlashIntensity);

    return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_Options, FlashSlider_OnMove);

//-----------------------------------------------------------------------

bool cLuxMainMenu_Options::GammaSlider_OnMove(iWidget* apWidget, const cGuiMessageData& aData)
{
    float fGamma = GetGamma();
    SetGammaLabelString(fGamma);

    gpBase->mpEngine->GetGraphics()->GetLowLevel()->SetGammaCorrection(fGamma);

    return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_Options, GammaSlider_OnMove);

//-----------------------------------------------------------------------

bool cLuxMainMenu_Options::MouseSensitivitySlider_OnMove(iWidget* apWidget, const cGuiMessageData& aData)
{
    float fSensitivity = GetSensitivity();
    SetSensitivityLabelString(fSensitivity);

    gpBase->mpInputHandler->SetMouseSensitivity(fSensitivity);

    return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_Options, MouseSensitivitySlider_OnMove);

//-----------------------------------------------------------------------

#if USE_GAMEPAD
bool cLuxMainMenu_Options::GamepadLookSensitivitySlider_OnMove(iWidget* apWidget, const cGuiMessageData& aData)
{
    float fSensitivity = GetGamepadLookSensitivity();
    SetGamepadLookSensitivityLabelString(fSensitivity);

    gpBase->mpInputHandler->SetGamepadLookSensitivity(fSensitivity);

    return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_Options, GamepadLookSensitivitySlider_OnMove);
#endif

//-----------------------------------------------------------------------

bool cLuxMainMenu_Options::SoundSlider_OnMove(iWidget* apWidget, const cGuiMessageData& aData)
{
    float fSndVol = GetVolume();
    SetVolumeLabelString(fSndVol);

    cSound *pSound = gpBase->mpEngine->GetSound();

    pSound->GetLowLevel()->SetVolume(fSndVol);

    return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_Options, SoundSlider_OnMove);

//-----------------------------------------------------------------------

bool cLuxMainMenu_Options::PressToggleShowGfxOptions(iWidget* apWidget, const cGuiMessageData& aData)
{
    bool bBasicVisible = mpDBasicGfxOptions->IsVisible();
    tWString sText[] = { kTranslate("OptionsMenu", "BasicOptions"), kTranslate("OptionsMenu", "AdvancedOptions") };

    mpDBasicGfxOptions->SetVisible(bBasicVisible == false);
    mpDBasicGfxOptions->SetEnabled(bBasicVisible == false);

    mpDAdvancedGfxOptions->SetVisible(bBasicVisible);
    mpDAdvancedGfxOptions->SetEnabled(bBasicVisible);

    cWidgetTab* pTab = mpTabGraphics;

    if(bBasicVisible)
    {
        mpBToggleShowGfxOptions->SetFocusNavigation(eUIArrow_Up, mpChBRefraction);

        pTab->GetTabLabel()->SetUserData(mpBToggleShowGfxOptions);
    }
    else
    {
        mpBToggleShowGfxOptions->SetFocusNavigation(eUIArrow_Up, mpSGamma);

        pTab->GetTabLabel()->SetUserData(mpCBResolution);
    }

    apWidget->SetText(sText[bBasicVisible == false]);

    return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_Options, PressToggleShowGfxOptions);

//-----------------------------------------------------------------------

bool cLuxMainMenu_Options::PressKeyConfig(iWidget* apWidget, const cGuiMessageData& aData)
{
    mbKeyConfigOpen = true;
    gpBase->mpMainMenu->SetWindowActive(eLuxMainMenuWindow_KeyConfig);

    return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_Options, PressKeyConfig);

//-----------------------------------------------------------------------

bool cLuxMainMenu_Options::PressOK(iWidget* apWidget, const cGuiMessageData& aData)
{
    ApplyChanges();
    gpBase->SaveConfig();
    //If some major stuff (that needs restart) have been changed. Then say so!
    if(gpBase->mpConfigHandler->ShowRestartWarning(mpGuiSet, this, kGuiCallback(MessageBoxCallback)) == false)
    {
        gpBase->mpMainMenu->SetWindowActive(eLuxMainMenuWindow_LastEnum);
    }

    return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_Options, PressOK);

//-----------------------------------------------------------------------

bool cLuxMainMenu_Options::PressCancel(iWidget* apWidget, const cGuiMessageData& aData)
{
    //Cancel any changes made.

    /////////////////////////////////////////////////////////////
    // --- REMOVED - Language is now loaded only at startup
    //if(gpBase->msCurrentLanguage!=mInitialValues.GetVarString("Language"))
    //{
    //    gpBase->LoadLanguage(mInitialValues.GetVarString("Language"));
    //    gpBase->mpMainMenu->RecreateGui();
    //}
    gpBase->mpEngine->GetGraphics()->GetLowLevel()->SetGammaCorrection(mInitialValues.GetVarFloat("Gamma"));
    gpBase->mpInputHandler->SetMouseSensitivity(mInitialValues.GetVarFloat("MouseSensitivity"));
#if USE_GAMEPAD
    gpBase->mpInputHandler->SetGamepadLookSensitivity(mInitialValues.GetVarFloat("GamepadLookSensitivity"));
#endif
    gpBase->mpEngine->GetSound()->GetLowLevel()->SetVolume(mInitialValues.GetVarFloat("SoundVolume"));

    gpBase->mpEffectHandler->SetFlashIntensity(mInitialValues.GetVarFloat("FlashIntensity", 1.0f));

    MessageBoxCallback(apWidget, aData);

    return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_Options, PressCancel);

//-----------------------------------------------------------------------

bool cLuxMainMenu_Options::UIPressCancel(iWidget* apWidget, const cGuiMessageData& aData)
{
    if(aData.mlVal == eUIButton_Secondary)
    {
        return PressCancel(apWidget, aData);
    }

    return false;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_Options, UIPressCancel);


//-----------------------------------------------------------------------

void cLuxMainMenu_Options::OnSetActive(bool abX)
{
    // If the key config menu is open, dont touch anything. Else, set up initial values.
    if(abX)
    {
        SetTabNavigation(mpTabGame->GetParentTabFrame()->GetTabOnTop(), true);

        if(mbKeyConfigOpen == false)
        {
            if(mInitialValues.GetVarBool("SettingLanguage", false))
            {
                SetInputValues(mCurrentValues);
                mInitialValues.AddVarBool("SettingLanguage", false);
            }
            else
            {
                if(mInitialValues.GetVarBool("InitialValuesSet") == false)
                {
                    DumpInitialValues(mInitialValues);
                }

                SetInputValues(mInitialValues);
                mInitialValues.AddVarBool("InitialValuesSet", true);
            }
        }
        else
        {
            mbKeyConfigOpen = false;
        }
    }

#if USE_GAMEPAD
    if(mpShoulderHint[0])
    {
        mpShoulderHint[0]->SetVisible(gpBase->mpInputHandler->IsGamepadPresent());
        mpShoulderHint[1]->SetVisible(gpBase->mpInputHandler->IsGamepadPresent());
    }
#endif
}

//-----------------------------------------------------------------------

bool cLuxMainMenu_Options::MessageBoxCallback(iWidget* apWidget, const cGuiMessageData& aData)
{
    gpBase->mpMainMenu->SetWindowActive(eLuxMainMenuWindow_LastEnum);

    return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_Options, MessageBoxCallback);

//-----------------------------------------------------------------------

bool cLuxMainMenu_Options::TabFrame_OnPageChange(iWidget* apWidget, const cGuiMessageData& aData)
{
    cWidgetTabFrame* pTabFrame = static_cast<cWidgetTabFrame *>(apWidget);
    cWidgetTab* pTab = pTabFrame->GetTab(aData.mlVal);

    SetTabNavigation(pTab, true);

    return true;
}
kGuiCallbackDeclaredFuncEnd(cLuxMainMenu_Options, TabFrame_OnPageChange);

//-----------------------------------------------------------------------

void cLuxMainMenu_Options::SetTabNavigation(cWidgetTab* apTab, bool abSetFocus)
{
    iWidget* pFirstWidget = static_cast<iWidget *>(apTab->GetTabLabel()->GetUserData());
    iWidget* pLastWidget = static_cast<iWidget *>(apTab->GetUserData());

    mpGuiSet->SetDefaultFocusNavWidget(pFirstWidget);
    if(abSetFocus)
    {
        mpGuiSet->SetFocusedWidget(pFirstWidget);
    }

    mpBOK->SetFocusNavigation(eUIArrow_Up, pLastWidget);
    mpBCancel->SetFocusNavigation(eUIArrow_Up, pLastWidget);
}

//-----------------------------------------------------------------------
