#pragma once
#include <Gameplay/helper.h>
#include <UE/structs.h>
#include <UE/other.h>

namespace Henchmans
{
	void SpawnHenchmans()
	{
		static auto BPC = FindObject("Class /Script/Engine.BlueprintGeneratedClass");
		//auto HenchmanSpawnerClass = FindObject("BlueprintGeneratedClass /Game/Athena/AI/MANG/BP_MANG_Spawner_C.BP_MANG_Spawner_C");
		auto HenchmanSpawner = StaticLoadObject(BPC, nullptr, "/Game/Athena/AI/MANG/BP_MANG_Spawner_C.BP_MANG_Spawner_C");
		Easy::SpawnActor(HenchmanSpawner);

		auto SetupTurretsFn = HenchmanSpawner->Function("SetupTurrets");

		bool CO = true;

		if (HenchmanSpawner)
			HenchmanSpawner->ProcessEvent(SetupTurretsFn, &CO);
	}
	void OpenVaults()
	{
		static auto AgencyVault = FindObject("BlueprintGeneratedClass /Game/Athena/Items/EnvironmentalItems/Locks/Keycard/Actors/Locks/BGA_Athena_Keycard_Lock_TheAgency.BGA_Athena_Keycard_Lock_TheAgency_C");

		static auto OpenVaultFn = FindObject("Function /Game/Athena/Items/EnvironmentalItems/Locks/Keycard/Actors/Locks/BGA_Athena_Keycard_Lock_Parent.BGA_Athena_Keycard_Lock_Parent_C.CallOpenVault");
		//bool CO = true;


		if (AgencyVault)
			AgencyVault->ProcessEvent(OpenVaultFn);
	}
}