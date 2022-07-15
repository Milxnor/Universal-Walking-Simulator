#pragma once

#include <UE/structs.h>
#include <functional>

#include "color.hpp"
#include <Net/server.h>
#include <Gameplay/helper.h>
#include <Net/funcs.h>
#include <Net/nethooks.h>

#include <mutex>

#define LOGGING

// INSPIRED BY KEMOS UFUNCTION HOOKING

// TODO: Switch to phmap

bool bStarted = false;

inline void initStuff()
{
	if (!bStarted)
	{
		bStarted = true;
		auto world = Helper::GetWorld();
		auto gameState = *world->Member<UObject*>(_("GameState"));

		if (gameState)
		{
			*gameState->Member<char>(_("bGameModeWillSkipAircraft")) = true;
			*gameState->Member<float>(_("AircraftStartTime")) = 99999.0f;
			*gameState->Member<float>(_("WarmupCountdownEndTime")) = 99999.0f;

			*gameState->Member<EAthenaGamePhase>(_("GamePhase")) = EAthenaGamePhase::Warmup;

			struct {
				EAthenaGamePhase OldPhase;
			} params2{ EAthenaGamePhase::None };

			static const auto fnGamephase = gameState->Function(_("OnRep_GamePhase"));

			if (fnGamephase)
			{
				std::cout << _("Changing Phase!\n");
				gameState->ProcessEvent(fnGamephase, &params2);
				std::cout << _("Changed Phase!\n");
			}

			auto authGameMode = *world->Member<UObject*>(_("AuthorityGameMode"));

			if (authGameMode)
			{
				authGameMode->ProcessEvent(authGameMode->Function(_("StartPlay")), nullptr); 

				/* *gameState->Member<char>(_("bReplicatedHasBegunPlay")) = true;
				gameState->ProcessEvent(gameState->Function(_("OnRep_ReplicatedHasBegunPlay")), nullptr);

				static auto StartMatchFn = authGameMode->Function(_("StartMatch"));

				if (StartMatchFn)
				{
					authGameMode->ProcessEvent(StartMatchFn, nullptr);
					std::cout << _("Started Match!\n");
				} */

			}
			else
			{
				std::cout << dye::yellow(_("[WARNING] ")) << _("Failed to find AuthorityGameMode!\n");
			}
		}

		Listen(7777);
	}
}

void ReadyToStartMatchHook(UObject* Object, UFunction* Function, void* Parameters)
{
	return initStuff();
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
		Map.Set(_(L"Athena_Terrain?game=/Game/Athena/Athena_GameMode.Athena_GameMode_C"));
		PlayerController->ProcessEvent(SwitchLevelFn, &Map);
		// Map.Free();
	}
	else
	{
		std::cout << dye::red(_("[ERROR] ")) << _("Unable to find PlayerController!\n");
	}
}

inline void PlayButtonHook(UObject* Object, UFunction* Function, void* Parameters)
{
	InitializeNetHooks();

	std::cout << _("Initialized NetHooks!\n");

	LoadInMatch();
}

void FinishInitializeHooks()
{
	if (Engine_Version < 422)
		AddHook(_("BndEvt__BP_PlayButton_K2Node_ComponentBoundEvent_1_CommonButtonClicked__DelegateSignature"), PlayButtonHook);
	AddHook(_("Function /Script/Engine.GameMode.ReadyToStartMatch"), ReadyToStartMatchHook);

	for (auto& Func : FunctionsToHook)
	{
		if (!Func.first)
			std::cout << _("Detected invalid UFunction!\n");
	}

	std::cout << std::format("Hooked {} UFunctions!\n", std::to_string(FunctionsToHook.size()));
}

void* ProcessEventDetour(UObject* Object, UFunction* Function, void* Parameters)
{
	if (Object && Function)
	{
		for (auto& Func : FunctionsToHook)
		{
			if (Function == Func.first)
				Func.second(Object, Function, Parameters);
		}

#ifdef LOGGING
		if (bStarted && bListening)
		{
			auto FunctionName = Function->GetName();
			// if (Function->FunctionFlags & 0x00200000 || Function->FunctionFlags & 0x01000000) // && FunctionName.find("Ack") == -1 && FunctionName.find("AdjustPos") == -1))
			if (FunctionName.starts_with(_("Server")) || FunctionName.starts_with(_("Client") || FunctionName.starts_with(_("OnRep_"))))
			{
				if (FunctionName.find("ServerUpdateCamera") == -1) // && FunctionName.find("ServerMove") == -1)
				{
					std::cout << "RPC Called: " << FunctionName << '\n';
				}
			}
		}
#endif
	}

	return ProcessEventO(Object, Function, Parameters);
}