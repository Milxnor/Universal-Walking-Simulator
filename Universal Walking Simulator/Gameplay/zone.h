#pragma once

#include "helper.h"

bool OnSafeZoneStateChangeHook(UObject* Indicator, UFunction* Function, void* Parameters) // TODO: Change this func
{
	std::cout << "OnSafeZoneStateChange!\n";

	if (Indicator && Parameters && Helper::IsSmallZoneEnabled())
	{
		struct ASafeZoneIndicator_C_OnSafeZoneStateChange_Params
		{
		public:
			EFortSafeZoneState              NewState;                                          // 0x0(0x1)(BlueprintVisible, BlueprintReadOnly, Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash)
			bool                                       bInitial;                                          // 0x1(0x1)(BlueprintVisible, BlueprintReadOnly, Parm, ZeroConstructor, IsPlainOldData, NoDestructor)
		};

		auto Params = (ASafeZoneIndicator_C_OnSafeZoneStateChange_Params*)Parameters;

		std::random_device rd; // obtain a random number from hardware
		std::mt19937 gen(rd()); // seed the generator
		std::uniform_int_distribution<> distr(100.f, 1000.f);

		std::random_device rd1; // obtain a random number from hardware
		std::mt19937 gen1(rd1()); // seed the generator
		std::uniform_int_distribution<> distr1(100.f, 1000.f);

		std::random_device rd2; // obtain a random number from hardware
		std::mt19937 gen2(rd2()); // seed the generator
		std::uniform_int_distribution<> distr2(100.f, 1000.f);

		static auto NextCenterOffset = FindOffsetStruct("Class /Script/FortniteGame.FortSafeZoneIndicator", "NextCenter");
		auto NextCenter = (FVector*)(__int64(Indicator) + NextCenterOffset);

		auto world = Helper::GetWorld();
		auto AuthGameMode = Helper::GetGameMode();

		static auto SafeZonePhaseOffset = GetOffset(AuthGameMode, "SafeZonePhase");
		auto SafeZonePhase = *(int*)(__int64(AuthGameMode) + SafeZonePhaseOffset);

		bool bIsStartZone = SafeZonePhase == 0; // Params->NewState == EFortSafeZoneState::Starting

		//*Indicator->Member<float>("SafeZoneStartShrinkTime"); // how fast the storm iwll shrink

		//*Indicator->Member<float>("NextRadius") = bIsStartZone ? 10000 : Radius / 2;

		auto GameState = Helper::GetGameState();

		static auto AircraftsOffset = GetOffset(GameState, "Aircrafts");
		auto Aircrafts = (TArray<UObject*>*)(__int64(GameState) + AircraftsOffset);

		auto Aircraft = Aircrafts->At(0);

		static FVector AircraftLocation;

		static int LastResetNum = 824524899135;

		if (LastResetNum != AmountOfRestarts) // only run these once per game // we would do this when map is fully loaded but no func
		{
			LastResetNum = AmountOfRestarts;
			AircraftLocation = Helper::GetActorLocation(Aircraft);
		}

		FString StartShrinkSafeZone;
		StartShrinkSafeZone.Set(L"startshrinksafezone");

		FString SkipShrinkSafeZone;
		SkipShrinkSafeZone.Set(L"skipshrinksafezone");

		auto NextRadiusOffset = GetOffset(Indicator, "NextRadius");
		auto NextRadius = (float*)(__int64(Indicator) + NextRadiusOffset);

		std::cout << "SafeZonePhase: " << SafeZonePhase << '\n';

		if (SafeZonePhase == 0) {
			Helper::Console::ExecuteConsoleCommand(SkipShrinkSafeZone);
			static auto RadiusOffset = GetOffset(Indicator, "Radius");
			*(float*)(__int64(Indicator) + RadiusOffset) = 20000;

			*NextRadius = 20000;
			*NextCenter = AircraftLocationToUse;
		}
		if (SafeZonePhase == 1) {
			Helper::Console::ExecuteConsoleCommand(SkipShrinkSafeZone);
			*NextRadius = 20000;
			*NextCenter = AircraftLocationToUse;
		}
		if (SafeZonePhase == 2) {
			Helper::Console::ExecuteConsoleCommand(SkipShrinkSafeZone);
			*NextRadius = 20000;
			*NextCenter = AircraftLocationToUse;
		}
		if (SafeZonePhase == 3) {
			Helper::Console::ExecuteConsoleCommand(SkipShrinkSafeZone);
			*NextRadius = 20000;
			*NextCenter = AircraftLocationToUse;
		}
		if (SafeZonePhase == 4) {
			Helper::Console::ExecuteConsoleCommand(StartShrinkSafeZone);
			*NextRadius = 9500;
			*NextCenter = AircraftLocationToUse; // + FVector{ distr(gen), distr1(gen1), distr2(gen2) };

			// *NextCenter += FVector{ distr(gen), distr1(gen1), distr2(gen2) };
		}
		if (SafeZonePhase == 5) {
			Helper::Console::ExecuteConsoleCommand(StartShrinkSafeZone);
			*NextRadius = 4000;
			*NextCenter = AircraftLocationToUse;
		}
		if (SafeZonePhase == 6) {
			Helper::Console::ExecuteConsoleCommand(StartShrinkSafeZone);
			*NextRadius = 1000;
			*NextCenter = AircraftLocationToUse;
		}
		if (SafeZonePhase == 7) {
			Helper::Console::ExecuteConsoleCommand(StartShrinkSafeZone);
			*NextRadius = 0;
			*NextCenter = AircraftLocationToUse;
		}
		if (SafeZonePhase == 8) {
			Helper::Console::ExecuteConsoleCommand(StartShrinkSafeZone);
			*NextRadius = 0;
			*NextCenter = AircraftLocationToUse;
		}
		if (SafeZonePhase == 9) {
			Helper::Console::ExecuteConsoleCommand(StartShrinkSafeZone);
			*NextRadius = 0;
			*NextCenter = AircraftLocationToUse;
		}
	}

	return true;
}
