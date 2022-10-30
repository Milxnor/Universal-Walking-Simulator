#pragma once

#include <UE/structs.h>
#include <functional>

#include "color.hpp"
#include <Net/server.h>
#include <Gameplay/helper.h>
#include <Net/funcs.h>
#include <Net/nethooks.h>
#include <Gameplay/abilities.h>
#include <Gameplay/events.h>

#include <mutex>
#include "Gameplay/player.h"

#define LOGGING

// HEAVILY INSPIRED BY KEMOS UFUNCTION HOOKING

static bool bStarted = false;
static bool bLogRpcs = false;
static bool bLogProcessEvent = false;

inline void initStuff()
{
	if (!bStarted && bTraveled && bIsReadyToRestart)
	{
		bStarted = true;

		CreateThread(0, 0, Helper::Console::Setup, 0, 0, 0);

		auto world = Helper::GetWorld();
		auto gameState = *world->Member<UObject*>("GameState");

		auto Playlist = FindObject(PlaylistToUse);

		static auto FortGameStateZoneClass = FindObject("Class /Script/FortniteGame.FortGameStateZone");

		if (gameState && gameState->IsA(FortGameStateZoneClass))
		{
			auto AuthGameMode = Helper::GetGameMode();

			auto dbnoEnabledPtr = AuthGameMode->Member<bool>("bDBNOEnabled");

			if (dbnoEnabledPtr)
				*dbnoEnabledPtr = false;

			std::cout << "AA: " << *AuthGameMode->Member<bool>("bStartPlayersAsSpectators") << '\n';

			*AuthGameMode->Member<bool>("bStartPlayersAsSpectators") = false;
			
			if (bAutomaticPawnSpawning) // one of these make the battle bus start when soemone joins
			{
				*gameState->Member<bool>("bPlayerSpawningBlocked_Temporarily") = false;
				*AuthGameMode->Member<bool>("bWorldIsReady") = true; // bitfield moment
			}

			if (AuthGameMode)
			{
				auto PlayerControllerClass = // bIsSTW ? FindObject("Class /Script/FortniteGame.FortPlayerControllerZone") :
					FindObject(("BlueprintGeneratedClass /Game/Athena/Athena_PlayerController.Athena_PlayerController_C"));
				std::cout << ("PlayerControllerClass: ") << PlayerControllerClass << '\n';
				*AuthGameMode->Member<UObject*>(("PlayerControllerClass")) = PlayerControllerClass;

				static auto PawnClass = FindObject(("BlueprintGeneratedClass /Game/Athena/PlayerPawn_Athena.PlayerPawn_Athena_C"));

				*AuthGameMode->Member<UObject*>(("DefaultPawnClass")) = PawnClass;
			}

			auto WillSkipAircraft = gameState->Member<bool>(("bGameModeWillSkipAircraft"));

			/* if (!WillSkipAircraft) // we are in like frontend or some gamestate that is not athena
				return; */

			if (WillSkipAircraft)
				*WillSkipAircraft = true;

			auto bSkipTeamReplication = gameState->Member<bool>("bSkipTeamReplication");

			if (bSkipTeamReplication)
				*bSkipTeamReplication = false;

			// auto FriendlyFireTypeOffset = GetOffset(gameState, "FriendlyFireType");

			EFriendlyFireType* FFT = gameState->Member<EFriendlyFireType>(("FriendlyFireType"));
			//Super scuffed way of checking if its valid.
			if (FFT && __int64(FFT) != __int64(gameState)) {
				*gameState->Member<EFriendlyFireType>(("FriendlyFireType")) = EFriendlyFireType::On;
			}
			else {
				std::cout << "FriendlyFireType is not valid!\n";
			}

			Helper::SetGamePhase(EAthenaGamePhase::None);

			if (AuthGameMode)
			{
				// *AuthGameMode->Member<bool>("bAlwaysDBNO") = true;

				// AuthGameMode->ProcessEvent(AuthGameMode->Function(("StartPlay")), nullptr);

				/* if (std::floor(FnVerDouble) == 3 || FnVerDouble >= 8.0)
				{
					//If This is called on Seasons 4, 6, or 7 then Setting The Playlist Crashes.

					AuthGameMode->ProcessEvent("StartMatch");
				} */

				if (!bIsSTW)
				{
					bIsPlayground = PlaylistToUse == "FortPlaylistAthena /Game/Athena/Playlists/Playground/Playlist_Playground.Playlist_Playground";

					if (FnVerDouble >= 6.10) // WRONG
					{
						auto OnRepPlaylist = gameState->Function(("OnRep_CurrentPlaylistInfo"));

						static auto BasePlaylistOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.PlaylistPropertyArray"), ("BasePlaylist"));
						static auto PlaylistReplicationKeyOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.PlaylistPropertyArray"), ("PlaylistReplicationKey"));

						if (BasePlaylistOffset && OnRepPlaylist && Playlist)
						{
							auto PlaylistInfo = gameState->Member<UScriptStruct>(("CurrentPlaylistInfo"));

							// PlaylistInfo->MemberStruct<UObject*>("BasePlaylist");
							auto BasePlaylist = (UObject**)(__int64(PlaylistInfo) + BasePlaylistOffset);// *gameState->Member<UObject>(("CurrentPlaylistInfo"))->Member<UObject*>(("BasePlaylist"), true);
							auto PlaylistReplicationKey = (int*)(__int64(PlaylistInfo) + PlaylistReplicationKeyOffset);

							if (BasePlaylist)
							{
								*BasePlaylist = Playlist;
								(*PlaylistReplicationKey)++;
								if (Engine_Version <= 422)
									((FFastArraySerializerOL*)PlaylistInfo)->MarkArrayDirty();
								else
									((FFastArraySerializerSE*)PlaylistInfo)->MarkArrayDirty();
								std::cout << ("Set playlist to: ") << Playlist->GetFullName() << '\n';
							}
							else
								std::cout << ("Base Playlist is null!\n");
						}
						else
						{
							std::cout << ("Missing something related to the Playlist!\n");
							std::cout << ("BasePlaylist Offset: ") << BasePlaylistOffset << '\n';
							std::cout << ("OnRepPlaylist: ") << OnRepPlaylist << '\n';
							std::cout << ("Playlist: ") << Playlist << '\n';
						}

						auto MapInfoPTR = gameState->Member<UObject*>("MapInfo");

						if (MapInfoPTR)
						{
							std::cout << "MapInfo: " << *MapInfoPTR << '\n';
						}

						gameState->ProcessEvent(OnRepPlaylist);
					}
					else
					{
						if (Playlist) {
							static auto OnRepPlaylist = gameState->Function(("OnRep_CurrentPlaylistData"));
							*gameState->Member<UObject*>(("CurrentPlaylistData")) = Playlist;

							if (OnRepPlaylist)
								gameState->ProcessEvent(OnRepPlaylist);
						}
						else {
							std::cout << ("Playlist is NULL") << '\n';
						}
					}

					std::cout << "GameSessionName: " << (*AuthGameMode->Member<UObject*>("GameSession"))->GetFullName() << '\n';
				}
			}
			else
			{
				std::cout << dye::yellow(("[WARNING] ")) << ("Failed to find AuthorityGameMode!\n");
			}

			if (FnVerDouble >= 13)
			{
				static auto LastSafeZoneIndexOffset = GetOffset(Playlist, "LastSafeZoneIndex");

				if (LastSafeZoneIndexOffset != -1)
					 *(int*)(__int64(Playlist) + LastSafeZoneIndexOffset) = 0;
			}

			static auto Playlist_MaxPlayersOffset = GetOffset(Playlist, "MaxPlayers");

			// if (Playlist_MaxPlayersOffset != -1)
				// *(*AuthGameMode->Member<UObject*>(("GameSession")))->Member<int>(("MaxPlayers")) = *(int*)(__int64(Playlist) + Playlist_MaxPlayersOffset); // GameState->GetMaxPlaylistPlayers()
			// else
			*(*AuthGameMode->Member<UObject*>(("GameSession")))->Member<int>(("MaxPlayers")) = 100;

			if (Engine_Version != 421)
			{
				auto PlayersLeft = gameState->Member<int>(("PlayersLeft"));

				if (PlayersLeft && *PlayersLeft)
					*PlayersLeft = 0;

				static auto OnRep_PlayersLeft = gameState->Function(("OnRep_PlayersLeft"));

				if (OnRep_PlayersLeft)
					gameState->ProcessEvent(OnRep_PlayersLeft);
			}

			static auto AircraftStartTimeOffset = GetOffset(gameState, "AircraftStartTime");

			if (AircraftStartTimeOffset != -1)
			{
				auto AircraftStartTime = (float*)(__int64(gameState) + AircraftStartTimeOffset);

				if (AircraftStartTime)
				{
					*AircraftStartTime = 99999.0f;
					*gameState->Member<float>(("WarmupCountdownEndTime")) = 99999.0f;
				}
			}
		}

		auto PlayerController = Helper::GetOurPlayerController();

		if (PlayerController)
			*PlayerController->Member<UObject*>("CheatManager") = Easy::SpawnObject(FindObject("Class /Script/Engine.CheatManager"), PlayerController);

		Listen(7777);
		// CreateThread(0, 0, MapLoadThread, 0, 0, 0);

		InitializeNetHooks();

		std::cout << ("Initialized NetHooks!\n");

		if (Engine_Version >= 420 && !bIsSTW) {
			Events::LoadEvents();
			Helper::FixPOIs();
		}
		
		// if (FnVerDouble != 12.61)
		LootingV2::InitializeWeapons(nullptr);

		/*

		// cursed server pawn if you want to see level names or something

		if (FnVerDouble < 7.4)
		{
			static const auto QuickBarsClass = FindObject("Class /Script/FortniteGame.FortQuickBars", true);
			auto QuickBars = PlayerController->Member<UObject*>("QuickBars");

			if (QuickBars)
			{
				*QuickBars = Easy::SpawnActor(QuickBarsClass, FVector(), FRotator());
				Helper::SetOwner(*QuickBars, PlayerController);
			}
		}

		*PlayerController->Member<char>(("bReadyToStartMatch")) = true;
		*PlayerController->Member<char>(("bClientPawnIsLoaded")) = true;
		*PlayerController->Member<char>(("bHasInitiallySpawned")) = true;

		*PlayerController->Member<bool>(("bHasServerFinishedLoading")) = true;
		*PlayerController->Member<bool>(("bHasClientFinishedLoading")) = true;

		auto PlayerState = *PlayerController->Member<UObject*>(("PlayerState"));

		*PlayerState->Member<char>(("bHasStartedPlaying")) = true;
		*PlayerState->Member<char>(("bHasFinishedLoading")) = true;
		*PlayerState->Member<char>(("bIsReadyToContinue")) = true;

		Helper::InitPawn(PlayerController); */

		// GAMEMODE STUFF HERE

		bRestarting = false;
		bIsReadyToRestart = false;

		if (Helper::IsRespawnEnabled())
		{
			// Enable glider redeploy (idk if this works)
			FString GliderRedeployCmd;
			GliderRedeployCmd.Set(L"Athena.EnableParachuteEverywhere 1");
			// Helper::Console::ExecuteConsoleCommand(GliderRedeployCmd);
		}

		/* if (PlaylistToUse.contains("FortPlaylistAthena /Game/Athena/Playlists/Low/Playlist_Low_Squads.Playlist_Low_"))
		{
			float GravityScale = 50;
			auto WorldSettings = *(*world->Member<UObject*>("PersistentLevel"))->Member<UObject*>("WorldSettings");

			std::cout << "WorldSettings: " << WorldSettings << '\n';

			*WorldSettings->Member<float>("WorldGravityZ") = GravityScale;
			*WorldSettings->Member<float>("GlobalGravityZ") = GravityScale;
		} */
	}
}

bool ServerReviveFromDBNOHook(UObject* DownedPawn, UFunction*, void* Parameters)
{
	if (DownedPawn)
	{
		auto DownedController = *DownedPawn->Member<UObject*>(("Controller"));
		*DownedPawn->Member<bool>(("bIsDBNO")) = false;

		static auto OnRep_IsDBNO = DownedPawn->Function("OnRep_IsDBNO");
		DownedPawn->ProcessEvent(OnRep_IsDBNO);

		struct parms { UObject* EventInstigator; }; // AController*

		auto Params = (parms*)Parameters;

		auto Instigator = Params->EventInstigator;

		static auto ClientOnPawnRevived = DownedController->Function("ClientOnPawnRevived");

		if (ClientOnPawnRevived)
			DownedController->ProcessEvent(ClientOnPawnRevived, &Instigator);

		Helper::SetHealth(DownedPawn, 30);
	}

	return false;
}

bool ServerLoadingScreenDroppedHook(UObject* PlayerController, UFunction* Function, void* Parameters)
{
	// auto PlayerState = *PlayerController->Member<UObject*>(("PlayerState"));
	// auto Pawn = *PlayerController->Member<UObject*>(("Pawn"));

	return false;
}

char (__fastcall* ShouldStartZoneOrSomethingO)(__int64 Unused, char Phase, char SafeZoneStartup);

char __fastcall ShouldStartZoneOrSomethingDetour(__int64 Unused, char Phase, char SafeZoneStartup)
{
	std::cout << "ShouldStartZoneOrSomethingDetour!\n";
	std::cout << "Phase: " << (int)Phase << '\n';
	std::cout << "SafeZoneStartup: " << (int)SafeZoneStartup << '\n';

	std::cout << "Returning " << zoneRet << '\n';
	
	if (zoneRet)
	{
		zoneRet = false;
		return true;
	}

	return false;
}

// WE NEED TO COMPLETELY SWITCH OFF OF THIS FUNCTION!
bool ServerUpdatePhysicsParamsHook(UObject* Vehicle, UFunction* Function, void* Parameters) // FortAthenaVehicle
{
	if (Vehicle && Parameters)
	{
		struct parms { __int64 InState; };
		auto Params = (parms*)Parameters;

		static auto TranslationOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.ReplicatedAthenaVehiclePhysicsState"), ("Translation"));
		auto Translation = (FVector*)(__int64(&Params->InState) + TranslationOffset);

		static auto RotationOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.ReplicatedAthenaVehiclePhysicsState"), ("Rotation"));
		auto Rotation = (FQuat*)(__int64(&Params->InState) + RotationOffset);

		static auto LinearVelocityOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.ReplicatedAthenaVehiclePhysicsState"), ("LinearVelocity"));
		auto LinearVelocity = (FVector*)(__int64(&Params->InState) + LinearVelocityOffset);

		static auto AngularVelocityOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.ReplicatedAthenaVehiclePhysicsState"), ("AngularVelocity"));
		auto AngularVelocity = (FVector*)(__int64(&Params->InState) + AngularVelocityOffset);

		if (Translation && Rotation)
		{
			/* std::cout << ("X: ") << Translation->X << '\n';
			std::cout << ("Y: ") << Translation->Y << '\n';
			std::cout << ("Z: ") << Translation->Z << '\n';

			auto rot = Rotation->Rotator();

			// rot.Pitch = 0;

			std::cout << ("Pitch: ") << rot.Pitch << '\n';
			std::cout << ("Yaw: ") << rot.Yaw << '\n';
			std::cout << ("Roll: ") << rot.Roll << '\n'; */

			auto OGRot = Helper::GetActorRotation(Vehicle);

			Helper::SetActorLocation(Vehicle, *Translation);
			// Helper::SetActorLocationAndRotation(Vehicle, *Translation, FRotator{ rot.Pitch, OGRot.Yaw, OGRot.Roll });

			UObject* RootComp = nullptr;
			static auto GetRootCompFunc = Vehicle->Function("K2_GetRootComponent");

			if (GetRootCompFunc)
				Vehicle->ProcessEvent(GetRootCompFunc, &RootComp);

			if (RootComp)
			{
				// SetSimulatePhysics

				static auto SetPhysicsLinearVelocity = RootComp->Function("SetPhysicsLinearVelocity");

				struct {
					FVector                                     NewVel;                                                   // (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
					bool                                        bAddToCurrent;                                            // (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
					FName                                       BoneName;
				} SetPhysicsLinearVelocity_Params{*LinearVelocity, false, FName()};

				if (SetPhysicsLinearVelocity)
					RootComp->ProcessEvent(SetPhysicsLinearVelocity, &SetPhysicsLinearVelocity);

				static auto SetPhysicsAngularVelocity = RootComp->Function("SetPhysicsAngularVelocity");

				struct {
					FVector                                     NewAngVel;                                                // (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
					bool                                               bAddToCurrent;                                            // (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
					FName                                       BoneName;
				} SetPhysicsAngularVelocity_Params{*AngularVelocity, false, FName()};

				if (SetPhysicsLinearVelocity)
					RootComp->ProcessEvent(SetPhysicsAngularVelocity, &SetPhysicsAngularVelocity_Params);
			}

			// Vehicle->ProcessEvent("OnRep_ServerCorrection");
		}
	}

	return false;
}

bool ServerAttemptAircraftJumpHook(UObject* PlayerController, UFunction* Function, void* Parameters)
{
	if (Engine_Version >= 424)
		PlayerController = Helper::GetOwnerOfComponent(PlayerController); // CurrentAircraft

	struct Param{
		FRotator                                    ClientRotation;
	};

	auto Params = (Param*)Parameters;

	if (PlayerController && Params) // PlayerController->IsInAircraft()
	{
		auto world = Helper::GetWorld();
		auto GameState = Helper::GetGameState();

		if (GameState && Helper::IsInAircraft(PlayerController))
		{
			static auto AircraftsOffset = GetOffset(GameState, "Aircrafts");
			auto Aircrafts = (TArray<UObject*>*)(__int64(GameState) + AircraftsOffset);

			if (Aircrafts)
			{
				auto Aircraft = Aircrafts->At(0);

				if (Aircraft)
				{
					if (bClearInventoryOnAircraftJump && FnVerDouble < 19.00)
						ClearInventory(PlayerController); // TODO: Clear on ServerSetInAircraft

					auto ExitLocation = Helper::GetActorLocation(Aircraft);

					auto Pawn = Helper::InitPawn(PlayerController, false, ExitLocation);

					/* static auto CSRfn = PlayerController->Function(("ClientSetRotation"));

					struct {
						FRotator NewRotation;
						bool bResetCamera;
					} CSRparams{ Params->ClientRotation, false };

					if (CSRfn)
						PlayerController->ProcessEvent(CSRfn, &CSRparams); */

					if (Pawn)
					{
						Helper::SetShield(Pawn, bIsLateGame ? 100.f : 0.f);
					}

					if (bIsLateGame && LootingV2::bInitialized) // give random loot
					{
						if (!(bClearInventoryOnAircraftJump && FnVerDouble < 19.00)) // dont clear twice
							ClearInventory(PlayerController);

						std::cout << "A!\n";

						Inventory::GiveAllAmmo(PlayerController, 15, 50, 350, 300, 50);
						Inventory::GiveMats(PlayerController, 500, 500, 500);

						std::cout << "C!\n";

						auto AR = LootingV2::GetRandomItem(ItemType::Weapon);

						while (!AR.Definition || (!AR.Definition->GetFullName().contains("Assault") && !AR.Definition->GetFullName().contains("LMG")))
						{
							AR = LootingV2::GetRandomItem(ItemType::Weapon);
						}

						std::cout << "B!\n";

						auto Shotgun = LootingV2::GetRandomItem(ItemType::Weapon);

						while (!Shotgun.Definition || !Shotgun.Definition->GetFullName().contains("Shotgun"))
						{
							Shotgun = LootingV2::GetRandomItem(ItemType::Weapon);
						}

						std::cout << "D!\n";

						Inventory::CreateAndAddItem(PlayerController, AR.Definition, EFortQuickBars::Primary, 1, 1, true);
						Inventory::CreateAndAddItem(PlayerController, Shotgun.Definition, EFortQuickBars::Primary, 2, 1, true);

						std::cout << "CC!\n";

						std::random_device rd; // obtain a random number from hardware
						std::mt19937 gen(rd()); // seed the generator

						std::uniform_int_distribution<> distr(0, 10);

						int slotForFirstConsumable = 3;
						int slotForSecondConsumable = 4;
						int slotForThirdConsumable = 5;

						if (FnVerDouble < 9)
						{
							if (distr(gen) > 6) // 2 heals
							{
								if (distr(gen) >= 4) // 40/60 sniper or smg
								{
									auto SMG = LootingV2::GetRandomItem(ItemType::Weapon);

									while (!SMG.Definition || IsBadReadPtr(SMG.Definition) || !SMG.Definition->GetFullName().contains("PDW")) // bad
									{
										SMG = LootingV2::GetRandomItem(ItemType::Weapon);
									}

									Inventory::CreateAndAddItem(PlayerController, SMG.Definition, EFortQuickBars::Primary, 3, 1, true);
								}
								else
								{
									auto Sniper = LootingV2::GetRandomItem(ItemType::Weapon);

									while (!Sniper.Definition || IsBadReadPtr(Sniper.Definition) || !Sniper.Definition->GetFullName().contains("Sniper"))
									{
										Sniper = LootingV2::GetRandomItem(ItemType::Weapon);
									}

									Inventory::CreateAndAddItem(PlayerController, Sniper.Definition, EFortQuickBars::Primary, 3, 1, true);
								}

								std::cout << "E!\n";

								slotForFirstConsumable = 4;
								slotForSecondConsumable = 5;
								slotForThirdConsumable = -1;
							}
							else // 1 heal
							{
								{
									auto SMG = LootingV2::GetRandomItem(ItemType::Weapon);

									while (!SMG.Definition || IsBadReadPtr(SMG.Definition) || !SMG.Definition->GetFullName().contains("PDW")) // bad
									{
										SMG = LootingV2::GetRandomItem(ItemType::Weapon);
									}

									Inventory::CreateAndAddItem(PlayerController, SMG.Definition, EFortQuickBars::Primary, 3, 1, true);
								}

								{
									auto Sniper = LootingV2::GetRandomItem(ItemType::Weapon);

									while (!Sniper.Definition || IsBadReadPtr(Sniper.Definition) || !Sniper.Definition->GetFullName().contains("Sniper"))
									{
										Sniper = LootingV2::GetRandomItem(ItemType::Weapon);
									}

									Inventory::CreateAndAddItem(PlayerController, Sniper.Definition, EFortQuickBars::Primary, 4, 1, true);
								}

								std::cout << "G!\n";

								slotForFirstConsumable = 5;
								slotForSecondConsumable = -1;
								slotForThirdConsumable = -1;
							}
						}

						if (slotForFirstConsumable != -1)
						{
							auto Consumable1 = LootingV2::GetRandomItem(ItemType::Consumable);

							/* while (!Consumable1.Definition || !Consumable1.Definition->GetFullName().contains("Shield") || !Consumable1.Definition->GetFullName().contains("Med"))
							{
								Consumable1 = LootingV2::GetRandomItem(ItemType::Consumable);
							} */

							Inventory::GiveItem(PlayerController, Consumable1.Definition, EFortQuickBars::Primary, slotForFirstConsumable, Consumable1.DropCount);
						}

						if (slotForSecondConsumable != -1)
						{
							auto Consumable2 = LootingV2::GetRandomItem(ItemType::Consumable);

							/* while (!Consumable2.Definition || !Consumable2.Definition->GetFullName().contains("Shield") || !Consumable2.Definition->GetFullName().contains("Med"))
							{
								Consumable2 = LootingV2::GetRandomItem(ItemType::Consumable);
							} */

							Inventory::GiveItem(PlayerController, Consumable2.Definition, EFortQuickBars::Primary, slotForSecondConsumable, Consumable2.DropCount);
						}

						if (slotForThirdConsumable != -1)
						{
							auto Consumable3 = LootingV2::GetRandomItem(ItemType::Consumable);

							/* while (!Consumable3.Definition || !Consumable3.Definition->GetFullName().contains("Shield") || !Consumable3.Definition->GetFullName().contains("Med"))
							{
								Consumable3 = LootingV2::GetRandomItem(ItemType::Consumable);
							} */

							Inventory::GiveItem(PlayerController, Consumable3.Definition, EFortQuickBars::Primary, slotForThirdConsumable, Consumable3.DropCount);
						}
					}

					// ASC->RemoveActiveGameplayEffectBySourceEffect(SlurpEffect);
				}
			}
		}
	}

	return false;
}

bool ReadyToStartMatchHook(UObject* Object, UFunction* Function, void* Parameters)
{
	// std::cout << "Ready to start match!\n";
	initStuff();
	return false;
}

void LoadInMatch()
{
	auto PlayerController = Helper::GetOurPlayerController();

	if (PlayerController)
	{
		FString Map;
		Map.Set(GetMapName());

		static auto SwitchLevelFn = PlayerController->Function(("SwitchLevel"));
		PlayerController->ProcessEvent(SwitchLevelFn, &Map);
		// Map.FreeString();
		bTraveled = true;
	}
	else
	{
		std::cout << dye::red(("[ERROR] ")) << ("Unable to find PlayerController!\n");
	}

	/* FString Open;
	Open.Set((L"open " + std::wstring(GetMapName())).c_str());

	Helper::Console::ExecuteConsoleCommand(Open);
	bTraveled = true; */
}

uint8_t GetDeathCause(UObject* PlayerState, FGameplayTagContainer Tags, int* OutWasDBNO = nullptr)
{
	// UFortDeathCauseFromTagMapping
	// FortDeathCauseFromTagMapping

	uint8_t DeathCause = 0;

	std::cout << "Tags: " << Tags.ToStringSimple(true) << '\n';

	if (FnVerDouble >= 6.21)
	{
		static auto FortPlayerStateAthenaDefault = FindObject("FortPlayerStateAthena /Script/FortniteGame.Default__FortPlayerStateAthena");

		struct
		{
			FGameplayTagContainer                       InTags;                                                   // (ConstParm, Parm, OutParm, ReferenceParm, NativeAccessSpecifierPublic)
			bool                                               bWasDBNO;                                                 // (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			uint8_t                                        ReturnValue;                                              // (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		} AFortPlayerStateAthena_ToDeathCause_Params{ Tags, false };

		static auto ToDeathCause = FortPlayerStateAthenaDefault->Function("ToDeathCause");

		if (ToDeathCause)
			FortPlayerStateAthenaDefault->ProcessEvent(ToDeathCause, &AFortPlayerStateAthena_ToDeathCause_Params);

		DeathCause = AFortPlayerStateAthena_ToDeathCause_Params.ReturnValue;
	}
	else
	{
		static auto DeathCauseEnum = FindObject("Enum /Script/FortniteGame.EDeathCause");

		for (int i = 0; i < Tags.GameplayTags.Num(); i++) // SKUNK
		{
			auto& Tag = Tags.GameplayTags.At(i);

			if (Tag.TagName.ComparisonIndex)
			{
				auto TagNameStr = Tag.TagName.ToString(); // tbh we should compre fnames instead

				if (TagNameStr.contains("Gameplay.Damage.Environment.Falling"))
					return GetEnumValue(DeathCauseEnum, "FallDamage");

				else if (TagNameStr.contains("Weapon.Ranged.Shotgun"))
					return GetEnumValue(DeathCauseEnum, "Shotgun");

				else if (TagNameStr.contains("weapon.ranged.heavy.rocket_launcher"))
					return GetEnumValue(DeathCauseEnum, "RocketLauncher");

				else if (TagNameStr.contains("weapon.ranged.assault."))
					return GetEnumValue(DeathCauseEnum, "Rifle");

				else if (TagNameStr.contains("DeathCause.LoggedOut"))
					return GetEnumValue(DeathCauseEnum, "LoggedOut");

				else if (TagNameStr.contains("Weapon.Ranged.SMG"))
					return GetEnumValue(DeathCauseEnum, "SMG");

				else if (TagNameStr.contains("weapon.ranged.sniper."))
					return GetEnumValue(DeathCauseEnum, "Sniper");

				else if (TagNameStr.contains("Weapon.Ranged.Pistol."))
					return GetEnumValue(DeathCauseEnum, "Pistol");

				else if (TagNameStr.contains("Weapon.Ranged.Grenade.Gas"))
					return GetEnumValue(DeathCauseEnum, "GasGrenade");

				else if (TagNameStr.contains("weapon.ranged.heavy.grenade_launcher"))
					return GetEnumValue(DeathCauseEnum, "GrenadeLauncher");

				else if (TagNameStr.contains("Weapon.Ranged.Minigun"))
					return GetEnumValue(DeathCauseEnum, "Minigun");

				else if (TagNameStr.contains("trap."))
					return GetEnumValue(DeathCauseEnum, "Trap");

				else if (TagNameStr.contains("Gameplay.Damage.TeamSwitchSuicide"))
					return GetEnumValue(DeathCauseEnum, "TeamSwitchSuicide");
			}
		}
	}

	if (OutWasDBNO)
	{

	}

	return DeathCause;
}

void RequestExitWithStatusHook(bool Force, uint8_t ReturnCode)
{
	std::cout << std::format("Force: {} ReturnCode: {}", Force, std::to_string((int)ReturnCode));
	return;
}

void Restart()
{
	bStarted = false;
	bListening = false;
	bTraveled = false;
	bRestarting = true;
	Carmine::SpawnedCarmine = false;
	AmountOfRestarts++;
	serverStatus = EServerStatus::Restarting;

	std::string KickReason = "Server is restarting!";
	std::wstring wstr = std::wstring(KickReason.begin(), KickReason.end());
	FString Reason;
	Reason.Set(wstr.c_str());

	auto afdau = [&](UObject* Controller) {
		static auto ClientReturnToMainMenu = Controller->Function("ClientReturnToMainMenu");

		if (ClientReturnToMainMenu)
			Controller->ProcessEvent(ClientReturnToMainMenu, &Reason);
	};

	Helper::LoopConnections(afdau, true);

	DisableNetHooks();

	if (BeaconHost)
		Helper::DestroyActor(BeaconHost);

	ExistingBuildings.clear();

	LoadInMatch();

	bIsReadyToRestart = true;

	Teams::NextTeamIndex = Teams::StartingTeamIndex;
}

DWORD WINAPI AutoRestartThread(LPVOID)
{
	Sleep(RestartSeconds * 1000); // the issue why we cant auto restart is because destroying the beacon actor kicks all players

	Restart();

	return 0;
}

inline bool ClientOnPawnDiedHook(UObject* DeadPC, UFunction* Function, void* Parameters)
{
	if (DeadPC && Parameters)
	{
		struct parms { __int64 DeathReport; };

		auto Params = (parms*)Parameters;

		auto DeadPawn = Helper::GetPawnFromController(DeadPC);
		auto DeadPlayerState = Helper::GetPlayerStateFromController(DeadPC);
		auto DeadController = Helper::GetControllerFromPawn(DeadPawn);
		auto GameState = Helper::GetGameState();

		static auto KillerPawnOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortPlayerDeathReport"), ("KillerPawn"));
		static auto KillerPlayerStateOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortPlayerDeathReport"), ("KillerPlayerState"));

		auto KillerPawn = *(UObject**)(__int64(&Params->DeathReport) + KillerPawnOffset);
		auto KillerPlayerState = *(UObject**)(__int64(&Params->DeathReport) + KillerPlayerStateOffset);

		UObject* KillerController = nullptr;

		if (KillerPawn)
			KillerController = Helper::GetControllerFromPawn(KillerPawn);
			
		static auto DeathLocationOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.DeathInfo"), ("DeathLocation"));	
		auto DeathInfoOffset = GetOffset(DeadPlayerState, "DeathInfo");

		if (DeathInfoOffset == -1) // iirc if u rejoin and die this is invalid idfk why
			return false;

		auto DeathInfo = (__int64*)(__int64(DeadPlayerState) + DeathInfoOffset);

		auto DeathLocation = Helper::GetActorLocation(DeadPawn); // *(FVector*)(__int64(&*DeathInfo) + DeathLocationOffset);

		static auto TagsOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortPlayerDeathReport"), ("Tags"));
		FGameplayTagContainer* Tags = (FGameplayTagContainer*)(__int64(&Params->DeathReport) + TagsOffset);
		auto DeathCause = Tags ? GetDeathCause(DeadPlayerState, *Tags) : 0;

		// std::cout << "bIsDBNO: " << DeadPawn->Member< bitfield>("bIsDBNO")->idk1 << '\n';
		// std::cout << "bWasDBNOOnDeath: " << DeadPawn->Member< bitfield>("bWasDBNOOnDeath")->idk2 << '\n';
		
		if (Helper::IsRespawnEnabled()) // || bIsTrickshotting ? !KillerController : false) // basically, if trickshotting, respawn player if they dont die to a player
		{
			if (bExperimentalRespawning)
				DeadController->ProcessEvent("RespawnPlayer");
			else
				Player::RespawnPlayer(DeadPC);
		}
		else
		{
			static auto PlayersLeftOffset = GetOffset(GameState, "PlayersLeft");
			auto PlayersLeft = (int*)(__int64(GameState) + PlayersLeftOffset);

			(*PlayersLeft)--;

			std::cout << "PlayersLeft: " << *PlayersLeft << '\n';
			std::cout << "KillerController: " << KillerController << '\n';
			//std::cout << "TeamAlivePlayers: " << *Helper::GetGameMode()->Member<int>("TeamAlivePlayers") << '\n';

			std::unordered_set<int> AliveTeamIndexes;

			auto laivefoap = [&](UObject* Controller) {
				if (Controller != DeadController)
					AliveTeamIndexes.emplace(*Teams::GetTeamIndex(Helper::GetPlayerStateFromController(Controller)));
			};

			Helper::LoopAlivePlayers(laivefoap);

			bool bShouldPlayWinEffects = Helper::HasAircraftStarted() && AliveTeamIndexes.size() <= 1; // *PlayersLeft <= Teams::GetMaxPlayersPerTeam();

			if (bShouldPlayWinEffects)
			{
				static auto DamageCauserOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortPlayerDeathReport"), ("DamageCauser"));

				auto DamageCauser = *(UObject**)(__int64(&Params->DeathReport) + DamageCauserOffset);

				// if (DamageCauser)
					// std::cout << "DamageCauser Name: " << DamageCauser->GetFullName() << '\n';

				UObject* FinishingWeaponDefinition = nullptr;

				if (DamageCauser)
				{
					static auto ProjectileClass = FindObject("Class /Script/FortniteGame.FortProjectileBase");
					static auto FortWeaponClass = FindObject("Class /Script/FortniteGame.FortWeapon");

					if (DamageCauser->IsA(ProjectileClass))
						FinishingWeaponDefinition = Helper::GetWeaponData(Helper::GetOwner(DamageCauser));
					else if (DamageCauser->IsA(FortWeaponClass))
						FinishingWeaponDefinition = Helper::GetWeaponData(DamageCauser); // *(*KillerPawn->Member<UObject*>("CurrentWeapon"))->Member<UObject*>("WeaponData");
				}

				static auto ClientNotifyWon = FindObject("Function /Script/FortniteGame.FortPlayerControllerAthena.ClientNotifyTeamWon");
				static auto PlayWinEffects = FindObject("Function /Script/FortniteGame.FortPlayerControllerAthena.PlayWinEffects");

				// Give Class FortniteGame.FortVictoryCrownAccountItemDefinition?

				struct
				{
					UObject* FinisherPawn;          // APawn                                   // (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
					UObject* FinishingWeapon; // UFortWeaponItemDefinition                                          // (ConstParm, Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
					EDeathCause                                        DeathCause;                                               // (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
				} AFortPlayerControllerAthena_ClientNotifyWon_Params{ KillerPawn, FinishingWeaponDefinition, (EDeathCause)DeathCause };

				auto lmbda = [&](UObject* Controller) {
					if (Controller != DeadController)
					{
						Controller->ProcessEvent(ClientNotifyWon, &AFortPlayerControllerAthena_ClientNotifyWon_Params);

						// if (Engine_Version >= 424)
							// Controller->ProcessEvent(PlayWinEffects, &AFortPlayerControllerAthena_ClientNotifyWon_Params);
					}
				};

				Helper::LoopAlivePlayers(lmbda);

				if (KillerController)
				{
					// KillerController->ProcessEvent(ClientNotifyWon, &AFortPlayerControllerAthena_ClientNotifyWon_Params);

					if (bAutoRestart)
						CreateThread(0, 0, AutoRestartThread, 0, 0, 0);

					auto WinningPlayerState = GameState->Member<UObject*>("WinningPlayerState");

					if (WinningPlayerState)
					{
						*WinningPlayerState = KillerPlayerState;
						*GameState->Member<int>("WinningTeam") = *Teams::GetTeamIndex(KillerPlayerState);

						struct FFortWinnerPlayerData { int PlayerId; };

						GameState->Member<TArray<FFortWinnerPlayerData>>("WinningPlayerList")->Add(FFortWinnerPlayerData{ *KillerPlayerState->Member<int>("WorldPlayerId") });

						GameState->ProcessEvent("OnRep_WinningPlayerState");
						GameState->ProcessEvent("OnRep_WinningTeam");
					}
				}
			}

			// DeadPC->ClientSendMatchStatsForPlayer(DeadPC->GetMatchReport()->MatchStats);

			auto ItemInstances = Inventory::GetItemInstances(DeadPC);

			if (ItemInstances)
			{
				for (int i = 6; i < ItemInstances->Num(); i++)
				{
					auto ItemInstance = ItemInstances->At(i);

					if (ItemInstance)
					{
						struct
						{
							FGuid                                       ItemGuid;                                                 // (Parm, ZeroConstructor, IsPlainOldData)
							int                                                Count;                                                    // (Parm, ZeroConstructor, IsPlainOldData)
						} AFortPlayerController_ServerAttemptInventoryDrop_Params{ Inventory::GetItemGuid(ItemInstance), *FFortItemEntry::GetCount(GetItemEntryFromInstance<__int64>(ItemInstance)) };

						// ServerAttemptInventoryDropHook(DeadPC, nullptr, &AFortPlayerController_ServerAttemptInventoryDrop_Params);

						auto Params = &AFortPlayerController_ServerAttemptInventoryDrop_Params;

						auto Definition = Inventory::TakeItem(DeadPC, Params->ItemGuid, Params->Count, true);
						auto Pawn = Helper::GetPawnFromController(DeadPC);

						static auto bDropOnDeathOffset = GetOffset(Definition, "bDropOnDeath");
						static auto bDropOnDeathBI = GetBitIndex(GetProperty(Definition, "bDropOnDeath"));

						int LoadedAmmo = 0;

						// if (readd((uint8_t*)(__int64(Actor) + bDropOnDeathOffset), bDropOnDeathBI)) // TODO: Test
						// if (IsDroppable(DeadController, Definition))
						{
							if (Pawn && Definition)
							{
								auto loc = Helper::GetActorLocation(Pawn);

								auto Pickup = Helper::SummonPickup(Pawn, Definition, DeathLocation, EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::PlayerElimination,
									Params->Count, true, false, LoadedAmmo);
							}
						}
					}
				}
			}

			if (Engine_Version >= 423) // wrong
			{
				auto Chip = nullptr; // Helper::SpawnChip(DeadPC, DeathLocation);

				if (Chip)
				{
					// 0x0018
					struct FFortResurrectionData
					{
						bool                                               bResurrectionChipAvailable;                               // 0x0000(0x0001) (ZeroConstructor, IsPlainOldData)
						unsigned char                                      UnknownData00[0x3];                                       // 0x0001(0x0003) MISSED OFFSET
						float                                              ResurrectionExpirationTime;                               // 0x0004(0x0004) (ZeroConstructor, IsPlainOldData)
						float                                              ResurrectionExpirationLength;                             // 0x0008(0x0004) (ZeroConstructor, IsPlainOldData)
						struct FVector                                     WorldLocation;                                            // 0x000C(0x000C) (ZeroConstructor, IsPlainOldData)
					};

					auto FortResurrectionData = DeadPlayerState->Member<FFortResurrectionData>(("ResurrectionChipAvailable"));

					if (FortResurrectionData)
					{
						std::cout << ("FortResurrectionData valid!\n");
						std::cout << ("FortResurrectionData Location X: ") << FortResurrectionData->WorldLocation.X << '\n';
					}
					else
						std::cout << ("No FortResurrectionData!\n");

					static auto ServerTimeForResurrectOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortPlayerDeathReport"), ("ServerTimeForResurrect"));

					auto ServerTimeForResurrect = *(float*)(__int64(&Params->DeathReport) + ServerTimeForResurrectOffset);

					std::cout << "ServerTimeForResurrect: " << ServerTimeForResurrect << '\n';

					auto ResurrectionData = FFortResurrectionData{};
					ResurrectionData.bResurrectionChipAvailable = true;
					ResurrectionData.ResurrectionExpirationLength = ServerTimeForResurrect;
					ResurrectionData.ResurrectionExpirationTime = ServerTimeForResurrect;
					ResurrectionData.WorldLocation = Helper::GetActorLocation(Chip);

					if (FortResurrectionData)
						*FortResurrectionData = ResurrectionData;

					std::cout << ("Spawned Chip!\n");
				}
			}
		}

		{
			// if (KillerPlayerState)
			{
				static auto DeathCauseOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.DeathInfo"), ("DeathCause"));
				static auto FinisherOrDownerOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.DeathInfo"), ("FinisherOrDowner"));
				static auto bDBNOOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.DeathInfo"), ("bDBNO"));
				static auto DistanceOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.DeathInfo"), ("Distance"));
				static auto DeathCauseEnum = FindObject("Enum /Script/FortniteGame.EDeathCause");

				// /Script/FortniteGame.FortPlayerStateAthena.OnRep_TeamKillScore

				*(uint8_t*)(__int64(&*DeathInfo) + DeathCauseOffset) = DeathCause;
				*(UObject**)(__int64(&*DeathInfo) + FinisherOrDownerOffset) = KillerPlayerState ? KillerPlayerState : DeadPlayerState;
				*(bool*)(__int64(&*DeathInfo) + bDBNOOffset) = false;

				static auto FallDamageEnumValue = GetEnumValue(DeathCauseEnum, "FallDamage");

				if (DeathCause != FallDamageEnumValue)
				{
					*(float*)(__int64(&*DeathInfo) + DistanceOffset) = KillerPawn ? Helper::GetDistanceTo(KillerPawn, DeadPawn) : 0.f;
				}
				else
				{
					static auto LastFallDistanceOffset = GetOffset(DeadPawn, "LastFallDistance");

					if (LastFallDistanceOffset != -1)
						*(float*)(__int64(&*DeathInfo) + DistanceOffset) = *(float*)(__int64(DeadPawn) + LastFallDistanceOffset);
				}

				std::cout << "DeathCause: " << (int)DeathCause << '\n';

				static auto OnRep_DeathInfo = DeadPlayerState->Function(("OnRep_DeathInfo"));

				if (OnRep_DeathInfo)
					DeadPlayerState->ProcessEvent(OnRep_DeathInfo); // sopmetimes crashes

				if (bSiphonEnabled && KillerPawn && KillerPawn != DeadPawn)
				{
					float Health = Helper::GetHealth(KillerPawn);
					float Shield = Helper::GetShield(KillerPawn);

					int MaxHealth = 100;
					int MaxShield = 100;
					int AmountGiven = 0;

					if ((MaxHealth - Health) > 0)
					{ 
						int AmountToGive = MaxHealth - Health >= 50 ? 50 : MaxHealth - Health;
						Helper::SetHealth(KillerPawn, Health + AmountToGive);
						AmountGiven += AmountToGive;
						std::cout << "Health To Give: " << AmountToGive << '\n';
					}

					std::cout << "Before Shield AmountGiven: " << AmountGiven << '\n';
					std::cout << "MaxShield - Shield: " << MaxShield - Shield << '\n';

					if ((MaxShield - Shield) > 0 && AmountGiven < 50)
					{
						int AmountToGive = MaxShield - Shield >= 50 ? 50 : MaxShield - Shield;
						AmountToGive -= AmountGiven;

						if (AmountToGive > 0)
						{
							Helper::SetShield(KillerPawn, Shield + AmountToGive);
							AmountGiven += AmountToGive;
							std::cout << "Shield to Give: " << AmountToGive << '\n';
						}
					}
				}
			}
		}

		if (KillerPlayerState && KillerPlayerState != DeadPlayerState) // make sure if they didnt die of like falling or rejoining they dont get their kill
		{
			if (KillerController)
			{
				static auto ClientReceiveKillNotification = KillerController->Function(("ClientReceiveKillNotification"));

				struct {
					// Both playerstates
					UObject* Killer;
					UObject* Killed;
				} ClientReceiveKillNotification_Params{ KillerPlayerState, DeadPlayerState };

				if (ClientReceiveKillNotification)
					KillerController->ProcessEvent(ClientReceiveKillNotification, &ClientReceiveKillNotification_Params);
			}

			static auto ClientReportKill = KillerPlayerState->Function(("ClientReportKill")); // ClientReportDBNO
			struct { UObject* PlayerState; }ClientReportKill_Params{ DeadPlayerState };

			if (ClientReportKill)
				KillerPlayerState->ProcessEvent(ClientReportKill, &ClientReportKill_Params);

			static auto KillScoreOffset = GetOffset(KillerPlayerState, "KillScore");

			(*(int*)(__int64(KillerPlayerState) + KillScoreOffset))++;

			if (Engine_Version >= 423) // idgaf wrong
				(*KillerPlayerState->Member<int>(("TeamKillScore")))++;

			static auto OnRep_Kills = KillerPlayerState->Function(("OnRep_Kills"));

			if (OnRep_Kills)
				KillerPlayerState->ProcessEvent(OnRep_Kills);
		}
		
		if (!Helper::IsRespawnEnabled())
			ProcessEventO(DeadPC, Function, Parameters);

		if (FnVerDouble < 6 && false)
		{
			// void(__fastcall * BeginSpectating)(UObject * OurPlayerState, UObject * PlayerStateToSpectate); // some versions it returns how many spectators are spectating the player (including you)

			// BeginSpectating = decltype(BeginSpectating)(FindPattern("40 53 55 48 83 EC 48 48 8B DA 48 8B E9 48 3B 91 ? ? ? ? 75 40 80 3D ? ? ? ? ? 0F 82 ? ? ? ? 48 8B 05 ? ? ? ? 4C 8D 44 24 ? 48 89 44 24 ? 41 B9"));

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

							auto aaController = *Connection->Member<UObject*>(("PlayerController"));

							if (aaController)
							{
								auto aaPlayerState = *aaController->Member<UObject*>(("PlayerState"));
								auto aaPawn = Helper::GetPawnFromController(aaController);

								if (aaPawn)
								{
									static auto IsActorBeingDestroyed = aaPawn->Function(("IsActorBeingDestroyed"));

									bool bIsActorBeingDestroyed = true;

									if (IsActorBeingDestroyed)
										aaPawn->ProcessEvent(IsActorBeingDestroyed, &bIsActorBeingDestroyed);

									if (aaPlayerState && aaPlayerState != DeadPlayerState && aaPawn) // && !bIsActorBeingDestroyed)
									{
										// BeginSpectating(DeadPlayerState, KillerPlayerState ? KillerPlayerState : aaPlayerState);
										std::cout << "wtf!\n";
										break;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return true;
}

inline bool ServerAttemptExitVehicleHook(UObject* Controller, UFunction* Function, void* Parameters)
{
	auto Pawn = Helper::GetPawnFromController(Controller);

	if (Pawn)
	{
		UObject* Vehicle = Helper::GetVehicle(Pawn);

		if (Vehicle)
		{
			Helper::SetLocalRole(Pawn, ENetRole::ROLE_Authority);
			Helper::SetLocalRole(Vehicle, ENetRole::ROLE_Authority);

			if (bAreVehicleWeaponsEnabled)
			{
				UObject* VehicleWeaponDef = Helper::GetWeaponForVehicle(Helper::GetVehicle(Pawn));

				std::cout << "VehicleWeaponDef: " << VehicleWeaponDef << '\n';

				if (VehicleWeaponDef)
				{
					auto VehicleWeaponInstance = Inventory::FindItemInInventory(Controller, VehicleWeaponDef);

					if (VehicleWeaponInstance)
					{
						Inventory::RemoveItem(Controller, Inventory::GetItemGuid(VehicleWeaponInstance));
						Inventory::EquipInventoryItem(Controller, Inventory::GetItemGuid(Inventory::FindItemInInventory(Controller, Helper::GetPickaxeDef(Controller)))); // crazy
					}
				}
			}
		}
	}

	return false;
}

__int64 IsNoMCPDetour()
{
	return 1;
}

__int64 __fastcall FReplicationGraphDebugInfo_LogDetour(__int64 a1, __int64 a2)
{
	std::cout << "logg!\n";
	return 0;
}

UObject* __fastcall CreateReplicationDriverDetour(__int64 NetDriver, __int64 URL, __int64 World)
{
	static bool bbb = false;

	if (!bbb) // needed idk
	{
		std::cout << "Blocking creation!\n";
		bbb = true;
		return nullptr;
	}

	// idk why bu the replciationdriverclas iusnt set and if i set it ig it gvets unset by initreplicationdriverclass
	std::cout << "Skidad!\n";
	static auto ReplicationDriverClass = FindObject(("Class /Script/FortniteGame.FortReplicationGraph"));
	return Easy::SpawnObject(ReplicationDriverClass, Helper::GetTransientPackage());
}

void (__fastcall* LifeO)(_int64 a1, const void* a2, int a3);

void __fastcall LifeDetour(__int64 a1, const void* a2, int a3)
{
	std::cout << std::format("off: 0x{:x}\n", (uintptr_t)_ReturnAddress() - (uintptr_t)GetModuleHandleW(0));
	return LifeO(a1, a2, a3);
}

__int64(__fastcall* getnetmodeO)(UObject* World);

__int64 __fastcall getnetmodedetour(UObject* World)
{
	return 1; // ded
}

inline bool ServerPlayEmoteItemHook(UObject* Controller, UFunction* Function, void* Parameters)
{
	auto Pawn = Helper::GetPawnFromController(Controller);

	struct SPEIParams { UObject* EmoteAsset; }; // UFortMontageItemDefinitionBase
	auto EmoteParams = (SPEIParams*)Parameters;

	auto EmoteAsset = EmoteParams->EmoteAsset;

	std::cout << "aaaaaaa!\n";

	if (Controller /* && !Controller->IsInAircraft() */ && Pawn && EmoteAsset)
	{
		struct {
			TEnumAsByte<EFortCustomBodyType> BodyType;
			TEnumAsByte<EFortCustomGender> Gender;
			UObject* AnimMontage; // UAnimMontage
		} GAHRParams{ EFortCustomBodyType::All, EFortCustomGender::Both }; // (CurrentPawn->CharacterBodyType, CurrentPawn->CharacterGender)
		static auto fn = EmoteAsset->Function(("GetAnimationHardReference"));

		auto EmoteAssetName = EmoteAsset->GetFullName();

		std::cout << "EmoteAsset: " << EmoteAsset->GetFullName() << '\n';

		/* if (EmoteAssetName.contains("Spray"))
		{
			struct
			{
				UObject* SpawningPC;      // AFortPlayerController                                          // (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
				UObject* InSprayAsset;            // UAthenaSprayItemDefinition                                  // (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			}AFortSprayDecalInstance_SetSprayParameters_Params{ Controller, EmoteAsset };
			struct FFortSprayDecalRepPayload
			{
				UObject* SprayAsset;             // UAthenaSprayItemDefinition                                   // 0x0000(0x0008) (BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
				FName                                       BannerName;                                               // 0x0008(0x0008) (BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
				FName                                       BannerColor;                                              // 0x0010(0x0008) (BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
				int                                                SavedStatValue;                                           // 0x0018(0x0004) (BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
				unsigned char                                      UnknownData00[0x4];                                       // 0x001C(0x0004) MISSED OFFSET
			};
			static auto SprayDecalInstanceClass = FindObject("BlueprintGeneratedClass /Game/Athena/Cosmetics/Sprays/BP_SprayDecal.BP_SprayDecal_C");
			auto DecalInstance = Easy::SpawnActor(SprayDecalInstanceClass, Helper::GetActorLocation(Pawn), FRotator());
			std::cout << "Instance: " << DecalInstance << '\n';
			static auto SetSprayParameters = DecalInstance->Function("SetSprayParameters");
			if (SetSprayParameters)
			{
				DecalInstance->ProcessEvent(SetSprayParameters, &AFortSprayDecalInstance_SetSprayParameters_Params);
				DecalInstance->ProcessEvent("OnRep_SprayInfo");
				std::cout << "dababy!\n";
				Controller->Member<TArray<UObject*>>("ActiveSprayInstances")->Add(DecalInstance);
			}
			else
				std::cout << "No SetSprayParameters!\n";
			return false;
		}
		// 	class AActor* SpawnToyInstance(class UClass* ToyClass, struct FTransform SpawnPosition);
		//  SoftClassProperty FortniteGame.AthenaToyItemDefinition.ToyActorClass
		else */ if (EmoteAssetName.contains("Toy"))
		{
			auto ToyActorClass = EmoteAsset->Member<TSoftClassPtr>("ToyActorClass");

			std::cout << "BRUH: " << ToyActorClass->ObjectID.AssetPathName.ToString() << '\n';

			static auto BPC = FindObject("Class /Script/Engine.BlueprintGeneratedClass");

			auto ToyClass = StaticLoadObject(BPC, nullptr, ToyActorClass->ObjectID.AssetPathName.ToString()); // FindObject(ToyActorClass->ObjectID.AssetPathName.ToString());

			if (ToyClass)
			{
				/* auto Toy = Easy::SpawnActor(ToyClass, Helper::GetActorLocation(Pawn));
				std::cout << "Toy: " << Toy->GetFullName() << '\n';
				static auto InitializeToyInstance = Toy->Function("InitializeToyInstance");
				struct { UObject* OwningPC; int32_t NumTimesSummoned; } paafiq23{ Controller, 1 };
				if (InitializeToyInstance)
					Toy->ProcessEvent(InitializeToyInstance, &paafiq23); */

					// bool idk = true;
					// Toy->ProcessEvent("OnReplicatedVelocityStartOrStop", &idk);

					// ^ SEMI WORKING CODE (Removed because they never disappear), below desnt do anything

					/* FTransform transform;
					transform.Scale3D = { 1, 1, 1 };
					transform.Translation = Helper::GetActorLocation(Pawn);
					transform.Rotation = {};
					struct
					{
						UObject* ToyClass;      // UClass                                            // (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic)
						FTransform                                  SpawnPosition;                                            // (Parm, IsPlainOldData, NoDestructor, NativeAccessSpecifierPublic)
						UObject* ReturnValue;   // AActor                                            // (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
					} AFortPlayerController_SpawnToyInstance_Params{ ToyClass, transform };
					static auto SpawnToyInstance = Controller->Function("SpawnToyInstance");
					// InitializeToyInstance
					if (SpawnToyInstance)
						Controller->ProcessEvent(SpawnToyInstance, &AFortPlayerController_SpawnToyInstance_Params); */

						// *AFortPlayerController_SpawnToyInstance_Params.ReturnValue->Member<UObject*>("OwningPawn") = Pawn;
			}
			else
				std::cout << "Unable to find ToyClass!\n";
		}

		if (fn && true)
		{
			EmoteAsset->ProcessEvent(fn, &GAHRParams);
			auto Montage = GAHRParams.AnimMontage;

			std::cout << ("Playing Montage: ") << Montage->GetFullName() << '\n';

			/* static float (*PlayMontageReplicated)(UObject * FortPawn, UObject * Montage, float idk, __int64 idkagain);
			PlayMontageReplicated = decltype(PlayMontageReplicated)(Pawn->VFTable[0xF0]);
			std::cout << "ugh: " << PlayMontageReplicated(Pawn, Montage, 0, 0) << '\n'; */

			using SpecType = TArray<FGameplayAbilitySpec<FGameplayAbilityActivationInfo, 0x50>>; // 7.40

			static auto FN = FindObject("Function /Script/FortniteGame.FortAbilityTask_PlayMontageWaitTarget.PlayMontageWaitTarget");
			static auto Def = FindObject("FortAbilityTask_PlayMontageWaitTarget /Script/FortniteGame.Default__FortAbilityTask_PlayMontageWaitTarget");
			static auto EmoteClass = FindObject(("BlueprintGeneratedClass /Game/Abilities/Emotes/GAB_Emote_Generic.GAB_Emote_Generic_C"));

			auto AnimInstance = Helper::GetAnimInstance(Pawn);

			static auto Montage_Play = AnimInstance->Function("Montage_Play");

			struct {
				UObject* MontageToPlay;                                            // (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
				float                                              InPlayRate;                                               // (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
				EMontagePlayReturnType                             ReturnValueType;                                          // (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
				float                                              InTimeToStartMontageAt;                                   // (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
				bool                                               bStopAllMontages;                                         // (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
				float                                              ReturnValue;
			} Montage_Play_Params{ Montage, 1.0f, EMontagePlayReturnType::Duration, 0.f, true };

			if (Montage_Play)
				AnimInstance->ProcessEvent(Montage_Play, &Montage_Play_Params);

			float Dura = Montage_Play_Params.ReturnValue;

			auto ASC = Helper::GetAbilitySystemComponent(Pawn);

			struct FFortCharacterPartsRepMontageInfo
			{
				TArray<__int64>       CharPartMontages;                                         // 0x0000(0x0010) (ZeroConstructor, NativeAccessSpecifierPublic)
				UObject* PawnMontage;
			};

			static auto RepCharPartAnimMontageInfoOffset = GetOffset(Pawn, "RepCharPartAnimMontageInfo");

			if (RepCharPartAnimMontageInfoOffset != -1)
				((FFortCharacterPartsRepMontageInfo*)(__int64(Pawn) + RepCharPartAnimMontageInfoOffset))->PawnMontage = Montage;

			static auto LocalAnimMontageInfoOffset = GetOffset(ASC, "LocalAnimMontageInfo");
			*(UObject**)(__int64(ASC) + LocalAnimMontageInfoOffset) = Montage;

			static auto OnRep_CharPartAnimMontageInfo = Pawn->Function("OnRep_CharPartAnimMontageInfo");

			if (OnRep_CharPartAnimMontageInfo)
				Pawn->ProcessEvent(OnRep_CharPartAnimMontageInfo);

			static auto OnRep_ReplicatedAnimMontage = Pawn->Function("OnRep_ReplicatedAnimMontage");

			if (OnRep_ReplicatedAnimMontage)
				Pawn->ProcessEvent(OnRep_ReplicatedAnimMontage);

			static auto OnRep_RepAnimMontageStartSection = Pawn->Function("OnRep_RepAnimMontageStartSection");

			if (OnRep_RepAnimMontageStartSection)
				Pawn->ProcessEvent(OnRep_RepAnimMontageStartSection);

			/*
			auto AbilitySystemComponent = *Pawn->CachedMember<UObject*>(("AbilitySystemComponent"));
			auto sog = FindPattern("48 89 5C 24 ? 55 56 57 48 8B EC 48 83 EC 70 49 8B 40 10 49 8B D8 48 8B FA 48 8B F1 80 B8 ? ? ? ? ? 0F 84 ? ? ? ? 80 B8 ? ? ? ? ? 0F");
			// unsigned int* (__fastcall* GiveAbilityAndActivateOnce)(UObject* ASC, SpecType* Spec, void* GameplayEventData) = decltype(GiveAbilityAndActivateOnce)(sog);
			unsigned int* (__fastcall * GiveAbilityAndActivateOnce)(UObject * ASC, int* outHandle, SpecType * Spec) = decltype(GiveAbilityAndActivateOnce)(sog);
			// Pawn->ProcessEvent("PlayEmoteItem", EmoteAsset);
			int out;
			GiveAbilityAndActivateOnce(AbilitySystemComponent, &out, (SpecType*)GenerateNewSpec(GetDefaultObject(EmoteClass)));
			Pawn->ProcessEvent("EmoteStarted", &EmoteParams->EmoteAsset);
			*/
		}
	}
	else
	{
		std::cout << "Controller: " << Controller << '\n';
		std::cout << "Emoteasset: " << EmoteAsset << '\n';
		std::cout << "Pawn: " << Pawn << '\n';
	}

	return false;
}

UObject* (*CreateNewInstanceOfAbilityO)(UObject* ASC, FGameplayAbilitySpec<FGameplayAbilityActivationInfo, 0x50>& Spec, UObject* Ability);

UObject* CreateNewInstanceOfAbilityDetour(UObject* ASC, FGameplayAbilitySpec<FGameplayAbilityActivationInfo, 0x50>& Spec, UObject* Ability)
{
	static auto EmoteAbilityClass = GetDefaultObject(FindObject(("BlueprintGeneratedClass /Game/Abilities/Emotes/GAB_Emote_Generic.GAB_Emote_Generic_C")));

	if (boozasdgwq9i)
		Ability = EmoteAbilityClass;

	auto newiNstanceavg = CreateNewInstanceOfAbilityO(ASC, Spec, Ability);

	// std::cout << "newiNstanceavg: " << newiNstanceavg << '\n';
	std::cout << "Aibliy: " << Ability->GetFullName() << '\n';
	std::cout << "newiNstanceavzcqe2v2vg: " << newiNstanceavg->GetFullName() << '\n';

	if (Ability == EmoteAbilityClass)
	{
		// newiNstanceavg->ProcessEvent("OnMontageStartedPlaying");
		// newiNstanceavg->ProcessEvent("PlayInitialEmoteMontage");

		struct FGameplayAbilityTargetDataHandle
		{
			unsigned char                                      UnknownData00[0x20];                                      // 0x0000(0x0020) MISSED OFFSET
		};

		struct {
			FGameplayAbilityTargetDataHandle aa;
			FGameplayTag aplai;
		} afhfw8u{ FGameplayAbilityTargetDataHandle (), FGameplayTag()};

		newiNstanceavg->ProcessEvent("Triggered_DE7019AA4E006879EDD264899869FEE2", &afhfw8u);
		newiNstanceavg->ProcessEvent("K2_ActivateAbility");

		if (false)
		{
			auto PlayerState = Helper::GetOwner(ASC);
			auto Pawn = *PlayerState->Member<UObject*>("PawnPrivate");
			auto Controller = Helper::GetControllerFromPawn(Pawn);

			std::cout << "Controller: " << Controller << '\n';

			static auto Montage = FindObject("AnimMontage /Game/Animation/Game/MainPlayer/Montages/Emotes/Emote_DanceMoves.Emote_DanceMoves");
			// auto Dura = PlayMontage(ASC, newiNstanceavg, FGameplayAbilityActivationInfo(), Montage, 1.0f, FName(-1));

			std::cout << "EMOTE??!\n";

			enum class EFortGameplayAbilityMontageSectionToPlay : uint8_t
			{
				FirstSection = 0,
				RandomSection = 1,
				TestedRandomSection = 2,
				EFortGameplayAbilityMontageSectionToPlay_MAX = 3
			};

			struct FFortGameplayAbilityMontageInfo
			{
				UObject* MontageToPlay;                                            // 0x0000(0x0008) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
				float                                              AnimPlayRate;                                             // 0x0008(0x0004) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
				float                                              AnimRootMotionTranslationScale;                           // 0x000C(0x0004) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
				EFortGameplayAbilityMontageSectionToPlay           MontageSectionToPlay;                                     // 0x0010(0x0001) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
				unsigned char                                      UnknownData00[0x3];                                       // 0x0011(0x0003) MISSED OFFSET
				FName                                       OverrideSection;                                          // 0x0014(0x0008) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
				bool                                               bPlayRandomSection;                                       // 0x001C(0x0001) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
				unsigned char                                      UnknownData01[0x3];                                       // 0x001D(0x0003) MISSED OFFSET
				TArray<__int64>       CharacterPartMontages;                                    // 0x0020(0x0010) (Edit, BlueprintVisible, ZeroConstructor, NativeAccessSpecifierPublic)
				unsigned char                                      UnknownData02[0x28];                                      // 0x0030(0x0028) MISSED OFFSET
			};

			// static auto calcauq9 = FindObject("Class /Script/FortniteGame.FortAbilityTask_PlayMontageWaitTarget");
			static auto calcauq9 = FindObject("Class /Script/FortniteGame.FortAbilityTask_PlayAnimWaitTarget");

			struct
			{
				UObject* OwningAbility;                                            // (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
				FName                                       TaskInstanceName;                                         // (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
				UObject* MontageToPlay;                                            // (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
				float                                              AnimPlayRate;                                             // (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
				EFortGameplayAbilityMontageSectionToPlay           SectionToPlay;                                            // (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
				FName                                       OverrideSection;                                          // (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
				float                                              AnimRootMotionTranslationScale;                           // (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
				EFortAbilityTargetDataPolicy                       TargetDataPolicy;                                         // (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
				UObject* ReturnValue;                                              // (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			} UFortAbilityTask_PlayMontageWaitTarget_PlayMontageWaitTarget_Params{ newiNstanceavg, FName(NAME_None), Montage, 1.0f, EFortGameplayAbilityMontageSectionToPlay::FirstSection,
			FName(NAME_None), 1.0f, EFortAbilityTargetDataPolicy::SimulateOnServer };

			static auto FN = FindObject("Function /Script/FortniteGame.FortAbilityTask_PlayMontageWaitTarget.PlayMontageWaitTarget");
			static auto Def = FindObject("FortAbilityTask_PlayMontageWaitTarget /Script/FortniteGame.Default__FortAbilityTask_PlayMontageWaitTarget");

			/*

			Def->ProcessEvent(FN, &UFortAbilityTask_PlayMontageWaitTarget_PlayMontageWaitTarget_Params);
			auto NewAbilityTask = UFortAbilityTask_PlayMontageWaitTarget_PlayMontageWaitTarget_Params.ReturnValue;

			*/

			auto NewAbilityTask = Easy::SpawnObject(calcauq9, Helper::GetTransientPackage());

			std::cout << "NewAbilityTask: " << NewAbilityTask << '\n';

			FFortGameplayAbilityMontageInfo AbilityMontageInfo = FFortGameplayAbilityMontageInfo{
				Montage, 1.0f, 1.0f, EFortGameplayAbilityMontageSectionToPlay::FirstSection };

			if (NewAbilityTask)
			{
				*NewAbilityTask->Member<FFortGameplayAbilityMontageInfo>("MontageInfo") = AbilityMontageInfo;
				*NewAbilityTask->Member<UObject*>("AbilitySystemComponent") = ASC;
				*NewAbilityTask->Member<UObject*>("Ability") = newiNstanceavg;
				*NewAbilityTask->Member<FName>("InstanceName") = FName(-1);
				newiNstanceavg->Member<TArray<UObject*>>("ActiveTasks")->Add(NewAbilityTask);
				NewAbilityTask->ProcessEvent("ReadyForActivation");

				struct FGameplayAbilityActorInfo
				{
					unsigned char                                      UnknownData00[0x8];                                       // 0x0000(0x0008) MISSED OFFSET
					TWeakObjectPtr<UObject>                       OwnerActor;                                               // 0x0008(0x0008) (BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData)
					TWeakObjectPtr<UObject>                       AvatarActor;                                              // 0x0010(0x0008) (BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData)
					TWeakObjectPtr<UObject>            PlayerController;                                         // 0x0018(0x0008) (BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData)
					TWeakObjectPtr<UObject>      AbilitySystemComponent;                                   // 0x0020(0x0008) (BlueprintVisible, ExportObject, BlueprintReadOnly, ZeroConstructor, InstancedReference, IsPlainOldData)
					TWeakObjectPtr<UObject>       SkeletalMeshComponent;                                    // 0x0028(0x0008) (BlueprintVisible, ExportObject, BlueprintReadOnly, ZeroConstructor, InstancedReference, IsPlainOldData)
					TWeakObjectPtr<UObject>                AnimInstance;                                             // 0x0030(0x0008) (BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData)
					TWeakObjectPtr<UObject>           MovementComponent;                                        // 0x0038(0x0008) (BlueprintVisible, ExportObject, BlueprintReadOnly, ZeroConstructor, InstancedReference, IsPlainOldData)
					FName                                       AffectedAnimInstanceTag;                                  // 0x0040(0x0008) (BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData)
				};

				auto Character = *Controller->Member<UObject*>("Character");
				auto ASC = *Pawn->Member<UObject*>("AbilitySystemComponent");
				auto SkeletalMeshComponent = *Character->Member<UObject*>("Mesh");

				// if (false)
				{

					auto CurrentActorInfo = (FGameplayAbilityActorInfo*)(__int64(newiNstanceavg) + 0x378);

					std::cout << "CurrentActorInfo: " << CurrentActorInfo << '\n';

					std::cout << "SkeletalMeshComponent: " << SkeletalMeshComponent << '\n';

					auto newActorInfo = FGameplayAbilityActorInfo();
					newActorInfo.PlayerController = TWeakObjectPtr(Controller);
					newActorInfo.AbilitySystemComponent = TWeakObjectPtr(ASC);
					newActorInfo.AnimInstance = TWeakObjectPtr(Helper::GetAnimInstance(Pawn));
					newActorInfo.SkeletalMeshComponent = TWeakObjectPtr(SkeletalMeshComponent); // WHY DOESNT THIS FIX IT!
					newActorInfo.MovementComponent = TWeakObjectPtr(*Character->Member<UObject*>("CharacterMovement"));
					newActorInfo.OwnerActor = TWeakObjectPtr(*ASC->Member<UObject*>("OwnerActor"));
					newActorInfo.AvatarActor = TWeakObjectPtr(*ASC->Member<UObject*>("AvatarActor"));
					newActorInfo.AffectedAnimInstanceTag = FName(NAME_None);

					*CurrentActorInfo = newActorInfo;
				}

				// newiNstanceavg->ProcessEvent("PlayInitialEmoteMontage");

				Sleep(15);

				// *(TWeakObjectPtr<UObject>*)(__int64(&*(FGameplayAbilityActorInfo*)(newiNstanceavg + 0x378)) + 0x28) = TWeakObjectPtr(SkeletalMeshComponent);

				void (*Activate)(UObject * AbilityTask) = decltype(Activate)(NewAbilityTask->VFTable[0x48]);
				Activate(NewAbilityTask);
			}

			// ASC->ProcessEvent("OnRep_ReplicatedAnimMontage");
		}
	}

	return newiNstanceavg;
}

static __int64 (__fastcall* FWeakObjectPtr_GetO)(__int64 a1);

__int64 __fastcall FWeakObjectPtr_GetDetour(__int64 a1)
{
	auto RetOffset = (uintptr_t)_ReturnAddress() - (uintptr_t)GetModuleHandleW(0);

	if (bPrintFUnny && RetOffset != 0x27d3757 && RetOffset != 0x2706264 && RetOffset != 0x3195dd && RetOffset != 0xcd23ea && RetOffset != 0x1ac1363 && RetOffset != 0x2895b00
		)
	{
		// std::cout << std::format("RetOffset: 0x{:x}\n", RetOffset);
	}

	if (!a1 || RetOffset == 0xB31C27 || RetOffset == 0x30d146 || RetOffset == 0x6e0d82)
		return 0;

	return FWeakObjectPtr_GetO(a1);
}

__int64 (__fastcall* FWeakObjectPtr_Get2O)(int* a1, char a2);

__int64 __fastcall FWeakObjectPtr_Get2Detour(int* a1, char a2)
{
	auto RetOffset = (uintptr_t)_ReturnAddress() - (uintptr_t)GetModuleHandleW(0);

	if (bPrintFUnny && RetOffset != 0x318dbf)
		std::cout << std::format("RetOffset: 0x{:x}\n", RetOffset);

	if (!a1 || RetOffset == 0x6e0da4)
		return 0;

	return FWeakObjectPtr_Get2O(a1, a2);
}

void replace_all(std::string& input, const std::string& from, const std::string& to) {
	size_t pos = 0;

	while ((pos = input.find(from, pos)) != std::string::npos) {
		input.replace(pos, from.size(), to);
		pos += to.size();
	}
}

char __fastcall CallsReinitializeALlProfilesDetour(__int64 a1, __int64 a2, char a3)
{
	return true;
}

inline bool ServerAttemptInteractHook(UObject* Controllera, UFunction* Function, void* Parameters)
{
	UObject* Controller = Controllera;

	if (Engine_Version >= 423)
	{
		Controller = Helper::GetOwnerOfComponent(Controllera);
	}

	// ProcessEventO(Controller, Function, Parameters);

	struct SAIParams {
		UObject* ReceivingActor;                                           // (Parm, ZeroConstructor, IsPlainOldData)
		UObject* InteractComponent;                // mesh normally
		TEnumAsByte<ETInteractionType>                     InteractType;                                             // (Parm, ZeroConstructor, IsPlainOldData)
		UObject* OptionalObjectData;                                       // (Parm, ZeroConstructor, IsPlainOldData)
	};

	auto Params = (SAIParams*)Parameters;

	if (Params && Controller)
	{
		auto Pawn = Helper::GetPawnFromController(Controller);
		auto ReceivingActor = Params->ReceivingActor;

		if (ReceivingActor && (true || basicLocationCheck(Pawn, ReceivingActor, 450.f)))
		{
			auto ReceivingActorName = ReceivingActor->GetName(); // There has to be a better way, right?

			std::cout << ("ReceivingActorName: ") << ReceivingActorName << '\n';

			static auto BuildingContainerClass = FindObject("Class /Script/FortniteGame.BuildingContainer");

			if (ReceivingActor->IsA(BuildingContainerClass))
			{
				static auto BuildingContainerClass = FindObject(("Class /Script/FortniteGame.BuildingContainer"));

				auto Prop = GetProperty(ReceivingActor, "bAlreadySearched");

				static auto offset = GetOffsetFromProp(Prop);

				if (offset == -1)
					return false;

				if (FnVerDouble != 10.40)
				{
					auto Actual = (__int64*)(__int64(ReceivingActor) + offset);

					static auto FieldMask = GetFieldMask(Prop);
					static auto BitIndex = GetBitIndex(Prop, FieldMask);

					std::cout << "BitIndex: " << (int)BitIndex << '\n';

					if (!(bool)(*(uint8_t*)Actual & BitIndex))
						return false;

					uint8_t* Byte = (uint8_t*)Actual;

					if (((bool(1) << BitIndex) & *(bool*)(Actual)) != true)
					{
						*Byte = (*Byte & ~FieldMask) | (FieldMask);
					}
				}
				else
				{
					struct ee {
						uint8_t                                        bUseLootProperties_Athena : 1;                     // Mask : 0x1 0xC41(0x1)(Edit, BlueprintVisible, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
						uint8_t                                        bAlwaysShowContainer : 1;                          // Mask : 0x2 0xC41(0x1)(Edit, BlueprintVisible, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
						uint8_t                                        bAlwaysMaintainLoot : 1;                           // Mask : 0x4 0xC41(0x1)(Edit, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
						uint8_t                                        bDestroyContainerOnSearch : 1;                     // Mask : 0x8 0xC41(0x1)(Edit, BlueprintVisible, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
						uint8_t                                        bForceHidePickupMinimapIndicator : 1;              // Mask : 0x10 0xC41(0x1)(Edit, BlueprintVisible, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
						uint8_t                                        bAlreadySearched : 1;
					};

					auto barleadysear = (ee*)(__int64(ReceivingActor) + offset);

					if (barleadysear->bAlreadySearched)
						return false;

					barleadysear->bAlreadySearched = true;
				}

				if (Engine_Version >= 424)
				{
					/* static auto StaticMeshOffset = GetOffset(ReceivingActor, "StaticMesh");
					static auto SearchedMeshOffset = GetOffset(ReceivingActor, "SearchedMesh");

					*(UObject**)(__int64(ReceivingActor) + StaticMeshOffset) = *(UObject**)(__int64(ReceivingActor) + SearchedMeshOffset); */
				}

				/* static auto bAlreadySearchedOffset = GetOffset(ReceivingActor, "bAlreadySearched");
				static auto bAlreadySearchedFM = GetFieldMask(GetProperty(ReceivingActor, "bAlreadySearched"));
				static auto bAlreadySearchedBI = GetBitIndex(GetProperty(ReceivingActor, "bAlreadySearched"), bAlreadySearchedFM);

				if (readd((uint8_t*)(__int64(ReceivingActor) + bAlreadySearchedOffset), bAlreadySearchedBI))
					return true;

				sett((uint8_t*)(__int64(ReceivingActor) + bAlreadySearchedOffset), bAlreadySearchedBI, bAlreadySearchedFM, true); */

				static auto AlreadySearchedFn = ReceivingActor->Function(("OnRep_bAlreadySearched"));

				if (AlreadySearchedFn)
					ReceivingActor->ProcessEvent(AlreadySearchedFn);
			}

			static auto VendingMachineClass = FindObject("BlueprintGeneratedClass /Game/Athena/Items/Gameplay/VendingMachine/B_Athena_VendingMachine.B_Athena_VendingMachine_C");

			if (bIsPlayground && ReceivingActor->IsA(VendingMachineClass))
			{
				static auto WoodItemData = FindObject(("FortResourceItemDefinition /Game/Items/ResourcePickups/WoodItemData.WoodItemData"));
				static auto StoneItemData = FindObject(("FortResourceItemDefinition /Game/Items/ResourcePickups/StoneItemData.StoneItemData"));
				static auto MetalItemData = FindObject(("FortResourceItemDefinition /Game/Items/ResourcePickups/MetalItemData.MetalItemData"));

				auto CurrentMaterial = *ReceivingActor->Member<UObject*>("ActiveInputItem");

				float CostAmount = 0;

				auto MaterialGUID = Inventory::GetItemGuid(Inventory::FindItemInInventory(Controller, CurrentMaterial));
				auto Count = Inventory::GetCount(Inventory::GetItemInstanceFromGuid(Controller, MaterialGUID));

				TArray<__int64>* ItemCollections = ReceivingActor->Member<TArray<__int64>>(("ItemCollections")); // CollectorUnitInfo

				UObject* DefinitionToDrop = nullptr;

				if (ItemCollections)
				{
					static UObject* CollectorUnitInfoClass = FindObject(("ScriptStruct /Script/FortniteGame.CollectorUnitInfo"));
					static std::string CollectorUnitInfoClassName = ("ScriptStruct /Script/FortniteGame.CollectorUnitInfo");

					if (!CollectorUnitInfoClass)
					{
						CollectorUnitInfoClassName = ("ScriptStruct /Script/FortniteGame.ColletorUnitInfo"); // die fortnite
						CollectorUnitInfoClass = FindObject(CollectorUnitInfoClassName); // Wedc what this value is
					}

					static auto OutputItemOffset = FindOffsetStruct(CollectorUnitInfoClassName, ("OutputItem"));
					static auto InputCountOffset = FindOffsetStruct(CollectorUnitInfoClassName, ("InputCount"));

					std::cout << ("Offset: ") << OutputItemOffset << '\n';
					// ItemCollections->At(i).OutputItem = LootingTables::GetWeaponDef();
					// So this is equal to Array[1] + OutputItemOffset, but since the array is __int64, it doesn't calcuate it properly so we have to implement it ourselves

					int Index = 0;

					if (CurrentMaterial == StoneItemData)
						Index = 1;
					else if (CurrentMaterial == MetalItemData)
						Index = 2;

					DefinitionToDrop = *(UObject**)(__int64((__int64*)((__int64(ItemCollections->GetData()) + (GetSizeOfStruct(CollectorUnitInfoClass) * Index)))) + OutputItemOffset);
					CostAmount = ((FScalableFloat*)(__int64((__int64*)((__int64(ItemCollections->GetData()) + (GetSizeOfStruct(CollectorUnitInfoClass) * Index)))) + InputCountOffset))->Value;
				}

				std::cout << "Cost Amount: " << CostAmount << '\n';

				if (Count && *Count >= CostAmount) {
					Inventory::TakeItem(Controller, MaterialGUID, CostAmount);
				}
				else
					return false;

				// CostAmount

				if (CurrentMaterial)
				{
					std::cout << "CurrentMaterial Name: " << CurrentMaterial->GetFullName() << '\n';
				}

				if (DefinitionToDrop)
				{
					Helper::SummonPickup(Pawn, DefinitionToDrop, Helper::GetCorrectLocation(ReceivingActor), EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Unset);
				}
			}

			static auto PawnClass = FindObject(("BlueprintGeneratedClass /Game/Athena/PlayerPawn_Athena.PlayerPawn_Athena_C"));

			if (ReceivingActor->IsA(PawnClass))
			{
				auto InstigatorController = Controller;
				auto DeadPawn = ReceivingActor;
				auto DeadController = *DeadPawn->Member<UObject*>("Controller");

				DeadPawn->ProcessEvent("ServerReviveFromDBNO", &InstigatorController);
				DeadPawn->ProcessEvent("ReviveFromDBNO", &InstigatorController);
				DeadController->ProcessEvent("ClientOnPawnRevived", &InstigatorController);

				setBitfield(DeadPawn, "bIsDBNO", false);
				setBitfield(DeadPawn, "bPlayedDying", false);

				*Pawn->Member<uint8_t>("DBNORevivalStacking") = 0;

				Helper::SetHealth(DeadPawn, 30);
			}

			if (ReceivingActorName.contains(("Vehicle")) || ReceivingActorName.contains("Cannon"))
			{
				Helper::SetLocalRole(Pawn, ENetRole::ROLE_Authority);
				Helper::SetLocalRole(ReceivingActor, ENetRole::ROLE_Authority);
				// Helper::SetLocalRole(*Controller->Member<UObject*>(("Pawn")), ENetRole::ROLE_AutonomousProxy);
				// Helper::SetLocalRole(ReceivingActor, ENetRole::ROLE_AutonomousProxy);

				// static auto StartupAbilitySet = *ReceivingActor->Member<UObject*>("StartupAbilitySet");
				// auto Abilities = GiveAbilitySet(Pawn, StartupAbilitySet);

				if (bAreVehicleWeaponsEnabled)
				{
					UObject* VehicleWeaponDefinition = Helper::GetWeaponForVehicle(ReceivingActor);

					std::cout << "goofy ahh\n";

					if (VehicleWeaponDefinition)
					{
						auto instnace = Inventory::GiveItem(Controller, VehicleWeaponDefinition, EFortQuickBars::Primary, 1, 1);
						*FFortItemEntry::GetLoadedAmmo(GetItemEntryFromInstance(instnace)) = INT32_MAX; // pro code
						Inventory::EquipInventoryItem(Controller, Inventory::GetItemGuid(instnace));
						std::cout << "vehicle weapon!\n";
					}
				}	
			}

			if (Engine_Version >= 424 && ReceivingActorName.contains("Wumba")) // Workbench/Upgrade Bench
			{
				auto CurrentWeapon = Helper::GetCurrentWeapon(Pawn);
				auto CurrentWeaponDefinition = Helper::GetWeaponData(CurrentWeapon);
				auto CurrentRarity = CurrentWeaponDefinition->Member<EFortRarityC2>("Rarity");
				 
				// im stupid ok

				std::pair<std::string, std::string> thingToReplace;
				
				switch (*CurrentRarity)
				{
				case EFortRarityC2::Common:
					thingToReplace = std::make_pair("_C_", "_UC_");
					break;
				case EFortRarityC2::Uncommon:
					thingToReplace = std::make_pair("_UC_", "_R_");
					break;
				case EFortRarityC2::Rare:
					thingToReplace = std::make_pair("_R_", "_VR_");
					break;
				case EFortRarityC2::Epic:
					thingToReplace = std::make_pair("_VR_", "_SR_");
					break;
				default:
					thingToReplace = std::make_pair("NONE", "NONE");
					break;
				}

				if (thingToReplace.first == "NONE")
					return false;

				std::string newDefStr = CurrentWeaponDefinition->GetFullName();
				// newDefStr.replace(newDefStr.find(thingToReplace.first), thingToReplace.first.size(), thingToReplace.second);
				replace_all(newDefStr, thingToReplace.first, thingToReplace.second);

				auto newDef = FindObject(newDefStr);

				if (!newDef)
				{
					std::cout << "Unable to find to " << newDefStr << '\n';
					return false;
				}

				auto CostPerMat = ((int)(*CurrentRarity) + 1) * 50;

				static auto WoodItemData = FindObject(("FortResourceItemDefinition /Game/Items/ResourcePickups/WoodItemData.WoodItemData"));
				static auto StoneItemData = FindObject(("FortResourceItemDefinition /Game/Items/ResourcePickups/StoneItemData.StoneItemData"));
				static auto MetalItemData = FindObject(("FortResourceItemDefinition /Game/Items/ResourcePickups/MetalItemData.MetalItemData"));

				auto WoodGUID = Inventory::GetItemGuid(Inventory::FindItemInInventory(Controller, WoodItemData));
				auto StoneGUID = Inventory::GetItemGuid(Inventory::FindItemInInventory(Controller, StoneItemData));
				auto MetalGUID = Inventory::GetItemGuid(Inventory::FindItemInInventory(Controller, MetalItemData));

				Inventory::TakeItem(Controller, WoodGUID, CostPerMat);
				Inventory::TakeItem(Controller, StoneGUID, CostPerMat);
				Inventory::TakeItem(Controller, MetalGUID, CostPerMat);

				Inventory::TakeItem(Controller, Inventory::GetWeaponGuid(CurrentWeapon), 1, true);
				Inventory::GiveItem(Controller, newDef, EFortQuickBars::Primary, 1, 1);
			}

			if (ReceivingActorName.contains("Portapotty")) // dont work rn
			{
				return true;
			}

			// Looting::Tables::HandleSearch(ReceivingActor);
			LootingV2::HandleSearch(ReceivingActor);
		}
	}

	return false;
}

inline bool ServerSendZiplineStateHook(UObject* Pawn, UFunction* Function, void* Parameters)
{
	if (Pawn && Parameters)
	{
		std::cout << "mf!\n";
		
		struct FZiplinePawnState
		{
			UObject* Zipline;           // AFortAthenaZipline                                        // 0x0000(0x0008) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			bool                                               bIsZiplining;                                             // 0x0008(0x0001) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			bool                                               bJumped;                                                  // 0x0009(0x0001) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			unsigned char                                      UnknownData00[0x2];                                       // 0x000A(0x0002) MISSED OFFSET
			int                                                AuthoritativeValue;                                       // 0x000C(0x0004) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			FVector                                     SocketOffset;                                             // 0x0010(0x000C) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			float                                              TimeZipliningBegan;                                       // 0x001C(0x0004) (ZeroConstructor, IsPlainOldData, RepSkip, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			float                                              TimeZipliningEndedFromJump;                               // 0x0020(0x0004) (ZeroConstructor, IsPlainOldData, RepSkip, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			unsigned char                                      UnknownData01[0x4];                                       // 0x0024(0x0004) MISSED OFFSET
		};

		struct parms { FZiplinePawnState ZiplineState; };
		auto Params = (parms*)Parameters;

		static auto ZiplineOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.ZiplinePawnState"), ("Zipline"));
		static auto SocketOffsetOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.ZiplinePawnState"), ("SocketOffset"));

		auto Zipline = (UObject**)(__int64(&Params->ZiplineState) + ZiplineOffset);
		auto SocketOffset = (FVector*)(__int64(&Params->ZiplineState) + SocketOffsetOffset);

		// Helper::SetLocalRole(Pawn, ENetRole::ROLE_AutonomousProxy);
		// Helper::SetRemoteRole(Pawn, ENetRole::ROLE_Authority);

		struct FFortAnimInput_Zipline
		{
			unsigned char                                      bIsZiplining : 1;
		};

		if (Zipline && *Zipline)
		{
			// TWeakObjectPtr<class AFortPlayerPawn>  CurrentInteractingPawn
			
			TWeakObjectPtr<UObject>* CurrentInteractingPawn = nullptr; // (*Zipline)->Member<TWeakObjectPtr<UObject>>("CurrentInteractingPawn");

			if (CurrentInteractingPawn)
			{
				CurrentInteractingPawn->ObjectIndex = Pawn->InternalIndex;
				CurrentInteractingPawn->ObjectSerialNumber = GetSerialNumber(Pawn);
			}

			// Helper::SetLocalRole(Pawn, ENetRole::ROLE_AutonomousProxy);

			struct
			{
				UObject* Zipline;        // AFortAthenaZipline                                           // (ConstParm, Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
				UObject* SocketComponent;         // USceneComponent                                 // (Parm, ZeroConstructor, InstancedReference, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
				FName                                       SocketName;                                               // (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
				FVector                                     SocketOffset;                                             // (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			} AFortPlayerPawn_BeginZiplining_Params{*Zipline, *(*Zipline)->Member<UObject*>("RootComponent"), FName(-1), *SocketOffset};

			// ^^ they eventually remove the last 2 params

			static auto BeginZiplining = Pawn->Function("BeginZiplining");

			if (BeginZiplining)
				Pawn->ProcessEvent(BeginZiplining, &AFortPlayerPawn_BeginZiplining_Params);

			/* static auto StartZipLining = (*Zipline)->Function("StartZipLining");

			if (StartZipLining)
				(*Zipline)->ProcessEvent(StartZipLining, &Pawn); */

			/* Helper::GetAnimInstance(Pawn)->Member<FFortAnimInput_Zipline>("ZiplineInput")->bIsZiplining = true;
			
			*Pawn->Member<FZiplinePawnState>("ZiplineState") = Params->ZiplineState;
			Pawn->ProcessEvent("OnRep_ZiplineState"); */

			// Helper::SetLocalRole(*Zipline, ENetRole::ROLE_AutonomousProxy);
			// Helper::SetLocalRole(*Zipline, ENetRole::ROLE_Authority); // UNTESTED
			// Helper::SetRemoteRole(*Zipline, ENetRole::ROLE_Authority);
		}
		else
		{
			bool bJumped = Params->ZiplineState.bJumped;

			static auto EndZiplining = Pawn->Function("EndZiplining");

			if (EndZiplining)
				Pawn->ProcessEvent(EndZiplining, &bJumped);

			Helper::GetAnimInstance(Pawn)->Member<FFortAnimInput_Zipline>("ZiplineInput")->bIsZiplining = false;

			*Pawn->Member<FZiplinePawnState>("ZiplineState") = Params->ZiplineState;
			Pawn->ProcessEvent("OnRep_ZiplineState");
			std::cout << ("Player is getting off of zipline!\n");
		}
	}

	return false;
}

inline bool ReceiveActorEndOverlapHook(UObject* Actor, UFunction*, void* Parameters)
{
	if (Actor)
	{
		struct parms { UObject* otherActor; };

		auto Params = (parms*)Parameters;

		std::cout << "Actor: " << Actor->GetFullName() << '\n';

		if (Params->otherActor && Actor->GetFullName().contains("Zipline"))
		{
			struct FFortAnimInput_Zipline
			{
				unsigned char                                      bIsZiplining : 1;
			};

			auto Pawn = Params->otherActor;
			Helper::GetAnimInstance(Pawn)->Member<FFortAnimInput_Zipline>("ZiplineInput")->bIsZiplining = false;
			bool bJumped = true;
			Pawn->ProcessEvent("EndZiplining", &bJumped);

			struct FZiplinePawnState
			{
				UObject* Zipline;           // AFortAthenaZipline                                        // 0x0000(0x0008) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
				bool                                               bIsZiplining;                                             // 0x0008(0x0001) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
				bool                                               bJumped;                                                  // 0x0009(0x0001) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
				unsigned char                                      UnknownData00[0x2];                                       // 0x000A(0x0002) MISSED OFFSET
				int                                                AuthoritativeValue;                                       // 0x000C(0x0004) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
				FVector                                     SocketOffset;                                             // 0x0010(0x000C) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
				float                                              TimeZipliningBegan;                                       // 0x001C(0x0004) (ZeroConstructor, IsPlainOldData, RepSkip, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
				float                                              TimeZipliningEndedFromJump;                               // 0x0020(0x0004) (ZeroConstructor, IsPlainOldData, RepSkip, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
				unsigned char                                      UnknownData01[0x4];                                       // 0x0024(0x0004) MISSED OFFSET
			};

			auto ZiplineState = Pawn->Member<FZiplinePawnState>("ZiplineState");
			ZiplineState->bIsZiplining = false;
			ZiplineState->SocketOffset = FVector();
			ZiplineState->Zipline = nullptr;
			ZiplineState->bJumped = true;
			ZiplineState->AuthoritativeValue = 0;
			*Actor->Member<UObject*>("PlayerPawn") = nullptr;

			Pawn->ProcessEvent("OnRep_ZiplineState");

			/* struct
			{
			public:
				UObject* OverlappedComponent;         // UPrimitiveComponent                      // 0x0(0x8)(BlueprintVisible, BlueprintReadOnly, Parm, ZeroConstructor, InstancedReference, IsPlainOldData, NoDestructor, HasGetValueTypeHash)
				UObject* OtherActor;      // AActor                                   // 0x8(0x8)(BlueprintVisible, BlueprintReadOnly, Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash)
				UObject* OtherComp; // UPrimitiveComponent // 0x10(0x8)(BlueprintVisible, BlueprintReadOnly, Parm, ZeroConstructor, InstancedReference, IsPlainOldData, NoDestructor, HasGetValueTypeHash)
				int32_t                                       OtherBodyIndex;                                    // 0x18(0x4)(BlueprintVisible, BlueprintReadOnly, Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash)
			} ABP_Athena_Environmental_ZipLine_C_HandleOnEndOverlap_Params{ *Actor->Member<UObject*>("RootComponent"), Params->otherActor, nullptr, 0};

			static auto HandleOnEndOverlap = Actor->Function("HandleOnEndOverlap");

			if (HandleOnEndOverlap)
				Actor->ProcessEvent(HandleOnEndOverlap, &ABP_Athena_Environmental_ZipLine_C_HandleOnEndOverlap_Params); */
		}
	}

	return false;
}

inline bool PlayButtonHook(UObject* Object, UFunction* Function, void* Parameters)
{
	LoadInMatch();
	return false;
}

inline bool ClientWasKickedHook(UObject* Controller, UFunction*, void* Params)
{
	std::cout << "ClientWasKicked!\n";

	if (FnVerDouble >= 16.00)
	{
		return true;
	}

	return false;
}

inline bool IsResurrectionChipAvailableHook(UObject* PlayerState, UFunction* Function, void* Parameters)
{
	struct IRCAParams { bool ret; };

	auto Params = (IRCAParams*)Parameters;

	if (Params)
	{
		Params->ret = true;
	}

	return true;
}

inline bool ServerClientPawnLoadedHook(UObject* Controller, UFunction* Function, void* Parameters)
{
	struct SCPLParams { bool bIsPawnLoaded; };
	auto Params = (SCPLParams*)Parameters;

	if (Parameters && Controller)
	{
		std::cout << ("bIsPawnLoaded: ") << Params->bIsPawnLoaded << '\n';

		auto Pawn = *Controller->Member<UObject*>(("Pawn"));

		if (Pawn)
		{
			auto bLoadingScreenDropped = *Controller->Member<bool>(("bLoadingScreenDropped"));
			if (bLoadingScreenDropped && !Params->bIsPawnLoaded)
			{
			}
			else
				std::cout << ("Loading screen is not dropped!\n");
		}
		else
			std::cout << ("Pawn is not valid!\n");
	}

	return false;
}

inline bool pawnClassHook(UObject* GameModeBase, UFunction*, void* Parameters)
{
	struct parms{ UObject* InController; UObject* Class; };
	std::cout << "called!\n";

	static auto PawnClass = FindObject(("BlueprintGeneratedClass /Game/Athena/PlayerPawn_Athena.PlayerPawn_Athena_C"));

	auto Params = (parms*)Parameters;
	Params->Class = PawnClass;

	return true;
}

inline bool AircraftExitedDropZoneHook(UObject* GameMode, UFunction* Function, void* Parameters)
{
	/*
	
	Function called: Function /Script/Engine.GameModeBase.FindPlayerStart
	Function called: Function /Script/Engine.GameModeBase.ChoosePlayerStart
	Function called: Function /Script/Engine.GameModeBase.MustSpectate
	Function called: Function /Script/Engine.GameModeBase.GetDefaultPawnClassForController
	Function called: Function /Script/Engine.GameModeBase.SpawnDefaultPawnFor
	Function called: Function /Script/Engine.GameModeBase.GetDefaultPawnClassForController
	Function called: Function /Script/Engine.Pawn.ReceiveUnpossessed
	Function called: Function /Script/Engine.Actor.ReceiveEndPlay
	Function called: Function /Script/Engine.Actor.ReceiveDestroyed

	*/

	return true;
}

inline bool ServerChoosePartHook(UObject* Pawn, UFunction* Function, void* Parameters)
{
	struct SCP_Params {
		TEnumAsByte<EFortCustomPartType> Part;
		UObject* ChosenCharacterPart;
	};

	auto Params = (SCP_Params*)Parameters;

	if (Params && (!Params->ChosenCharacterPart && Params->Part.Get() != EFortCustomPartType::Backpack))
		return true;

	return false;
}

inline bool OnDeathServerHook(UObject* BuildingActor, UFunction* Function, void* Parameters) // credits: Pro100kat
{
	if (BuildingActor && bStarted && !bRestarting)
	{
		if (bDoubleBuildFix)
		{
			static auto BuildingSMActorClass = FindObject(("Class /Script/FortniteGame.BuildingSMActor"));
			if (ExistingBuildings.size() > 0 && BuildingActor->IsA(BuildingSMActorClass))
			{
				for (int i = 0; i < ExistingBuildings.size(); i++)
				{
					auto Building = ExistingBuildings[i];

					if (!Building)
						continue;

					if (Building == BuildingActor)
					{
						ExistingBuildings.erase(ExistingBuildings.begin() + i);
						break;
					}
				}
			}
		}

		if (Engine_Version >= 424)
		{
			static auto BuildingContainerClass = FindObject(("Class /Script/FortniteGame.BuildingContainer"));

			// static auto bDoNotDropLootOnDestructionOffset = GetOffset(BuildingActor, "bDoNotDropLootOnDestruction");
			// static auto bDoNotDropLootOnDestructionBI = GetBitIndex(GetProperty(BuildingActor, "bDoNotDropLootOnDestruction"));

			auto Prop = GetProperty(BuildingActor, "bAlreadySearched");

			auto offset = GetOffsetFromProp(Prop);

			if (offset == -1)
				return false;

			auto Actual = (__int64*)(__int64(BuildingActor) + offset);

			static auto FieldMask = GetFieldMask(Prop);
			static auto BitIndex = GetBitIndex(Prop, FieldMask);

			if (!(bool)(*(uint8_t*)Actual & BitIndex))
				return false;

			if (BuildingActor->IsA(BuildingContainerClass)
				// && !readd((uint8_t*)(__int64(BuildingActor) + bDoNotDropLootOnDestructionOffset), bDoNotDropLootOnDestructionBI)
				)
			{
				LootingV2::HandleSearch(BuildingActor);
			}
		}
	}

	return false;
}

bool ServerUpdateVehicleInputStateReliableHook(UObject* Pawn, UFunction* Function, void* Parameters) // used for tricks like rolling
{
	if (Pawn)
	{
		struct FFortAthenaVehicleInputStateReliable
		{
			unsigned char                                      bIsSprinting : 1;                                         // 0x0000(0x0001) (NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			unsigned char                                      bIsJumping : 1;                                           // 0x0000(0x0001) (NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			unsigned char                                      bIsBraking : 1;                                           // 0x0000(0x0001) (NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			unsigned char                                      bIsHonking : 1;                                           // 0x0000(0x0001) (NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			unsigned char                                      bIgnoreForwardInAir : 1;                                  // 0x0000(0x0001) (NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			unsigned char                                      bMovementModifier0 : 1;                                   // 0x0000(0x0001) (NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			unsigned char                                      bMovementModifier1 : 1;                                   // 0x0000(0x0001) (NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			unsigned char                                      bMovementModifier2 : 1;                                   // 0x0000(0x0001) (NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		};

		struct parms { FFortAthenaVehicleInputStateReliable ReliableInput; };

		auto Params = (parms*)Parameters;

		static auto MulticastUpdateVehicleInputStateReliable = Pawn->Function("MulticastUpdateVehicleInputStateReliable");

		struct { FFortAthenaVehicleInputStateReliable ReliableInput; } newParams{Params->ReliableInput};

		if (MulticastUpdateVehicleInputStateReliable)
			Pawn->ProcessEvent(MulticastUpdateVehicleInputStateReliable, &newParams);
		else
			std::cout << "No MulticastUpdateVehicleInputStateReliable!\n";
		
		std::cout << "aa!\n";
	}

	return false;
}

bool ServerUpdateVehicleInputStateUnreliableHook(UObject* Pawn, UFunction* Function, void* Parameters) // used for turning
{
	struct FFortAthenaVehicleInputStateUnreliable
	{
		float                                              ForwardAlpha;                                             // 0x0000(0x0004) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		float                                              RightAlpha;                                               // 0x0004(0x0004) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		float                                              PitchAlpha;                                               // 0x0008(0x0004) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		float                                              LookUpDelta;                                              // 0x000C(0x0004) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		float                                              TurnDelta;                                                // 0x0010(0x0004) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		float                                              SteerAlpha;                                               // 0x0014(0x0004) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		float                                              GravityOffset;                                            // 0x0018(0x0004) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		FVector                      MovementDir;                                              // 0x001C(0x000C) (NoDestructor, NativeAccessSpecifierPublic)
	};

	struct parms { FFortAthenaVehicleInputStateUnreliable InState; };
	auto Params = (parms*)Parameters;

	// static auto PitchAlphaOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortAthenaVehicleInputStateUnreliable"), ("PitchAlpha"));
	// auto PitchAlpha = (float*)(__int64(&Params->InState) + PitchAlphaOffset);

	static auto MovementDirOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortAthenaVehicleInputStateUnreliable"), ("MovementDir"));
	auto MovementDir = (FVector*)(__int64(&Params->InState) + MovementDirOffset);

	if (Pawn && Params && MovementDir)
	{
		auto Vehicle = Helper::GetVehicle(Pawn);

		if (Vehicle)
		{
			auto Rotation = Helper::GetActorRotation(Vehicle);
			auto Location = Helper::GetActorLocation(Vehicle);

			/* std::cout << ("ForwardAlpha: ") << Params->InState.ForwardAlpha << '\n';
			std::cout << ("RightAlpha: ") << Params->InState.RightAlpha << '\n';
			std::cout << ("PitchAlpha: ") << Params->InState.PitchAlpha << '\n';
			std::cout << ("LookUpDelta: ") << Params->InState.LookUpDelta << '\n';
			std::cout << ("TurnDelta: ") << Params->InState.TurnDelta << '\n';
			std::cout << ("SteerAlpha: ") << Params->InState.SteerAlpha << '\n';
			std::cout << ("GravityOffset: ") << Params->InState.GravityOffset << '\n';
			std::cout << ("MovementDir: ") << MovementDir->Describe() << '\n'; */

			auto newRotation = FRotator{ Rotation.Pitch, Rotation.Yaw, Rotation.Roll };

			// Helper::SetActorLocationAndRotation(Vehicle, Location, newRotation); 
		}
	}

	return false;
}

bool Server_SpawnProjectileHook(UObject* something, UFunction* Function, void* Parameters)
{
	if (something)
	{
		/* struct parm { FVector Loc; FRotator Direction; };
		auto Params = (parm*)Parameters;
		static auto ProjClass = FindObject("BlueprintGeneratedClass /Game/Athena/Items/Consumables/TowerGrenade/B_Prj_Athena_TowerGrenade.B_Prj_Athena_TowerGrenade_C");
		UObject* Proj = Easy::SpawnActor(ProjClass, Params->Loc, FRotator());// *something->Member<UObject*>("ProjectileReference"); // i think im stupid

		if (Proj)
		{
			std::cout << "Proj name: " << Proj->GetFullName() << '\n';

			// ClearAndBuild is useless
			// so is CreateBaseSection

			struct
			{
				FVector                                     ReferenceLocation;                                        // (BlueprintVisible, BlueprintReadOnly, Parm, IsPlainOldData)
			} AB_Prj_Athena_TowerGrenade_C_SpawnTires_Params{ Params->Loc };

			static auto SpawnTires = Proj->Function("SpawnTires");

			if (SpawnTires)
				Proj->ProcessEvent(SpawnTires, &AB_Prj_Athena_TowerGrenade_C_SpawnTires_Params);
			else
				std::cout << "No SpawnTires\n";

		}
		else
			std::cout << "No Proj!\n"; */

		/*
		
		static void SpawnActorsInPlayset(class AActor* WorldContextObject, class UFortPlaysetItemDefinition* Playset);
		struct FName GetPlaysetName();
		static struct FVector AdjustToFinalLocation(class UObject* WorldContextObject, class UFortPlaysetItemDefinition* Playset, struct FVector BaseLocation, struct FRotator Rotation)

		*/
	}

	return false;
}

bool UpdateControlStateHook(UObject* DoghouseVehicle, UFunction*, void* Parameters)
{
	struct FReplicatedControlState
	{
		FVector                   Up;                                                       // 0x0000(0x000C) (NoDestructor, NativeAccessSpecifierPublic)
		FVector                   Forward;                                                  // 0x000C(0x000C) (NoDestructor, NativeAccessSpecifierPublic)
		unsigned char                                      bIsEngineOn : 1;                                          // 0x0018(0x0001) (NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		unsigned char                                      UnknownData00[0x3];                                       // 0x0019(0x0003) MISSED OFFSET
	};

	if (DoghouseVehicle && Parameters)
	{
		auto Params = *(FReplicatedControlState*)Parameters;

		std::cout << "Up: " << Params.Up.Describe() << '\n';
		std::cout << "Forward: " << Params.Forward.Describe() << '\n';

		auto oldRot = Helper::GetActorRotation(DoghouseVehicle);
		auto newRot = oldRot;
		Helper::SetActorLocationAndRotation(DoghouseVehicle, Helper::GetActorLocation(DoghouseVehicle), newRot);
	}

	return false;
}

bool balloonFunHook(UObject* ability, UFunction* Function, void* Parameters)
{
	if (ability)
	{
		std::cout << "balloon!\n";
		// ability->ProcessEvent("SpawnBalloon");
		auto Pawn = nullptr; // *ability->Member<UObject*>("PlayerPawn");
		std::cout << "Pawn: " << Pawn << '\n';
		ability->Member<TArray<UObject*>>("Balloons")->Add(Easy::SpawnActor(FindObject("Class /Script/FortniteGame.BuildingGameplayActorBalloon"), Helper::GetActorLocation(Pawn)));
	}

	return false;
}

bool boomboxHook(UObject* ability, UFunction* Function, void* Parameters)
{
	if (ability)
	{
		struct parm { FVector Loc; FRotator Direction; };
		auto Params = (parm*)Parameters;

		static auto ProjClass = FindObject("BlueprintGeneratedClass /Game/Athena/Items/Consumables/BoomBox/B_Proj_BoomBox.B_Proj_BoomBox_C");
		UObject* Proj = Easy::SpawnActor(ProjClass, Params->Loc, FRotator());// *something->Member<UObject*>("ProjectileReference"); // i think im stupid

		if (Proj)
		{
			std::cout << "Proj name: " << Proj->GetFullName() << '\n';
			static auto actualBoomBoxClass = FindObject("BlueprintGeneratedClass /Game/Athena/Items/Consumables/BoomBox/BGA_Athena_BoomBox.BGA_Athena_BoomBox_C");
			auto newBoomBox = Easy::SpawnActor(actualBoomBoxClass, Helper::GetActorLocation(Proj));
			*Proj->Member<UObject*>("SpawnedBGA") = newBoomBox;

			UObject* Pawn; // Helper::GetOwner(ability);
			ability->ProcessEvent("GetActivatingPawn", &Pawn);

			std::cout << "Pawn: " << Pawn << '\n';

			std::cout << "Pawn Name: " << Pawn->GetFullName() << '\n';

			Helper::InitializeBuildingActor(*Pawn->Member<UObject*>("Controller"), newBoomBox);
		}
	}

	return false;
}

bool throwableConsumablesHook(UObject* ability, UFunction*, void* Parameters)
{
	if (ability)
	{
		// ability->Server_SpawnProjectile
		// ability->ThrowConsumable
		
		auto memberNames = GetMemberNames(ability, true, false);

		for (auto& MemberName : memberNames)
			std::cout << "memberName: " << MemberName << '\n';

		std::cout << "throwable!\n";
	}

	return false;
}

bool riftItemHook(UObject* ability, UFunction* Function, void* Parameters)
{
	std::cout << "rift!\n";

	if (ability)
	{
		UObject* Pawn; // Helper::GetOwner(ability);
		ability->ProcessEvent("GetActivatingPawn", &Pawn);
		Helper::TeleportToSkyDive(Pawn, 11000);

		__int64* SkydiveAbilitySpec = nullptr;

		auto FindSkydiveAbility = [&SkydiveAbilitySpec](__int64* Spec) -> void {
			auto Ability = *GetAbilityFromSpec(Spec);
			if (Ability && Ability->GetFullName().contains("GA_Rift_Athena_Skydive_C"))
			{
				SkydiveAbilitySpec = Spec;
			}
		};

		auto ASC = *Pawn->CachedMember<UObject*>("AbilitySystemComponent");
		LoopSpecs(ASC, FindSkydiveAbility);

		if (SkydiveAbilitySpec)
		{
			std::cout << "foujd spec!\n";
			// std::cout << "found skydive ability: " << SkydiveAbilitySpec->GetFullName() << '\n';
			auto SkyDiveAbility = *GetAbilityFromSpec(SkydiveAbilitySpec);
			*SkyDiveAbility->Member<UObject*>("PlayerPawn") = Pawn;
			SkyDiveAbility->ProcessEvent("SetPlayerToSkydive");
			SkyDiveAbility->ProcessEvent("K2_ActivateAbility"); // does nothing
		}
		else
			std::cout << "failed to fmind skydive ability!\n";
	}

	return false;
}

__int64 (__fastcall* FailedToSpawnPawnO)(__int64 a1);

__int64 __fastcall FailedToSpawnPawnDetour(__int64 a1)
{
	std::cout << "FAiled to spawn a pawn: " << __int64(_ReturnAddress()) - (uintptr_t)GetModuleHandleW(0) << '\n';
	return FailedToSpawnPawnO(a1);
}

bool onpawnmotnageblendingoutHook(UObject* weapon, UFunction*, void*)
{
	if (weapon)
	{
		auto pawn = Helper::GetOwner(weapon);

		auto Controller = *pawn->Member<UObject*>(("Controller"));
		auto AmmoCount = weapon->Member<int>("AmmoCount");
		auto oldAmmoCount = *AmmoCount;
		auto entry = Inventory::GetEntryFromWeapon(Controller, weapon);
		(*FFortItemEntry::GetLoadedAmmo(entry)) -= 1; // = *AmmoCount;
		Inventory::Update(Controller, -1, true, (FFastArraySerializerItem*)entry);

		static auto OnRep_AmmoCount = weapon->Function("OnRep_AmmoCount");

		if (OnRep_AmmoCount)
			weapon->ProcessEvent(OnRep_AmmoCount, &oldAmmoCount);

		std::cout << "called!\n";
	}

	return false;
}

char (__fastcall* sutpidfucnO)(FFastArraySerializerSE::TFastArraySerializeHelper* a1, __int64 a2, int a3);

char __fastcall sutpidfucnDetour(FFastArraySerializerSE::TFastArraySerializeHelper* a1, __int64 a2, int a3)
{
	// oldmap size does not match blah blah
	std::cout << "faastararai!\n";

	std::cout << "AHH: " <<  a1->Struct->GetName() << '\n';// GetOwnerStruct()->GetName();
	
	// a1->ArraySerializer.MarkArrayDirty();

	return sutpidfucnO(a1, a2, a3);
}

bool RiftPortalActivateAbility(UObject* Ability, UObject* Func, void* Parameters)
{
	auto pawn = *Ability->Member("PlayerPawn");

	if (pawn)
	{
		static auto TeleportToSkyDiveFn = pawn->Function(("TeleportToSkyDive"));

		float heihr5 = 10000;

		if (TeleportToSkyDiveFn)
			pawn->ProcessEvent(TeleportToSkyDiveFn, &heihr5);
	}

	return false;
}

bool PlayerCanRestartHook(UObject* GameMode, UFunction*, void* Parameters)
{
	std::cout << "eehhg!~\n";

	struct parm { UObject* PC; bool ret; };

	auto Params = (parm*)(Parameters);

	Params->ret = true;

	return true;
}

bool MustSpectateHook(UObject* GameMode, UFunction*, void* Parameters)
{
	struct parm { UObject* PC; bool ret; };

	auto Params = (parm*)(Parameters);

	Params->ret = true;

	std::cout << "aefiu132q8f!\n";
	return false;
}

bool ServerPlaySquadQuickChatMessageHook(UObject* Controller, UFunction* func, void* Parameters)
{
	return false;
}

bool NetMulticast_InvokeGameplayCueExecuted_WithParamsHook(UObject* Pawn, UFunction*, void* Parameters)
{
	struct someparms { FGameplayTagContainer tags; };
	auto Params = (someparms*)Parameters;
	std::cout << "fafhquyur8321!\n";
	// std::cout << "Called with " << Params->tags.ToStringSimple(true) << '\n';

	return true;
}

bool OnBuildingActorInitializedHook(UObject* newBuilding, UFunction*, void* Parameters)
{
	std::cout << "Owner: " << Helper::GetOwner(newBuilding)->GetFullName() << '\n';

	if (false)
	{
		auto Controller = *(UObject**)Parameters;
		auto Pawn = Helper::GetPawnFromController(Controller);

		UObject* MatDefinition = nullptr;

		static auto WoodItemData = FindObject(("FortResourceItemDefinition /Game/Items/ResourcePickups/WoodItemData.WoodItemData"));
		static auto StoneItemData = FindObject(("FortResourceItemDefinition /Game/Items/ResourcePickups/StoneItemData.StoneItemData"));
		static auto MetalItemData = FindObject(("FortResourceItemDefinition /Game/Items/ResourcePickups/MetalItemData.MetalItemData"));

		if (Controller && Pawn)
		{
			{
				static auto ResourceTypeOffset = GetOffset(newBuilding, "ResourceType");
				auto ResourceType = (TEnumAsByte<EFortResourceType>*)(__int64(newBuilding) + ResourceTypeOffset);

				switch (ResourceType ? ResourceType->Get() : EFortResourceType::None)
				{
				case EFortResourceType::Wood:
					MatDefinition = WoodItemData;
					break;
				case EFortResourceType::Stone:
					MatDefinition = StoneItemData;
					break;
				case EFortResourceType::Metal:
					MatDefinition = MetalItemData;
					break;
				}
			}

			UObject* MatInstance = Inventory::FindItemInInventory(Controller, MatDefinition);

			bool bSuccessful = false;
			constexpr bool bUseAnticheat = false;

			if (MatInstance && *FFortItemEntry::GetCount(GetItemEntryFromInstance(MatInstance)) >= 10 &&
				(!bUseAnticheat || validBuild(newBuilding, Pawn)))
			{
				auto PlayerState = Helper::GetPlayerStateFromController(Controller);

				static auto TeamOffset = GetOffset(newBuilding, "Team");

				auto PlayersTeamIndex = *Teams::GetTeamIndex(PlayerState);;

				if (TeamOffset != -1)
				{
					auto Team = (TEnumAsByte<EFortTeam>*)(__int64(newBuilding) + TeamOffset);
					*Team = PlayersTeamIndex; // *PlayerState->Member<TEnumAsByte<EFortTeam>>("Team");
				}

				static auto Building_TeamIndexOffset = GetOffset(newBuilding, "TeamIndex");

				if (Building_TeamIndexOffset != -1)
				{
					auto TeamIndex = (uint8_t*)(__int64(newBuilding) + Building_TeamIndexOffset);
					*TeamIndex = PlayersTeamIndex;
				}

				// Helper::SetMirrored(BuildingActor, bMirrored);
				// Helper::InitializeBuildingActor(Controller, newBuilding, true);

				bSuccessful = true;

				// if (!Helper::IsStructurallySupported(BuildingActor))
					// bSuccessful = false;
			}

			if (!bSuccessful)
			{
				Helper::SetActorScale3D(newBuilding, {});
				Helper::SilentDie(newBuilding);
				return true;
			}
			else
			{
				if (!bIsPlayground)
				{
					Inventory::DecreaseItemCount(Controller, MatInstance, 10);
				}
			}
		}
	}

	return false;
}

bool SpawnDefaultPawnForHook(UObject* GameMode, UFunction*, void* Parameters)
{
	struct parms { UObject* NewPlayer; UObject* StartSpot; UObject* Pawn; };

	auto Params = (parms*)Parameters;

	Helper::GetPlayerStart(&Params->StartSpot); // probably unneeded

	// static auto PawnClass = FindObject(("BlueprintGeneratedClass /Game/Athena/PlayerPawn_Athena.PlayerPawn_Athena_C"));
	// Params->Pawn = Easy::SpawnActor(PawnClass, Helper::GetActorLocation(Params->StartSpot)); // Helper::InitPawn(Params->NewPlayer);

	FTransform Transform;
	Transform.Translation = Helper::GetActorLocation(Params->StartSpot);
	Transform.Scale3D = FVector{ 1, 1, 1 };

	struct {
		UObject* NewController;
		FTransform SpawnTransform;
		UObject* NewPawn;
	} SpawnDefaultPawnAtTransform_Params{Params->NewPlayer, Transform};

	static auto SpawnDefaultPawnAtTransform = GameMode->Function("SpawnDefaultPawnAtTransform");

	if (SpawnDefaultPawnAtTransform)
		GameMode->ProcessEvent(SpawnDefaultPawnAtTransform, &SpawnDefaultPawnAtTransform_Params);

	auto NewPawn = SpawnDefaultPawnAtTransform_Params.NewPawn;

	return true;
}

bool ServerClientIsReadyToRespawnHook(UObject* Controller, UFunction*, void* Parameters)
{
	if (!bIsPlayground)
		return false;

	auto Pawn = Controller->Member<UObject*>("Pawn");

	auto RespawnLocation = FVector(); // help

	*Pawn = Helper::InitPawn(Controller, false, RespawnLocation);

	return false;
}

bool OnBounceHook(UObject* ConsumablePrj, UFunction*, void* Parameters)
{
	// I DONT HAVE A SDK IDK WHAT TO DO

	return false;
}

bool ServerTeleportToPlaygroundLobbyIslandHook(UObject* Controller, UFunction*, void* Parameters)
{
	if (!bIsCreative)
		return false;

	auto Pawn = Helper::GetPawnFromController(Controller);

	if (Pawn)
	{
		Helper::SetActorLocation(Pawn, Helper::GetPlayerStart());
	}

	return false;
}

bool ServerRequestSeatChangeHook(UObject* Controller, UFunction*, void* Parameters)
{
	auto Pawn = Helper::GetPawnFromController(Controller);

	if (Pawn && Parameters)
	{
		// CURSED

		if (bAreVehicleWeaponsEnabled)
		{
			EVehicleType Type;
			UObject* VehicleWeaponDef = Helper::GetWeaponForVehicle(Helper::GetVehicle(Pawn), &Type);

			auto TargetSeatIndex = *(int*)Parameters;

			if (VehicleWeaponDef && Type != EVehicleType::Unknown)
			{
				auto VehicleWeaponInstance = Inventory::FindItemInInventory(Controller, VehicleWeaponDef);

				if (VehicleWeaponInstance)
				{
					Inventory::RemoveItem(Controller, Inventory::GetItemGuid(VehicleWeaponInstance));
					Inventory::EquipInventoryItem(Controller, Inventory::GetItemGuid(Inventory::FindItemInInventory(Controller, Helper::GetPickaxeDef(Controller)))); // crazy
				}

				switch (Type)
				{
				case EVehicleType::Biplane:
					if (TargetSeatIndex == 0) // idk if for all vehicles seat 0 is driver (well i dont think all have ewwea9fuohgaoii ida skid
					{
						auto instnace = Inventory::GiveItem(Controller, VehicleWeaponDef, EFortQuickBars::Primary, 1, 1);
						*FFortItemEntry::GetLoadedAmmo(GetItemEntryFromInstance(instnace)) = INT32_MAX; // pro code
						Inventory::EquipInventoryItem(Controller, Inventory::GetItemGuid(instnace));
					}
				}
			}
			
			std::cout << "TargetSeatIndex: " << TargetSeatIndex << '\n';
		}
	}

	return false;
}

bool OnAircraftExitedDropZoneHook(UObject* GameMode, UFunction*, void* Parameters)
{
	auto aa = [](UObject* Controller) {
		if (Helper::IsInAircraft(Controller))
		{
			FRotator rot{ 0, 0, 0 };
			ServerAttemptAircraftJumpHook(Controller, nullptr, &rot);
		}
	};

	Helper::LoopConnections(aa, true);

	return false;
}

void FinishInitializeUHooks()
{
	AddHook("Function /Script/FortniteGame.FortPlayerControllerZone.ServerRequestSeatChange", ServerRequestSeatChangeHook);

	if (Engine_Version < 422)
		AddHook(("BndEvt__BP_PlayButton_K2Node_ComponentBoundEvent_1_CommonButtonClicked__DelegateSignature"), PlayButtonHook);

	if (Engine_Version >= 424)
		AddHook("Function /Game/Athena/Items/Consumables/Parents/B_Prj_Athena_Consumable_Thrown.B_Prj_Athena_Consumable_Thrown_C.OnBounce", OnBounceHook);

	if (bExperimentalRespawning)
		AddHook("Function /Script/FortniteGame.FortPlayerControllerAthena.ServerClientIsReadyToRespawn", ServerClientIsReadyToRespawnHook);

	AddHook("Function /Script/FortniteGame.FortPlayerControllerAthena.ServerTeleportToPlaygroundLobbyIsland", ServerTeleportToPlaygroundLobbyIslandHook);
	AddHook("Function /Script/FortniteGame.FortPawn.NetMulticast_InvokeGameplayCueExecuted_WithParams", NetMulticast_InvokeGameplayCueExecuted_WithParamsHook);
	// AddHook("Function /Script/FortniteGame.FortPlayerControllerAthena.ServerPlaySquadQuickChatMessage", ServerPlaySquadQuickChatMessageHook);
	// AddHook("Function /Script/FortniteGame.FortWeapon.OnPawnMontageBlendingOut", onpawnmotnageblendingoutHook);

	// LoadObject<UObject>(FindObject("Class /Script/Engine.BlueprintGeneratedClass"), nullptr, "/Game/Athena/SafeZone/SafeZoneIndicator.SafeZoneIndicator_C"); // i think im slow

	AddHook("Function /Script/FortniteGame.FortGameModeAthena.OnAircraftExitedDropZone", OnAircraftExitedDropZoneHook);
	AddHook(("Function /Script/FortniteGame.BuildingActor.OnDeathServer"), OnDeathServerHook);
	AddHook(("Function /Script/Engine.GameMode.ReadyToStartMatch"), ReadyToStartMatchHook);

	if (bAutomaticPawnSpawning)
	{
		AddHook("Function /Script/Engine.GameModeBase.MustSpectate", MustSpectateHook);
		AddHook("Function /Script/Engine.GameModeBase.PlayerCanRestart", PlayerCanRestartHook);
		AddHook("Function /Script/Engine.GameModeBase.GetDefaultPawnClassForController", pawnClassHook);

		// ^^ pretyt sure these are useless

		AddHook("Function /Script/Engine.GameModeBase.SpawnDefaultPawnFor", SpawnDefaultPawnForHook);
	}

	// AddHook("Function /Script/Engine.Actor.ReceiveActorEndOverlap", ReceiveActorEndOverlapHook);

	// AddHook("Function /Script/FortniteGame.FortPlayerPawn.ServerUpdateVehicleInputStateReliable", ServerUpdateVehicleInputStateReliableHook);

	if (Engine_Version > 424)
	{
		// AddHook(("Function /Game/Athena/Items/Consumables/Parents/GA_Athena_Consumable_ThrowWithTrajectory_Parent.GA_Athena_Consumable_ThrowWithTrajectory_Parent_C.Server_SpawnProjectile"), throwableConsumablesHook); // wrong func
	}
	
	// AddHook("Function /Game/Athena/Items/Consumables/Grenade/GA_Athena_Grenade_WithTrajectory.GA_Athena_Grenade_WithTrajectory_C.Server_SpawnProjectile", boomboxHook);
	// AddHook("Function /Game/Athena/Items/Consumables/TowerGrenade/GA_Athena_TowerGrenadeWithTrajectory.GA_Athena_TowerGrenadeWithTrajectory_C.Server_SpawnProjectile", Server_SpawnProjectileHook);
	// AddHook("Function /Game/Athena/Items/Consumables/Balloons/GA_Athena_Balloons_Consumable_Passive.GA_Athena_Balloons_Consumable_Passive_C.K2_ActivateAbility", balloonFunHook);

	if (FnVerDouble < 9) // idk if right
		AddHook(("Function /Script/FortniteGame.FortPlayerControllerAthena.ServerAttemptAircraftJump"), ServerAttemptAircraftJumpHook);
	else if (Engine_Version < 424)
		AddHook(("Function /Script/FortniteGame.FortPlayerController.ServerAttemptAircraftJump"), ServerAttemptAircraftJumpHook);
	else
		AddHook(("Function /Script/FortniteGame.FortControllerComponent_Aircraft.ServerAttemptAircraftJump"), ServerAttemptAircraftJumpHook);

	// AddHook(("Function /Script/FortniteGame.FortPlayerController.ServerCheat"), ServerCheatHook); // Commands Hook
	// AddHook(("Function /Script/FortniteGame.FortPlayerController.ServerClientPawnLoaded"), ServerClientPawnLoadedHook);

	AddHook(("Function /Script/FortniteGame.FortPlayerControllerZone.ClientOnPawnDied"), ClientOnPawnDiedHook);
	// AddHook("Function /Script/FortniteGame.FortAthenaDoghouseVehicle.ServerUpdateControlState", UpdateControlStateHook);
	// AddHook(("Function /Script/FortniteGame.FortPlayerPawn.ServerSendZiplineState"), ServerSendZiplineStateHook);
	AddHook("Function /Script/Engine.PlayerController.ClientWasKicked", ClientWasKickedHook);
	// AddHook(("Function /Script/FortniteGame.FortPlayerPawn.ServerUpdateVehicleInputStateUnreliable"), ServerUpdateVehicleInputStateUnreliableHook);

	if (Engine_Version >= 420 && Engine_Version < 423)
	{
		AddHook(("Function /Script/FortniteGame.FortAthenaVehicle.ServerUpdatePhysicsParams"), ServerUpdatePhysicsParamsHook);
	}

	// if (PlayMontage)
	// if (bEmotingEnabled)
	AddHook(("Function /Script/FortniteGame.FortPlayerController.ServerPlayEmoteItem"), ServerPlayEmoteItemHook);

	if (Engine_Version < 423)
	{ // ??? Idk why we need the brackets
		AddHook(("Function /Script/FortniteGame.FortPlayerController.ServerAttemptInteract"), ServerAttemptInteractHook);
	}
	else
		AddHook(("Function /Script/FortniteGame.FortControllerComponent_Interaction.ServerAttemptInteract"), ServerAttemptInteractHook);

	AddHook(("Function /Script/FortniteGame.FortPlayerControllerZone.ServerAttemptExitVehicle"), ServerAttemptExitVehicleHook);

	AddHook(("Function /Script/FortniteGame.FortPlayerPawn.ServerChoosePart"), ServerChoosePartHook);
	AddHook("Function /Script/FortniteGame.FortPlayerPawn.ServerReviveFromDBNO", ServerReviveFromDBNOHook);

	for (auto& Func : FunctionsToHook)
	{
		if (!Func.first)
			std::cout << ("Detected null UFunction!\n");
	}

	std::cout << std::format("Hooked {} UFunctions!\n", std::to_string(FunctionsToHook.size()));
}

void* ProcessEventDetour(UObject* Object, UFunction* Function, void* Parameters)
{
	if (Object && Function)
	{
		if (bStarted && bListening && (bLogRpcs || bLogProcessEvent))
		{
			auto FunctionName = Function->GetFullName();
			// if (Function->FunctionFlags & 0x00200000 || Function->FunctionFlags & 0x01000000) // && FunctionName.find("Ack") == -1 && FunctionName.find("AdjustPos") == -1))

			if (bLogRpcs && (FunctionName.starts_with(("Server")) || FunctionName.starts_with(("Client")) || FunctionName.starts_with(("OnRep_"))))
			{
				/* if (!FunctionName.contains("ServerUpdateCamera") && !FunctionName.contains("ServerMove")
					&& !FunctionName.contains(("ServerUpdateLevelVisibility"))
					&& !FunctionName.contains(("AckGoodMove"))) */
				{
					std::cout << ("RPC Called: ") << FunctionName << '\n';
				}
			}

			if (bLogProcessEvent)
			{
				if (!strstr(FunctionName.c_str(), ("EvaluateGraphExposedInputs")) &&
					!strstr(FunctionName.c_str(), ("Tick")) &&
					!strstr(FunctionName.c_str(), ("OnSubmixEnvelope")) &&
					!strstr(FunctionName.c_str(), ("OnSubmixSpectralAnalysis")) &&
					!strstr(FunctionName.c_str(), ("OnMouse")) &&
					!strstr(FunctionName.c_str(), ("Pulse")) &&
					!strstr(FunctionName.c_str(), ("BlueprintUpdateAnimation")) &&
					!strstr(FunctionName.c_str(), ("BlueprintPostEvaluateAnimation")) &&
					!strstr(FunctionName.c_str(), ("BlueprintModifyCamera")) &&
					!strstr(FunctionName.c_str(), ("BlueprintModifyPostProcess")) &&
					!strstr(FunctionName.c_str(), ("Loop Animation Curve")) &&
					!strstr(FunctionName.c_str(), ("UpdateTime")) &&
					!strstr(FunctionName.c_str(), ("GetMutatorByClass")) &&
					!strstr(FunctionName.c_str(), ("UpdatePreviousPositionAndVelocity")) &&
					!strstr(FunctionName.c_str(), ("IsCachedIsProjectileWeapon")) &&
					!strstr(FunctionName.c_str(), ("LockOn")) &&
					!strstr(FunctionName.c_str(), ("GetAbilityTargetingLevel")) &&
					!strstr(FunctionName.c_str(), ("ReadyToEndMatch")) &&
					!strstr(FunctionName.c_str(), ("ReceiveDrawHUD")) &&
					!strstr(FunctionName.c_str(), ("OnUpdateDirectionalLightForTimeOfDay")) &&
					!strstr(FunctionName.c_str(), ("GetSubtitleVisibility")) &&
					!strstr(FunctionName.c_str(), ("GetValue")) &&
					!strstr(FunctionName.c_str(), ("InputAxisKeyEvent")) &&
					!strstr(FunctionName.c_str(), ("ServerTouchActiveTime")) &&
					!strstr(FunctionName.c_str(), ("SM_IceCube_Blueprint_C")) &&
					!strstr(FunctionName.c_str(), ("OnHovered")) &&
					!strstr(FunctionName.c_str(), ("OnCurrentTextStyleChanged")) &&
					!strstr(FunctionName.c_str(), ("OnButtonHovered")) &&
					!strstr(FunctionName.c_str(), ("ExecuteUbergraph_ThreatPostProcessManagerAndParticleBlueprint")) &&
					!strstr(FunctionName.c_str(), ("UpdateCamera")) &&
					!strstr(FunctionName.c_str(), ("GetMutatorContext")) &&
					!strstr(FunctionName.c_str(), ("CanJumpInternal")) && 
					!strstr(FunctionName.c_str(), ("OnDayPhaseChanged")) &&
					!strstr(FunctionName.c_str(), ("Chime")) && 
					!strstr(FunctionName.c_str(), ("ServerMove")) &&
					!strstr(FunctionName.c_str(), ("OnVisibilitySetEvent")) &&
					!strstr(FunctionName.c_str(), "ReceiveHit") &&
					!strstr(FunctionName.c_str(), "ReadyToStartMatch") && 
					!strstr(FunctionName.c_str(), "ClientAckGoodMove") &&
					!strstr(FunctionName.c_str(), "Prop_WildWest_WoodenWindmill_01") &&
					!strstr(FunctionName.c_str(), "ContrailCheck") &&
					!strstr(FunctionName.c_str(), "B_StockBattleBus_C") &&
					!strstr(FunctionName.c_str(), "Subtitles.Subtitles_C.") &&
					!strstr(FunctionName.c_str(), "/PinkOatmeal/PinkOatmeal_") &&
					!strstr(FunctionName.c_str(), "BP_SpectatorPawn_C") &&
					!strstr(FunctionName.c_str(), "FastSharedReplication") &&
					!strstr(FunctionName.c_str(), "OnCollisionHitEffects") &&
					!strstr(FunctionName.c_str(), "BndEvt__SkeletalMesh") &&
					!strstr(FunctionName.c_str(), ".FortAnimInstance.AnimNotify_") &&
					!strstr(FunctionName.c_str(), "OnBounceAnimationUpdate") &&
					!strstr(FunctionName.c_str(), "ShouldShowSoundIndicator") &&
					!strstr(FunctionName.c_str(), "Primitive_Structure_AmbAudioComponent_C") &&
					!strstr(FunctionName.c_str(), "PlayStoppedIdleRotationAudio") &&
					!strstr(FunctionName.c_str(), "UpdateOverheatCosmetics") &&
					!strstr(FunctionName.c_str(), "StormFadeTimeline__UpdateFunc") &&
					!strstr(FunctionName.c_str(), "BindVolumeEvents") &&
					!strstr(FunctionName.c_str(), "UpdateStateEvent") &&
					!strstr(FunctionName.c_str(), "VISUALS__UpdateFunc") &&
					!strstr(FunctionName.c_str(), "Flash__UpdateFunc") &&
					!strstr(FunctionName.c_str(), "SetCollisionEnabled") &&
					!strstr(FunctionName.c_str(), "SetIntensity") &&
					!strstr(FunctionName.c_str(), "Storm__UpdateFunc") &&
					!strstr(FunctionName.c_str(), "CloudsTimeline__UpdateFunc") &&
					!strstr(FunctionName.c_str(), "SetRenderCustomDepth") &&
					!strstr(FunctionName.c_str(), "K2_UpdateCustomMovement") &&
					!strstr(FunctionName.c_str(), "AthenaHitPointBar_C.Update") &&
					!strstr(FunctionName.c_str(), "ExecuteUbergraph_Farm_WeatherVane_01") &&
					!strstr(FunctionName.c_str(), "HandleOnHUDElementVisibilityChanged"))
				{
					std::cout << ("Function called: ") << FunctionName << '\n';
				}
			}
		}

		for (auto& Func : FunctionsToHook)
		{
			if (Function == Func.first)
			{
				if (Func.second(Object, Function, Parameters)) // If the function returned true, then cancel default execution.
				{
					// std::cout << "ret true!\n";
					return 0;
				}
			}
		}
	}

	return ProcessEventO(Object, Function, Parameters);
}

__int64 __fastcall FixCrashDetour(int32_t* PossiblyNull, __int64 a2, int* a3)
{
	if (!PossiblyNull)
	{
		std::cout << "Prevented Crash!\n";
		return 0;
	}

	return FixCrash(PossiblyNull, a2, a3);
}

void __fastcall GetPlayerViewPointDetour(UObject* pc, FVector* a2, FRotator* a3)
{
	if (pc)
	{
		static auto fn = FindObject(("Function /Script/Engine.Controller.GetViewTarget"));
		UObject* TheViewTarget = nullptr; // *pc->Member<UObject*>(("Pawn"));
		pc->ProcessEvent(fn, &TheViewTarget);

		if (TheViewTarget)
		{
			if (a2)
			{
				auto Loc = Helper::GetActorLocation(TheViewTarget);
				*a2 = Loc;

				// if (bTraveled)
					// std::cout << std::format("X: {} Y: {} Z {}\n", Loc.X, Loc.Y, Loc.Z);
			}
			if (a3)
			{
				auto Rot = Helper::GetActorRotation(TheViewTarget);
				*a3 = Rot;
			}

			return;
		}
		// else
			// std::cout << ("unable to get viewpoint!\n"); // This will happen if someone leaves iirc
	}

	return GetPlayerViewPoint(pc, a2, a3);
}

__int64(__fastcall* idkbroke)(UObject* a1);

__int64 (__fastcall* ehehheO)(__int64 a1, __int64* a2);

__int64 __fastcall ehehheDetour(__int64 NetViewer, UObject* Connection)
{
	static auto Connection_ViewTargetOffset = GetOffset(Connection, "ViewTarget");
	static auto Connection_PlayerControllerOffset = GetOffset(Connection, "PlayerController");
	static auto Connection_OwningActorOffset = GetOffset(Connection, "OwningActor");

	auto Connection_ViewTarget = *(UObject**)(__int64(Connection) + Connection_ViewTargetOffset);
	auto Connection_PlayerController = *(UObject**)(__int64(Connection) + Connection_PlayerControllerOffset);

	static auto Viewer_ConnectionOffset = FindOffsetStruct("ScriptStruct /Script/Engine.NetViewer", "Connection");
	*(UObject**)(__int64(NetViewer) + Viewer_ConnectionOffset) = Connection;

	static auto Viewer_InViewerOffset = FindOffsetStruct("ScriptStruct /Script/Engine.NetViewer", "InViewer");
	*(UObject**)(__int64(NetViewer) + Viewer_InViewerOffset) = Connection_PlayerController ? Connection_PlayerController : *(UObject**)(__int64(Connection) + Connection_OwningActorOffset);

	static auto Viewer_ViewTargetOffset = FindOffsetStruct("ScriptStruct /Script/Engine.NetViewer", "ViewTarget");
	auto Viewer_ViewTarget = (UObject**)(__int64(NetViewer) + Viewer_ViewTargetOffset);
	*Viewer_ViewTarget = Connection_ViewTarget;

	static auto Viewer_ViewLocationOffset = FindOffsetStruct("ScriptStruct /Script/Engine.NetViewer", "ViewLocation");
	auto Viewer_ViewLocation = (FVector*)(__int64(NetViewer) + Viewer_ViewLocationOffset);

	if (*Viewer_ViewTarget)
		*(FVector*)(__int64(NetViewer) + Viewer_ViewLocationOffset) = Helper::GetActorLocation(*Viewer_ViewTarget);

	float CP, SP, CY, SY;

	FRotator ViewRotation = (*Viewer_ViewTarget) ? Helper::GetActorRotation(*Viewer_ViewTarget) : FRotator();

	SinCos(&SP, &CP, DegreesToRadians(ViewRotation.Pitch));
	SinCos(&SY, &CY, DegreesToRadians(ViewRotation.Yaw));

	static auto Viewer_ViewDirOffset = FindOffsetStruct("ScriptStruct /Script/Engine.NetViewer", "ViewDir");

	*(FVector*)(__int64(NetViewer) + Viewer_ViewDirOffset) = FVector(CP * CY, CP * SY, SP);

	return NetViewer;

	// return ehehheO(a1, a2);
}

static UObject* SpawnActorDetour(UObject* World, UObject* Class, FVector* Position, FRotator* Rotation, const FActorSpawnParameters& SpawnParameters)
{
	static auto BuildingSMActorClass = FindObject("Class /Script/FortniteGame.BuildingSMActor");

	auto SuperStructOfClass = GetSuperStructOfClass(Class);

	if (!NoMcpAddr && FnVerDouble >= 6 && FnVerDouble < 10 && Class->GetFullName().contains("Glider"))
	{
		return nullptr;
	}

	if (bPrintSpawnActor)
	{
		std::cout << "class fullanem: " << Class->GetFullName() << '\n';
		// std::cout << "SuperStructOfClass ful name: " << SuperStructOfClass->GetFullName() << '\n';
	}

	// if (bDoubleBuildFix2 && (SuperStructOfClass == BuildingSMActorClass || GetSuperStructOfClass(SuperStructOfClass) == BuildingSMActorClass))

	return SpawnActorO(World, Class, Position, Rotation, SpawnParameters);
}

char __fastcall StupidMfDetour(__int64 a1, int* a2, __int64 a3)
{
	return 1;
}

void InitializeHooks()
{
	MH_CreateHook((PVOID)ProcessEventAddr, ProcessEventDetour, (void**)&ProcessEventO);
	MH_EnableHook((PVOID)ProcessEventAddr);

	if (Engine_Version >= 423)
	{
		MH_CreateHook((PVOID)FixCrashAddr, FixCrashDetour, (void**)&FixCrash);
		MH_EnableHook((PVOID)FixCrashAddr);
	}

	if (FnVerDouble < 17.00)
	{
		auto sig = FindPattern("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 40 48 89 11 48 8B D9 48 8B 42 30 48 85 C0 75 07 48 8B 82 ? ? ? ? 48");

		if (!sig)
			sig = FindPattern("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 40 48 89 11");

		bool bNewFlashingFix = false;

		if (sig) //Engine_Version == 423)
		{
			// fixes flashing

			MH_CreateHook((PVOID)sig, ehehheDetour, (void**)&ehehheO);
			MH_EnableHook((PVOID)sig);
		}
		else
			std::cout << ("[WARNING] Could not fix flashing!\n");
	}
	else
	{
		MH_CreateHook((PVOID)GetPlayerViewpointAddr, GetPlayerViewPointDetour, (void**)&GetPlayerViewPoint);
		MH_EnableHook((PVOID)GetPlayerViewpointAddr);
	}

	if (LP_SpawnPlayActorAddr && !bSpawnAPC) // bad time but eh
	{
		MH_CreateHook((PVOID)LP_SpawnPlayActorAddr, LP_SpawnPlayActorDetour, (void**)&LP_SpawnPlayActor);
		MH_EnableHook((PVOID)LP_SpawnPlayActorAddr);
	}
}
