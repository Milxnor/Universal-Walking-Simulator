#pragma once
#include <functional>
#include <unordered_map>

#include "structs.h"

namespace Hooks
{
    inline std::unordered_map<UFunction*, std::function<bool(UObject *, UFunction *, void*)>> FunctionsToHook;

    inline void Add(const std::string& Str, std::function<bool(UObject *, UFunction *, void*)> Func)
    {
        auto FuncObject = FindObject<UFunction>(Str);
        if (!FuncObject)
        {
            return;
        }

        FunctionsToHook.insert({FuncObject, Func});
    }
}
