#ifndef LUX_MAP_H
#define LUX_MAP_H

//----------------------------------------------

#include "LuxBase.h"

//----------------------------------------------

class cLuxNode_Pos;
class cLuxNode_PlayerStart;
class cLuxArea_Sticky;
class cLuxLampLightConnection;
class cLuxProp_Lamp;

typedef std::multimap<tString, cLuxNode_Pos *> tLuxPosNodeMap;
typedef tLuxPosNodeMap::iterator tLuxPosNodeMapIt;

typedef std::multimap<tString, cLuxNode_PlayerStart *> tLuxPlayerStartMap;
typedef tLuxPlayerStartMap::iterator tLuxPlayerStartMapIt;

typedef std::list<cLuxArea_Sticky *> tLuxArea_StickyList;
typedef tLuxArea_StickyList::iterator tLuxArea_StickyListIt;

typedef std::list<cLuxLampLightConnection *> tLuxLampLightConnectionList;
typedef tLuxLampLightConnectionList::iterator tLuxLampLightConnectionListIt;

//----------------------------------------------
class cLuxMap;

class cLuxDissolveEntity
{
public:
    cLuxDissolveEntity(cLuxMap *apMap);
    ~cLuxDissolveEntity();

    cLuxMap *mpMap;
    cMeshEntity *mpEntity;
    float mfFadeSpeed;
    float mfAlpha;
};

typedef std::list<cLuxDissolveEntity *> tLuxDissolveEntityList;
typedef tLuxDissolveEntityList::iterator tLuxDissolveEntityListIt;

//----------------------------------------------

class cLuxMap
{
    friend class cLuxDissolveEntity;
    friend class cLuxSavedMap;
    friend class cLuxSavedGameMap;
public:
    cLuxMap(const tString& asName);
    ~cLuxMap();

    const tString &GetName()
    {
        return msName;
    }
    const tString &GetFileName()
    {
        return msFileName;
    }

    void SetDisplayNameEntry(const tString& asEntry)
    {
        msDisplayNameEntry = asEntry;
    }
    const tString &GetDisplayNameEntry()
    {
        return msDisplayNameEntry;
    }

    bool LoadFromFile(const tString & asFile, bool abLoadEntities);

    void AfterWorldLoadEntitySetup();

    void OnEnter(bool abRunScript, bool abFirstTime);
    void OnLeave(bool abRunScript);

    void Update(double adFixedDelta);

    void RunScript(const tString& asCommand);
    void RunTimer(const tString& asTimerFunc, tString& asTimerName);
    void RunUpdateCallback(double adFixedDelta);
    bool RecompileScript(tString *apOutput);

    void OnRenderSolid(cRendererCallbackFunctions* apFunctions);

    cWorld *GetWorld()
    {
        return mpWorld;
    }
    iPhysicsWorld *GetPhysicsWorld()
    {
        return mpPhysicsWorld;
    }

    void PlacePlayerAtStartPos(const tString& asPosName);

    void CreateEntity(const tString& asName, const tString& asFile, const cMatrixf& a_mtxTransform, const cVector3f& avScale);

    /**
     * This also destroys timer and light connections
     */
    void DestroyAllEntities();

    void AddEntity(iLuxEntity *apEntity);

    /**
     * Do not call this when IsDeletingAllWorldEntities is true!
     */
    void DestroyEntity(iLuxEntity *apEntity);
    iLuxEntity *GetEntityByName(const tString& asName, eLuxEntityType aType = eLuxEntityType_LastEnum, int alSubType = -1);
    iLuxEntity *GetEntityByID(int alID, eLuxEntityType aType = eLuxEntityType_LastEnum, int alSubType = -1);
    iLuxEntity *GetLatestEntity()
    {
        return mpLatestAddedEntity;
    }
    void ResetLatestEntity()
    {
        mpLatestAddedEntity = NULL;
    }
    bool EntityExists(iLuxEntity *apEntity);
    cLuxEntityIterator GetEntityIterator();
    cLuxEnemyIterator GetEnemyIterator();

    void BroadcastEnemyMessage(eLuxEnemyMessage aType, bool abHasPosition, const cVector3f& avPos, float afRadius,
                               float afTime = 0, bool abLocalScope = false, const cVector3f& avX = 0, float afX = 0, int alX = 0);
    void BroadcastEnemySoundMessage(const cVector3f& avPos, float afVolume, float afMinDist, float afMaxDist, tString asSoundName);
    /**
     * Gets number of enemies that are in range of player
     */
    int GetInRangeEnemyNum();

    bool AINodeIsUsedAsGoal(cAINode *apNode);
    bool DoorIsBroken(int alID);
    bool DoorIsClosed(int alID);
    /**
    * -1 = angle is close to 0, 1=angle is 70% or higher of max, 0=inbetween -1 and 1.
    */
    int GetDoorState(int alID);

    bool BodyIsInDetachableStickyArea(iPhysicsBody* apBody);
    bool DetachBodyFromStickyArea(iPhysicsBody* apBody);

    void DestroyAllRopes();

    iPhysicsBody *GetBodyFromEntityBodyIdPair(const cLuxIdPair &aIdPair);

    bool CheckCollision(iLuxCollideCallbackContainer *apCollider1, iLuxCollideCallbackContainer* apCollider2);

    void AddPlayerStart(cLuxNode_PlayerStart *apNode);
    cLuxNode_PlayerStart *GetPlayerStart(const tString & asName);
    cLuxNode_PlayerStart *GetFirstPlayerStart();
    int GetPlayerStartNodeNum()
    {
        return (int)mvPlayerStartNodes.size();
    }
    cLuxNode_PlayerStart *GetPlayerStartNode(int alIdx)
    {
        return mvPlayerStartNodes[alIdx];
    }

    void AddPosNode(cLuxNode_Pos *apNode);
    cLuxNode_Pos *GetPosNode(const tString & asName);

    void SetCheckPoint(const tString& asName, const tString& asStartPos, const tString& asCallback, bool abKeepPlayerInLimbo);
    void LoadCheckPoint();

    void AddUseItemCallback(    const tString& asName, const tString& asItem, const tString& asEntity,
                                const tString& asFunction, bool abAutoCallback);
    void RemoveUseItemCallback( const tString& asName);

    /**
     * if asName is not "" it can be used as a safty check so it really is the correct callback that is destroyed!
     */
    void RemoveUseItemCallback( cLuxUseItemCallback * apCallback, const tString& asName = "");
    cLuxUseItemCallback *GetUseItemCallback(const tString& asItem, const tString& asEntity);

    void AddTimer(const tString& asName, float afTime, const tString& asFunction);
    void RemoveTimer(const tString& asName);
    cLuxEventTimer *GetTimer(const tString& asName);

    void AddDissolveEntity(cMeshEntity *apMeshEntity, float afTime);

    cLuxLampLightConnection *AddLampLightConnection(cLuxProp_Lamp *apLamp, iLight *apLight, float afAmount, bool abUseOnColor, bool abUseSpec);
    cLuxLampLightConnection *GetLampLightConnection(iLight *apLight);

    cLuxScriptVar *GetVar(const tString &asName);

    bool IsDeletingAllWorldEntities()
    {
        return mbDeletingAllWorldEntities;
    }

    //////////////////////////
    // Properties
    void SetNumberOfQuests(int alX)
    {
        mlNumberOfQuests = alX;
    }
    int GetNumberOfQuests()
    {
        return mlNumberOfQuests;
    }
    void AddCompletionAmount(int alAmount, float afDelay = 0.0f);

    void SetLanternLitCallback(const tString& asCallback)
    {
        msLanternLitCallback = asCallback;
    }
    const tString &GetLanternLitCallback()
    {
        return msLanternLitCallback;
    }

    void RunCheckPointCallbackScript();

private:
    void CalculateTotalCompletionAmount();

    int GetFreeEntityID();

    void UpdateToBeDestroyedEntities(bool abUseCallbacks);
    void UpdateTimers(double adFixedDelta);
    void UpdateDissolveEntities(double adFixedDelta);
    void UpdateLampLightConnections(double adFixedDelta);
    void UpdateCheckCommentaryIconActive(double adFixedDelta);

    tString msName;
    tString msFileName;

    tString msDisplayNameEntry;

    bool mbUpdatingTimers;

    bool mbDeletingAllWorldEntities;

    cEngine *mpEngine;
    cWorld *mpWorld;
    iPhysicsWorld *mpPhysicsWorld;

    iScript *mpScript;

    tString msLanternLitCallback;

    int mlNumberOfQuests;
    int mlTotalCompletionAmount;
    int mlCurrentCompletionAmount;

    bool mbCommentaryIconsActive;

    tString msCheckPointName;
    tString msCheckPointStartPos;
    tString msCheckPointCallback;
    int mlCheckPointCount;
    tString msCheckPointMusic;
    int mlCheckPointMusicPrio;
    bool mbCheckPointMusicResume;
    float mfCheckPointMusicVolume;

    tLuxEventTimerList mlstTimers;

    tLuxScriptVarMap m_mapVars;

    tLuxEntityNameMap m_mapEntitiesByName;
    tLuxEntityIDMap m_mapEntitiesByID;
    tLuxEntityList mlstEntities;
    tLuxEnemyList mlstEnemies;
    tLuxEntityList mlstToBeDestroyedEntities;
    iLuxEntity *mpLatestAddedEntity;
    tLuxArea_StickyList mlstStickyAreas;

    tLuxPlayerStartMap m_mapPlayerStartNodes;
    std::vector<cLuxNode_PlayerStart *> mvPlayerStartNodes;

    tLuxPosNodeMap m_mapPosNodes;

    tLuxUseItemCallbackList mlstUseItemCallbacks;

    tLuxDissolveEntityList mlstDissolveEntities;

    tLuxLampLightConnectionList mlstLampLightConnections;

    bool mbRunUpdateScript;
    float mfTimeToNextCriticalCheck;
};

//----------------------------------------------


#endif // LUX_MAP_H
