#include "SdlInputManager.h"

#include <cmath>

static void SetButtonState(CKDWORD *buttonsLow, CKDWORD *buttonsHigh, int button, CKBOOL down)
{
    CKDWORD mask;

    if (!buttonsLow || !buttonsHigh)
        return;

    if (button < 0 || button >= 64)
        return;

    mask = 1u << (button & 31);
    if (button < 32)
    {
        if (down)
            *buttonsLow |= mask;
        else
            *buttonsLow &= ~mask;
    }
    else
    {
        if (down)
            *buttonsHigh |= mask;
        else
            *buttonsHigh &= ~mask;
    }
}

// Helper function to normalize axis value from SDL range to [-1, 1]
static double NormalizeAxis(Sint16 value, CKDWORD minValue, CKDWORD maxValue)
{
    double dMin = (double)(int)minValue;
    double range = (double)(int)maxValue - dMin;
    if (range == 0.0)
        return 0.0;

    double normalized = ((double)value - dMin) * 2.0 / range - 1.0;
    if (normalized > 1.0)
        normalized = 1.0;
    else if (normalized < -1.0)
        normalized = -1.0;

    return normalized;
}

static float ClampAxis(float value)
{
    if (value > 1.0f)
        return 1.0f;
    if (value < -1.0f)
        return -1.0f;
    return value;
}

// =============================================================================
// CKGamepad Implementation
// =============================================================================

SdlInputManager::CKGamepad::CKGamepad()
{
    m_Gamepad = NULL;
    m_InstanceID = 0;
    memset(m_DeviceName, 0, sizeof(m_DeviceName));
    m_DeadzoneRadius = 0.01f;
    m_Gain = 1.0f;
    m_ButtonCount = 0;
    m_Polled = FALSE;
    m_Position.Set(0.0f, 0.0f, 0.0f);
    m_Rotation.Set(0.0f, 0.0f, 0.0f);
    m_Sliders.Set(0.0f, 0.0f);
    m_PointOfViewAngle = (CKDWORD)-1;
    m_Buttons = 0;
    m_ButtonsHigh = 0;

    // Default SDL gamepad axis range
    m_Xmin = m_Ymin = m_Zmin = -32768;
    m_Xmax = m_Ymax = m_Zmax = 32767;
    m_XRmin = m_YRmin = m_ZRmin = -32768;
    m_XRmax = m_YRmax = m_ZRmax = 32767;
    m_Umin = m_Vmin = 0;
    m_Umax = m_Vmax = 32767;

    // SDL gamepads have standard axes
    m_HasX = m_HasY = TRUE;
    m_HasZ = FALSE; // Left trigger (mapped differently)
    m_HasRx = m_HasRy = TRUE;
    m_HasRz = FALSE; // Right trigger
    m_HasSlider0 = m_HasSlider1 = FALSE;
}

void SdlInputManager::CKGamepad::Init(SDL_Gamepad *gamepad, SDL_JoystickID id)
{
    m_Gamepad = gamepad;
    m_InstanceID = id;

    // Get device name
    const char *name = SDL_GetGamepadName(gamepad);
    if (name)
    {
        strncpy(m_DeviceName, name, sizeof(m_DeviceName) - 1);
        m_DeviceName[sizeof(m_DeviceName) - 1] = '\0';
    }
    else
    {
        strcpy(m_DeviceName, "Game Controller");
    }

    // SDL gamepads have a standard button layout. The legacy aggregate button
    // state remains 32-bit, but per-button queries can see a second 32-bit bank.
    m_ButtonCount = SDL_GAMEPAD_BUTTON_COUNT;
    if (m_ButtonCount > 64)
        m_ButtonCount = 64;

    // SDL gamepads always have these axes in the standard mapping:
    // Left stick X/Y, Right stick X/Y, Triggers as axes
    m_HasX = m_HasY = TRUE;   // Left stick
    m_HasRx = m_HasRy = TRUE; // Right stick
    
    // Map triggers to Z axes (left trigger = Z, right trigger = Rz)
    m_HasZ = TRUE;
    m_HasRz = TRUE;
    m_HasSlider0 = TRUE;
    m_HasSlider1 = TRUE;
    
    // SDL triggers range from 0 to 32767
    m_Zmin = m_ZRmin = 0;
    m_Zmax = m_ZRmax = 32767;
    m_Umin = m_Vmin = 0;
    m_Umax = m_Vmax = 32767;
}

void SdlInputManager::CKGamepad::Release()
{
    if (m_Gamepad)
    {
        SDL_CloseGamepad(m_Gamepad);
        m_Gamepad = NULL;
    }
    m_InstanceID = 0;
}

void SdlInputManager::CKGamepad::ResetState()
{
    m_Position.Set(0.0f, 0.0f, 0.0f);
    m_Rotation.Set(0.0f, 0.0f, 0.0f);
    m_Sliders.Set(0.0f, 0.0f);
    m_PointOfViewAngle = (CKDWORD)-1;
    m_Buttons = 0;
    m_ButtonsHigh = 0;
    m_Polled = TRUE;
}

void SdlInputManager::CKGamepad::Poll()
{
    if (m_Polled)
        return;

    if (!m_Gamepad || !SDL_GamepadConnected(m_Gamepad))
    {
        ResetState();
        return;
    }

    // Get axis values
    Sint16 leftX = SDL_GetGamepadAxis(m_Gamepad, SDL_GAMEPAD_AXIS_LEFTX);
    Sint16 leftY = SDL_GetGamepadAxis(m_Gamepad, SDL_GAMEPAD_AXIS_LEFTY);
    Sint16 rightX = SDL_GetGamepadAxis(m_Gamepad, SDL_GAMEPAD_AXIS_RIGHTX);
    Sint16 rightY = SDL_GetGamepadAxis(m_Gamepad, SDL_GAMEPAD_AXIS_RIGHTY);
    Sint16 leftTrigger = SDL_GetGamepadAxis(m_Gamepad, SDL_GAMEPAD_AXIS_LEFT_TRIGGER);
    Sint16 rightTrigger = SDL_GetGamepadAxis(m_Gamepad, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER);

    // Normalize left stick to position (X, Y)
    double x = NormalizeAxis(leftX, m_Xmin, m_Xmax);
    double y = NormalizeAxis(leftY, m_Ymin, m_Ymax);
    
    // Normalize left trigger to Z axis (0 to 1 range, then scale to -1 to 1 for compatibility)
    double z = NormalizeAxis(leftTrigger, m_Zmin, m_Zmax);

    // Apply radial deadzone to left stick
    const double deadzone = static_cast<double>(m_DeadzoneRadius);
    double xyMagnitude = sqrt(x * x + y * y);
    if (xyMagnitude < deadzone || deadzone >= 1.0)
    {
        x = y = 0.0;
    }
    else if (xyMagnitude > 0.0)
    {
        double scale = (xyMagnitude - deadzone) / (1.0 - deadzone);
        x = (x / xyMagnitude) * scale;
        y = (y / xyMagnitude) * scale;
    }

    // Apply linear deadzone to trigger
    if (fabs(z) < deadzone)
        z = 0.0;

    // Apply gain and clamp
    const double gain = static_cast<double>(m_Gain);
    m_Position.Set(ClampAxis((float)(x * gain)), ClampAxis((float)(y * gain)), ClampAxis((float)(z * gain)));

    // Normalize right stick to rotation (Rx, Ry)
    double rx = NormalizeAxis(rightX, m_XRmin, m_XRmax);
    double ry = NormalizeAxis(rightY, m_YRmin, m_YRmax);
    
    // Normalize right trigger to Rz
    double rz = NormalizeAxis(rightTrigger, m_ZRmin, m_ZRmax);

    // Apply radial deadzone to right stick
    double rxryMagnitude = sqrt(rx * rx + ry * ry);
    if (rxryMagnitude < deadzone || deadzone >= 1.0)
    {
        rx = ry = 0.0;
    }
    else if (rxryMagnitude > 0.0)
    {
        double scale = (rxryMagnitude - deadzone) / (1.0 - deadzone);
        rx = (rx / rxryMagnitude) * scale;
        ry = (ry / rxryMagnitude) * scale;
    }

    if (fabs(rz) < deadzone)
        rz = 0.0;

    m_Rotation.Set(ClampAxis((float)(rx * gain)), ClampAxis((float)(ry * gain)), ClampAxis((float)(rz * gain)));

    double slider0 = NormalizeAxis(leftTrigger, m_Umin, m_Umax);
    double slider1 = NormalizeAxis(rightTrigger, m_Vmin, m_Vmax);

    if (fabs(slider0) < deadzone)
        slider0 = 0.0;
    if (fabs(slider1) < deadzone)
        slider1 = 0.0;

    m_Sliders.Set(ClampAxis((float)(slider0 * gain)), ClampAxis((float)(slider1 * gain)));

    // D-pad to POV angle conversion
    // POV uses DirectInput format: degrees * 100, or -1 for centered
    bool dpadUp = SDL_GetGamepadButton(m_Gamepad, SDL_GAMEPAD_BUTTON_DPAD_UP);
    bool dpadDown = SDL_GetGamepadButton(m_Gamepad, SDL_GAMEPAD_BUTTON_DPAD_DOWN);
    bool dpadLeft = SDL_GetGamepadButton(m_Gamepad, SDL_GAMEPAD_BUTTON_DPAD_LEFT);
    bool dpadRight = SDL_GetGamepadButton(m_Gamepad, SDL_GAMEPAD_BUTTON_DPAD_RIGHT);

    if (!dpadUp && !dpadDown && !dpadLeft && !dpadRight)
    {
        m_PointOfViewAngle = (CKDWORD)-1;
    }
    else
    {
        // Calculate angle based on D-pad state
        // 0 = up, 9000 = right, 18000 = down, 27000 = left (degrees * 100)
        if (dpadUp && !dpadLeft && !dpadRight)
            m_PointOfViewAngle = 0;
        else if (dpadUp && dpadRight)
            m_PointOfViewAngle = 4500;
        else if (dpadRight && !dpadUp && !dpadDown)
            m_PointOfViewAngle = 9000;
        else if (dpadDown && dpadRight)
            m_PointOfViewAngle = 13500;
        else if (dpadDown && !dpadLeft && !dpadRight)
            m_PointOfViewAngle = 18000;
        else if (dpadDown && dpadLeft)
            m_PointOfViewAngle = 22500;
        else if (dpadLeft && !dpadUp && !dpadDown)
            m_PointOfViewAngle = 27000;
        else if (dpadUp && dpadLeft)
            m_PointOfViewAngle = 31500;
    }

    // Get button states (mapping SDL gamepad buttons to a 32-bit mask)
    m_Buttons = 0;
    m_ButtonsHigh = 0;
    
    // Map SDL gamepad buttons to DirectInput-style button indices
    // SDL gamepad button order: A, B, X, Y, Back, Guide, Start, LeftStick, RightStick, LeftShoulder, RightShoulder, DPad...
    // We skip D-pad since it's mapped to POV
    static const SDL_GamepadButton buttonMap[] = {
        SDL_GAMEPAD_BUTTON_SOUTH,        // 0 - A / Cross
        SDL_GAMEPAD_BUTTON_EAST,         // 1 - B / Circle
        SDL_GAMEPAD_BUTTON_WEST,         // 2 - X / Square
        SDL_GAMEPAD_BUTTON_NORTH,        // 3 - Y / Triangle
        SDL_GAMEPAD_BUTTON_LEFT_SHOULDER,  // 4 - LB / L1
        SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER, // 5 - RB / R1
        SDL_GAMEPAD_BUTTON_BACK,         // 6 - Back / Select
        SDL_GAMEPAD_BUTTON_START,        // 7 - Start
        SDL_GAMEPAD_BUTTON_LEFT_STICK,   // 8 - Left stick click
        SDL_GAMEPAD_BUTTON_RIGHT_STICK,  // 9 - Right stick click
        SDL_GAMEPAD_BUTTON_GUIDE,        // 10 - Guide / Home
        SDL_GAMEPAD_BUTTON_MISC1,        // 11 - Misc
        SDL_GAMEPAD_BUTTON_TOUCHPAD,     // 12 - Touchpad (PS controllers)
        SDL_GAMEPAD_BUTTON_MISC2,        // 13
        SDL_GAMEPAD_BUTTON_MISC3,        // 14
        SDL_GAMEPAD_BUTTON_MISC4,        // 15
    };

    const int numMappedButtons = sizeof(buttonMap) / sizeof(buttonMap[0]);
    for (int i = 0; i < numMappedButtons && i < 32; i++)
    {
        if (SDL_GetGamepadButton(m_Gamepad, buttonMap[i]))
            SetButtonState(&m_Buttons, &m_ButtonsHigh, i, TRUE);
    }

    m_Polled = TRUE;
}

CKBOOL SdlInputManager::CKGamepad::IsAttached()
{
    return m_Gamepad != NULL && SDL_GamepadConnected(m_Gamepad);
}
