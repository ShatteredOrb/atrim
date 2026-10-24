#ifndef LUX_BASE_H
#define LUX_BASE_H

//----------------------------------------------

#include "StdAfx.h"

#include "LuxTypes.h"

//----------------------------------------------

class cLuxMapHandler;
class cLuxMapHelper;
class cLuxInputHandler;

class cLuxEffectHandler;

class cLuxDebugHandler;
class cLuxSaveHandler;
class cLuxScriptHandler;
class cLuxHelpFuncs;
class cLuxEffectRenderer;
class cLuxMusicHandler;
class cLuxMessageHandler;
class cLuxJournal;
class cLuxCompletionCountHandler;
class cLuxGlobalDataHandler;
class cLuxHintHandler;
class cLuxConfigHandler;
class cLuxPostEffectHandler;
class cLuxInsanityHandler;
class cLuxProgressLogHandler;
class cLuxLoadScreenHandler;

class cLuxInventory;

class cLuxCredits;

class cLuxPreMenu;

class cLuxMainMenu;

class cLuxPlayer;


//----------------------------------------------

extern eLuxAxis StringToAxis(const tString &asAxis);

/**
* Calculates an offset and size based on virtual size that keeps the ratio of the size. Note that the outsize is the size of the ENTIRE screen!
*/
extern void LuxCalcGuiSetOffset(const cVector2f &avVirtualSizeIn, const cVector2f& avScreenSize, cVector2f& avOutSize, cVector2f & avOutOffset);

/**
* Same as CalcGuiSetOffset, but assumes avScreenSize is fromm screen.
*/
extern void LuxCalcGuiSetScreenOffset(const cVector2f &avVirtualSizeIn, cVector2f& avOutSize, cVector2f & avOutOffset);


extern void ProgLog(eLuxProgressLogLevel aLevel, const tString& asMessage);

//----------------------------------------------

/**
 * LuxCustomStorySettings - class used to store custom made story data
 *
 */
class cLuxCustomStorySettings
{
public:
    cLuxCustomStorySettings();
    cLuxCustomStorySettings(cLuxCustomStorySettings* apStory);
    ~cLuxCustomStorySettings();

    bool CreateFromPath(const tWString& asPath);

    /**
     * Creates a copy of the current object and "feeds" it to cLuxBase
     * (this because these objects are created and destroyed in the Custom Story List window)
     */
    void SetActive();

    /**
     * Starts a new game using the custom settings.
     * NOTE: Should be called from a copied object, NEVER from one in the Custom Story List window!!
     */
    bool StartGame();

    tWString msName;
    tWString msAuthor;
    tString msImgFile;

    tWString msStoryRootFolder;
    tString msStartMap;
    tString msStartPos;
    tString msMapsFolder;

    tString msExtraLangFilePrefix;
    tString msDefaultExtraLanguage;

    //Full conversion (1.5 style): InitCfgFile in custom_story_settings.cfg, relative to the
    //story root. Empty for normal custom stories.
    tString msInitCfgFile;
    bool IsFullConversion()
    {
        return msInitCfgFile != "";
    }
};

typedef std::vector<cLuxCustomStorySettings *> tCustomStorySettingsVec;

//----------------------------------------------

class cLuxBase
{
public:
    cLuxBase();
    ~cLuxBase();

    bool Init(const tString &asCommandline);
    void Exit();

    void Run();

    void Reset();

    void RunModuleMessage(eLuxUpdateableMessage aMessage, void * apData = NULL);

    bool StartGame(const tString& asFile, const tString& asFolder, const tString& asStartPos);
    bool StartCustomStory();

    /////////////////////////
    // Full conversion switching
    // A full conversion started from the custom story menu runs in its own process: atrim
    // exits and starts itself again with "-fc <story root>". Leaving the full conversion
    // (quit from its menus) starts the base game again.

    /** Loads resources.cfg (+ the base game's one as fallback inside a full conversion). */
    void LoadResourceDirs();

    /** Exits and restarts atrim as the full conversion in asStoryRootFolder. */
    void LaunchFullConversion(const tWString& asStoryRootFolder);
    /** Quit from a menu: inside a full conversion, go back to the base game instead. */
    void ExitFromMenu();

    bool IsFullConversionMode()
    {
        return mbFullConversionMode;
    }
    /** Root folder of the running full conversion (with trailing slash), empty if none. */
    const tWString &GetFullConversionRoot()
    {
        return msFullConversionRoot;
    }
    /** Alt path for resource / language dirs (the full conversion root or empty). */
    tWString GetResourceAltPath()
    {
        return mbFullConversionMode ? msFullConversionRoot : tWString(_W(""));
    }

    bool mbRelaunchAtExit;
    tString msRelaunchCommandLine;

    void SaveConfig();

    bool InitUserConfig();

    bool CreateProfile(const tWString& asName);
    void SetProfile(const tWString& asName);

    void PreloadSound(const tString &asFile);
    void PreloadParticleSystem(const tString &asFile);

    void SetCustomStory(cLuxCustomStorySettings* apCustomStory);

//private:
    bool ParseCommandLine(const tString &asCommandline);

    bool InitApp();
    bool CheckFeatureSupport();

    bool InitMainConfig();
    void ExitConfig();

    cConfigFile *LoadConfigFile(const tWString& asDefaultPath, const tWString& asWantedPath, bool abForceLoadDefault = false, bool *abDidLoadDefault = NULL);

    bool InitEngine();
    void ExitEngine();

    bool InitGame();
    void ExitGame();

    void InitOver();

    void DrawVersionWatermark();


    bool LoadLanguage(const tString& asName, bool abForceReload = false);

    iLuxUpdateable *AddModule(iLuxUpdateable *apModule, const tString& asContainer);
    iLuxUpdateable *AddGlobalModule(iLuxUpdateable *apModule);


    void RaiseCrashFlag();
    void LowerCrashFlag();
    bool CheckCrashFlag();

    void RaiseFirstStartFlag();
    void LowerFirstStartFlag();
    bool CheckFirstStartFlag();

    /////////////////////////
    // Public variables
public:
    cEngine *mpEngine;

    cConfigFile *mpMainConfig;
    cConfigFile *mpUserConfig;
    cConfigFile *mpUserKeyConfig;

    cConfigFile *mpGameCfg;
    cConfigFile *mpMenuCfg;
    cGuiSet *mpGameDebugSet;
    cGuiSet *mpGameHudSet;
    iFontData *mpDefaultFont;
    cVector2f mvHudVirtualCenterSize;//This is size of what is inside a 4:3 ratio!
    cVector2f mvHudVirtualSize;
    cVector2f mvHudVirtualOffset;
    cVector3f mvHudVirtualStartPos;

    cLuxCustomStorySettings *mpCustomStory;

    cLuxConfigHandler *mpConfigHandler;
    cLuxInputHandler *mpInputHandler;
    cLuxEffectHandler *mpEffectHandler;
    cLuxMapHandler *mpMapHandler;
    cLuxMapHelper *mpMapHelper;
    cLuxDebugHandler *mpDebugHandler;
    cLuxSaveHandler *mpSaveHandler;
    cLuxScriptHandler *mpScriptHandler;
    cLuxHelpFuncs *mpHelpFuncs;
    cLuxEffectRenderer *mpEffectRenderer;
    cLuxInventory *mpInventory;
    cLuxPreMenu *mpPreMenu;
    cLuxMainMenu *mpMainMenu;
    cLuxPlayer *mpPlayer;
    cLuxMusicHandler *mpMusicHandler;
    cLuxMessageHandler *mpMessageHandler;
    cLuxJournal *mpJournal;
    cLuxCompletionCountHandler *mpCompletionCountHandler;
    cLuxGlobalDataHandler *mpGlobalDataHandler;
    cLuxHintHandler *mpHintHandler;
    cLuxPostEffectHandler *mpPostEffectHandler;
    cLuxInsanityHandler *mpInsanityHandler;
    cLuxProgressLogHandler *mpProgressLogHandler;
    cLuxLoadScreenHandler *mpLoadScreenHandler;
    cLuxCredits *mpCredits;

    tString msGameName;
    tWString msErrorMessage;

    bool mbShowPreMenu;
    bool mbShowMenu;

    bool mbShowVersionInfo;
    cGuiSet *mpVersionWatermarkSet;
    tWString msVersionWatermarkText;

    tString msCommandLineMapFile;

    tString msStartMapFile;
    tString msStartMapFolder;
    tString msStartMapPos;

    tWString msDefaultProfileName;

    tWString msBaseSavePath;
    tWString msProfileSavePath;
    tWString msMainProfileSavePath;
    tWString msProfileName;

    bool mbExitMenuDirectly;

    bool mbPTestActivated;

    cLuxMap *mpCurrentMapLoading;

    /////////////////////////
    // HARDMODE
    bool mbHardMode;
    bool mbAllowHardMode;

    /////////////////////////
    // Kinda private variables
    tWString msDefaultInitConfigFile;
    tWString msInitConfigFile;

    tWString msDefaultMainConfigPath;
    tWString msDefaultUserConfigPath;
    tWString msDefaultUserKeyConfigPath;
    tString msResourceConfigPath;
    tString msMaterialConfigPath;

    tString msDefaultPatchLanguage;
    tString msDefaultBaseLanguage;
    tString msDefaultGameLanguage;

    tString msPatchLanguageFolder;
    tString msBaseLanguageFolder;
    tString msGameLanguageFolder;

    tString msCustomStoryPath;

    tString msCurrentLanguage;

    tWString msMainSaveFolder;
    tWString msGameConfigPath;
    tWString msMenuConfigPath;
    tWString msPreMenuConfigPath;

    tWString msCrashFlagPath;
    tWString msFirstStartFlagPath;

    bool mbSaveConfigAtExit;

    bool mbFullConversionMode;
    tWString msFullConversionRoot;
    tString msBaseResourceConfigPath;

private:
    bool ParseFullConversionCommandLine(const tString& asArgs);
    tString ResolveFullConversionPath(const tString& asPath);
    tWString ResolveFullConversionPathW(const tWString& asPath);
    void CopyBaseMainSettingsToFullConversion();
public:

    std::vector<iLuxUpdateable *> mvModules;

};

//----------------------------------------------

extern cLuxBase *gpBase;

//----------------------------------------------


#endif // LUX_BASE_H
