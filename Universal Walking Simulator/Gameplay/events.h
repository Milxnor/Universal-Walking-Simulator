#pragma once
#include <UE/structs.h>
namespace Events {
	bool HasEvent() {
		float Version = std::stof(FN_Version);
		return (Version == 10.40f || Version == 9.40f || Version == 8.51f || Version == 7.30f || Version == 7.20f || Version == 6.21f || Version == 4.5f);
	}

	void LoadEvents() {
		float Version = std::stof(FN_Version);
		if (HasEvent()) {
			std::cout << _("Loading Event!\n");
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
			if (Version == 7.20f) {
				UObject* ML = FindObject("BP_MooneyLoader_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.BP_MooneyLoader_2");
				UObject* Func = ML->Function("LoadSequence");
				UObject* Func2 = ML->Function("LoadMap");
				ML->ProcessEvent(Func2);
				ML->ProcessEvent(Func);
			}
			if (Version == 6.21f) {
				UObject* BF = FindObject("BP_Butterfly_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.BP_Butterfly_4");
				UObject* Func = BF->Function("LoadButterflySublevel");
				BF->ProcessEvent(Func);
			}
		}
	}

	void StartEvent() {
		float Version = std::stof(FN_Version);
		if (HasEvent()) {
			std::cout << _("Starting Event!\n");
			if (Version == 10.40f) {
				UObject* NN = FindObject("LevelSequencePlayer /Game/Athena/Maps/Test/S10/NightNightSequenceMap.NightNightSequenceMap.PersistentLevel.NightNight.AnimationPlayer");
				if (NN)
				{
					UObject* Func = NN->Function("Play");
					if (Func)
						NN->ProcessEvent(Func);
					else
						std::cout << _("Unable to find Night Night Play function!\n");
				}
				else
					std::cout << _("No NightNight LevelSequence!\n");
			}
			else if (Version == 9.40f) {
				UObject* CD = FindObject("BP_CattusDoggus_Scripting_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.BP_CattusDoggus_Scripting_2");
				UObject* Func = CD->Function("startevent");
				CD->ProcessEvent(Func);
			}
			else if (Version == 8.51f) {
				UObject* SS = FindObject(_("LevelSequencePlayer /Temp/Game/Athena/Maps/POI/Athena_POI_Lake_004c_e347d57e.Athena_POI_Lake_004c.PersistentLevel.SnowSequence_2.AnimationPlayer"));

				if (SS)
				{
					UObject* Func = SS->Function("Play");
					if (Func)
					{
						SS->ProcessEvent(Func);
						HostingWebHook.send_message(_("Started Unvaulting Event!\n"));
						std::cout << _("Started Event!\n");
					}
					else
						std::cout << _("No play func!\n");
				}
				else
					std::cout << _("No Player!\n");

				//(TODO: GD) Find a way to auto destroy death barrier and load the Map for all players when the screen goes white (Map Name: Next2)
				// (MILXNOR) ^ ClientTravel?
			}
			else if (Version == 7.30f) {
				UObject* FS = FindObject(".PersistentLevel.FestivusSequence_01_2.AnimationPlayer2");
				UObject* Func = FS->Function("Play");
				FS->ProcessEvent(Func);
			}
			else if (Version == 7.20f) {
				UObject* MS = FindObject("LevelSequencePlayer /Game/Athena/Maps/Test/S7/MooneySequenceMap.MooneySequenceMap.PersistentLevel.MooneySequence.AnimationPlayer");
				UObject* Func = MS->Function("Play");
				MS->ProcessEvent(Func);
			}
			else if (Version == 6.21f) {
				UObject* BF = FindObject("BP_Butterfly_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.BP_Butterfly_4");
				UObject* Func = BF->Function("ButterflySequence");
				BF->ProcessEvent(Func);
				//(TODO) Fix screen going white when cube explodes (Im guessing its similar to the Map loading issue on 8.51 Event)
			}
			else if (Version == 4.5f) {
				UObject* LR = FindObject("LevelSequencePlayer /Game/Athena/Maps/Test/Events/Athena_Gameplay_Geode.Athena_Gameplay_Geode.PersistentLevel.LevelSequence_LaunchRocket.AnimationPlayer");
				UObject* Func = LR->Function("Play");
				if (Func)
					LR->ProcessEvent(Func);
			}
		}
	}
}
