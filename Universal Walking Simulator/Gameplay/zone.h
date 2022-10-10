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

		auto world = Helper::GetWorld();
		auto AuthGameMode = *world->Member<UObject*>(("AuthorityGameMode"));
		auto SafeZonePhase = *AuthGameMode->Member<int>("SafeZonePhase");

		bool bIsStartZone = SafeZonePhase == 0; // Params->NewState == EFortSafeZoneState::Starting

		//*Indicator->Member<float>("SafeZoneStartShrinkTime"); // how fast the storm iwll shrink

		auto Radius = *Indicator->Member<float>("Radius");
		//*Indicator->Member<float>("NextRadius") = bIsStartZone ? 10000 : Radius / 2;

		auto NextCenter = Indicator->Member<FVector>("NextCenter");

		/*if (bIsStartZone)
		{
			*Indicator->Member<float>("Radius") = 14000;
			*NextCenter = AircraftLocationToUse;
		}
		else {
			*NextCenter += FVector{ (float)distr(gen), (float)distr1(gen1), (float)distr2(gen2) };
		}*/
		static auto GameState = *world->Member<UObject*>(("GameState"));
		auto Aircraft = GameState->Member<TArray<UObject*>>(("Aircrafts"))->At(0);
		static FVector AircraftLocationStatic = Helper::GetActorLocation(Aircraft);

		FString StartShrinkSafeZone;
		StartShrinkSafeZone.Set(L"startshrinksafezone");

		FString SkipShrinkSafeZone;
		SkipShrinkSafeZone.Set(L"skipshrinksafezone");
		if (SafeZonePhase == 0) {
			Helper::Console::ExecuteConsoleCommand(SkipShrinkSafeZone);
			*Indicator->Member<float>("Radius") = 20000;
			*Indicator->Member<float>("NextRadius") = 20000;
			*NextCenter = AircraftLocationToUse;
		}
		if (SafeZonePhase == 1) {
			Helper::Console::ExecuteConsoleCommand(SkipShrinkSafeZone);
			*Indicator->Member<float>("NextRadius") = 20000;
			*NextCenter = AircraftLocationToUse;
		}
		if (SafeZonePhase == 2) {
			Helper::Console::ExecuteConsoleCommand(SkipShrinkSafeZone);
			*Indicator->Member<float>("NextRadius") = 20000;
			*NextCenter = AircraftLocationToUse;
		}
		if (SafeZonePhase == 3) {
			Helper::Console::ExecuteConsoleCommand(SkipShrinkSafeZone);
			*Indicator->Member<float>("NextRadius") = 20000;
			*NextCenter = AircraftLocationToUse;
		}
		if (SafeZonePhase == 4) {
			Helper::Console::ExecuteConsoleCommand(StartShrinkSafeZone);
			*Indicator->Member<float>("NextRadius") = 9500;
			*NextCenter = AircraftLocationToUse;
		}
		if (SafeZonePhase == 5) {
			Helper::Console::ExecuteConsoleCommand(StartShrinkSafeZone);
			*Indicator->Member<float>("NextRadius") = 4000;
			*NextCenter = AircraftLocationToUse;
		}
		if (SafeZonePhase == 6) {
			Helper::Console::ExecuteConsoleCommand(StartShrinkSafeZone);
			*Indicator->Member<float>("NextRadius") = 1000;
			*NextCenter = AircraftLocationToUse;
		}
		if (SafeZonePhase == 7) {
			Helper::Console::ExecuteConsoleCommand(StartShrinkSafeZone);
			*Indicator->Member<float>("NextRadius") = 0;
			*NextCenter = AircraftLocationToUse;
		}
		if (SafeZonePhase == 8) {
			Helper::Console::ExecuteConsoleCommand(StartShrinkSafeZone);
			*Indicator->Member<float>("NextRadius") = 0;
			*NextCenter = AircraftLocationToUse;
		}
		if (SafeZonePhase == 9) {
			Helper::Console::ExecuteConsoleCommand(StartShrinkSafeZone);
			*Indicator->Member<float>("NextRadius") = 0;
			*NextCenter = AircraftLocationToUse;
		}
	}

	return true;
}
