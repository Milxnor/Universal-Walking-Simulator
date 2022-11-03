#pragma once
#include <Gameplay/helper.h>
#include <UE/structs.h>
#include <UE/other.h>

namespace FortAI
{
	void SpawnHenchmans()
	{
		auto world = Helper::GetWorld();
		auto AuthGameMode = Helper::GetGameMode();

		auto HenchmanSpawnerClass = LoadObject(Helper::GetBGAClass(), nullptr, "/Game/Athena/AI/MANG/BP_MANG_Spawner.BP_MANG_Spawner");
		auto HenchmanClass = LoadObject(Helper::GetBGAClass(), nullptr, "/Game/Athena/AI/MANG/BP_MangPlayerPawn_Boss.BP_MangPlayerPawn_Boss");

		std::cout << "Henchman Class: " << HenchmanClass << '\n';
		std::cout << "HenchmanSpawnerClass Class: " << HenchmanSpawnerClass << '\n';

		auto AllSpawners = Helper::GetAllActorsOfClass(HenchmanSpawnerClass);

		std::cout << "Henchman Spawners: " << AllSpawners.Num() << '\n';

		for (int i = 0; i < AllSpawners.Num(); i++)
		{
			auto CurrentSpawner = AllSpawners.At(i);

			if (CurrentSpawner)
			{
				auto Henchman = Easy::SpawnActor(HenchmanClass, Helper::GetActorLocation(CurrentSpawner));
			}
		}

		AllSpawners.Free();
	}

	void OpenVaults()
	{
		auto AgencyVault = FindObjectOld("PersistentLevel.BGA_Athena_Keycard_Lock_TheAgency_");
		auto SharkVault = FindObjectOld("PersistentLevel.BGA_Athena_Keycard_Lock_SharkIsland_");
		auto OilRigVault = FindObjectOld("PersistentLevel.BGA_Athena_Keycard_Lock_OilRig_");
		auto UndergroundBaseVault = FindObjectOld("PersistentLevel..BGA_Athena_Keycard_Lock_UndergroundBase_");
		auto YachtVault = FindObjectOld("PersistentLevel.BGA_Athena_Keycard_Lock_Yacht");

		static auto OpenVaultFn = FindObject("Function /Game/Athena/Items/EnvironmentalItems/Locks/Keycard/Actors/Locks/BGA_Athena_Keycard_Lock_Parent.BGA_Athena_Keycard_Lock_Parent_C.CallOpenVault");
		//bool CO = true;

		if (AgencyVault)
			AgencyVault->ProcessEvent(OpenVaultFn);

		if (YachtVault)
			YachtVault->ProcessEvent(OpenVaultFn);

		if (SharkVault)
			SharkVault->ProcessEvent(OpenVaultFn);

		if (UndergroundBaseVault)
			UndergroundBaseVault->ProcessEvent(OpenVaultFn);

		if (OilRigVault)
			OilRigVault->ProcessEvent(OpenVaultFn);
	}
}