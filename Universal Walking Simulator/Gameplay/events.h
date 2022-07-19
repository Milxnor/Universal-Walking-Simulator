#pragma once
#include <UE/structs.h>
namespace Events {
	bool HasEvent() {
		float Version = std::stof(FN_Version);
		return (Version == 10.40f || Version == 9.40f || Version == 7.30f || Version == 7.20f || Version == 6.21f || Version == 4.5f);
	}

	void LoadEvents() {
		std::cout << _("Loading Event!\n");
		float Version = std::stof(FN_Version);
		if (HasEvent()) {
			if (Version == 10.40f) {
				UObject* NN = FindObject("BP_NightNight_Scripting_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.BP_NightNight_Scripting_2");
				UObject* Func = NN->Function("LoadNightNightLevel");
				bool Condition = true;
				NN->ProcessEvent(Func, &Condition);
			}
			if (Version == 9.40f) {
				UObject* CD = FindObject("BP_CattusDoggus_Scripting_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.BP_CattusDoggus_Scripting_2");
				UObject* Func = CD->Function("LoadCattusLevel");
				bool Condition = true;
				CD->ProcessEvent(Func, &Condition);
			}
		}
	}

	void StartEvent() {
		float Version = std::stof(FN_Version);
		if (HasEvent()) {
			std::cout << _("Starting Event!\n");
			if (Version == 10.40f) {
				UObject* NN = FindObject("LevelSequencePlayer /Game/Athena/Maps/Test/S10/NightNightSequenceMap.NightNightSequenceMap.PersistentLevel.NightNight.AnimationPlayer");
				UObject* Func = NN->Function("Play");
				NN->ProcessEvent(Func);
			}
			if (Version == 9.40f) {
				UObject* CD = FindObject("BP_CattusDoggus_Scripting_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.BP_CattusDoggus_Scripting_2");
				UObject* Func = CD->Function("startevent");
				CD->ProcessEvent(Func);
			}
			if (Version == 7.30f) {
				UObject* CD = FindObject(".Athena_POI_CommunityPark_003_M.PersistentLevel.FestivusSequence_01_2.AnimationPlayer2");
				UObject* Func = CD->Function("Play");
				CD->ProcessEvent(Func);
			}
			if (Version == 4.5f) {
				UObject* CD = FindObject("LevelSequencePlayer /Game/Athena/Maps/Test/Events/Athena_Gameplay_Geode.Athena_Gameplay_Geode.Athena_Gameplay_Geode.PersistentLevel.LevelSequence_LaunchRocket.AnimationPlayer");
				UObject* Func = CD->Function("Play");
				CD->ProcessEvent(Func);
			}
		}
	}
}