#pragma once

#include <MinHook/MinHook.h>

#include <Net/funcs.h>
#include <Gameplay/helper.h>
#include <Gameplay/inventory.h>
#include <Gameplay/abilities.h>
#include <Gameplay/loot.h>

#include <discord.h>
#include <Net/replication.h>

static bool bTraveled = false;

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

		*Object->Member<UObject*>(("AcknowledgedPawn")) = Pawn;
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
			static auto& ClientConnections = *NetDriver->Member<TArray<UObject*>>(("ClientConnections"));

			if (ClientConnections.Num() > 0)
			{
				ServerReplicateActors(NetDriver);
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
	static bool bHasdonething = false;
	
	if (!bHasdonething)
	{
		bHasdonething = true;

		if (Engine_Version >= 423)
			Helper::GetGameState()->ProcessEvent("OnRep_CurrentPlaylistInfo"); // fix battle bus lol
	}
	
	std::cout << ("SpawnPlayActor called!\n");
	auto PlayerController = SpawnPlayActor(Helper::GetWorld(), NewPlayer, RemoteRole, URL, UniqueId, Error, NetPlayerIndex); // crashes 0x356 here sometimes when rejoining
	static auto FortPlayerControllerAthenaClass = FindObject(("Class /Script/FortniteGame.FortPlayerControllerAthena"));

	if (!PlayerController || !PlayerController->IsA(FortPlayerControllerAthenaClass))
		return nullptr;

	auto PlayerState = *PlayerController->Member<UObject*>(("PlayerState"));

	if (!PlayerState) // this happened somehow
		return PlayerController;

	auto OPlayerName = Helper::GetPlayerName(PlayerController);
	std::string PlayerName = OPlayerName;
	std::transform(OPlayerName.begin(), OPlayerName.end(), OPlayerName.begin(), ::tolower);

	if (OPlayerName.contains(("fuck")) || OPlayerName.contains(("shit")))
	{
		FString Reason;
		Reason.Set(L"Inappropriate name!");
		Helper::KickController(PlayerController, Reason);
	}

	if (OPlayerName.length() >= 40)
	{
		FString Reason;
		Reason.Set(L"Too long of a name!");
		Helper::KickController(PlayerController, Reason);
	}

	*NewPlayer->Member<UObject*>(("PlayerController")) = PlayerController;

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

	*PlayerState->Member<char>(("bHasStartedPlaying")) = true;
	*PlayerState->Member<char>(("bHasFinishedLoading")) = true;
	*PlayerState->Member<char>(("bIsReadyToContinue")) = true;

	auto Pawn = Helper::InitPawn(PlayerController, true, Helper::GetPlayerStart(), true);

	if (!Pawn)
		return PlayerController;

	if (GiveAbility || GiveAbilityFTS || GiveAbilityNewer)
	{
		auto AbilitySystemComponent = *Pawn->Member<UObject*>(("AbilitySystemComponent"));

		if (AbilitySystemComponent)
		{
			std::cout << ("Granting abilities!\n");

			if (FnVerDouble < 8) // idk CDO offset
			{
				static auto AbilitySet = FindObject(("FortAbilitySet /Game/Abilities/Player/Generic/Traits/DefaultPlayer/GAS_DefaultPlayer.GAS_DefaultPlayer"));

				if (AbilitySet)
				{
					auto Abilities = AbilitySet->Member<TArray<UObject*>>(("GameplayAbilities"));

					if (Abilities)
					{
						for (int i = 0; i < Abilities->Num(); i++)
						{
							auto Ability = Abilities->At(i);

							if (!Ability)
								continue;

							Abilities::GrantGameplayAbility(Pawn, Ability);
							std::cout << "Granting ability " << Ability->GetFullName() << '\n';
						}
					}
				}

				static auto RangedAbility = FindObject(("BlueprintGeneratedClass /Game/Abilities/Weapons/Ranged/GA_Ranged_GenericDamage.GA_Ranged_GenericDamage_C"));

				if (RangedAbility)
					Abilities::GrantGameplayAbility(Pawn, RangedAbility);
			}
			else
			{
				{
					static auto AbilitySet = FindObject(("FortAbilitySet /Game/Abilities/Player/Generic/Traits/DefaultPlayer/GAS_AthenaPlayer.GAS_AthenaPlayer"));

					if (AbilitySet)
					{
						auto Abilities = AbilitySet->Member<TArray<UObject*>>(("GameplayAbilities"));

						if (Abilities)
						{
							for (int i = 0; i < Abilities->Num(); i++)
							{
								auto Ability = Abilities->At(i);

								if (!Ability)
									continue;

								Abilities::GrantGameplayAbility(Pawn, Ability);
								std::cout << "Granting ability " << Ability->GetFullName() << '\n';
							}
						}
					}
				}

			}

			if (Engine_Version < 424) // i dont think needed
			{
				static auto EmoteAbility = FindObject(("BlueprintGeneratedClass /Game/Abilities/Emotes/GAB_Emote_Generic.GAB_Emote_Generic_C"));

				if (EmoteAbility)
				{
					Abilities::GrantGameplayAbility(Pawn, EmoteAbility);
				}
			}

			std::cout << ("Granted Abilities!\n");
		}
		else
			std::cout << ("Unable to find AbilitySystemComponent!\n");
	}
	else
		std::cout << ("Unable to grant abilities due to no GiveAbility!\n");

	// if (FnVerDouble >= 4.0) // if (std::floor(FnVerDouble) != 9 && std::floor(FnVerDouble) != 10 && Engine_Version >= 420) // if (FnVerDouble < 15) // if (Engine_Version < 424) // if (FnVerDouble < 9) // (std::floor(FnVerDouble) != 9)
	// if (FnVerDouble < 16.00)

	// itementrysize 0xb0 on 2.4.2

	if (Engine_Version >= 420)
	{
		static auto PickaxeDef = FindObject(("FortWeaponMeleeItemDefinition /Game/Athena/Items/Weapons/WID_Harvest_Pickaxe_Athena_C_T01.WID_Harvest_Pickaxe_Athena_C_T01"));

		static auto First = FindObject(StartingSlot1.first);
		static auto Second = FindObject(StartingSlot2.first);
		static auto Third = FindObject(StartingSlot3.first);
		static auto Fourth = FindObject(StartingSlot4.first);
		static auto Fifth = FindObject(StartingSlot5.first);

		Inventory::CreateAndAddItem(PlayerController, PickaxeDef, EFortQuickBars::Primary, 0, 1);
		if (FnVerDouble < 5.0) {
			// Gives the needed items like edit tool and builds
			Inventory::GiveStartingItems(PlayerController);
		}
		if (!Helper::HasAircraftStarted() || !bClearInventoryOnAircraftJump)
		{
			Inventory::CreateAndAddItem(PlayerController, First, EFortQuickBars::Primary, 1, StartingSlot1.second);
			Inventory::CreateAndAddItem(PlayerController, Second, EFortQuickBars::Primary, 2, StartingSlot2.second);
			Inventory::CreateAndAddItem(PlayerController, Third, EFortQuickBars::Primary, 3, StartingSlot3.second);
			Inventory::CreateAndAddItem(PlayerController, Fourth, EFortQuickBars::Primary, 4, StartingSlot4.second);
			Inventory::CreateAndAddItem(PlayerController, Fifth, EFortQuickBars::Primary, 5, StartingSlot5.second);

			Inventory::GiveAllAmmo(PlayerController);
			Inventory::GiveMats(PlayerController);
		}
		if (FnVerDouble >= 5.0) {
			// Gives the needed items like edit tool and builds
			Inventory::GiveStartingItems(PlayerController);
		}
	}

	if (false && FnVerDouble >= 7.40)
	{
		struct FUniqueNetIdRepl
		{
			unsigned char UnknownData00[0x1];
			unsigned char UnknownData01[0x17];
			TArray<unsigned char> ReplicationBytes;
		};

		static auto GameState = Helper::GetGameState();
		auto GameMemberInfoArray = GameState->Member<__int64>("GameMemberInfoArray");

		static int TeamIDX = 4;

		auto teamIndexThing = PlayerState->Member<uint8_t>("TeamIndex");

		auto oldTeamIDX = *teamIndexThing;

		*teamIndexThing = TeamIDX;
		*PlayerState->Member<uint8_t>("SquadId") = TeamIDX;

		static auto SquadIdInfoOffset = FindOffsetStruct("ScriptStruct /Script/FortniteGame.GameMemberInfo", "SquadId");
		static auto TeamIndexOffset = FindOffsetStruct("ScriptStruct /Script/FortniteGame.GameMemberInfo", "TeamIndex");
		static auto MemberUniqueIdOffset = FindOffsetStruct("ScriptStruct /Script/FortniteGame.GameMemberInfo", "MemberUniqueId");

		static auto GameMemberInfoStruct = FindObject("ScriptStruct /Script/FortniteGame.GameMemberInfo");
		static auto SizeOfGameMemberInfo = GetSizeOfStruct(GameMemberInfoStruct);
		auto NewInfo = (__int64*)malloc(SizeOfGameMemberInfo);

		static auto UniqueIdSize = GetSizeOfStruct(FindObject("ScriptStruct /Script/Engine.UniqueNetIdRepl"));

		std::cout << "acutal size: " << UniqueIdSize << '\n';
		std::cout << "ours: " << sizeof(FUniqueNetIdRepl) << '\n';

		RtlSecureZeroMemory(NewInfo, SizeOfGameMemberInfo);

		*(uint8_t*)(__int64(NewInfo) + SquadIdInfoOffset) = TeamIDX;
		*(uint8_t*)(__int64(NewInfo) + TeamIndexOffset) = TeamIDX;
		*(FUniqueNetIdRepl*)(__int64(NewInfo) + MemberUniqueIdOffset) = *PlayerState->Member<FUniqueNetIdRepl>("UniqueId");

		static auto MembersOffset = FindOffsetStruct("ScriptStruct /Script/FortniteGame.GameMemberInfoArray", "Members");

		auto Members = (TArray<__int64>*)(__int64(GameMemberInfoArray) + MembersOffset);

		MarkArrayDirty(GameMemberInfoArray);
		Members->Add(*NewInfo, SizeOfGameMemberInfo);
		MarkArrayDirty(GameMemberInfoArray);

		auto PlayerTeam = PlayerState->Member<UObject*>("PlayerTeam");

		(*PlayerTeam)->Member<TArray<UObject*>>("TeamMembers")->Add(PlayerController);

		static auto OnRep_SquadId = PlayerState->Function("OnRep_SquadId");
		static auto OnRep_PlayerTeam = PlayerState->Function("OnRep_PlayerTeam");
		static auto OnRep_TeamIndex = PlayerState->Function("OnRep_TeamIndex");

		PlayerState->ProcessEvent(OnRep_SquadId);
		PlayerState->ProcessEvent(OnRep_PlayerTeam);
		PlayerState->ProcessEvent(OnRep_TeamIndex, &oldTeamIDX);
	}
	else
	{
		static int Team = 4;
		Team++;

		std::cout << "After Team: " << Team << '\n';

		*PlayerState->Member<uint8_t>("TeamIndex") = Team;
		*PlayerState->Member<uint8_t>("SquadId") = Team;
	}

	Inventory::Update(PlayerController);

	std::cout << ("Spawned Player!\n");

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

char __fastcall malformedDetour(__int64 a1, __int64 a2)
{
	/* 9.41:

	v20 = *(_QWORD *)(a1 + 48);
	*(double *)(a1 + 0x430) = v19;
	if ( v20 )
	  (*(void (__fastcall **)(__int64))(*(_QWORD *)v20 + 0xC90i64))(v20);// crashes

	*/

	auto old = *(__int64*)(a1 + 48);
	std::cout << "Old: " << old << '\n';
	*(__int64*)(a1 + 48) = 0;
	std::cout << "Old2: " << old << '\n';
	auto ret = malformed(a1, a2);
	*(__int64*)(a1 + 48) = old;
	return ret;
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
	std::cout << ("Receieved control message: ") << std::to_string((int)MessageType) << '\n';

	switch (MessageType)
	{
	case 0:
	{
		//S5-6 Fix
		if (Engine_Version == 421) // not the best way to fix it...
		{
			{
				/* uint8_t */ char IsLittleEndian = 0;
				unsigned int RemoteNetworkVersion = 0;
				auto v38 = (__int64*)Bunch[1];
				FString EncryptionToken; // This should be a short* but ye

				if ((unsigned __int64)(*v38 + 1) > v38[1])
					(*(void(__fastcall**)(__int64*, char*, __int64))(*Bunch + 72))(Bunch, &IsLittleEndian, 1);
				else
					IsLittleEndian = *(char*)(*v38)++;
				auto v39 = Bunch[1];
				if ((unsigned __int64)(*(__int64*)v39 + 4) > *(__int64*)(v39 + 8))
				{
					(*(void(__fastcall**)(__int64*, unsigned int*, __int64))(*Bunch + 72))(Bunch, &RemoteNetworkVersion, 4);
					if ((*((char*)Bunch + 41) & 0x10) != 0)
						Idkf(__int64(Bunch), __int64(&RemoteNetworkVersion), 4);
				}
				else
				{
					RemoteNetworkVersion = **(int32_t**)v39;
					*(__int64*)v39 += 4;
				}

				ReceiveFString(Bunch, EncryptionToken);
				// if (*((char*)Bunch + 40) < 0)
					// do stuff

				std::cout << ("EncryptionToken: ") << __int64(EncryptionToken.Data.GetData()) << '\n';

				if (EncryptionToken.Data.GetData())
					std::cout << ("EncryptionToken Str: ") << EncryptionToken.ToString() << '\n';

				std::cout << ("Sending challenge!\n");
				SendChallenge(World, Connection);
				std::cout << ("Sent challenge!\n");
				// World_NotifyControlMessage(World, Connection, MessageType, (void*)Bunch);
				// std::cout << "IDK: " << *((char*)Bunch + 40) << '\n';
				return;
			}
		}
		break;
	}
	case 4: // NMT_Netspeed // Do we even have to rei,plment this?
		// if (Engine_Version >= 423)
		  //  *Connection->Member<int>(("CurrentNetSpeed")) = 60000; // sometimes 60000
		// else
		*Connection->Member<int>(("CurrentNetSpeed")) = 30000; // sometimes 60000
		return;
	case 5: // NMT_Login
	{
		if (Engine_Version >= 420)
		{
			Bunch[7] += (16 * 1024 * 1024);

			FString OnlinePlatformName;

			static double CurrentFortniteVersion = FnVerDouble;

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

			ReceiveUniqueIdRepl(Bunch, Connection->Member<__int64>(("PlayerID")));
			ReceiveFString(Bunch, OnlinePlatformName);
			if (OnlinePlatformName.Data.GetData())
				std::cout << ("OnlinePlatformName => ") << OnlinePlatformName.ToString() << '\n';

			Bunch[7] -= (16 * 1024 * 1024);

			WelcomePlayer(Helper::GetWorld(), Connection);

			return;
		}
		break;
	}
	case 6:
		return;
	case 9: // NMT_Join
	{
		if (Engine_Version == 421 || Engine_Version < 420)
		{
			auto ConnectionPC = Connection->Member<UObject*>(("PlayerController"));
			if (!*ConnectionPC)
			{
				FURL InURL;
				InURL.Map.Set(L"/Game/Maps/Frontend");
				InURL.Valid = 1;
				InURL.Protocol.Set(L"unreal");
				InURL.Port = 7777;

				// ^ This was all taken from 3.5

				FString ErrorMsg;
				SpawnPlayActorDetour(World, Connection, ENetRole::ROLE_AutonomousProxy, InURL, Connection->Member<void>(("PlayerID")), ErrorMsg, 0);

				if (!*ConnectionPC)
				{
					std::cout << ("Failed to spawn PlayerController! Error Msg: ") << ErrorMsg.ToString() << "\n";
					return;
				}
				else
				{
					std::cout << ("Join succeeded!\n");
					FString LevelName;
					const bool bSeamless = true;

					static auto ClientTravelFn = (*ConnectionPC)->Function(("ClientTravel"));
					struct {
						FString URL;
						ETravelType TravelType;
						bool bSeamless;
						FGuid MapPackageGuid;
					} paramsTravel{ LevelName, ETravelType::TRAVEL_Relative, bSeamless, FGuid() };

					(*ConnectionPC)->ProcessEvent(("ClientTravel"), &paramsTravel);

					std::cout << ("Traveled client.\n");

					*(int32_t*)(__int64(&*Connection->Member<int>(("LastReceiveTime"))) + (sizeof(double) * 4) + (sizeof(int32_t) * 2)) = 0;
				}
				return;
			}
		}
		break;
	}
	}

	return World_NotifyControlMessage(Helper::GetWorld(), Connection, MessageType, Bunch);
}

char Beacon_NotifyControlMessageDetour(UObject* Beacon, UObject* Connection, uint8_t MessageType, __int64* Bunch)
{
	std::cout << "beacon ncm!\n";
	World_NotifyControlMessageDetour(Helper::GetWorld(), Connection, MessageType, Bunch);
	return true;
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

int __fastcall ReceivedPacketDetour(__int64* a1, LARGE_INTEGER a2, char a3)
{
}

void InitializeNetHooks()
{
	MH_CreateHook((PVOID)SpawnPlayActorAddr, SpawnPlayActorDetour, (void**)&SpawnPlayActor);
	MH_EnableHook((PVOID)SpawnPlayActorAddr);

	MH_CreateHook((PVOID)Beacon_NotifyControlMessageAddr, Beacon_NotifyControlMessageDetour, (void**)&Beacon_NotifyControlMessage);
	MH_EnableHook((PVOID)Beacon_NotifyControlMessageAddr);

	MH_CreateHook((PVOID)World_NotifyControlMessageAddr, World_NotifyControlMessageDetour, (void**)&World_NotifyControlMessage);
	MH_EnableHook((PVOID)World_NotifyControlMessageAddr);

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
		std::cout << "Hooked kickplayer!\n";
		MH_CreateHook((PVOID)KickPlayerAddr, KickPlayerDetour, (void**)&KickPlayer);
		MH_EnableHook((PVOID)KickPlayerAddr);
	}

	if (LP_SpawnPlayActorAddr)
	{
		MH_CreateHook((PVOID)LP_SpawnPlayActorAddr, LP_SpawnPlayActorDetour, (void**)&LP_SpawnPlayActor);
		MH_EnableHook((PVOID)LP_SpawnPlayActorAddr);
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
