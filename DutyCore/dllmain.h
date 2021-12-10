#pragma once

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#endif

#include <windows.h>

#define MINLOG_OUT_FILE "DutyCore.log"
#define MINLOG_OUT_DIR "dutycore"

using tD3D11CreateDevice = HRESULT(*)(void*, UINT, HMODULE, UINT, const void*, UINT, UINT, void**, void*, void**);
static tD3D11CreateDevice p_D3D11CreateDevice;

extern "C" BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved);
extern "C" __declspec(dllexport) HRESULT D3D11CreateDevice(void* pAdapter, UINT DriverType, HMODULE Software, UINT Flags, const void* pFeatureLevels, UINT FeatureLevels, UINT SDKVersion, void** ppDevice, void* pFeatureLevel, void** ppImmediateContext);

namespace dutycore
{
	class main
	{
	public:
		static void CreateEntryPoint();
		static DWORD InterceptWaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds);
	};

	constexpr unsigned int SL_GenerateCanonicalString(const char* name)
	{
		int i = 0;
		unsigned int hash = 0x4B9ACE2F;
		while (name[i] != 0)
		{
			hash ^= name[i];
			hash *= 0x1000193;
			i++;
		}

		hash ^= name[i];
		hash *= 0x1000193;

		return hash;
	}
}
