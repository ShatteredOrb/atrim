#include <cmath>
#include <algorithm>
#include <numeric>
#include <queue>
#include <vector>

#include "engine/Engine.h"
#include "scene/RenderInterpolation.h"

#include "system/System.h"
#include "physics/Physics.h"
#include "ai/AI.h"
#include "resources/Resources.h"
#include "graphics/Graphics.h"
#include "gui/Gui.h"
#include "scene/Scene.h"

#include "system/String.h"
#include "system/Platform.h"
#include "system/Timer.h"
#include "system/Mutex.h"

#include "input/Input.h"
#include "input/Mouse.h"

#include "sound/LowLevelSound.h"
#include "sound/Sound.h"

#include "graphics/LowLevelGraphics.h"
#include "graphics/Renderer.h"

#include "engine/Updater.h"
#include "engine/EngineInitVars.h"

#include "system/LowLevelSystem.h"
#include "engine/LowLevelEngineSetup.h"

#include "impl/SDLEngineSetup.h"

namespace hpl
{

//////////////////////////////////////////////////////////////////////////
// FPS COUNTER
//////////////////////////////////////////////////////////////////////////

cFPSCounter::cFPSCounter(iLowLevelSystem* apLowLevelSystem)
{
    mfFPS = 60;

    mlFramecounter = 0;
    mfFrametimestart = 0;
    mfFrametime = 0;

    mfUpdateRate = 1;

    mpLowLevelSystem = apLowLevelSystem;

    mfFrametimestart = (float)cPlatform::GetApplicationTimeX();
}

void cFPSCounter::AddFrame()
{
    mlFramecounter++;

    mfFrametime = (float)cPlatform::GetApplicationTimeX() - mfFrametimestart;

    // update the timer
    if(mfFrametime >= mfUpdateRate)
    {
        mfFPS = ((float)mlFramecounter) / mfFrametime;
        mlFramecounter = 0;

        mfFrametimestart += mfFrametime;
    }
}

//////////////////////////////////////////////////////////////////////////
// SETUP VAR CONTAINER
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cSetupVarContainer::cSetupVarContainer()
{
    msBlank = "";
}

//-----------------------------------------------------------------------


void cSetupVarContainer::AddString(const tString& asName, const tString& asValue)
{
    std::map<tString, tString>::value_type val(asName, asValue);
    m_mapVars.insert(val);
}

void cSetupVarContainer::AddInt(const tString& asName, int alValue)
{
    AddString(asName, cString::ToString(alValue));
}
void cSetupVarContainer::AddFloat(const tString& asName, float afValue)
{
    AddString(asName, cString::ToString(afValue));
}
void cSetupVarContainer::AddBool(const tString& asName, bool abValue)
{
    AddString(asName, abValue ? "true" : "false");
}

//-----------------------------------------------------------------------


const tString &cSetupVarContainer::GetString(const tString& asName)
{
    std::map<tString, tString>::iterator it = m_mapVars.find(asName);
    if(it == m_mapVars.end())
    {
        return msBlank;
    }
    else
    {
        return it->second;
    }
}

float cSetupVarContainer::GetFloat(const tString& asName, float afDefault)
{
    const tString& sVal = GetString(asName);
    if(sVal == "")
    {
        return afDefault;
    }
    else
    {
        return cString::ToFloat(sVal.c_str(), afDefault);
    }
}
int cSetupVarContainer::GetInt(const tString& asName, int alDefault)
{
    const tString& sVal = GetString(asName);
    if(sVal == "")
    {
        return alDefault;
    }
    else
    {
        return cString::ToInt(sVal.c_str(), alDefault);
    }
}
bool cSetupVarContainer::GetBool(const tString& asName, bool abDefault)
{
    const tString& sVal = GetString(asName);
    if(sVal == "")
    {
        return abDefault;
    }
    else
    {
        return cString::ToBool(sVal.c_str(), abDefault);
    }
}

//////////////////////////////////////////////////////////////////////////
// GLOBAL FUCNTIONS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

cEngine *CreateHPLEngine(eHplAPI aApi, tFlag alHplModuleFlags, cEngineInitVars *apVars)
{
    iLowLevelEngineSetup *pGameSetup = NULL;

    switch(aApi)
    {
    case eHplAPI_OpenGL:
        pGameSetup = hplNew(cSDLEngineSetup, (alHplModuleFlags) );
        break;
    }

    return hplNew( cEngine,  (pGameSetup, alHplModuleFlags, apVars) );
}

//-----------------------------------------------------------------------

void DestroyHPLEngine(cEngine* apGame)
{
    hplDelete( apGame );
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cEngine::mbDevicePlugged = false;
bool cEngine::mbDeviceRemoved = false;

//-----------------------------------------------------------------------

cEngine::cEngine(iLowLevelEngineSetup *apGameSetup, tFlag alHplSetupFlags, cEngineInitVars *apVars)
    : mbGameIsDone(false)
    , mbPaused(false)
    , iMaxGameUpdates(6)
    , mbNonGameplayFrame(false)
    , kMaxFrameTime(0.25)
    , kFixedDelta(1.0 / 60.0)
    , iUpdatesOnCurrentFrame(0)
    , dAccumulator(0.0)
    , dLogicTime(0.0)
    , dSpeedMul(1.0)
    , dFrameTime(0.0)
    , dCurrentTime(0.0)
    , dRenderAlpha(0.0)
    , mlFrameRateLimit(0)
    , mdFrameLimitPeriod(0.0)
    , mdNextFrameDeadline(0.0)
{
    //Set up variables
    mbApplicationHasInputFocus = false;
    mbApplicationHasMouseFocus = false;
    mbApplicationIsVisible = false;

    mvEngineTypeStrings.resize(eVariableType_LastEnum);
    mvEngineTypeStrings[eVariableType_Int] =    "Int";
    mvEngineTypeStrings[eVariableType_Float] =    "Float";
    mvEngineTypeStrings[eVariableType_Vec2] =    "Vector2";
    mvEngineTypeStrings[eVariableType_Vec3] =    "Vector3";
    mvEngineTypeStrings[eVariableType_Color] =    "Color";
    mvEngineTypeStrings[eVariableType_String] =    "String";
    mvEngineTypeStrings[eVariableType_Enum] =    "Enum";
    mvEngineTypeStrings[eVariableType_Bool] =    "Bool";

    GameInit(apGameSetup, alHplSetupFlags, apVars);
}

//-----------------------------------------------------------------------

void cEngine::GameInit(iLowLevelEngineSetup *apGameSetup, tFlag alHplSetupFlags, cEngineInitVars *apVars)
{
    mpGameSetup = apGameSetup;

    Log("Creating Engine Modules\n");
    Log("--------------------------------------------------------\n");

    //Create the modules that game connects to and init them!
    Log(" Creating graphics module\n");
    mpGraphics = mpGameSetup->CreateGraphics();

    Log(" Creating system module\n");
    mpSystem = mpGameSetup->CreateSystem();

    Log(" Creating resource module\n");
    mpResources = mpGameSetup->CreateResources(mpGraphics);

    Log(" Creating input module\n");
    mpInput = mpGameSetup->CreateInput(mpGraphics);

    Log(" Creating sound module\n");
    mpSound = mpGameSetup->CreateSound();

    Log(" Creating physics module\n");
    mpPhysics = mpGameSetup->CreatePhysics();

    Log(" Creating ai module\n");
    mpAI = mpGameSetup->CreateAI();

    Log(" Creating gui module\n");
    mpGui = hplNew(cGui, ());

    Log(" Creating scene module\n");
    mpScene = mpGameSetup->CreateScene(mpGraphics, mpResources, mpSound, mpPhysics, mpSystem, mpAI, mpGui);

    Log("--------------------------------------------------------\n\n");


    //Init the resources
    mpResources->Init(mpGraphics, mpSystem, mpSound, mpScene, mpGui, mpPhysics);

    //Init the graphics
    mpGraphics->Init(    apVars->mGraphics.mvScreenSize.x,
                         apVars->mGraphics.mvScreenSize.y,
                         apVars->mGraphics.mlScreenBpp,
                         apVars->mGraphics.mbFullscreen,
                         apVars->mGraphics.mGpuProgramFormat,
                         apVars->mGraphics.msWindowCaption,
                         apVars->mGraphics.mvWindowPosition,
                         mpResources, alHplSetupFlags);

    //Init Sound
    mpSound->Init(mpResources, apVars->mSound.mlSoundDeviceID,
                  apVars->mSound.mbUseEnvironmentalAudio,
                  apVars->mSound.mlMaxChannels,
                  apVars->mSound.mlStreamUpdateFreq,
                  apVars->mSound.mbUseThreading,
                  apVars->mSound.mbUseVoiceManagement,
                  apVars->mSound.mlMaxMonoChannelsHint,
                  apVars->mSound.mlMaxStereoChannelsHint,
                  apVars->mSound.mlStreamBufferSize,
                  apVars->mSound.mlStreamBufferCount,
                  apVars->mSound.mbLowLevelLogging);

    //Init physics
    mpPhysics->Init(mpResources);

    //Init AI
    mpAI->Init();

    //Init Gui
    mpGui->Init(mpResources, mpGraphics, mpSound, mpScene, mpInput);

    Log("Initializing Game Module\n");
    Log("--------------------------------------------------------\n");
    //Create the updatehandler
    Log(" Adding engine updates\n");
    mpUpdater = hplNew( cUpdater, (mpSystem->GetLowLevel()));

    //Add some loaded modules to the updater
    mpUpdater->AddGlobalUpdate(mpInput);
    mpUpdater->AddGlobalUpdate(mpPhysics);
    mpUpdater->AddGlobalUpdate(mpScene);
    mpUpdater->AddGlobalUpdate(mpGraphics);
    mpUpdater->AddGlobalUpdate(mpSound);
    mpUpdater->AddGlobalUpdate(mpAI);
    mpUpdater->AddGlobalUpdate(mpGui);
    mpUpdater->AddGlobalUpdate(mpResources);

    //Setup the "default" updater container
    mpUpdater->AddContainer("Default");
    mpUpdater->SetContainer("Default");

    mpMutex = cPlatform::CreateMutEx();

    //Since game is not done:
    mbGameIsDone = false;

    mbPaused = false;

    mpFPSCounter = hplNew( cFPSCounter, (mpSystem->GetLowLevel()) );
    mpFrameTimer = cPlatform::CreateTimer();
    Log("--------------------------------------------------------\n\n");

    Log("User Initialization\n");
    Log("--------------------------------------------------------\n");
}

//-----------------------------------------------------------------------

cEngine::~cEngine()
{
    Log("--------------------------------------------------------\n\n");

    hplDelete(mpFPSCounter);
    hplDelete(mpFrameTimer);
    hplDelete(mpMutex);

    hplDelete(mpUpdater);

    hplDelete(mpGui);
    hplDelete(mpScene);
    hplDelete(mpInput);
    hplDelete(mpSound);
    hplDelete(mpGraphics);
    hplDelete(mpResources);
    hplDelete(mpPhysics);
    hplDelete(mpAI);
    hplDelete(mpSystem);

    Log(" Deleting game setup provided by user\n");
    hplDelete(mpGameSetup);

    Log("HPL Exit was successful!\n");
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PUBLIC METHOD
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cEngine::Run()
{
    bool bGameWasFocused = true;
    float fMasterVolume = 1.0f;

    double dAverageFPS = 0.0;
    double dLastLogClearedTime = 0.0;
    const double kLogClearedInterval = 1.0;

    size_t iTotalFrames = 0; // Every frame i saw in the game!
    size_t iGameplayFrames = 0; // Only frames actually drawn during gameplay

    unsigned int iFrameTimeHg[1000] = {0};
    double dTempTime = cPlatform::GetApplicationTimeX();

    //Log line that ends user init.
    Log("--------------------------------------------------------\n\n");

    mpUpdater->BroadcastMessageToAll(eUpdateableMessage_OnStart);

    Log("Engine Initialized!\n");
    Log("--------------------------------------------------------\n");

    mpFrameTimer->Start();

    // Reset mutable variables since engine init!
    dLogicTime = 0.0;
    dAccumulator = 0.0;
    dSpeedMul = 1.0;

    // Should return seconds natively instead of SDL_GetTicks divided by 1000.
    dCurrentTime = cPlatform::GetApplicationTimeX();

    // Render interpolation works in fixed ticks
    cRenderInterpolation::SetFixedDelta(kFixedDelta);

    while(!GetGameIsDone())
    {
        ///////////////////////////////////////
        // Get the time from the last frame.
        const double dNewTime = cPlatform::GetApplicationTimeX();
        double dRawFrameTime = dNewTime - dCurrentTime;

        ++iTotalFrames;

        if(dRawFrameTime > 0.0 && mbNonGameplayFrame == false)
        {
            ++iGameplayFrames;

            int iFrameMs = (int)(dRawFrameTime * 1000.0);

            if(iFrameMs < 0)
            {
                iFrameMs = 0;
            }
            if(iFrameMs > 999)
            {
                iFrameMs = 999;
            }

            iFrameTimeHg[iFrameMs]++;
        }
        mbNonGameplayFrame = false;

        dFrameTime = std::max(0.0, dRawFrameTime);

        // Clamp the game frame time and prevent huge single wallclock jump
        if(dFrameTime > kMaxFrameTime)
        {
            dFrameTime = kMaxFrameTime;
        }
        dCurrentTime = dNewTime;

        // Do we actually have window focus?
        bool bGameHasFocus = mpGraphics->GetLowLevel()->GetWindowInputFocus();

        // Detect whether game was in focus at the time
        if(bGameHasFocus != bGameWasFocused)
        {
            if(!bGameHasFocus)
            {
                fMasterVolume = mpSound->GetLowLevel()->GetVolume();
                mpSound->GetLowLevel()->SetVolume(0.0f);
            }
            else
            {
                mpSound->GetLowLevel()->SetVolume(fMasterVolume);
            }

            // State tracker update
            bGameWasFocused = bGameHasFocus;
        }

        if(!bGameHasFocus && !mbPaused)
        {
            cPlatform::Sleep(33);
        }

        // Check if quit message was actually posted.
        if(mpInput->isQuitMessagePosted())
        {
            mpUpdater->RunMessage(eUpdateableMessage_OnQuit);
            mpInput->resetQuitMessagePosted();
            break;
        }

        // Reset update on current frame, then Accumulate the time since last one
        // and multiply the accumulated time with the game logic speed mul
        iUpdatesOnCurrentFrame = 0;
        dAccumulator += dFrameTime * dSpeedMul;

        int iDynamicMaxUpdates = iMaxGameUpdates;
        if(dSpeedMul > 1.0)
        {
            iDynamicMaxUpdates = (int)(iMaxGameUpdates * dSpeedMul);
        }

        while(dAccumulator >= kFixedDelta && iUpdatesOnCurrentFrame < iDynamicMaxUpdates)
        {
            if(GetGameIsDone())
            {
                break;
            }

            // Run Update callback in updater
            START_TIMING(FixedUpdate)
            cRenderInterpolation::BeginTick();
            mpUpdater->RunMessage(eUpdateableMessage_PreUpdate, kFixedDelta);
            mpUpdater->RunMessage(eUpdateableMessage_Update, kFixedDelta);
            mpUpdater->RunMessage(eUpdateableMessage_PostUpdate, kFixedDelta);
            cRenderInterpolation::EndTick();
            STOP_TIMING(FixedUpdate)

            // If log update is active, clear it regularly.
            if(GetUpdateLogActive() && mpUpdater->GetCurrentContainerName() == "Default" &&
                    dLogicTime - dLastLogClearedTime >= kLogClearedInterval)
            {
                ClearUpdateLogFile();
                dLastLogClearedTime = dLogicTime;
            }

            // Increase simulated logic then subtract
            dLogicTime += kFixedDelta;
            dAccumulator -= kFixedDelta;

            // Increment updates based on current frame
            ++iUpdatesOnCurrentFrame;
        }

        // Hit the update cap (use the same cap as the loop above, it is raised with the speed mul)
        if(dAccumulator >= kFixedDelta && iUpdatesOnCurrentFrame >= iDynamicMaxUpdates)
        {
            dAccumulator = std::fmod(dAccumulator, kFixedDelta);
        }

        // Make alpha dividing the accumulated time by the fixed delta timestep.
        // Rendering shows the world this far between the last two ticks.
        //
        // The time spent running the ticks of this frame is added too. The clock was read
        // before the ticks ran, so without it frames that ran a tick showed the world
        // "older" than frames that did not (by the tick's CPU time), which is felt as
        // micro stutter at high frame rates. The time is not removed from the
        // accumulator, the next frame's delta includes it as usual.
        double dTimeSpentInTicks = 0.0;
        if(iUpdatesOnCurrentFrame > 0)
        {
            dTimeSpentInTicks = (cPlatform::GetApplicationTimeX() - dCurrentTime) * dSpeedMul;
            if(dTimeSpentInTicks < 0.0)
            {
                dTimeSpentInTicks = 0.0;
            }
        }
        dRenderAlpha = (dAccumulator + dTimeSpentInTicks) / kFixedDelta;
        if(dRenderAlpha > 1.0)
        {
            dRenderAlpha = 1.0;
        }
        cRenderInterpolation::SetFrameAlpha(dRenderAlpha);

        // Read the very latest mouse/input state right before drawing, so things
        // driven every frame (mouse look, GUI cursor) use input that is as fresh as possible.
        mpInput->FrameUpdate();
        mpUpdater->RunMessage(eUpdateableMessage_OnFrameInput, dFrameTime);

        START_TIMING(OnDraw)
        mpUpdater->RunMessage(eUpdateableMessage_OnDraw, dFrameTime);
        STOP_TIMING(OnDraw)

        START_TIMING(RenderAll)
        mpScene->Render(dFrameTime, tSceneRenderFlag_All);
        STOP_TIMING(RenderAll)

        START_TIMING(FlushRender)
        mpGraphics->GetLowLevel()->FlushRendering();
        STOP_TIMING(FlushRender)

        START_TIMING(SwapBuffers)
        mpGraphics->GetLowLevel()->SwapBuffers();
        STOP_TIMING(SwapBuffers)

        mpUpdater->RunMessage(eUpdateableMessage_OnPostBufferSwap);

        mpFPSCounter->AddFrame();

        WaitForFrameLimit();
    }

    Log("--------------------------------------------------------\n\n");

    Log("Statistics\n");
    Log("--------------------------------------------------------\n");

    double dTime = cPlatform::GetApplicationTimeX() - dTempTime;
    dAverageFPS = iTotalFrames / dTime;

    Log(" Average FrameTime: %.1f ms\n", (1.0 / dAverageFPS) * 1000.0);
    Log(" Average Framerate: %.1f FPS\n", dAverageFPS);

    if(iGameplayFrames > 0)
    {
        // Count both 1 and .1% low frame times of the whole session
        size_t iWorstCount01  = std::max((size_t)1, (size_t)(iGameplayFrames * 0.01));
        double dWorstSum01 = 0.0;

        size_t iWorstCount001 = std::max((size_t)1, (size_t)(iGameplayFrames * 0.001));
        double dWorstSum001 = 0.0;

        size_t iCountedFrames = 0;

        // Iterate backwards from worst possible time...
        for(int i = 999; i >= 0; --i)
        {
            unsigned int iFramesInBucket = iFrameTimeHg[i];

            while(iFramesInBucket > 0 && iCountedFrames < iWorstCount01)
            {
                // Convert the time back to seconds
                double dSeconds = (double)i / 1000.0;

                dWorstSum01 += dSeconds;

                if(iCountedFrames < iWorstCount001)
                {
                    dWorstSum001 += dSeconds;
                }

                iCountedFrames++;
                iFramesInBucket--;
            }

            if(iCountedFrames >= iWorstCount01)
            {
                break;
            }
        }

        double dAvgWorst01 = dWorstSum01 / iWorstCount01;
        double dAvgWorst001 = dWorstSum001 / iWorstCount001;

        Log("      1%% Framerate: %.1f FPS\n", 1.0 / dAvgWorst01);
        Log("    0.1%% Framerate: %.1f FPS\n", 1.0 / dAvgWorst001);
    }

    Log("--------------------------------------------------------\n\n");

    Log("User Exit\n");
    Log("--------------------------------------------------------\n");

    mpUpdater->BroadcastMessageToAll(eUpdateableMessage_OnExit);
}

//-----------------------------------------------------------------------

void cEngine::SetFrameRateLimit(int alFPS)
{
    if(alFPS < 0)
    {
        alFPS = 0;
    }
    //Anything below the logic rate makes no sense (and breaks pacing), clamp it.
    if(alFPS > 0 && alFPS < 30)
    {
        alFPS = 30;
    }

    mlFrameRateLimit = alFPS;
    mdFrameLimitPeriod = alFPS > 0 ? 1.0 / (double)alFPS : 0.0;
    mdNextFrameDeadline = 0.0;
}

//-----------------------------------------------------------------------

void cEngine::WaitForFrameLimit()
{
    if(mdFrameLimitPeriod <= 0.0)
    {
        return;
    }

    double dNow = cPlatform::GetApplicationTimeX();

    // Keep a fixed cadence (deadline += period) so the presented frames are evenly spaced.
    // If we fell behind by more than a frame (load, hitch), start over from now instead
    // of trying to catch up with a burst of frames.
    mdNextFrameDeadline += mdFrameLimitPeriod;
    if(mdNextFrameDeadline < dNow - mdFrameLimitPeriod || mdNextFrameDeadline > dNow + mdFrameLimitPeriod)
    {
        mdNextFrameDeadline = dNow + mdFrameLimitPeriod;
    }

    // Sleep while there is plenty of time left (Sleep has ~1 ms granularity at best)...
    while(mdNextFrameDeadline - dNow > 0.002)
    {
        cPlatform::Sleep(1);
        dNow = cPlatform::GetApplicationTimeX();
    }

    // ...and spin the last bit for accuracy.
    while(dNow < mdNextFrameDeadline)
    {
        dNow = cPlatform::GetApplicationTimeX();
    }
}

//-----------------------------------------------------------------------

void cEngine::Exit()
{
    mbGameIsDone = true;
}

bool cEngine::GetGameIsDone()
{
    return mbGameIsDone;
}

//-----------------------------------------------------------------------

float cEngine::GetFPS()
{
    return mpFPSCounter->mfFPS;
}

//-----------------------------------------------------------------------

float cEngine::GetAvgFrameTimeInMS()
{
    return (1.0f / mpFPSCounter->mfFPS) * 1000.0f;
}

//-----------------------------------------------------------------------

void cEngine::SetFPSUpdateRate(float afSec)
{
    mpFPSCounter->mfUpdateRate = afSec;
}
float cEngine::GetFPSUpdateRate()
{
    return mpFPSCounter->mfUpdateRate;
}

//-----------------------------------------------------------------------

cScriptVar *cEngine::CreateLocalVar(const tString& asName)
{
    cScriptVar* pVar;
    pVar = GetLocalVar(asName);
    if(pVar == NULL)
    {
        cScriptVar Var;
        Var.msName = asName;
        m_mapLocalVars.insert(tScriptVarMap::value_type(cString::ToLowerCase(asName), Var));
        pVar = GetLocalVar(asName);
        if(pVar == NULL)
        {
            FatalError("Very strange error when creating script var!\n");
        }
    }
    return pVar;
}

//-----------------------------------------------------------------------

cScriptVar *cEngine::GetLocalVar(const tString& asName)
{
    tScriptVarMapIt it = m_mapLocalVars.find(cString::ToLowerCase(asName));
    if(it == m_mapLocalVars.end())
    {
        return NULL;
    }

    return &it->second;
}

//-----------------------------------------------------------------------

tScriptVarMap *cEngine::GetLocalVarMap()
{
    return &m_mapLocalVars;
}

//-----------------------------------------------------------------------

cScriptVar *cEngine::CreateGlobalVar(const tString& asName)
{
    cScriptVar* pVar;
    pVar = GetGlobalVar(asName);
    if(pVar == NULL)
    {
        cScriptVar Var;
        Var.msName = asName;
        m_mapGlobalVars.insert(tScriptVarMap::value_type(cString::ToLowerCase(asName), Var));
        pVar = GetGlobalVar(asName);
        if(pVar == NULL)
        {
            FatalError("Very strange error when creating script var!\n");
        }
    }
    return pVar;
}

//-----------------------------------------------------------------------

cScriptVar *cEngine::GetGlobalVar(const tString& asName)
{
    tScriptVarMapIt it = m_mapGlobalVars.find(cString::ToLowerCase(asName));
    if(it == m_mapGlobalVars.end())
    {
        return NULL;
    }

    return &it->second;
}

//-----------------------------------------------------------------------

tScriptVarMap *cEngine::GetGlobalVarMap()
{
    return &m_mapGlobalVars;
}

//-----------------------------------------------------------------------

void cEngine::ClearAllVariables()
{
    m_mapGlobalVars.clear();
    m_mapLocalVars.clear();
}

//-----------------------------------------------------------------------

eVariableType cEngine::GetEngineTypeFromString(const tString& asType)
{
    for(int i = 0; i < (int)mvEngineTypeStrings.size(); ++i)
    {
        if(asType == mvEngineTypeStrings[i])
        {
            return (eVariableType)i;
        }
    }

    return eVariableType_LastEnum;
}

eVariableType cEngine::GetEngineTypeFromStringW(const tWString& asType)
{
    return GetEngineTypeFromString(cString::To8Char(asType));
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// PRIVATE METHOD
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------
}
