#ifndef LUX_MESSAGE_HANDLER_H
#define LUX_MESSAGE_HANDLER_H

//----------------------------------------------

#include "LuxBase.h"

//----------------------------------------


class cLuxMessageHandler : public iLuxUpdateable
{
    friend class cLuxMusicHandler_SaveData;
public:
    cLuxMessageHandler();
    ~cLuxMessageHandler();

    void LoadFonts();
    void OnStart();
    void Update(double adFixedDelta);
    void Reset();

    void LoadUserConfig();
    void SaveUserConfig();

    void OnMapEnter(cLuxMap *apMap);
    void OnMapLeave(cLuxMap *apMap);

    void StarQuestAddedMessage();

    void StartPauseMessage(const tWString& asText, bool abYesNo, iLuxMessageCallback *apCallback);

    /**
    * if time is <=0 then the life time is calculated based on string length.
    */
    void SetMessage(const tWString& asText, float afTime);
    bool IsMessageActive()
    {
        return mfMessageTime > 0;
    }

    void OnDraw(double adFrameTime);

    void DoAction(eLuxPlayerAction aAction, bool abPressed);

    bool IsPauseMessageActive()
    {
        return mbPauseMessageActive;
    }
    void SetPauseMessageActive(bool abX);

    bool ShowSubtitles()
    {
        return mbShowSubtitles;
    }
    void SetShowSubtitles(bool abX)
    {
        mbShowSubtitles = abX;
    }

    bool ShowEffectSubtitles()
    {
        return mbShowEffectSubtitles;
    }
    void SetShowEffectSubtitles(bool abX)
    {
        mbShowEffectSubtitles = abX;
    }
private:
    void DrawQuestAdded();
    void DrawMessage();
    void DrawPauseMessage();


    //////////////////
    // Data
    cGuiGfxElement *mpBlackGfx;
    iFontData *mpFont;

    cGuiGfxElement *mpQuestAddedIcon;
    tString msQuestAddedSound;

    cVector2f mvFontSize;

    //////////////////
    // Variables
    bool mbShowSubtitles;
    bool mbShowEffectSubtitles;

    bool mbPauseMessageActive;
    cInterpolatedFloat mfPauseMessageAlpha;

    cInterpolatedFloat mfMessageAlpha;
    float mfMessageTime;

    bool mbQuestMessageActive;
    cInterpolatedFloat mfQuestMessageAlpha;
    float mfQuestMessageTime;

    cLinearOscillation mQuestOscill;

    tWStringVec mvMessageRows;

    tWStringVec mvLines;
    bool mbMessageYesNo;
    iLuxMessageCallback *mpCallback;
};

//----------------------------------------------


#endif // LUX_MESSAGE_HANDLER_H
