#pragma once

#include <Gameplay/helper.h>
#include <Gameplay/inventory.h>

//(TODO)

/*

Function /Script/FortniteGame.FortWeapon.GetReloadTime
[55798] FloatProperty /Script/FortniteGame.FortWeapon.LastSuccessfulReloadTime
[55799] FloatProperty /Script/FortniteGame.FortWeapon.LastReloadTime

*/

static float GetFireRate(UObject* Weapon)
{
	float FiringRate = 0;
	
	static auto GetFiringRate = Weapon->Function("GetFiringRate");

	if (GetFiringRate)
		Weapon->ProcessEvent(GetFiringRate, &FiringRate);

	return FiringRate;
}

bool basicLocationCheck(UObject* Actor, UObject* OtherActor, float FarthestPossible)
{
	if (Helper::GetDistanceTo(Actor, OtherActor) > FarthestPossible)
		return false;

	return true;
}

bool validBuild(UObject* BuildingActor, UObject* Pawn)
{
	auto StructuralSupportSystem = Helper::GetStructuralSupportSystem();

	bool worldLocationValid = false;
	bool locationValid = false;

	if (StructuralSupportSystem)
	{
		static auto IsWorldLocValid = StructuralSupportSystem->Function("IsWorldLocValid");

		struct { FVector Loc; bool ret; } parms{ Helper::GetActorLocation(BuildingActor) };

		if (IsWorldLocValid)
			StructuralSupportSystem->ProcessEvent(IsWorldLocValid, &parms);

		worldLocationValid = parms.ret;
	}

	if (Pawn)
	{
		locationValid = basicLocationCheck(Pawn, BuildingActor, 1150);
	}

	return worldLocationValid && locationValid;
}

bool validBuildingClass(UObject* BuildingActorClass)
{
	static auto BuildingActorClasses = *Helper::GetGameState()->Member<TArray<UObject*>>("BuildingActorClasses");

	static int LastResetNum = 0;

	if (LastResetNum != AmountOfRestarts)
	{
		LastResetNum = AmountOfRestarts;
		BuildingActorClasses = *Helper::GetGameState()->Member<TArray<UObject*>>("BuildingActorClasses");
	}

	// if (BuildingActorClasses)
	{
		for (int i = 0; i < BuildingActorClasses.Num(); i++)
		{
			auto currentClass = BuildingActorClasses.At(i);

			if (currentClass && currentClass == BuildingActorClass)
				return true;
		}
	}

	return false;
}