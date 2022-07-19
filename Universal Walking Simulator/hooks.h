#pragma once

#include <UE/structs.h>
#include <functional>

#include "color.hpp"
#include <Net/server.h>
#include <Gameplay/helper.h>
#include <Net/funcs.h>
#include <Net/nethooks.h>
#include <Gameplay/abilities.h>

#include <mutex>

#define LOGGING

// HEAVILY INSPIRED BY KEMOS UFUNCTION HOOKING

static bool bStarted = false;

inline void initStuff()
{
	if (!bStarted && bTraveled)
	{
		bStarted = true;

		CreateThread(0, 0, Helper::Console::Setup, 0, 0, 0);

		auto world = Helper::GetWorld();
		auto gameState = *world->Member<UObject*>(_("GameState"));

		if (gameState)
		{
			auto AuthGameMode = *world->Member<UObject*>(_("AuthorityGameMode"));

			if (std::stod(FN_Version) >= 8 && AuthGameMode)
			{
				static auto PlayerControllerClass = FindObject(_("BlueprintGeneratedClass /Game/Athena/Athena_PlayerController.Athena_PlayerController_C"));
				std::cout << _("PlayerControllerClass: ") << PlayerControllerClass << '\n';
				*AuthGameMode->Member<UObject*>(_("PlayerControllerClass")) = PlayerControllerClass;
			}

			*gameState->Member<char>(_("bGameModeWillSkipAircraft")) = false;
			*gameState->Member<float>(_("AircraftStartTime")) = 99999.0f;
			*gameState->Member<float>(_("WarmupCountdownEndTime")) = 99999.0f;

			*gameState->Member<EAthenaGamePhase>(_("GamePhase")) = EAthenaGamePhase::Warmup;

			struct {
				EAthenaGamePhase OldPhase;
			} params2{ EAthenaGamePhase::None };

			static const auto fnGamephase = gameState->Function(_("OnRep_GamePhase"));

			if (fnGamephase)
				gameState->ProcessEvent(fnGamephase, &params2);

			if (AuthGameMode)
			{
				AuthGameMode->ProcessEvent(AuthGameMode->Function(_("StartPlay")), nullptr);

				AuthGameMode->ProcessEvent(AuthGameMode->Function(_("StartMatch")), nullptr);

				// Is this correct?
				
				// static auto Playlist = FindObject(_("FortPlaylistAthena /Game/Athena/Playlists/Playlist_DefaultSolo.Playlist_DefaultSolo"));
				// static auto Playlist = FindObject(_("FortPlaylistAthena /Game/Athena/Playlists/Playlist_DefaultDuo.Playlist_DefaultDuo"));
				static auto Playlist = FindObject(_("FortPlaylistAthena /Game/Athena/Playlists/Playlist_DefaultSquad.Playlist_DefaultSquad"));

				static auto OnRepPlaylist = FindObject(_("Function /Script/FortniteGame.FortGameStateAthena.OnRep_CurrentPlaylistInfo"));

				static auto BasePlaylistOffset = FindOffsetStruct(_("ScriptStruct /Script/FortniteGame.PlaylistPropertyArray"), _("BasePlaylist"));

				if (std::stod(FN_Version) > 6.00)
				{
					OnRepPlaylist = gameState->Function(_("OnRep_CurrentPlaylistInfo"));

					static auto BasePlaylistOffset = FindOffsetStruct(_("ScriptStruct /Script/FortniteGame.PlaylistPropertyArray"), _("BasePlaylist"));
					static auto PlaylistInfo = gameState->Member<FFastArraySerializer>(_("CurrentPlaylistInfo"));

					if (BasePlaylistOffset && OnRepPlaylist && Playlist)
					{
						auto BasePlaylist = (UObject**)(__int64(PlaylistInfo) + BasePlaylistOffset);// *gameState->Member<UObject>(_("CurrentPlaylistInfo"))->Member<UObject*>(_("BasePlaylist"), true);

						if (BasePlaylist)
						{
							*BasePlaylist = Playlist;
							// PlaylistInfo->MarkArrayDirty();
							std::cout << _("Set playlist to: ") << Playlist->GetFullName() << '\n';
						}
						else
							std::cout << _("Base Playlist is null!\n");
					}
					else
					{
						std::cout << _("Missing something related to the Playlist!\n");
						std::cout << _("BasePlaylist Offset: ") << BasePlaylistOffset << '\n';
						std::cout << _("OnRepPlaylist: ") << OnRepPlaylist << '\n';
						std::cout << _("Playlist: ") << Playlist << '\n';
					}
				}
				else
				{
					OnRepPlaylist = gameState->Function(_("OnRep_CurrentPlaylistData"));
					*gameState->Member<UObject*>(_("CurrentPlaylistData")) = Playlist;
				}

				gameState->ProcessEvent(OnRepPlaylist);
			}
			else
			{
				std::cout << dye::yellow(_("[WARNING] ")) << _("Failed to find AuthorityGameMode!\n");
			}
		}

		Listen(7777);

		InitializeNetHooks();

		std::cout << _("Initialized NetHooks!\n");
	}
}

bool ServerLoadingScreenDroppedHook(UObject* PlayerController, UFunction* Function, void* Parameters)
{
	auto PlayerState = *PlayerController->Member<UObject*>(_("PlayerState"));
	auto Pawn = *PlayerController->Member<UObject*>(_("Pawn"));

	return false;
}

bool ServerAttemptAircraftJumpHook(UObject* PlayerController, UFunction* Function, void* Parameters)
{
	struct Param{
		FRotator                                    ClientRotation;
	};

	auto Params = (Param*)Parameters;

	if (PlayerController && Params) // PlayerController->IsInAircraft()
	{
		static auto world = Helper::GetWorld();
		static auto GameState = *world->Member<UObject*>(_("GameState"));

		if (GameState)
		{
			auto Aircraft = GameState->Member<TArray<UObject*>>(_("Aircrafts"))->At(0);

			if (Aircraft)
			{
				auto ExitLocation = Helper::GetActorLocation(Aircraft);

				Helper::InitPawn(PlayerController, ExitLocation);

				// ClientSetRotation
			}
		}
	}

	return false;
}

bool ReadyToStartMatchHook(UObject* Object, UFunction* Function, void* Parameters)
{
	initStuff();
	return false;
}

void LoadInMatch()
{
	auto GameInstance = *GetEngine()->Member<UObject*>(_("GameInstance"));
	auto& LocalPlayers = *GameInstance->Member<TArray<UObject*>>(_("LocalPlayers"));
	auto PlayerController = *LocalPlayers.At(0)->Member<UObject*>(_("PlayerController"));

	if (PlayerController)
	{
		static auto SwitchLevelFn = PlayerController->Function(_("SwitchLevel"));
		FString Map;
		Map.Set(GetMapName());
		PlayerController->ProcessEvent(SwitchLevelFn, &Map);
		// Map.Free();
		bTraveled = true;
	}
	else
	{
		std::cout << dye::red(_("[ERROR] ")) << _("Unable to find PlayerController!\n");
	}
}

inline bool ServerAttemptExitVehicleHook(UObject* Controller, UFunction* Function, void* Parameters)
{
	auto Pawn = Controller->Member<UObject*>(_("Pawn"));

	if (Pawn && *Pawn)
	{
		UObject* Vehicle = Helper::GetVehicle(*Pawn);

		if (Vehicle)
		{
			Helper::SetLocalRole(*Pawn, ENetRole::ROLE_Authority);
			Helper::SetLocalRole(Vehicle, ENetRole::ROLE_Authority);
		}
	}

	return false;
}

inline bool ServerPlayEmoteItemHook(UObject* Controller, UFunction* Function, void* Parameters)
{
	auto CurrentPawn = *Controller->Member<UObject*>(_("Pawn"));

	struct SPEIParams  { UObject* EmoteAsset; }; // UFortMontageItemDefinitionBase
	auto EmoteParams = (SPEIParams*)Parameters;

	auto EmoteAsset = EmoteParams->EmoteAsset;

	if (Controller /* && !Controller->IsInAircraft() */ && CurrentPawn && EmoteAsset)
	{
		struct {
			TEnumAsByte<EFortCustomBodyType> BodyType;
			TEnumAsByte<EFortCustomGender> Gender;
			UObject* AnimMontage; // UAnimMontage
		} GAHRParams{EFortCustomBodyType::All, EFortCustomGender::Both}; // (CurrentPawn->CharacterBodyType, CurrentPawn->CharacterGender)
		static auto fn = EmoteAsset->Function(_("GetAnimationHardReference"));

		if (fn)
		{
			EmoteAsset->ProcessEvent(fn, &GAHRParams);
			auto Montage = GAHRParams.AnimMontage;
			if (Montage)
			{
				auto StartSection = FName();

				struct {
					UObject* AnimMontage;
					float InPlayRate;
					FName& StartSectionName;
				} PLAMParams{ Montage, 1.0f, StartSection };

				static auto PLAMFn = CurrentPawn->Function(_("PlayLocalAnimMontage"));
				CurrentPawn->ProcessEvent(PLAMFn, &PLAMParams);

				struct {
					UObject* AnimMontage;
					float InPlayRate;
					FName& StartSectionName;
				} PAMParams{Montage, 1.0f, StartSection};
				
				static auto PAMFn = CurrentPawn->Function(_("PlayAnimMontage"));
				CurrentPawn->ProcessEvent(PAMFn, &PAMParams);
				// CurrentPawn->PlayAnimMontage(Montage, 1.0f, FName(0));
				CurrentPawn->ProcessEvent(_("OnRep_CharPartAnimMontageInfo"));
				CurrentPawn->ProcessEvent(_("OnRep_ReplicatedAnimMontage"));
			}
		}
	}

	return false;
}

inline bool ServerAttemptInteractHook(UObject* Controller, UFunction* Function, void* Parameters)
{
	if (Engine_Version >= 423) // They use FortControllerComponent_Interaction and we have not implemented yet.
		return false; // TODO: Check the owener to see if its a controller

	ProcessEventO(Controller, Function, Parameters);

	struct SAIParams {
		UObject* ReceivingActor;                                           // (Parm, ZeroConstructor, IsPlainOldData)
		UObject* InteractComponent;                                        // (Parm, ZeroConstructor, InstancedReference, IsPlainOldData)
		TEnumAsByte<ETInteractionType>                     InteractType;                                             // (Parm, ZeroConstructor, IsPlainOldData)
		UObject* OptionalObjectData;                                       // (Parm, ZeroConstructor, IsPlainOldData)
	};

	auto Params = (SAIParams*)Parameters;

	if (Params && Controller && false)
	{
		auto ReceivingActor = Params->ReceivingActor;
		if (ReceivingActor)
		{
			auto ReceivingActorName = ReceivingActor->GetName(); // There has to be a better way, right?

			if (ReceivingActorName.contains(_("Vehicle")))
			{
				Helper::SetLocalRole(*Controller->Member<UObject*>(_("Pawn")), ENetRole::ROLE_AutonomousProxy);
				Helper::SetLocalRole(ReceivingActor, ENetRole::ROLE_AutonomousProxy);
			}
		}
	}

	return true;
}

inline bool PlayButtonHook(UObject* Object, UFunction* Function, void* Parameters)
{
	LoadInMatch();
	return false;
}

inline bool ServerSuicideHook(UObject* Controller, UFunction* Function, void* Parameters)
{
	if (Controller)
	{
		struct {
			FVector LaunchVelocity;
			bool bXYOverride;
			bool bZOverride;
			bool bIgnoreFallDamage;
			bool bPlayFeedbackEvent;
		} LCJParams{{ 90000, 90000, 90000 }, false, false, true, true};

		auto Pawn = Controller->Member<UObject*>(_("Pawn"));

		if (Pawn && *Pawn)
		{
			Helper::SpawnChip(Controller);
			std::cout << _("Spawned Chip!\n");
			/* static auto fn = (*Pawn)->Function(_("LaunchCharacterJump"));

			if (fn)
				(*Pawn)->ProcessEvent(fn, &LCJParams);
			else
				std::cout << _("No LaunchCharacterJump!\n"); */
		}
		else
			std::cout << _("No Pawn!\n");
	}

	return false;
}

inline bool IsResurrectionChipAvailableHook(UObject* PlayerState, UFunction* Function, void* Parameters)
{
	struct IRCAParams { bool ret; };

	auto Params = (IRCAParams*)Parameters;

	if (Params)
	{
		Params->ret = true;
	}

	return true;
}

inline bool ServerClientPawnLoadedHook(UObject* Controller, UFunction* Function, void* Parameters)
{
	struct SCPLParams { bool bIsPawnLoaded; };
	auto Params = (SCPLParams*)Parameters;

	if (Parameters && Controller)
	{
		std::cout << _("bIsPawnLoaded: ") << Params->bIsPawnLoaded << '\n';

		auto Pawn = *Controller->Member<UObject*>(_("Pawn"));

		if (Pawn)
		{
			auto bLoadingScreenDropped = *Controller->Member<bool>(_("bLoadingScreenDropped"));
			if (bLoadingScreenDropped)
			{
				auto Chip = Helper::SpawnChip(Controller);
				// 0x0018
				struct FFortResurrectionData
				{
					bool                                               bResurrectionChipAvailable;                               // 0x0000(0x0001) (ZeroConstructor, IsPlainOldData)
					unsigned char                                      UnknownData00[0x3];                                       // 0x0001(0x0003) MISSED OFFSET
					float                                              ResurrectionExpirationTime;                               // 0x0004(0x0004) (ZeroConstructor, IsPlainOldData)
					float                                              ResurrectionExpirationLength;                             // 0x0008(0x0004) (ZeroConstructor, IsPlainOldData)
					struct FVector                                     WorldLocation;                                            // 0x000C(0x000C) (ZeroConstructor, IsPlainOldData)
				};

				auto PlayerState = *Pawn->Member<UObject*>(_("PlayerState"));
				auto FortResurrectionData = PlayerState->Member<FFortResurrectionData>(_("ResurrectionChipAvailable"));
				
				if (FortResurrectionData)
				{
					std::cout << _("FortResurrectionData valid!\n");
					std::cout << _("FortResurrectionData Location X: ") << FortResurrectionData->WorldLocation.X << '\n';
				}
				else
					std::cout << _("No FortResurrectionData!\n");

				auto ResurrectionData = FFortResurrectionData{};
				ResurrectionData.bResurrectionChipAvailable = true;
				ResurrectionData.ResurrectionExpirationLength = 99999.f;
				ResurrectionData.ResurrectionExpirationTime = 99999.f;
				ResurrectionData.WorldLocation = Helper::GetActorLocation(Chip);

				if (FortResurrectionData)
					*FortResurrectionData = ResurrectionData;
				std::cout << _("Spawned Chip!\n");
			}
			else
				std::cout << _("Loading screen is not dropped!\n");
		}
		else
			std::cout << _("Pawn is not valid!\n");
	}

	return false;
}

inline bool AircraftExitedDropZoneHook(UObject* GameMode, UFunction* Function, void* Parameters)
{
	return true;
}

void FinishInitializeUHooks()
{
	if (Engine_Version < 422)
		AddHook(_("BndEvt__BP_PlayButton_K2Node_ComponentBoundEvent_1_CommonButtonClicked__DelegateSignature"), PlayButtonHook);

	AddHook(_("Function /Script/Engine.GameMode.ReadyToStartMatch"), ReadyToStartMatchHook);
	AddHook(_("Function /Script/FortniteGame.FortPlayerControllerAthena.ServerAttemptAircraftJump"), ServerAttemptAircraftJumpHook);
	AddHook(_("Function /Script/FortniteGame.FortGameModeAthena.OnAircraftExitedDropZone"), AircraftExitedDropZoneHook); // "fix" (temporary) for aircraft after it ends on newer versions.
	AddHook(_("Function /Script/FortniteGame.FortPlayerController.ServerSuicide"), ServerSuicideHook);
	// AddHook(_("Function /Script/FortniteGame.FortPlayerController.ServerClientPawnLoaded"), ServerClientPawnLoadedHook);
	// AddHook(_("Function /Script/FortniteGame.FortPlayerController.ServerPlayEmoteItem"), ServerPlayEmoteItemHook);

	if (Engine_Version < 423)
	{ // ??? Idk why we need the brackets
		AddHook(_("Function /Script/FortniteGame.FortPlayerController.ServerAttemptInteract"), ServerAttemptInteractHook);
	}
	else
		AddHook(_("Function /Script/FortniteGame.FortControllerComponent_Interaction.ServerAttemptInteract"), ServerAttemptInteractHook);

	AddHook(_("Function /Script/FortniteGame.FortPlayerControllerZone.ServerAttemptExitVehicle"), ServerAttemptExitVehicleHook);

	for (auto& Func : FunctionsToHook)
	{
		if (!Func.first)
			std::cout << _("Detected null UFunction!\n");
	}

	std::cout << std::format("Hooked {} UFunctions!\n", std::to_string(FunctionsToHook.size()));
}

void* ProcessEventDetour(UObject* Object, UFunction* Function, void* Parameters)
{
	if (Object && Function)
	{
#ifdef LOGGING
		if (bStarted && bListening)
		{
			auto FunctionName = Function->GetName();
			// if (Function->FunctionFlags & 0x00200000 || Function->FunctionFlags & 0x01000000) // && FunctionName.find("Ack") == -1 && FunctionName.find("AdjustPos") == -1))
			if (FunctionName.starts_with(_("Server")) || FunctionName.starts_with(_("Client") || FunctionName.starts_with(_("OnRep_"))))
			{
				if (!FunctionName.contains("ServerUpdateCamera") && FunctionName.find("ServerMove") == -1)
				{
					std::cout << "RPC Called: " << FunctionName << '\n';
				}
			}
		}
#endif

		for (auto& Func : FunctionsToHook)
		{
			if (Function == Func.first)
			{
				if (Func.second(Object, Function, Parameters)) // If the fuynction returned true, then cancel execution.
				{
					return 0;
				}
			}
		}
	}

	return ProcessEventO(Object, Function, Parameters);
}

__int64 __fastcall FixCrashDetour(int32_t* PossiblyNull, __int64 a2, int* a3)
{
	if (!PossiblyNull)
	{
		std::cout << "Prevented Crash!\n";
		return 0;
	}

	return FixCrash(PossiblyNull, a2, a3);
}

void __fastcall GetPlayerViewPointDetour(UObject* pc, FVector* a2, FRotator* a3)
{
	if (pc)
	{
		static auto fn = FindObject(_("Function /Script/Engine.Controller.GetViewTarget"));
		UObject* TheViewTarget = nullptr;
		pc->ProcessEvent(fn, &TheViewTarget);

		if (TheViewTarget)
		{
			if (a2)
				*a2 = Helper::GetActorLocation(TheViewTarget);
			if (a3)
				*a3 = Helper::GetActorRotation(TheViewTarget);
			// std::cout << _("Did the ViewPoint!\n");

			return;
		}
		else
			std::cout << _("unable to get viewpoint!\n");
	}

	return GetPlayerViewPoint(pc, a2, a3);
}

void InitializeHooks()
{
	MH_CreateHook((PVOID)ProcessEventAddr, ProcessEventDetour, (void**)&ProcessEventO);
	MH_EnableHook((PVOID)ProcessEventAddr);

	if (Engine_Version >= 423)
	{
		MH_CreateHook((PVOID)FixCrashAddr, FixCrashDetour, (void**)&FixCrash);
		MH_EnableHook((PVOID)FixCrashAddr);
	}

	if (GetPlayerViewpointAddr) //Engine_Version == 423)
	{
		MH_CreateHook((PVOID)GetPlayerViewpointAddr, GetPlayerViewPointDetour, (void**)&GetPlayerViewPoint);
		MH_EnableHook((PVOID)GetPlayerViewpointAddr);
	}
	else
		std::cout << _("[WARNING] Could not fix flashing!\n");
}
