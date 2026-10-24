#include "LuxBase.h"

//---------------------------------------
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <vector>
#elif defined(__linux__)
#include <unistd.h>
#endif

//---------------------------------------

cLuxBase *gpBase = NULL;

//---------------------------------------

/**
 * Starts a new instance of the game with the given command line. Used to switch between the
 * base game and a full conversion (started from the custom story menu) and back.
 * Called after everything was shut down, so the new instance gets the window, sound device
 * and files to itself.
 */
static void RelaunchGame(const tString &asCommandline)
{
#ifdef _WIN32
    wchar_t sExePath[MAX_PATH];
    DWORD lLen = GetModuleFileNameW(NULL, sExePath, MAX_PATH);
    if(lLen == 0 || lLen >= MAX_PATH)
    {
        return;
    }

    //CreateProcessW wants a writable command line buffer: "exe" args
    tWString sCmd = _W("\"") + tWString(sExePath) + _W("\"");
    if(asCommandline != "")
    {
        sCmd += _W(" ") + cString::To16Char(asCommandline);
    }
    std::vector<wchar_t> vCmd(sCmd.begin(), sCmd.end());
    vCmd.push_back(0);

    STARTUPINFOW startInfo;
    PROCESS_INFORMATION procInfo;
    ZeroMemory(&startInfo, sizeof(startInfo));
    startInfo.cb = sizeof(startInfo);
    ZeroMemory(&procInfo, sizeof(procInfo));

    //NULL working dir = same as ours (the game folder)
    if(CreateProcessW(sExePath, &vCmd[0], NULL, NULL, FALSE, 0, NULL, NULL, &startInfo, &procInfo))
    {
        CloseHandle(procInfo.hThread);
        CloseHandle(procInfo.hProcess);
    }
    else
    {
        cPlatform::CreateMessageBox(_W("Error!"), _W("Could not restart the game!"));
    }

#elif defined(__linux__)
    //Replace this process. Arguments as separate argv entries (main() joins them again).
    if(asCommandline.size() >= 3 && asCommandline.substr(0, 3) == "-fc")
    {
        tString sRoot = asCommandline.substr(3);
        while(sRoot.empty() == false && (sRoot[0] == ' ' || sRoot[0] == '"'))
        {
            sRoot = sRoot.substr(1);
        }
        while(sRoot.empty() == false && sRoot[sRoot.size() - 1] == '"')
        {
            sRoot = sRoot.substr(0, sRoot.size() - 1);
        }
        execl("/proc/self/exe", "/proc/self/exe", "-cwd", "-fc", sRoot.c_str(), (char *)NULL);
    }
    else
    {
        execl("/proc/self/exe", "/proc/self/exe", "-cwd", (char *)NULL);
    }
#endif
}

//---------------------------------------

int hplMain(const tString &asCommandline)
{
    //////////////////////////
    // Game creation and exit
    gpBase = hplNew( cLuxBase, ());

    //Init and run if all okay
    if(gpBase->Init(asCommandline))
    {
        gpBase->Run();
        gpBase->Exit();
    }
    //Error occurred
    else
    {
        if(gpBase->msErrorMessage == _W(""))
        {
            gpBase->msErrorMessage = _W("Failed to initialize base engine!");
        }

        cPlatform::CreateMessageBox(_W("Error!"), gpBase->msErrorMessage.c_str());
        //No Exit, since it was not sure everything was created as it should.

        //A full conversion that fails to start: go back to the base game instead of leaving
        //the player at the desktop.
        if(gpBase->mbFullConversionMode)
        {
            gpBase->mbRelaunchAtExit = true;
            gpBase->msRelaunchCommandLine = "";
        }
    }

    //Remember if we need to switch between base game and full conversion, gpBase is gone after this.
    bool bRelaunch = gpBase->mbRelaunchAtExit;
    tString sRelaunchCommandline = gpBase->msRelaunchCommandLine;

    hplDelete(gpBase);

    cMemoryManager::LogResults();

    if(bRelaunch)
    {
        RelaunchGame(sRelaunchCommandline);
    }

    return 0;
}
