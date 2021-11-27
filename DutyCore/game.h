#pragma once
#include <Windows.h>
#include <atomic>
#include "dllmain.h"
#include "xasset.h"

namespace dutycore
{
	class game
	{
		using tGetXAssetSize = int(*)(int);
		static tGetXAssetSize DB_GetXAssetTypeSize;

		using tSys_ShowConsole = void(*)();
		static tSys_ShowConsole Sys_ShowConsolePtr;

		static XAsset* DB_XAssetPool;
		static int GetXAssetSizeHandler(int index);
		static XAsset* GetXAssetPool(int index);

	public:
		static HMODULE ModuleBase;

		static int GetXAssetSize(int index);
		static void ResizeAssetLimits(int index, int newSize);
	};
}
