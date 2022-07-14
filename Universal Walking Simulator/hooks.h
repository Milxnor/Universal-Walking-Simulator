#pragma once

#include <UE/structs.h>
#include <functional>

#include "color.hpp"
#include <Net/server.h>
#include <Gameplay/helper.h>
#include <Net/funcs.h>
#include <Net/nethooks.h>

#include <mutex>

// INSPIRED BY KEMOS UFUNCTION HOOKING

// TODO: Switch to phmap

inline void initStuff()
{
	auto world = Helper::GetWorld();
	auto gameState = *world->Member<UObject*>(_("GameState"));

	*gameState->Member<char>(_("bGameModeWillSkipAircraft")) = true;
	*gameState->Member<float>(_("AircraftStartTime")) = 99999.0f;
	*gameState->Member<float>(_("WarmupCountdownEndTime")) = 99999.0f;

	*gameState->Member<EAthenaGamePhase>(_("GamePhase")) = EAthenaGamePhase::Warmup;
	struct {
		EAthenaGamePhase OldPhase;
	} params2{ EAthenaGamePhase::None };
	gameState->ProcessEvent(gameState->Function(_("OnRep_GamePhase")), &params2);

	auto authGameMode = *world->Member<UObject*>(_("AuthorityGameMode"));

	if (authGameMode)
	{
		std::cout << _("Calling StartPlay!\n");

		authGameMode->ProcessEvent(authGameMode->Function(_("StartPlay")), nullptr);
		std::cout << _("StartPlay!\n");

		// *gameState->Member<char>(_("bReplicatedHasBegunPlay")) = true;
		// gameState->ProcessEvent(gameState->Function(_("OnRep_ReplicatedHasBegunPlay")), nullptr);

		/* static auto StartMatchFn = authGameMode->Function(_("StartMatch"));

		if (StartMatchFn)
		{
			authGameMode->ProcessEvent(StartMatchFn, nullptr);
			std::cout << _("Started Match!\n");
		}
		*/
	}
	else
	{
		std::cout << dye::yellow(_("[WARNING] ")) << _("Failed to find AuthorityGameMode!\n");
	}

	Listen(7777);

	InitializeNetHooks();

	std::cout << _("Initialized NetHooks!\n");
}

void ReadyToStartMatchHook(UObject* Object, UFunction* Function, void* Parameters)
{
	return initStuff();
}

inline void PlayButtonHook(UObject* Object, UFunction* Function, void* Parameters)
{
	auto GameInstance = *GetEngine()->Member<UObject*>(_("GameInstance"));
	auto& LocalPlayers = *GameInstance->Member<TArray<UObject*>>(_("LocalPlayers"));
	auto PlayerController = *LocalPlayers.At(0)->Member<UObject*>(_("PlayerController"));

	if (PlayerController)
	{
		static auto SwitchLevelFn = PlayerController->Function(_("SwitchLevel"));
		FString Map;
		Map.Set(L"Athena_Terrain?game=/Game/Athena/Athena_GameMode.Athena_GameMode_C");
		PlayerController->ProcessEvent(SwitchLevelFn, &Map);
		// Map.Free();
	}
	else
	{
		std::cout << dye::red(_("[ERROR] ")) << _("Unable to find PlayerController!\n");
	}
}

void FinishInitializeHooks()
{
	AddHook(_("BndEvt__BP_PlayButton_K2Node_ComponentBoundEvent_1_CommonButtonClicked__DelegateSignature"), PlayButtonHook);
	// AddHook(_("Function /Script/Engine.GameMode.ReadyToStartMatch"), ReadyToStartMatchHook);

	for (auto& Func : FunctionsToHook)
	{
		if (!Func.first)
			std::cout << _("Detected invalid UFunction!\n");
	}

	std::cout << std::format(_("Hooked {} UFunctions!\n"), std::to_string(FunctionsToHook.size()));
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
	}

	return ProcessEventO(Object, Function, Parameters);
}