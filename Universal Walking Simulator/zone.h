#pragma once

#include "helper.h"

bool OnSafeZoneStateChangeHook(UObject* Indicator, UFunction* Function, void* Parameters) // TODO: Change this func
{
	std::cout << "OnSafeZoneStateChange!\n";

	struct ASafeZoneIndicator_C_OnSafeZoneStateChange_Params
	{
	public:
		EFortSafeZoneState              NewState;                                          // 0x0(0x1)(BlueprintVisible, BlueprintReadOnly, Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash)
		bool                                       bInitial;                                          // 0x1(0x1)(BlueprintVisible, BlueprintReadOnly, Parm, ZeroConstructor, IsPlainOldData, NoDestructor)
	};

	auto Params = (ASafeZoneIndicator_C_OnSafeZoneStateChange_Params*)Parameters;

	ProcessEventO(Indicator, Function, Parameters);

	if (Indicator && Parameters && Helper::IsSmallZoneEnabled())
	{
		std::random_device rd; // obtain a random number from hardware
		std::mt19937 gen(rd()); // seed the generator
		std::uniform_int_distribution<> distr(300.f, 5000.f);

		std::random_device rd1; // obtain a random number from hardware
		std::mt19937 gen1(rd1()); // seed the generator
		std::uniform_int_distribution<> distr1(300.f, 5000.f);

		std::random_device rd2; // obtain a random number from hardware
		std::mt19937 gen2(rd2()); // seed the generator
		std::uniform_int_distribution<> distr2(300.f, 5000.f);

		auto world = Helper::GetWorld();
		auto AuthGameMode = Helper::GetGameMode();
		auto SafeZonePhase = *AuthGameMode->Member<int>("SafeZonePhase");

		bool bIsStartZone = SafeZonePhase == 0; // Params->NewState == EFortSafeZoneState::Starting

		// *Indicator->Member<float>("SafeZoneStartShrinkTime") // how fast the storm iwll shrink

		auto Radius = *Indicator->Member<float>("Radius");
		*Indicator->Member<float>("NextRadius") = 8250;

		auto NextCenter = Indicator->Member<FVector>("NextCenter");

		// if (bIsStartZone)
		{
			*Indicator->Member<float>("Radius") = 14000;
			*NextCenter = AircraftLocationToUse;
		}
		// else
			// *NextCenter += FVector{ (float)distr(gen), (float)distr1(gen1), (float)distr2(gen2) };

		std::cout << "NewState: " << (int)Params->NewState << '\n';
		std::cout << "bInitial: " << Params->bInitial << '\n';
		std::cout << "SafeZonePhase: " << SafeZonePhase << '\n';
	}

	return true;
}
