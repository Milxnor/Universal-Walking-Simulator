#pragma once

#include <Net/funcs.h>
#include <Gameplay/helper.h>
#include <Gameplay/inventory.h>

#include <unordered_map>

// Includes building and editing..

struct IsDestroyedBitField {
	unsigned char                                      bSurpressHealthBar : 1;                                   // 0x0541(0x0001) (Edit, BlueprintVisible, BlueprintReadOnly, DisableEditOnInstance)
	unsigned char                                      bCreateVerboseHealthLogs : 1;                             // 0x0541(0x0001) (Edit, BlueprintVisible, BlueprintReadOnly, DisableEditOnInstance)
	unsigned char                                      bIsIndestructibleForTargetSelection : 1;                  // 0x0541(0x0001) (Edit, BlueprintVisible, BlueprintReadOnly, DisableEditOnInstance)
	unsigned char                                      bDestroyed : 1;                                           // 0x0541(0x0001) (BlueprintVisible, BlueprintReadOnly, Net, Transient)
	unsigned char                                      bPersistToWorld : 1;                                      // 0x0541(0x0001) (Edit, DisableEditOnInstance)
	unsigned char                                      bRefreshFullSaveDataBeforeZoneSave : 1;                   // 0x0541(0x0001) (Edit, DisableEditOnInstance)
	unsigned char                                      bBeingDragged : 1;                                        // 0x0541(0x0001) (Transient)
	unsigned char                                      bRotateInPlaceGame : 1;                                   // 0x0541(0x0001)
};

bool CanBuild(UObject* BuildingActor)
{
	return true;

	if (!BuildingActor)
		return false;

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
			(*Building->Member<EFortBuildingType>(("BuildingType")) == *BuildingActor->Member<EFortBuildingType>(("BuildingType"))))
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

		static const auto FnVerDouble = std::stod(FN_Version);

		if (FnVerDouble > 8)
		{
			struct FCreateBuildingActorData
			{
				uint32_t                                           BuildingClassHandle;                                      // 0x0000(0x0004) (ZeroConstructor, Transient, IsPlainOldData)
				struct FVector				                       BuildLoc;                                                 // 0x0004(0x000C) (Transient)
				struct FRotator                                    BuildRot;                                                 // 0x0010(0x000C) (ZeroConstructor, Transient, IsPlainOldData)
				bool                                               bMirrored;                                                // 0x001C(0x0001) (ZeroConstructor, Transient, IsPlainOldData)
				unsigned char                                      UnknownData00[0x3];                                       // 0x001D(0x0003) MISSED OFFSET
				float                                              SyncKey;                                                  // 0x0020(0x0004) (ZeroConstructor, Transient, IsPlainOldData)
				unsigned char                                      UnknownData01[0x4];                                       // 0x0024(0x0004) MISSED OFFSET
				// struct FBuildingClassData                          BuildingClassData;                                        // 0x0028(0x0010) (Transient)
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

				auto Pawn = *Controller->Member<UObject*>(("Pawn"));

				// auto BuildingClassData = Get<__int64>(BuildingClassDataOffset, CreateBuildingData);

				// if (BuildingClassData)
				{
					// auto BuildingClass = Get<UObject*>(BuildingClassOffset, *BuildingClassData);
					auto RemoteClientInfo = Controller->Member<UObject*>(("BroadcastRemoteClientInfo"));

					if (RemoteClientInfo && *RemoteClientInfo)
					{
						// auto BuildingClass = Controller->Member<UObject*>(("CurrentBuildableClass"));
						auto BuildingClass = (*RemoteClientInfo)->Member<UObject*>(("RemoteBuildableClass"));

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

								if (*FFortItemEntry::GetCount(MatInstance->Member<__int64>(("ItemEntry"))) < 10)
									return false;

								// std::cout << ("Goofy class: ") << (*BuildingClass)->GetFullName();
								UObject* BuildingActor = Easy::SpawnActor(*BuildingClass, BuildLoc, BuildRot); // Helper::GetActorLocation(Pawn), Helper::GetActorRotation(Pawn));

								if (BuildingActor)
								{
									if (CanBuild(BuildingActor))
									{
										Helper::InitializeBuildingActor(Controller, BuildingActor, true);

										bSuccessful = true;

										if (!Helper::IsStructurallySupported(BuildingActor))
											bSuccessful = false;
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
					else
						std::cout << ("Unable to get RemoteClientInfo!\n");
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
				float SyncKey; // does this exist below 7.4
			};

			auto Params = (SCBAParams*)Parameters;

			// static auto BuildingClassOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.BuildingClassData"), ("BuildingClass"));

			// auto BuildingClass = Get<UObject*>(BuildingClassOffset, __int64(Params->BuildingClassData));
			
			// auto RemoteClientInfo = Controller->Member<UObject*>(("BroadcastRemoteClientInfo"));

			// if (RemoteClientInfo && *RemoteClientInfo)
			// if (false)
			{
				// auto BuildingClass = *Controller->Member<UObject*>(("CurrentBuildableClass"));
				// auto BuildingClass = (*RemoteClientInfo)->Member<UObject*>(("RemoteBuildableClass"));

				auto BuildingClass = Params->BuildingClassData.BuildingClass;

				if (BuildingClass) // && *BuildingClass)
				{
					auto BuildingClassName = BuildingClass->GetFullName();

					// TODO:" figure out a better way

					if (BuildingClassName.contains(("W1")))
						MatDefinition = WoodItemData;
					else if (BuildingClassName.contains(("S1")))
						MatDefinition = StoneItemData;
					else if (BuildingClassName.contains(("M1")))
						MatDefinition = MetalItemData;

					if (MatDefinition)
					{
						auto MatInstance = Inventory::FindItemInInventory(Controller, MatDefinition);

						if (*FFortItemEntry::GetCount(MatInstance->Member<__int64>(("ItemEntry"))) < 10)
							return false;

						UObject* BuildingActor = Easy::SpawnActor(BuildingClass, Params->BuildLoc, Params->BuildRot); // Helper::GetActorLocation(Pawn), Helper::GetActorRotation(Pawn));

						if (BuildingActor)
						{
							if (CanBuild(BuildingActor))
							{
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
						else
							std::cout << ("Unable to summon the building!\n");
					}
				}
				else
					std::cout << ("No BuildingClass!\n");
			}
		}

		if (bSuccessful && !bIsPlayground)
		{
			// TEnumAsByte<EFortResourceType>                     ResourceType;
			// auto ResourceType = *BuildingActor->Member<TEnumAsByte<EFortResourceType>>(("ResourceType"));

			if (MatDefinition)
				Inventory::DecreaseItemCount(Controller, MatDefinition, 10);
			else
				std::cout << ("Is bro using permanite!?!?!?");
		}
		// else
			// std::cout << ("failed to build!\n");
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
		}
		else
			std::cout << ("No Edit Tool Instance?\n");
	}

	return false;
}

inline bool ServerEditBuildingActorHook(UObject* Controller, UFunction* Function, void* Parameters)
{
	struct Parms {
		UObject* BuildingActorToEdit;                                      // (Parm, ZeroConstructor, IsPlainOldData)
		UObject* NewBuildingClass;                                         // (Parm, ZeroConstructor, IsPlainOldData)
		int                                                RotationIterations;                                       // (Parm, ZeroConstructor, IsPlainOldData)
		bool                                               bMirrored;                                                // (Parm, ZeroConstructor, IsPlainOldData)
	};

	auto Params = (Parms*)Parameters;

	if (Params && Controller)
	{
		auto BuildingActor = Params->BuildingActorToEdit;
		auto NewBuildingClass = Params->NewBuildingClass;
		auto RotationIterations = Params->RotationIterations;

		if (BuildingActor && NewBuildingClass)
		{
			IsDestroyedBitField* BitField = Params->BuildingActorToEdit->Member<IsDestroyedBitField>(("bDestroyed"));

			if (BitField && BitField->bDestroyed)
				return false;

			auto Location = Helper::GetActorLocation(BuildingActor);
			auto Rotation = Helper::GetActorRotation(BuildingActor);

			if (*BuildingActor->Member<EFortBuildingType>(("BuildingType")) != EFortBuildingType::Wall) // wtf this isnt working this line
			{
				int Yaw = (int(Rotation.Yaw) + 360) % 360; // credits: PRO100KatYT

				if (Yaw > 80 && Yaw < 100) // 90
				{
					if (RotationIterations == 1)
						Location = Location + FVector(-256, 256, 0);
					else if (RotationIterations == 2)
						Location = Location + FVector(-512, 0, 0);
					else if (RotationIterations == 3)
						Location = Location + FVector(-256, -256, 0);
				}
				else if (Yaw > 170 && Yaw < 190) // 180
				{
					if (RotationIterations == 1)
						Location = Location + FVector(-256, -256, 0);
					else if (RotationIterations == 2)
						Location = Location + FVector(0, -512, 0);
					else if (RotationIterations == 3)
						Location = Location + FVector(256, -256, 0);
				}
				else if (Yaw > 260 && Yaw < 280) // 270
				{
					if (RotationIterations == 1)
						Location = Location + FVector(256, -256, 0);
					else if (RotationIterations == 2)
						Location = Location + FVector(512, 0, 0);
					else if (RotationIterations == 3)
						Location = Location + FVector(256, 256, 0);
				}
				else // 0 - 360
				{
					if (RotationIterations == 1)
						Location = Location + FVector(256, 256, 0);
					else if (RotationIterations == 2)
						Location = Location + FVector(0, 512, 0);
					else if (RotationIterations == 3)
						Location = Location + FVector(-256, 256, 0);
				}
			}

			Rotation.Yaw += 90 * RotationIterations;

			auto Team = *Params->BuildingActorToEdit->Member<TEnumAsByte<uint8_t>>(("Team"));

			/* float BuildingHealth = 0;

			static auto GetHealth = Params->BuildingActorToEdit->Function(("GetHealth"));

			if (GetHealth)
				Params->BuildingActorToEdit->ProcessEvent(GetHealth, &BuildingHealth); */

			Helper::SilentDie(Params->BuildingActorToEdit);

			auto EditedActor = Easy::SpawnActor(Params->NewBuildingClass, Location, Rotation);

			if (EditedActor)
			{
				if (true) // !Helper::IsStructurallySupported(EditedActor))
				{
					Helper::SetOwner(EditedActor, Controller);

					static auto SetMirroredFn = EditedActor->Function(("SetMirrored"));

					struct { bool bMirrored; }mirroredParams{ Params->bMirrored };
					EditedActor->ProcessEvent(SetMirroredFn, &mirroredParams);

					Helper::InitializeBuildingActor(Controller, EditedActor);

					*EditedActor->Member<TEnumAsByte<uint8_t>>(("Team")) = Team;

					// TODO: Check bIsInitiallyBuilding

					/* static auto ForceBuildingHealth = EditedActor->Function(("ForceBuildingHealth"));

					if (ForceBuildingHealth)
						EditedActor->ProcessEvent(ForceBuildingHealth, &BuildingHealth); */
				}
				else
				{
					Helper::SetActorScale3D(BuildingActor, {});
					Helper::SilentDie(BuildingActor);
				}
			}
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

	if (Engine_Version < 424)
	{
		AddHook(("Function /Script/FortniteGame.FortPlayerController.ServerBeginEditingBuildingActor"), ServerBeginEditingBuildingActorHook);
		AddHook(("Function /Script/FortniteGame.FortPlayerController.ServerEditBuildingActor"), ServerEditBuildingActorHook);
		AddHook(("Function /Script/FortniteGame.FortPlayerController.ServerEndEditingBuildingActor"), ServerEndEditingBuildingActorHook);
	}
}