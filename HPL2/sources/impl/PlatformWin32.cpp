#include "system/Platform.h"
#include "impl/PlatformWin32.h"

#include "system/String.h"

#include "system/LowLevelSystem.h"

#define UNICODE
#include <windows.h>
#include <shlwapi.h>
#include <shlobj.h>

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <fstream>
#include <string>
#include <io.h>
#include <direct.h>

#include "SDL/SDL.h"

#include "impl/TimerSDL.h"
#include "impl/ThreadWin32.h"
#include "impl/MutexWin32.h"

#include <algorithm>

namespace hpl
{

static bool bTimerInitialized = false;
static LARGE_INTEGER lTimerFrequency;
static LARGE_INTEGER lTimerStart;

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// DWM COMPOSITOR CHECK
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

bool cPlatformWin32::DWMCompositorActive()
{
    static bool bDwmChecked = false;
    static bool bDwmActive  = false;

    // This check might not be thread safe... but ok ig ;P
    if(bDwmChecked)
    {
        return bDwmActive;
    }
    bDwmChecked = true;

    wchar_t sysDir[MAX_PATH];
    if(GetSystemDirectoryW(sysDir, MAX_PATH) == 0)
    {
        return false;
    }

    wchar_t dwmPath[MAX_PATH];
    if(PathCombineW(dwmPath, sysDir, L"dwmapi.dll") == 0)
    {
        return false;
    }

    // Only dwmapi.dll should be valid in System32 and SysWOW64 to prevent malicious hijacks

    HMODULE hDwmapi = LoadLibraryW(dwmPath);
    if(!hDwmapi)
    {
        return false;
    }

    // Loading process and final check if its active, free library otherwise

    typedef HRESULT(WINAPI * DwmIsCompositionEnabledFunc)(BOOL*);

    DwmIsCompositionEnabledFunc pDwmIsCompositionEnabled =
        (DwmIsCompositionEnabledFunc)GetProcAddress(hDwmapi, "DwmIsCompositionEnabled");

    BOOL bDwmEnabled = FALSE;
    if(pDwmIsCompositionEnabled && SUCCEEDED(pDwmIsCompositionEnabled(&bDwmEnabled)))
    {
        bDwmActive = (bDwmEnabled != FALSE);
    }

    FreeLibrary(hDwmapi);
    return bDwmActive;
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// APP THREAD LOCK
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void LockApplicationThread()
{
    // HPET usually runs at ~14.31818 MHz
    // ACPI timer runs at ~3.579545 MHz

    static LARGE_INTEGER iCpuFreq = {};
    static bool bFreqQueried = false;

    if(!bFreqQueried)
    {
        bFreqQueried = true;
        QueryPerformanceFrequency(&iCpuFreq);
    }

    // Check if this is on an invariant TSC or safe virtualization
    if(iCpuFreq.QuadPart >= 10000000)
    {
        return;
    }

    // Load the kernel32 stub for win9x/nt4/2k/xp and the affinity stuff
    // If not found, just return and execute anyway assuming the user's
    // cpu is a 1c1t single socket system on win9x to 2k...

    HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");
    if(!hKernel32)
    {
        return;
    }

    typedef DWORD_PTR (WINAPI * SetThreadAffMaskFunc)(HANDLE, DWORD_PTR);
    typedef BOOL (WINAPI * GetProcAffMaskFunc)(HANDLE, PDWORD_PTR, PDWORD_PTR);

    SetThreadAffMaskFunc pSetThreadAffinity =
        (SetThreadAffMaskFunc)GetProcAddress(hKernel32, "SetThreadAffinityMask");
    GetProcAffMaskFunc pGetProcessAffinity =
        (GetProcAffMaskFunc)GetProcAddress(hKernel32, "GetProcessAffinityMask");

    if(pSetThreadAffinity && pGetProcessAffinity)
    {
        DWORD_PTR processMask = 0;
        DWORD_PTR systemMask = 0;

        if(pGetProcessAffinity(GetCurrentProcess(), &processMask, &systemMask))
        {
            // Find lowest available core mask
            DWORD_PTR coreMask = 1;

            while(coreMask != 0 && (coreMask & processMask) == 0)
            {
                coreMask <<= 1;
            }

            if(coreMask != 0)
            {
                pSetThreadAffinity(GetCurrentThread(), coreMask);
            }
        }
    }
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// FILE HANDLING
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

unsigned long cPlatform::GetFileSize(const tWString& asFileName)
{
    /*__stat64 fileStat;
    int lErr = _wstat64( asFileName.c_str(), &fileStat );
    if (lErr != 0) return 0;
    return (unsigned long)fileStat.st_size; */

    FILE *pFile = _wfopen(asFileName.c_str(), _W("rb"));
    if(pFile == NULL)
    {
        return 0;
    }

    fseek(pFile, 0, SEEK_END);
    long lFileSize = ftell(pFile);
    rewind(pFile);

    fclose(pFile);
    return (unsigned long)lFileSize;
}

//-----------------------------------------------------------------------

bool cPlatform::CopyFileToBuffer(const tWString& asFileName, void *apBuffer, unsigned long alSize)
{
    FILE *pFile = _wfopen(asFileName.c_str(), _W("rb"));
    if(pFile == NULL)
    {
        return false;
    }

    fread(apBuffer, sizeof(char), alSize, pFile);

    fclose(pFile);
    return true;
}

//-----------------------------------------------------------------------

bool cPlatform::FileExists(const tWString& asFileName)
{
    FILE *f = _wfopen(asFileName.c_str(), _W("rb"));
    if(f == NULL)
    {
        return false;
    }

    fclose(f);
    return true;
}

//-----------------------------------------------------------------------

void cPlatform::RemoveFile(const tWString& asFilePath)
{
    _wremove(asFilePath.c_str());
}

//-----------------------------------------------------------------------

bool cPlatform::CloneFile(    const tWString& asSrcFileName, const tWString& asDestFileName,
                              bool abFailIfExists)
{
    return CopyFile(asSrcFileName.c_str(), asDestFileName.c_str(), abFailIfExists) == TRUE;
}

//-----------------------------------------------------------------------

bool cPlatform::CreateFolder(const tWString& asPath)
{
    tWString sPath = cString::ReplaceCharToW(asPath, _W("/"), _W("\\"));

    return CreateDirectory(sPath.c_str(), NULL) == TRUE;
}

//-----------------------------------------------------------------------

bool cPlatform::RemoveFolder(const tWString& asPath, bool abDeleteAllFiles, bool abDeleteAllSubFolders)
{
    tWString sPath = cString::ReplaceCharToW(asPath, _W("/"), _W("\\"));

    ////////////////////
    // Remove any files in the directory
    if(abDeleteAllFiles)
    {
        tWStringList lstFiles;
        FindFilesInDir(lstFiles, sPath, _W("*.*"), true);
        for(tWStringListIt it = lstFiles.begin(); it != lstFiles.end(); ++it)
        {
            tWString sFilePath = cString::SetFilePathW(*it, sPath);
            RemoveFile(sFilePath);
        }
    }

    ////////////////////
    // Remove any sub folders in the directory
    if(abDeleteAllSubFolders)
    {
        tWStringList lstFolders;
        FindFoldersInDir(lstFolders, sPath, true, false);
        for(tWStringListIt it = lstFolders.begin(); it != lstFolders.end(); ++it)
        {
            tWString sFolderPath = cString::SetFilePathW(*it, sPath);

            DWORD dwAttrs = GetFileAttributesW(sFolderPath.c_str());
            if (dwAttrs != INVALID_FILE_ATTRIBUTES &&
                    (dwAttrs & FILE_ATTRIBUTE_REPARSE_POINT))
            {
                if(dwAttrs & FILE_ATTRIBUTE_READONLY)
                {
                    SetFileAttributesW(sFolderPath.c_str(), dwAttrs & ~FILE_ATTRIBUTE_READONLY);
                }

                RemoveDirectoryW(sFolderPath.c_str());
            }
            else
            {
                RemoveFolder(sFolderPath, abDeleteAllFiles, abDeleteAllSubFolders);
            }
        }
    }

    ////////////////////
    // Clear read only bit on the folder to be removed
    {
        DWORD dwAttrs = GetFileAttributesW(sPath.c_str());

        if(dwAttrs != INVALID_FILE_ATTRIBUTES && (dwAttrs & FILE_ATTRIBUTE_READONLY))
        {
            SetFileAttributesW(sPath.c_str(), dwAttrs & ~FILE_ATTRIBUTE_READONLY);
        }
    }

    if(RemoveDirectoryW(sPath.c_str()) != TRUE)
    {
        wchar_t sTempString[2048];
        FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, 0, GetLastError(), 0, sTempString, sizeof(sTempString), NULL);
        Error("Could not remove folder: '%s': %s", cString::To8Char(sPath).c_str(), cString::To8Char(sTempString).c_str());
        return false;
    }
    return true;
}

//-----------------------------------------------------------------------

bool cPlatform::FolderExists(const tWString& asPath)
{
    tWString sDir = cString::ReplaceCharToW(asPath, _W("/"), _W("\\"));
    DWORD lFileAttributes = GetFileAttributes(sDir.c_str());

    return (lFileAttributes != INVALID_FILE_ATTRIBUTES && (lFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);
}

//-----------------------------------------------------------------------

tWString cPlatform::GetFullFilePath(const tWString& asFilePath)
{
    wchar_t sOutFilePath[2048];
    int x = GetFullPathNameW(asFilePath.c_str(), 2048, sOutFilePath, NULL);

    return sOutFilePath;
}

//-----------------------------------------------------------------------

FILE *cPlatform::OpenFile(const tWString& asFileName, const tWString asMode)
{
    return _wfopen(asFileName.c_str(), asMode.c_str());
}

//-----------------------------------------------------------------------

static cDate DateFromLocalTime(struct tm* apClock)
{
    if(!apClock)
    {
        return cDate();
    }

    cDate date;

    date.seconds = apClock->tm_sec;
    date.minutes = apClock->tm_min;
    date.hours = apClock->tm_hour;
    date.month_day = apClock->tm_mday;
    date.month = apClock->tm_mon;
    date.year = 1900 + apClock->tm_year;
    date.week_day = apClock->tm_wday;
    date.year_day = apClock->tm_yday;

    return date;
}

//-----------------------------------------------------------------------

cDate cPlatform::FileModifiedDate(const tWString& asFilePath)
{
    struct _stat attrib;
    if(_wstat(asFilePath.c_str(), &attrib) != 0)
    {
        Error("Cannot create attrib time struct for file '%s'!\n", cString::To8Char(asFilePath).c_str());
        return cDate();
    }

    // Get the last modified time and put it into the time structure
    struct tm* pClock = localtime(&(attrib.st_mtime));
    if(!pClock)
    {
        Error("Failed to get last modified time for file '%s'!\n", cString::To8Char(asFilePath).c_str());
        return cDate();
    }

    cDate date = DateFromLocalTime(pClock);

#ifdef _DEBUG
    Log("DEBUG: Modified time for file '%s': %02d-%02d-%04d %02d:%02d:%02d\n",
        cString::To8Char(asFilePath).c_str(),
        date.month, date.month_day, date.year,
        date.hours, date.minutes, date.seconds);
#endif

    return date;
}

//-----------------------------------------------------------------------

cDate cPlatform::FileCreationDate(const tWString& asFilePath)
{
    struct _stat attrib;
    if(_wstat(asFilePath.c_str(), &attrib) != 0)
    {
        Error("Cannot create attrib time struct for file '%s'!\n", cString::To8Char(asFilePath).c_str());
        return cDate();
    }

    // Get the last created time and put it into the time structure
    struct tm* pClock = localtime(&(attrib.st_ctime));
    if(!pClock)
    {
        Error("Failed to get last created time for file '%s'!\n", cString::To8Char(asFilePath).c_str());
        return cDate();
    }

    cDate date = DateFromLocalTime(pClock);

#ifdef _DEBUG
    Log("DEBUG: Creation time for file '%s': %02d-%02d-%04d %02d:%02d:%02d\n",
        cString::To8Char(asFilePath).c_str(),
        date.month, date.month_day, date.year,
        date.hours, date.minutes, date.seconds);
#endif

    return date;
}

//-----------------------------------------------------------------------

void cPlatform::FindFilesInDir(tWStringList &alstStrings, const tWString& asDir, const tWString& asMask, bool abAddHidden)
{
    //these windows functions only works with "\".. sucks ...
    tWString sDir = cString::ReplaceCharToW(asDir, _W("/"), _W("\\"));

    int lLastMaskAsteriskPos = cString::GetLastStringPosW(asMask, _W("*"));
    tWString sMaskExt = cString::ToLowerCaseW(cString::SubW(asMask, lLastMaskAsteriskPos + 2));
    bool bMaskNotNull = (sMaskExt != _W(""));

    //Get the search string
    wchar_t sSpec[256];

    if (sDir.empty())
    {
        swprintf(sSpec, 256, _W("%s"), asMask.c_str());
    }
    else
    {
        wchar_t end = sDir[sDir.size() - 1];
        if (end == _W('\\') || end == _W('/'))
        {
            swprintf(sSpec, 256, _W("%s%s"), sDir.c_str(), asMask.c_str());
        }
        else
        {
            //these windows functions only works with "\".. sucks ...
            swprintf(sSpec, 256, _W("%s\\%s"), sDir.c_str(), asMask.c_str());
        }
    }

    //The needed structs
    intptr_t lHandle;
    struct _wfinddata_t FileInfo;


    //Find the first file:
    lHandle = _wfindfirst(sSpec, &FileInfo );
    if(lHandle == -1L)
    {
        return;
    }

    //Check so it is not a subdir
    if((FileInfo.attrib & _A_SUBDIR) == 0 && (abAddHidden || (FileInfo.attrib & _A_HIDDEN) == 0) )
    {
        if(bMaskNotNull)
        {
            if(cString::ToLowerCaseW(cString::GetFileExtW(FileInfo.name)) == sMaskExt)
            {
                alstStrings.push_back(FileInfo.name);
            }
        }
        else
        {
            alstStrings.push_back(FileInfo.name);
        }
    }

    //Get the other files.
    while( _wfindnext( lHandle, &FileInfo ) == 0 )
    {
        if((FileInfo.attrib & _A_SUBDIR) == 0 && (abAddHidden || (FileInfo.attrib & _A_HIDDEN) == 0) )
        {
            if(bMaskNotNull)
            {
                if(cString::ToLowerCaseW(cString::GetFileExtW(FileInfo.name)) == sMaskExt)
                {
                    alstStrings.push_back(FileInfo.name);
                }
            }
            else
            {
                alstStrings.push_back(FileInfo.name);
            }
        }
    }

    //Close the handle to release reosuces and unlock files
    _findclose(lHandle);
}

//-----------------------------------------------------------------------

static bool IsGoodFolder(struct _wfinddata_t* pFileInfo, bool abAddHidden, bool abAddUpFolder)
{
    if( (pFileInfo->attrib & _A_SUBDIR) == 0)
    {
        return false;
    }
    if( abAddHidden == false && (pFileInfo->attrib & _A_HIDDEN))
    {
        return false;
    }

    tWString sName = pFileInfo->name;
    if( sName == _W("..") )
    {
        return abAddUpFolder;
    }
    if( sName == _W(".") )
    {
        return false;
    }

    return true;
}

void cPlatform::FindFoldersInDir(tWStringList &alstStrings, const tWString& asDir, bool abAddHidden, bool abAddUpFolder)
{
    //these windows functions only works with "\".. sucks ...
    tWString sDir = cString::ReplaceCharToW(asDir, _W("/"), _W("\\"));

    //Get the search string
    wchar_t sSpec[256];
    wchar_t end = sDir[sDir.size() - 1];

    if(end == _W('\\') || end == _W('/'))
    {
        swprintf(sSpec, 256, _W("%s%s"), sDir.c_str(), _W("*.*"));
    }
    else
    {
        //these windows functions only works with "\".. sucks ...
        swprintf(sSpec, 256, _W("%s\\%s"), sDir.c_str(), _W("*.*"));
    }

    //The needed structs
    intptr_t lHandle;
    struct _wfinddata_t FileInfo;

    //Find the first file:
    lHandle = _wfindfirst(sSpec, &FileInfo );
    if(lHandle == -1L)
    {
        return;
    }

    //Check so it IS a sub dir
    if( IsGoodFolder(&FileInfo, abAddHidden, abAddUpFolder))
    {
        alstStrings.push_back(FileInfo.name);
    }

    //Get the other files.
    while( _wfindnext( lHandle, &FileInfo ) == 0 )
    {
        if( IsGoodFolder(&FileInfo, abAddHidden, abAddUpFolder) )
        {
            alstStrings.push_back(FileInfo.name);
        }

    }

    //Close the handle to release reosuces and unlock files
    _findclose(lHandle);
}

//-----------------------------------------------------------------------

tWString cPlatform::GetWorkingDir()
{
    tWString sDir = tWString( _wgetcwd(NULL, 0));

    return sDir;
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// APPLICATION
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

double cPlatform::GetApplicationTimeX()
{
    if(!bTimerInitialized)
    {
        LockApplicationThread();

        QueryPerformanceFrequency(&lTimerFrequency);
        QueryPerformanceCounter(&lTimerStart);
        bTimerInitialized = true;
    }

    LARGE_INTEGER lCurrentTime;
    QueryPerformanceCounter(&lCurrentTime);

    return (double)(lCurrentTime.QuadPart - lTimerStart.QuadPart) /
           (double)lTimerFrequency.QuadPart;
}

//-----------------------------------------------------------------------

unsigned long cPlatform::GetApplicationTime()
{
    return SDL_GetTicks();
}

//-----------------------------------------------------------------------

void cPlatform::Sleep ( const unsigned int alMillisecs )
{
    SDL_Delay ( alMillisecs );
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// DIALOG
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

void cPlatform::CreateMessageBoxBase(eMsgBoxType eType, const wchar_t* asCaption, const wchar_t* fmt, va_list ap)
{
    wchar_t text[2048];

    if (fmt == NULL)
    {
        return;
    }
    vswprintf(text, 2047, fmt, ap);

    tWString sMess = _W("");

    sMess += text;

    UINT lType = MB_OK;

    switch (eType)
    {
    case eMsgBoxType_Info:
        lType += MB_ICONINFORMATION;
        break;
    case eMsgBoxType_Error:
        lType += MB_ICONERROR;
        break;
    case eMsgBoxType_Warning:
        lType += MB_ICONWARNING;
        break;
    default:
        break;
    }

    MessageBox( NULL, sMess.c_str(), asCaption, lType );
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// SYSTEM STATS
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

ePlatform cPlatform::GetPlatform()
{
    return ePlatform_Win32;
}

//-----------------------------------------------------------------------

tString cPlatform::msName = "Win32";

//-----------------------------------------------------------------------

iTimer *cPlatform::CreateTimer()
{
    return hplNew(cTimerSDL, () );
}

//-----------------------------------------------------------------------

cDate cPlatform::GetDate()
{
    time_t lTime;
    time(&lTime);

    struct tm* pClock;
    pClock = localtime(&lTime);

    return DateFromLocalTime(pClock);
}

//-----------------------------------------------------------------------

void cPlatform::CopyTextToClipboard(const tWString &asText)
{
    if(!OpenClipboard(NULL))
    {
        return;
    }

    EmptyClipboard();

    HGLOBAL clipbuffer = GlobalAlloc(GMEM_MOVEABLE, (asText.size() + 1) * sizeof(wchar_t));
    if(clipbuffer)
    {
        wchar_t *pBuffer = (wchar_t*)GlobalLock(clipbuffer);
        if(pBuffer)
        {
            wcscpy(pBuffer, asText.c_str());
            GlobalUnlock(clipbuffer);
            SetClipboardData(CF_UNICODETEXT, clipbuffer);
        }
    }

    CloseClipboard();
}

//-----------------------------------------------------------------------

tWString cPlatform::LoadTextFromClipboard()
{
    tWString sText = _W("");
    OpenClipboard(NULL);

    HGLOBAL clipbuffer = GetClipboardData(CF_UNICODETEXT);

    wchar_t *pBuffer = (wchar_t*)GlobalLock(clipbuffer);

    if(pBuffer != NULL)
    {
        sText = pBuffer;
    }

    GlobalUnlock(clipbuffer);

    CloseClipboard();

    return sText;
}

//-----------------------------------------------------------------------

unsigned long cPlatform::GetSystemAvailableDrives()
{
    return _getdrives();
}

//-----------------------------------------------------------------------

void cPlatform::GetAvailableVideoModes(tVideoModeVec &avDestVidModes, int alMinBpp, int alMinRefreshRate)
{
    bool bSkipBppCheck = (alMinBpp == -1);
    bool bSkipRateCheck = (alMinRefreshRate == -1);

    avDestVidModes.clear();

    DEVMODE mode = {};
    mode.dmSize = sizeof(DEVMODE);
    int lModeCounter = 0;
    while(EnumDisplaySettings(NULL, lModeCounter, &mode))
    {
        if((bSkipBppCheck || alMinBpp <= (int)mode.dmBitsPerPel) &&
                (bSkipRateCheck || alMinRefreshRate <= (int)mode.dmDisplayFrequency))
        {
            avDestVidModes.push_back(cVideoMode(cVector2l(mode.dmPelsWidth, mode.dmPelsHeight),
                                                mode.dmBitsPerPel,
                                                mode.dmDisplayFrequency));
        }

        ++lModeCounter;
    }

    sort(avDestVidModes.begin(), avDestVidModes.end(), VideoComp());
}

//-----------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// THREADING
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------

iThread *cPlatform::CreateThread(iThreadClass* apThreadClass)
{
    iThread* pThread = hplNew(cThreadWin32, ());
    pThread->SetThreadClass(apThreadClass);

    return pThread;
}

//-----------------------------------------------------------------------

iMutex *cPlatform::CreateMutEx()
{
    return hplNew(cMutexWin32, ());
}

//-----------------------------------------------------------------------

}
