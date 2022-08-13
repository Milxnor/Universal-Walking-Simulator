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
		auto HenchmanSpawners = StaticLoadObject(BPC, nullptr, "/Game/Athena/AI/MANG/BP_MANG_Spawner_C.BP_MANG_Spawner_C");
		
		if (!HenchmanSpawners)
		{
			std::cout << "Invalid HenchmanSpawners!\n";
			return;
		}

		// RespawnHenchmen
		// SetupCameras
		// ApplyReplicationFrequencyOptimizations

		auto MemberNames = GetMemberNames(HenchmanSpawners);

		for (auto& MemberName : MemberNames)
			std::cout << "MemberName: " << MemberName << '\n';

		/* auto Actors = Helper::GetAllActorsOfClass(HenchmanSpawners);

		for (int i = 0; i < Actors.Num(); i++)
		{
			auto HenchmanSpawner = Actors.At(i);

			if (HenchmanSpawner)
			{
				Easy::SpawnActor(Helper::GetActorLocation(HenchmanSpawner));
			}
		}

		auto SetupTurretsFn = HenchmanSpawner->Function("SetupTurrets");

		bool CO = true;

		if (HenchmanSpawner)
			HenchmanSpawner->ProcessEvent(SetupTurretsFn, &CO); */
	}

	void OpenVaults()
	{
		// static auto AgencyVault = FindObject("BlueprintGeneratedClass /Game/Athena/Items/EnvironmentalItems/Locks/Keycard/Actors/Locks/BGA_Athena_Keycard_Lock_TheAgency.BGA_Athena_Keycard_Lock_TheAgency_C");
		static auto AgencyVault = FindObjectOld(".PersistentLevel.BGA_SparklyJibblet_VaultDoor_2");

		// OnRep_HasBeenOpening

		static auto OpenVaultFn = AgencyVault->Function("OpeningTimeline"); // VaultOpen
		//bool CO = true;

		if (AgencyVault)
			AgencyVault->ProcessEvent(OpenVaultFn);
		else
			std::cout << "No vault!\n";
	}
}