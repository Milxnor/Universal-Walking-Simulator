#pragma once

#include <UE/structs.h>
#include <Gameplay/helper.h>

static UObject* JerkyBPLoader = nullptr;
static UObject* JerkyLoaderActual = nullptr;
static UObject* JerkyPlayerInteraction = nullptr;

namespace EventHelper
{
	void TeleportPlayersToButterfly()
	{
		static auto scripting = FindObject("BP_IslandScripting_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.BP_IslandScripting3", true);

		if (!scripting)
		{
			std::cout << "[WARNING] Unable to find IslandScripting!\n";
			return;
		}

		UObject* ButterflyBP = *scripting->Member<UObject*>("ButterflyBP");
		auto Locations = ButterflyBP->Member<TArray<FTransform>>("PlayerLocations");

		std::cout << "Num PlayerLocations: " << Locations->Num() << '\n';

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
							Helper::SetActorLocation(NewPawn, Locations->At(i).Translation);
						}
					}
				}
			}
		}

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

	// 5.30 Event hook for spawning the Cube
	bool SpawnCube(UObject* EventComponents, UFunction*, void* Parameters)
	{
		static auto bAlreadySpawned = false;

		if (!bAlreadySpawned)
		{
			auto Cube = FindObject(("CUBE_C /Game/Athena/Maps/Test/Level_CUBE.Level_CUBE.PersistentLevel.CUBE_2"));

			if (Cube) {
				auto Func = Cube->Function(("SpawnCube"));
				Cube->ProcessEvent(Func);
			}

			bAlreadySpawned = true;
		}
		return false;
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

namespace Events {
	inline bool HasEvent() {
		float Version = std::stof(FN_Version);
		return (Version == 12.61f ||  Version == 12.41f ||  Version == 10.40f || Version == 9.40f || Version == 8.51f || Version == 7.30f || Version == 7.20f || Version == 6.21f || Version == 5.30f || Version == 4.5f);
	}

	void LoadEvents() {
		float Version = std::stof(FN_Version);

		if (HasEvent()) {
			std::cout << ("Loading Event!\n");

			if (Version == 12.61f) {
				auto FritterLoader = FindObject(("BP_Fritter_Loader_C /Fritter/Level/FritterLoaderLevel.FritterLoaderLevel.PersistentLevel.BP_Fritter_Loader_0"));
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
					UObject* Func = JL->Function("LoadJerkyLevel");
					bool Condition = true;
					JL->ProcessEvent(Func, &Condition);
				}				
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

				static auto scripting = FindObjectOld("BP_IslandScripting_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.BP_IslandScripting3", true);
				scripting->ProcessEvent("LoadDynamicLevels");

				scripting->ProcessEvent("OnRep_CachedTime");
				scripting->ProcessEvent("TrySetIslandLocation");
			}
			if (Version == 5.30f) {
				//Cube Spawn
				UObject* AEC = FindObject(("BP_Athena_Event_Components_C /Game/Athena/Maps/Streaming/Athena_GameplayActors.Athena_GameplayActors.PersistentLevel.BP_Athena_Event_Components_54"));

				UObject* Func = AEC->Function(("OnRep_CrackProgression"));
				UObject* Func2 = AEC->Function(("OnRep_Corruption"));

				*AEC->Member<float>(("CrackOpacity")) = 0.0f; // Hide the initial crack
				AEC->ProcessEvent(Func);
				*AEC->Member<float>(("Corruption")) = 1.0f; // Show the smaller purple crack
				AEC->ProcessEvent(Func2);

				//Hooked it here because it's not loaded in yet when hooking other functions in hooks.h
				AddHook("Function /Game/Athena/Events/BP_Athena_Event_Components.BP_Athena_Event_Components_C.DisableFinalLightning", EventHelper::SpawnCube);
			}
		}
	}

	void StartEvent() {
		float Version = std::stof(FN_Version);
		if (HasEvent()) {
			std::cout << ("Starting Event!\n");

			if (Version == 12.61f) {
				auto FL = FindObject(("BP_Fritter_Loader_C /Fritter/Level/FritterLoaderLevel.FritterLoaderLevel.PersistentLevel.BP_Fritter_Loader_0"));
				
				if (FL)
				{
					auto Function = FL->Function("startevent");
					FL->ProcessEvent(Function);
				}
				else
					std::cout << "Failed to find Fritter!\n";
			}

			if (Version == 12.41f) {
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
				//The End C1
				UObject* NN = FindObject("BP_NightNight_Scripting_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.BP_NightNight_Scripting_2"); // FindObject(("LevelSequencePlayer /Game/Athena/Maps/Test/S10/NightNightSequenceMap.NightNightSequenceMap.PersistentLevel.NightNight.AnimationPlayer"));
				if (NN)
				{
					UObject* Func = NN->Function(("startevent"));
					if (Func)
						NN->ProcessEvent(Func);
					else
						std::cout << ("Unable to find Night Night Start function!\n");
				}
				else
					std::cout << ("No NightNight!\n");
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
				//(TODO) Teleport back after unvaulting part is done.
			}
			else if (Version == 7.30f) {
				//Marshmello
				UObject* FS = FindObject((".PersistentLevel.FestivusSequence_01_2.AnimationPlayer2"));
				UObject* Func = FS->Function(("Play"));
				FS->ProcessEvent(Func);
				//(TODO) Fix audio
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
			}
			else if (Version == 5.30f) {
				//Cube Spawn
				UObject* AEC = FindObject(("BP_Athena_Event_Components_C /Game/Athena/Maps/Streaming/Athena_GameplayActors.Athena_GameplayActors.PersistentLevel.BP_Athena_Event_Components_54"));
				UObject* Func = AEC->Function(("Final"));
				AEC->ProcessEvent(Func);
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
