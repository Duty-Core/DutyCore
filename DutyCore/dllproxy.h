#pragma once

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <map>
#include <string>

class DLLProxy
{
private:
    std::map<std::string, HMODULE> LoadedModules = { };

public:
    static DLLProxy& Instance() {
        static DLLProxy _instance = { };
        return _instance;
    }

    HMODULE XLoadLibrary(const char* Source) {
        auto mod = LoadedModules.find(std::string(Source));
        if (mod != LoadedModules.end()) {
            return mod->second;
        }
        char dirBuffer[MAX_PATH] = { };
        GetSystemDirectoryA(dirBuffer, MAX_PATH);
        strcat_s(dirBuffer, MAX_PATH, "\\");
        strcat_s(dirBuffer, MAX_PATH, Source);
        auto lib = LoadLibraryA(dirBuffer);
        LoadedModules.emplace(std::string(Source), lib);
        return lib;
    }

    void* XLoadExport(const char* lpProcName, const char* Source) {
        auto mod = XLoadLibrary(Source);
        auto procAddress = GetProcAddress(mod, lpProcName);
        if (!procAddress)
            return 0;

        return procAddress;
    }
};
