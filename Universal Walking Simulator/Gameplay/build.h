#pragma once

#include <Net/funcs.h>
#include <Gameplay/helper.h>
#include <Gameplay/inventory.h>

#include <unordered_map>

// Includes building and editing..

void SetBuildingActorTeam(UObject* BuildingActor, int NewTeamIndex)
{
	static auto TeamOffset = GetOffset(BuildingActor, "Team");

	if (TeamOffset != -1)
	{
		auto Team = (TEnumAsByte<EFortTeam>*)(__int64(BuildingActor) + TeamOffset);
		*Team = NewTeamIndex; // *PlayerState->Member<TEnumAsByte<EFortTeam>>("Team");
	}

	static auto Building_TeamIndexOffset = GetOffset(BuildingActor, "TeamIndex");

	if (Building_TeamIndexOffset != -1)
	{
		auto TeamIndex = (uint8_t*)(__int64(BuildingActor) + Building_TeamIndexOffset);
		*TeamIndex = NewTeamIndex;
	}
}

inline bool ServerCreateBuildingActorHook(UObject* Controller, UFunction* Function, void* Parameters)
{
	if (Controller && Parameters)
	{
		static auto WoodItemData = FindObject(("FortResourceItemDefinition /Game/Items/ResourcePickups/WoodItemData.WoodItemData"));
		static auto StoneItemData = FindObject(("FortResourceItemDefinition /Game/Items/ResourcePickups/StoneItemData.StoneItemData"));
		static auto MetalItemData = FindObject(("FortResourceItemDefinition /Game/Items/ResourcePickups/MetalItemData.MetalItemData"));

		UObject* MatDefinition = nullptr;
		UObject* MatInstance = nullptr;

		auto Pawn = Helper::GetPawnFromController(Controller);

		UObject* BuildingClass = nullptr;
		FVector BuildingLocation;
		FRotator BuildingRotation;
		bool bMirrored;

		if (FnVerDouble >= 8.30)
		{
			struct FCreateBuildingActorData
			{
				uint32_t                                           BuildingClassHandle;                                      // 0x0000(0x0004) (ZeroConstructor, Transient, IsPlainOldData)
				struct FVector				                       BuildLoc;                                                 // 0x0004(0x000C) (Transient)
				struct FRotator                                    BuildRot;                                                 // 0x0010(0x000C) (ZeroConstructor, Transient, IsPlainOldData)
				bool                                               bMirrored;                                                // 0x001C(0x0001) (ZeroConstructor, Transient, IsPlainOldData)
			};

			struct SCBAParams { FCreateBuildingActorData CreateBuildingData; };
			auto Params = (SCBAParams*)Parameters;

			static auto BroadcastRemoteClientInfoOffset = GetOffset(Controller, "BroadcastRemoteClientInfo");
			auto RemoteClientInfo = (UObject**)(__int64(Controller) + BroadcastRemoteClientInfoOffset);

			if (RemoteClientInfo && *RemoteClientInfo)
			{
				static auto RemoteBuildableClassOffset = GetOffset(*RemoteClientInfo, "RemoteBuildableClass");
				auto bBuildingClass = (UObject**)(__int64(*RemoteClientInfo) + RemoteBuildableClassOffset);

				BuildingClass = bBuildingClass ? *bBuildingClass : nullptr;
				BuildingLocation = Params->CreateBuildingData.BuildLoc;
				BuildingRotation = Params->CreateBuildingData.BuildRot;
				bMirrored = Params->CreateBuildingData.bMirrored;

				{
					static auto RemoteBuildingMaterialOffset = GetOffset(*RemoteClientInfo, "RemoteBuildingMaterial");
					auto RemoteBuildingMaterial = (TEnumAsByte<EFortResourceType>*)(__int64(*RemoteClientInfo) + RemoteBuildingMaterialOffset);

					switch (RemoteBuildingMaterial ? RemoteBuildingMaterial->Get() : EFortResourceType::None)
					{
					case EFortResourceType::Wood:
						MatDefinition = WoodItemData;
						break;
					case EFortResourceType::Stone:
						MatDefinition = StoneItemData;
						break;
					case EFortResourceType::Metal:
						MatDefinition = MetalItemData;
						break;
					}
				}
			}
		}
		else
		{
			struct FBuildingClassData {
				UObject* BuildingClass;
				int                                                PreviousBuildingLevel;                                    // 0x0008(0x0004) (ZeroConstructor, Transient, IsPlainOldData)
				int                                                UpgradeLevel;
			};

			struct SCBAParams {
				FBuildingClassData BuildingClassData; // FBuildingClassData&
				FVector BuildLoc;
				FRotator BuildRot;
				bool bMirrored;
			};

			auto Params = (SCBAParams*)Parameters;

			BuildingClass = Params->BuildingClassData.BuildingClass;
			BuildingLocation = Params->BuildLoc;
			BuildingRotation = Params->BuildRot;
			bMirrored = Params->bMirrored;
		}

		if (BuildingClass)
		{
			constexpr bool bUseAnticheat = false;

			if (bUseAnticheat && !validBuildingClass(BuildingClass))
				return false;

			{
				bool bCanBuild = true;

				if (CanBuild)
				{
					__int64 v32[2]{};
					char dababy;

					bCanBuild = !CanBuild(Helper::GetWorld(), BuildingClass, BuildingLocation, BuildingRotation, bMirrored, v32, &dababy);
				}

				if (bCanBuild)
				{
					UObject* BuildingActor = Easy::SpawnActor(BuildingClass, BuildingLocation, BuildingRotation, Pawn);

					if (BuildingActor)
					{
						if (!MatDefinition)
						{
							static auto ResourceTypeOffset = GetOffset(BuildingActor, "ResourceType");
							auto ResourceType = (TEnumAsByte<EFortResourceType>*)(__int64(BuildingActor) + ResourceTypeOffset);

							switch (ResourceType ? ResourceType->Get() : EFortResourceType::None)
							{
							case EFortResourceType::Wood:
								MatDefinition = WoodItemData;
								break;
							case EFortResourceType::Stone:
								MatDefinition = StoneItemData;
								break;
							case EFortResourceType::Metal:
								MatDefinition = MetalItemData;
								break;
							}
						}

						MatInstance = Inventory::FindItemInInventory(Controller, MatDefinition);

						bool bSuccessful = false;

						if (MatInstance && *FFortItemEntry::GetCount(GetItemEntryFromInstance(MatInstance)) >= 10 &&
							(!bUseAnticheat || validBuild(BuildingActor, Pawn)))
						{
							auto PlayerState = Helper::GetPlayerStateFromController(Controller);

							auto PlayersTeamIndex = *Teams::GetTeamIndex(PlayerState);;

							SetBuildingActorTeam(BuildingActor, PlayersTeamIndex);

							// Helper::SetMirrored(BuildingActor, bMirrored);
							Helper::InitializeBuildingActor(Controller, BuildingActor, true);

							bSuccessful = true;

							// if (!Helper::IsStructurallySupported(BuildingActor))
								// bSuccessful = false;
						}

						if (!bSuccessful)
						{
							Helper::SetActorScale3D(BuildingActor, {});
							Helper::SilentDie(BuildingActor);
						}
						else
						{
							if (!bIsPlayground)
							{
								Inventory::DecreaseItemCount(Controller, MatInstance, 10);
							}
						}
					}
				}
			}
		}
	}

	return false;
}

UObject** GetEditingPlayer(UObject* BuildingActor)
{
	static auto EditingPlayerOffset = GetOffset(BuildingActor, "EditingPlayer");

	return (UObject**)(__int64(BuildingActor) + EditingPlayerOffset);
}

UObject** GetEditActor(UObject* EditTool)
{
	static auto EditActorOffset = GetOffset(EditTool, "EditActor");

	return (UObject**)(__int64(EditTool) + EditActorOffset);
}

inline bool ServerBeginEditingBuildingActorHook(UObject* Controller, UFunction* Function, void* Parameters)
{
	struct Parms { UObject* BuildingActor; };
	static UObject* EditToolDefinition = FindObject(("FortEditToolItemDefinition /Game/Items/Weapons/BuildingTools/EditTool.EditTool"));

	auto EditToolInstance = Inventory::FindItemInInventory(Controller, EditToolDefinition);

	auto Pawn = Helper::GetPawnFromController(Controller);

	UObject* BuildingToEdit = ((Parms*)Parameters)->BuildingActor;

	if (Controller && BuildingToEdit)
	{
		// TODO: Check TeamIndex of BuildingToEdit and the Controller
		// TODO: Add distance check

		if (EditToolInstance)
		{
			auto EditTool = Inventory::EquipWeaponDefinition(Pawn, EditToolDefinition, Inventory::GetItemGuid(EditToolInstance));

			if (EditTool)
			{
				auto PlayerState = Helper::GetPlayerStateFromController(Controller);

				*GetEditActor(EditTool) = BuildingToEdit;
				*GetEditingPlayer(BuildingToEdit) = PlayerState;
			}
			else
				std::cout << "Failed to equip edittool??\n";
		}
		else
			std::cout << ("No Edit Tool Instance?\n");
	}

	return false;
}

inline bool ServerEditBuildingActorHook(UObject* Controller, UFunction* Function, void* Parameters)
{
	struct Parms {
		UObject* BuildingActorToEdit;
		UObject* NewBuildingClass;
		int RotationIterations;
		bool bMirrored;
	};

	auto Params = (Parms*)Parameters;

	if (Params && Controller)
	{
		auto BuildingActor = Params->BuildingActorToEdit;
		auto NewBuildingClass = Params->NewBuildingClass;

		static auto bMirroredOffset = FindOffsetStruct("Function /Script/FortniteGame.FortPlayerController.ServerEditBuildingActor", "bMirrored");
		auto bMirrored = *(bool*)(__int64(Parameters) + bMirroredOffset);

		static auto RotationIterationsOffset = FindOffsetStruct("Function /Script/FortniteGame.FortPlayerController.ServerEditBuildingActor", "RotationIterations");
		auto RotationIterations = FnVerDouble < 8.30 ? *(int*)(__int64(Parameters) + RotationIterationsOffset) : *(char*)(__int64(Parameters) + RotationIterationsOffset); // I HATE FORTNITE

		if (BuildingActor && NewBuildingClass)
		{
			// if (false && RotationIterations > 3)
				// return false;

			// std::cout << "rotation iterations: " << RotationIterations << '\n';

			static bool bFound = false;

			static auto BuildingSMActorReplaceBuildingActorAddr = FindPattern("4C 8B DC 55 57 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 8B 85 ? ? ? ? 33 FF 40 38 3D ? ? ? ?");

			if (!bFound)
			{
				bFound = true;

				if (Engine_Version >= 426 && !BuildingSMActorReplaceBuildingActorAddr)
					BuildingSMActorReplaceBuildingActorAddr = FindPattern("48 8B C4 48 89 58 18 55 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 70 B8 0F 29 78 A8 44 0F 29 40 ? 44 0F 29 48 ? 44 0F 29 90 ? ? ? ? 44 0F 29 B8 ? ? ? ? 48 8B 05");

				if (!BuildingSMActorReplaceBuildingActorAddr || Engine_Version <= 421)
					BuildingSMActorReplaceBuildingActorAddr = FindPattern("48 8B C4 44 89 48 20 55 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 48 89 70 E8 33 FF 40 38 3D ? ? ? ? 48 8B F1 4C 89 60 E0 44 8B E2");


				if (!BuildingSMActorReplaceBuildingActorAddr)
					BuildingSMActorReplaceBuildingActorAddr = FindPattern("48 8B C4 48 89 58 18 55 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 70 B8 0F 29 78 A8 44 0F 29 40 ? 44 0F 29 48 ? 44 0F 29 90 ? ? ? ? 44 0F 29 B8 ? ? ? ? 48 8B 05");

				if (!BuildingSMActorReplaceBuildingActorAddr) // c3
					BuildingSMActorReplaceBuildingActorAddr = FindPattern("48 8B C4 48 89 58 18 55 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 70 B8 0F 29 78 A8 44 0F 29 40 ? 44 0F 29 48 ? 44 0F 29 90 ? ? ? ? 48 8B 05 ? ? ? ?");
			}

			static UObject* (__fastcall* BuildingSMActorReplaceBuildingActor)(UObject* BuildingSMActor, unsigned int a2, UObject * a3, unsigned int a4, int a5, unsigned __int8 bMirrored, UObject* Controller)
				= decltype(BuildingSMActorReplaceBuildingActor)(BuildingSMActorReplaceBuildingActorAddr);

			if (BuildingSMActorReplaceBuildingActor)
				BuildingSMActorReplaceBuildingActor(BuildingActor, 1, NewBuildingClass, 0, RotationIterations, bMirrored, Controller);
			else
				std::cout << "No BuildingSMActorReplaceBuildingActor!\n";
		}
	}

	return false;
}


inline bool ServerEndEditingBuildingActorHook(UObject* Controller, UFunction* Function, void* Parameters)
{
	if (Controller && Parameters && !Helper::IsInAircraft(Controller))
	{
		struct Parms {
			UObject* BuildingActorToStopEditing;
		};

		auto Params = (Parms*)Parameters;

		auto Pawn = Helper::GetPawnFromController(Controller);

		// TODO: Check BuildingActorToStopEditing->EditingPlayer and make sure its our player.

		if (Pawn)
		{
			auto CurrentWep = Helper::GetCurrentWeapon(Pawn);

			if (CurrentWep)
			{
				auto CurrentWepItemDef = Helper::GetWeaponData(CurrentWep);
				static UObject* EditToolDefinition = FindObject(("FortEditToolItemDefinition /Game/Items/Weapons/BuildingTools/EditTool.EditTool"));

				if (CurrentWepItemDef == EditToolDefinition) // Player CONFIRMED the edit
				{
					// auto EditToolInstance = Inventory::FindItemInInventory(Controller, EditToolDefinition);
					auto EditTool = CurrentWep;// Inventory::EquipWeaponDefinition(*Pawn, EditToolDefinition, Inventory::GetItemGuid(EditToolInstance));
					
					// static auto bEditConfirmedOffset = GetOffset(EditTool, "bEditConfirmed");
					// *(bool*)(__int64(EditTool) + bEditConfirmedOffset) = true;
					*GetEditActor(EditTool) = nullptr;
				}
			}
		}

		if (Params->BuildingActorToStopEditing)
		{
			*GetEditingPlayer(Params->BuildingActorToStopEditing) = nullptr;
		}
	}

	return false;
}

inline bool ServerRepairBuildingActorHook(UObject* Controller, UFunction* Function, void* Parameters)
{
	auto BuildingActorToRepair = *(UObject**)Parameters;

	if (Controller && BuildingActorToRepair)
	{
		static auto RepairBuilding = Controller->Function("RepairBuilding");

		// BuildingRepairCostMultiplierHandles

		int ResourcesSpent = 0;

		struct { UObject* Controller; int ResourcesSpent; } parms{Controller, ResourcesSpent };

		BuildingActorToRepair->ProcessEvent(RepairBuilding, &parms);
	}

	return false;
}

inline bool ServerSpawnDecoHook(UObject* DecoTool, UFunction*, void* Parameters)
{
	if (DecoTool && Parameters)
	{
		// DecoTool->TryToPlace // TODO: Try

		struct parmas {
			FVector Location; 
			FRotator Rotation;
			UObject* AttachedActor;
			// TEnumAsByte<EBuildingAttachmentType> InBuildingAttachmentType
		};

		auto Params = (parmas*)Parameters;

		auto Pawn = Helper::GetOwner(DecoTool);
		auto Controller = Helper::GetControllerFromPawn(Pawn);

		static auto ItemDefinitionOffset = GetOffset(DecoTool, "ItemDefinition");
		auto TrapItemDefinition = *(UObject**)(__int64(DecoTool) + ItemDefinitionOffset);

		// Inventory::DecreaseItemCount(Controller, Inventory::FindItemInInventory(Controller, TrapItemDefinition), 1);

		static auto GetBlueprintClass = TrapItemDefinition->Function("GetBlueprintClass");
		UObject* BlueprintClass = nullptr;
		TrapItemDefinition->ProcessEvent(GetBlueprintClass, &BlueprintClass);

		if (!BlueprintClass)
			return false;

		auto NewTrap = Easy::SpawnActor(BlueprintClass, Params->Location, Params->Rotation);

		if (!NewTrap)
			return false;

		Helper::InitializeBuildingActor(Controller, NewTrap);

		static auto AttachedToOffset = GetOffset(NewTrap, "AttachedTo");
		auto AttachedTo = (UObject**)(__int64(NewTrap) + AttachedToOffset);

		if (AttachedTo)
			*AttachedTo = Params->AttachedActor;

		// BuildingActor->BuildingAttachmentType = Params->InBuildingAttachmentType;

		SetBuildingActorTeam(NewTrap, *Teams::GetTeamIndex(Helper::GetPlayerStateFromController(Controller)));

		static auto TrapDataOffset = GetOffset(NewTrap, "TrapData");
		auto TrapData = (UObject**)(__int64(NewTrap) + TrapDataOffset);

		if (TrapData)
			*TrapData = TrapItemDefinition; // probably useless
	}

	return false;
}

inline bool ServerCreateBuildingAndSpawnDecoHook(UObject* DecoTool, UFunction*, void* Parameters) // ofc they change this (Params)
{
	if (DecoTool)
	{

	}
}

void InitializeBuildHooks()
{
	// if (Engine_Version < 426)
	if (std::floor(FnVerDouble) != 14)
	{
		// AddHook("Function /Script/FortniteGame.FortDecoTool.ServerCreateBuildingAndSpawnDeco", ServerCreateBuildingAndSpawnDecoHook);
		AddHook("Function /Script/FortniteGame.FortDecoTool.ServerSpawnDeco", ServerSpawnDecoHook);
		AddHook(("Function /Script/FortniteGame.FortPlayerController.ServerCreateBuildingActor"), ServerCreateBuildingActorHook);
		// AddHook("Function /Script/FortniteGame.FortPlayerController.ServerRepairBuildingActor", ServerRepairBuildingActorHook);

		// if (Engine_Version < 424)
		// if (std::floor(FnVerDouble) != 15)
		if (Engine_Version != 420)
		{
			AddHook(("Function /Script/FortniteGame.FortPlayerController.ServerBeginEditingBuildingActor"), ServerBeginEditingBuildingActorHook);
			AddHook(("Function /Script/FortniteGame.FortPlayerController.ServerEditBuildingActor"), ServerEditBuildingActorHook);
			AddHook(("Function /Script/FortniteGame.FortPlayerController.ServerEndEditingBuildingActor"), ServerEndEditingBuildingActorHook);
		}
	}
}