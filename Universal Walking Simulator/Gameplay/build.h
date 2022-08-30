#pragma once

#include <Net/funcs.h>
#include <Gameplay/helper.h>
#include <Gameplay/inventory.h>

#include <unordered_map>

namespace Building {

	// Includes building and editing..

	struct IsDestroyedBitField {
		unsigned char bSurpressHealthBar : 1;
		unsigned char bCreateVerboseHealthLogs : 1;
		unsigned char bIsIndestructibleForTargetSelection : 1;
		unsigned char bDestroyed : 1;
		unsigned char bPersistToWorld : 1;
		unsigned char bRefreshFullSaveDataBeforeZoneSave : 1;
		unsigned char bBeingDragged : 1;
		unsigned char bRotateInPlaceGame : 1;
	};

	bool CanBuild(UObject* BuildingActor, bool bMirrored)
	{
		return true;

		// if (!bDoubleBuildFix)
			// return true;

		if (!BuildingActor)
			return false;

		auto MainLocation = Helper::GetActorLocation(BuildingActor);
		auto MainRot = Helper::GetActorRotation(BuildingActor);
		auto MainCellIdx = Helper::GetCellIndexFromLocation(MainLocation);

		auto StructuralSupportSystem = Helper::GetStructuralSupportSystem();

		/*

		TESTED:

		WouldBuildingBeStructurallySupportedByNeighbors
		StartActorRemovalBatch & StopActorRemovalBatch

		*/

		static auto BuildingTypeOffset = GetOffset(BuildingActor, "BuildingType");
		auto BuildingType = *(EFortBuildingType*)(__int64(BuildingActor) + BuildingTypeOffset);

		struct
		{
			TEnumAsByte<EFortBuildingType>              BuildingType;        // (Parm, ZeroConstructor, IsPlainOldData)
			FVector                                     WorldLocation;       // (ConstParm, Parm, OutParm, ReferenceParm, IsPlainOldData)
			FBuildingSupportCellIndex                   OutActorGridIndices; // (Parm, OutParm)
			FBuildingNeighboringActorInfo               OutNeighboringActors;                                     // (Parm, OutParm)
		} UBuildingStructuralSupportSystem_K2_GetNeighboringBuildingActors_Params{ BuildingType, MainLocation };

		static auto K2_GetNeighboringBuildingActors = StructuralSupportSystem->Function("K2_GetNeighboringBuildingActors");

		if (K2_GetNeighboringBuildingActors)
			StructuralSupportSystem->ProcessEvent(K2_GetNeighboringBuildingActors, &UBuildingStructuralSupportSystem_K2_GetNeighboringBuildingActors_Params);

		auto& NeighboringActors = UBuildingStructuralSupportSystem_K2_GetNeighboringBuildingActors_Params.OutNeighboringActors;

		if (BuildingType == EFortBuildingType::Wall)
		{
			for (int i = 0; i < NeighboringActors.NeighboringWallInfos.Num(); i++)
			{
				auto& WallInfo = NeighboringActors.NeighboringWallInfos.At(i);

				if (MainCellIdx == WallInfo.NeighboringCellIdx)
					return false;

				/* auto WallActor = WallInfo.NeighboringActor.Get();

				if (!WallActor)
					continue;

				if (Helper::GetActorLocation(WallActor) == MainLocation)
					return false; */
			}
		}

		return true;

		/* auto StructuralSupportSystem = Helper::GetStructuralSupportSystem();

		static auto K2_GetBuildingActorsInGridCell = StructuralSupportSystem->Function("K2_GetBuildingActorsInGridCell");

		struct
		{
			struct FVector                                     WorldLocation;       // (ConstParm, Parm, OutParm, ReferenceParm, IsPlainOldData)
			FBuildingGridActorFilter                    Filter;              // (ConstParm, Parm, OutParm, ReferenceParm)
			FBuildingNeighboringActorInfo               OutActorsInGridCell; // (Parm, OutParm)
			bool          ReturnValue;         // (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData)
		} UBuildingStructuralSupportSystem_K2_GetBuildingActorsInGridCell_Params{ Helper::GetActorLocation(BuildingActor), FBuildingGridActorFilter{true, true, true, true}};

		if (K2_GetBuildingActorsInGridCell)
			StructuralSupportSystem->ProcessEvent(K2_GetBuildingActorsInGridCell, &UBuildingStructuralSupportSystem_K2_GetBuildingActorsInGridCell_Params);

		std::cout << "Neighboring Wall Size: " << UBuildingStructuralSupportSystem_K2_GetBuildingActorsInGridCell_Params.OutActorsInGridCell.NeighboringWallInfos.Num() << '\n';
		std::cout << "Neighboring Floor Size: " << UBuildingStructuralSupportSystem_K2_GetBuildingActorsInGridCell_Params.OutActorsInGridCell.NeighboringFloorInfos.Num() << '\n';
		std::cout << "Neighboring CenterCell Size: " << UBuildingStructuralSupportSystem_K2_GetBuildingActorsInGridCell_Params.OutActorsInGridCell.NeighboringCenterCellInfos.Num() << '\n';

		auto MainCellIdx = Helper::GetCellIndexFromLocation(MainLocation);

		auto HandleNeighbor = [](UObject* NeighboringActor) -> bool {

		};

		auto NewBuildingType = *(EFortBuildingType*)(__int64(BuildingActor) + BuildingTypeOffset);

		if (NewBuildingType == EFortBuildingType::Wall)
		{
			auto& WallInfos = UBuildingStructuralSupportSystem_K2_GetBuildingActorsInGridCell_Params.OutActorsInGridCell.NeighboringWallInfos;
			for (int i = 0; i < WallInfos.Num(); i++)
			{
				auto& CurrentWallInfo = WallInfos.At(i);

				auto WallActor = CurrentWallInfo.NeighboringActor.Get();

				if (CurrentWallInfo.NeighboringCellIdx == MainCellIdx)
				{
					// auto bSameRotation = CurrentWallInfo.WallPosition == BuildingActor->WallPosition;
					auto bSameRotation = Helper::GetActorRotation(WallActor) == MainRot;

					if (bSameRotation)
						return false;
				}
			}
		}
		else if (NewBuildingType == EFortBuildingType::Floor) // depending on the like rotation or idfk it osmetiems double builds
		{
			auto& FloorInfos = UBuildingStructuralSupportSystem_K2_GetBuildingActorsInGridCell_Params.OutActorsInGridCell.NeighboringFloorInfos;
			for (int i = 0; i < FloorInfos.Num(); i++)
			{
				auto& CurrentFloorInfo = FloorInfos.At(i);

				auto FloorActor = CurrentFloorInfo.NeighboringActor.Get();

				if (CurrentFloorInfo.NeighboringCellIdx == MainCellIdx)
				{
					// auto bSameRotation = CurrentWallInfo.WallPosition == BuildingActor->WallPosition;
					auto bSameRotation = Helper::GetActorRotation(FloorActor) == MainRot;

					if (bSameRotation)
						return false;
				}
			}
		}
		else */ // if (NewBuildingType == EFortBuildingType::GenericCenterCellActor)
		{
			bool bCanBuild = true;

			for (int i = 0; i < ExistingBuildings.size(); i++) // (const auto Building : ExistingBuildings)
			{
				auto Building = ExistingBuildings[i];

				if (!Building)
					continue;

				// TODO: Test the code below!

				// if (Building->Member<IsDestroyedBitField>(("bDestroyed"))->bDestroyed)
					// sExistingBuildings.erase(ExistingBuildings.begin() + i);

				if ((Helper::GetActorLocation(Building) == Helper::GetActorLocation(BuildingActor)) &&
					(*(EFortBuildingType*)(__int64(Building) + BuildingTypeOffset) == BuildingType))
				{
					bCanBuild = false;
				}
			}

			if (bCanBuild || ExistingBuildings.size() == 0)
			{
				ExistingBuildings.push_back(BuildingActor);

				return true;
			}

			return false;

			/* auto& CenterCellInfos = UBuildingStructuralSupportSystem_K2_GetBuildingActorsInGridCell_Params.OutActorsInGridCell.NeighboringCenterCellInfos;
			for (int i = 0; i < CenterCellInfos.Num(); i++)
			{
				auto& CurrentCenterInfo = CenterCellInfos.At(i);

				if (CurrentCenterInfo.NeighboringCellIdx == MainCellIdx)
					return false;
			} */
		}

		return true;
	}

	inline bool ServerCreateBuildingActorHook(UObject* Controller, UFunction* Function, void* Parameters)
	{
		if (Controller && Parameters)
		{
			static auto WoodItemData = FindObject(("FortResourceItemDefinition /Game/Items/ResourcePickups/WoodItemData.WoodItemData"));
			static auto StoneItemData = FindObject(("FortResourceItemDefinition /Game/Items/ResourcePickups/StoneItemData.StoneItemData"));
			static auto MetalItemData = FindObject(("FortResourceItemDefinition /Game/Items/ResourcePickups/MetalItemData.MetalItemData"));

			bool bSuccessful = false;
			UObject* MatDefinition = nullptr;

			if (FnVerDouble > 8)
			{
				struct FCreateBuildingActorData
				{
					uint32_t BuildingClassHandle;
					struct FVector BuildLoc;
					struct FRotator BuildRot;
					bool bMirrored;
					unsigned char UnknownData00[0x3];
					float SyncKey;
					unsigned char UnknownData01[0x4];
					char pad[0x10];
				};

				struct SCBAParams { FCreateBuildingActorData CreateBuildingData; };
				auto Params = (SCBAParams*)Parameters;
				{
					auto CreateBuildingData = Params->CreateBuildingData;

					static auto BuildLocOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.CreateBuildingActorData"), ("BuildLoc"));
					static auto BuildRotOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.CreateBuildingActorData"), ("BuildRot"));
					static auto BuildingClassDataOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.CreateBuildingActorData"), ("bMirrored"));
					static auto bMirroredOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.CreateBuildingActorData"), ("BuildingClassData"));

					static auto BuildingClassOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.BuildingClassData"), ("BuildingClass"));

					auto Pawn = Helper::GetPawnFromController(Controller);

					// auto BuildingClassData = Get<__int64>(BuildingClassDataOffset, CreateBuildingData);

						// auto BuildingClass = Get<UObject*>(BuildingClassOffset, *BuildingClassData);
					static auto BroadcastRemoteClientInfoOffset = GetOffset(Controller, "BroadcastRemoteClientInfo");
					auto RemoteClientInfo = (UObject**)(__int64(Controller) + BroadcastRemoteClientInfoOffset);

					if (RemoteClientInfo && *RemoteClientInfo)
					{
						// auto BuildingClass = Controller->Member<UObject*>(("CurrentBuildableClass"));
						static auto RemoteBuildableClassOffset = GetOffset(*RemoteClientInfo, "RemoteBuildableClass");
						auto BuildingClass = (UObject**)(__int64(*RemoteClientInfo) + RemoteBuildableClassOffset);

						// std::cout << ("BuildLocation Offset: ") << BuildLocOffset << '\n';
						auto BuildLoc = Params->CreateBuildingData.BuildLoc; // (FVector*)(__int64(Params->CreateBuildingData) + BuildLocOffset)// Get<FVector>(BuildLocOffset, Params->CreateBuildingData);
						auto BuildRot = Params->CreateBuildingData.BuildRot; // Get<FRotator>(BuildRotOffset, Params->CreateBuildingData);
						// auto bMirrored = Get<bool>(bMirroredOffset, Params->CreateBuildingData);

						/*
						std::cout << ("BuildingClassOffset: ") << BuildingClassOffset << '\n';
						std::cout << ("BuildingClassData: ") << BuildingClassDataOffset << '\n';
						std::cout << ("BuildingClass: ") << BuildingClass << '\n';
						*/

						if (BuildingClass && *BuildingClass)
						{
							auto BuildingClassName = (*BuildingClass)->GetFullName();

							// TODO: figure out a better way

							if (BuildingClassName.contains(("W1")))
								MatDefinition = WoodItemData;
							else if (BuildingClassName.contains(("S1")))
								MatDefinition = StoneItemData;
							else if (BuildingClassName.contains(("M1")))
								MatDefinition = MetalItemData;

							if (MatDefinition)
							{
								auto MatInstance = Inventory::FindItemInInventory(Controller, MatDefinition);

								if (*FFortItemEntry::GetCount(GetItemEntryFromInstance(MatInstance)) < 10)
									return false;

								// std::cout << ("Goofy class: ") << (*BuildingClass)->GetFullName();
								UObject* BuildingActor = Easy::SpawnActor(*BuildingClass, BuildLoc, BuildRot); // Helper::GetActorLocation(Pawn), Helper::GetActorRotation(Pawn));

								if (BuildingActor)
								{
									if (CanBuild(BuildingActor, Params->CreateBuildingData.bMirrored))
									{
										if (bDoubleBuildFix)
											ExistingBuildings.push_back(BuildingActor);

										Helper::InitializeBuildingActor(Controller, BuildingActor, true);

										bSuccessful = true;

										// if (!Helper::IsStructurallySupported(BuildingActor))
											// bSuccessful = false;
									}
									else
										bSuccessful = false;

									if (!bSuccessful)
									{
										Helper::SetActorScale3D(BuildingActor, {});
										Helper::SilentDie(BuildingActor);
									}
								}
							}
						}
						else
							std::cout << ("Unable to get BuildingClass!\n");
					}
				}
			}
			else
			{
				struct FBuildingClassData {
					UObject* BuildingClass;
					int PreviousBuildingLevel;
					int UpgradeLevel;
				};

				struct SCBAParams {
					FBuildingClassData BuildingClassData;
					FVector BuildLoc;
					FRotator BuildRot;
					bool bMirrored;
					float SyncKey; // does this exist below 7.4
				};

				auto Params = (SCBAParams*)Parameters;

				// static auto BuildingClassOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.BuildingClassData"), ("BuildingClass"));

				// auto BuildingClass = Get<UObject*>(BuildingClassOffset, __int64(Params->BuildingClassData));

				// auto RemoteClientInfo = Controller->Member<UObject*>(("BroadcastRemoteClientInfo"));

				// if (RemoteClientInfo && *RemoteClientInfo)
					// auto BuildingClass = *Controller->Member<UObject*>(("CurrentBuildableClass"));
					// auto BuildingClass = (*RemoteClientInfo)->Member<UObject*>(("RemoteBuildableClass"));

				auto BuildingClass = Params->BuildingClassData.BuildingClass;

				if (BuildingClass) // && *BuildingClass)
				{
					UObject* BuildingActor = Easy::SpawnActor(BuildingClass, Params->BuildLoc, Params->BuildRot); // Helper::GetActorLocation(Pawn), Helper::GetActorRotation(Pawn));

					if (BuildingActor)
					{
						if (CanBuild(BuildingActor, Params->bMirrored))
						{
							if (bDoubleBuildFix)
								ExistingBuildings.push_back(BuildingActor);

							Helper::InitializeBuildingActor(Controller, BuildingActor, true);

							bSuccessful = true;

							// if (!Helper::IsStructurallySupported(BuildingActor))
								// bSuccessful = false;
						}
						else
							bSuccessful = false;
					}
					else
						std::cout << ("Unable to summon the building!\n");

					// auto BuildingClassName = BuildingClass->GetFullName();

					auto ResourceType = *BuildingActor->Member<TEnumAsByte<EFortResourceType>>(("ResourceType"));

					// TODO:" figure out a better way

					if (ResourceType.Get() == EFortResourceType::Wood)// (BuildingClassName.contains(("W1")))
						MatDefinition = WoodItemData;
					else if (ResourceType.Get() == EFortResourceType::Stone)// (BuildingClassName.contains(("S1")))
						MatDefinition = StoneItemData;
					else if (ResourceType.Get() == EFortResourceType::Metal) // (BuildingClassName.contains(("M1")))
						MatDefinition = MetalItemData;

					if (MatDefinition)
					{
						auto MatInstance = Inventory::FindItemInInventory(Controller, MatDefinition);

						if (*FFortItemEntry::GetCount(GetItemEntryFromInstance(MatInstance)) < 10)
							bSuccessful = false;
					}

					if (!bSuccessful && BuildingActor)
					{
						Helper::SetActorScale3D(BuildingActor, {});
						Helper::SilentDie(BuildingActor);
						return false;
					}
				}
				else
					std::cout << ("No BuildingClass!\n");
			}

			if (bSuccessful && !bIsPlayground)
			{
				if (MatDefinition)
					Inventory::DecreaseItemCount(Controller, MatDefinition, 10);
				else
					std::cout << ("Is bro using permanite!?!?!?");
			}
		}

		return false;
	}

	inline bool ServerBeginEditingBuildingActorHook(UObject* Controller, UFunction* Function, void* Parameters)
	{
		struct Parms { UObject* BuildingActor; };
		static UObject* EditToolDefinition = FindObject(("FortEditToolItemDefinition /Game/Items/Weapons/BuildingTools/EditTool.EditTool"));

		auto EditToolInstance = Inventory::FindItemInInventory(Controller, EditToolDefinition);

		auto Pawn = *Controller->Member<UObject*>(("Pawn"));

		UObject* BuildingToEdit = ((Parms*)Parameters)->BuildingActor;

		if (Controller && BuildingToEdit)
		{
			if (EditToolInstance)
			{
				auto EditTool = Inventory::EquipWeaponDefinition(Pawn, EditToolDefinition, Inventory::GetItemGuid(EditToolInstance));

				if (EditTool)
				{
					auto PlayerState = *Controller->Member<UObject*>(("PlayerState"));
					*BuildingToEdit->Member<UObject*>(("EditingPlayer")) = PlayerState;
					static auto OnRep_EditingPlayer = BuildingToEdit->Function(("OnRep_EditingPlayer"));

					if (OnRep_EditingPlayer)
						BuildingToEdit->ProcessEvent(OnRep_EditingPlayer);

					*EditTool->Member<UObject*>(("EditActor")) = BuildingToEdit;
					static auto OnRep_EditActor = EditTool->Function(("OnRep_EditActor"));

					if (OnRep_EditActor)
						EditTool->ProcessEvent(OnRep_EditActor);
				}
				else
					std::cout << "Failed to equip edit tool?\n";
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
			auto RotationIterations = *(int*)(__int64(Parameters) + RotationIterationsOffset);

			if (BuildingActor && NewBuildingClass)
			{
				IsDestroyedBitField* BitField = Params->BuildingActorToEdit->Member<IsDestroyedBitField>(("bDestroyed"));

				if (!BitField || BitField->bDestroyed || RotationIterations > 3)
					return false;

				auto Location = Helper::GetActorLocation(BuildingActor);
				auto Rotation = Helper::GetActorRotation(BuildingActor);

				auto BuildingSMActorReplaceBuildingActorAddr = FindPattern("4C 8B DC 55 57 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 8B 85 ? ? ? ? 33 FF 40 38 3D ? ? ? ?");

				if (Engine_Version >= 426 && !BuildingSMActorReplaceBuildingActorAddr)
					BuildingSMActorReplaceBuildingActorAddr = FindPattern("48 8B C4 48 89 58 18 55 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 70 B8 0F 29 78 A8 44 0F 29 40 ? 44 0F 29 48 ? 44 0F 29 90 ? ? ? ? 44 0F 29 B8 ? ? ? ? 48 8B 05");

				if (!BuildingSMActorReplaceBuildingActorAddr || Engine_Version <= 421)
					BuildingSMActorReplaceBuildingActorAddr = FindPattern("48 8B C4 44 89 48 20 55 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 48 89 70 E8 33 FF 40 38 3D ? ? ? ? 48 8B F1 4C 89 60 E0 44 8B E2");

				UObject* (__fastcall * BuildingSMActorReplaceBuildingActor)(UObject * BuildingSMActor, unsigned int a2, UObject * a3, unsigned int a4, int a5, unsigned __int8 bMirrored, UObject * Controller);

				BuildingSMActorReplaceBuildingActor = decltype(BuildingSMActorReplaceBuildingActor)(BuildingSMActorReplaceBuildingActorAddr);

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
			// TODO: Check if the controller is in aircraft, if they edit on spawn island, it will make them end on the battle bus, which will not go well.

			struct Parms {
				UObject* BuildingActorToStopEditing;
			};

			auto Params = (Parms*)Parameters;

			auto Pawn = Controller->Member<UObject*>(("Pawn"));

			if (Pawn && *Pawn)
			{
				auto CurrentWep = (*Pawn)->Member<UObject*>(("CurrentWeapon"));

				if (CurrentWep && *CurrentWep)
				{
					auto CurrentWepItemDef = *(*CurrentWep)->Member<UObject*>(("WeaponData"));
					static UObject* EditToolDefinition = FindObject(("FortEditToolItemDefinition /Game/Items/Weapons/BuildingTools/EditTool.EditTool"));

					if (CurrentWepItemDef == EditToolDefinition) // Player CONFIRMED the edit
					{
						// auto EditToolInstance = Inventory::FindItemInInventory(Controller, EditToolDefinition);
						auto EditTool = *CurrentWep;// Inventory::EquipWeaponDefinition(*Pawn, EditToolDefinition, Inventory::GetItemGuid(EditToolInstance));

						*EditTool->Member<bool>(("bEditConfirmed")) = true;
						*EditTool->Member<UObject*>(("EditActor")) = nullptr;
						static auto OnRep_EditActorFn = EditTool->Function(("OnRep_EditActor"));

						if (OnRep_EditActorFn)
							EditTool->ProcessEvent(OnRep_EditActorFn);
					}
				}

				if (Params->BuildingActorToStopEditing)
				{
					*Params->BuildingActorToStopEditing->Member<UObject*>(("EditingPlayer")) = nullptr;
					static auto OnRep_EditingPlayer = Params->BuildingActorToStopEditing->Function(("OnRep_EditingPlayer"));

					if (OnRep_EditingPlayer)
						Params->BuildingActorToStopEditing->ProcessEvent(OnRep_EditingPlayer);
				}
			}
		}

		return false;
	}

	void InitializeBuildHooks()
	{
		AddHook(("Function /Script/FortniteGame.FortPlayerController.ServerCreateBuildingActor"), ServerCreateBuildingActorHook);
		AddHook(("Function /Script/FortniteGame.FortPlayerController.ServerBeginEditingBuildingActor"), ServerBeginEditingBuildingActorHook);
		AddHook(("Function /Script/FortniteGame.FortPlayerController.ServerEditBuildingActor"), ServerEditBuildingActorHook);
		AddHook(("Function /Script/FortniteGame.FortPlayerController.ServerEndEditingBuildingActor"), ServerEndEditingBuildingActorHook);
	}
}