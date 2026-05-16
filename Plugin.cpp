#include "CKAll.h"

#include "SdlInputManager.h"

#ifdef CK_LIB
#define CreateNewManager                CreateNewInputManager
#define RemoveManager                   RemoveInputManager
#define CKGetPluginInfoCount            CKGet_InputManager_PluginInfoCount
#define CKGetPluginInfo                 CKGet_InputManager_PluginInfo
#define g_PluginInfo                    g_InputManager_PluginInfo
#define CKInitializeParameterTypes      CKInputManagerInitializeParameterTypes
#define CKInitializeOperationTypes      CKInputManagerInitializeOperationTypes
#define CKInitializeOperationFunctions  CKInputManagerInitializeOperationFunctions
#define CKUnInitializeParameterTypes    CKInputManagerUnInitializeParameterTypes
#define CKUnInitializeOperationTypes    CKInputManagerUnInitializeOperationTypes
#else
#define CreateNewManager                CreateNewManager
#define RemoveManager                   RemoveManager
#define CKGetPluginInfoCount            CKGetPluginInfoCount
#define CKGetPluginInfo                 CKGetPluginInfo
#define g_PluginInfo                    g_PluginInfo
#endif

CKPluginInfo g_PluginInfo;

#define SDL_INPUTMANAGER_GUID INPUT_MANAGER_GUID

void CKInitializeParameterTypes(CKContext *context);
void CKInitializeOperationTypes(CKContext *context);
void CKInitializeOperationFunctions(CKContext *context);
void CKUnInitializeParameterTypes(CKContext *context);
void CKUnInitializeOperationTypes(CKContext *context);

CKERROR CreateNewManager(CKContext *context)
{
    CKInitializeParameterTypes(context);
    CKInitializeOperationTypes(context);
    CKInitializeOperationFunctions(context);

    new SdlInputManager(context);

    return CK_OK;
}

CKERROR RemoveManager(CKContext *context)
{
    SdlInputManager *man = (SdlInputManager *)context->GetManagerByName("SDL Input Manager");
    if (man) delete man;

    CKUnInitializeParameterTypes(context);
    CKUnInitializeOperationTypes(context);

    return CK_OK;
}

PLUGIN_EXPORT CKPluginInfo *CKGetPluginInfo(int Index)
{
    g_PluginInfo.m_Author = "Ballanced";
    g_PluginInfo.m_Description = "SDL3 Keyboard/Mouse/Gamepad Manager";
    g_PluginInfo.m_Extension = "";
    g_PluginInfo.m_Type = CKPLUGIN_MANAGER_DLL;
    g_PluginInfo.m_Version = 0x000001;
    g_PluginInfo.m_InitInstanceFct = CreateNewManager;
    g_PluginInfo.m_ExitInstanceFct = RemoveManager;
    g_PluginInfo.m_GUID = SDL_INPUTMANAGER_GUID;
    g_PluginInfo.m_Summary = "SDL Input Manager";
    return &g_PluginInfo;
}
