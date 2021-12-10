#include "dllmain.h"
#include "MinLog.h"
#include "dllproxy.h"
#include "resolve_imports.h"
#include "Minhook.h"
#include "game.h"
#include "script.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        dutycore::main::CreateEntryPoint();
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

HRESULT D3D11CreateDevice(void* pAdapter, UINT DriverType, HMODULE Software, UINT Flags, const void* pFeatureLevels, UINT FeatureLevels, UINT SDKVersion, void** ppDevice, void* pFeatureLevel, void** ppImmediateContext)
{
    MinLog::Instance().WriteLine("Called D3D11CreateDevice");
    if (!p_D3D11CreateDevice) {
        p_D3D11CreateDevice = (tD3D11CreateDevice)DLLProxy::Instance().XLoadExport("D3D11CreateDevice", "d3d11.dll");
    }

    return p_D3D11CreateDevice(pAdapter, DriverType, Software, Flags, pFeatureLevels, FeatureLevels, SDKVersion, ppDevice, pFeatureLevel, ppImmediateContext);
}

using tWaitForSingleObject = DWORD(*)(HANDLE, DWORD);

static DWORD OldProtect = 0;

static tWaitForSingleObject oWaitForSingleObject = 0;
static tWaitForSingleObject p_WaitForSingleObject = 0;

namespace dutycore
{
    void main::CreateEntryPoint()
    {
        // Locate Kernel32.dll
        auto const kernel32 = Resolver::find(L"KERNEL32.dll");
        // Find vtable pointer for WaitForSingleObject
        kernel32.resolve("WaitForSingleObject", &oWaitForSingleObject);

        // Copy the original pointer to a local variable
        if (!p_WaitForSingleObject) {
            p_WaitForSingleObject = oWaitForSingleObject;
        }

        *((void**)((uint8_t*)game::ModuleBase + 0x1AAEAD14)) = (void*)main::InterceptWaitForSingleObject;
        *((void**)(game::ModuleBase + 0x1AAEAD14)) = (void*)main::InterceptWaitForSingleObject;
    }

    DWORD main::InterceptWaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds) {

        // Check if the AssetPool has been initialized
        if (*(game::ModuleBase + 0x9412942) == 1)
        {
            // Unhook WaitForSingleObject
            *((void**)(game::ModuleBase + 0x1AAEAD14)) = (void*)p_WaitForSingleObject;

            // All Code that should be executed after hooking should be done here.
            // This entry point may be too delayed for certain features
            // Must evaluate on a case-by-case basis
            auto cfuncs = callofduty::script::GetCommonFunctions();
            cfuncs->AddDebugCommand.actionFunc = callofduty::script::GScr_AddDebugCommand;
        }

    }
}