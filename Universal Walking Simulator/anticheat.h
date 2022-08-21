#pragma once

#include <UE/structs.h>

#include <windows.h>

// bool ValidShot(UObject* Pawn)

//bool ValidVehicleMove()
//{
//    static auto ptr = UObject::FindClass("BlueprintGeneratedClass VehicleObject.VehicleObject_C"); // Finds class
//    return ptr;
//}

float GetFireRate(UObject*)
{
    // TODO: Add fire rate projector
}

class AFortWeapon {
public:
    AFortItemDefinition* WeaponData() {
        if (!valid_pointer((uintptr_t)this)) return 0;
        return (AFortItemDefinition*)(*(uintptr_t)(this + 0x380)); // Offset: 0x380 is dynamic
    }

    float GetWeaponBulletSpeed(float speed)
    {
        if (!valid_pointer((uintptr_t)this)) return 0;
        AFortWeapon_GetProjectileSpeed_Params params;
        ProcessEvent((uintptr_t)this, GetProjectileSpeed, &params);
        return params.ReturnValue;
    }

    AFortWeapon* GetReloadTime() {
        if (!valid_pointer((uintptr_t)this)) return 0;
        uintptr_t result;
        ProcessEvent((uintptr_t)this, GetReloadTime, &result);
        return (AFortWeapon*)result;
    }
};

class AFortPawn {
public:
    const char* ObjectName() {
        if (!valid_pointer((uintptr_T)this)) return "";
        return GetUObjectName((uintptr_t*)this);
    }

    AFortWeapon* CurrentWeapon() {
        if (!valid_pointer((uintptr_t)this)) return 0;
        return (AFortWeapon*)(*(uintptr_t)(this + 0x5E8));
    }

    BOOL IsInVehicle() { // Check for player in vehicle
        if (!valid_pointer((uintptr_t)this)) return 0;
        bool result;
        ProcessEvent((uintptr_t)this, IsInVehicle, &result);
        return result;
    }

    Vector3 GetAcceleration()
    {
        if (!valid_pointer((uintptr_t)this)) return {0, 0, 0};
        auto char_movement = *(uintptr_t*)(uintptr_t(this) + 0x288); // DGOffset_Movement
        if (valid_pointer(char_movement)) {
            return *(Vector3*)(char_movement + 0x24C); // DGOffset_Acceleration
        }
        else {
            return {0, 0, 0};
        }
    }

    AFortPawn* GetVehicle() { // Gets player vehicle
        if (!valid_pointer((uintptr_t)this)) return 0;
        uintptr_t result;
        ProcessEvent((uintptr_t)this, GetVehicle, &result);
        return (AFortPawn*)result;
    }
};
