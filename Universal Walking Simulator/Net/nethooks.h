#pragma once

#include <MinHook/MinHook.h>

#include <Gameplay/helper.h>
#include <Gameplay/inventory.h>
#include <Gameplay/ability.h>

#include <Net/replication.h>

#include "Gameplay/gamemode.h"
#include "UE/hooks.h"

namespace NetHooks
{
    inline auto Traveled = false;
    inline auto BusFix = false;

    // DOCUMENTATION: This fixes movement on S5+, Cat led me to the right direction, then I figured out it's something with ClientRestart and did some common sense and found this.
    inline bool ServerAcknowledgePossessionHook(UObject* Object, [[maybe_unused]] UFunction* Function, void* Parameters)
    {
        const auto Params = static_cast<FixMovementParameters*>(Parameters);
        if (!Params)
        {
            return false;
        }

        const auto Pawn = Params->Object;
        *Object->Member<UObject*>("AcknowledgedPawn") = Pawn;
        return false;
    }

    inline void InitUHooks()
    {
        Hooks::Add("Function /Script/Engine.PlayerController.ServerAcknowledgePossession",
                ServerAcknowledgePossessionHook);
    }

    inline void TickFlushDetour(UObject* ThisNetDriver, const float DeltaSeconds)
    {
        auto World = Helper::GetWorld();

        if (!World)
        {
            return TickFlush(ThisNetDriver, DeltaSeconds);
        }

        static auto NetDriverOffset = GetOffset(World, "NetDriver");
        const auto NetDriver = *reinterpret_cast<UObject**>(reinterpret_cast<long long>(World) + NetDriverOffset);

        if (!NetDriver)
        {
            return TickFlush(ThisNetDriver, DeltaSeconds);
        }

        if (static auto ClientConnections = *NetDriver->Member<TArray<UObject*>>(("ClientConnections"));
            ClientConnections.Num() <= 0)
        {
            return TickFlush(ThisNetDriver, DeltaSeconds);
        }

        RepGraphServerReplicateActors(NetDriver);
        return TickFlush(ThisNetDriver, DeltaSeconds);
    }

    inline auto GetNetModeDetour([[maybe_unused]] UObject* World)
    {
        return NM_ListenServer;
    }

    inline auto LpSpawnPlayActorDetour(UObject* Player, const FString& URL, FString& OutError, UObject* World)
    {
        if (!Traveled)
        {
            return LpSpawnPlayActor(Player, URL, OutError, World);
        }

        return true;
    }

    inline char KickPlayerDetour(long long, long long, long long)
    {
        return 0;
    }

    inline char __fastcall ValidationFailureDetour(long long, long long)
    {
        return 0;
    }

    // FIXME: Crashes 0x356 here sometimes when rejoining
    inline auto SpawnPlayActorDetour(UObject* NewPlayer, const ENetRole RemoteRole, FURL& Url, void* UniqueId,
                                     FString& Error, const uint8_t NetPlayerIndex) -> UObject*
    {
        if (!BusFix)
        {
            BusFix = true;
            if (EngineVersion >= 423)
            {
                Helper::GetGameState()->ProcessEvent("OnRep_CurrentPlaylistInfo");
            }
        }

        const auto PlayerController = SpawnPlayActor(Helper::GetWorld(), NewPlayer, RemoteRole, Url, UniqueId, Error,
                                                     NetPlayerIndex);
        if (static auto FortPlayerControllerAthenaClass = FindObject(
                "Class /Script/FortniteGame.FortPlayerControllerAthena"); !PlayerController || !
            PlayerController
            ->IsA(FortPlayerControllerAthenaClass))
        {
            return nullptr;
        }

        const auto PlayerState = *PlayerController->Member<UObject*>(("PlayerState"));
        if (!PlayerState)
        {
            return PlayerController;
        }

        auto OPlayerName = Helper::GetPlayerName(PlayerController);
        std::string PlayerName = OPlayerName;
        std::ranges::transform(OPlayerName, OPlayerName.begin(), ::tolower);

        if (OPlayerName.contains("fuck") || OPlayerName.contains("shit"))
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

        *NewPlayer->Member<UObject*>("PlayerController") = PlayerController;
        if (FortniteVersion < 7.4)
        {
            static const auto QuickBarsClass = FindObject("Class /Script/FortniteGame.FortQuickBars", true);
            if (const auto QuickBars = PlayerController->Member<UObject*>("QuickBars"))
            {
                *QuickBars = Easy::SpawnActor(QuickBarsClass, FVector(), FRotator());
                Helper::SetOwner(*QuickBars, PlayerController);
            }
        }

        *PlayerController->Member<char>("bReadyToStartMatch") = true;
        *PlayerController->Member<char>("bClientPawnIsLoaded") = true;
        *PlayerController->Member<char>("bHasInitiallySpawned") = true;

        *PlayerController->Member<bool>("bHasServerFinishedLoading") = true;
        *PlayerController->Member<bool>("bHasClientFinishedLoading") = true;

        *PlayerState->Member<char>("bHasStartedPlaying") = true;
        *PlayerState->Member<char>("bHasFinishedLoading") = true;
        *PlayerState->Member<char>("bIsReadyToContinue") = true;

        const auto Pawn = Helper::InitPawn(PlayerController, true, Helper::GetPlayerStart(), true);
        if (!Pawn)
        {
            return PlayerController;
        }

        if (GiveAbility || GiveAbilityFts || GiveAbilityNewer)
            if (FortniteVersion < 8)
            {
                if (static auto AbilitySet = FindObject(
                    "FortAbilitySet /Game/Abilities/Player/Generic/Traits/DefaultPlayer/GAS_DefaultPlayer.GAS_DefaultPlayer"))
                {
                    if (const auto Abilities = AbilitySet->Member<TArray<UObject*>>(("GameplayAbilities")))
                    {
                        for (int i = 0; i < Abilities->Num(); i++)
                        {
                            const auto Ability = Abilities->At(i);
                            if (!Ability)
                            {
                                continue;
                            }

                            Ability::EnableGameplayAbility(Pawn, Ability);
                        }
                    }
                }

                if (static const auto RangedAbility = FindObject(
                    "BlueprintGeneratedClass /Game/Abilities/Weapons/Ranged/GA_Ranged_GenericDamage.GA_Ranged_GenericDamage_C"))
                {
                    Ability::EnableGameplayAbility(Pawn, RangedAbility);
                }
            }
            else
            {
                {
                    if (static const auto AbilitySet = FindObject(
                        "FortAbilitySet /Game/Abilities/Player/Generic/Traits/DefaultPlayer/GAS_AthenaPlayer.GAS_AthenaPlayer"))
                    {
                        if (const auto Abilities = AbilitySet->Member<TArray<UObject*>>("GameplayAbilities"))
                        {
                            for (int i = 0; i < Abilities->Num(); i++)
                            {
                                const auto Ability = Abilities->At(i);
                                if (!Ability)
                                {
                                    continue;
                                }

                                Ability::EnableGameplayAbility(Pawn, Ability);
                            }
                        }
                    }
                }
            }

        if (EngineVersion < 424)
        {
            if (static const auto EmoteAbility = FindObject(
                "BlueprintGeneratedClass /Game/Abilities/Emotes/GAB_Emote_Generic.GAB_Emote_Generic_C"))
            {
                Ability::EnableGameplayAbility(Pawn, EmoteAbility);
            }
        }


        if (EngineVersion >= 420)
        {
            static const auto First = FindObject(Item::StartingSlot1.first);
            static const auto Second = FindObject(Item::StartingSlot2.first);
            static const auto Third = FindObject(Item::StartingSlot3.first);
            static const auto Fourth = FindObject(Item::StartingSlot4.first);
            static const auto Fifth = FindObject(Item::StartingSlot5.first);

            Inventory::GiveBattleRoyalItems(PlayerController);

            if (GameMode::IsPlayground())
            {
                Inventory::CreateAndAddItem(PlayerController, First, EFortQuickBars::Primary, 1,
                                            Item::StartingSlot1.second);
                Inventory::CreateAndAddItem(PlayerController, Second, EFortQuickBars::Primary, 2,
                                            Item::StartingSlot2.second);
                Inventory::CreateAndAddItem(PlayerController, Third, EFortQuickBars::Primary, 3,
                                            Item::StartingSlot3.second);
                Inventory::CreateAndAddItem(PlayerController, Fourth, EFortQuickBars::Primary, 4,
                                            Item::StartingSlot4.second);
                Inventory::CreateAndAddItem(PlayerController, Fifth, EFortQuickBars::Primary, 5,
                                            Item::StartingSlot5.second);

                Inventory::GiveAllAmmo(PlayerController);
                Inventory::GiveMaxMats(PlayerController);
            }
        }

        static int Team = 4;
        Team++;

        *PlayerState->Member<uint8_t>("TeamIndex") = Team;
        *PlayerState->Member<uint8_t>("SquadId") = Team;

        Inventory::Update(PlayerController);

        return PlayerController;
    }

    inline void* NetDebugDetour(UObject*)
    {
        return nullptr;
    }

    inline auto GetRequestUrl(UObject* Connection) -> FString*
    {
        if (FortniteVersion >= 7 && EngineVersion < 424)
        {
            return reinterpret_cast<FString*>(reinterpret_cast<long long>(Connection) + 424);
        }

        if (FortniteVersion < 7)
        {
            return reinterpret_cast<FString*>(reinterpret_cast<long long*>(Connection) + 54);
        }

        if (EngineVersion >= 424)
        {
            return reinterpret_cast<FString*>(reinterpret_cast<long long>(Connection) + 440);
        }

        return nullptr;
    }

    inline void WorldNotifyControlMessageDetour(UObject* World, UObject* Connection, const uint8_t MessageType,
                                                long long* Bunch)
    {
        switch (MessageType)
        {
        case 0:
            {
                if (EngineVersion != 421)
                {
                    break;
                }

                char IsLittleEndian = 0;
                unsigned int RemoteNetworkVersion = 0;
                const auto v38 = reinterpret_cast<long long*>(Bunch[1]);
                FString EncryptionToken;

                if (static_cast<unsigned long long>(*v38 + 1) > v38[1])
                {
                    (*reinterpret_cast<void(**)(long long*, char*, long long)>(*Bunch + 72))(Bunch, &IsLittleEndian, 1);
                }

                if (const auto v39 = Bunch[1]; static_cast<unsigned long long>(*reinterpret_cast<long long*>(v39) + 4) >
                    *reinterpret_cast<long long*>(v39 + 8))
                {
                    (*reinterpret_cast<void(**)(long long*, unsigned*, long long)>(*Bunch + 72))(
                        Bunch, &RemoteNetworkVersion, 4);
                    if ((*(reinterpret_cast<char*>(Bunch) + 41) & 0x10) != 0)
                    {
                        Challenge(reinterpret_cast<long long>(Bunch),
                                  reinterpret_cast<long long>(&RemoteNetworkVersion), 4);
                    }
                }
                else
                {
                    *reinterpret_cast<long long*>(v39) += 4;
                }

                ReceiveFString(Bunch, EncryptionToken);

                SendChallenge(World, Connection);
                return;
            }
        case 4:
            *Connection->Member<int>("CurrentNetSpeed") = 30000;
            return;
        case 5:
            {
                if (EngineVersion < 420)
                {
                    break;
                }

                Bunch[7] += 1024 * 16 * 1024;

                FString OnlinePlatformName;
                if (static const auto CurrentFortniteVersion = FortniteVersion; CurrentFortniteVersion >= 7 &&
                    EngineVersion < 424)
                {
                    ReceiveFString(Bunch, *reinterpret_cast<FString*>(reinterpret_cast<long long>(Connection) + 400));
                }
                else if (CurrentFortniteVersion < 7)
                {
                    ReceiveFString(Bunch, *reinterpret_cast<FString*>(51 + reinterpret_cast<long long*>(Connection)));
                }
                else if (EngineVersion >= 424)
                {
                    ReceiveFString(Bunch, *reinterpret_cast<FString*>(reinterpret_cast<long long>(Connection) + 416));
                }

                const auto RequestUrl = GetRequestUrl(Connection);
                if (!RequestUrl)
                {
                    return;
                }

                ReceiveFString(Bunch, *RequestUrl);
                ReceiveUniqueIdRepl(Bunch, Connection->Member<long long>("PlayerID"));
                ReceiveFString(Bunch, OnlinePlatformName);
                Bunch[7] -= (16 * 1024 * 1024);
                WelcomePlayer(Helper::GetWorld(), Connection);
                return;
            }
        case 6:
            return;
        case 9:
            {
                if (EngineVersion != 421 && EngineVersion >= 420)
                {
                    break;
                }

                if (const auto ConnectionPc = Connection->Member<UObject*>(("PlayerController")); !*ConnectionPc)
                {
                    FURL InUrl;
                    InUrl.Map.Set(L"/Game/Maps/Frontend");
                    InUrl.Valid = 1;
                    InUrl.Protocol.Set(L"unreal");
                    InUrl.Port = 7777;

                    FString ErrorMsg;
                    SpawnPlayActorDetour(Connection, ENetRole::ROLE_AutonomousProxy, InUrl,
                                         Connection->Member<void>("PlayerID"),
                                         ErrorMsg, 0);
                    if (!*ConnectionPc)
                    {
                        return;
                    }

                    const FString LevelName;
                    TravelParameters TravelParameters{LevelName, TRAVEL_Relative, true, FGuid()};
                    (*ConnectionPc)->ProcessEvent("ClientTravel", &TravelParameters);

                    *reinterpret_cast<int32_t*>(reinterpret_cast<long long>(&*Connection->Member<
                        int>("LastReceiveTime")) + sizeof(double) * 4 + sizeof(int32_t) * 2) = 0;
                    return;
                }
            }
        default:
            break;
        }

        return WorldNotifyControlMessage(Helper::GetWorld(), Connection, MessageType, Bunch);
    }

    inline char BeaconNotifyControlMessageDetour([[maybe_unused]] UObject* Beacon, UObject* Connection,
                                                 const uint8_t MessageType, long long* Bunch)
    {
        WorldNotifyControlMessageDetour(Helper::GetWorld(), Connection, MessageType, Bunch);
        return true;
    }

    inline char __fastcall NoReserveDetour(long long*, long long, char, long long*)
    {
        return 0;
    }

    inline long long CollectGarbageDetour(long long)
    {
        return 0;
    }

    inline bool TryCollectGarbageHook()
    {
        return false;
    }

    inline int __fastcall ReceivedPacketDetour(long long*, LARGE_INTEGER, char)
    {
        return 0;
    }

    inline void InitHooks()
    {
        MH_CreateHook(reinterpret_cast<PVOID>(SpawnPlayActorAddress), SpawnPlayActorDetour,
                      reinterpret_cast<void**>(&SpawnPlayActor));
        MH_EnableHook(reinterpret_cast<PVOID>(SpawnPlayActorAddress));

        MH_CreateHook(reinterpret_cast<PVOID>(BeaconNotifyControlMessageAddress), BeaconNotifyControlMessageDetour,
                      reinterpret_cast<void**>(&BeaconNotifyControlMessage));
        MH_EnableHook(reinterpret_cast<PVOID>(BeaconNotifyControlMessageAddress));

        MH_CreateHook(reinterpret_cast<PVOID>(WorldNotifyControlMessageAddress), WorldNotifyControlMessageDetour,
                      reinterpret_cast<void**>(&WorldNotifyControlMessage));
        MH_EnableHook(reinterpret_cast<PVOID>(WorldNotifyControlMessageAddress));

        if (EngineVersion < 424 && GetNetModeAddress) // i dont even think we have to hook this
        {
            MH_CreateHook(reinterpret_cast<PVOID>(GetNetModeAddress), GetNetModeDetour,
                          reinterpret_cast<void**>(&GetNetMode));
            MH_EnableHook(reinterpret_cast<PVOID>(GetNetModeAddress));
        }

        if (EngineVersion > 423)
        {
            MH_CreateHook(reinterpret_cast<PVOID>(ValidationFailureAddress), ValidationFailureDetour,
                          reinterpret_cast<void**>(&ValidationFailure));
            MH_EnableHook(reinterpret_cast<PVOID>(ValidationFailureAddress));
        }

        MH_CreateHook(reinterpret_cast<PVOID>(TickFlushAddress), TickFlushDetour, reinterpret_cast<void**>(&TickFlush));
        MH_EnableHook(reinterpret_cast<PVOID>(TickFlushAddress));

        if (KickPlayer)
        {
            MH_CreateHook(reinterpret_cast<PVOID>(KickPlayerAddress), KickPlayerDetour,
                          reinterpret_cast<void**>(&KickPlayer));
            MH_EnableHook(reinterpret_cast<PVOID>(KickPlayerAddress));
        }

        if (LpSpawnPlayActorAddress)
        {
            MH_CreateHook(reinterpret_cast<PVOID>(LpSpawnPlayActorAddress), LpSpawnPlayActorDetour,
                          reinterpret_cast<void**>(&LpSpawnPlayActor));
            MH_EnableHook(reinterpret_cast<PVOID>(LpSpawnPlayActorAddress));
        }

        if (NoReserveAddress)
        {
            MH_CreateHook(reinterpret_cast<PVOID>(NoReserveAddress), NoReserveDetour,
                          reinterpret_cast<void**>(&NoReserve));
            MH_EnableHook(reinterpret_cast<PVOID>(NoReserveAddress));
        }

        if (NetDebugAddress && EngineVersion >= 419 && FortniteVersion < 17.00)
        {
            MH_CreateHook(reinterpret_cast<PVOID>(NetDebugAddress), NetDebugDetour,
                          reinterpret_cast<void**>(&NetDebug));
            MH_EnableHook(reinterpret_cast<PVOID>(NetDebugAddress));
        }

        if (CollectGarbageAddress)
        {
            if (EngineVersion >= 422 && EngineVersion < 424)
            {
                MH_CreateHook(reinterpret_cast<PVOID>(CollectGarbageAddress), TryCollectGarbageHook, nullptr);
                MH_EnableHook(reinterpret_cast<PVOID>(CollectGarbageAddress));
            }
            else
            {
                MH_CreateHook(reinterpret_cast<PVOID>(CollectGarbageAddress), CollectGarbageDetour,
                              reinterpret_cast<void**>(&CollectGarbage));
                MH_EnableHook(reinterpret_cast<PVOID>(CollectGarbageAddress));
            }
        }
    }

    inline void DisableNetHooks()
    {
        MH_DisableHook(reinterpret_cast<PVOID>(SpawnPlayActorAddress));
        MH_DisableHook(reinterpret_cast<PVOID>(BeaconNotifyControlMessageAddress));
        MH_DisableHook(reinterpret_cast<PVOID>(WorldNotifyControlMessageAddress));

        if (EngineVersion < 424 && GetNetModeAddress)
        {
            MH_DisableHook(reinterpret_cast<PVOID>(GetNetModeAddress));
        }

        if (EngineVersion > 423)
        {
            MH_DisableHook(reinterpret_cast<PVOID>(ValidationFailureAddress));
        }

        MH_DisableHook(reinterpret_cast<PVOID>(TickFlushAddress));

        if (KickPlayer)
        {
            MH_DisableHook(reinterpret_cast<PVOID>(KickPlayerAddress));
        }

        if (LpSpawnPlayActorAddress)
        {
            MH_DisableHook(reinterpret_cast<PVOID>(LpSpawnPlayActorAddress));
        }

        if (NoReserveAddress)
        {
            MH_DisableHook(reinterpret_cast<PVOID>(NoReserveAddress));
        }

        if (NetDebugAddress)
        {
            MH_DisableHook(reinterpret_cast<PVOID>(NetDebugAddress));
        }

        if (CollectGarbageAddress)
        {
            MH_DisableHook(reinterpret_cast<PVOID>(CollectGarbageAddress));
        }
    }
}
