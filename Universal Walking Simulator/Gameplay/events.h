#pragma once

#include <UE/structs.h>
#include <Gameplay/helper.h>

static UObject* JerkyBPLoader = nullptr;
static UObject* JerkyLoaderActual = nullptr;
static UObject* JerkyPlayerInteraction = nullptr;

bool POI1Hook(UObject* SequenceDirector, UFunction*, void* Parameters)
{
	auto SnowScripting = GetSnowScripting();

	SnowScripting->ProcessEvent("LoadFoundation1");

	return false;
}

bool POI2Hook(UObject* SequenceDirector, UFunction*, void* Parameters)
{
	auto SnowScripting = GetSnowScripting();

	SnowScripting->ProcessEvent("LoadFoundation2");

	return false;
}

bool POI3Hook(UObject* SequenceDirector, UFunction*, void* Parameters)
{
	auto SnowScripting = GetSnowScripting();

	SnowScripting->ProcessEvent("LoadFoundation3");

	return false;
}

void InitializeEventHooks()
{
	if (FnVerDouble == 8.51)
	{
		AddHook("Function /Game/Athena/Prototype/Blueprints/White/SnowEndSequence.SequenceDirector_C.POI1", POI1Hook); // Retail
		AddHook("Function /Game/Athena/Prototype/Blueprints/White/SnowEndSequence.SequenceDirector_C.POI2", POI2Hook); // Polar
		AddHook("Function /Game/Athena/Prototype/Blueprints/White/SnowEndSequence.SequenceDirector_C.POI3", POI3Hook); // Tilted
	}
}

namespace EventHelper
{
	std::string UV_ItemName = "DrumGun";
	void UnvaultItem(FName ItemName) {
		UObject* BSS = GetSnowScripting();
		UObject* Func = BSS->Function("PillarsConcluded");
		BSS->ProcessEvent(Func, &ItemName);
		BSS->ProcessEvent("PillarsAlreadyConcluded");
	}

	void TeleportPlayersToButterfly()
	{
		static auto scripting = FindObject("BP_IslandScripting_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.BP_IslandScripting3");

		if (!scripting)
		{
			std::cout << "[WARNING] Unable to find IslandScripting!\n";
			return;
		}

		UObject* ButterflyBP = *scripting->Member<UObject*>("ButterflyBP");
		
		auto PlayersArrayPawns = ButterflyBP->Member<TArray<UObject*>>("PlayersArray");

		std::cout << "PlayersArrayPawns: " << PlayersArrayPawns->Num() << '\n';

		ButterflyBP->ProcessEvent("TeleportPawns");

		auto Locations = ButterflyBP->Member<TArray<FTransform>>("PlayerLocations");

		std::cout << "Num PlayerLocations: " << Locations->Num() << '\n';

		/* auto World = Helper::GetWorld();

		if (World)
		{
			auto NetDriver = *World->Member<UObject*>(("NetDriver"));
			if (NetDriver)
			{
				auto ClientConnections = NetDriver->Member<TArray<UObject*>>(("ClientConnections"));

				if (ClientConnections)
				{
					for (int i = 0; i < ClientConnections->Num(); i++)
					{
						auto Connection = ClientConnections->At(i);

						if (!Connection)
							continue;

						auto Controller = *Connection->Member<UObject*>(("PlayerController"));

						if (Controller)
						{
							auto NewPawn = *Controller->Member<UObject*>("Pawn");
							Helper::SetActorLocation(NewPawn, Locations->At(i).Translation);
						}
					}
				}
			}
		} */
	}
	
	void ApplyGEsTravis()
	{
		auto World = Helper::GetWorld();
		if (World)
		{
			auto NetDriver = *World->Member<UObject*>(("NetDriver"));
			if (NetDriver)
			{
				auto ClientConnections = NetDriver->Member<TArray<UObject*>>(("ClientConnections"));

				if (ClientConnections)
				{
					for (int i = 0; i < ClientConnections->Num(); i++)
					{
						auto Connection = ClientConnections->At(i);

						if (!Connection)
							continue;

						auto Controller = *Connection->Member<UObject*>(("PlayerController"));

						if (Controller)
						{
							auto NewPawn = *Controller->Member<UObject*>("Pawn");

							if (NewPawn)
							{
								static auto GES = JerkyBPLoader->Function("AddEventInProgressGEs");
								static auto GES2 = JerkyBPLoader->Function("EventInProgressGEs");
								JerkyBPLoader->ProcessEvent(GES, &NewPawn);
								JerkyBPLoader->ProcessEvent(GES2, &NewPawn);
							}

						}
					}

				}

			}
		}
	}

	// idk if this works
	void LoadAndUnloadLake(bool bAfterEvent = true) // load the after event lake and unload the old one
	{
		static auto FloatingIsland = FindObject(("LF_Athena_POI_15x15_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.LF_FloatingIsland"));
		static auto Lake = FindObject(("LF_Athena_POI_75x75_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.LF_Lake1"));
		static auto Lake2 = FindObject("LF_Athena_POI_75x75_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.LF_Lake2");

		Helper::ShowBuilding(FloatingIsland, !bAfterEvent);
		Helper::ShowBuilding(Lake, !bAfterEvent);
		Helper::ShowBuilding(Lake2, bAfterEvent);

		// OnRep_ShowLakeRainbow
	}

	void BoostUpTravis()
	{
		TArray<UObject*> Pawns;

		auto World = Helper::GetWorld();
		if (World)
		{
			auto NetDriver = *World->Member<UObject*>(("NetDriver"));
			if (NetDriver)
			{
				auto ClientConnections = NetDriver->Member<TArray<UObject*>>(("ClientConnections"));

				if (ClientConnections)
				{
					for (int i = 0; i < ClientConnections->Num(); i++)
					{
						auto Connection = ClientConnections->At(i);

						if (!Connection)
							continue;

						auto Controller = *Connection->Member<UObject*>(("PlayerController"));

						if (Controller)
						{
							auto NewPawn = *Controller->Member<UObject*>("Pawn");

							if (NewPawn)
							{
								Pawns.Add(NewPawn);
							}

						}

					}
				}
			}
		}

		auto fn = JerkyBPLoader->Function("PawnsAvailableFlyUp");
		auto disablegliders = JerkyBPLoader->Function("ToggleParachute");

		bool bdisabled = true;

		JerkyBPLoader->ProcessEvent(fn, &Pawns);
		JerkyBPLoader->ProcessEvent(disablegliders, &bdisabled);
	}
}

namespace Events { // made by GD
	inline bool HasEvent() {
		float Version = std::stof(FN_Version);
		return (Version == 13.40f || Version == 13.30f || Version == 12.61f ||  Version == 12.41f ||  Version == 10.40f || Version == 9.40f || Version == 8.51f || Version == 7.30f || Version == 7.20f || Version == 6.21f || Version == 4.5f);
	}

	void LoadEvents() {
		float Version = std::stof(FN_Version);
		int Season = std::floor(Version);

		if (HasEvent()) {
			std::cout << ("Loading Event!\n");

			if (Version == 12.61f) {
				auto FritterLoader = FindObjectOld(("BP_Fritter_Loader_C /Fritter/Level/FritterLoaderLevel.FritterLoaderLevel.PersistentLevel.BP_Fritter_Loader_"));
				auto LoadFritterLevel = FindObject(("Function /Fritter/BP_Fritter_Loader.BP_Fritter_Loader_C.LoadFritterLevel"));
				bool CO = true;

				if (FritterLoader)
					FritterLoader->ProcessEvent(LoadFritterLevel, &CO);
				else
					std::cout << "Unable to find FritterLoader!\n";
			}

			if (Version == 12.41f) {
				static auto JL = FindObject(("BP_Jerky_Loader_C /CycloneJerky/Levels/JerkyLoaderLevel.JerkyLoaderLevel.PersistentLevel.BP_Jerky_Loader_2"));

				if (JL)
				{
					JerkyLoaderActual = JL;
				}
				else
					std::cout << "No loader!\n";
			}

			if (Version == 10.40f) {
				//The End C1
				UObject* NN = FindObject(("BP_NightNight_Scripting_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.BP_NightNight_Scripting_2"));
				UObject* Func = NN->Function(("LoadNightNightLevel"));
				bool Condition = true;
				NN->ProcessEvent(Func, &Condition);

				// LoadLevel
			}
			if (Version == 9.40f) {
				//The Final Showdown
				UObject* CD = FindObject(("BP_CattusDoggus_Scripting_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.BP_CattusDoggus_Scripting_2"));
				UObject* Func = CD->Function(("LoadCattusLevel"));
				bool Condition = true;
				CD->ProcessEvent(Func, &Condition);
			}
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

			if (Version == 13.30f || Version == 13.40f) {
				UObject* Main_C = FindObject("AthenaQuestActor_SW_Main_C /Temp/Game/Athena/Apollo/Maps/Streaming/Apollo_Terrain_Sub_F1_61e6b1c6.Apollo_Terrain_Sub_F1.PersistentLevel.AthenaQuestActor_SW_Main2");
				UObject* Function = Main_C->Function("PlayLaunchAnim");
				Main_C->ProcessEvent(Function);
			}

			if (Version == 12.61f) {
				auto gamestate = Helper::GetGameState();
				static auto loader = FindObject("BP_Fritter_Loader_C /Fritter/Level/FritterLoaderLevel.FritterLoaderLevel.PersistentLevel.BP_Fritter_Loader_0");

				std::cout << "loader: " << loader << '\n';

				struct
				{
					UObject* GameState;
					UObject* Playlist;
					FGameplayTagContainer PlaylistContextTags;
				}Func1_params{ gamestate, Helper::GetPlaylist(), FGameplayTagContainer()};

				auto Func1 = loader->Function("OnReady_1216203B4B63E3DFA03042A62380A674");

				if (Func1)
					loader->ProcessEvent(Func1, &Func1_params);

				struct
				{
					float NewParam;
				}Func2_params{ 0 };
				static auto Func2 = loader->Function("startevent");

				if (Func2)
					loader->ProcessEvent(Func2, &Func2_params);
				/*
				static auto scripoting = FindObject("BP_Fritter_Script_C /Fritter/Level/FritterSequenceLevel_LevelInstance_1.FritterSequenceLevel.PersistentLevel.BP_Fritter_Script_2");

				if (scripoting)
				{
					static auto startevent = scripoting->Function("startevent");
					
					if (startevent)
						scripoting->ProcessEvent(startevent);
					else
						std::cout << "no start event!\n";
				}
				else
				{
					std::cout << "no scripting!\n";

					auto AP = FindObject(("LevelSequencePlayer /Fritter/Level/FritterSequenceLevel.FritterSequenceLevel.PersistentLevel.Fritter_2.AnimationPlayer"));

					if (AP)
					{
						auto Function = FindObject(("Function /Script/MovieScene.MovieSceneSequencePlayer.Play"));
						AP->ProcessEvent(Function);
					}
					else
						std::cout << "Failed to find Fritter AP!\n";
				} */
			}

			if (Version == 12.41f) {
				JerkyPlayerInteraction = FindObject(("BP_Jerky_PlayerInteraction_C /CycloneJerky/Levels/JerkySequenceMap_LevelInstance_1.JerkySequenceMap.PersistentLevel.BP_Jerky_PlayerInteraction_2"));
				JerkyBPLoader = FindObject(("BP_Jerky_Scripting_C /CycloneJerky/Levels/JerkySequenceMap_LevelInstance_1.JerkySequenceMap.PersistentLevel.BP_Jerky_Scripting_2"));
			
				JerkyPlayerInteraction = FindObject(("BP_Jerky_PlayerInteraction_C /CycloneJerky/Levels/JerkySequenceMap_LevelInstance_1.JerkySequenceMap.PersistentLevel.BP_Jerky_PlayerInteraction_2"));
				JerkyBPLoader = FindObject(("BP_Jerky_Scripting_C /CycloneJerky/Levels/JerkySequenceMap_LevelInstance_1.JerkySequenceMap.PersistentLevel.BP_Jerky_Scripting_2"));

				if (JerkyBPLoader && JerkyPlayerInteraction)
				{
					auto TeleportDistant = JerkyBPLoader->Function("TeleportDistantPlayers");
					auto DebugStartSequence = JerkyBPLoader->Function("DebugStartSequence");

					float SequenceTime = 0.0f;

					JerkyBPLoader->ProcessEvent(TeleportDistant);
					JerkyBPLoader->ProcessEvent(DebugStartSequence, &SequenceTime);
					EventHelper::ApplyGEsTravis();
					
					auto World = Helper::GetWorld();
					if (World)
					{
						auto NetDriver = *World->Member<UObject*>(("NetDriver"));
						if (NetDriver)
						{
							auto ClientConnections = NetDriver->Member<TArray<UObject*>>(("ClientConnections"));

							if (ClientConnections)
							{
								static auto BuildingItemData_Wall = FindObject(("FortBuildingItemDefinition /Game/Items/Weapons/BuildingTools/BuildingItemData_Wall.BuildingItemData_Wall"));
								static auto BuildingItemData_Floor = FindObject(("FortBuildingItemDefinition /Game/Items/Weapons/BuildingTools/BuildingItemData_Floor.BuildingItemData_Floor"));
								static auto BuildingItemData_Stair_W = FindObject(("FortBuildingItemDefinition /Game/Items/Weapons/BuildingTools/BuildingItemData_Stair_W.BuildingItemData_Stair_W"));
								static auto BuildingItemData_RoofS = FindObject(("FortBuildingItemDefinition /Game/Items/Weapons/BuildingTools/BuildingItemData_RoofS.BuildingItemData_RoofS"));

								for (int i = 0; i < ClientConnections->Num(); i++)
								{
									auto Connection = ClientConnections->At(i);

									if (!Connection)
										continue;

									auto Controller = *Connection->Member<UObject*>(("PlayerController"));

									ClearInventory(Controller, true);
								}
							}
						}
					}

					bIsInEvent = true;
				}
				else
				{
					printf("Jerky Sequencer or Jerky Player Interaction is not valid \n");
				}
			}

			else if (Version == 10.40f) {
				// UObject* NN = FindObject("LevelSequencePlayer /Game/Athena/Maps/Test/S10/NightNightSequenceMap.NightNightSequenceMap.PersistentLevel.NightNight_3.AnimationPlayer"); // FindObject(("LevelSequencePlayer /Game/Athena/Maps/Test/S10/NightNightSequenceMap.NightNightSequenceMap.PersistentLevel.NightNight.AnimationPlayer"));
				
				UObject* NNScripting = FindObjectOld("BP_NightNight_Scripting_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.BP_NightNight_Scripting_");

				if (NNScripting)
				{
					static UObject* Func = NNScripting->Function(("startevent"));
					if (Func)
						NNScripting->ProcessEvent(Func);
					else
						std::cout << ("Unable to find Night Night startevetn function!\n");
				}
				else
					std::cout << ("No NightNight scripting!\n");
			}
			else if (Version == 9.40f) {
				//Final Showdown
				UObject* CD = FindObject(("BP_CattusDoggus_Scripting_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.BP_CattusDoggus_Scripting_2"));
				UObject* Func = CD->Function(("startevent"));
				CD->ProcessEvent(Func);
			}
			else if (Version == 8.51f) {
				//Unvaulting
				UObject* BSS = FindObject("BP_SnowScripting_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.BP_SnowScripting_2");
				UObject* Func = BSS->Function("FinalSequence");
				BSS->ProcessEvent(Func);

				// BSS->Member<FTimespan>("TimeUntilCountdownEnd")->Ticks = 1000000000; // 1:38
				// StartCountdown
			}

			else if (Version == 7.30f) {
				//Marshmello
				/* UObject* FS = FindObjectOld(".Athena_POI_CommunityPark_003_M.PersistentLevel.FestivusSequence_01_2.AnimationPlayer");
				if (FS)
				{
					UObject* Func = FS->Function(("Play"));
					FS->ProcessEvent(Func);
				}
				else
					std::cout << "Failed to find FestivusSequence!\n"; */

				UObject* RepSequencePlayer = FindObjectOld(".PersistentLevel.BP_RepSequencePlayer_");

				std::cout << "RepSequencePlayer: " << RepSequencePlayer << '\n';

				if (RepSequencePlayer)
				{
					auto SequenceActor = *RepSequencePlayer->Member<UObject*>("Sequence");

					std::cout << "SequenceActor: " << SequenceActor << '\n';

					if (SequenceActor)
					{
						SequenceActor->ProcessEvent("PlaySequence");
						*SequenceActor->Member<bool>("ShouldPlaySequence") = true;
						SequenceActor->ProcessEvent("OnRep_ShouldPlaySequence");

						auto FestivusManager = *SequenceActor->Member<UObject*>("FestivusManager");

						std::cout << "FestivusManager: " << FestivusManager << '\n';

						if (FestivusManager)
						{
							FestivusManager->ProcessEvent("StartFestivus");
							FestivusManager->ProcessEvent("MulticastLoad");
							FestivusManager->ProcessEvent("PlayConcert");

							struct { UObject* GameState; UObject* Playlist; FGameplayTagContainer Container; } parmaas{Helper::GetGameState(), Helper::GetPlaylist(), FGameplayTagContainer ()};

							FestivusManager->ProcessEvent("OnReady_EE7676604ADFD92D7B2972AC0ABD4BB8", &parmaas);
							FestivusManager->ProcessEvent("OnReady_0B1AD66A4F9D46C366C04FB29300CB0B");
							FestivusManager->ProcessEvent("OnReady_BF08320243FE76C480E22BB6BF6BA40D");
							
							*FestivusManager->Member<bool>("LoadSubLevel") = true;
							FestivusManager->ProcessEvent("OnRep_LoadSubLevel");
						}
					}
				}
			}
			else if (Version == 7.20f) {
				//Ice King
				/* UObject* MS = FindObject(("LevelSequencePlayer /Game/Athena/Maps/Test/S7/MooneySequenceMap.MooneySequenceMap.PersistentLevel.MooneySequence.AnimationPlayer"));
				UObject* Func = MS->Function(("Play"));
				MS->ProcessEvent(Func); */

				auto Scripoting = FindObject("BP_MooneyScripting_C /Game/Athena/Maps/Test/S7/MooneyMap.MooneyMap.PersistentLevel.BP_MooneyScripting_2");
				std::cout << "BP_MooneyScripting_C: " << Scripoting << '\n';

				if (Scripoting)
				{
					std::cout << "Scripoting Name: " << Scripoting->GetFullName() << '\n';

					Scripoting->ProcessEvent("BeginIceKingEvent");
					// Scripoting->ProcessEvent("SetupCountdown");
				}
			}
			else if (Version == 6.21f) {
				//Butterfly
				UObject* BF = FindObject(("BP_Butterfly_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.BP_Butterfly_4"));
				UObject* Func = BF->Function(("ButterflySequence"));
				BF->ProcessEvent(Func);
				//(TODO) Fix screen going white when cube explodes

				/*
				
					void ButterflyScriptingReady();
					void ButterflyStart();
					void CubeEvent();

				*/			
			}
			else if (Version == 4.5f) {
				//Rocket
				UObject* LR = FindObject(("LevelSequencePlayer /Game/Athena/Maps/Test/Events/Athena_Gameplay_Geode.Athena_Gameplay_Geode.PersistentLevel.LevelSequence_LaunchRocket.AnimationPlayer"));
				UObject* Func = LR->Function(("Play"));

				if (Func)
					LR->ProcessEvent(Func);

				// BP_GeodeScripting_C /Game/Athena/Maps/Test/Events/Athena_Gameplay_Geode.Athena_Gameplay_Geode.PersistentLevel.BP_GeodeScripting_2
			}
		}
	}
}