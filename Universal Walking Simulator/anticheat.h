#pragma once

#include <UE/structs.h>

//(TODO)

/*

Function /Script/FortniteGame.FortWeapon.GetReloadTime
[55798] FloatProperty /Script/FortniteGame.FortWeapon.LastSuccessfulReloadTime
[55799] FloatProperty /Script/FortniteGame.FortWeapon.LastReloadTime

*/

float GetFireRate(UObject*)
{
	// GetFiringRate
}

// /Script/FortniteGame.FortWeapon.OnRep_LastFireTimeVerified 

// bool ValidShot(UObject* Pawn)
// bool ValidVehicleMove(UObject* Vehicle, whatevermovementclass Move) // We can check the velocity for the vehicle and we can figure out the max velocity for each vehicle and if it goes over then make the user exit the vehicle.