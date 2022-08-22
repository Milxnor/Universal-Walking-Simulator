#pragma once

#include <UE/structs.h>

//(TODO)

/*

Function /Script/FortniteGame.FortWeapon.GetReloadTime
[55798] FloatProperty /Script/FortniteGame.FortWeapon.LastSuccessfulReloadTime
[55799] FloatProperty /Script/FortniteGame.FortWeapon.LastReloadTime

*/

static float GetFireRate(UObject* Weapon)
{
	// in 0.00 of a second
	// like 0.0055
	float FiringRate = 0;
	
	static auto GetFiringRate = Weapon->Function("GetFiringRate");

	if (GetFiringRate)
		Weapon->ProcessEvent(GetFiringRate, &FiringRate);

	return FiringRate;
}

bool commitExecuteWeapon(UObject* Ability, UFunction*, void* Parameters)
{
	std::cout << "execute\n";

	if (Ability)
	{
		UObject* Pawn; // Helper::GetOwner(ability);
		Ability->ProcessEvent("GetActivatingPawn", &Pawn);

		if (Pawn)
		{
			std::cout << "pawn: " << Pawn->GetFullName() << '\n';
			auto currentWeapon = *Pawn->Member<UObject*>("CurrentWeapon");

			if (currentWeapon)
			{
				float TimeToNextFire = 0;

				currentWeapon->ProcessEvent("GetTimeToNextFire", &TimeToNextFire);

				auto LastFireTime = *currentWeapon->Member<float>("LastFireTime"); // in seconds since game has started
				/* auto TimeSeconds = Helper::GetTimeSeconds();

				std::cout << "LastFireTime: " << LastFireTime << '\n';
				std::cout << "TimeToNextFire: " << TimeToNextFire << '\n';
				std::cout << "FIirng Rate: " << GetFireRate(currentWeapon) << '\n';
				std::cout << "Time Seconds: " << TimeSeconds << '\n';
				std::cout << "Time Seconds - LastFireTime: " << TimeSeconds - LastFireTime << '\n'; */

				if (TimeToNextFire > 0.02)
				{
					std::cout << "Player is demospeeding!\n";
					return true;
				}
			}
			else
				std::cout << "No CurrentWeapon!\n";
		}
	}

	return false;
}

// /Script/FortniteGame.FortWeapon.OnRep_LastFireTimeVerified 

// bool ValidShot(UObject* Pawn)
// bool ValidVehicleMove(UObject* Vehicle, whatevermovementclass Move) // We can check the velocity for the vehicle and we can figure out the max velocity for each vehicle and if it goes over then make the user exit the vehicle.