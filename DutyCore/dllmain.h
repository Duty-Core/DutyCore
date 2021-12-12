#pragma once

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#endif

#include <windows.h>
#include "MinLog.h"

using tWICConvertBitmapSource = HRESULT(*)(void*, void*, void**);
static tWICConvertBitmapSource p_WICConvertBitmapSource;

extern "C" BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved);
extern "C" __declspec(dllexport) HRESULT WICConvertBitmapSource(void* dstFormat, void* pISrc, void** ppIDst);

namespace dutycore
{
	class main
	{
	public:
		static MinLog Log;
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
