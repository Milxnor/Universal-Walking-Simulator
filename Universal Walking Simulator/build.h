#pragma once

#include <Net/funcs.h>
#include <Gameplay/helper.h>

// Includes building and editing..

inline bool ServerCreateBuildingActorHook(UObject* Controller, UFunction* Function, void* Parameters)
{
	if (Controller && Parameters)
	{
		if (Engine_Version >= 423 && false)
		{
			struct SCBAParams { __int64 CreateBuildingData; };
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
						auto BuildLoc = Get<FVector>(BuildLocOffset, Params->CreateBuildingData);
						auto BuildRot = Get<FRotator>(BuildRotOffset, Params->CreateBuildingData);
						auto bMirrored = Get<bool>(bMirroredOffset, Params->CreateBuildingData);

						std::cout << _("BuildingClassOffset: ") << BuildingClassOffset << '\n';
						std::cout << _("BuildingClassData: ") << BuildingClassDataOffset << '\n';
						std::cout << _("BuildingClass: ") << BuildingClass << '\n';

						if (BuildingClass && *BuildingClass)
						{
							std::cout << _("Goofy class: ") << (*BuildingClass)->GetFullName();
							auto BuildingActor = Easy::SpawnActor(*BuildingClass, *BuildLoc, *BuildRot); // Helper::GetActorLocation(Pawn), Helper::GetActorRotation(Pawn));

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
			struct SCBAParams {
				void* BuildingClassData; // FBuildingClassData&
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

				auto BuildingClass = (UObject**)Params->BuildingClassData;

				if (BuildingClass && *BuildingClass)
				{
					std::cout << _("Goofy class: ") << (*BuildingClass)->GetFullName();
					auto BuildingActor = Easy::SpawnActor(*BuildingClass, Params->BuildLoc, Params->BuildRot); // Helper::GetActorLocation(Pawn), Helper::GetActorRotation(Pawn));

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

void InitializeBuildHooks()
{
	AddHook(_("Function /Script/FortniteGame.FortPlayerController.ServerCreateBuildingActor"), ServerCreateBuildingActorHook);
}