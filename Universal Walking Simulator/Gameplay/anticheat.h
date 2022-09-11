#pragma once

#include <UE/structs.h>

// TODO: Add anti cheat capabilities 
namespace AntiCheat
{
    // in 0.00 of a second
    // like 0.0055
    inline float GetFireRate(UObject* Weapon)
    {
        float FiringRate = 0;

        if (static auto GetFiringRate = Weapon->Function("GetFiringRate"))
        {
            Weapon->ProcessEvent(GetFiringRate, &FiringRate);
        }
	
        return FiringRate;
    }

    inline auto CommitExecuteWeapon(UObject* Ability, UFunction*, [[maybe_unused]] void* Parameters)
    {
        if (!Ability)
        {
            return false;
        }
        
        UObject* Pawn;
        Ability->ProcessEvent("GetActivatingPawn", &Pawn);
        if (!Pawn)
        {
            return false;
        }

        const auto CurrentWeapon = *Pawn->Member<UObject*>("CurrentWeapon");

        if (!CurrentWeapon)
        {
            return false;
        }
        
        double TimeToNextFire = 0;
        CurrentWeapon->ProcessEvent("GetTimeToNextFire", &TimeToNextFire);
        return TimeToNextFire > 0.02;
    }
}