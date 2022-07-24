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

// #define LOGGING

// HEAVILY INSPIRED BY KEMOS UFUNCTION HOOKING

static bool bStarted = false;

inline void initStuff()
{
	if (!bStarted && bTraveled)
	{
		bStarted = true;

		CreateThread(0, 0, Helper::Console::Setup, 0, 0, 0);

		auto world = Helper::GetWorld();
		auto gameState = *world->Member<UObject*>(_("GameState"));

		if (gameState)
		{
			auto AuthGameMode = *world->Member<UObject*>(_("AuthorityGameMode"));

			*(*AuthGameMode->Member<UObject*>(_("GameSession")))->Member<int>(_("MaxPlayers")) = 100;

			if (std::stod(FN_Version) >= 8 && AuthGameMode)
			{
				static auto PlayerControllerClass = FindObject(_("BlueprintGeneratedClass /Game/Athena/Athena_PlayerController.Athena_PlayerController_C"));
				std::cout << _("PlayerControllerClass: ") << PlayerControllerClass << '\n';
				*AuthGameMode->Member<UObject*>(_("PlayerControllerClass")) = PlayerControllerClass;
			}

			*gameState->Member<char>(_("bGameModeWillSkipAircraft")) = false;
			*gameState->Member<float>(_("AircraftStartTime")) = 99999.0f;
			*gameState->Member<float>(_("WarmupCountdownEndTime")) = 99999.0f;

			*gameState->Member<EFriendlyFireType>(_("FriendlyFireType")) = EFriendlyFireType::On;

			*gameState->Member<EAthenaGamePhase>(_("GamePhase")) = EAthenaGamePhase::Warmup;

			struct {
				EAthenaGamePhase OldPhase;
			} params2{ EAthenaGamePhase::None };

			static const auto fnGamephase = gameState->Function(_("OnRep_GamePhase"));

			if (fnGamephase)
				gameState->ProcessEvent(fnGamephase, &params2);

			if (AuthGameMode)
			{
				AuthGameMode->ProcessEvent(AuthGameMode->Function(_("StartPlay")), nullptr);

				AuthGameMode->ProcessEvent(AuthGameMode->Function(_("StartMatch")), nullptr);

				// *AuthGameMode->Member<bool>(_("bAlwaysDBNO")) = true;

				// Is this correct?
				/*class UClass* VehicleClass = (UClass*)(FindObject(_("Class /Script/FortniteGame.FortAthenaFerretVehicle")));//(UClass*(FindObject(_("Class /Script/FortniteGame.FortAthenaFerretVehicle.FortAthenaFerretVehicle"))));
				struct 
				{
					class UClass* VehicleClass;
				} SSVParams{ VehicleClass };
				
				auto SSVFn = *AuthGameMode->Member<bool>(_("ShouldSpawnVehicle")) = true;*/
				
				// static auto Playlist = FindObject(_("FortPlaylistAthena /Game/Athena/Playlists/Playlist_DefaultSolo.Playlist_DefaultSolo"));
				// static auto Playlist = FindObject(_("FortPlaylistAthena /Game/Athena/Playlists/Playlist_DefaultDuo.Playlist_DefaultDuo"));
				// static auto Playlist = FindObject(_("FortPlaylistAthena /Game/Athena/Playlists/Playlist_DefaultSquad.Playlist_DefaultSquad"));
				static auto Playlist = FindObject(_("FortPlaylistAthena /Game/Athena/Playlists/Playground/Playlist_Playground.Playlist_Playground"));
				// static auto Playlist = FindObject(_("/Game/Athena/Playlists/Fill/Playlist_Fill_Solo.Playlist_Fill_Solo"));
				if (std::stod(FN_Version) > 6.00)
				{
					auto OnRepPlaylist = gameState->Function(_("OnRep_CurrentPlaylistInfo"));

					static auto BasePlaylistOffset = FindOffsetStruct(_("ScriptStruct /Script/FortniteGame.PlaylistPropertyArray"), _("BasePlaylist"));
					static auto PlaylistReplicationKeyOffset = FindOffsetStruct(_("ScriptStruct /Script/FortniteGame.PlaylistPropertyArray"), _("PlaylistReplicationKey"));

					static auto PlaylistInfo = gameState->Member<FFastArraySerializer>(_("CurrentPlaylistInfo"));

					if (BasePlaylistOffset && OnRepPlaylist && Playlist)
					{
						auto BasePlaylist = (UObject**)(__int64(PlaylistInfo) + BasePlaylistOffset);// *gameState->Member<UObject>(_("CurrentPlaylistInfo"))->Member<UObject*>(_("BasePlaylist"), true);
						auto PlaylistReplicationKey = (int*)(__int64(PlaylistInfo) + PlaylistReplicationKeyOffset);

						if (BasePlaylist)
						{
							*BasePlaylist = Playlist;
							(*PlaylistReplicationKey)++;
							((FFastArraySerializer*)PlaylistInfo)->MarkArrayDirty();
							std::cout << _("Set playlist to: ") << Playlist->GetFullName() << '\n';
						}
						else
							std::cout << _("Base Playlist is null!\n");
					}
					else
					{
						std::cout << _("Missing something related to the Playlist!\n");
						std::cout << _("BasePlaylist Offset: ") << BasePlaylistOffset << '\n';
						std::cout << _("OnRepPlaylist: ") << OnRepPlaylist << '\n';
						std::cout << _("Playlist: ") << Playlist << '\n';
					}

					gameState->ProcessEvent(OnRepPlaylist);
				}
				else
				{
					/*static auto OnRepPlaylist = gameState->Function(_("OnRep_CurrentPlaylistData"));
					*gameState->Member<UObject*>(_("CurrentPlaylistData")) = Playlist;*/

					/*gameState->ProcessEvent(OnRepPlaylist);*/
				}
			}
			else
			{
				std::cout << dye::yellow(_("[WARNING] ")) << _("Failed to find AuthorityGameMode!\n");
			}

			*gameState->Member<int>(_("PlayersLeft")) = 0;
			gameState->ProcessEvent(_("OnRep_PlayersLeft"));

			if (Helper::IsRespawnEnabled())
			{
				// Enable glider redeploy
				FString GliderRedeployCmd;
				GliderRedeployCmd.Set(L"Athena.EnableParachuteEverywhere 1");
				Helper::Console::ExecuteConsoleCommand(GliderRedeployCmd);
			}
		}

		Listen(7777);
		// CreateThread(0, 0, MapLoadThread, 0, 0, 0);

		InitializeNetHooks();

		std::cout << _("Initialized NetHooks!\n");
		Events::LoadEvents();
	}
}

bool ServerLoadingScreenDroppedHook(UObject* PlayerController, UFunction* Function, void* Parameters)
{
	auto PlayerState = *PlayerController->Member<UObject*>(_("PlayerState"));
	auto Pawn = *PlayerController->Member<UObject*>(_("Pawn"));

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
		static auto GameState = *world->Member<UObject*>(_("GameState"));

		if (GameState)
		{
			auto Aircraft = GameState->Member<TArray<UObject*>>(_("Aircrafts"))->At(0);

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
	auto GameInstance = *GetEngine()->Member<UObject*>(_("GameInstance"));
	auto& LocalPlayers = *GameInstance->Member<TArray<UObject*>>(_("LocalPlayers"));
	auto PlayerController = *LocalPlayers.At(0)->Member<UObject*>(_("PlayerController"));

	if (PlayerController)
	{
		static auto SwitchLevelFn = PlayerController->Function(_("SwitchLevel"));
		FString Map;
		Map.Set(GetMapName());
		PlayerController->ProcessEvent(SwitchLevelFn, &Map);
		// Map.Free();
		bTraveled = true;
	}
	else
	{
		std::cout << dye::red(_("[ERROR] ")) << _("Unable to find PlayerController!\n");
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
				Controller->ProcessEvent(_("Suicide"));
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
inline bool ClientOnPawnDiedHook(UObject* Controller, UFunction* Function, void* Parameters)
{
	static bool bRespawning = true;
	if (bRespawning)
		Player::RespawnPlayer(Controller);
	else
	{
		auto Pawn = *Controller->Member<UObject*>(_("Pawn"));
		auto Chip = Helper::SpawnChip(Controller);
		// 0x0018
		struct FFortResurrectionData
		{
			bool                                               bResurrectionChipAvailable;                               // 0x0000(0x0001) (ZeroConstructor, IsPlainOldData)
			unsigned char                                      UnknownData00[0x3];                                       // 0x0001(0x0003) MISSED OFFSET
			float                                              ResurrectionExpirationTime;                               // 0x0004(0x0004) (ZeroConstructor, IsPlainOldData)
			float                                              ResurrectionExpirationLength;                             // 0x0008(0x0004) (ZeroConstructor, IsPlainOldData)
			struct FVector                                     WorldLocation;                                            // 0x000C(0x000C) (ZeroConstructor, IsPlainOldData)
		};

		auto PlayerState = *Pawn->Member<UObject*>(_("PlayerState"));
		auto FortResurrectionData = PlayerState->Member<FFortResurrectionData>(_("ResurrectionChipAvailable"));

		if (FortResurrectionData)
		{
			std::cout << _("FortResurrectionData valid!\n");
			std::cout << _("FortResurrectionData Location X: ") << FortResurrectionData->WorldLocation.X << '\n';
		}
		else
			std::cout << _("No FortResurrectionData!\n");

		auto ResurrectionData = FFortResurrectionData{};
		ResurrectionData.bResurrectionChipAvailable = true;
		ResurrectionData.ResurrectionExpirationLength = 99999.f;
		ResurrectionData.ResurrectionExpirationTime = 99999.f;
		ResurrectionData.WorldLocation = Helper::GetActorLocation(Chip);

		if (FortResurrectionData)
			*FortResurrectionData = ResurrectionData;
		std::cout << _("Spawned Chip!\n");
	}
	
	return true;
}



inline bool ServerAttemptExitVehicleHook(UObject* Controller, UFunction* Function, void* Parameters)
{
	auto Pawn = Controller->Member<UObject*>(_("Pawn"));

	if (Pawn && *Pawn)
	{
		UObject* Vehicle = Helper::GetVehicle(*Pawn);

		if (Vehicle)
		{
			Helper::SetLocalRole(*Pawn, ENetRole::ROLE_Authority);
			Helper::SetLocalRole(Vehicle, ENetRole::ROLE_Authority);
		}
	}

	return false;
}

inline bool ServerPlayEmoteItemHook(UObject* Controller, UFunction* Function, void* Parameters)
{
	auto CurrentPawn = *Controller->Member<UObject*>(_("Pawn"));

	struct SPEIParams  { UObject* EmoteAsset; }; // UFortMontageItemDefinitionBase
	auto EmoteParams = (SPEIParams*)Parameters;

	auto EmoteAsset = EmoteParams->EmoteAsset;

	if (Controller /* && !Controller->IsInAircraft() */ && CurrentPawn && EmoteAsset)
	{
		struct {
			TEnumAsByte<EFortCustomBodyType> BodyType;
			TEnumAsByte<EFortCustomGender> Gender;
			UObject* AnimMontage; // UAnimMontage
		} GAHRParams{EFortCustomBodyType::All, EFortCustomGender::Both}; // (CurrentPawn->CharacterBodyType, CurrentPawn->CharacterGender)
		static auto fn = EmoteAsset->Function(_("GetAnimationHardReference"));

		if (fn)
		{
			EmoteAsset->ProcessEvent(fn, &GAHRParams);
			auto Montage = GAHRParams.AnimMontage;
			if (Montage)
			{
				auto StartSection = FName();

				struct {
					UObject* AnimMontage;
					float InPlayRate;
					FName& StartSectionName;
				} PLAMParams{ Montage, 1.0f, StartSection };

				static auto PLAMFn = CurrentPawn->Function(_("PlayLocalAnimMontage"));
				CurrentPawn->ProcessEvent(PLAMFn, &PLAMParams);

				struct {
					UObject* AnimMontage;
					float InPlayRate;
					FName& StartSectionName;
				} PAMParams{Montage, 1.0f, StartSection};
				
				static auto PAMFn = CurrentPawn->Function(_("PlayAnimMontage"));
				CurrentPawn->ProcessEvent(PAMFn, &PAMParams);
				// CurrentPawn->PlayAnimMontage(Montage, 1.0f, FName(0));
				CurrentPawn->ProcessEvent(_("OnRep_CharPartAnimMontageInfo"));
				CurrentPawn->ProcessEvent(_("OnRep_ReplicatedAnimMontage"));
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

	ProcessEventO(Controller, Function, Parameters);

	struct SAIParams {
		UObject* ReceivingActor;                                           // (Parm, ZeroConstructor, IsPlainOldData)
		UObject* InteractComponent;                                        // (Parm, ZeroConstructor, InstancedReference, IsPlainOldData)
		TEnumAsByte<ETInteractionType>                     InteractType;                                             // (Parm, ZeroConstructor, IsPlainOldData)
		UObject* OptionalObjectData;                                       // (Parm, ZeroConstructor, IsPlainOldData)
	};

	auto Params = (SAIParams*)Parameters;

	if (Params && Controller)
	{
		auto Pawn = *Controller->Member<UObject*>(_("Pawn"));
		auto ReceivingActor = Params->ReceivingActor;
		if (ReceivingActor)
		{
			auto ReceivingActorName = ReceivingActor->GetName(); // There has to be a better way, right?

			std::cout << _("ReceivingActorName: ") << ReceivingActorName << '\n';

			if (ReceivingActorName.contains(_("Tiered")))
			{
				struct BitField_Container
				{
					unsigned char                                      bAlwaysShowContainer : 1;                                 // 0x0D99(0x0001) (Edit, BlueprintVisible)
					unsigned char                                      bAlwaysMaintainLoot : 1;                                  // 0x0D99(0x0001) (Edit, DisableEditOnTemplate)
					unsigned char                                      bDestroyContainerOnSearch : 1;                            // 0x0D99(0x0001) (Edit, BlueprintVisible)
					unsigned char                                      bAlreadySearched : 1;
				};

				// TODO: Implement bitfields better

				auto BitField = ReceivingActor->Member<BitField_Container>(_("bAlreadySearched"));
				BitField->bAlreadySearched = true;

				static auto AlreadySearchedFn = ReceivingActor->Function(_("OnRep_bAlreadySearched"));
				if (AlreadySearchedFn)
				{
					ReceivingActor->ProcessEvent(AlreadySearchedFn);

					if (ReceivingActorName.contains(_("Chest")))
					{
						/* static auto BlueAR = FindObject(_("FortWeaponRangedItemDefinition /Game/Athena/Items/Weapons/WID_Assault_Auto_Athena_R_Ore_T03.WID_Assault_Auto_Athena_R_Ore_T03"));
						
						if (BlueAR)
							Helper::SummonPickup(Pawn, BlueAR, Helper::GetActorLocation(ReceivingActor), EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::Chest, 1); */
						Looting::Tables::HandleSearch(ReceivingActor);
					}
					else if (ReceivingActorName.contains(_("Ammo")))
					{
						if (Engine_Version < 423)
						{
							static UObject* AthenaAmmoDataRockets = FindObject(_("FortAmmoItemDefinition /Game/Athena/Items/Ammo/AthenaAmmoDataRockets.AthenaAmmoDataRockets"));
							if (AthenaAmmoDataRockets)
								Helper::SummonPickup(Pawn, AthenaAmmoDataRockets, Helper::GetActorLocation(ReceivingActor), EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::AmmoBox, 1);
						}
						else
						{
							static UObject* AthenaAmmoDataRockets = FindObject(_("FortAmmoItemDefinition /Game/Athena/Items/Ammo/AmmoDataRockets.AmmoDataRockets"));
							
							if (AthenaAmmoDataRockets)
								Helper::SummonPickup(Pawn, AthenaAmmoDataRockets, Helper::GetActorLocation(ReceivingActor), EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::AmmoBox, 1);
						}
					}
				}
			}

			if (ReceivingActorName.contains(_("B_Athena_VendingMachine")))
			{
				// *ReceivingActor->Member<int>(_("CostAmount"))
				// MaterialType
				auto Super = (UClass_FTT*)ReceivingActor;
				for (auto Super = (UClass_FTT*)ReceivingActor; Super; Super = (UClass_FTT*)Super->SuperStruct)
				{
					std::cout << _("SuperStruct: ") << Super->GetFullName() << '\n';
				}

				static auto GetCurrentActiveItem = ReceivingActor->Function(_("GetCurrentActiveItem"));
				UObject* CurrentMaterial = nullptr;
				ReceivingActor->ProcessEvent(GetCurrentActiveItem, &CurrentMaterial);

				auto SpawnLocation = *ReceivingActor->Member<FVector>(_("LootSpawnLocation"));
			}

			/* if (ReceivingActorName.contains(_("Vehicle")))
			{
				Helper::SetLocalRole(*Controller->Member<UObject*>(_("Pawn")), ENetRole::ROLE_AutonomousProxy);
				Helper::SetLocalRole(ReceivingActor, ENetRole::ROLE_AutonomousProxy);
			} */
		}
	}

	return true;
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

		auto Pawn = Controller->Member<UObject*>(_("Pawn"));

		if (Pawn && *Pawn)
		{
			auto ParachuteAttachment = (*Pawn)->Member<UObject*>(_("ParachuteAttachment"));

			if (ParachuteAttachment && *ParachuteAttachment)
			{
				*(*ParachuteAttachment)->Member<bool>(_("bParachuteVisible")) = false;
				*(*ParachuteAttachment)->Member<UObject*>(_("PlayerPawn")) = nullptr;
				(*ParachuteAttachment)->ProcessEvent(_("OnRep_PlayerPawn"));
				(*Pawn)->ProcessEvent(_("OnRep_ParachuteAttachment"));
				Helper::DestroyActor(*ParachuteAttachment);
				std::cout << _("Destroyed ParachuteAttachment!\n");
			}
			else
				std::cout << _("No ParachuteAttachment!\n");

			static auto FortGliderInstance = FindObject(_("BlueprintGeneratedClass /Game/Athena/Cosmetics/Blueprints/Gliders/B_BaseGlider.B_BaseGlider_C"));
			auto Gliders = Helper::GetAllActorsOfClass(FortGliderInstance);
			
			std::cout << _("Glider Num: ") << Gliders.Num() << '\n';

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
				auto OwnerGlider = GliderAnimInstance->Member<UObject*>(_("OwnerGlider"));

				if (OwnerGlider && *OwnerGlider)
				{

				}
			} */
			// Helper::SpawnChip(Controller);
			*(*Pawn)->Member<bool>(_("bIsDBNO")) = false;
			(*Pawn)->ProcessEvent(_("OnRep_IsDBNO"));

			struct { UObject* EventInstigator; } COPRParams{ Controller };
			static auto COPRFn = FindObject(_("Function /Script/FortniteGame.FortPlayerControllerZone.ClientOnPawnRevived"));

			Controller->ProcessEvent(COPRFn, &COPRParams);

			static auto setHealthFn = (*Pawn)->Function(_("SetHealth"));
			struct { float NewHealthVal; }healthParams{ 30 };

			if (setHealthFn)
				(*Pawn)->ProcessEvent(setHealthFn, &healthParams);
			else
				std::cout << _("Unable to find setHealthFn!\n");

			(*Pawn)->ProcessEvent(_("ForceReviveFromDBNO"));

			// std::cout << _("Spawned Chip!\n");
			/* static auto fn = (*Pawn)->Function(_("LaunchCharacterJump"));

			if (fn)
				(*Pawn)->ProcessEvent(fn, &LCJParams);
			else
				std::cout << _("No LaunchCharacterJump!\n"); */
		}
		else
			std::cout << _("No Pawn!\n");
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
		std::cout << _("bIsPawnLoaded: ") << Params->bIsPawnLoaded << '\n';

		auto Pawn = *Controller->Member<UObject*>(_("Pawn"));

		if (Pawn)
		{
			auto bLoadingScreenDropped = *Controller->Member<bool>(_("bLoadingScreenDropped"));
			if (bLoadingScreenDropped && !Params->bIsPawnLoaded)
			{
			}
			else
				std::cout << _("Loading screen is not dropped!\n");
		}
		else
			std::cout << _("Pawn is not valid!\n");
	}

	return false;
}

inline bool AircraftExitedDropZoneHook(UObject* GameMode, UFunction* Function, void* Parameters)
{
	return true;
}

void FinishInitializeUHooks()
{
	if (Engine_Version < 422)
		AddHook(_("BndEvt__BP_PlayButton_K2Node_ComponentBoundEvent_1_CommonButtonClicked__DelegateSignature"), PlayButtonHook);

	AddHook(_("Function /Script/Engine.GameMode.ReadyToStartMatch"), ReadyToStartMatchHook);
	AddHook(_("Function /Script/FortniteGame.FortPlayerControllerAthena.ServerAttemptAircraftJump"), ServerAttemptAircraftJumpHook);
	AddHook(_("Function /Script/FortniteGame.FortGameModeAthena.OnAircraftExitedDropZone"), AircraftExitedDropZoneHook); // "fix" (temporary) for aircraft after it ends on newer versions.
	AddHook(_("Function /Script/FortniteGame.FortPlayerController.ServerSuicide"), ServerSuicideHook);
	// AddHook(_("Function /Script/FortniteGame.FortPlayerController.ServerCheat"), ServerCheatHook); // Commands Hook
	AddHook(_("Function /Script/FortniteGame.FortPlayerController.ServerClientPawnLoaded"), ServerClientPawnLoadedHook);
	AddHook(_("Function /Script/FortniteGame.FortPlayerControllerZone.ClientOnPawnDied"), ClientOnPawnDiedHook);

	// AddHook(_("Function /Script/FortniteGame.FortPlayerController.ServerPlayEmoteItem"), ServerPlayEmoteItemHook);

	if (Engine_Version < 423)
	{ // ??? Idk why we need the brackets
		AddHook(_("Function /Script/FortniteGame.FortPlayerController.ServerAttemptInteract"), ServerAttemptInteractHook);
	}
	else
		AddHook(_("Function /Script/FortniteGame.FortControllerComponent_Interaction.ServerAttemptInteract"), ServerAttemptInteractHook);

	AddHook(_("Function /Script/FortniteGame.FortPlayerControllerZone.ServerAttemptExitVehicle"), ServerAttemptExitVehicleHook);

	for (auto& Func : FunctionsToHook)
	{
		if (!Func.first)
			std::cout << _("Detected null UFunction!\n");
	}

	std::cout << std::format("Hooked {} UFunctions!\n", std::to_string(FunctionsToHook.size()));
}

void* ProcessEventDetour(UObject* Object, UFunction* Function, void* Parameters)
{
	if (Object && Function)
	{
#ifdef LOGGING
		if (bStarted && bListening)
		{
			auto FunctionName = Function->GetName();
			// if (Function->FunctionFlags & 0x00200000 || Function->FunctionFlags & 0x01000000) // && FunctionName.find("Ack") == -1 && FunctionName.find("AdjustPos") == -1))
			if (FunctionName.starts_with(_("Server")) || FunctionName.starts_with(_("Client")) || FunctionName.starts_with(_("OnRep_")))
			{
				if (!FunctionName.contains("ServerUpdateCamera") && !FunctionName.contains("ServerMove")
					&& !FunctionName.contains(_("ServerUpdateLevelVisibility"))
					&& !FunctionName.contains(_("AckGoodMove")))
				{
					std::cout << "RPC Called: " << FunctionName << '\n';
				}
			}
		}
#endif

		for (auto& Func : FunctionsToHook)
		{
			if (Function == Func.first)
			{
				if (Func.second(Object, Function, Parameters)) // If the fuynction returned true, then cancel execution.
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
		// static auto fn = FindObject(_("Function /Script/Engine.Controller.GetViewTarget"));
		UObject* TheViewTarget = *pc->Member<UObject*>(_("Pawn"));
		// pc->ProcessEvent(fn, &TheViewTarget);

		if (TheViewTarget)
		{
			if (a2)
				*a2 = Helper::GetActorLocation(TheViewTarget);
			if (a3)
				*a3 = Helper::GetActorRotation(TheViewTarget);
			// std::cout << _("Did the ViewPoint!\n");

			return;
		}
		// else
			// std::cout << _("unable to get viewpoint!\n"); // This will happen if someone leaves iirc
	}

	return GetPlayerViewPoint(pc, a2, a3);
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
		MH_CreateHook((PVOID)GetPlayerViewpointAddr, GetPlayerViewPointDetour, (void**)&GetPlayerViewPoint);
		MH_EnableHook((PVOID)GetPlayerViewpointAddr);
	}
	else
		std::cout << _("[WARNING] Could not fix flashing!\n");
}
