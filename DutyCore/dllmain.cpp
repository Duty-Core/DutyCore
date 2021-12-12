#include "dllmain.h"
#include "MinHook.h"
#include "dllproxy.h"
#include "resolve_imports.h"
#include "game.h"
#include "script.h"
#include "renderer.h"

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

extern "C" HRESULT __declspec(dllexport) WICConvertBitmapSource(void* dstFormat, void* pISrc, void** ppIDst)
{
    dutycore::main::Log.WriteLine("Called WICConvertBitmapSource");
    if (!p_WICConvertBitmapSource) {
        p_WICConvertBitmapSource = (tWICConvertBitmapSource)DLLProxy::Instance().XLoadExport("WICConvertBitmapSource", "WindowsCodecs.dll");
        dutycore::main::Log.WriteLine("Found WICConvertBitmapSource");
    }

    return p_WICConvertBitmapSource(dstFormat, pISrc, ppIDst);
}

using tWaitForSingleObject = DWORD(*)(HANDLE, DWORD);

static DWORD OldProtect = {};

static tWaitForSingleObject oWaitForSingleObject = {};
static tWaitForSingleObject p_WaitForSingleObject = {};
static bool InitOnce = false;

namespace dutycore
{
    MinLog main::Log = MinLog("DutyCore.log", "dutycore");

    void main::CreateEntryPoint()
    {
        if (MH_Initialize() != MH_OK)
        {
            main::Log.WriteLine("Duty Core Failed Loading");
            return;
        }
        main::Log.WriteLine("Duty Core Loaded");

        // Locate Kernel32.dll
        auto const kernel32 = Resolver::find(L"KERNEL32.dll");
        // Find vtable pointer for WaitForSingleObject
        kernel32.resolve("WaitForSingleObject", &oWaitForSingleObject);

        // Copy the original pointer to a local variable
        if (!p_WaitForSingleObject) {
            p_WaitForSingleObject = oWaitForSingleObject;
        }

        *((void**)(game::ModuleBase + 0x1AAEAD14)) = (void*)main::InterceptWaitForSingleObject;
    }

    DWORD main::InterceptWaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds) {

        // Check if the AssetPool has been initialized
        if (*(game::ModuleBase + 0x9412942) == 1 && !InitOnce)
        {
            InitOnce = true;

            main::Log.WriteLine("Duty Core Init");
            main::Log << std::hex << (uint64_t)game::ModuleBase << "\n";
            // Unhook WaitForSingleObject
            *((void**)(game::ModuleBase + 0x1AAEAD14)) = (void*)p_WaitForSingleObject;

            // All Code that should be executed after hooking should be done here.
            // This entry point may be too delayed for certain features
            // Must evaluate on a case-by-case basis

            auto cfuncs = callofduty::script::GetCommonFunctions();
            cfuncs->AddDebugCommand.actionFunc = callofduty::script::GScr_AddDebugCommand;

            renderer::SetupDXDevice();
        }

        return p_WaitForSingleObject(hHandle, dwMilliseconds);
    }
}