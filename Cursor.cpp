#include "SdlInputManager.h"

#include "VxWindowFunctions.h"

void SdlInputManager::ShowCursor(CKBOOL iShow)
{
    m_ShowCursor = iShow;
    EnsureCursorVisible(iShow);
}

void SdlInputManager::EnsureCursorVisible(CKBOOL iShow)
{
    // Use platform cursor visibility (same as DX8 manager)
    // This ensures compatibility with the rest of NeMo2
    if (iShow)
    {
        int dc = VxShowCursor(TRUE);
        if (dc > 0)
            dc = VxShowCursor(FALSE);
        if (dc < 0)
            while (VxShowCursor(TRUE) < 0)
                continue;
    }
    else
    {
        int dc = VxShowCursor(FALSE);
        if (dc < -1)
            dc = VxShowCursor(TRUE);
        if (dc >= 0)
            while (VxShowCursor(FALSE) >= 0)
                continue;
    }
}

CKBOOL SdlInputManager::GetCursorVisibility()
{
    return m_ShowCursor;
}

VXCURSOR_POINTER SdlInputManager::GetSystemCursor()
{
    return m_Cursor;
}

void SdlInputManager::SetSystemCursor(VXCURSOR_POINTER cursor)
{
    m_Cursor = cursor;
    VxSetCursor(cursor);
}
