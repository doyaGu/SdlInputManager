#include "SdlInputManager.h"

#include "CKAll.h"

#ifdef WIN32
#include <Windows.h>
#endif

// SDL scancode to Virtools key mapping table
// Virtools uses DirectInput scancodes (which match Windows virtual key scancodes)
// This table maps SDL3 scancodes to DirectInput scancodes

static const CKDWORD g_SdlToVirtoolsKeyMap[SDL_SCANCODE_COUNT] = {
    // Initialize all to 0 (no mapping)
    0
};

// Initialize the mapping table statically
static struct KeyMapInitializer {
    CKDWORD map[SDL_SCANCODE_COUNT];
    KeyMapInitializer() {
        memset(map, 0, sizeof(map));
        
        // Letters A-Z (DirectInput: 0x1E-0x32 for A-Z row by row)
        map[SDL_SCANCODE_A] = 0x1E;
        map[SDL_SCANCODE_B] = 0x30;
        map[SDL_SCANCODE_C] = 0x2E;
        map[SDL_SCANCODE_D] = 0x20;
        map[SDL_SCANCODE_E] = 0x12;
        map[SDL_SCANCODE_F] = 0x21;
        map[SDL_SCANCODE_G] = 0x22;
        map[SDL_SCANCODE_H] = 0x23;
        map[SDL_SCANCODE_I] = 0x17;
        map[SDL_SCANCODE_J] = 0x24;
        map[SDL_SCANCODE_K] = 0x25;
        map[SDL_SCANCODE_L] = 0x26;
        map[SDL_SCANCODE_M] = 0x32;
        map[SDL_SCANCODE_N] = 0x31;
        map[SDL_SCANCODE_O] = 0x18;
        map[SDL_SCANCODE_P] = 0x19;
        map[SDL_SCANCODE_Q] = 0x10;
        map[SDL_SCANCODE_R] = 0x13;
        map[SDL_SCANCODE_S] = 0x1F;
        map[SDL_SCANCODE_T] = 0x14;
        map[SDL_SCANCODE_U] = 0x16;
        map[SDL_SCANCODE_V] = 0x2F;
        map[SDL_SCANCODE_W] = 0x11;
        map[SDL_SCANCODE_X] = 0x2D;
        map[SDL_SCANCODE_Y] = 0x15;
        map[SDL_SCANCODE_Z] = 0x2C;

        // Numbers 1-0 (top row)
        map[SDL_SCANCODE_1] = 0x02;
        map[SDL_SCANCODE_2] = 0x03;
        map[SDL_SCANCODE_3] = 0x04;
        map[SDL_SCANCODE_4] = 0x05;
        map[SDL_SCANCODE_5] = 0x06;
        map[SDL_SCANCODE_6] = 0x07;
        map[SDL_SCANCODE_7] = 0x08;
        map[SDL_SCANCODE_8] = 0x09;
        map[SDL_SCANCODE_9] = 0x0A;
        map[SDL_SCANCODE_0] = 0x0B;

        // Function keys F1-F12
        map[SDL_SCANCODE_F1] = 0x3B;
        map[SDL_SCANCODE_F2] = 0x3C;
        map[SDL_SCANCODE_F3] = 0x3D;
        map[SDL_SCANCODE_F4] = 0x3E;
        map[SDL_SCANCODE_F5] = 0x3F;
        map[SDL_SCANCODE_F6] = 0x40;
        map[SDL_SCANCODE_F7] = 0x41;
        map[SDL_SCANCODE_F8] = 0x42;
        map[SDL_SCANCODE_F9] = 0x43;
        map[SDL_SCANCODE_F10] = 0x44;
        map[SDL_SCANCODE_F11] = 0x57;
        map[SDL_SCANCODE_F12] = 0x58;

        // Special keys
        map[SDL_SCANCODE_ESCAPE] = 0x01;
        map[SDL_SCANCODE_MINUS] = 0x0C;
        map[SDL_SCANCODE_EQUALS] = 0x0D;
        map[SDL_SCANCODE_BACKSPACE] = 0x0E;
        map[SDL_SCANCODE_TAB] = 0x0F;
        map[SDL_SCANCODE_LEFTBRACKET] = 0x1A;
        map[SDL_SCANCODE_RIGHTBRACKET] = 0x1B;
        map[SDL_SCANCODE_RETURN] = 0x1C;
        map[SDL_SCANCODE_LCTRL] = 0x1D;
        map[SDL_SCANCODE_SEMICOLON] = 0x27;
        map[SDL_SCANCODE_APOSTROPHE] = 0x28;
        map[SDL_SCANCODE_GRAVE] = 0x29;
        map[SDL_SCANCODE_LSHIFT] = 0x2A;
        map[SDL_SCANCODE_BACKSLASH] = 0x2B;
        map[SDL_SCANCODE_COMMA] = 0x33;
        map[SDL_SCANCODE_PERIOD] = 0x34;
        map[SDL_SCANCODE_SLASH] = 0x35;
        map[SDL_SCANCODE_RSHIFT] = 0x36;
        map[SDL_SCANCODE_KP_MULTIPLY] = 0x37;
        map[SDL_SCANCODE_LALT] = 0x38;
        map[SDL_SCANCODE_SPACE] = 0x39;
        map[SDL_SCANCODE_CAPSLOCK] = 0x3A;

        // Numpad
        map[SDL_SCANCODE_NUMLOCKCLEAR] = 0x45;
        map[SDL_SCANCODE_SCROLLLOCK] = 0x46;
        map[SDL_SCANCODE_KP_7] = 0x47;
        map[SDL_SCANCODE_KP_8] = 0x48;
        map[SDL_SCANCODE_KP_9] = 0x49;
        map[SDL_SCANCODE_KP_MINUS] = 0x4A;
        map[SDL_SCANCODE_KP_4] = 0x4B;
        map[SDL_SCANCODE_KP_5] = 0x4C;
        map[SDL_SCANCODE_KP_6] = 0x4D;
        map[SDL_SCANCODE_KP_PLUS] = 0x4E;
        map[SDL_SCANCODE_KP_1] = 0x4F;
        map[SDL_SCANCODE_KP_2] = 0x50;
        map[SDL_SCANCODE_KP_3] = 0x51;
        map[SDL_SCANCODE_KP_0] = 0x52;
        map[SDL_SCANCODE_KP_PERIOD] = 0x53;

        // Extended keys (with 0x80 offset for some)
        map[SDL_SCANCODE_KP_ENTER] = 0x9C;      // DIK_NUMPADENTER
        map[SDL_SCANCODE_RCTRL] = 0x9D;         // DIK_RCONTROL
        map[SDL_SCANCODE_KP_DIVIDE] = 0xB5;     // DIK_DIVIDE
        map[SDL_SCANCODE_PRINTSCREEN] = 0xB7;   // DIK_SYSRQ
        map[SDL_SCANCODE_RALT] = 0xB8;          // DIK_RMENU
        map[SDL_SCANCODE_PAUSE] = 0xC5;         // DIK_PAUSE
        map[SDL_SCANCODE_HOME] = 0xC7;          // DIK_HOME
        map[SDL_SCANCODE_UP] = 0xC8;            // DIK_UP
        map[SDL_SCANCODE_PAGEUP] = 0xC9;        // DIK_PRIOR
        map[SDL_SCANCODE_LEFT] = 0xCB;          // DIK_LEFT
        map[SDL_SCANCODE_RIGHT] = 0xCD;         // DIK_RIGHT
        map[SDL_SCANCODE_END] = 0xCF;           // DIK_END
        map[SDL_SCANCODE_DOWN] = 0xD0;          // DIK_DOWN
        map[SDL_SCANCODE_PAGEDOWN] = 0xD1;      // DIK_NEXT
        map[SDL_SCANCODE_INSERT] = 0xD2;        // DIK_INSERT
        map[SDL_SCANCODE_DELETE] = 0xD3;        // DIK_DELETE
        map[SDL_SCANCODE_LGUI] = 0xDB;          // DIK_LWIN
        map[SDL_SCANCODE_RGUI] = 0xDC;          // DIK_RWIN
        map[SDL_SCANCODE_APPLICATION] = 0xDD;   // DIK_APPS
    }
} g_KeyMapInit;

// Reverse mapping for Virtools key to SDL scancode
static SDL_Scancode g_VirtoolsToSdlKeyMap[256] = { SDL_SCANCODE_UNKNOWN };

static struct ReverseKeyMapInitializer {
    ReverseKeyMapInitializer() {
        memset(g_VirtoolsToSdlKeyMap, 0, sizeof(g_VirtoolsToSdlKeyMap));
        for (int i = 0; i < SDL_SCANCODE_COUNT; i++) {
            CKDWORD vk = g_KeyMapInit.map[i];
            if (vk > 0 && vk < 256) {
                g_VirtoolsToSdlKeyMap[vk] = (SDL_Scancode)i;
            }
        }
    }
} g_ReverseKeyMapInit;

CKDWORD SdlInputManager::SdlScancodeToVirtoolsKey(SDL_Scancode scancode)
{
    if (scancode >= 0 && scancode < SDL_SCANCODE_COUNT)
        return g_KeyMapInit.map[scancode];
    return 0;
}

SDL_Scancode SdlInputManager::VirtoolsKeyToSdlScancode(CKDWORD key)
{
    if (key < 256)
        return g_VirtoolsToSdlKeyMap[key];
    return SDL_SCANCODE_UNKNOWN;
}

void SdlInputManager::EnableKeyboardRepetition(CKBOOL iEnable)
{
    m_EnableKeyboardRepetition = iEnable;
}

CKBOOL SdlInputManager::IsKeyboardRepetitionEnabled()
{
    return m_EnableKeyboardRepetition;
}

CKBOOL SdlInputManager::IsKeyDown(CKDWORD iKey, CKDWORD *oStamp)
{
    if (iKey >= KEYBOARD_BUFFER_SIZE)
        return FALSE;
    if ((m_KeyboardState[iKey] & KS_PRESSED) == 0)
        return FALSE;
    if (oStamp)
        *oStamp = m_KeyboardStamps[iKey];
    return TRUE;
}

CKBOOL SdlInputManager::IsKeyUp(CKDWORD iKey)
{
    return iKey < KEYBOARD_BUFFER_SIZE && m_KeyboardState[iKey] == KS_IDLE;
}

CKBOOL SdlInputManager::IsKeyToggled(CKDWORD iKey, CKDWORD *oStamp)
{
    if (iKey >= KEYBOARD_BUFFER_SIZE)
        return FALSE;
    if ((m_KeyboardState[iKey] & KS_RELEASED) == 0)
        return FALSE;
    if (oStamp)
        *oStamp = m_KeyboardStamps[iKey];
    return TRUE;
}

int SdlInputManager::GetKeyName(CKDWORD iKey, char *oKeyName)
{
    return VxScanCodeToName(iKey, oKeyName);
}

CKDWORD SdlInputManager::GetKeyFromName(CKSTRING iKeyName)
{
    char keyName[32];
    CKDWORD iKey;
    for (iKey = 0; iKey < KEYBOARD_BUFFER_SIZE; iKey++)
        if (GetKeyName(iKey, keyName) != 0 && stricmp(keyName, iKeyName) == 0)
            break;
    return iKey;
}

unsigned char *SdlInputManager::GetKeyboardState()
{
    return m_KeyboardState;
}

CKBOOL SdlInputManager::IsKeyboardAttached()
{
    return m_KeyboardAttached;
}

int SdlInputManager::GetNumberOfKeyInBuffer()
{
    return m_NumberOfKeyInBuffer;
}

int SdlInputManager::GetKeyFromBuffer(int i, CKDWORD &oKey, CKDWORD *oTimeStamp)
{
    if (i >= m_NumberOfKeyInBuffer)
        return 0;
    oKey = m_KeyInBuffer[i].dwOfs;
    if (oTimeStamp)
        *oTimeStamp = m_KeyInBuffer[i].dwTimeStamp;
    return (m_KeyInBuffer[i].dwData & 0x80) ? KS_PRESSED : KS_RELEASED;
}

CKBOOL SdlInputManager::IsMouseButtonDown(CK_MOUSEBUTTON iButton)
{
    return m_Mouse.m_State.rgbButtons[iButton] & KS_PRESSED;
}

CKBOOL SdlInputManager::IsMouseClicked(CK_MOUSEBUTTON iButton)
{
    return (m_Mouse.m_State.rgbButtons[iButton] & KS_PRESSED) != 0 && (m_Mouse.m_LastButtons[iButton] & KS_PRESSED) == 0;
}

CKBOOL SdlInputManager::IsMouseToggled(CK_MOUSEBUTTON iButton)
{
    return (m_Mouse.m_State.rgbButtons[iButton] >> 1) & KS_PRESSED;
}

void SdlInputManager::GetMouseButtonsState(CKBYTE *oStates)
{
    *(CKDWORD *)oStates = *(CKDWORD *)(m_Mouse.m_State.rgbButtons);
}

void SdlInputManager::GetMousePosition(Vx2DVector &oPosition, CKBOOL iAbsolute)
{
    if (iAbsolute)
    {
        oPosition = m_Mouse.m_Position;
    }
    else
    {
        CKRenderContext *rc = m_Context->GetPlayerRenderContext();
        if (rc)
        {
            VxRect rect;
            rc->GetWindowRect(rect, TRUE);
            oPosition.Set(m_Mouse.m_Position.x - rect.left, m_Mouse.m_Position.y - rect.top);
        }
        else
        {
            oPosition = m_Mouse.m_Position;
        }
    }
}

void SdlInputManager::GetMouseRelativePosition(VxVector &oPosition)
{
    oPosition.Set((float)m_Mouse.m_State.lX, (float)m_Mouse.m_State.lY, (float)m_Mouse.m_State.lZ);
}

CKBOOL SdlInputManager::IsMouseAttached()
{
    return m_Mouse.m_Attached;
}

CKBOOL SdlInputManager::IsJoystickAttached(int iJoystick)
{
    return 0 <= iJoystick && iJoystick < m_GamepadCount && m_Gamepads[iJoystick].IsAttached();
}

void SdlInputManager::GetJoystickPosition(int iJoystick, VxVector *oPosition)
{
    if (iJoystick < 0 || iJoystick >= m_GamepadCount)
        return;

    if (oPosition)
    {
        CKGamepad *gamepad = &m_Gamepads[iJoystick];
        gamepad->Poll();
        *oPosition = gamepad->m_Position;
    }
}

void SdlInputManager::GetJoystickRotation(int iJoystick, VxVector *oRotation)
{
    if (iJoystick < 0 || iJoystick >= m_GamepadCount)
        return;

    if (oRotation)
    {
        CKGamepad *gamepad = &m_Gamepads[iJoystick];
        gamepad->Poll();
        *oRotation = gamepad->m_Rotation;
    }
}

void SdlInputManager::GetJoystickSliders(int iJoystick, Vx2DVector *oPosition)
{
    if (iJoystick < 0 || iJoystick >= m_GamepadCount)
        return;

    if (oPosition)
    {
        CKGamepad *gamepad = &m_Gamepads[iJoystick];
        gamepad->Poll();
        *oPosition = gamepad->m_Sliders;
    }
}

void SdlInputManager::GetJoystickPointOfViewAngle(int iJoystick, float *oAngle)
{
    if (iJoystick < 0 || iJoystick >= m_GamepadCount)
        return;

    if (oAngle)
    {
        CKGamepad *gamepad = &m_Gamepads[iJoystick];
        gamepad->Poll();
        *oAngle = (gamepad->m_PointOfViewAngle == (CKDWORD)-1)
                      ? -1.0f
                      : (float)(gamepad->m_PointOfViewAngle * PI * 0.000055555556);
    }
}

CKDWORD SdlInputManager::GetJoystickButtonsState(int iJoystick)
{
    if (iJoystick < 0 || iJoystick >= m_GamepadCount)
        return 0;

    CKGamepad *gamepad = &m_Gamepads[iJoystick];
    gamepad->Poll();
    return gamepad->m_Buttons;
}

CKBOOL SdlInputManager::IsJoystickButtonDown(int iJoystick, int iButton)
{
    if (iJoystick < 0 || iJoystick >= m_GamepadCount)
        return FALSE;

    CKGamepad *gamepad = &m_Gamepads[iJoystick];
    gamepad->Poll();
    return (gamepad->m_Buttons & (1 << iButton)) != 0;
}

void SdlInputManager::Pause(CKBOOL pause)
{
    if (pause && !m_Paused)
    {
        ClearBuffers();
        m_Paused = pause;
        m_WasPaused = TRUE;
        return;
    }

    m_Paused = pause;
}

int SdlInputManager::GetJoystickCount()
{
    return m_GamepadCount;
}

void *SdlInputManager::GetJoystickInterface(int iJoystick)
{
    if (iJoystick < 0 || iJoystick >= m_GamepadCount)
        return NULL;
    else
        return m_Gamepads[iJoystick].m_Gamepad;
}

int SdlInputManager::GetMaxJoysticks()
{
    return m_MaxGamepads;
}

void SdlInputManager::SetMaxJoysticks(int maxJoysticks)
{
    if (m_Gamepads != NULL)
        return;

    if (maxJoysticks < 1)
        maxJoysticks = 1;
    if (maxJoysticks > 16)
        maxJoysticks = 16;

    m_MaxGamepads = maxJoysticks;
}

const char *SdlInputManager::GetJoystickName(int iJoystick)
{
    if (iJoystick < 0 || iJoystick >= m_GamepadCount)
        return "Unknown";

    if (m_Gamepads[iJoystick].m_DeviceName[0] != '\0')
        return m_Gamepads[iJoystick].m_DeviceName;

    return "Game Controller";
}

CKBOOL SdlInputManager::GetJoystickCapabilities(int iJoystick, CKDWORD *caps)
{
    if (iJoystick < 0 || iJoystick >= m_GamepadCount)
        return FALSE;

    if (!caps)
        return FALSE;

    CKGamepad &gamepad = m_Gamepads[iJoystick];

    *caps = 0;
    *caps |= ((gamepad.m_ButtonCount & 0xFFFF) << CK_JOYSTICK_BUTTON_COUNT_SHIFT);

    if (gamepad.m_HasX)
        *caps |= CK_JOYSTICK_HAS_X;
    if (gamepad.m_HasY)
        *caps |= CK_JOYSTICK_HAS_Y;
    if (gamepad.m_HasZ)
        *caps |= CK_JOYSTICK_HAS_Z;
    if (gamepad.m_HasRx)
        *caps |= CK_JOYSTICK_HAS_RX;
    if (gamepad.m_HasRy)
        *caps |= CK_JOYSTICK_HAS_RY;
    if (gamepad.m_HasRz)
        *caps |= CK_JOYSTICK_HAS_RZ;
    if (gamepad.m_HasSlider0)
        *caps |= CK_JOYSTICK_HAS_SLIDER0;
    if (gamepad.m_HasSlider1)
        *caps |= CK_JOYSTICK_HAS_SLIDER1;

    // SDL gamepads always have D-pad which we map to POV
    *caps |= CK_JOYSTICK_HAS_POV;

    return TRUE;
}

float SdlInputManager::GetJoystickDeadzone(int iJoystick)
{
    if (iJoystick >= 0 && iJoystick < m_GamepadCount)
        return m_Gamepads[iJoystick].m_DeadzoneRadius;
    return 0.01f;
}

void SdlInputManager::SetJoystickDeadzone(int iJoystick, float radius)
{
    if (iJoystick < 0 || iJoystick >= m_GamepadCount)
        return;

    if (radius < 0.0f)
        radius = 0.0f;
    if (radius > 1.0f)
        radius = 1.0f;

    m_Gamepads[iJoystick].m_DeadzoneRadius = radius;
}

float SdlInputManager::GetJoystickGain(int iJoystick)
{
    if (iJoystick >= 0 && iJoystick < m_GamepadCount)
        return m_Gamepads[iJoystick].m_Gain;
    return 1.0f;
}

void SdlInputManager::SetJoystickGain(int iJoystick, float gain)
{
    if (iJoystick < 0 || iJoystick >= m_GamepadCount)
        return;

    if (gain < 0.0f)
        gain = 0.0f;
    if (gain > 2.0f)
        gain = 2.0f;

    m_Gamepads[iJoystick].m_Gain = gain;
}

CKBOOL SdlInputManager::GetJoystickAxisRange(int iJoystick, CK_JOYSTICK_AXIS axis, CKDWORD *min, CKDWORD *max)
{
    if (iJoystick < 0 || iJoystick >= m_GamepadCount)
        return FALSE;

    if (!min || !max)
        return FALSE;

    CKGamepad &gamepad = m_Gamepads[iJoystick];

    switch (axis)
    {
    case CK_AXIS_X:
        *min = gamepad.m_Xmin;
        *max = gamepad.m_Xmax;
        break;
    case CK_AXIS_Y:
        *min = gamepad.m_Ymin;
        *max = gamepad.m_Ymax;
        break;
    case CK_AXIS_Z:
        *min = gamepad.m_Zmin;
        *max = gamepad.m_Zmax;
        break;
    case CK_AXIS_RX:
        *min = gamepad.m_XRmin;
        *max = gamepad.m_XRmax;
        break;
    case CK_AXIS_RY:
        *min = gamepad.m_YRmin;
        *max = gamepad.m_YRmax;
        break;
    case CK_AXIS_RZ:
        *min = gamepad.m_ZRmin;
        *max = gamepad.m_ZRmax;
        break;
    case CK_AXIS_SLIDER0:
        *min = gamepad.m_Umin;
        *max = gamepad.m_Umax;
        break;
    case CK_AXIS_SLIDER1:
        *min = gamepad.m_Vmin;
        *max = gamepad.m_Vmax;
        break;
    default:
        return FALSE;
    }

    return TRUE;
}

CKBOOL SdlInputManager::SetJoystickAxisRange(int iJoystick, CK_JOYSTICK_AXIS axis, CKDWORD min, CKDWORD max)
{
    if (iJoystick < 0 || iJoystick >= m_GamepadCount)
        return FALSE;

    if (min >= max)
        return FALSE;

    CKGamepad &gamepad = m_Gamepads[iJoystick];

    switch (axis)
    {
    case CK_AXIS_X:
        gamepad.m_Xmin = min;
        gamepad.m_Xmax = max;
        break;
    case CK_AXIS_Y:
        gamepad.m_Ymin = min;
        gamepad.m_Ymax = max;
        break;
    case CK_AXIS_Z:
        gamepad.m_Zmin = min;
        gamepad.m_Zmax = max;
        break;
    case CK_AXIS_RX:
        gamepad.m_XRmin = min;
        gamepad.m_XRmax = max;
        break;
    case CK_AXIS_RY:
        gamepad.m_YRmin = min;
        gamepad.m_YRmax = max;
        break;
    case CK_AXIS_RZ:
        gamepad.m_ZRmin = min;
        gamepad.m_ZRmax = max;
        break;
    case CK_AXIS_SLIDER0:
        gamepad.m_Umin = min;
        gamepad.m_Umax = max;
        break;
    case CK_AXIS_SLIDER1:
        gamepad.m_Vmin = min;
        gamepad.m_Vmax = max;
        break;
    default:
        return FALSE;
    }

    return TRUE;
}

CKBOOL SdlInputManager::ResetJoystickAxisRanges(int iJoystick)
{
    if (iJoystick < 0 || iJoystick >= m_GamepadCount)
        return FALSE;

    CKGamepad &gamepad = m_Gamepads[iJoystick];

    // SDL gamepads use -32768 to 32767 range
    gamepad.m_Xmin = gamepad.m_Ymin = gamepad.m_Zmin = -32768;
    gamepad.m_Xmax = gamepad.m_Ymax = gamepad.m_Zmax = 32767;
    gamepad.m_XRmin = gamepad.m_YRmin = gamepad.m_ZRmin = -32768;
    gamepad.m_XRmax = gamepad.m_YRmax = gamepad.m_ZRmax = 32767;
    gamepad.m_Umin = gamepad.m_Vmin = 0;
    gamepad.m_Umax = gamepad.m_Vmax = 32767;

    return TRUE;
}

CKDWORD SdlInputManager::GetKeyboardRepeatDelay()
{
    return m_KeyboardRepeatDelay;
}

void SdlInputManager::SetKeyboardRepeatDelay(CKDWORD delay)
{
    m_KeyboardRepeatDelay = delay;
}

CKDWORD SdlInputManager::GetKeyboardRepeatInterval()
{
    return m_KeyboardRepeatInterval;
}

void SdlInputManager::SetKeyboardRepeatInterval(CKDWORD interval)
{
    m_KeyboardRepeatInterval = interval;
}

int SdlInputManager::GetMouseWheelDelta()
{
    return m_Mouse.m_State.lZ;
}

int SdlInputManager::GetMouseWheelPosition()
{
    return m_MouseWheelPosition;
}

void SdlInputManager::SetKeyDown(CKDWORD iKey)
{
    if (iKey < KEYBOARD_BUFFER_SIZE)
    {
        m_KeyboardState[iKey] |= KS_PRESSED;
        m_KeyboardStamps[iKey] = (int)SDL_GetTicks();
    }
}

void SdlInputManager::SetKeyUp(CKDWORD iKey)
{
    if (iKey < KEYBOARD_BUFFER_SIZE)
    {
        m_KeyboardState[iKey] |= KS_RELEASED;
        m_KeyboardStamps[iKey] = (int)SDL_GetTicks();
    }
}

void SdlInputManager::SetMultipleKeys(const CKDWORD *keys, int count, CKBOOL pressed)
{
    if (!keys)
        return;

    CKDWORD ticks = (CKDWORD)SDL_GetTicks();
    for (int i = 0; i < count; i++)
    {
        CKDWORD key = keys[i];
        if (key < KEYBOARD_BUFFER_SIZE)
        {
            if (pressed)
            {
                m_KeyboardState[key] |= KS_PRESSED;
                m_KeyboardStamps[key] = ticks;
            }
            else
            {
                m_KeyboardState[key] |= KS_RELEASED;
                m_KeyboardStamps[key] = ticks;
            }
        }
    }
}

void SdlInputManager::SetMouseButtonDown(CK_MOUSEBUTTON iButton)
{
    if (iButton < 4)
    {
        m_Mouse.m_State.rgbButtons[iButton] |= KS_PRESSED;
    }
}

void SdlInputManager::SetMouseButtonUp(CK_MOUSEBUTTON iButton)
{
    if (iButton < 4)
    {
        m_Mouse.m_State.rgbButtons[iButton] |= KS_RELEASED;
    }
}

void SdlInputManager::SetMousePosition(const Vx2DVector &position)
{
    m_Mouse.m_Position = position;
#ifdef WIN32
    ::SetCursorPos((int)position.x, (int)position.y);
#else
    // SDL_WarpMouseGlobal requires a window, use platform-specific if needed
    SDL_WarpMouseGlobal(position.x, position.y);
#endif
}

void SdlInputManager::SetMouseWheel(int wheelDelta)
{
    m_Mouse.m_State.lZ = wheelDelta;
    m_MouseWheelPosition += wheelDelta;
}

void SdlInputManager::SetMouseWheelPosition(int position)
{
    int delta = position - m_MouseWheelPosition;
    m_Mouse.m_State.lZ = delta;
    m_MouseWheelPosition = position;
}

void SdlInputManager::SetJoystickButtonDown(int iJoystick, int iButton)
{
    if (iJoystick >= 0 && iJoystick < m_GamepadCount && iButton >= 0 && iButton < 32)
    {
        m_Gamepads[iJoystick].m_Buttons |= (1 << iButton);
        m_Gamepads[iJoystick].m_Polled = TRUE;
    }
}

void SdlInputManager::SetJoystickButtonUp(int iJoystick, int iButton)
{
    if (iJoystick >= 0 && iJoystick < m_GamepadCount && iButton >= 0 && iButton < 32)
    {
        m_Gamepads[iJoystick].m_Buttons &= ~(1 << iButton);
        m_Gamepads[iJoystick].m_Polled = TRUE;
    }
}

void SdlInputManager::SetJoystickPosition(int iJoystick, const VxVector &position)
{
    if (iJoystick >= 0 && iJoystick < m_GamepadCount)
    {
        m_Gamepads[iJoystick].m_Position = position;
        m_Gamepads[iJoystick].m_Polled = TRUE;
    }
}

void SdlInputManager::SetJoystickRotation(int iJoystick, const VxVector &rotation)
{
    if (iJoystick >= 0 && iJoystick < m_GamepadCount)
    {
        m_Gamepads[iJoystick].m_Rotation = rotation;
        m_Gamepads[iJoystick].m_Polled = TRUE;
    }
}

void SdlInputManager::SetJoystickSliders(int iJoystick, const Vx2DVector &sliders)
{
    if (iJoystick >= 0 && iJoystick < m_GamepadCount)
    {
        m_Gamepads[iJoystick].m_Sliders = sliders;
        m_Gamepads[iJoystick].m_Polled = TRUE;
    }
}

void SdlInputManager::SetJoystickPOV(int iJoystick, float angle)
{
    if (iJoystick >= 0 && iJoystick < m_GamepadCount)
    {
        if (angle < 0.0f)
        {
            m_Gamepads[iJoystick].m_PointOfViewAngle = (CKDWORD)-1;
        }
        else
        {
            float degrees = angle * 180.0f / PI;
            m_Gamepads[iJoystick].m_PointOfViewAngle = (CKDWORD)(degrees * 100.0f);
        }
        m_Gamepads[iJoystick].m_Polled = TRUE;
    }
}

void SdlInputManager::SetKeyboardState(const CKBYTE *states, const int *stamps)
{
    if (states)
    {
        memcpy(m_KeyboardState, states, sizeof(m_KeyboardState));
        if (stamps)
            memcpy(m_KeyboardStamps, stamps, sizeof(m_KeyboardStamps));
    }
}

void SdlInputManager::SetMouseState(const Vx2DVector &pos, const CKBYTE *buttons, const VxVector &delta)
{
    m_Mouse.m_Position = pos;
    if (buttons)
        memcpy(m_Mouse.m_State.rgbButtons, buttons, 4);
    m_Mouse.m_State.lX = (long)delta.x;
    m_Mouse.m_State.lY = (long)delta.y;
    m_Mouse.m_State.lZ = (long)delta.z;
}

void SdlInputManager::SetJoystickState(int iJoystick, const VxVector &pos, const VxVector &rot, const Vx2DVector &sliders, CKDWORD buttons, CKDWORD pov)
{
    if (iJoystick >= 0 && iJoystick < m_GamepadCount)
    {
        m_Gamepads[iJoystick].m_Position = pos;
        m_Gamepads[iJoystick].m_Rotation = rot;
        m_Gamepads[iJoystick].m_Sliders = sliders;
        m_Gamepads[iJoystick].m_Buttons = buttons;
        m_Gamepads[iJoystick].m_PointOfViewAngle = (pov == 0xFFFFFFFF) ? (CKDWORD)-1 : pov;
        m_Gamepads[iJoystick].m_Polled = TRUE;
    }
}

void SdlInputManager::ClearKeyboardState()
{
    memset(m_KeyboardState, 0, sizeof(m_KeyboardState));
    memset(m_KeyboardStamps, 0, sizeof(m_KeyboardStamps));
}

void SdlInputManager::ClearMouseState()
{
    memset(m_Mouse.m_State.rgbButtons, 0, sizeof(m_Mouse.m_State.rgbButtons));
    memset(m_Mouse.m_LastButtons, 0, sizeof(m_Mouse.m_LastButtons));
}

void SdlInputManager::ClearJoystickState(int joystickIndex)
{
    if (joystickIndex >= 0 && joystickIndex < m_GamepadCount)
    {
        m_Gamepads[joystickIndex].m_Buttons = 0;
        m_Gamepads[joystickIndex].m_Position.Set(0, 0, 0);
        m_Gamepads[joystickIndex].m_Rotation.Set(0, 0, 0);
        m_Gamepads[joystickIndex].m_Sliders.Set(0, 0);
        m_Gamepads[joystickIndex].m_PointOfViewAngle = (CKDWORD)-1;
    }
    else
    {
        for (int i = 0; i < m_GamepadCount; i++)
        {
            m_Gamepads[i].m_Buttons = 0;
            m_Gamepads[i].m_Position.Set(0, 0, 0);
            m_Gamepads[i].m_Rotation.Set(0, 0, 0);
            m_Gamepads[i].m_Sliders.Set(0, 0);
            m_Gamepads[i].m_PointOfViewAngle = (CKDWORD)-1;
        }
    }
}

void SdlInputManager::ClearInputState()
{
    ClearKeyboardState();
    ClearMouseState();
    ClearJoystickState();
}

CKERROR SdlInputManager::OnCKInit()
{
    if (!m_SdlInitialized)
        Initialize();
    return CK_OK;
}

CKERROR SdlInputManager::OnCKEnd()
{
    Uninitialize();
    return CK_OK;
}

CKERROR SdlInputManager::OnCKReset()
{
    m_ShowCursor = TRUE;
    ClearBuffers();
    return CK_OK;
}

CKERROR SdlInputManager::OnCKPause()
{
    if (!m_ShowCursor)
        EnsureCursorVisible(TRUE);
    return CK_OK;
}

CKERROR SdlInputManager::OnCKPlay()
{
    m_Mouse.Poll(m_Paused);

    memset(m_KeyboardState, 0, sizeof(m_KeyboardState));
    m_NumberOfKeyInBuffer = 0;

    // Pump SDL events to clear any queued input
    SDL_PumpEvents();
    SDL_FlushEvents(SDL_EVENT_KEY_DOWN, SDL_EVENT_KEY_UP);

    if (!m_ShowCursor)
        EnsureCursorVisible(FALSE);

    return CK_OK;
}

CKERROR SdlInputManager::PreProcess()
{
    // Pump SDL events to update input state
    SDL_PumpEvents();

    // Reset relative motion accumulators
    m_RelMouseX = 0;
    m_RelMouseY = 0;
    m_RelMouseWheel = 0;

    // Process only input events here. Window/quit/display events belong to the
    // player window loop and draining the full SDL queue can keep CK processing
    // from returning on some platforms.
    SDL_Event event;
    m_NumberOfKeyInBuffer = 0;

    // Snapshot previous button state before applying this frame's mouse events.
    m_Mouse.Poll(m_Paused);

    const Uint32 eventRanges[][2] = {
        { SDL_EVENT_KEY_DOWN, SDL_EVENT_SCREEN_KEYBOARD_HIDDEN },
        { SDL_EVENT_MOUSE_MOTION, SDL_EVENT_MOUSE_REMOVED },
        { SDL_EVENT_GAMEPAD_AXIS_MOTION, SDL_EVENT_GAMEPAD_STEAM_HANDLE_UPDATED },
    };

    for (int rangeIndex = 0; rangeIndex < (int)(sizeof(eventRanges) / sizeof(eventRanges[0])); ++rangeIndex)
    {
        while (SDL_PeepEvents(&event, 1, SDL_GETEVENT, eventRanges[rangeIndex][0], eventRanges[rangeIndex][1]) > 0)
        {
            if (!m_Paused)
            {
                switch (event.type)
                {
                case SDL_EVENT_KEY_DOWN:
                case SDL_EVENT_KEY_UP:
                {
                    CKDWORD vkey = SdlScancodeToVirtoolsKey(event.key.scancode);
                    if (vkey > 0 && vkey < KEYBOARD_BUFFER_SIZE)
                    {
                        CKBOOL pressed = (event.type == SDL_EVENT_KEY_DOWN);
                        CKDWORD timestamp = (CKDWORD)SDL_NS_TO_MS(event.key.timestamp);
                        
                        if (pressed)
                        {
                            // Only process if not a repeat, or if repetition is enabled
                            if (!event.key.repeat || m_EnableKeyboardRepetition)
                            {
                                m_KeyboardState[vkey] |= KS_PRESSED;
                                m_KeyboardStamps[vkey] = timestamp;
                                
                                if (m_NumberOfKeyInBuffer < KEYBOARD_BUFFER_SIZE)
                                {
                                    m_KeyInBuffer[m_NumberOfKeyInBuffer].dwOfs = vkey;
                                    m_KeyInBuffer[m_NumberOfKeyInBuffer].dwData = 0x80;
                                    m_KeyInBuffer[m_NumberOfKeyInBuffer].dwTimeStamp = timestamp;
                                    ++m_NumberOfKeyInBuffer;
                                }
                            }
                        }
                        else
                        {
                            m_KeyboardState[vkey] |= KS_RELEASED;
                            m_KeyboardStamps[vkey] = (CKDWORD)(timestamp - m_KeyboardStamps[vkey]);
                            
                            if (m_NumberOfKeyInBuffer < KEYBOARD_BUFFER_SIZE)
                            {
                                m_KeyInBuffer[m_NumberOfKeyInBuffer].dwOfs = vkey;
                                m_KeyInBuffer[m_NumberOfKeyInBuffer].dwData = 0x00;
                                m_KeyInBuffer[m_NumberOfKeyInBuffer].dwTimeStamp = timestamp;
                                ++m_NumberOfKeyInBuffer;
                            }
                        }
                    }
                    break;
                }

                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                case SDL_EVENT_MOUSE_BUTTON_UP:
                {
                    int button = -1;
                    switch (event.button.button)
                    {
                    case SDL_BUTTON_LEFT:
                        button = 0;
                        break;
                    case SDL_BUTTON_RIGHT:
                        button = 1;
                        break;
                    case SDL_BUTTON_MIDDLE:
                        button = 2;
                        break;
                    case SDL_BUTTON_X1:
                        button = 3;
                        break;
                    }
                    if (button >= 0 && button < 4)
                    {
                        if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
                            m_Mouse.m_State.rgbButtons[button] |= KS_PRESSED;
                        else
                            m_Mouse.m_State.rgbButtons[button] |= KS_RELEASED;
                    }
                    break;
                }

                case SDL_EVENT_MOUSE_MOTION:
                    m_RelMouseX += event.motion.xrel;
                    m_RelMouseY += event.motion.yrel;
                    break;

                case SDL_EVENT_MOUSE_WHEEL:
                    m_RelMouseWheel += event.wheel.y * 120; // Match Windows wheel delta scale
                    m_MouseWheelPosition += (int)(event.wheel.y * 120);
                    break;

                case SDL_EVENT_GAMEPAD_ADDED:
                {
                    // Try to open the gamepad if we have room
                    if (m_GamepadCount < m_MaxGamepads)
                    {
                        CKBOOL alreadyOpen = FALSE;
                        for (int i = 0; i < m_GamepadCount; i++)
                        {
                            if (m_Gamepads[i].m_InstanceID == event.gdevice.which)
                            {
                                alreadyOpen = TRUE;
                                break;
                            }
                        }

                        if (!alreadyOpen)
                        {
                            SDL_Gamepad *gamepad = SDL_OpenGamepad(event.gdevice.which);
                            if (gamepad)
                            {
                                m_Gamepads[m_GamepadCount].Init(gamepad, event.gdevice.which);
                                m_GamepadCount++;
                            }
                        }
                    }
                    break;
                }

                case SDL_EVENT_GAMEPAD_REMOVED:
                {
                    // Find and remove the gamepad
                    for (int i = 0; i < m_GamepadCount; i++)
                    {
                        if (m_Gamepads[i].m_InstanceID == event.gdevice.which)
                        {
                            m_Gamepads[i].Release();
                            // Shift remaining gamepads
                            for (int j = i; j < m_GamepadCount - 1; j++)
                            {
                                m_Gamepads[j] = m_Gamepads[j + 1];
                            }
                            m_GamepadCount--;
                            break;
                        }
                    }
                    break;
                }

                default:
                    break;
                }
            }
        }
    }

    // Keyboard repetition (emulate DX8 behavior)
    if (!m_Paused && m_EnableKeyboardRepetition)
    {
        CKDWORD currentTicks = (CKDWORD)SDL_GetTicks();
        for (int i = 0; i < KEYBOARD_BUFFER_SIZE; i++)
        {
            if (m_KeyboardState[i] == KS_PRESSED)
            {
                if (m_KeyboardStamps[i] > 0 && currentTicks - m_KeyboardStamps[i] > m_KeyboardRepeatDelay)
                    m_KeyboardStamps[i] = -(int)m_KeyboardStamps[i];
                if ((int)m_KeyboardStamps[i] < 0)
                {
                    for (int t = -(int)m_KeyboardStamps[i] - m_KeyboardRepeatDelay + currentTicks; t > (int)m_KeyboardRepeatInterval;)
                    {
                        t -= m_KeyboardRepeatInterval;
                        m_KeyboardStamps[i] = (CKDWORD)((int)m_KeyboardStamps[i] - (int)m_KeyboardRepeatInterval);
                        if (m_NumberOfKeyInBuffer < KEYBOARD_BUFFER_SIZE)
                        {
                            m_KeyInBuffer[m_NumberOfKeyInBuffer].dwData = 0x80;
                            m_KeyInBuffer[m_NumberOfKeyInBuffer].dwOfs = i;
                            m_KeyInBuffer[m_NumberOfKeyInBuffer].dwTimeStamp = -(int)m_KeyboardStamps[i];
                            ++m_NumberOfKeyInBuffer;
                        }
                    }
                }
            }
        }
    }

    if (m_Paused)
    {
        if (!m_WasPaused)
        {
            memset(m_KeyInBuffer, 0, sizeof(m_KeyInBuffer));
            memset(m_KeyboardStamps, 0, sizeof(m_KeyboardStamps));
            memset(m_KeyboardState, 0, sizeof(m_KeyboardState));
            m_NumberOfKeyInBuffer = 0;
            m_WasPaused = TRUE;
        }
    }
    else if (m_WasPaused)
    {
        m_WasPaused = FALSE;
    }

    // Update relative mouse motion
    m_Mouse.m_State.lX = (long)m_RelMouseX;
    m_Mouse.m_State.lY = (long)m_RelMouseY;
    m_Mouse.m_State.lZ = (long)m_RelMouseWheel;

    for (int i = 0; i < m_GamepadCount; i++)
        m_Gamepads[i].m_Polled = FALSE;

    return CK_OK;
}

CKERROR SdlInputManager::PostProcess()
{
    for (int iKey = 0; iKey < KEYBOARD_BUFFER_SIZE; iKey++)
    {
        if ((m_KeyboardState[iKey] & KS_RELEASED) != 0)
            m_KeyboardState[iKey] = KS_IDLE;
    }

    for (int iButton = 0; iButton < 4; iButton++)
    {
        if ((m_Mouse.m_State.rgbButtons[iButton] & KS_RELEASED) != 0)
            m_Mouse.m_State.rgbButtons[iButton] = KS_IDLE;
    }

    return CK_OK;
}

SdlInputManager::~SdlInputManager()
{
    Uninitialize();

    if (m_Gamepads)
    {
        delete[] m_Gamepads;
        m_Gamepads = NULL;
    }
}

SdlInputManager::SdlInputManager(CKContext *context) : CKInputManager(context, "SDL Input Manager")
{
    m_Gamepads = NULL;
    m_GamepadCount = 0;
    m_MaxGamepads = 4;
    m_SdlInitialized = FALSE;
    m_KeyboardAttached = FALSE;

    // Get system keyboard repeat settings
#ifdef WIN32
    int keyboardDelay;
    DWORD keyboardSpeed;
    ::SystemParametersInfo(SPI_GETKEYBOARDDELAY, 0, &keyboardDelay, 0);
    ::SystemParametersInfo(SPI_GETKEYBOARDSPEED, 0, &keyboardSpeed, 0);
    m_KeyboardRepeatDelay = 50 * (5 * keyboardDelay + 5);
    m_KeyboardRepeatInterval = (CKDWORD)(1000.0 / (keyboardSpeed + 2.5));
#else
    m_KeyboardRepeatDelay = 500;
    m_KeyboardRepeatInterval = 50;
#endif

    m_Paused = FALSE;
    m_WasPaused = FALSE;
    m_EnableKeyboardRepetition = FALSE;
    m_MouseWheelPosition = 0;
    m_RelMouseX = 0;
    m_RelMouseY = 0;
    m_RelMouseWheel = 0;

    Initialize();

    memset(m_KeyboardState, 0, sizeof(m_KeyboardState));
    memset(m_KeyInBuffer, 0, sizeof(m_KeyInBuffer));
    m_NumberOfKeyInBuffer = 0;
    m_ShowCursor = TRUE;
    SetSystemCursor(VXCURSOR_NORMALSELECT);

    m_Context->RegisterNewManager(this);
}

void SdlInputManager::Initialize()
{
    // Allocate gamepad array
    if (m_Gamepads == NULL)
    {
        m_Gamepads = new CKGamepad[m_MaxGamepads];
        if (!m_Gamepads)
        {
            m_MaxGamepads = 0;
        }
    }

    // Initialize SDL subsystems if not already done
    if (!SDL_WasInit(SDL_INIT_GAMEPAD))
    {
        if (!SDL_InitSubSystem(SDL_INIT_GAMEPAD))
        {
            // Log error but continue - keyboard/mouse may still work
        }
    }

    if (!SDL_WasInit(SDL_INIT_EVENTS))
    {
        if (!SDL_InitSubSystem(SDL_INIT_EVENTS))
        {
            // Log error
        }
    }

    m_SdlInitialized = TRUE;
    m_KeyboardAttached = TRUE; // SDL always has keyboard access

    // Initialize mouse
    m_Mouse.Init();

    // Enumerate connected gamepads
    int numJoysticks = 0;
    SDL_JoystickID *joysticks = SDL_GetGamepads(&numJoysticks);
    if (joysticks)
    {
        for (int i = 0; i < numJoysticks && m_GamepadCount < m_MaxGamepads; i++)
        {
            SDL_Gamepad *gamepad = SDL_OpenGamepad(joysticks[i]);
            if (gamepad)
            {
                m_Gamepads[m_GamepadCount].Init(gamepad, joysticks[i]);
                m_GamepadCount++;
            }
        }
        SDL_free(joysticks);
    }
}

void SdlInputManager::Uninitialize()
{
    m_Mouse.Release();

    for (int i = 0; i < m_GamepadCount; i++)
        m_Gamepads[i].Release();

    m_GamepadCount = 0;
    m_SdlInitialized = FALSE;
    m_KeyboardAttached = FALSE;
}

void SdlInputManager::ClearBuffers()
{
    // Pump and discard all pending events
    SDL_PumpEvents();
    SDL_FlushEvents(SDL_EVENT_KEY_DOWN, SDL_EVENT_KEY_UP);
    SDL_FlushEvents(SDL_EVENT_MOUSE_BUTTON_DOWN, SDL_EVENT_MOUSE_WHEEL);
    SDL_FlushEvents(SDL_EVENT_GAMEPAD_AXIS_MOTION, SDL_EVENT_GAMEPAD_BUTTON_UP);

    m_Mouse.Clear();
    memset(m_KeyboardState, 0, sizeof(m_KeyboardState));
    m_NumberOfKeyInBuffer = 0;
}
