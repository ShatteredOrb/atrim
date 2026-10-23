#ifndef HPL_UPDATEABLE_H
#define HPL_UPDATEABLE_H

#include "engine/EngineTypes.h"
#include "system/SystemTypes.h"

namespace hpl
{

class iUpdateable
{
public:
    iUpdateable(const tString& asName) : msName(asName) {}
    virtual ~iUpdateable() {}

    virtual void OnPostBufferSwap() {}

    virtual void OnStart() {}

    virtual void OnDraw(double adFrameTime) {}

    /**
     * Called once every rendered frame, after the fixed updates and right before OnDraw,
     * with freshly sampled input. Use for things that must react at frame rate (mouse look, cursors).
     */
    virtual void OnFrameInput(double adFrameTime) {}

    virtual void PreUpdate(double adFixedDelta) {}
    virtual void Update(double adFixedDelta) {}
    virtual void PostUpdate(double adFixedDelta) {}

    virtual void OnPauseUpdate(double adFixedDelta) {}

    virtual void OnQuit() {}
    virtual void OnExit() {}

    virtual void Reset() {}

    virtual void OnEnterContainer(const tString& asOldContainer) {}
    virtual void OnLeaveContainer(const tString& asNewContainer) {}

    virtual void AppGotInputFocus() {}
    virtual void AppGotMouseFocus() {}
    virtual void AppGotVisibility() {}

    virtual void AppLostInputFocus() {}
    virtual void AppLostMouseFocus() {}
    virtual void AppLostVisibility() {}

    virtual void AppDeviceWasPlugged() {}
    virtual void AppDeviceWasRemoved() {}

    void RunMessage(eUpdateableMessage aMessage, double adX)
    {
        switch(aMessage)
        {
        case eUpdateableMessage_OnPostBufferSwap:
            OnPostBufferSwap();
            break;
        case eUpdateableMessage_OnStart:
            OnStart();
            break;
        case eUpdateableMessage_OnDraw:
            OnDraw(adX);
            break;
        case eUpdateableMessage_PreUpdate:
            PreUpdate(adX);
            break;
        case eUpdateableMessage_Update:
            Update(adX);
            break;
        case eUpdateableMessage_PostUpdate:
            PostUpdate(adX);
            break;
        case eUpdateableMessage_OnQuit:
            OnQuit();
            break;
        case eUpdateableMessage_OnExit:
            OnExit();
            break;
        case eUpdateableMessage_Reset:
            Reset();
            break;
        case eUpdateableMessage_OnPauseUpdate:
            OnPauseUpdate(adX);
            break;

        case eUpdateableMessage_AppGotInputFocus:
            AppGotInputFocus();
            break;
        case eUpdateableMessage_AppGotMouseFocus:
            AppGotMouseFocus();
            break;
        case eUpdateableMessage_AppGotVisibility:
            AppGotVisibility();
            break;

        case eUpdateableMessage_AppLostInputFocus:
            AppLostInputFocus();
            break;
        case eUpdateableMessage_AppLostMouseFocus:
            AppLostMouseFocus();
            break;
        case eUpdateableMessage_AppLostVisibility:
            AppLostVisibility();
            break;

        case eUpdateableMessage_AppDeviceWasPlugged:
            AppDeviceWasPlugged();
            break;
        case eUpdateableMessage_AppDeviceWasRemoved:
            AppDeviceWasRemoved();
            break;

        case eUpdateableMessage_OnFrameInput:
            OnFrameInput(adX);
            break;
        };
    }

    const tString &GetName()
    {
        return msName;
    }

private:
    tString msName;
};
};

#endif // HPL_UPDATEABLE_H
