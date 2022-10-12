#pragma once

#include <MinHook/MinHook.h>

#include <Net/funcs.h>
#include <Gameplay/helper.h>
#include <Gameplay/inventory.h>
#include <Gameplay/abilities.h>
#include <Gameplay/loot.h>

#include <discord.h>
#include <Net/replication.h>
#include <Gameplay/zone.h>
#include <Gameplay/harvesting.h>
#include <Gameplay/events.h>
#include <team.h>

static bool bTraveled = false;

bool commitExecuteWeapon(UObject* Ability, UFunction*, void* Parameters)
{
	// std::cout << "execute\n";

	if (Ability)
	{
		UObject* Pawn; // Helper::GetOwner(ability);
		Ability->ProcessEvent("GetActivatingPawn", &Pawn);

		if (Pawn)
		{
			// std::cout << "pawn: " << Pawn->GetFullName() << '\n';
			auto currentWeapon = Helper::GetCurrentWeapon(Pawn);

			if (currentWeapon)
			{
				auto Controller = *Pawn->Member<UObject*>("Controller");
				auto entry = Inventory::GetEntryFromWeapon(Controller, currentWeapon);
				(*FFortItemEntry::GetLoadedAmmo(entry)) = *currentWeapon->Member<int>("AmmoCount"); // -= 1;

				std::cout << "loaded ammo: " << (*FFortItemEntry::GetLoadedAmmo(entry)) << '\n';
				std::cout << "weapon ammo: " << *currentWeapon->Member<int>("AmmoCount") << '\n';
				std::cout << "bozo!\n";

				setBitfield(currentWeapon, "bUpdateLocalAmmoCount", true);

				Inventory::Update(Controller, -1, true, (FFastArraySerializerItem*)entry);
				Inventory::GetWorldInventory(Controller)->ProcessEvent("ForceNetUpdate");
			}
			else
				std::cout << "No CurrentWeapon!\n";
		}
	}

	return false;
}

inline bool ServerAcknowledgePossessionHook(UObject* Object, UFunction* Function, void* Parameters) // This fixes movement on S5+, Cat led me to the right direction, then I figured out it's something with ClientRestart and did some common sense and found this.
{
	struct SAP_Params
	{
		UObject* P;
	};

	auto Params = (SAP_Params*)Parameters;
	if (Params)
	{
		auto Pawn = Params->P; // (UObject*)Parameters;

		static auto AcknowledgedPawnOffset = GetOffset(Object, "AcknowledgedPawn");
		*(UObject**)(__int64(Object) + AcknowledgedPawnOffset) = Pawn;
		std::cout << "Set Pawn!\n";
	}

	return false;
}

void InitializeNetUHooks()
{
	AddHook(("Function /Script/Engine.PlayerController.ServerAcknowledgePossession"), ServerAcknowledgePossessionHook);
}

void TickFlushDetour(UObject* thisNetDriver, float DeltaSeconds)
{
	auto World = Helper::GetWorld();

	if (World)
	{
		static auto NetDriverOffset = GetOffset(World, "NetDriver");
		auto NetDriver = *(UObject**)(__int64(World) + NetDriverOffset);

		if (NetDriver)
		{
			static auto ClientConnectionsOffset = GetOffset(NetDriver, "ClientConnections");
			// auto& ClientConnections = *NetDriver->CachedMember<TArray<UObject*>>(("ClientConnections"));

			auto ClientConnections = (TArray<UObject*>*)(__int64(NetDriver) + ClientConnectionsOffset);

			if (ClientConnections && ClientConnections->Num() > 0)
			{
				if (FnVerDouble <= 3.3)
				{
					ReplicateActors(NetDriver, World);
				}
				else
				{
					static auto ReplicationDriverOffset = GetOffset(NetDriver, "ReplicationDriver");
					auto ReplicationDriver = (UObject**)(__int64(NetDriver) + ReplicationDriverOffset);

					if (ReplicationDriver && *ReplicationDriver)
					{
						RepGraph_ServerReplicateActors(*ReplicationDriver);
					}
				}
			}
		}
		else
			std::cout << "No World netDriver??\n";
	}
	else
		std::cout << "no world?!?!\n";

	return TickFlush(thisNetDriver, DeltaSeconds);
}

uint64_t GetNetModeDetour(UObject* World)
{
	return ENetMode::NM_ListenServer;
}

bool LP_SpawnPlayActorDetour(UObject* Player, const FString& URL, FString& OutError, UObject* World)
{
	if (!bTraveled)
	{
		return LP_SpawnPlayActor(Player, URL, OutError, World);
	}
	return true;
}

char KickPlayerDetour(__int64 a1, __int64 a2, __int64 a3)
{
	std::cout << ("Player Kick! Returning 0..\n");
	return 0;
}

char __fastcall ValidationFailureDetour(__int64* a1, __int64 a2)
{
	std::cout << ("Validation!\n");
	return 0;
}

bool bMyPawn = false;

template <typename T>
auto getPair(T map, int index)
{
	auto it = map.begin();
	std::advance(it, index);
	return it;
}

UObject* SpawnPlayActorDetour(UObject* World, UObject* NewPlayer, ENetRole RemoteRole, FURL& URL, void* UniqueId, FString& Error, uint8_t NetPlayerIndex)
{
	static int LastResetNum = 824524899135;

	if (LastResetNum != AmountOfRestarts) // only run these once per game // we would do this when map is fully loaded but no func
	{
		LastResetNum = AmountOfRestarts;

		InitializeHarvestingHooks();

		// AddHook("Function /Game/Abilities/Weapons/Ranged/GA_Ranged_GenericDamage.GA_Ranged_GenericDamage_C.K2_CommitExecute", commitExecuteWeapon);

		// CreateThread(0, 0, LootingV2::SummonFloorLoot, 0, 0, 0);

		// Helper::SetGamePhase(EAthenaGamePhase::Warmup); // i hate u nomcp

		static auto func1 = FindObject("Function /Game/Athena/SafeZone/SafeZoneIndicator.SafeZoneIndicator_C.OnSafeZoneStateChange");

		if (func1)
			AddHook("Function /Game/Athena/SafeZone/SafeZoneIndicator.SafeZoneIndicator_C.OnSafeZoneStateChange", OnSafeZoneStateChangeHook);
		else
			AddHook("Function /Script/FortniteGame.FortSafeZoneIndicator.OnSafeZoneStateChange", OnSafeZoneStateChangeHook);

		if (Engine_Version >= 423)
		{
			Helper::GetGameState()->ProcessEvent("OnRep_CurrentPlaylistInfo"); // fix battle bus lol
		}
	}

	std::cout << ("SpawnPlayActor called!\n");
	auto PlayerController = SpawnPlayActor(Helper::GetWorld(), NewPlayer, RemoteRole, URL, UniqueId, Error, NetPlayerIndex);
	static auto FortPlayerControllerAthenaClass = FindObject(("Class /Script/FortniteGame.FortPlayerControllerAthena"));

	// std::cout << "PC Name: " << PlayerController->GetFullName() << '\n';

	if (!PlayerController || !PlayerController->IsA(FortPlayerControllerAthenaClass))
		return nullptr;

	auto PlayerState = Helper::GetPlayerStateFromController(PlayerController);

	if (!PlayerState) // this happened somehow
		return PlayerController;

	auto OPlayerName = Helper::GetPlayerName(PlayerController);
	std::string PlayerName = OPlayerName;
	std::transform(OPlayerName.begin(), OPlayerName.end(), OPlayerName.begin(), ::tolower);

	static auto ClientReturnToMainMenu = PlayerController->Function("ClientReturnToMainMenu");

	if (OPlayerName.contains(("fuck")) || OPlayerName.contains(("shit")))
	{
		FString Reason;
		Reason.Set(L"Inappropriate name!");

		if (ClientReturnToMainMenu)
			PlayerController->ProcessEvent(ClientReturnToMainMenu, &Reason);
	}

	if (OPlayerName.length() >= 40)
	{
		FString Reason;
		Reason.Set(L"Too long of a name!");

		if (ClientReturnToMainMenu)
			PlayerController->ProcessEvent(ClientReturnToMainMenu, &Reason);
	}

	/* if (FnVerDouble >= 12.61) // fix crash kinda
	{
		static auto NetPriorityOffset = GetOffset(PlayerController, "NetPriority");
		*(float*)(__int64(Inventory::GetWorldInventory(PlayerController)) + NetPriorityOffset) = 3.0f;
	} */

	static auto Connection_PlayerController = GetOffset(NewPlayer, "PlayerController");
	*(UObject**)(__int64(NewPlayer) + Connection_PlayerController) = PlayerController;

	if (FnVerDouble < 7.4)
	{
		static const auto QuickBarsClass = FindObjectOld("Class /Script/FortniteGame.FortQuickBars", true);
		static auto QuickBarsOffset = GetOffset(PlayerController, "QuickBars");
		auto QuickBars = (UObject**)(__int64(PlayerController) + QuickBarsOffset);

		if (QuickBars)
		{
			*QuickBars = Easy::SpawnActor(QuickBarsClass, FVector(), FRotator(), PlayerController);
			Helper::SetOwner(*QuickBars, PlayerController); // unneeded?
		}
	}

	// half of these are useless

	static auto bReadyToStartMatchOffset = GetOffset(PlayerController, "bReadyToStartMatch");
	*(char*)(__int64(PlayerController) + bReadyToStartMatchOffset) = true;

	static auto bClientPawnIsLoadedOffset = GetOffset(PlayerController, "bClientPawnIsLoaded");
	*(char*)(__int64(PlayerController) + bClientPawnIsLoadedOffset) = true;

	static auto bHasInitiallySpawnedOffset = GetOffset(PlayerController, "bHasInitiallySpawned");
	*(char*)(__int64(PlayerController) + bHasInitiallySpawnedOffset) = true;

	static auto bHasServerFinishedLoadingOffset = GetOffset(PlayerController, "bHasServerFinishedLoading");
	*(char*)(__int64(PlayerController) + bHasServerFinishedLoadingOffset) = true;

	static auto bHasClientFinishedLoadingOffset = GetOffset(PlayerController, "bHasClientFinishedLoading");
	*(char*)(__int64(PlayerController) + bHasClientFinishedLoadingOffset) = true;

	static auto bHasStartedPlayingOffset = GetOffset(PlayerState, "bHasStartedPlaying");
	*(char*)(__int64(PlayerState) + bHasStartedPlayingOffset) = true;

	static auto bHasFinishedLoadingOffset = GetOffset(PlayerState, "bHasFinishedLoading");
	*(char*)(__int64(PlayerState) + bHasFinishedLoadingOffset) = true;

	static auto bIsReadyToContinueOffset = GetOffset(PlayerState, "bIsReadyToContinue");
	*(char*)(__int64(PlayerState) + bIsReadyToContinueOffset) = true;

	/* setBitfield(PlayerState, "bReadyToStartMatch", true);
	setBitfield(PlayerState, "bClientPawnIsLoaded", true);
	setBitfield(PlayerState, "bHasInitiallySpawned", true);

	setBitfield(PlayerState, "bHasServerFinishedLoading", true);
	setBitfield(PlayerState, "bHasClientFinishedLoading", true);

	setBitfield(PlayerState, "bHasStartedPlaying", true);
	setBitfield(PlayerState, "bHasFinishedLoading", true);
	setBitfield(PlayerState, "bIsReadyToContinue", true); */

	/* static auto bHasServerFinishedLoadingOffset = GetOffset(PlayerController, "bHasServerFinishedLoading");
	static auto bHasServerFinishedLoadingFM = GetFieldMask(GetProperty(PlayerController, "bHasServerFinishedLoading"));
	static auto bHasServerFinishedLoadingBI = GetBitIndex(GetProperty(PlayerController, "bHasServerFinishedLoading"), bHasServerFinishedLoadingFM);

	sett((uint8_t*)(__int64(PlayerController) + bHasServerFinishedLoadingOffset), bHasServerFinishedLoadingBI, bHasServerFinishedLoadingFM, true);

	static auto bHasStartedPlayingOffset = GetOffset(PlayerController, "bHasStartedPlaying");
	static auto bHasStartedPlayingFM = GetFieldMask(GetProperty(PlayerController, "bHasStartedPlaying"));
	static auto bHasStartedPlayingBI = GetBitIndex(GetProperty(PlayerController, "bHasStartedPlaying"), bHasStartedPlayingFM);

	sett((uint8_t*)(__int64(PlayerController) + bHasStartedPlayingOffset), bHasStartedPlayingBI, bHasStartedPlayingFM, true); */

	auto Pawn = Helper::InitPawn(PlayerController, true, Helper::GetPlayerStart(), true);

	// FindObjectOld(".FortReplicationGraphNode_AlwaysRelevantForSquad_")->Member<TArray<UObject*>>("RebootCards")->Add(Helper::SpawnChip(PlayerController, Helper::GetActorLocation(Pawn)));
	// FindObjectOld(".FortReplicationGraphNode_AlwaysRelevantForSquad_")->Member<TArray<UObject*>>("PlayerStates")->Add(PlayerState);

	if (!Pawn)
	{
		std::cout << "Failed to spawn Pawn!\n";
		return PlayerController;
	}

	// todo: not do this for invicibility

	// if (Engine_Version <= 421 || NoMcpAddr)
	{
		static auto CheatManagerOffset = GetOffset(PlayerController, "CheatManager");
		auto CheatManager = (UObject**)(__int64(PlayerController) + CheatManagerOffset);

		static auto CheatManagerClass = FindObject("Class /Script/Engine.CheatManager");
		*CheatManager = Easy::SpawnObject(CheatManagerClass, PlayerController);

		static auto God = (*CheatManager)->Function("God");

		if (God)
			(*CheatManager)->ProcessEvent(God);
	}

	if (bTeamsEnabled)
	{
		// if (FnVerDouble >= 8 && FnVerDouble < 13)
		{
			Teams::AssignTeam(PlayerController);
		}
	}

	if (FnVerDouble < AboveVersionDisableAbilities && std::floor(FnVerDouble) != 13)
	{
		GiveAllBRAbilities(Pawn);
	}

	// if (Engine_Version >= 420) // && FnVerDouble < 19.00)

	{
		Inventory::GiveStartingItems(PlayerController); // Gives the needed items like edit tool and builds

		auto PickaxeDef = Helper::GetPickaxeDef(PlayerController, true);

		std::cout << "PickaxeDef: " << PickaxeDef << '\n';

		static int LastResetNum = 0;

		static UObject* First = FindObject(StartingSlot1.first);
		static UObject* Second = FindObject(StartingSlot2.first);
		static UObject* Third = FindObject(StartingSlot3.first);
		static UObject* Fourth = FindObject(StartingSlot4.first);
		static UObject* Fifth = FindObject(StartingSlot5.first);

		if (LastResetNum != AmountOfRestarts)
		{
			LastResetNum = AmountOfRestarts;

			First = FindObject(StartingSlot1.first);
			Second = FindObject(StartingSlot2.first);
			Third = FindObject(StartingSlot3.first);
			Fourth = FindObject(StartingSlot4.first);
			Fifth = FindObject(StartingSlot5.first);
		}

		Inventory::CreateAndAddItem(PlayerController, PickaxeDef, EFortQuickBars::Primary, 0, 1);

		if (!Helper::HasAircraftStarted() || !bClearInventoryOnAircraftJump)
		{
			Inventory::CreateAndAddItem(PlayerController, First, EFortQuickBars::Primary, 1, StartingSlot1.second, true);
			Inventory::CreateAndAddItem(PlayerController, Second, EFortQuickBars::Primary, 2, StartingSlot2.second, true);
			Inventory::CreateAndAddItem(PlayerController, Third, EFortQuickBars::Primary, 3, StartingSlot3.second, true);
			Inventory::CreateAndAddItem(PlayerController, Fourth, EFortQuickBars::Primary, 4, StartingSlot4.second, true);
			Inventory::CreateAndAddItem(PlayerController, Fifth, EFortQuickBars::Primary, 5, StartingSlot5.second, true);

			if (bIsPlayground)
			{
				Inventory::GiveAllAmmo(PlayerController);
				Inventory::GiveMats(PlayerController);
			}
		}
	}

	/* static auto SeasonLevelUIDisplayOffset = GetOffset(PlayerState, "SeasonLevelUIDisplay");
	
	if (SeasonLevelUIDisplayOffset != -1)
	{
		*(int*)(__int64(PlayerState) + SeasonLevelUIDisplayOffset) = 1;
		PlayerState->ProcessEvent("OnRep_SeasonLevelUIDisplay");
	} */

	std::cout << ("Spawned Player!\n");

	/* if (FnVerDouble >= 8) // just make the color blue // bruh it sets it to team idx 2 after ??
	{
		auto CurrentTeamIdx = Teams::StartingTeamIndex;
		*Teams::GetTeamIndex(PlayerState) = CurrentTeamIdx;

		auto SquadId = CurrentTeamIdx;

		static auto SquadIdOffset = GetOffset(PlayerState, "SquadId");
		*(uint8_t*)(__int64(PlayerState) + SquadIdOffset) = SquadId;
	} */

	return PlayerController;
}

static bool __fastcall HasClientLoadedCurrentWorldDetour(UObject* pc)
{
	std::cout << ("yes\n");
	return true;
}

void* NetDebugDetour(UObject* idk)
{
	return nullptr;
}

FString* GetRequestURL(UObject* Connection)
{
	if (FnVerDouble >= 7 && Engine_Version < 424)
		return (FString*)(__int64(Connection) + 424);
	else if (FnVerDouble < 7)
		return (FString*)((__int64*)Connection + 54);
	else if (Engine_Version >= 424)
		return (FString*)(__int64(Connection) + 440);

	return nullptr;
}

void World_NotifyControlMessageDetour(UObject* World, UObject* Connection, uint8_t MessageType, __int64* Bunch)
{
	auto correctWorld = World;
	std::cout << ("Receieved control message: ") << std::to_string((int)MessageType) << '\n';

	switch (MessageType)
	{
	case 0:
	{
		if (Helper::HasAircraftStarted() && !bIsPlayground)
		{
			std::cout << "Denying join because aircraft has started!\n";
			return;
		}

		break;
	}
	case 4: // NMT_Netspeed // Do we even have to rei,plment this?
		// if (Engine_Version >= 423)
		  //  *Connection->Member<int>(("CurrentNetSpeed")) = 60000; // sometimes 60000
		// else
		static auto CurrentNetSpeedOffset = GetOffset(Connection, "CurrentNetSpeed");
		*(int*)(__int64(Connection) + CurrentNetSpeedOffset) = 30000; // sometimes 60000
		return;
	case 5: // NMT_Login
	{
		if (Engine_Version >= 420)
		{
			Bunch[7] += (16 * 1024 * 1024);

			FString OnlinePlatformName;

			static double CurrentFortniteVersion = FnVerDouble;

			static auto PlayerIDOffset = GetOffset(Connection, "PlayerID");

			if (FnVerDouble < 19.00)
			{
				if (CurrentFortniteVersion >= 7 && Engine_Version < 424)
					ReceiveFString(Bunch, *(FString*)(__int64(Connection) + 400)); // clientresponse
				else if (CurrentFortniteVersion < 7)
					ReceiveFString(Bunch, *(FString*)((__int64*)Connection + 51));
				else if (Engine_Version >= 424)
					ReceiveFString(Bunch, *(FString*)(__int64(Connection) + 416));

				auto RequestURL = GetRequestURL(Connection);

				if (!RequestURL)
					return;

				ReceiveFString(Bunch, *RequestURL);

				ReceiveUniqueIdRepl(Bunch, (__int64*)(__int64(Connection) + PlayerIDOffset));
				ReceiveFString(Bunch, OnlinePlatformName);

				if (OnlinePlatformName.Data.GetData())
					std::cout << ("OnlinePlatformName => ") << OnlinePlatformName.ToString() << '\n';
			}
			else
			{
				auto addr = FindPattern("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 49 8B F9 49 8B D8 48 8B F1 E8 ? ? ? ? 48 8B D3 48 8B CE E8 ? ? ? ? 48 8B D7 48 8B CE E8 ? ? ? ? 48 8B 54 24 ? 48 8B CE E8 ? ? ? ?");
				
				bool (__fastcall* receiveloginthingy)(__int64* Bunch, FString clientResponse, FString reqeustURL, FUniqueNetIdRepl uniqueId, FString OnlinePlatformName);

				receiveloginthingy = decltype(receiveloginthingy)(addr);

				// *(char*)(Bunch + 40) = -1;

				std::cout << "res: " << 
					receiveloginthingy(Bunch, *(FString*)(__int64(Connection) + 416), *GetRequestURL(Connection), *(FUniqueNetIdRepl*)(__int64(Connection) + PlayerIDOffset), OnlinePlatformName)
					<< '\n';
			}

			Bunch[7] -= (16 * 1024 * 1024);

			WelcomePlayer(correctWorld, Connection);

			if (false) // TODO: Test
			{
				static auto sizeOfHistogram = 16 + 4 + 8 + 4 + 4;

				static auto PlayerOnlinePlatformNameOffset = GetOffset(Connection, "ChannelsToTick") + 16 + sizeOfHistogram;
				auto PlayerOnlinePlatformName = (FName*)(__int64(Connection) + PlayerOnlinePlatformNameOffset);

				*PlayerOnlinePlatformName = Helper::StringToName(OnlinePlatformName);
			}

			return;
		}
		break;
	}
	}

	return World_NotifyControlMessage(correctWorld, Connection, MessageType, Bunch);
}

static bool fau2881q = false;

void Beacon_NotifyControlMessageDetour(UObject* Beacon, UObject* Connection, uint8_t MessageType, __int64* Bunch)
{
	// if (fau2881q)
		// return World_NotifyControlMessage(Helper::GetWorld(), Connection, MessageType, Bunch);

	if (MessageType == 15)
	{
		std::cout << "NMT_PCSwap this should not happen!\n";
		return;
	}

	std::cout << "beacon ncm!\n";
	World_NotifyControlMessageDetour(Helper::GetWorld(), Connection, MessageType, Bunch);
}

char __fastcall NoReserveDetour(__int64* a1, __int64 a2, char a3, __int64* a4)
{
	std::cout << ("No Reserve!\n");
	return 0;
}

__int64 CollectGarbageDetour(__int64) { return 0; }

bool TryCollectGarbageHook()
{
	return 0;
}

void InitializeNetHooks()
{
	MH_CreateHook((PVOID)SpawnPlayActorAddr, SpawnPlayActorDetour, (void**)&SpawnPlayActor);
	MH_EnableHook((PVOID)SpawnPlayActorAddr);

	if (FnVerDouble < 11.00)
	{
		if (std::floor(FnVerDouble) != 13 && Engine_Version != 421 && Engine_Version != 419 && Engine_Version != 422 && Engine_Version != 416 && Engine_Version != 423 
			&& Engine_Version != 420) // we dont really need this im just too lazy to get setworld sig
		{
			MH_CreateHook((PVOID)Beacon_NotifyControlMessageAddr, Beacon_NotifyControlMessageDetour, (void**)&Beacon_NotifyControlMessage);
			MH_EnableHook((PVOID)Beacon_NotifyControlMessageAddr);

			MH_CreateHook((PVOID)World_NotifyControlMessageAddr, World_NotifyControlMessageDetour, (void**)&World_NotifyControlMessage);
			MH_EnableHook((PVOID)World_NotifyControlMessageAddr);
		}
	}

	if (std::floor(FnVerDouble) == 13 || std::floor(FnVerDouble) == 14 || std::floor(FnVerDouble) == 18) // bruhhh
	{
		MH_CreateHook((PVOID)Beacon_NotifyControlMessageAddr, Beacon_NotifyControlMessageDetour, (void**)&Beacon_NotifyControlMessage);
		MH_EnableHook((PVOID)Beacon_NotifyControlMessageAddr);

		fau2881q = true;
	}

	if (Engine_Version < 424 && GetNetModeAddr) // i dont even think we have to hook this
	{
		MH_CreateHook((PVOID)GetNetModeAddr, GetNetModeDetour, (void**)&GetNetMode);
		MH_EnableHook((PVOID)GetNetModeAddr);
	}

	if (Engine_Version > 423)
	{
		MH_CreateHook((PVOID)ValidationFailureAddr, ValidationFailureDetour, (void**)&ValidationFailure);
		MH_EnableHook((PVOID)ValidationFailureAddr);
	}

	MH_CreateHook((PVOID)TickFlushAddr, TickFlushDetour, (void**)&TickFlush);
	MH_EnableHook((PVOID)TickFlushAddr);

	if (KickPlayer)
	{
		// std::cout << "Hooked kickplayer!\n";
		MH_CreateHook((PVOID)KickPlayerAddr, KickPlayerDetour, (void**)&KickPlayer);
		MH_EnableHook((PVOID)KickPlayerAddr);
	}

	if (NoReserveAddr)
	{
		MH_CreateHook((PVOID)NoReserveAddr, NoReserveDetour, (void**)&NoReserve);
		MH_EnableHook((PVOID)NoReserveAddr);
	}

	if (NetDebugAddr && Engine_Version >= 419 && FnVerDouble < 17.00)
	{
		MH_CreateHook((PVOID)NetDebugAddr, NetDebugDetour, (void**)&NetDebug);
		MH_EnableHook((PVOID)NetDebugAddr);
	}

	if (CollectGarbageAddr)
	{
		// if (Engine_Version < 424)
		{
			if (Engine_Version >= 422 && Engine_Version < 424)
			{
				MH_CreateHook((PVOID)CollectGarbageAddr, TryCollectGarbageHook, nullptr);
				MH_EnableHook((PVOID)CollectGarbageAddr);
			}
			else
			{
				MH_CreateHook((PVOID)CollectGarbageAddr, CollectGarbageDetour, (void**)&CollectGarbage);
				MH_EnableHook((PVOID)CollectGarbageAddr);
			}
		}
	}
	else
		std::cout << ("[WARNING] Unable to hook CollectGarbage!\n");
}

void DisableNetHooks()
{
	MH_DisableHook((PVOID)SpawnPlayActorAddr);
	MH_DisableHook((PVOID)Beacon_NotifyControlMessageAddr);
	MH_DisableHook((PVOID)World_NotifyControlMessageAddr);

	if (Engine_Version < 424 && GetNetModeAddr) // i dont even think we have to hook this
	{
		MH_DisableHook((PVOID)GetNetModeAddr);
	}

	if (Engine_Version > 423)
	{
		MH_DisableHook((PVOID)ValidationFailureAddr);
	}

	MH_DisableHook((PVOID)TickFlushAddr);

	if (KickPlayer)
	{
		MH_DisableHook((PVOID)KickPlayerAddr);
	}

	if (LP_SpawnPlayActorAddr)
	{
		MH_DisableHook((PVOID)LP_SpawnPlayActorAddr);
	}

	if (NoReserveAddr)
	{
		MH_DisableHook((PVOID)NoReserveAddr);
	}

	// if (NetDebug)
	{
		if (NetDebugAddr)
		{
			MH_DisableHook((PVOID)NetDebugAddr);
		}

		if (CollectGarbageAddr)
		{
			if (Engine_Version >= 422 && Engine_Version < 424)
			{
				MH_DisableHook((PVOID)CollectGarbageAddr);
			}
			else
			{
				MH_DisableHook((PVOID)CollectGarbageAddr);
			}
		}
		else
			std::cout << ("[WARNING] Unable to hook CollectGarbage!\n");
	}
}
