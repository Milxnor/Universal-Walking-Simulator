#pragma once

#include <MinHook/MinHook.h>

// #include "hooks.h"
#include <Net/funcs.h>
#include <Gameplay/helper.h>

inline void ServerAcknowledgePossessionHook(UObject* Object, UFunction* Function, void* Parameters)
{
    auto Pawn = (UObject*)Parameters; // Params->P;

    *Object->Member<UObject*>(_("AcknowledgedPawn")) = Pawn;
}

void InitializeNetUHooks()
{
    AddHook(_("Function /Script/Engine.PlayerController.ServerAcknowledgePossession"), ServerAcknowledgePossessionHook);
}

void TickFlushDetour(UObject* _netDriver, float DeltaSeconds)
{
    static auto NetDriver = *Helper::GetWorld()->Member<UObject*>(_("NetDriver"));
    static auto ReplicationDriver = *NetDriver->Member<UObject*>(_("ReplicationDriver"));
    static auto& ClientConnections = *NetDriver->Member<TArray<UObject*>>(_("ClientConnections"));

    if (ClientConnections.Num() > 0)
    {
        if (!*ClientConnections[0]->Member<char>(_("InternalAck")))
        {
            if (ReplicationDriver)
            {
                ServerReplicateActors(ReplicationDriver);
            }
            else
                std::cout << "invalid Rep;licatoiNDriveR!\n";
        }
        // else
            // std::cout << "internalack is true!\n";
    }
    // else
        // std::cout << "No connections.";

    return TickFlush(_netDriver, DeltaSeconds);
}

uint64_t GetNetModeDetour(UObject* World)
{
    return ENetMode::NM_ListenServer;
}

char KickPlayerDetour(__int64 a1, __int64 a2, __int64 a3)
{
    //MessageBoxA(0, "Handled Player Kick", 0, 0);
    return 0;
}

void World_NotifyControlMessageDetour(UObject* World, UObject* Connection, uint8_t MessageType, __int64* Bunch)
{
    printf("Recieved control message %i\n", MessageType);

    switch (MessageType)
    {
    case 4: // NMT_Netspeed
        *Connection->Member<int>(_("CurrentNetSpeed")) = 30000;
        return;
    case 5: // NMT_Login
    {
        Bunch[7] += (16 * 1024 * 1024);

        FString OnlinePlatformName;

        static double CurrentFortniteVersion = 3.5;

        FString* clientResponse = nullptr;
        FString* requestURL = nullptr;

        if (7 <= CurrentFortniteVersion)
        {
            clientResponse = (FString*)(__int64(Connection) + 400);
            requestURL = (FString*)(__int64(Connection) + 424);
        }
        else
        {
            clientResponse = (FString*)((__int64*)Connection + 51);
            requestURL = (FString*)((__int64*)Connection + 54);
        }

        ReceiveFString(Bunch, *clientResponse);
        std::cout << "Response => " << clientResponse->ToString() << '\n';
        ReceiveFString(Bunch, *requestURL);
        std::cout << "RequestURL => " << requestURL->ToString() << '\n';
        ReceiveUniqueIdRepl(Bunch, Connection->Member<__int64>(_("PlayerID")));
        std::cout << "Got PlayerID!\n";
        // std::cout << "PlayerID => " + *Connection->Member<__int64>(_("PlayerID")) << '\n';
        // std::cout << "PlayerID => " + std::to_string(*UniqueId) << '\n';
        ReceiveFString(Bunch, OnlinePlatformName);

        std::cout << "Got OnlinePlatformName!\n";
        if (OnlinePlatformName.Data.GetData())
            std::cout << "OnlinePlatformName => " << OnlinePlatformName.ToString() << '\n';

        Bunch[7] -= (16 * 1024 * 1024);

        WelcomePlayer(Helper::GetWorld(), Connection);

        return;
    }
    }

    return World_NotifyControlMessage(Helper::GetWorld(), Connection, MessageType, Bunch);
}

void Beacon_NotifyControlMessageDetour(UObject* Beacon, UObject* Connection, uint8_t MessageType, __int64* Bunch)
{
    return World_NotifyControlMessageDetour(Helper::GetWorld(), Connection, MessageType, Bunch);
}


UObject* SpawnPlayActorDetour(UObject* World, UObject* NewPlayer, ENetRole RemoteRole, FURL& URL, void* UniqueId, FString& Error, uint8_t NetPlayerIndex)
{
    std::cout << _("SpawnPlayActor called!\n");
    auto PlayerController = SpawnPlayActor(Helper::GetWorld(), NewPlayer, RemoteRole, URL, UniqueId, Error, NetPlayerIndex);
    *NewPlayer->Member<UObject*>(_("PlayerController")) = PlayerController;

    auto PlayerState = *PlayerController->Member<UObject*>(_("PlayerState"));

    static auto QuickBarsClass = FindObject(_("Class /Script/FortniteGame.FortQuickBarsAthena"));
    *PlayerController->Member<UObject*>(_("QuickBars")) = Easy::SpawnActor(QuickBarsClass, FVector(), FRotator());

    // InitInventory(PlayerController);

    static auto PawnClass = FindObject(_("BlueprintGeneratedClass /Game/Athena/PlayerPawn_Athena.PlayerPawn_Athena_C"));
    auto Pawn = Easy::SpawnActor(PawnClass, Helper::GetPlayerStart(PlayerController), {});

    static auto HeroType = FindObject(_("FortHeroType /Game/Athena/Heroes/HID_058_Athena_Commando_M_SkiDude_GER.HID_058_Athena_Commando_M_SkiDude_GER"));
    *PlayerState->Member<UObject*>(_("HeroType")) = HeroType;

    static auto headPart = FindObject(_("CustomCharacterPart /Game/Characters/CharacterParts/Female/Medium/Heads/F_Med_Head1.F_Med_Head1"));
    static auto bodyPart = FindObject(_("CustomCharacterPart /Game/Characters/CharacterParts/Female/Medium/Bodies/F_Med_Soldier_01.F_Med_Soldier_01"));

    if (headPart && bodyPart)
    {
        Helper::ChoosePart(Pawn, EFortCustomPartType::Head, headPart);
        Helper::ChoosePart(Pawn, EFortCustomPartType::Body, bodyPart);
        PlayerState->ProcessEvent(PlayerState->Function(_("OnRep_CharacterParts")), nullptr);
    }

    auto PossessFn = PlayerController->Function(_("Possess"));
    if (PossessFn)
    {
        struct {
            UObject* pawn;
        } params{ Pawn }; // idk man
        PlayerController->ProcessEvent(PossessFn, &params);
    }
    else
        std::cout << _("Could not find Possess!\n");

    *Pawn->Member<bool>(_("bCanBeDamaged")) = false;

    *PlayerController->Member<char>(_("bReadyToStartMatch")) = true;
    *PlayerController->Member<char>(_("bClientPawnIsLoaded")) = true;
    *PlayerController->Member<char>(_("bHasInitiallySpawned")) = true;

    *PlayerController->Member<bool>(_("bHasServerFinishedLoading")) = true;
    *PlayerController->Member<bool>(_("bHasClientFinishedLoading")) = true;

    static auto OnRep_bHasServerFinishedLoading = PlayerController->Function(_("OnRep_bHasServerFinishedLoading"));

    PlayerController->ProcessEvent(OnRep_bHasServerFinishedLoading, nullptr);

    *PlayerState->Member<char>(_("bHasStartedPlaying")) = true;
    *PlayerState->Member<char>(_("bHasFinishedLoading")) = true;
    *PlayerState->Member<char>(_("bIsReadyToContinue")) = true;

    static auto OnRep_bHasStartedPlaying = PlayerState->Function(_("OnRep_bHasStartedPlaying"));
    PlayerState->ProcessEvent(OnRep_bHasStartedPlaying, nullptr);

    *PlayerState->Member<uint8_t>(_("TeamIndex")) = 11;
    *PlayerState->Member<unsigned char>(_("SquadId")) = 1;

    std::cout << _("Spawned Player!\n");

    return PlayerController;
}

void InitializeNetHooks()
{
    MH_CreateHook((PVOID)SpawnPlayActorAddr, SpawnPlayActorDetour, (void**)&SpawnPlayActor);
    MH_EnableHook((PVOID)SpawnPlayActorAddr);

    MH_CreateHook((PVOID)Beacon_NotifyControlMessageAddr, Beacon_NotifyControlMessageDetour, (void**)&Beacon_NotifyControlMessage);
    MH_EnableHook((PVOID)Beacon_NotifyControlMessageAddr);

    MH_CreateHook((PVOID)World_NotifyControlMessageAddr, World_NotifyControlMessageDetour, (void**)&World_NotifyControlMessage);
    MH_EnableHook((PVOID)World_NotifyControlMessageAddr);

    MH_CreateHook((PVOID)GetNetModeAddr, GetNetModeDetour, (void**)&GetNetMode);
    MH_EnableHook((PVOID)GetNetModeAddr);

    MH_CreateHook((PVOID)TickFlushAddr, TickFlushDetour, (void**)&TickFlush);
    MH_EnableHook((PVOID)TickFlushAddr);

    MH_CreateHook((PVOID)KickPlayerAddr, KickPlayerDetour, (void**)&KickPlayer);
    MH_EnableHook((PVOID)KickPlayerAddr);
}