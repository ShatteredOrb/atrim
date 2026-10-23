#ifndef HPL_ENGINE_TYPES_H
#define HPL_ENGINE_TYPES_H

#include <map>

#include "system/SerializeClass.h"

namespace hpl
{

//---------------------------------------

enum eHplAPI
{
    eHplAPI_OpenGL
};

//---------------------------------------

enum eHplSetup
{
    eHplSetup_Screen =        eFlagBit_0,
    eHplSetup_Sound =        eFlagBit_1,
    eHplSetup_Input =        eFlagBit_2,
    eHplSetup_Video =        eFlagBit_3, // To Do a "Half" init of the video
    eHplSetup_All =        eFlagBit_All
};

//---------------------------------------

enum eUpdateableMessage
{
    eUpdateableMessage_OnPostBufferSwap,
    eUpdateableMessage_OnStart,
    eUpdateableMessage_OnDraw,
    eUpdateableMessage_PreUpdate,
    eUpdateableMessage_Update,
    eUpdateableMessage_PostUpdate,
    eUpdateableMessage_OnQuit,
    eUpdateableMessage_OnExit,
    eUpdateableMessage_Reset,
    eUpdateableMessage_OnPauseUpdate,

    eUpdateableMessage_AppGotInputFocus,
    eUpdateableMessage_AppGotMouseFocus,
    eUpdateableMessage_AppGotVisibility,

    eUpdateableMessage_AppLostInputFocus,
    eUpdateableMessage_AppLostMouseFocus,
    eUpdateableMessage_AppLostVisibility,

    eUpdateableMessage_AppDeviceWasPlugged,
    eUpdateableMessage_AppDeviceWasRemoved,

    eUpdateableMessage_OnFrameInput,

    eUpdateableMessage_LastEnum
};

//---------------------------------------

enum eVariableType
{
    eVariableType_Int,
    eVariableType_Float,
    eVariableType_Vec2,
    eVariableType_Vec3,
    eVariableType_Color,
    eVariableType_String,
    eVariableType_Enum,
    eVariableType_Bool,

    eVariableType_LastEnum
};

//---------------------------------------

enum eMultipleSettingsCalcType
{
    eMultipleSettingsCalcType_Mul,
    eMultipleSettingsCalcType_Min,
    eMultipleSettingsCalcType_Max,
    eMultipleSettingsCalcType_Avg,
    eMultipleSettingsCalcType_LastEnum
};

class cMultipleSettingsHandler
{
public:
    cMultipleSettingsHandler();
    ~cMultipleSettingsHandler();

    class cGSEntry
    {
        friend class cMultipleSettingsHandler;
    public:
        cGSEntry() : mfVal(1), mfDest(1), mTypes(0), mbDestroyAtDest(false) {}
        cGSEntry(float afVal, tFlag aTypes) : mfVal(afVal), mfDest(afVal), mTypes(aTypes), mbDestroyAtDest(false) {}

        void FadeTo(float afDest, float afSpeed, bool abDestroyAtDest);
        void SetValAndDest(float afVal);

        void SetVal(float afVal)
        {
            mfVal = afVal;
        }
        void SetTypes(tFlag aTypes)
        {
            mTypes = aTypes;
        }
        void SetSpeed(float afSpeed)
        {
            mfSpeed = afSpeed;
        }
        void SetDest(float afDest)
        {
            mfDest = afDest;
        }
        void SetDestroyAtDest(bool abX)
        {
            mbDestroyAtDest = abX;
        }

        float GetVal()
        {
            return mfVal;
        }
        tFlag GetTypes()
        {
            return mTypes;
        }
        float GetSpeed()
        {
            return mfSpeed;
        }
        float GetDest()
        {
            return mfDest;
        }
        bool GetDestroyAtDest()
        {
            return mbDestroyAtDest;
        }

    private:
        float mfVal;
        float mfDest;
        float mfSpeed;
        tFlag mTypes;
        bool mbDestroyAtDest;
    };

    typedef std::vector<cGSEntry *> tEntryVec;
    typedef tEntryVec::iterator tEntryVecIt;

public:
    cGSEntry *GetEntry(int alIdx, bool abForceCreation = true);
    int CreateEntry();
    void DestroyEntry(int alIdx);
    bool EntryExists(int alIdx)
    {
        return mvEntries[alIdx] != NULL;
    }

    float CalcResults(tFlag mTypes, eMultipleSettingsCalcType aCalcType, float afDefault);

    void Update(double adFixedDelta);

private:
    tEntryVec mvEntries;
};

//---------------------------------------


class cScriptVar : public iSerializable
{
    kSerializableClassInit(cScriptVar)
public:
    cScriptVar();

    tString msName;
    int mlVal;
};

typedef std::map<tString, cScriptVar> tScriptVarMap;
typedef tScriptVarMap::iterator tScriptVarMapIt;

//---------------------------------------

};

#endif // HPL_ENGINE_TYPES_H
