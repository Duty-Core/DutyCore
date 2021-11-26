#include "dllmain.h"
#include "MinLog.h"
#include "dllproxy.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
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
