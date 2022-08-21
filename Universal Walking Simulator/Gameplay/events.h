#pragma once

#include <UE/structs.h>
#include <Gameplay/helper.h>

static UObject* JerkyBPLoader;
static UObject* JerkyLoaderActual;
static UObject* JerkyPlayerInteraction;

namespace EventHelper // credits spooky man
{
	void ApplyGEs()
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

	void BoostUp()
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
		return (Version == 17.50f || Version == 17.30f || Version == 12.61f ||  Version == 12.41f ||  Version == 10.40f || Version == 9.40f || Version == 8.51f || Version == 7.30f || Version == 7.20f || Version == 6.21f || Version == 4.5f);
	}

	void LoadEvents() {
		float Version = std::stof(FN_Version);

		if (HasEvent()) {
			std::cout << ("Loading Event!\n");

			if (Version == 17.50f) {
				auto KiwiLoader = FindObject(("Kiwi_EventScript_C /Kiwi/Levels/Kiwi_P.Kiwi_P.PersistentLevel.Kiwi_EventScript_2"));
				auto AwfulDuckBGA = FindObject(("BGA_Kiwi_AwfulDuck_C /Kiwi/Levels/Kiwi_P.Kiwi_P.PersistentLevel.BGA_Kiwi_AwfulDuck_2"));
				auto AwfulDuckManager = FindObject(("BP_Kiwi_AwfulDuck_Manager_C /Kiwi/Levels/Kiwi_P.Kiwi_P.PersistentLevel.BP_Kiwi_AwfulDuck_Manager_4"));
				if (KiwiLoader)
				{
					static auto Function = KiwiLoader->Function("LoadKiwiAssets");
					KiwiLoader->ProcessEvent(Function);

					/*static auto Ability1 = FindObject("BlueprintGeneratedClass /Kiwi/Gameplay/GameplayAbilities/GA_Kiwi_AwfulDuck_Warning.GA_Kiwi_AwfulDuck_Warning_C");
					static auto Ability2 = FindObject("BlueprintGeneratedClass /Kiwi/Gameplay/GameplayAbilities/GA_Kiwi_AwfulDuck_WarmUpPulse.GA_Kiwi_AwfulDuck_WarmUpPulse_C");
					static auto Ability3 = FindObject("BlueprintGeneratedClass /Kiwi/Gameplay/GameplayAbilities/GA_Kiwi_AwfulDuck.GA_Kiwi_AwfulDuck_C");*/
					
				}
				

			}
			if (Version == 17.30f) {
				static auto BuffetLoaderClass = FindObject(("BlueprintGeneratedClass /Buffet/Gameplay/Blueprints/BP_Buffet_Level_Loader.BP_Buffet_Level_Loader_C"));
				static auto BuffetLoader = Easy::SpawnObject(BuffetLoaderClass, nullptr);
				struct 
				{
					bool Condition;
					int PhaseIndex;
				} Function_params {true, 0};
				 
				static auto Function = BuffetLoader->Function("LoadBuffetLevel");
				BuffetLoader->ProcessEvent(Function, &Function_params);
				
					
				
			}

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

			if (Version == 17.50f) {
				static auto KiwiScripting = FindObject(("BP_Kiwi_Master_Scripting_C /Kiwi/Levels/Kiwi_P.Kiwi_P.PersistentLevel.BP_Kiwi_Master_Scripting_2"));
				static auto AwfulDuckManager = FindObject(("BP_Kiwi_AwfulDuck_Manager_C /Kiwi/Levels/Kiwi_P.Kiwi_P.PersistentLevel.BP_Kiwi_AwfulDuck_Manager_4"));
				

				if (KiwiScripting)
				{
					static auto Function = KiwiScripting->Function("startevent");
					KiwiScripting->ProcessEvent(Function);
					static auto Function2 = AwfulDuckManager->Function("CheckMassAbductionComplete");
					AwfulDuckManager->ProcessEvent(Function2);
					

				}
				

			}
			if (Version == 17.30f) {
				static auto BuffetScripting = FindObject(("BP_Buffet_Master_Scripting_C /Buffet/Levels/Buffet_P.Buffet_P.PersistentLevel.BP_Event_Master_Scripting_2"));

				if (BuffetScripting)
				{
					static auto Function = BuffetScripting->Function("startevent");
					BuffetScripting->ProcessEvent(Function);
				}
			}
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
					EventHelper::ApplyGEs();
					
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

									auto ItemInstances = Inventory::GetItemInstances(Controller);

									for (int i = 0; i < ItemInstances->Num(); i++)
									{
										auto CurrentItemInstance = ItemInstances->At(i);

										if (CurrentItemInstance->IsA(BuildingItemData_Wall) || CurrentItemInstance->IsA(BuildingItemData_Floor) || CurrentItemInstance->IsA(BuildingItemData_Stair_W) || CurrentItemInstance->IsA(BuildingItemData_RoofS))
											continue;

										Inventory::TakeItem(Controller, Inventory::GetItemGuid(CurrentItemInstance), *FFortItemEntry::GetCount(GetItemEntryFromInstance(CurrentItemInstance)), true);
									}
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
				//The End C1 (Crashes)
				UObject* NN = FindObject("LevelSequencePlayer /Game/Athena/Maps/Test/S10/NightNightSequenceMap.NightNightSequenceMap.PersistentLevel.NightNight_3.AnimationPlayer"); // FindObject(("LevelSequencePlayer /Game/Athena/Maps/Test/S10/NightNightSequenceMap.NightNightSequenceMap.PersistentLevel.NightNight.AnimationPlayer"));
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
			}
		}
	}
}
