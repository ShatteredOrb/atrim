#ifndef LUX_MUSIC_HANDLER_H
#define LUX_MUSIC_HANDLER_H

//----------------------------------------------

#include "LuxBase.h"

//----------------------------------------------

class cLuxMusic
{
    friend class cLuxMusicHandler;
public:
    cLuxMusic();

    void Reset();

    tString msFile;
    bool mbLoop;
    bool mbResume;
    float mfVolume;
    bool mbSpecialEffect;    //If started by enemy close or similar.
};

//----------------------------------------


class cLuxMusicHandler : public iLuxUpdateable
{
    friend class cLuxMusicHandler_SaveData;
public:
    cLuxMusicHandler();
    ~cLuxMusicHandler();

    void OnStart();
    void Update(double adFixedDelta);
    void Reset();

    float GetVolumeMul()
    {
        return mfVolumeMul;
    }

    int GetMaxPrio()
    {
        return mlMaxPrio;
    }
    int GetCurrentMaxPrio()
    {
        return mlCurrentMaxPrio;
    }

    cLuxMusic *GetMusic(int alPrio)
    {
        return &mvGameMusic[alPrio];
    }

    void OnMapEnter(cLuxMap *apMap);
    void OnMapLeave(cLuxMap *apMap);

    void Play(const tString &asFile, bool abLoop, float afVolume, float afFadeTime, int alPrio, bool abResume, bool abSpecialEffect);
    void Stop(float afFadeTime, int alPrio, bool abForceStop = false);

    //Stops enemy (attack/search) and danger music. Must force stop because
    //their mvGameMusic slots are blanked on Play so they do not auto resume.
    void StopAllEnemyMusic(float afFadeTime);

    void AddEnemy(eLuxEnemyMusic aType, iLuxEnemy *apEnemy);
    void RemoveEnemy(eLuxEnemyMusic aType, iLuxEnemy *apEnemy);
    bool EnemyExist(eLuxEnemyMusic aType, iLuxEnemy *apEnemy);

    void AddSearcher(iLuxEnemy *apEnemy);
    void RemoveSearcher(iLuxEnemy *apEnemy);
    bool SearcherExist(iLuxEnemy *apEnemy);

private:
    void UpdateDangerMusic(double adFixedDelta);
    void UpdateEnemyMusic(double adFixedDelta, eLuxEnemyMusic aType);

    void PlayHighestPriority();

    //////////////////
    // Data
    cMusicHandler *mpMusicHandler;
    int mlMaxPrio;
    float mfVolumeMul;

    //////////////////
    // Variables
    float mfUpdateDangerCount;
    float mfUpdateAttackCount;

    std::vector<cLuxMusic> mvGameMusic;

    int mlCurrentMaxPrio;

    tLuxEnemySet m_setEnemies[eLuxEnemyMusic_LastEnum];

    bool mbEnemyPlaying[eLuxEnemyMusic_LastEnum];
    float mfEnemyPlayCount[eLuxEnemyMusic_LastEnum];
    float mfEnemyStopCount[eLuxEnemyMusic_LastEnum];

    bool mbEnemyClosePlaying;
    float mfEnemyCloseCount;
    float mfEnemyGoneCount;
};

//----------------------------------------------


#endif // LUX_MUSIC_HANDLER_H
