#include "game.h"
#include "MinLog.h"

namespace dutycore
{
	HMODULE game::ModuleBase = GetModuleHandle(NULL);
	game::tGetXAssetSize game::DB_GetXAssetTypeSize;
	XAsset* game::DB_XAssetPool;

	int game::GetXAssetSizeHandler(int index)
	{
		if (!game::DB_GetXAssetTypeSize)
		{
			game::DB_GetXAssetTypeSize = reinterpret_cast<game::tGetXAssetSize>((uint8_t*)game::ModuleBase + 0x13E9DD0);
			sprintf_s(main::PrintBuffer, "DB_GetXAssetSizeHandler @ 0x%p", game::DB_GetXAssetTypeSize);
			MinLog::Instance().WriteLine(main::PrintBuffer);
		}
		return game::DB_GetXAssetTypeSize(index);
	}

	XAsset* game::GetXAssetPool(int index)
	{
		if (!game::DB_XAssetPool)
		{
			game::DB_XAssetPool = reinterpret_cast<XAsset*>((uint8_t*)game::ModuleBase + 0x94093F0);
			sprintf_s(main::PrintBuffer, "DB_XAssetPool @ 0x%p", game::DB_XAssetPool);
			MinLog::Instance().WriteLine(main::PrintBuffer);
		}
		return &game::DB_XAssetPool[index];
	}

	int game::GetXAssetSize(int index)
	{
		if (index >= 103 || index < 0)
			return 0;
		return game::GetXAssetSizeHandler(index);
	}

	void game::ResizeAssetLimits(int index, int newSize)
	{
		auto structSize = GetXAssetSize(index);
		auto assetPool = GetXAssetPool(index);
		// TODO: Support resizing assetPools to be less than current size
		if (assetPool != 0 && assetPool->maximum < newSize)
		{
			auto newBlock = (LinkedListEntry*)calloc(newSize - assetPool->maximum, structSize);
			if (newBlock != 0)
			{
				LinkedListEntry* blockPtr = (LinkedListEntry*)assetPool->firstEntry;
				LinkedListEntry* nextBlockPtr = newBlock;
				auto size_c = newSize - 1;
				do
				{
					if (!blockPtr->next)
					{
						blockPtr->next = nextBlockPtr;
						blockPtr = nextBlockPtr;
						nextBlockPtr = (LinkedListEntry*)((uint8_t*)nextBlockPtr + structSize);
					}
					else
					{
						blockPtr = (LinkedListEntry*)((uint8_t*)blockPtr + structSize);
					}
					--size_c;
				} while (size_c);
				assetPool->maximum = newSize;
			}
		}
	}
}