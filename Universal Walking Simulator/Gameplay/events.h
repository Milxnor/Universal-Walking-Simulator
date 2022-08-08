#pragma once
#include <UE/structs.h>
namespace Events {
	inline bool HasEvent() {
		float Version = std::stof(FN_Version);
		return (Version == 12.61f || /* Version == 12.41f || */ Version == 10.40f || Version == 9.40f || Version == 8.51f || Version == 7.30f || Version == 7.20f || Version == 6.21f || Version == 4.5f);
	}

	void LoadEvents() {
		float Version = std::stof(FN_Version);

		if (HasEvent()) {
			std::cout << ("Loading Event!\n");

			if (Version == 12.61f) {
				auto FritterLoader = FindObject(("BP_Fritter_Loader_C /Fritter/Level/FritterLoaderLevel.FritterLoaderLevel.PersistentLevel.BP_Fritter_Loader_0"));
				auto LoadFritterLevel = FindObject(("Function /Fritter/BP_Fritter_Loader.BP_Fritter_Loader_C.LoadFritterLevel"));
				bool CO = true;
				FritterLoader->ProcessEvent(LoadFritterLevel, &CO);
			}

			if (Version == 12.41f) {
				static auto JL = FindObject(("BP_Jerky_Loader_C /CycloneJerky/Levels/JerkyLoaderLevel.JerkyLoaderLevel.PersistentLevel.BP_Jerky_Loader_2"));

				static auto JerkyLoadLevel = FindObject(("Function /CycloneJerky/Gameplay/BP_Jerky_Loader.BP_Jerky_Loader_C.LoadJerkyLevel"));

				bool CO = true;

				if (JL)
					JL->ProcessEvent(JerkyLoadLevel, &CO);
			}
			if (Version == 10.40f) {
				//The End C1
				UObject* NN = FindObject(("BP_NightNight_Scripting_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.BP_NightNight_Scripting_2"));
				UObject* Func = NN->Function(("LoadNightNightLevel"));
				bool Condition = true;
				NN->ProcessEvent(Func, &Condition);
			}
			if (Version == 9.40f) {
				//The Final Showdown
				UObject* CD = FindObject(("BP_CattusDoggus_Scripting_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.BP_CattusDoggus_Scripting_2"));
				UObject* Func = CD->Function(("LoadCattusLevel"));
				bool Condition = true;
				CD->ProcessEvent(Func, &Condition);
			}
			/*if (Version == 8.51f) {
				UObject* SS = FindObject("");
				UObject* Func = SS->Function("FinalSequence");
			}*/
			if (Version == 7.20f) {
				//Ice King
				UObject* ML = FindObject(("BP_MooneyLoader_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.BP_MooneyLoader_2"));
				UObject* Func = ML->Function(("LoadSequence"));
				UObject* Func2 = ML->Function(("LoadMap"));
				ML->ProcessEvent(Func2);
				ML->ProcessEvent(Func);
			}
			if (Version == 6.21f) {
				//Butterfly
				UObject* BF = FindObject(("BP_Butterfly_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.BP_Butterfly_4"));
				UObject* Func = BF->Function(("LoadButterflySublevel"));
				BF->ProcessEvent(Func);
			}
		}
	}

	void StartEvent() {
		float Version = std::stof(FN_Version);
		if (HasEvent()) {
			std::cout << ("Starting Event!\n");

			if (Version == 12.61f) {
				auto AP = FindObject(("LevelSequencePlayer /Fritter/Level/FritterSequenceLevel.FritterSequenceLevel.PersistentLevel.Fritter_2.AnimationPlayer"));
				
				if (AP)
				{
					auto Function = FindObject(("Function /Script/MovieScene.MovieSceneSequencePlayer.Play"));
					AP->ProcessEvent(Function);
				}
				else
					std::cout << "Failed to find Fritter AP!\n";
			}

			if (Version == 12.41f) {
				static auto JL = FindObject(("BP_Jerky_Loader_C /CycloneJerky/Levels/JerkyLoaderLevel.JerkyLoaderLevel.PersistentLevel.BP_Jerky_Loader_2"));

				static auto Function = FindObject(("Function /CycloneJerky/Gameplay/BP_Jerky_Loader.BP_Jerky_Loader_C.CallStartEventOnScripting"));
				JL->ProcessEvent(Function);
				// static auto JS = FindObject(("LevelSequencePlayer /CycloneJerky/Levels/JerkySequenceMap_LevelInstance_1.JerkySequenceMap.PersistentLevel.Jerky.AnimationPlayer"));
			}

			else if (Version == 10.40f) {
				//The End C1 (Crashes)
				UObject* NN = FindObject(("LevelSequencePlayer /Game/Athena/Maps/Test/S10/NightNightSequenceMap.NightNightSequenceMap.PersistentLevel.NightNight.AnimationPlayer"));
				if (NN)
				{
					UObject* Func = NN->Function(("Play"));
					if (Func)
						NN->ProcessEvent(Func);
					else
						std::cout << ("Unable to find Night Night Play function!\n");
				}
				else
					std::cout << ("No NightNight LevelSequence!\n");
			}
			else if (Version == 9.40f) {
				//Final Showdown
				UObject* CD = FindObject(("BP_CattusDoggus_Scripting_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.BP_CattusDoggus_Scripting_2"));
				UObject* Func = CD->Function(("startevent"));
				CD->ProcessEvent(Func);
			}
			else if (Version == 8.51f) {
				//Unvaulting
				/*UObject* SS = FindObject(("LevelSequencePlayer /Temp/Game/Athena/Maps/POI/Athena_POI_Lake_004c_e347d57e.Athena_POI_Lake_004c.PersistentLevel.SnowSequence_2.AnimationPlayer"));

				if (SS)
				{
					UObject* Func = SS->Function(("Play"));
					if (Func)
					{
						SS->ProcessEvent(Func);
						HostingWebHook.send_message(("Started Unvaulting Event!\n"));
						std::cout << ("Started Event!\n");
					}
					else
						std::cout << ("No play func!\n");
				}
				else
					std::cout << ("No Player!\n");*/

				UObject* BSS = FindObject("BP_SnowScripting_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.BP_SnowScripting_2");
				UObject* Func = BSS->Function("FinalSequence");
				BSS->ProcessEvent(Func);

					//(TODO: GD) Find a way to auto destroy death barrier and load the Map for all players when the screen goes white (Map Name: Next2)
					// (MILXNOR) ^ ClientTravel?
			}
			else if (Version == 7.30f) {
				//Marshmello
				UObject* FS = FindObject((".PersistentLevel.FestivusSequence_01_2.AnimationPlayer2"));
				UObject* Func = FS->Function(("Play"));
				FS->ProcessEvent(Func);
			}
			else if (Version == 7.20f) {
				//Ice King
				UObject* MS = FindObject(("LevelSequencePlayer /Game/Athena/Maps/Test/S7/MooneySequenceMap.MooneySequenceMap.PersistentLevel.MooneySequence.AnimationPlayer"));
				UObject* Func = MS->Function(("Play"));
				MS->ProcessEvent(Func);
			}
			else if (Version == 6.21f) {
				//Butterfly
				UObject* BF = FindObject(("BP_Butterfly_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.BP_Butterfly_4"));
				UObject* Func = BF->Function(("ButterflySequence"));
				BF->ProcessEvent(Func);
				//(TODO) Fix screen going white when cube explodes (Im guessing its similar to the Map loading issue on 8.51 Event)
			}
			else if (Version == 4.5f) {
				//Rocket
				UObject* LR = FindObject(("LevelSequencePlayer /Game/Athena/Maps/Test/Events/Athena_Gameplay_Geode.Athena_Gameplay_Geode.PersistentLevel.LevelSequence_LaunchRocket.AnimationPlayer"));
				UObject* Func = LR->Function(("Play"));
				if (Func)
					LR->ProcessEvent(Func);
			}
		}
	}
}
