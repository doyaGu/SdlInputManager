#ifndef SDLINPUTMANAGER_H
#define SDLINPUTMANAGER_H

#include "CKInputManager.h"

#include <SDL3/SDL.h>

#ifdef LoadImage
#undef LoadImage
#endif

#ifdef GetClassName
#undef GetClassName
#endif

#define KEYBOARD_BUFFER_SIZE 256
#define MOUSE_BUFFER_SIZE 256

// Joystick axis enumeration for configuration methods
enum CK_JOYSTICK_AXIS
{
    CK_AXIS_X = 0,
    CK_AXIS_Y = 1,
    CK_AXIS_Z = 2,
    CK_AXIS_RX = 3,
    CK_AXIS_RY = 4,
    CK_AXIS_RZ = 5,
    CK_AXIS_SLIDER0 = 6,
    CK_AXIS_SLIDER1 = 7
};

// Joystick capabilities flags
enum CK_JOYSTICK_CAPS
{
    CK_JOYSTICK_HAS_X = 0x00000001,
    CK_JOYSTICK_HAS_Y = 0x00000002,
    CK_JOYSTICK_HAS_Z = 0x00000004,
    CK_JOYSTICK_HAS_RX = 0x00000008,
    CK_JOYSTICK_HAS_RY = 0x00000010,
    CK_JOYSTICK_HAS_RZ = 0x00000020,
    CK_JOYSTICK_HAS_SLIDER0 = 0x00000040,
    CK_JOYSTICK_HAS_SLIDER1 = 0x00000080,
    CK_JOYSTICK_HAS_POV = 0x00000100,
    // Button count is stored in the upper bits (shifted by 16)
    CK_JOYSTICK_BUTTON_COUNT_SHIFT = 16,
    CK_JOYSTICK_BUTTON_COUNT_MASK = 0xFFFF0000
};

// Helper functions for CK_JOYSTICK_CAPS
inline int GetJoystickButtonCount(CKDWORD caps) { return (caps & CK_JOYSTICK_BUTTON_COUNT_MASK) >> CK_JOYSTICK_BUTTON_COUNT_SHIFT; }
inline CKBOOL HasJoystickX(CKDWORD caps) { return (caps & CK_JOYSTICK_HAS_X) != 0; }
inline CKBOOL HasJoystickY(CKDWORD caps) { return (caps & CK_JOYSTICK_HAS_Y) != 0; }
inline CKBOOL HasJoystickZ(CKDWORD caps) { return (caps & CK_JOYSTICK_HAS_Z) != 0; }
inline CKBOOL HasJoystickRx(CKDWORD caps) { return (caps & CK_JOYSTICK_HAS_RX) != 0; }
inline CKBOOL HasJoystickRy(CKDWORD caps) { return (caps & CK_JOYSTICK_HAS_RY) != 0; }
inline CKBOOL HasJoystickRz(CKDWORD caps) { return (caps & CK_JOYSTICK_HAS_RZ) != 0; }
inline CKBOOL HasJoystickSlider0(CKDWORD caps) { return (caps & CK_JOYSTICK_HAS_SLIDER0) != 0; }
inline CKBOOL HasJoystickSlider1(CKDWORD caps) { return (caps & CK_JOYSTICK_HAS_SLIDER1) != 0; }
inline CKBOOL HasJoystickPOV(CKDWORD caps) { return (caps & CK_JOYSTICK_HAS_POV) != 0; }

class SdlInputManager : public CKInputManager
{
public:
    // Keyboard buffer event for compatibility with DX8 behavior
    struct KeyboardEvent
    {
        CKDWORD dwOfs;       // Scancode / key index
        CKDWORD dwData;      // 0x80 = pressed, 0x00 = released
        CKDWORD dwTimeStamp; // Timestamp in milliseconds
    };

    // Mouse state structure (mirroring DirectInput DIMOUSESTATE)
    struct MouseState
    {
        long lX;           // Relative X movement
        long lY;           // Relative Y movement
        long lZ;           // Wheel movement
        CKBYTE rgbButtons[4]; // Button states
    };

    class CKMouse
    {
        friend SdlInputManager;

    public:
        CKMouse();
        void Init();
        void Release();
        void Clear();
        void Poll(CKBOOL pause);

    private:
        Vx2DVector m_Position;
        MouseState m_State;
        CKBYTE m_LastButtons[4];
        int m_NumberOfBuffer;
        CKBOOL m_Attached;
    };

    class CKGamepad
    {
        friend SdlInputManager;

    public:
        CKGamepad();
        void Init(SDL_Gamepad *gamepad, SDL_JoystickID id);
        void Release();
        void Poll();
        CKBOOL IsAttached();

    private:
        void ResetState();

        SDL_Gamepad *m_Gamepad;
        SDL_JoystickID m_InstanceID;
        char m_DeviceName[256];
        float m_DeadzoneRadius;
        float m_Gain;
        int m_ButtonCount;
        CKDWORD m_Polled;
        VxVector m_Position;
        VxVector m_Rotation;
        Vx2DVector m_Sliders;
        CKDWORD m_PointOfViewAngle;
        CKDWORD m_Buttons;
        CKDWORD m_ButtonsHigh;
        // Axis ranges (SDL gamepads use -32768 to 32767)
        CKDWORD m_Xmin, m_Xmax;
        CKDWORD m_Ymin, m_Ymax;
        CKDWORD m_Zmin, m_Zmax;
        CKDWORD m_XRmin, m_XRmax;
        CKDWORD m_YRmin, m_YRmax;
        CKDWORD m_ZRmin, m_ZRmax;
        CKDWORD m_Umin, m_Umax;
        CKDWORD m_Vmin, m_Vmax;
        // Axis capability flags
        CKBOOL m_HasX, m_HasY, m_HasZ;
        CKBOOL m_HasRx, m_HasRy, m_HasRz;
        CKBOOL m_HasSlider0, m_HasSlider1;
    };

    virtual void EnableKeyboardRepetition(CKBOOL iEnable = TRUE);
    virtual CKBOOL IsKeyboardRepetitionEnabled();

    virtual CKBOOL IsKeyDown(CKDWORD iKey, CKDWORD *oStamp = NULL);
    virtual CKBOOL IsKeyUp(CKDWORD iKey);
    virtual CKBOOL IsKeyToggled(CKDWORD iKey, CKDWORD *oStamp = NULL);

    virtual int GetKeyName(CKDWORD iKey, char *oKeyName);
    virtual CKDWORD GetKeyFromName(CKSTRING iKeyName);
    virtual unsigned char *GetKeyboardState();

    virtual CKBOOL IsKeyboardAttached();

    virtual int GetNumberOfKeyInBuffer();
    virtual int GetKeyFromBuffer(int i, CKDWORD &oKey, CKDWORD *oTimeStamp = NULL);

    virtual CKBOOL IsMouseButtonDown(CK_MOUSEBUTTON iButton);
    virtual CKBOOL IsMouseClicked(CK_MOUSEBUTTON iButton);
    virtual CKBOOL IsMouseToggled(CK_MOUSEBUTTON iButton);

    virtual void GetMouseButtonsState(CKBYTE oStates[4]);
    virtual void GetMousePosition(Vx2DVector &oPosition, CKBOOL iAbsolute = TRUE);
    virtual void GetMouseRelativePosition(VxVector &oPosition);

    virtual CKBOOL IsMouseAttached();

    virtual CKBOOL IsJoystickAttached(int iJoystick);

    virtual void GetJoystickPosition(int iJoystick, VxVector *oPosition);
    virtual void GetJoystickRotation(int iJoystick, VxVector *oRotation);
    virtual void GetJoystickSliders(int iJoystick, Vx2DVector *oPosition);
    virtual void GetJoystickPointOfViewAngle(int iJoystick, float *oAngle);
    virtual CKDWORD GetJoystickButtonsState(int iJoystick);
    virtual CKBOOL IsJoystickButtonDown(int iJoystick, int iButton);

    virtual void Pause(CKBOOL pause);

    virtual void ShowCursor(CKBOOL iShow);
    virtual CKBOOL GetCursorVisibility();
    virtual VXCURSOR_POINTER GetSystemCursor();
    virtual void SetSystemCursor(VXCURSOR_POINTER cursor);

    virtual int GetJoystickCount();
    virtual void *GetJoystickInterface(int iJoystick); // Returns SDL_Gamepad*

    virtual int GetMaxJoysticks();
    virtual void SetMaxJoysticks(int maxJoysticks);

    virtual const char *GetJoystickName(int iJoystick);
    virtual CKBOOL GetJoystickCapabilities(int iJoystick, CKDWORD *caps);

    // Joystick configuration methods
    virtual float GetJoystickDeadzone(int iJoystick);
    virtual void SetJoystickDeadzone(int iJoystick, float radius);

    virtual float GetJoystickGain(int iJoystick);
    virtual void SetJoystickGain(int iJoystick, float gain);

    virtual CKBOOL GetJoystickAxisRange(int iJoystick, CK_JOYSTICK_AXIS axis, CKDWORD *min, CKDWORD *max);
    virtual CKBOOL SetJoystickAxisRange(int iJoystick, CK_JOYSTICK_AXIS axis, CKDWORD min, CKDWORD max);
    virtual CKBOOL ResetJoystickAxisRanges(int iJoystick);

    // Keyboard repeat configuration methods
    virtual CKDWORD GetKeyboardRepeatDelay();
    virtual void SetKeyboardRepeatDelay(CKDWORD delay);
    virtual CKDWORD GetKeyboardRepeatInterval();
    virtual void SetKeyboardRepeatInterval(CKDWORD interval);

    // Mouse wheel methods
    virtual int GetMouseWheelDelta();
    virtual int GetMouseWheelPosition();

    // State setting methods
    virtual void SetKeyDown(CKDWORD iKey);
    virtual void SetKeyUp(CKDWORD iKey);
    virtual void SetMultipleKeys(const CKDWORD *keys, int count, CKBOOL pressed);

    virtual void SetMouseButtonDown(CK_MOUSEBUTTON iButton);
    virtual void SetMouseButtonUp(CK_MOUSEBUTTON iButton);
    virtual void SetMousePosition(const Vx2DVector &position);
    virtual void SetMouseWheel(int wheelDelta);
    virtual void SetMouseWheelPosition(int position);

    virtual void SetJoystickButtonDown(int iJoystick, int iButton);
    virtual void SetJoystickButtonUp(int iJoystick, int iButton);
    virtual void SetJoystickPosition(int iJoystick, const VxVector &position);
    virtual void SetJoystickRotation(int iJoystick, const VxVector &rotation);
    virtual void SetJoystickSliders(int iJoystick, const Vx2DVector &sliders);
    virtual void SetJoystickPOV(int iJoystick, float angle);

    // Advanced state setting methods
    virtual void SetKeyboardState(const CKBYTE *states, const int *stamps);
    virtual void SetMouseState(const Vx2DVector &pos, const CKBYTE *buttons, const VxVector &delta);
    virtual void SetJoystickState(int iJoystick, const VxVector &pos, const VxVector &rot, const Vx2DVector &sliders, CKDWORD buttons, CKDWORD pov);
    virtual void ClearKeyboardState();
    virtual void ClearMouseState();
    virtual void ClearJoystickState(int joystickIndex = -1);
    virtual void ClearInputState();

    // Internal functions
    virtual CKERROR OnCKInit();
    virtual CKERROR OnCKEnd();

    virtual CKERROR OnCKReset();
    virtual CKERROR OnCKPause();
    virtual CKERROR OnCKPlay();

    virtual CKERROR PreProcess();
    virtual CKERROR PostProcess();

    virtual CKDWORD GetValidFunctionsMask()
    {
        return CKMANAGER_FUNC_OnCKInit |
               CKMANAGER_FUNC_OnCKEnd |
               CKMANAGER_FUNC_OnCKReset |
               CKMANAGER_FUNC_OnCKPause |
               CKMANAGER_FUNC_OnCKPlay |
               CKMANAGER_FUNC_PreProcess |
               CKMANAGER_FUNC_PostProcess;
    }

    virtual ~SdlInputManager();

    SdlInputManager(CKContext *context);

    void Initialize();
    void Uninitialize();
    void ClearBuffers();
    void EnsureCursorVisible(CKBOOL iShow);

    // SDL scancode to DirectInput scancode mapping
    static CKDWORD SdlScancodeToVirtoolsKey(SDL_Scancode scancode);
    static SDL_Scancode VirtoolsKeyToSdlScancode(CKDWORD key);

protected:
    VXCURSOR_POINTER m_Cursor;
    CKMouse m_Mouse;
    CKGamepad *m_Gamepads;
    int m_GamepadCount;
    int m_MaxGamepads;
    CKBYTE m_KeyboardState[KEYBOARD_BUFFER_SIZE];
    int m_KeyboardStamps[KEYBOARD_BUFFER_SIZE];
    KeyboardEvent m_KeyInBuffer[KEYBOARD_BUFFER_SIZE];
    int m_NumberOfKeyInBuffer;
    CKBOOL m_Paused;
    CKBOOL m_WasPaused;
    CKBOOL m_EnableKeyboardRepetition;
    CKDWORD m_KeyboardRepeatDelay;
    CKDWORD m_KeyboardRepeatInterval;
    int m_MouseWheelPosition;
    CKBOOL m_ShowCursor;
    CKBOOL m_SdlInitialized;
    CKBOOL m_KeyboardAttached;

    // Relative mouse motion accumulator (reset each frame)
    float m_RelMouseX;
    float m_RelMouseY;
    float m_RelMouseWheel;
};

#endif // SDLINPUTMANAGER_H
