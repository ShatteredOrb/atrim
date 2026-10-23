#ifndef HPL_INPUT_H
#define HPL_INPUT_H

#include <map>
#include <list>

#include "system/SystemTypes.h"
#include "engine/Updateable.h"
#include "input/InputTypes.h"

namespace hpl
{

class iKeyboard;
class iMouse;
#if USE_GAMEPAD
class iGamepad;
#endif
class iLowLevelInput;
class iInputDevice;
class cAction;
class iSubAction;

typedef std::map<tString, cAction *> tActionMap;
typedef tActionMap::iterator tActionMapIt;

typedef std::multimap<int, cAction *> tActionIdMap;
typedef tActionIdMap::iterator tActionIdMapIt;

typedef std::list<iInputDevice *> tInputDeviceList;
typedef tInputDeviceList::iterator tInputDeviceListIt;

#if USE_GAMEPAD
typedef std::list<iGamepad *> tGamepadList;
typedef tGamepadList::iterator tGamepadListIt;
#endif

class cInput : public iUpdateable
{
public:
    cInput(iLowLevelInput *apLowLevelInput);
    ~cInput();

    /**
     * Updates the input, called by cEngine
     */
    void Update(double adFixedDelta);

    /**
     * Called by the engine once per rendered frame, right before drawing.
     * Samples the newest mouse state (see iMouse::FrameUpdate).
     */
    void FrameUpdate();

    /**
     * Create and add a new action. The Name must be unique, and the Id is optional
     */
    cAction *CreateAction(const tString& asName, int alId = -1);

    /**
     * Check if an action is triggered.
     * \param asName
     * \return
     */
    bool IsTriggerd(const tString& asName);
    bool IsTriggerd(int alId);
    /**
     *
     * \param asName name of the action.
     * \return
     */
    bool WasTriggerd(const tString& asName);
    bool WasTriggerd(int alId);

    /**
     *
     * \param asName name of the action.
     * \return
     */
    bool BecameTriggerd(const tString& asName);
    bool BecameTriggerd(int alId);

    /**
    *
    * \param asName name of the action.
    * \return
    */
    bool DoubleTriggerd(const tString& asName, float afLimit);
    bool DoubleTriggerd(int alId, float afLimit);

    /**
     *
     * \return currently used keyboard
     */
    iKeyboard *GetKeyboard();

    /**
    *
    * \return currently used mouse
    */
    iMouse *GetMouse();

#if USE_GAMEPAD
    /**
    *
    * Updates list of plugged gamepads
    */
    void RefreshGamepads();

    /**
    *
    * \return number of available gamepads
    */
    int GetGamepadNum();

    /**
    *
    * \return gamepad in list
    */
    iGamepad *GetGamepad(int i);
#endif

    /**
    * Get action from map.
    * \param asName name of action.
    * \return Pointer to action, if not found NULL.
    */
    cAction *GetAction(const tString& asName);
    cAction *GetAction(int alId);

    /**
     * Destroys an action if it exists.
     */
    void DestroyAction(cAction *apAction);

    /**
     * Checks if any input is given.
     * \return true if any input is given, else false.
     */
    bool CheckForInput();

    /**
     * This resets all actions to the current way that
     */
    void ResetActionsToCurrentState();

    /**
     * Creates an action from the latest input made. Any action with the same name is destroyed.
     * \param &asName Name of action be be created.
     * \return NULL if no input was given.
     */
    iSubAction *InputToSubAction();

    bool isQuitMessagePosted();
    void resetQuitMessagePosted();

    iLowLevelInput *GetLowLevel()
    {
        return mpLowLevelInput;
    }
#if USE_GAMEPAD
    void AppDeviceWasPlugged();
    void AppDeviceWasRemoved();
#endif
private:

    tActionMap m_mapActions;
    tActionIdMap m_mapActionIds;
    tInputDeviceList mlstInputDevices;

    iLowLevelInput *mpLowLevelInput;

    iMouse *mpMouse;
    iKeyboard *mpKeyboard;
#if USE_GAMEPAD
    tGamepadList mlstGamepads;
#endif
};
};

#endif // HPL_INPUT_H
