#include "LuxLoadScreenHandler.h"

#include "LuxHelpFuncs.h"
#include "LuxInputHandler.h"

#include "resources/WorldLoaderHplMap.h"

#include <math.h>

//-----------------------------------------------------------------------

static const float kLoadBarWorldStart = 0.05f;
static const float kLoadBarWorldRange = 0.80f;

static void WorldLoadProgressCallback(float afProgress, void *apUserData)
{
    cLuxLoadScreenHandler *pHandler = static_cast<cLuxLoadScreenHandler*>(apUserData);
    pHandler->SetLoadProgress(kLoadBarWorldStart + afProgress * kLoadBarWorldRange);
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cLuxLoadScreenHandler::cLuxLoadScreenHandler() : iLuxUpdateable("LuxLoadScreenHandler")
{
    ///////////////////////////////
    //Get Engine
    mpGui = gpBase->mpEngine->GetGui();
    mpScene = gpBase->mpEngine->GetScene();
    mpGraphics = gpBase->mpEngine->GetGraphics();

    ///////////////////////////////
    //Setup GUI stuff
    mpGuiSkin = mpGui->CreateSkin("gui_main_menu.skin");
    mpGuiSet = mpGui->CreateSet("LoadScreen", mpGuiSkin);

    mvGuiSetCenterSize = cVector2f(800, 600);
    LuxCalcGuiSetScreenOffset(mvGuiSetCenterSize, mvGuiSetSize, mvGuiSetOffset);
    mvGuiSetStartPos = cVector3f(-mvGuiSetOffset.x, -mvGuiSetOffset.y, 0);

    mpGuiSet->SetVirtualSize(mvGuiSetSize, -1000, 1000, mvGuiSetOffset);
    mpGuiSet->SetActive(false);

    ///////////////////////////////
    //Create Viewport
    mpViewport = mpScene->CreateViewport();
    mpViewport->SetActive(false);
    mpViewport->SetVisible(false);

    mpViewport->AddGuiSet(mpGuiSet);

    ///////////////////////////////
    //Load graphics
    mpWhiteGfx = mpGui->CreateGfxFilledRect(cColor(1, 1), eGuiMaterial_Alpha);
    mpCurrentImage = NULL;

    ///////////////////////////////
    //Load config
    cConfigFile *pMenuCfg = gpBase->mpMenuCfg;

    mpFontDefault = NULL;

    mvLoadingFontSize = gpBase->mpMenuCfg->GetVector2f("LoadScreen", "LoadingFontSize", 0);
    mvTextFontSize = gpBase->mpMenuCfg->GetVector2f("LoadScreen", "TextFontSize", 0);
    mfLoadingY = gpBase->mpMenuCfg->GetFloat("LoadScreen", "LoadingY", 0);
    mfTextWithImageY = gpBase->mpMenuCfg->GetFloat("LoadScreen", "TextWithImageY", 0);
    mfTextAloneY = gpBase->mpMenuCfg->GetFloat("LoadScreen", "TextAloneY", 0);
    mfImageY = gpBase->mpMenuCfg->GetFloat("LoadScreen", "ImageY", 0);
    mfTextMaxWidth = gpBase->mpMenuCfg->GetFloat("LoadScreen", "TextMaxWidth", 0);

    mfFadeOutTime = gpBase->mpMenuCfg->GetFloat("LoadScreen", "FadeOutTime", 0);
    mfTextDurationMul = gpBase->mpMenuCfg->GetFloat("LoadScreen", "TextDurationMul", 0);

    mpLoadingTextColor     = gpBase->mpMenuCfg->GetColor("LoadScreen", "LoadingTextColor",     cColor(255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f));
    mpBaseLoadingTextColor = gpBase->mpMenuCfg->GetColor("LoadScreen", "BaseLoadingTextColor", cColor(255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f));
    mpGameLoadingTextColor = gpBase->mpMenuCfg->GetColor("LoadScreen", "GameLoadingTextColor", cColor(255.0f / 255.0f,  0.0f  / 255.0f,  0.0f  / 255.0f));

    ///////////////////////////////
    //Progress bar
    mbBarEnabled = gpBase->mpMenuCfg->GetBool("LoadScreen", "BarEnabled", false);
    mfBarWidth = gpBase->mpMenuCfg->GetFloat("LoadScreen", "BarWidth", 500.0f);
    mfBarHeight = gpBase->mpMenuCfg->GetFloat("LoadScreen", "BarHeight", 10.0f);
    mfBarGap = gpBase->mpMenuCfg->GetFloat("LoadScreen", "BarGap", 12.0f);
    mBarBackColor = gpBase->mpMenuCfg->GetColor("LoadScreen", "BarBackColor", cColor(0.118f, 0.098f, 0.075f));
    mBarFillColor = gpBase->mpMenuCfg->GetColor("LoadScreen", "BarFillColor", cColor(0.51f, 0.427f, 0.318f));

    mpMenuImage = NULL;
    mbLoadProgressActive = false;
    mbLoadProgressMenu = false;
    mfBarTarget = 0;
    mfBarShown = 0;
    mlLastBarDrawTime = 0;

    ///////////////////////////////
    //Get progress from the world loader
    cWorldLoaderHplMap::SetProgressCallback(WorldLoadProgressCallback, this);
}

//-----------------------------------------------------------------------

cLuxLoadScreenHandler::~cLuxLoadScreenHandler()
{
    cWorldLoaderHplMap::SetProgressCallback(NULL, NULL);
    DestroyMenuImage();
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxLoadScreenHandler::LoadFonts()
{
    tString sFontFile = gpBase->mpMenuCfg->GetString("LoadScreen", "DefaultFont", "");
    mpFontDefault = LoadFont(sFontFile);
}

//-----------------------------------------------------------------------

void cLuxLoadScreenHandler::OnStart()
{

}

//-----------------------------------------------------------------------

void cLuxLoadScreenHandler::Reset()
{
    msLoadTextCat = "";
    msLoadTextEntry = "";
    msLoadTextImage = "";
    mlTextRandomNum = 0;
    mlPrevTextNumBitFlags = 0;

    mfLoadingAlpha = 1.0f;
    mfAlpha = 0;
    mfExtraTime = 0;
    mState = eLuxLoadScreenState_LastEnum;

    msCurrentGameScreenTextEntry = "";

    msGameScreenEndSound = "";

    msCurrentGameScreenImage = "";

    if(mpCurrentImage)
    {
        mpGui->DestroyGfx(mpCurrentImage);
        mpCurrentImage = NULL;
        msCurrentImage = "";
    }
}

//-----------------------------------------------------------------------

void cLuxLoadScreenHandler::Update(double adFixedDelta)
{
    switch(mState)
    {
    case eLuxLoadScreenState_Game:
        UpdateGameState(adFixedDelta);
        break;
    }
}

//-----------------------------------------------------------------------

void cLuxLoadScreenHandler::OnEnterContainer(const tString& asOldContainer)
{
    gpBase->mpInputHandler->ChangeState(eLuxInputState_LoadScreen);

    mpViewport->SetActive(true);
    mpViewport->SetVisible(true);
    mpGuiSet->SetActive(true);
}

void cLuxLoadScreenHandler::OnLeaveContainer(const tString& asNewContainer)
{
    mpViewport->SetActive(false);
    mpViewport->SetVisible(false);
    mpGuiSet->SetActive(false);
}

//-----------------------------------------------------------------------

void cLuxLoadScreenHandler::OnDraw(double adFrameTime)
{
    switch(mState)
    {
    case eLuxLoadScreenState_Game:
        DrawGameState(adFrameTime);
        break;
    }
}

//-----------------------------------------------------------------------

void cLuxLoadScreenHandler::GameScreenLoadDone(const tString& asEndSound, float afLoadTime)
{
    /////////////////////////////
    // Init variables
    mfAlpha = 1;
    mfExtraTime = 0;
    mfLoadingAlpha = 1.0f;
    mState = eLuxLoadScreenState_Game;
    msGameScreenEndSound = asEndSound;

    EndLoadProgress();

    /////////////////////////////
    // Calculate extra time
    if(msCurrentGameScreenTextEntry != "")
    {
        tWString sLoadText = kTranslate(msLoadTextCat, msCurrentGameScreenTextEntry);
        float fTimeWanted = gpBase->mpHelpFuncs->GetStringDuration(sLoadText) * mfTextDurationMul;
        if(fTimeWanted > afLoadTime)
        {
            mfExtraTime = fTimeWanted - afLoadTime;
        }

        //Log("TimeTaken: %f TimeWanted: %f\n", afLoadTime,fTimeWanted);
    }


    /////////////////////////////
    // Change State
    gpBase->mpEngine->GetUpdater()->SetContainer("LoadScreen");

}

//-----------------------------------------------------------------------

void cLuxLoadScreenHandler::SetupLoadText(const tString& asCat, const tString& asEntry, int alRandomNum, const tString& asImage)
{
    if(msLoadTextCat == asCat && msLoadTextEntry == asEntry && msLoadTextImage == asImage && mlTextRandomNum == alRandomNum)
    {
        return;
    }

    msLoadTextCat = asCat;
    msLoadTextEntry = asEntry;
    msLoadTextImage = asImage;
    mlTextRandomNum = alRandomNum;
    mlPrevTextNumBitFlags = 0;
}

//-----------------------------------------------------------------------

void cLuxLoadScreenHandler::DrawGameScreen()
{
    msCurrentGameScreenTextEntry = GetGameScreenTextEntry();
    msCurrentGameScreenImage = msLoadTextImage;

    mfLoadingAlpha = 1.0f;

    mbLoadProgressActive = mbBarEnabled;
    mbLoadProgressMenu = false;
    mfBarTarget = 0;
    mfBarShown = 0;
    mlLastBarDrawTime = cPlatform::GetApplicationTime();

    DrawGameScreen(gpBase->mpHelpFuncs->GetSet(), mbLoadProgressActive);

    gpBase->mpHelpFuncs->DrawSetToScreen();

}

//-----------------------------------------------------------------------

void cLuxLoadScreenHandler::DrawMenuScreen(bool abShowProgressBar)
{
    cGuiSet *pSet = gpBase->mpHelpFuncs->GetSet();

    if(mpMenuImage == NULL)
    {
        mpMenuImage = mpGui->CreateGfxTexture("menu_loading_screen.jpg", eGuiMaterial_Alpha, eTextureType_Rect);
    }

    //////////////////////
    //Start/stop progress bar
    mbLoadProgressActive = abShowProgressBar && mbBarEnabled;
    mbLoadProgressMenu = true;
    mfBarTarget = 0;
    mfBarShown = 0;
    mlLastBarDrawTime = cPlatform::GetApplicationTime();

    //////////////////////
    //Draw
    DrawMenuScreenContents(pSet, mbLoadProgressActive);
    gpBase->mpHelpFuncs->DrawSetToScreen();

    if(abShowProgressBar == false)
    {
        DestroyMenuImage();
    }
}

//-----------------------------------------------------------------------

void cLuxLoadScreenHandler::DrawMenuScreenContents(cGuiSet *apSet, bool abDrawProgressBar)
{
    tWString sLoading = kTranslate("General", "Loading");

    //////////////////////
    //Draw Image
    if(mpMenuImage)
    {
        cVector2f vImageSize = mpMenuImage->GetImageSize();
        float fRatio = vImageSize.x / vImageSize.y;
        float fRatioMul = fRatio / (4.0f / 3.0f);

        apSet->DrawGfx(mpMenuImage, cVector3f(400 * (1 - fRatioMul), 0, 0), cVector2f(800 * fRatioMul, 600), cColor(1, 1));
    }

    //////////////////////
    //Draw Text
    cVector2f vSize(20);
    cVector3f vPos(400, 530, 1);
    apSet->DrawFont(sLoading, mpFontDefault, vPos, vSize, cColor(mpBaseLoadingTextColor.r, mpBaseLoadingTextColor.g, mpBaseLoadingTextColor.b, 1.0f), eFontAlign_Center);

    //////////////////////
    // Draw Progress Bar
    if(abDrawProgressBar)
    {
        DrawProgressBar(apSet, vPos.y - mfBarGap - mfBarHeight, 1.0f);
    }
}

//-----------------------------------------------------------------------

void cLuxLoadScreenHandler::DestroyMenuImage()
{
    if(mpMenuImage)
    {
        mpGui->DestroyGfx(mpMenuImage);
        mpMenuImage = NULL;
    }
}

//-----------------------------------------------------------------------

void cLuxLoadScreenHandler::DrawProgressBar(cGuiSet *apSet, float afY, float afAlpha)
{
    if(mbBarEnabled == false)
    {
        return;
    }

    float fX = 400 - mfBarWidth * 0.5f;

    // Background
    apSet->DrawGfx(mpWhiteGfx, cVector3f(fX, afY, 1.5f), cVector2f(mfBarWidth, mfBarHeight),
                   cColor(mBarBackColor.r, mBarBackColor.g, mBarBackColor.b, 0.8f * afAlpha));

    // Fill
    float fFillWidth = mfBarWidth * cMath::Clamp(mfBarShown, 0, 1);
    if(fFillWidth > 0)
    {
        apSet->DrawGfx(mpWhiteGfx, cVector3f(fX, afY, 1.6f), cVector2f(fFillWidth, mfBarHeight),
                       cColor(mBarFillColor.r, mBarFillColor.g, mBarFillColor.b, afAlpha));
    }
}

//-----------------------------------------------------------------------

void cLuxLoadScreenHandler::SetLoadProgress(float afProgress, bool abForceRedraw)
{
    if(mbLoadProgressActive == false || mbBarEnabled == false)
    {
        return;
    }

    // Should move forward
    afProgress = cMath::Clamp(afProgress, 0, 1);
    if(afProgress > mfBarTarget)
    {
        mfBarTarget = afProgress;
    }

    // 60 Hz redraw, unsure if this is too fast...
    unsigned long lNow = cPlatform::GetApplicationTime();
    float fDeltaTime = (float)(lNow - mlLastBarDrawTime) / 1000.0f;
    if(abForceRedraw == false && fDeltaTime < 1.0f / 60.0f)
    {
        return;
    }
    mlLastBarDrawTime = lNow;
    if(fDeltaTime > 0.25f)
    {
        fDeltaTime = 0.25f;
    }

    mfBarShown += (mfBarTarget - mfBarShown) * (1.0f - expf(-fDeltaTime * 8.0f));

    RedrawLoadScreen();
}

//-----------------------------------------------------------------------

void cLuxLoadScreenHandler::RedrawLoadScreen()
{
    if(mbLoadProgressMenu)
    {
        DrawMenuScreenContents(gpBase->mpHelpFuncs->GetSet(), true);
    }
    else
    {
        DrawGameScreen(gpBase->mpHelpFuncs->GetSet(), true);
    }

    gpBase->mpHelpFuncs->DrawSetToScreen();
}

//-----------------------------------------------------------------------

void cLuxLoadScreenHandler::OnMapFileLoaded()
{
    if(mbLoadProgressActive == false || mbLoadProgressMenu == false)
    {
        return;
    }

    mfBarTarget = 1;
    mfBarShown = 1;
    RedrawLoadScreen();

    EndLoadProgress();
}

//-----------------------------------------------------------------------

void cLuxLoadScreenHandler::EndLoadProgress()
{
    mbLoadProgressActive = false;
    mfBarTarget = 1;

    if(mbLoadProgressMenu)
    {
        DestroyMenuImage();
    }
}

//-----------------------------------------------------------------------

void cLuxLoadScreenHandler::DrawBlankScreen()
{

}

//-----------------------------------------------------------------------

void cLuxLoadScreenHandler::ExitPressed()
{
    if(mState == eLuxLoadScreenState_Game)
    {
        mfExtraTime = 0;
    }
    //Exit();
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cLuxLoadScreenHandler::Exit()
{
    if(mpCurrentImage)
    {
        mpGui->DestroyGfx(mpCurrentImage);
        mpCurrentImage = NULL;
        msCurrentImage = "";
    }

    gpBase->mpEngine->GetUpdater()->SetContainer("Default");
    gpBase->mpInputHandler->ChangeState(eLuxInputState_Game);

    if(mState == eLuxLoadScreenState_Game)
    {
        gpBase->mpHelpFuncs->PlayGuiSoundData(msGameScreenEndSound, eSoundEntryType_Gui);
    }
}

//-----------------------------------------------------------------------

tString cLuxLoadScreenHandler::GetGameScreenTextEntry()
{
    if(msLoadTextCat == "" || msLoadTextEntry == "")
    {
        return "";
    }

    tString sEntry = msLoadTextEntry;
    if(mlTextRandomNum > 1)
    {
        int lNum = -1;
        int lIdx = cMath::RandRectl(0, mlTextRandomNum - 1);
        for(int i = 0; i < mlTextRandomNum; ++i)
        {
            if((cMath::GetFlagBit(lIdx) & mlPrevTextNumBitFlags) == 0)
            {
                lNum = lIdx;
                break;
            }
            ++lIdx;
            if(lIdx >= mlTextRandomNum)
            {
                lIdx = 0;
            }
        }

        //All area taken, just take random.
        if(lNum < 0)
        {
            mlPrevTextNumBitFlags = 0;
            lNum = cMath::RandRectl(0, mlTextRandomNum - 1);
        }

        mlPrevTextNumBitFlags |= cMath::GetFlagBit(lNum);

        if(lNum < 10)
        {
            sEntry += '0' + cString::ToString(lNum + 1);
        }
        else
        {
            sEntry += cString::ToString(lNum + 1);
        }
    }
    return sEntry;
}

//-----------------------------------------------------------------------

void cLuxLoadScreenHandler::LoadCurrentImage(const tString &asImage)
{
    if(mpCurrentImage && msCurrentImage == asImage)
    {
        return;
    }

    msCurrentImage = asImage;
    mpCurrentImage = mpGui->CreateGfxTexture(asImage, eGuiMaterial_Alpha, eTextureType_Rect);
}

//-----------------------------------------------------------------------


void cLuxLoadScreenHandler::UpdateGameState(double adFixedDelta)
{
    if(mfBarShown < 1.0f)
    {
        mfBarShown += (float)adFixedDelta * 3.0f;
        if(mfBarShown > 1.0f)
        {
            mfBarShown = 1.0f;
        }
    }

    if(mfLoadingAlpha > 0)
    {
        mfLoadingAlpha -= (float)adFixedDelta * 0.33f;
        if(mfLoadingAlpha < 0)
        {
            mfLoadingAlpha = 0;
        }
    }

    if(mfExtraTime > 0)
    {
        mfExtraTime -= (float)adFixedDelta;
        return;
    }

    mfAlpha -= (float)adFixedDelta * (1.0f / mfFadeOutTime);
    if(mfAlpha < 0)
    {
        mfAlpha = 0;
        Exit();
    }
}

//-----------------------------------------------------------------------

void cLuxLoadScreenHandler::DrawGameState(double adFrameTime)
{
    mpGuiSet->DrawGfx(mpWhiteGfx, cVector3f(mvGuiSetStartPos.x, mvGuiSetStartPos.y, -1), mvGuiSetSize, cColor(0, 1));

    DrawGameScreen(mpGuiSet, msCurrentGameScreenTextEntry != "");

    mpGuiSet->DrawGfx(mpWhiteGfx, cVector3f(mvGuiSetStartPos.x, mvGuiSetStartPos.y, 40), mvGuiSetSize, cColor(0, 1.0f-mfAlpha));
}

//-----------------------------------------------------------------------

void cLuxLoadScreenHandler::DrawGameScreen(cGuiSet *apSet, bool abDrawProgressBar)
{
    //////////////////////
    //Load Image
    LoadCurrentImage(msCurrentGameScreenImage);

    //////////////////////
    //Draw Image
    if(mpCurrentImage)
    {
        cVector3f vPos(0, mfImageY, 0.5f);
        vPos.x = 400 - mpCurrentImage->GetImageSize().x / 2.0f;

        apSet->DrawGfx(mpCurrentImage, vPos, -1, cColor(1, 1, 1, mfLoadingAlpha));
    }

    //////////////////////
    //Draw Text
    if(msCurrentGameScreenTextEntry != "")
    {
        ///////////////////
        //Text
        tWStringVec vTextRows;
        mpFontDefault->GetWordWrapRows(mfTextMaxWidth, mvTextFontSize.y, mvTextFontSize, kTranslate(msLoadTextCat, msCurrentGameScreenTextEntry), &vTextRows);

        float fT = cMath::SmoothCurve(mfLoadingAlpha);
        float fY = mfTextWithImageY * fT + mfTextAloneY * (1.0f-fT);
        cVector3f vTextPos(400 - mfTextMaxWidth / 2, fY, 1);
        for(size_t i = 0; i < vTextRows.size(); ++i)
        {
            apSet->DrawFont(vTextRows[i], mpFontDefault, vTextPos, mvTextFontSize,
                            cColor(mpLoadingTextColor.r, mpLoadingTextColor.g, mpLoadingTextColor.b, 1.0f));

            vTextPos.y += mvTextFontSize.y + 2;
        }

        ///////////////////
        //Loading
        tWString sLoading = kTranslate("General", "Loading");
        cVector3f vPos(400, mfLoadingY, 1);

        apSet->DrawFont(sLoading, mpFontDefault, vPos, mvLoadingFontSize,
                        cColor(mpGameLoadingTextColor.r, mpGameLoadingTextColor.g, mpGameLoadingTextColor.b, mfLoadingAlpha),
                        eFontAlign_Center);

        ///////////////////
        //Progress Bar
        if(abDrawProgressBar)
        {
            DrawProgressBar(apSet, mfLoadingY - mfBarGap - mfBarHeight, mfLoadingAlpha);
        }
    }
    //////////////////////
    //Draw loading only
    else
    {
        ///////////////////
        //Loading
        tWString sLoading = kTranslate("General", "Loading");
        cVector3f vPos(400, 300 - mvLoadingFontSize.y / 2, 1);

        apSet->DrawFont(sLoading, mpFontDefault, vPos, mvLoadingFontSize,
                        cColor(mpBaseLoadingTextColor.r, mpBaseLoadingTextColor.g, mpBaseLoadingTextColor.b, 1.0f),
                        eFontAlign_Center);

        ///////////////////
        //Progress Bar
        if(abDrawProgressBar)
        {
            DrawProgressBar(apSet, vPos.y - mfBarGap - mfBarHeight, 1.0f);
        }
    }
}

//-----------------------------------------------------------------------
