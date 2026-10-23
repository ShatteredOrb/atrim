#ifndef LUX_INTERACT_CONNECTIONS_H
#define LUX_INTERACT_CONNECTIONS_H

//----------------------------------------------

#include "LuxBase.h"

//----------------------------------------------

class iLuxEntity;
class iLuxProp;
class cLuxProp_SwingDoor;
class cLuxProp_MoveObject;
class cLuxMap;

//----------------------------------------------

class iLuxInteractConnection;

class iLuxInteractConnection_SaveData : public iSerializable
{
    kSerializableClassInit(iLuxInteractConnection_SaveData)
public:
    virtual ~iLuxInteractConnection_SaveData() {}

    virtual iLuxInteractConnection *CreateConnection(cLuxMap *apMap) = 0;
    virtual void FromConnection(iLuxInteractConnection* apConnection);

    tString msName;
    bool mbInteractionOnly;
    int mlPropId;
    bool mbInvert;
    int mlStateUsed;

protected:
    iLuxProp *GetProp(int alId, cLuxMap *apMap);
};

//----------------------------------------------

class iLuxInteractConnection
{
    friend class iLuxInteractConnection_SaveData;
public:
    iLuxInteractConnection(const tString& asName, iLuxProp *apProp, bool abInvert, int alStatesUsed)
        : msName(asName), mpProp(apProp), mbInteractionOnly(true), mbInvert(abInvert), mlStateUsed(alStatesUsed) {}
    virtual ~iLuxInteractConnection() {}

    ///////////////
    //General
    virtual void Update(double adFixedDelta) = 0;
    virtual void UpdateProp(double adFixedDelta) = 0;

    ///////////////
    //Callbacks
    virtual void OnTurn(float afAngleAdd, float afT) = 0;
    virtual void OnLimit(int alState) = 0;

    ///////////////
    //Properties
    const tString &GetName()
    {
        return msName;
    }

    void SetInteractionOnly(bool abX)
    {
        mbInteractionOnly = abX;
    }
    bool GetInteractionOnly()
    {
        return mbInteractionOnly;
    }

    ///////////////
    //Save
    virtual iLuxInteractConnection_SaveData *CreateSaveData() = 0;

protected:
    tString msName;
    bool mbInteractionOnly;
    iLuxProp *mpProp;

    bool mbInvert;
    int mlStateUsed;
};

//----------------------------------------------

class cLuxInteractConnection_Rope_SaveData :  public iLuxInteractConnection_SaveData
{
    kSerializableClassInit(cLuxInteractConnection_Rope_SaveData)
public:
    iLuxInteractConnection *CreateConnection(cLuxMap *apMap);
    void FromConnection(iLuxInteractConnection* apConnection);

    int mlRopeId;
    float mfSpeedMul;
    float mfMinSpeed;
    float mfMaxSpeed;
};

//----------------------------------------------

class cLuxInteractConnection_Rope : public iLuxInteractConnection
{
    friend class cLuxInteractConnection_Rope_SaveData;
public:
    cLuxInteractConnection_Rope(const tString& asName, iLuxProp *apProp,  iPhysicsRope *apRope, float afSpeedMul, float afMinSpeed, float afMaxSpeed,
                                bool abInvert, int alStatesUsed);
    ~cLuxInteractConnection_Rope();

    ///////////////
    //General
    void Update(double adFixedDelta);
    void UpdateProp(double adFixedDelta);

    ///////////////
    //Callbacks
    void OnTurn(float afAngleAdd, float afT);
    void OnLimit(int alState);


    ///////////////
    //Script
    iLuxInteractConnection_SaveData *CreateSaveData();

private:
    iPhysicsRope *mpRope;

    float mfSpeedMul;
    float mfMinSpeed;
    float mfMaxSpeed;
};


//----------------------------------------------

class cLuxInteractConnection_MoveObject_SaveData :  public iLuxInteractConnection_SaveData
{
    kSerializableClassInit(cLuxInteractConnection_MoveObject_SaveData)
public:
    iLuxInteractConnection *CreateConnection(cLuxMap *apMap);
    void FromConnection(iLuxInteractConnection* apConnection);

    int mlMoveObjectId;
};
//----------------------------------------------

class cLuxInteractConnection_MoveObject : public iLuxInteractConnection
{
    friend class cLuxInteractConnection_MoveObject_SaveData;
public:

    /**
    * lStatesUsed, 0=max and min, 1=max only, -1=min only.
    */
    cLuxInteractConnection_MoveObject(const tString& asName, iLuxProp *apProp, cLuxProp_MoveObject *apMoveObject, bool abInvert, int alStatesUsed);
    ~cLuxInteractConnection_MoveObject();

    ///////////////
    //General
    void Update(double adFixedDelta);
    void UpdateProp(double adFixedDelta);

    ///////////////
    //Callbacks
    void OnTurn(float afAngleAdd, float afT);
    void OnLimit(int alState);

    ///////////////
    //Script
    iLuxInteractConnection_SaveData *CreateSaveData();

private:
    cLuxProp_MoveObject *mpMoveObject;
    bool mbPropNeedsUpdate;
};

//----------------------------------------------


#endif // LUX_INTERACT_CONNECTIONS_H
