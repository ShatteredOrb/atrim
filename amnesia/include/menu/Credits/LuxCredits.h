#ifndef LUX_CREDITS_H
#define LUX_CREDITS_H

//----------------------------------------------

#include "LuxBase.h"

//----------------------------------------------

class cLuxCredits : public iLuxUpdateable
{
public:
    cLuxCredits();
    ~cLuxCredits();

    void LoadFonts();

    void Reset();

    void Update(double adFixedDelta);

    void AddKeyPart(int alKey);
    void Setup(const tString& asMusic, bool abLoopMusic, const tString& asTextCat, const tString& asTextEntry, int alEndNum);

    void OnEnterContainer(const tString& asOldContainer);
    void OnLeaveContainer(const tString& asNewContainer);

    void OnDraw(double adFrameTime);

    void ExitPressed();

    void AppLostInputFocus();
    void AppGotInputFocus();

private:
    ///////////////////////
    // Data
    cGraphics *mpGraphics;

    cGui *mpGui;
    cGuiSet *mpGuiSet;

    iFontData *mpFontNormal;
    iFontData *mpFontHeader;

    cViewport *mpViewport;

    cGuiGfxElement *mpBlackGfx;

    cVector2f mvScreenSize;
    cVector2f mvGuiSetSize;
    cVector2f mvGuiSetCenterSize;
    cVector2f mvGuiSetOffset;
    cVector3f mvGuiSetStartPos;

    cVector2f mvNormalFontSize;
    cVector2f mvHeaderFontSize;
    cVector2f mvTheEndFontSize;

    float mfScrollSpeed;
    float mfFadeSpeed;

    ///////////////////////
    // Vars
    cBinaryBuffer mKeyBuffer;
    tString msMusic;
    bool mbLoopMusic;
    int mlEndNum;

    tWStringVec mvTextRows;

    bool mbActive;
    cInterpolatedFloat mfYPos;
    int mlState;
    float mfTime;
    cInterpolatedFloat mfFadeAlpha;

};

//----------------------------------------------


#endif // LUX_CREDITS_H
