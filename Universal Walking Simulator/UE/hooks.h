#pragma once
#include <functional>
#include <unordered_map>

#include "structs.h"

namespace Hooks
{
    inline std::unordered_map<UFunction*, std::function<bool(UObject *, UFunction *, void*)>> FunctionsToHook;

    inline void Add(const std::string& Str, const std::function<bool(UObject *, UFunction *, void*)>& Func)
    {
        auto FuncObject = FindObject<UFunction>(Str);
        if (!FuncObject)
        {
            std::cout << "Cannot find " <<  Str << std::endl;
            return;
        }

        std::cout << "Found " <<  Str << std::endl;
        FunctionsToHook.insert({FuncObject, Func});
    }
}
