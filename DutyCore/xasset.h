#pragma once
namespace dutycore
{
	struct XAsset {
		void* firstEntry;
		int size;
		int maximum;
		bool unk[4];
		int count;
		void* entries;
	};
}