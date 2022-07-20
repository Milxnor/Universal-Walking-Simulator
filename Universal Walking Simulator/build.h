#pragma once

#include <Net/funcs.h>
#include <Gameplay/helper.h>

// Includes building and editing..

inline bool ServerCreateBuildingActorHook(UObject* Controller, UFunction* Function, void* Parameters)
{
	if (Controller && Parameters)
	{
		if (Engine_Version >= 423)
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
		else
		{
			struct FBuildingClassData {
				UObject* BuildingClass;
				int                                                PreviousBuildingLevel;                                    // 0x0008(0x0004) (ZeroConstructor, Transient, IsPlainOldData)
				int                                                UpgradeLevel;
				// sometimes theres somethinhg here
			};

			struct SCBAParams {
				const FBuildingClassData& BuildingClassData; // FBuildingClassData&
				const FVector& BuildLoc;
				const FRotator& BuildRot;
				bool bMirrored;
				float SyncKey; // does this exist below 7.2
			};

			auto Params = (SCBAParams*)Parameters;

			// static auto BuildingClassOffset = FindOffsetStruct(_("ScriptStruct /Script/FortniteGame.BuildingClassData"), _("BuildingClass"));

			// auto BuildingClass = Get<UObject*>(BuildingClassOffset, __int64(Params->BuildingClassData));
			
			// auto RemoteClientInfo = Controller->Member<UObject*>(_("BroadcastRemoteClientInfo"));

			// if (RemoteClientInfo && *RemoteClientInfo)
			if (false)
			{
				// auto BuildingClass = Controller->Member<UObject*>(_("CurrentBuildableClass"));
				// auto BuildingClass = (*RemoteClientInfo)->Member<UObject*>(_("RemoteBuildableClass"));

				auto BuildingClass = Params->BuildingClassData.BuildingClass;

				if (BuildingClass) // && *BuildingClass)
				{
					std::cout << _("Printing name...");
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
	}

	return false;
}

inline bool ServerBeginEditingBuildingActorHook(UObject* Controller, UFunction* Function, void* Parameters)
{
	/* auto Params = (AFortPlayerController_ServerBeginEditingBuildingActor_Params*)Parameters;
	auto Controller = (AFortPlayerControllerAthena*)Object;
	bool bFound = false;
	auto EditToolEntry = Inventory::FindItemInInventory<UFortEditToolItemDefinition>(Controller, bFound);

	if (Controller && Pawn && Params->BuildingActorToEdit && bFound)
	{
		// auto EditTool = (AFortWeap_EditingTool*)Inventory::EquipWeaponDefinition(Pawn, (UFortWeaponItemDefinition*)EditToolEntry.ItemDefinition, EditToolEntry.ItemGuid);

		if (EditTool)
		{
			EditTool->EditActor = Params->BuildingActorToEdit;
			EditTool->OnRep_EditActor();
			Params->BuildingActorToEdit->EditingPlayer = (AFortPlayerStateZone*)Pawn->PlayerState;
			Params->BuildingActorToEdit->OnRep_EditingPlayer();
		}
	} */
}

void InitializeBuildHooks()
{
	AddHook(_("Function /Script/FortniteGame.FortPlayerController.ServerCreateBuildingActor"), ServerCreateBuildingActorHook);
}