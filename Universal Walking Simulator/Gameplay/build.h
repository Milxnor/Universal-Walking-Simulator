#pragma once

#include <Net/funcs.h>
#include <Gameplay/helper.h>
#include <Gameplay/inventory.h>

// Includes building and editing..

inline bool ServerCreateBuildingActorHook(UObject* Controller, UFunction* Function, void* Parameters)
{
	if (Controller && Parameters)
	{
#ifndef BEFORE_SEASONEIGHT
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

				static auto BuildLocOffset = FindOffsetStruct(_("ScriptStruct /Script/FortniteGame.CreateBuildingActorData"), _("BuildLoc"));
				static auto BuildRotOffset = FindOffsetStruct(_("ScriptStruct /Script/FortniteGame.CreateBuildingActorData"), _("BuildRot"));
				static auto BuildingClassDataOffset = FindOffsetStruct(_("ScriptStruct /Script/FortniteGame.CreateBuildingActorData"), _("bMirrored"));
				static auto bMirroredOffset = FindOffsetStruct(_("ScriptStruct /Script/FortniteGame.CreateBuildingActorData"), _("BuildingClassData"));

				static auto BuildingClassOffset = FindOffsetStruct(_("ScriptStruct /Script/FortniteGame.BuildingClassData"), _("BuildingClass"));

				auto Pawn = *Controller->Member<UObject*>(_("Pawn"));

				// auto BuildingClassData = Get<__int64>(BuildingClassDataOffset, CreateBuildingData);

				// if (BuildingClassData)
				{
					// auto BuildingClass = Get<UObject*>(BuildingClassOffset, *BuildingClassData);
					auto RemoteClientInfo = Controller->Member<UObject*>(_("BroadcastRemoteClientInfo"));

					if (RemoteClientInfo && *RemoteClientInfo)
					{
						// auto BuildingClass = Controller->Member<UObject*>(_("CurrentBuildableClass"));
						auto BuildingClass = (*RemoteClientInfo)->Member<UObject*>(_("RemoteBuildableClass"));
						// std::cout << _("BuildLocation Offset: ") << BuildLocOffset << '\n';
						auto BuildLoc = Params->CreateBuildingData.BuildLoc; // (FVector*)(__int64(Params->CreateBuildingData) + BuildLocOffset)// Get<FVector>(BuildLocOffset, Params->CreateBuildingData);
						auto BuildRot = Params->CreateBuildingData.BuildRot; // Get<FRotator>(BuildRotOffset, Params->CreateBuildingData);
						// auto bMirrored = Get<bool>(bMirroredOffset, Params->CreateBuildingData);

						/*
						std::cout << _("BuildingClassOffset: ") << BuildingClassOffset << '\n';
						std::cout << _("BuildingClassData: ") << BuildingClassDataOffset << '\n';
						std::cout << _("BuildingClass: ") << BuildingClass << '\n';
						*/

						if (BuildingClass && *BuildingClass)
						{
							// std::cout << _("Goofy class: ") << (*BuildingClass)->GetFullName();
							auto BuildingActor = Easy::SpawnActor(*BuildingClass, BuildLoc, BuildRot); // Helper::GetActorLocation(Pawn), Helper::GetActorRotation(Pawn));

							if (BuildingActor)
							{
								Helper::InitializeBuildingActor(Controller, BuildingActor, true);
							}
							else
								std::cout << _("Unable to summon the building!\n");
						}
						else
							std::cout << _("Unable to get BuildingClass!\n");
					}
					else
						std::cout << _("Unable to get RemoteClientInfo!\n");
				}
			}
		}
#else
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

			// static auto BuildingClassOffset = FindOffsetStruct(_("ScriptStruct /Script/FortniteGame.BuildingClassData"), _("BuildingClass"));

			// auto BuildingClass = Get<UObject*>(BuildingClassOffset, __int64(Params->BuildingClassData));
			
			// auto RemoteClientInfo = Controller->Member<UObject*>(_("BroadcastRemoteClientInfo"));

			// if (RemoteClientInfo && *RemoteClientInfo)
			// if (false)
			{
				// auto BuildingClass = *Controller->Member<UObject*>(_("CurrentBuildableClass"));
				// auto BuildingClass = (*RemoteClientInfo)->Member<UObject*>(_("RemoteBuildableClass"));

				auto BuildingClass = Params->BuildingClassData.BuildingClass;

				if (BuildingClass) // && *BuildingClass)
				{
					// std::cout << _("Printing name...");
					// std::cout << _("Goofy class: ") << BuildingClass->GetFullName();
					auto BuildingActor = Easy::SpawnActor(BuildingClass, Params->BuildLoc, Params->BuildRot); // Helper::GetActorLocation(Pawn), Helper::GetActorRotation(Pawn));

					if (BuildingActor)
					{
						Helper::InitializeBuildingActor(Controller, BuildingActor, true);
					}
					else
						std::cout << _("Unable to summon the building!\n");
				}
				else
					std::cout << _("No BuildingClass!\n");
			}
		}

#endif
	}

	return false;
}

inline bool ServerBeginEditingBuildingActorHook(UObject* Controller, UFunction* Function, void* Parameters)
{
	struct Parms { UObject* BuildingActor; };
	static UObject* EditToolDefinition = FindObject(_("FortEditToolItemDefinition /Game/Items/Weapons/BuildingTools/EditTool.EditTool"));

	auto EditToolInstance = Inventory::FindItemInInventory(Controller, EditToolDefinition);

	auto Pawn = *Controller->Member<UObject*>(_("Pawn"));

	UObject* BuildingToEdit = ((Parms*)Parameters)->BuildingActor;

	if (Controller && BuildingToEdit && EditToolInstance)
	{
		auto EditTool = Inventory::EquipWeaponDefinition(Pawn, EditToolDefinition, Inventory::GetItemGuid(EditToolInstance));

		if (EditTool)
		{
			auto PlayerState = *Controller->Member<UObject*>(_("PlayerState"));
			*BuildingToEdit->Member<UObject*>(_("EditingPlayer")) = PlayerState;
			static auto OnRep_EditingPlayer = BuildingToEdit->Function(_("OnRep_EditingPlayer"));
			BuildingToEdit->ProcessEvent(OnRep_EditingPlayer);

			*EditTool->Member<UObject*>(_("EditActor")) = BuildingToEdit;
			static auto OnRep_EditActor = EditTool->Function(_("OnRep_EditActor"));
			EditTool->ProcessEvent(OnRep_EditActor);
		}
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
			auto Location = Helper::GetActorLocation(BuildingActor);
			auto Rotation = Helper::GetActorRotation(BuildingActor);

			if (*BuildingActor->Member<EFortBuildingType>(_("BuildingType")) != EFortBuildingType::Wall)
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

			auto Team = *Params->BuildingActorToEdit->Member<TEnumAsByte<uint8_t>>(_("Team"));

			Helper::SilentDie(Params->BuildingActorToEdit);

			auto EditedActor = Easy::SpawnActor(Params->NewBuildingClass, Location, Rotation);

			if (EditedActor)
			{
				Helper::SetOwner(EditedActor, Controller);
				
				static auto SetMirroredFn = EditedActor->Function(_("SetMirrored"));

				struct { bool bMirrored; }mirroredParams{ Params->bMirrored };
				EditedActor->ProcessEvent(SetMirroredFn, &mirroredParams);

				*EditedActor->Member<TEnumAsByte<uint8_t>>(_("Team")) = Team;
				Helper::InitializeBuildingActor(Controller, EditedActor);
			}
		}
	}

	return false;
}

inline bool ServerEndEditingBuildingActorHook(UObject* Controller, UFunction* Function, void* Parameters)
{
	if (Controller && Parameters)
	{
		// TODO: Check if the controller is in aircraft, if they edit on spawn island, it will make them end on the battle bus, which will not go well.

		auto Pawn = Controller->Member<UObject*>(_("Pawn"));

		if (Pawn && *Pawn)
		{
			// auto EditTool = (*Pawn)->Member<UObject*>(_("CurrentWeapon"));
			static UObject* EditToolDefinition = FindObject(_("FortEditToolItemDefinition /Game/Items/Weapons/BuildingTools/EditTool.EditTool"));
			auto EditToolInstance = Inventory::FindItemInInventory(Controller, EditToolDefinition);
			auto EditTool = Inventory::EquipWeaponDefinition(*Pawn, EditToolDefinition, Inventory::GetItemGuid(EditToolInstance));

			if (EditTool && Parameters)
			{
				struct Parms {
					UObject* BuildingActorToStopEditing;
				};
				
				auto Params = (Parms*)Parameters;

				if (Params->BuildingActorToStopEditing)
				{
					*Params->BuildingActorToStopEditing->Member<UObject*>(_("EditingPlayer")) = nullptr;
					static auto OnRep_EditingPlayer = Params->BuildingActorToStopEditing->Function(_("OnRep_EditingPlayer"));
					Params->BuildingActorToStopEditing->ProcessEvent(OnRep_EditingPlayer);

					*EditTool->Member<bool>(_("bEditConfirmed")) = true;
					*EditTool->Member<UObject*>(_("EditActor")) = nullptr;
					static auto OnRep_EditActorFn = EditTool->Function(_("OnRep_EditActor")); // We make it static so then it doesn't have to be found again.

					if (OnRep_EditActorFn)
						EditTool->ProcessEvent(OnRep_EditActorFn);
				}
			}
		}
	}

	return false;
}

void InitializeBuildHooks()
{
	AddHook(_("Function /Script/FortniteGame.FortPlayerController.ServerCreateBuildingActor"), ServerCreateBuildingActorHook);

	AddHook(_("Function /Script/FortniteGame.FortPlayerController.ServerBeginEditingBuildingActor"), ServerBeginEditingBuildingActorHook);
	AddHook(_("Function /Script/FortniteGame.FortPlayerController.ServerEditBuildingActor"), ServerEditBuildingActorHook);
	AddHook(_("Function /Script/FortniteGame.FortPlayerController.ServerEndEditingBuildingActor"), ServerEndEditingBuildingActorHook);
}