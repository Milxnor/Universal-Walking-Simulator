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
#include <Gameplay/player.h>

#define LOGGING

// HEAVILY INSPIRED BY KEMOS UFUNCTION HOOKING

static bool bStarted = false;
static bool bLogRpcs = false;
static bool bLogProcessEvent = false;

inline void initStuff()
{
	if (!bStarted && bTraveled)
	{
		bStarted = true;

		CreateThread(0, 0, Helper::Console::Setup, 0, 0, 0);

		auto world = Helper::GetWorld();
		auto gameState = *world->Member<UObject*>(("GameState"));

		if (gameState)
		{
			auto AuthGameMode = *world->Member<UObject*>(("AuthorityGameMode"));

			*(*AuthGameMode->Member<UObject*>(("GameSession")))->Member<int>(("MaxPlayers")) = 100; // GameState->GetMaxPlaylistPlayers()

			if (std::stod(FN_Version) >= 8 && AuthGameMode)
			{
				static auto PlayerControllerClass = FindObject(("BlueprintGeneratedClass /Game/Athena/Athena_PlayerController.Athena_PlayerController_C"));
				std::cout << ("PlayerControllerClass: ") << PlayerControllerClass << '\n';
				*AuthGameMode->Member<UObject*>(("PlayerControllerClass")) = PlayerControllerClass;
			}

			auto WillSkipAircraft = gameState->Member<char>(("bGameModeWillSkipAircraft"));

			if (!WillSkipAircraft) // we are in like frontend or some gamestate that is not athena
				return;

			*WillSkipAircraft = false;
			*gameState->Member<float>(("AircraftStartTime")) = 99999.0f;
			*gameState->Member<float>(("WarmupCountdownEndTime")) = 99999.0f;

			EFriendlyFireType* FFT = gameState->Member<EFriendlyFireType>(("FriendlyFireType"));
			//Super scuffed way of checking if its valid.
			if (FFT && __int64(FFT) != __int64(gameState)) {
				*gameState->Member<EFriendlyFireType>(("FriendlyFireType")) = EFriendlyFireType::On;
			}
			else {
				std::cout << "FriendlyFireType is not valid!\n";
			}

			if (Engine_Version >= 420)
			{
				*gameState->Member<EAthenaGamePhase>(("GamePhase")) = EAthenaGamePhase::Warmup;

				struct {
					EAthenaGamePhase OldPhase;
				} params2{ EAthenaGamePhase::None };

				static const auto fnGamephase = gameState->Function(("OnRep_GamePhase"));

				if (fnGamephase)
					gameState->ProcessEvent(fnGamephase, &params2);
			}

			if (AuthGameMode)
			{
				AuthGameMode->ProcessEvent(AuthGameMode->Function(("StartPlay")), nullptr);

				auto FNVer = std::stod(FN_Version);

				if (std::floor(FNVer) == 3 || FNVer >= 8.0) 
				{
					//If This is called on Seasons 4, 6, or 7 then Setting The Playlist Crashes.
					AuthGameMode->ProcessEvent(AuthGameMode->Function(("StartMatch")), nullptr);
				}

				//auto BotManagerClass = FindObject("FortServerBotManagerAthena /Script/FortniteGame.Default__FortServerBotManagerAthena");

				//*AuthGameMode->Member<UObject*>("ServerBotManagerClass") = BotManagerClass;
				// *AuthGameMode->Member<bool>(("bAlwaysDBNO")) = true;

				// Is this correct?
				/*class UClass* VehicleClass = (UClass*)(FindObject(("Class /Script/FortniteGame.FortAthenaFerretVehicle")));//(UClass*(FindObject(("Class /Script/FortniteGame.FortAthenaFerretVehicle.FortAthenaFerretVehicle"))));
				struct 
				{
					class UClass* VehicleClass;
				} SSVParams{ VehicleClass };
				
				auto SSVFn = *AuthGameMode->Member<bool>(("ShouldSpawnVehicle")) = true;*/
				
				// static auto Playlist = FindObject(("FortPlaylistAthena /Game/Athena/Playlists/Playlist_DefaultSolo.Playlist_DefaultSolo"));
				static auto Playlist = FindObject(("FortPlaylistAthena /Game/Athena/Playlists/Playlist_DefaultDuo.Playlist_DefaultDuo"));
				// static auto Playlist = FindObject(("FortPlaylistAthena /Game/Athena/Playlists/Playlist_DefaultSquad.Playlist_DefaultSquad"));
				// static auto Playlist = FindObject(("FortPlaylistAthena /Game/Athena/Playlists/Playground/Playlist_Playground.Playlist_Playground"));
				// static auto Playlist = FindObject(("FortPlaylistAthena /Game/Athena/Playlists/Carmine/Playlist_Carmine.Playlist_Carmine"));
				// static auto Playlist = FindObject(("/Game/Athena/Playlists/Fill/Playlist_Fill_Solo.Playlist_Fill_Solo"));
				 
				if (std::stod(FN_Version) >= 6.10) // WRONG
				{
					auto OnRepPlaylist = gameState->Function(("OnRep_CurrentPlaylistInfo"));

					static auto BasePlaylistOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.PlaylistPropertyArray"), ("BasePlaylist"));
					static auto PlaylistReplicationKeyOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.PlaylistPropertyArray"), ("PlaylistReplicationKey"));

					if (BasePlaylistOffset && OnRepPlaylist && Playlist)
					{
						static auto PlaylistInfo = gameState->Member<UScriptStruct>(("CurrentPlaylistInfo"));

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
			}
			else
			{
				std::cout << dye::yellow(("[WARNING] ")) << ("Failed to find AuthorityGameMode!\n");
			}

			if (Engine_Version != 421)
			{
				auto PlayersLeft = gameState->Member<int>(("PlayersLeft"));

				if (PlayersLeft && *PlayersLeft)
					*PlayersLeft = 0;

				static auto OnRep_PlayersLeft = gameState->Function(("OnRep_PlayersLeft"));

				if (OnRep_PlayersLeft)
					gameState->ProcessEvent(OnRep_PlayersLeft);
			}
		}

		Listen(7777);
		// CreateThread(0, 0, MapLoadThread, 0, 0, 0);

		InitializeNetHooks();

		std::cout << ("Initialized NetHooks!\n");

		if (Engine_Version >= 420) {
			Events::LoadEvents();
			Helper::FixPOIs();
		}
		 
		LootingV2::InitializeWeapons(nullptr);

		/* auto bUseDistanceBasedRelevancy = (*world->Member<UObject*>("NetworkManager"))->Member<bool>("bUseDistanceBasedRelevancy");
		std::cout << "bUseDistanceBasedRelevancy: " << *bUseDistanceBasedRelevancy << '\n';
		*bUseDistanceBasedRelevancy = !(*bUseDistanceBasedRelevancy); */

		//CreateThread(0, 0, LootingV2::InitializeWeapons, 0, 0, 0); // doesnt work??

		// GAMEMODE STUFF HERE

		if (Helper::IsRespawnEnabled())
		{
			// Enable glider redeploy (idk if this works)
			FString GliderRedeployCmd;
			GliderRedeployCmd.Set(L"Athena.EnableParachuteEverywhere 1");
			Helper::Console::ExecuteConsoleCommand(GliderRedeployCmd);
		}
	}
}

bool ServerSendSquadFriendHook(UObject* Controller, UFunction*, void* Parameters)
{
	struct parms { UObject* Friend; }; // playerstate

	auto Params = (parms*)Parameters;

	if (Controller && Params)
	{
		if (Params->Friend)
		{
			std::cout << "Valid Friend!\n";
			std::wcout << Helper::GetfPlayerName(Params->Friend).Data.GetData() << '\n';
		}
		else
			std::cout << "No Friend!\n";
	}

	return false;
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

bool OnSafeZoneStateChangeHook(UObject* Indicator, UFunction*, void* Parameters)
{
	std::cout << "OnSafeZoneStateChange!\n";

	if (Indicator && Helper::IsSmallZoneEnabled() && false)
	{
		static auto GameState = *Helper::GetWorld()->Member<UObject*>(("GameState"));

		if (!GameState)
			return false;

		auto Aircraft = GameState->Member<TArray<UObject*>>(("Aircrafts"))->At(0);

		if (!Aircraft)
			return false;

		*Indicator->Member<FVector>("NextCenter") = Helper::GetActorLocation(Aircraft);

		*Indicator->Member<float>("Radius") = 15000;
	}

	return true;
}

bool ServerLoadingScreenDroppedHook(UObject* PlayerController, UFunction* Function, void* Parameters)
{
	auto PlayerState = *PlayerController->Member<UObject*>(("PlayerState"));
	auto Pawn = *PlayerController->Member<UObject*>(("Pawn"));

	return false;
}

bool ServerUpdatePhysicsParamsHook(UObject* Vehicle, UFunction* Function, void* Parameters) // FortAthenaVehicle
{
	if (Vehicle && Parameters)
	{
		struct parms { __int64 InState; };
		auto Params = (parms*)Parameters;

		static auto TranslationOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.ReplicatedAthenaVehiclePhysicsState"), ("Translation"));
		auto Translation = (FVector*)(__int64(&Params->InState) + TranslationOffset);

		std::cout << ("TranslationOffset: ") << TranslationOffset << '\n';

		static auto RotationOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.ReplicatedAthenaVehiclePhysicsState"), ("Rotation"));
		auto Rotation = (FQuat*)(__int64(&Params->InState) + RotationOffset);

		static auto LinearVelocityOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.ReplicatedAthenaVehiclePhysicsState"), ("LinearVelocity"));
		auto LinearVelocity = (FVector*)(__int64(&Params->InState) + LinearVelocityOffset);

		static auto AngularVelocityOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.ReplicatedAthenaVehiclePhysicsState"), ("AngularVelocity"));
		auto AngularVelocity = (FVector*)(__int64(&Params->InState) + AngularVelocityOffset);

		if (Translation && Rotation)
		{
			std::cout << ("X: ") << Translation->X << '\n';
			std::cout << ("Y: ") << Translation->Y << '\n';
			std::cout << ("Z: ") << Translation->Z << '\n';

			// Helper::SetActorLocation(Vehicle, *Translation);
			auto rot = Rotation->Rotator();

			// rot.Pitch = 0;

			std::cout << ("Pitch: ") << rot.Pitch << '\n';
			std::cout << ("Yaw: ") << rot.Yaw << '\n';
			std::cout << ("Roll: ") << rot.Roll << '\n';

			auto OGRot = Helper::GetActorRotation(Vehicle);

			Helper::SetActorLocation(Vehicle, *Translation);

			UObject* RootComp = nullptr;
			static auto GetRootCompFunc = Vehicle->Function("K2_GetRootComponent");

			if (GetRootCompFunc)
				Vehicle->ProcessEvent(GetRootCompFunc, &RootComp);

			if (RootComp)
			{
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

			return true;
		}
	}

	return false;
}

bool ServerAttemptAircraftJumpHook(UObject* PlayerController, UFunction* Function, void* Parameters)
{
	struct Param{
		FRotator                                    ClientRotation;
	};

	auto Params = (Param*)Parameters;

	if (PlayerController && Params) // PlayerController->IsInAircraft()
	{
		static auto world = Helper::GetWorld();
		static auto GameState = *world->Member<UObject*>(("GameState"));

		if (GameState)
		{
			auto Aircraft = GameState->Member<TArray<UObject*>>(("Aircrafts"))->At(0);

			if (Aircraft)
			{
				auto ExitLocation = Helper::GetActorLocation(Aircraft);

				Helper::InitPawn(PlayerController, false, ExitLocation);

				// ClientSetRotation
			}
		}
	}

	return false;
}

bool ReadyToStartMatchHook(UObject* Object, UFunction* Function, void* Parameters)
{
	initStuff();
	return false;
}

void LoadInMatch()
{
	auto GameInstance = *GetEngine()->Member<UObject*>(("GameInstance"));
	auto& LocalPlayers = *GameInstance->Member<TArray<UObject*>>(("LocalPlayers"));
	auto PlayerController = *LocalPlayers.At(0)->Member<UObject*>(("PlayerController"));

	if (PlayerController)
	{
		static auto SwitchLevelFn = PlayerController->Function(("SwitchLevel"));
		FString Map;
		Map.Set(GetMapName());
		PlayerController->ProcessEvent(SwitchLevelFn, &Map);
		// Map.Free();
		bTraveled = true;
	}
	else
	{
		std::cout << dye::red(("[ERROR] ")) << ("Unable to find PlayerController!\n");
	}
}
// HEAVILY INSPIRED BY RAIDER 
inline bool ServerCheatHook(UObject* Controller, UFunction* Function, void* Parameters)
{
	struct SCParams { FString* Msg; };
	auto MsgParams = (SCParams*)Parameters;

	if (Controller && MsgParams && MsgParams->Msg)
	{
		auto Message = MsgParams->Msg->ToString() + ' ';
		std::vector<std::string> Args;
		while (Message.find(" ") != -1)
		{
			Args.push_back(Message.substr(0, Message.find(' ')));
			Message.erase(0, Message.find(' ') + 1);
		}
		auto ArgsNum = Args.size() - 1;

		if (ArgsNum >= 0)
		{
			auto& Command = Args[0];
			std::transform(Command.begin(), Command.end(), Command.begin(), ::tolower);

			if (Command == "giveitem" && ArgsNum >= 1)
			{
				auto& weaponName = Args[1];
				int slot = 1;
				int count = 1;
				try
				{
					if (ArgsNum >= 2)
						slot = std::stoi(Args[2]);

					if (ArgsNum >= 3)
						count = std::stoi(Args[3]);
				}
				catch (...)
				{
				}
				auto WID = FindObject("FortWeaponRangedItemDefinition " + weaponName + '.' + weaponName);
				if (WID)
				{
					Inventory::CreateAndAddItem(Controller, WID, EFortQuickBars::Primary, slot, count);
					std::cout << "Gave weapon!\n";
				}
			}
			// Should work i think
			else if (Command == "launchplayer" && ArgsNum >= 0)
			{
				auto Pawn = *Controller->Member<UObject*>("Pawn");
				FVector LaunchVelocity{ 0, 0, 5000 };
				bool bXYOveride = false;
				bool bZOverride = false;
				bool bIgnoreFallDamage = true;
				bool bPlayFeedbackEvent = false;
				
				Helper::LaunchPlayer(Pawn, LaunchVelocity, bXYOveride = false, bZOverride = false, bIgnoreFallDamage = true, bPlayFeedbackEvent = false);
			}
			else if (Command == "revive")
			{
				Controller->ProcessEvent(("Suicide"));
				/* auto Pawn = Controller->Member<UObject>("Pawn");
				bool bIsDBNO = Pawn->Member<UObject>("bIsDBNO");
				bool bIsDBNO = false;
				auto OnRep_DBNOFn = Pawn->Function("OnRep_IsDBNO");
				Pawn->ProcessEvent(OnRep_DBNOFn);
				float NewHealth = 100.0f;
				struct 
				{
					float NewHealth;
				} SHParams{ NewHealth };
				auto SHFn = Pawn->Function("SetHealth");
				Pawn->ProcessEvent(SHFn, &SHParams); */
			}
		}
	}
	return true;
}

uint8_t GetDeathCause(FGameplayTagContainer Tags) // Credits: Pakchunk on github, from raider3.5
{
	static std::map<std::string, EDeathCause> DeathCauses{
		{ "weapon.ranged.shotgun", EDeathCause::Shotgun },
		{ "weapon.ranged.assault", EDeathCause::Rifle },
		{ "Gameplay.Damage.Environment.Falling", EDeathCause::FallDamage },
		{ "weapon.ranged.sniper", EDeathCause::Sniper },
		{ "Weapon.Ranged.SMG", EDeathCause::SMG },
		{ "weapon.ranged.heavy.rocket_launcher", EDeathCause::RocketLauncher },
		{ "weapon.ranged.heavy.grenade_launcher", EDeathCause::GrenadeLauncher },
		{ "Weapon.ranged.heavy.grenade", EDeathCause::Grenade },
		{ "Weapon.Ranged.Heavy.Minigun", EDeathCause::Minigun },
		{ "Weapon.Ranged.Crossbow", EDeathCause::Bow },
		{ "trap.floor", EDeathCause::Trap },
		{ "weapon.ranged.pistol", EDeathCause::Pistol },
		{ "Gameplay.Damage.OutsideSafeZone", EDeathCause::OutsideSafeZone },
		{ "Weapon.Melee.Impact.Pickaxe", EDeathCause::Melee }
	};

	static auto DeathEnum = FindObject(("Enum /Script/FortniteGame.EDeathCause"));
	static uint8_t SMGnum = GetEnumValue(DeathEnum, ("SMG"));

	std::cout << "SMGnum: " << (int)SMGnum << '\n';

	return SMGnum;

	for (int i = 0; i < Tags.GameplayTags.Num(); i++)
	{
		auto TagName = Tags.GameplayTags.At(i).TagName;

		if (TagName.ComparisonIndex == 0 || TagName.Number == 0)
			continue;

		for (auto& Map : DeathCauses)
		{
			if (TagName.ToString() == Map.first) return (uint8_t)Map.second;
			else continue;
		}
	}

	std::cout << std::format("Unspecified Death: {}\n", Tags.ToStringSimple(false));

	return (uint8_t)EDeathCause::Unspecified;
}

void RequestExitWithStatusHook(bool Force, uint8_t ReturnCode)
{
	std::cout << std::format("Force: {} ReturnCode: {}", Force, std::to_string((int)ReturnCode));
	return;
}

DWORD WINAPI WinThread(LPVOID)
{


	return 0;
}

inline bool ClientOnPawnDiedHook(UObject* DeadPC, UFunction* Function, void* Parameters)
{
	if (DeadPC && Parameters)
	{
		auto DeadPawn = *DeadPC->Member<UObject*>(("Pawn"));
		auto DeadPlayerState = *DeadPC->Member<UObject*>(("PlayerState"));
		auto GameState = Helper::GetGameState();

		struct parms { __int64 DeathReport; };

		auto Params = (parms*)Parameters;

		static auto DeathLocationOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.DeathInfo"), ("DeathLocation"));	
		auto DeathInfo = DeadPlayerState->Member<__int64>(("DeathInfo"));

		auto DeathLocation = Helper::GetActorLocation(DeadPawn); // *(FVector*)(__int64(&*DeathInfo) + DeathLocationOffset);

		if (Helper::IsRespawnEnabled())
			Player::RespawnPlayer(DeadPC);
		else
		{
			// PlayersLeft--;

			// DeadPC->ClientSendMatchStatsForPlayer(DeadPC->GetMatchReport()->MatchStats);

			auto ItemInstances = Inventory::GetItemInstances(DeadPC);

			if (ItemInstances)
			{
				for (int i = 1; i < ItemInstances->Num(); i++)
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

						if (Pawn && Definition)
						{
							auto loc = Helper::GetActorLocation(Pawn);

							auto Pickup = Helper::SummonPickup(Pawn, Definition, DeathLocation, EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::Unset, Params->Count);
						}
					}
				}
			}

			if (Engine_Version >= 423) // wrong
			{
				auto Chip = Helper::SpawnChip(DeadPC, DeathLocation);

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

					auto PlayerState = *DeadPawn->Member<UObject*>(("PlayerState"));
					auto FortResurrectionData = PlayerState->Member<FFortResurrectionData>(("ResurrectionChipAvailable"));

					if (FortResurrectionData)
					{
						std::cout << ("FortResurrectionData valid!\n");
						std::cout << ("FortResurrectionData Location X: ") << FortResurrectionData->WorldLocation.X << '\n';
					}
					else
						std::cout << ("No FortResurrectionData!\n");

					/*
					
					[13980] FloatProperty /Script/FortniteGame.FortPlayerDeathReport.ServerTimeForResurrect
					[13981] FloatProperty /Script/FortniteGame.FortPlayerDeathReport.ServerTimeForRespawn

					*/

					auto ResurrectionData = FFortResurrectionData{};
					ResurrectionData.bResurrectionChipAvailable = true;
					ResurrectionData.ResurrectionExpirationLength = 9999.f;
					ResurrectionData.ResurrectionExpirationTime = 9999.f;
					ResurrectionData.WorldLocation = Helper::GetActorLocation(Chip);

					if (FortResurrectionData)
						*FortResurrectionData = ResurrectionData;

					std::cout << ("Spawned Chip!\n");
				}
			}

			auto PlayersLeft = GameState->Member<int>(("PlayersLeft"));

			(*PlayersLeft)--;

			if (*PlayersLeft == 1)
			{
				// win
				// CreateThread(0, 0, WinThread, 0, 0, 0);


			}
		}

		static auto KillerPawnOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortPlayerDeathReport"), ("KillerPawn"));
		static auto KillerPlayerStateOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortPlayerDeathReport"), ("KillerPlayerState"));

		auto KillerPawn = *(UObject**)(__int64(&Params->DeathReport) + KillerPawnOffset);
		auto KillerPlayerState = *(UObject**)(__int64(&Params->DeathReport) + KillerPlayerStateOffset);

		if (false)
		{
			UObject* KillerController = nullptr;

			if (KillerPawn)
				KillerController = *KillerPawn->Member<UObject*>(("Controller"));

			if (KillerPlayerState)
			{
				// somethings skunked
				/* auto DeathInfo = DeadPlayerState->Member<__int64>(("DeathInfo"));

				static auto TagsOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.DeathInfo"), ("Tags"));
				static auto DeathCauseOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.DeathInfo"), ("DeathCause"));
				static auto FinisherOrDownerOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.DeathInfo"), ("FinisherOrDowner"));
				static auto bDBNOOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.DeathInfo"), ("bDBNO"));

				// /Script/FortniteGame.FortPlayerStateAthena.OnRep_TeamKillScore

				auto Tags = (FGameplayTagContainer*)(__int64(&*DeathInfo) + TagsOffset);

				*(uint8_t*)(__int64(&*DeathInfo) + DeathCauseOffset) = Tags ? GetDeathCause(*Tags) : (uint8_t)EDeathCause::Cube;
				*(UObject**)(__int64(&*DeathInfo) + FinisherOrDownerOffset) = KillerPlayerState ? KillerPlayerState : DeadPlayerState;
				*(bool*)(__int64(&*DeathInfo) + bDBNOOffset) = false; */

				static auto OnRep_DeathInfo = DeadPlayerState->Function(("OnRep_DeathInfo"));

				if (OnRep_DeathInfo)
					DeadPlayerState->ProcessEvent(OnRep_DeathInfo);

				if (KillerPlayerState != DeadPlayerState)
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

					// *DeadPlayerState->Member<__int64>(("DeathInfo")) = *(__int64*)malloc(GetSizeOfStruct(FindObject(("ScriptStruct /Script/FortniteGame.DeathInfo"))));

					static auto ClientReportKill = KillerPlayerState->Function(("ClientReportKill"));
					struct { UObject* PlayerState; }ClientReportKill_Params{ DeadPlayerState };
					if (ClientReportKill)
						KillerPlayerState->ProcessEvent(ClientReportKill, &ClientReportKill_Params);
				}
			}
		}

		if (KillerPlayerState && KillerPlayerState != DeadPlayerState) // make sure if they didnt die of like falling or rejoining they dont get their kill
		{
			(*KillerPlayerState->Member<int>(("KillScore")))++;

			if (Engine_Version >= 423) // idgaf wrong
				(*KillerPlayerState->Member<int>(("TeamKillScore")))++;

			static auto OnRep_Kills = KillerPlayerState->Function(("OnRep_Kills"));

			if (OnRep_Kills)
				KillerPlayerState->ProcessEvent(OnRep_Kills);
		}
	}
	
	return true;
}



inline bool ServerAttemptExitVehicleHook(UObject* Controller, UFunction* Function, void* Parameters)
{
	auto Pawn = Controller->Member<UObject*>(("Pawn"));

	if (Pawn && *Pawn)
	{
		UObject* Vehicle = Helper::GetVehicle(*Pawn);

		if (Vehicle)
		{
			Helper::SetLocalRole(*Pawn, ENetRole::ROLE_Authority);
			// Helper::SetLocalRole(Vehicle, ENetRole::ROLE_Authority);
		}
	}

	return false;
}

inline bool ServerPlayEmoteItemHook(UObject* Controller, UFunction* Function, void* Parameters)
{
	auto Pawn = *Controller->Member<UObject*>(("Pawn"));

	struct SPEIParams  { UObject* EmoteAsset; }; // UFortMontageItemDefinitionBase
	auto EmoteParams = (SPEIParams*)Parameters;

	auto EmoteAsset = EmoteParams->EmoteAsset;

	if (Controller /* && !Controller->IsInAircraft() */ && Pawn && EmoteAsset && Engine_Version < 424 && Engine_Version >= 421)
	{
		struct {
			TEnumAsByte<EFortCustomBodyType> BodyType;
			TEnumAsByte<EFortCustomGender> Gender;
			UObject* AnimMontage; // UAnimMontage
		} GAHRParams{EFortCustomBodyType::All, EFortCustomGender::Both}; // (CurrentPawn->CharacterBodyType, CurrentPawn->CharacterGender)
		static auto fn = EmoteAsset->Function(("GetAnimationHardReference"));

		if (fn)
		{
			EmoteAsset->ProcessEvent(fn, &GAHRParams);
			auto Montage = GAHRParams.AnimMontage;
			if (Montage && Engine_Version < 426)
			{
				std::cout << ("Playing Montage: ") << Montage->GetFullName() << '\n';

				auto AbilitySystemComponent = *Pawn->Member<UObject*>(("AbilitySystemComponent"));
				static auto EmoteClass = FindObject(("BlueprintGeneratedClass /Game/Abilities/Emotes/GAB_Emote_Generic.GAB_Emote_Generic_C"));

				TArray<FGameplayAbilitySpec<FGameplayAbilityActivationInfo>> Specs;

				if (Engine_Version <= 422)
					Specs = (*AbilitySystemComponent->Member<FGameplayAbilitySpecContainerOL>(("ActivatableAbilities"))).Items;
				else
					Specs = (*AbilitySystemComponent->Member<FGameplayAbilitySpecContainerSE>(("ActivatableAbilities"))).Items;

				UObject* DefaultObject = EmoteClass->CreateDefaultObject();

				for (int i = 0; i < Specs.Num(); i++)
				{
					auto& CurrentSpec = Specs[i];

					if (CurrentSpec.Ability == DefaultObject)
					{
						auto EmoteAbility = CurrentSpec.Ability;

						*EmoteAbility->Member<UObject*>("PlayerPawn") = Pawn;

						auto ActivationInfo = EmoteAbility->Member<FGameplayAbilityActivationInfo>(("CurrentActivationInfo"));

						// Helper::SetLocalRole(Pawn, ENetRole::ROLE_SimulatedProxy);
						if (PlayMontage)
						{
							// STATIC_CreatePlayMontageAndWaitProxy

							static auto def = FindObject("AbilityTask_PlayMontageAndWait /Script/GameplayAbilities.Default__AbilityTask_PlayMontageAndWait");

							struct
							{
							public:
								UObject* OwningAbility;                                     // 0x0(0x8)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
								FName                                  TaskInstanceName;                                  // 0x8(0x8)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
								UObject* MontageToPlay;                                     // 0x10(0x8)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
								float                                        Rate;                                              // 0x18(0x4)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
								FName                                  StartSection;                                      // 0x1C(0x8)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
								bool                                         bStopWhenAbilityEnds;                              // 0x24(0x1)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
								uint8_t                                        Pad_1103[0x3];                                     // Fixing Size After Last Property  [ Dumper-7 ]
								float                                        AnimRootMotionTranslationScale;                    // 0x28(0x4)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
								uint8_t                                        Pad_1104[0x4];                                     // Fixing Size After Last Property  [ Dumper-7 ]
								UObject* ReturnValue;        //UAbilityTask_PlayMontageAndWait                                // 0x30(0x8)(Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
							} UAbilityTask_PlayMontageAndWait_CreatePlayMontageAndWaitProxy_Params;

							UAbilityTask_PlayMontageAndWait_CreatePlayMontageAndWaitProxy_Params.OwningAbility = EmoteAbility;
							UAbilityTask_PlayMontageAndWait_CreatePlayMontageAndWaitProxy_Params.TaskInstanceName = FName();
							UAbilityTask_PlayMontageAndWait_CreatePlayMontageAndWaitProxy_Params.MontageToPlay = Montage;
							UAbilityTask_PlayMontageAndWait_CreatePlayMontageAndWaitProxy_Params.Rate = 1.0f;
							UAbilityTask_PlayMontageAndWait_CreatePlayMontageAndWaitProxy_Params.StartSection = FName(0);
							UAbilityTask_PlayMontageAndWait_CreatePlayMontageAndWaitProxy_Params.bStopWhenAbilityEnds = false;
							UAbilityTask_PlayMontageAndWait_CreatePlayMontageAndWaitProxy_Params.AnimRootMotionTranslationScale = 1.0f;

							static auto CreatePlayMontageAndWaitProxy = FindObject("Function /Script/GameplayAbilities.AbilityTask_PlayMontageAndWait.CreatePlayMontageAndWaitProxy");

							/* if (CreatePlayMontageAndWaitProxy)
								def->ProcessEvent(CreatePlayMontageAndWaitProxy, &UAbilityTask_PlayMontageAndWait_CreatePlayMontageAndWaitProxy_Params);
							else
								std::cout << "Unable to find CreatePlayMontageAndWaitProxy!\n"; */

							// IMPORTANT: https://ibb.co/H7RBBms
							// 
							auto Dura = PlayMontage(AbilitySystemComponent, EmoteAbility, FGameplayAbilityActivationInfo(), Montage, 1.0f, FName(0)); // TODO: Use PlayMontageAndWait
							// Helper::SetLocalRole(Pawn, ENetRole::ROLE_AutonomousProxy);

							// std::cout << ("Played for: ") << Dura << '\n';
							
							std::cout << "Played montage!\n";

							struct FFortGameplayAbilityMontageInfo
							{
								UObject* MontageToPlay;                                            // 0x0000(0x0008) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
								float                                              AnimPlayRate;
							};

							struct FFortCharacterPartMontageInfo
							{
								TEnumAsByte<EFortCustomPartType>                   CharacterPart;                                            // 0x0000(0x0001) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
								unsigned char                                      UnknownData00[0x7];                                       // 0x0001(0x0007) MISSED OFFSET
								UObject* AnimMontage;                                              // 0x0008(0x0008) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
							};

							auto MontageInfo = EmoteAbility->Member<__int64>("MontageInfo");

							static auto CharacterPartMontagesOffset = FindOffsetStruct("ScriptStruct /Script/FortniteGame.FortGameplayAbilityMontageInfo", "CharacterPartMontages");

							((FFortGameplayAbilityMontageInfo*)MontageInfo)->MontageToPlay = Montage;
							((FFortGameplayAbilityMontageInfo*)MontageInfo)->AnimPlayRate = 1.0f;

							// AbilityActorInfo->GetAnimInstance()

							struct { UObject* clientAnimMontage; float inPlayRate; } ServerCurrentMontageSetPlayRate_Params{ Montage, 1.0f };
							AbilitySystemComponent->ProcessEvent("ServerCurrentMontageSetPlayRate", &ServerCurrentMontageSetPlayRate_Params);

							auto CharacterPartMontages = (TArray< FFortCharacterPartMontageInfo>*)(__int64(MontageInfo) + CharacterPartMontagesOffset);
							FFortCharacterPartMontageInfo inf;
							inf.CharacterPart = EFortCustomPartType::Body;
							inf.AnimMontage = Montage;
							CharacterPartMontages->Add(inf);

							Pawn->ProcessEvent("OnMontageStarted", &Montage);

							static auto OnMontageStartedPlaying = EmoteAbility->Function("OnMontageStartedPlaying");
							/* static auto PlayInitialEmoteMontage = EmoteAbility->Function("PlayInitialEmoteMontage");

							if (PlayInitialEmoteMontage)
								EmoteAbility->ProcessEvent(PlayInitialEmoteMontage); // CRASHES
							else
								std::cout << "No PlayInitialEmoteMontage!\n"; */

							if (OnMontageStartedPlaying)
								EmoteAbility->ProcessEvent(OnMontageStartedPlaying);
							else
								std::cout << "No OnMontageStartedPlaying!\n";

							/*

							struct
							{
								UObject* OwningAbility;                                            // (Parm, ZeroConstructor, IsPlainOldData)
								FName                                       TaskInstanceName;                                         // (Parm, ZeroConstructor, IsPlainOldData)
								UObject* MontageToPlay;                                            // (Parm, ZeroConstructor, IsPlainOldData)
								float                                              PlayRate;                                                 // (Parm, ZeroConstructor, IsPlainOldData)
								FName                                       StartSection;                                             // (Parm, ZeroConstructor, IsPlainOldData)
								bool                                               bStopWhenAbilityEnds;                                     // (Parm, ZeroConstructor, IsPlainOldData)
								float                                              RootMotionTranslationScale;                               // (Parm, ZeroConstructor, IsPlainOldData)
								UObject* ReturnValue;     // UFortAbilityTask_PlayMontageWaitNotify                                          // (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData)
							} UFortAbilityTask_PlayMontageWaitNotify_PlayMontageAndWaitNotify_Params{EmoteAbility, FName(-1), Montage, 1.0f, FName(-1), false, 1.0f};

							*/

							static auto OnRep_LastReplicatedEmoteExecuted = Pawn->Function(("OnRep_LastReplicatedEmoteExecuted")); // this isnt in all versions

							if (OnRep_LastReplicatedEmoteExecuted)
								Pawn->ProcessEvent(OnRep_LastReplicatedEmoteExecuted);

							static auto OnRep_CharPartAnimMontageInfo = Pawn->Function("OnRep_CharPartAnimMontageInfo");

							if (OnRep_CharPartAnimMontageInfo)
								Pawn->ProcessEvent(OnRep_CharPartAnimMontageInfo);

							AbilitySystemComponent->ProcessEvent("OnRep_ReplicatedAnimMontage");
						}
					}
				}
			}
		}
	}

	return false;
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
		UObject* InteractComponent;                                        // (Parm, ZeroConstructor, InstancedReference, IsPlainOldData)
		TEnumAsByte<ETInteractionType>                     InteractType;                                             // (Parm, ZeroConstructor, IsPlainOldData)
		UObject* OptionalObjectData;                                       // (Parm, ZeroConstructor, IsPlainOldData)
	};

	auto Params = (SAIParams*)Parameters;

	if (Params && Controller)
	{
		auto Pawn = *Controller->Member<UObject*>(("Pawn"));
		auto ReceivingActor = Params->ReceivingActor;

		if (ReceivingActor)
		{
			auto ReceivingActorName = ReceivingActor->GetName(); // There has to be a better way, right?

			std::cout << ("ReceivingActorName: ") << ReceivingActorName << '\n';

			if (ReceivingActorName.contains(("Tiered")))
			{
				struct BitField_Container
				{
					unsigned char                                      bAlwaysShowContainer : 1;                                 // 0x0D99(0x0001) (Edit, BlueprintVisible)
					unsigned char                                      bAlwaysMaintainLoot : 1;                                  // 0x0D99(0x0001) (Edit, DisableEditOnTemplate)
					unsigned char                                      bDestroyContainerOnSearch : 1;                            // 0x0D99(0x0001) (Edit, BlueprintVisible)
					unsigned char                                      bAlreadySearched : 1;
				};

				struct BitField_Container2
				{
					uint8_t                                        bUseLootProperties_Athena : 1;                     // Mask : 0x1 0xC31(0x1)(Edit, BlueprintVisible, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
					uint8_t                                        bAlwaysShowContainer : 1;                          // Mask : 0x2 0xC31(0x1)(Edit, BlueprintVisible, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
					uint8_t                                        bAlwaysMaintainLoot : 1;                           // Mask : 0x4 0xC31(0x1)(Edit, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
					uint8_t                                        bDestroyContainerOnSearch : 1;                     // Mask : 0x8 0xC31(0x1)(Edit, BlueprintVisible, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
					uint8_t                                        bForceHidePickupMinimapIndicator : 1;              // Mask : 0x10 0xC31(0x1)(Edit, BlueprintVisible, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
					uint8_t                                        bForceSpawnLootOnDestruction : 1;                  // Mask : 0x20 0xC31(0x1)(Edit, BlueprintVisible, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
					uint8_t                                        bForceTossLootOnSpawn : 1;                         // Mask : 0x40 0xC31(0x1)(Edit, BlueprintVisible, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
					uint8_t                                        bAlreadySearched : 1;                              // Mask : 0x80
				};

				// TODO: Implement bitfields better

				if (Engine_Version < 424)
				{
					auto BitField = ReceivingActor->Member<BitField_Container>(("bAlreadySearched"));
					BitField->bAlreadySearched = true;
				}
				else
				{
					auto BitField = ReceivingActor->Member<BitField_Container2>(("bAlreadySearched"));
					BitField->bAlreadySearched = true;
				}

				static auto AlreadySearchedFn = ReceivingActor->Function(("OnRep_bAlreadySearched"));
				if (AlreadySearchedFn)
				{
					ReceivingActor->ProcessEvent(AlreadySearchedFn);
				}
			}

			if (ReceivingActorName.contains(("B_Athena_VendingMachine")))
			{
				// *ReceivingActor->Member<int>(("CostAmount"))
				// MaterialType
				auto Super = (UClass_FTT*)ReceivingActor;
				for (auto Super = (UClass_FTT*)ReceivingActor; Super; Super = (UClass_FTT*)Super->SuperStruct)
				{
					std::cout << ("SuperStruct: ") << Super->GetFullName() << '\n';
				}

				static auto GetCurrentActiveItem = ReceivingActor->Function(("GetCurrentActiveItem"));
				UObject* CurrentMaterial = nullptr;
				ReceivingActor->ProcessEvent(GetCurrentActiveItem, &CurrentMaterial);

				auto SpawnLocation = *ReceivingActor->Member<FVector>(("LootSpawnLocation"));
			}

			if (ReceivingActorName.contains(("Vehicle")))
			{
				Helper::SetLocalRole(*Controller->Member<UObject*>(("Pawn")), ENetRole::ROLE_Authority);
				Helper::SetLocalRole(ReceivingActor, ENetRole::ROLE_Authority);
				// Helper::SetLocalRole(*Controller->Member<UObject*>(("Pawn")), ENetRole::ROLE_AutonomousProxy);
				// Helper::SetLocalRole(ReceivingActor, ENetRole::ROLE_AutonomousProxy);
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
		struct parms { __int64 ZiplineState; };
		auto Params = (parms*)Parameters;

		static auto ZiplineOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.ZiplinePawnState"), ("Zipline"));

		auto Zipline = (UObject**)(__int64(&Params->ZiplineState) + ZiplineOffset);

		// Helper::SetLocalRole(Pawn, ENetRole::ROLE_AutonomousProxy);
		// Helper::SetRemoteRole(Pawn, ENetRole::ROLE_Authority);

		if (Zipline && *Zipline)
		{
			// Helper::SetLocalRole(*Zipline, ENetRole::ROLE_AutonomousProxy);
			// Helper::SetLocalRole(*Zipline, ENetRole::ROLE_Authority); // UNTESTED
			// Helper::SetRemoteRole(*Zipline, ENetRole::ROLE_Authority);
		}
		else
			std::cout << ("No zipline!\n");
	}

	return false;
}

inline bool PlayButtonHook(UObject* Object, UFunction* Function, void* Parameters)
{
	LoadInMatch();
	return false;
}

inline bool ServerSuicideHook(UObject* Controller, UFunction* Function, void* Parameters)
{
	if (Controller)
	{
		struct {
			FVector LaunchVelocity;
			bool bXYOverride;
			bool bZOverride;
			bool bIgnoreFallDamage;
			bool bPlayFeedbackEvent;
		} LCJParams{{ 90000, 90000, 90000 }, false, false, true, true};

		auto Pawn = Controller->Member<UObject*>(("Pawn"));

		if (Pawn && *Pawn)
		{
			auto ParachuteAttachment = (*Pawn)->Member<UObject*>(("ParachuteAttachment"));

			if (ParachuteAttachment && *ParachuteAttachment)
			{
				*(*ParachuteAttachment)->Member<bool>(("bParachuteVisible")) = false;
				*(*ParachuteAttachment)->Member<UObject*>(("PlayerPawn")) = nullptr;
				(*ParachuteAttachment)->ProcessEvent(("OnRep_PlayerPawn"));
				(*Pawn)->ProcessEvent(("OnRep_ParachuteAttachment"));
				Helper::DestroyActor(*ParachuteAttachment);
				std::cout << ("Destroyed ParachuteAttachment!\n");
			}
			else
				std::cout << ("No ParachuteAttachment!\n");

			static auto FortGliderInstance = FindObject(("BlueprintGeneratedClass /Game/Athena/Cosmetics/Blueprints/Gliders/B_BaseGlider.B_BaseGlider_C"));
			auto Gliders = Helper::GetAllActorsOfClass(FortGliderInstance);
			
			std::cout << ("Glider Num: ") << Gliders.Num() << '\n';

			for (int i = 0; i < Gliders.Num(); i++)
			{
				auto Glider = Gliders[i];

				if (!Glider)
					continue;

				Helper::DestroyActor(Glider);
			}

			Gliders.Free();

			/* UObject* GliderAnimInstance = nullptr;

			if (GliderAnimInstance)
			{
				auto OwnerGlider = GliderAnimInstance->Member<UObject*>(("OwnerGlider"));

				if (OwnerGlider && *OwnerGlider)
				{

				}
			} */
			// Helper::SpawnChip(Controller);
			*(*Pawn)->Member<bool>(("bIsDBNO")) = false;
			(*Pawn)->ProcessEvent(("OnRep_IsDBNO"));

			struct { UObject* EventInstigator; } COPRParams{ Controller };
			static auto COPRFn = FindObject(("Function /Script/FortniteGame.FortPlayerControllerZone.ClientOnPawnRevived"));

			Controller->ProcessEvent(COPRFn, &COPRParams);

			static auto setHealthFn = (*Pawn)->Function(("SetHealth"));
			struct { float NewHealthVal; }healthParams{ 30 };

			if (setHealthFn)
				(*Pawn)->ProcessEvent(setHealthFn, &healthParams);
			else
				std::cout << ("Unable to find setHealthFn!\n");

			(*Pawn)->ProcessEvent(("ForceReviveFromDBNO"));

			// std::cout << ("Spawned Chip!\n");
			/* static auto fn = (*Pawn)->Function(("LaunchCharacterJump"));

			if (fn)
				(*Pawn)->ProcessEvent(fn, &LCJParams);
			else
				std::cout << ("No LaunchCharacterJump!\n"); */
		}
		else
			std::cout << ("No Pawn!\n");
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

inline bool AircraftExitedDropZoneHook(UObject* GameMode, UFunction* Function, void* Parameters)
{
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
	if (BuildingActor && bStarted)
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

		static auto BuildingContainerClass = FindObject(("Class /Script/FortniteGame.BuildingContainer"));

		if (BuildingActor->IsA(BuildingContainerClass))
		{
			Looting::Tables::HandleSearch(BuildingActor);
		}
	}

	return false;
}

static UObject* __fastcall ReplicationGraph_EnableDetour(UObject* NetDriver, UObject* World)
{
	if (NetDriver && World)
	{
		std::cout << ("ReplicationGraph_Enable called!\n");
		std::cout << ("NetDriver: ") << NetDriver->GetFullName() << '\n';
		std::cout << ("World: ") << World->GetFullName() << '\n';

		static auto ReplicationDriverClass = FindObject(("Class /Script/FortniteGame.FortReplicationGraph"));

		struct {
			UObject* ObjectClass;
			UObject* Outer;
			UObject* ReturnValue;
		} params{ ReplicationDriverClass , NetDriver };

		static auto GSC = FindObject(("GameplayStatics /Script/Engine.Default__GameplayStatics"));
		static auto fn = GSC->Function(("SpawnObject"));
		// static auto fn = FindObject(("Function /Script/Engine.GameplayStatics.SpawnObject"));
		std::cout << "Creating graph\n";
		GSC->ProcessEvent(fn, &params);
		std::cout << "new rep graph: " << params.ReturnValue << '\n';
		return params.ReturnValue;
	}
	else
	{
		std::cout << ("ReplicationGraph_Enable but missing something!\n");
		std::cout << ("NetDriver: ") << NetDriver << '\n';
		std::cout << ("World: ") << World << '\n';

		World = Helper::GetWorld();

		if (World && NetDriver)
			return ReplicationGraph_EnableDetour(NetDriver, World);
		else
			return ReplicationGraph_Enable(NetDriver, World);
	}
}

bool ServerUpdateVehicleInputStateUnreliableHook(UObject* Pawn, UFunction* Function, void* Parameters) // FortAthenaVehicle
{
	struct parms { __int64 InState; float TimeStamp; };
	auto Params = (parms*)Parameters;

	static auto PitchAlphaOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortAthenaVehicleInputStateUnreliable"), ("PitchAlpha"));
	auto PitchAlpha = (float*)(__int64(&Params->InState) + PitchAlphaOffset);

	if (Pawn && Params && PitchAlpha)
	{
		auto Vehicle = Helper::GetVehicle(Pawn);

		if (Vehicle)
		{
			auto Rotation = Helper::GetActorRotation(Vehicle);
			auto Location = Helper::GetActorLocation(Vehicle);

			std::cout << ("Pitch Alpha: ") << *PitchAlpha << '\n';

			auto newRotation = FRotator{ *PitchAlpha, Rotation.Yaw, Rotation.Roll };

			Helper::SetActorLocationAndRotation(Vehicle, Location, newRotation);
		}
	}

	return false;
}

void FinishInitializeUHooks()
{
	if (Engine_Version < 422)
		AddHook(("BndEvt__BP_PlayButton_K2Node_ComponentBoundEvent_1_CommonButtonClicked__DelegateSignature"), PlayButtonHook);

	AddHook(("Function /Script/FortniteGame.BuildingActor.OnDeathServer"), OnDeathServerHook);
	AddHook(("Function /Script/Engine.GameMode.ReadyToStartMatch"), ReadyToStartMatchHook);
	AddHook(("Function /Script/FortniteGame.FortPlayerControllerAthena.ServerAttemptAircraftJump"), ServerAttemptAircraftJumpHook);
	AddHook(("Function /Script/FortniteGame.FortGameModeAthena.OnAircraftExitedDropZone"), AircraftExitedDropZoneHook); // "fix" (temporary) for aircraft after it ends on newer versions.
	//AddHook(("Function /Script/FortniteGame.FortPlayerController.ServerSuicide"), ServerSuicideHook);
	// AddHook(("Function /Script/FortniteGame.FortPlayerController.ServerCheat"), ServerCheatHook); // Commands Hook
	// AddHook(("Function /Script/FortniteGame.FortPlayerController.ServerClientPawnLoaded"), ServerClientPawnLoadedHook);
	AddHook(("Function /Script/FortniteGame.FortPlayerControllerZone.ClientOnPawnDied"), ClientOnPawnDiedHook);
	AddHook(("Function /Script/FortniteGame.FortPlayerPawn.ServerSendZiplineState"), ServerSendZiplineStateHook);
	// AddHook(("Function /Script/FortniteGame.FortPlayerPawn.ServerUpdateVehicleInputStateUnreliable"), ServerUpdateVehicleInputStateUnreliableHook)

	if (Engine_Version >= 420)
		AddHook(("Function /Script/FortniteGame.FortAthenaVehicle.ServerUpdatePhysicsParams"), ServerUpdatePhysicsParamsHook);

	if (PlayMontage)
		AddHook(("Function /Script/FortniteGame.FortPlayerController.ServerPlayEmoteItem"), ServerPlayEmoteItemHook);

	if (Engine_Version < 423)
	{ // ??? Idk why we need the brackets
		AddHook(("Function /Script/FortniteGame.FortPlayerController.ServerAttemptInteract"), ServerAttemptInteractHook);
	}
	else
		AddHook(("Function /Script/FortniteGame.FortControllerComponent_Interaction.ServerAttemptInteract"), ServerAttemptInteractHook);

	AddHook(("Function /Script/FortniteGame.FortPlayerControllerZone.ServerAttemptExitVehicle"), ServerAttemptExitVehicleHook);

	AddHook(("Function /Script/FortniteGame.FortPlayerPawn.ServerChoosePart"), ServerChoosePartHook);

	AddHook("Function /Script/FortniteGame.FortSafeZoneIndicator.OnSafeZoneStateChange", OnSafeZoneStateChangeHook);

	AddHook("Function /Script/FortniteGame.FortPlayerPawn.ServerReviveFromDBNO", ServerReviveFromDBNOHook);

	AddHook("Function /Script/FortniteGame.FortPlayerControllerAthena.ServerSendSquadFriend", ServerSendSquadFriendHook)

	// GameplayAbility.K2_CommitExecute We probably have to hook this for consumables

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
			auto FunctionName = Function->GetName();
			// if (Function->FunctionFlags & 0x00200000 || Function->FunctionFlags & 0x01000000) // && FunctionName.find("Ack") == -1 && FunctionName.find("AdjustPos") == -1))
			if (bLogRpcs && (FunctionName.starts_with(("Server")) || FunctionName.starts_with(("Client")) || FunctionName.starts_with(("OnRep_"))))
			{
				if (!FunctionName.contains("ServerUpdateCamera") && !FunctionName.contains("ServerMove")
					&& !FunctionName.contains(("ServerUpdateLevelVisibility"))
					&& !FunctionName.contains(("AckGoodMove")))
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
					!strstr(FunctionName.c_str(), "ReceiveHit"))
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

__int64 idkbrokeDetour(UObject* a1)
{
	std::cout << ("Idk\n");
	return 0;
}

void __fastcall HookToFixMaybeDetour(__int64 a1, unsigned int a2)
{
	std::cout << "Funne Called!\n";
	return;
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

	if (GetPlayerViewpointAddr) //Engine_Version == 423)
	{
		// fixes flashing

		MH_CreateHook((PVOID)GetPlayerViewpointAddr, GetPlayerViewPointDetour, (void**)&GetPlayerViewPoint);
		MH_EnableHook((PVOID)GetPlayerViewpointAddr);
	}
	else
		std::cout << ("[WARNING] Could not fix flashing!\n");
}
