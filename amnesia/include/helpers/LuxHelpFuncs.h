#ifndef LUX_HELP_FUNCS_H
#define LUX_HELP_FUNCS_H

//----------------------------------------------

#include "LuxBase.h"

//----------------------------------------------

class cLuxSoundExtraData
{
public:
    float mfMinDistance;
    float mfMaxDistance;
    float mfVolume;
    float mfAIVolume;
    cSoundEntry *mpSoundEntry;
};

//-------------------------------------

class cLuxModelCache
{
public:
    cLuxModelCache();
    ~cLuxModelCache();

    void Create();
    void Destroy();

private:
    std::list<cMesh *> mlstMeshCache;
    std::list<cAnimation *> mlstAnimationCache;
    std::list<iTexture *> mlstTextureCache;
};

//-------------------------------------

class cLuxHelpFuncs : public iLuxUpdateable
{
public:
    cLuxHelpFuncs();
    ~cLuxHelpFuncs();

    void LoadFonts();


    bool PlayGuiSoundData(    const tString& asName, eSoundEntryType aDestType, float afVolMul = 1.0f,
                              eSoundEntityType aSoundType = eSoundEntityType_Main, bool abSkipPreviousRandom = true,
                              cLuxSoundExtraData *apOutputData = NULL);

    void DrawSetToScreen(bool abClearScreen = true, const cColor& aCol = cColor(0, 0), cGuiSet* apSet = NULL);

    void CleanupData();

    tWString ParseString(const tWString& asInput);

    float GetStringDuration(const tWString& asStr);

    /**
     * To be used in journal and inventory, renders the screen without icons and such.
     */
    void RenderBackgroundScreen(bool abDrawFullHUD);


    cGuiSet *GetSet()
    {
        return mpSet;
    }
private:
    tWString ParseStringCommand(const tWString& asCommand);

    cGuiSet *mpSet;
    iLowLevelGraphics *mpLowLevelGfx;

    iFontData *mpFontDefault;

    float mfTextDuration_StartTime;
    float mfTextDuration_MinTime;
    float mfTextDuration_CharTime;
};

//----------------------------------------------


#endif // LUX_HELP_FUNCS_H
