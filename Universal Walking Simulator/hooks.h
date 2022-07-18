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

			/* *gameState->Member<EAthenaGamePhase>(_("GamePhase")) = EAthenaGamePhase::Setup;

			struct {
				EAthenaGamePhase OldPhase;
			} params2{ EAthenaGamePhase::None };

			static const auto fnGamephase = gameState->Function(_("OnRep_GamePhase"));

			if (fnGamephase)
				gameState->ProcessEvent(fnGamephase, &params2); */

			if (AuthGameMode)
			{
				AuthGameMode->ProcessEvent(AuthGameMode->Function(_("StartPlay")), nullptr);

				AuthGameMode->ProcessEvent(AuthGameMode->Function(_("StartMatch")), nullptr);

				// Is this correct?
				
				// static auto Playlist = FindObject(_("FortPlaylistAthena /Game/Athena/Playlists/Playlist_DefaultSolo.Playlist_DefaultSolo"));
				static auto Playlist = FindObject(_("FortPlaylistAthena /Game/Athena/Playlists/Playlist_DefaultDuo.Playlist_DefaultDuo"));

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

	if (Pawn)
	{
		auto AbilitySystemComponent = *Pawn->Member<UObject*>(_("AbilitySystemComponent"));

		if (AbilitySystemComponent)
		{
			std::cout << _("Granting abilities!\n");
			static auto SprintAbility = FindObject("Class /Script/FortniteGame.FortGameplayAbility_Sprint");
			static auto ReloadAbility = FindObject("Class /Script/FortniteGame.FortGameplayAbility_Reload");
			static auto JumpAbility = FindObject("Class /Script/FortniteGame.FortGameplayAbility_Jump");
			static auto InteractUseAbility = FindObject("BlueprintGeneratedClass /Game/Abilities/Player/Generic/Traits/DefaultPlayer/GA_DefaultPlayer_InteractUse.GA_DefaultPlayer_InteractUse_C");
			static auto InteractSearchAbility = FindObject("BlueprintGeneratedClass /Game/Abilities/Player/Generic/Traits/DefaultPlayer/GA_DefaultPlayer_InteractSearch.GA_DefaultPlayer_InteractSearch_C");

			if (SprintAbility)
				GrantGameplayAbility(Pawn, SprintAbility);
			if (ReloadAbility)
				GrantGameplayAbility(Pawn, ReloadAbility);
			if (JumpAbility)
				GrantGameplayAbility(Pawn, JumpAbility);
			if (InteractUseAbility)
				GrantGameplayAbility(Pawn, InteractUseAbility);
			if (InteractSearchAbility)
				GrantGameplayAbility(Pawn, InteractSearchAbility);
		}
		else
			std::cout << _("Unable to find AbilitySystemComponent!\n");
	}
	else
		std::cout << _("Unable to find Pawn on ServerLoadingScreenDropped!\n");

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

inline bool PlayButtonHook(UObject* Object, UFunction* Function, void* Parameters)
{
	LoadInMatch();
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
	// AddHook(_("Function /Script/FortniteGame.FortPlayerController.ServerLoadingScreenDropped"), ServerLoadingScreenDroppedHook);

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
