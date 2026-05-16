#include "SdlInputManager.h"

// =============================================================================
// CKMouse Implementation
// =============================================================================

SdlInputManager::CKMouse::CKMouse()
{
    memset(&m_State, 0, sizeof(m_State));
    memset(m_LastButtons, 0, sizeof(m_LastButtons));
    m_NumberOfBuffer = 0;
    m_Attached = FALSE;
}

void SdlInputManager::CKMouse::Init()
{
    m_Attached = SDL_HasMouse();
}

void SdlInputManager::CKMouse::Release()
{
    m_Attached = FALSE;
}

void SdlInputManager::CKMouse::Clear()
{
    memset(m_LastButtons, 0, sizeof(m_LastButtons));
    memset(m_State.rgbButtons, 0, sizeof(m_State.rgbButtons));
}

void SdlInputManager::CKMouse::Poll(CKBOOL pause)
{
    // Save last button states
    *(CKDWORD *)m_LastButtons = *(CKDWORD *)m_State.rgbButtons;

    if (pause)
    {
        m_NumberOfBuffer = 0;
        return;
    }

    // Get current mouse position (global screen coordinates)
    float x, y;
    SDL_GetGlobalMouseState(&x, &y);
    m_Position.x = x;
    m_Position.y = y;

    // Note: Button state and relative motion are updated via SDL events in PreProcess
}
