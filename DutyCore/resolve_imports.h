#pragma once
#include <Windows.h>
#include <cstdint>
#include <cstdio>

template <typename T>
constexpr auto cstr_copy(T* dst, T const* src) noexcept -> T* {
    while ((*dst = *src)) {
        ++dst, ++src;
    }
    return dst;
}

template <typename T>
constexpr auto cstr_icmp(T const* lhs, T const* rhs) noexcept -> int {
    constexpr auto const lower = [](T c) constexpr noexcept -> int {
        return (c >= 'A' && c <= 'Z') ? ((c - 'A') + 'a') : c;
    };
    while (*lhs && lower(*lhs) == lower(*rhs)) {
        ++lhs, ++rhs;
    }
    return lower(*lhs) - lower(*rhs);
};

template <typename T>
constexpr auto cstr_cmp(T const* lhs, T const* rhs) noexcept -> int {
    while (*lhs && *lhs == *rhs) {
        ++lhs, ++rhs;
    }
    return *lhs - *rhs;
};

struct UNI_STR {
    USHORT Length;
    USHORT MaximumLength;
    PWCH Buffer;
};

struct DLL_LOAD_CB_DATA {
    ULONG Flags;
    UNI_STR const* FullDllName;
    UNI_STR const* BaseDllName;
    PVOID DllBase;
    ULONG SizeOfImage;
};
using DLL_LOAD_CB = void (WINAPI*)(ULONG reason, DLL_LOAD_CB_DATA const* data, LPVOID ctx);

struct ModuleInfo {
    std::uintptr_t base = {};
    wchar_t const* name = {};

    static auto from_cb(DLL_LOAD_CB_DATA const* data) -> ModuleInfo {
        return { reinterpret_cast<std::uintptr_t>(data->DllBase), data->BaseDllName->Buffer };
    }

    static auto find(wchar_t const* name) noexcept -> ModuleInfo {
#if UINTPTR_MAX > 0xFFFFFFFF
        auto const peb = reinterpret_cast<char const*>(__readgsqword(0x60));
#else
        auto const peb = reinterpret_cast<char const*>(__readfsdword(0x30));
#endif
        auto const ldr = reinterpret_cast<char const* const*>(peb)[3];
        auto image_entry = reinterpret_cast<char const* const*>(ldr + 8)[1];
        while (auto const image_base = reinterpret_cast<std::uintptr_t const*>(image_entry)[6]) {
            auto const image_name = reinterpret_cast<wchar_t const* const*>(image_entry)[12];
            if (!name || cstr_icmp(name, image_name) == 0) {
                return { image_base, image_name };
            }
            image_entry = reinterpret_cast<char const* const*>(image_entry)[0];
        }
        return {};
    }
};

struct Resolver {
    std::uintptr_t base = {};
    std::uint32_t functions_num = {};
    std::uint32_t const* functions = {};
    std::uint32_t const* names = {};
    std::uint16_t const* ordinals = {};

    static auto find(wchar_t const* name) noexcept -> Resolver {
        auto const module = ModuleInfo::find(name);
        if (!module.base) {
            return {};
        }
        auto const dos = reinterpret_cast<PIMAGE_DOS_HEADER>(module.base);
        auto const nt = reinterpret_cast<PIMAGE_NT_HEADERS>(module.base + dos->e_lfanew);
        auto const eat_address = nt->OptionalHeader.DataDirectory[0].VirtualAddress;
        auto const eat = reinterpret_cast<std::uint32_t const*>(module.base + eat_address);
        auto result = Resolver{};
        result.base = module.base;
        result.functions_num = eat[5];
        result.functions = reinterpret_cast<std::uint32_t const*>(module.base + eat[7]);
        result.names = reinterpret_cast<std::uint32_t const*>(module.base + eat[8]);
        result.ordinals = (std::uint16_t const*)(module.base + eat[9]);
        return result;
    }

    auto find_function(char const* name) const noexcept -> std::uintptr_t {
        for (auto low = 0u, high = functions_num; low != high; ) {
            auto const mid = (low + high) / 2;
            auto const func_name = reinterpret_cast<char const*>(base + names[mid]);
            auto const result = cstr_cmp(name, func_name);
            if (result > 0) {
                low = mid + 1;
            }
            else if (result < 0) {
                high = mid;
            }
            else {
                return base + functions[ordinals[mid]];
            }
        }
        return false;
    }

    template<typename T>
    auto resolve(char const* name, T* out) const noexcept -> bool {
        auto const raw = find_function(name);
        return *out = reinterpret_cast<T>(raw);
    }
};