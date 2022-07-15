#pragma once

#include <MinHook/MinHook.h>

// #include "hooks.h"
#include <Net/funcs.h>
#include <Gameplay/helper.h>

inline void ServerAcknowledgePossessionHook(UObject* Object, UFunction* Function, void* Parameters)
{
    struct SAP_Params
    {
        UObject* P;
    };
    auto Params = (SAP_Params*)Parameters;
    if (Params)
    {
        auto Pawn = Params->P; // (UObject*)Parameters;

        *Object->Member<UObject*>(_("AcknowledgedPawn")) = Pawn;
        std::cout << "Set Pawn!\n";
    }
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

char(__fastcall* ValidationFailure)(__int64 a1, __int64 a2);

char __fastcall ValidationFailureDetour(__int64 a1, __int64 a2)
{
    std::cout << "Validation!\n";
    return 1;
}

static void (*SendChallenge)(UObject* a1, UObject* a2); // World, NetConnection
static __int64(__fastcall* Idkf)(__int64 a1, __int64 a2, int a3); // tbh we can just paste pseudo code

UObject* MyPawn = nullptr;

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
    auto Pawn = Easy::SpawnActor(PawnClass, Helper::GetPlayerStart(), {});

    if (!MyPawn)
        MyPawn = Pawn;

    static auto HeroType = FindObject(_("FortHeroType /Game/Athena/Heroes/HID_055_Athena_Commando_M_SkiDude_CAN.HID_055_Athena_Commando_M_SkiDude_CAN"));
    if (HeroType)
        *PlayerState->Member<UObject*>(_("HeroType")) = HeroType;
    else
        std::cout << _("Could not find HeroType!\n");

    static auto headPart = FindObject(_("CustomCharacterPart /Game/Characters/CharacterParts/Female/Medium/Heads/F_Med_Head1.F_Med_Head1"));
    static auto bodyPart = FindObject(_("CustomCharacterPart /Game/Characters/CharacterParts/Female/Medium/Bodies/F_Med_Soldier_01.F_Med_Soldier_01"));

    if (headPart && bodyPart)
    {
        Helper::ChoosePart(Pawn, EFortCustomPartType::Head, headPart);
        Helper::ChoosePart(Pawn, EFortCustomPartType::Body, bodyPart);
        PlayerState->ProcessEvent(PlayerState->Function(_("OnRep_CharacterParts")), nullptr);
    }
    else
        std::cout << _("Could not find CharacterPart!\n");

    std::cout << "Pawn: " << Pawn << '\n';

    auto PossessFn = PlayerController->Function(_("Possess"));
    if (PossessFn)
    {
        struct {
            UObject* pawn;
        } params{ Pawn }; // idk man
        std::cout << "Possessing!\n";
        // std::cout << "Possess Fn: " << PossessFn->GetFullName() << '\n';
        PlayerController->ProcessEvent(PossessFn, &params);
        std::cout << "Possessed!\n";
    }
    else
        std::cout << _("Could not find Possess!\n");

    static auto SetReplicateMovementFn = Pawn->Function(_("SetReplicateMovement"));
    bool tru = true;
    Pawn->ProcessEvent(SetReplicateMovementFn, &tru);
    *Pawn->Member<bool>(_("bReplicateMovement")) = true;
    static auto Rep_ReplicateMovement = Pawn->Function(_("OnRep_ReplicateMovement"));
    Pawn->ProcessEvent(Rep_ReplicateMovement);

    *Pawn->Member<bool>(_("bCanBeDamaged")) = false;
    *Pawn->Member<bool>(_("bAlwaysRelevant")) = true;

    *PlayerController->Member<char>(_("bReadyToStartMatch")) = true;
    *PlayerController->Member<char>(_("bClientPawnIsLoaded")) = true;
    *PlayerController->Member<char>(_("bHasInitiallySpawned")) = true;

    *PlayerController->Member<bool>(_("bHasServerFinishedLoading")) = true;
    *PlayerController->Member<bool>(_("bHasClientFinishedLoading")) = true;

    static auto OnRep_bHasServerFinishedLoading = PlayerController->Function(_("OnRep_bHasServerFinishedLoading"));

    if (OnRep_bHasServerFinishedLoading)
        PlayerController->ProcessEvent(OnRep_bHasServerFinishedLoading, nullptr);

    *PlayerState->Member<char>(_("bHasStartedPlaying")) = true;
    *PlayerState->Member<char>(_("bHasFinishedLoading")) = true;
    *PlayerState->Member<char>(_("bIsReadyToContinue")) = true;

    static auto OnRep_bHasStartedPlaying = PlayerState->Function(_("OnRep_bHasStartedPlaying"));
     
    if (OnRep_bHasStartedPlaying)
        PlayerState->ProcessEvent(OnRep_bHasStartedPlaying, nullptr);

    *PlayerState->Member<uint8_t>(_("TeamIndex")) = 11;
    *PlayerState->Member<unsigned char>(_("SquadId")) = 1;

    std::cout << _("Spawned Player!\n");

    return PlayerController;
}

static bool __fastcall HasClientLoadedCurrentWorldDetour(UObject* pc)
{
    std::cout << _("yes\n");
    return true;
}

void* NetDebugDetour(UObject* idk)
{
    std::cout << "NetDebug!\n";
    return nullptr;
}

void World_NotifyControlMessageDetour(UObject* World, UObject* Connection, uint8_t MessageType, __int64* Bunch)
{
    printf("Recieved control message %i\n", MessageType);

    static const auto FnVerDouble = std::stod(FN_Version);

    switch (MessageType)
    {
    case 0:
    {
        if (Engine_Version == 421)
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
            std::cout << "EncryptionToken: " << __int64(EncryptionToken.Data.GetData()) << '\n';

            if (EncryptionToken.Data.GetData())
                std::cout << "EncryptionToken Str: " << EncryptionToken.ToString() << '\n';

            std::cout << "Sending challenge!\n";
            SendChallenge(World, Connection); // This is actually UWorld
            std::cout << "Sent challenge!\n";
            // World_NotifyControlMessage(World, Connection, MessageType, (void*)Bunch); // DON'T REMOVE THIS
            // std::cout << "IDK: " << *((char*)Bunch + 40) << '\n';
            return;
        }
        break;
    }
    case 4: // NMT_Netspeed
        *Connection->Member<int>(_("CurrentNetSpeed")) = 30000; // sometimes 60000
        return;
    case 5: // NMT_Login
    {
        Bunch[7] += (16 * 1024 * 1024);

        FString OnlinePlatformName;

        static double CurrentFortniteVersion = 3.5;

        if (7 <= CurrentFortniteVersion)
        {
            ReceiveFString(Bunch, *(FString*)(__int64(Connection) + 400)); // clientresponse
            ReceiveFString(Bunch, *(FString*)(__int64(Connection) + 424)); // requesturl
        }
        else
        {
            ReceiveFString(Bunch, *(FString*)((__int64*)Connection + 51));
            ReceiveFString(Bunch, *(FString*)((__int64*)Connection + 54));
        }

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
    case 9: // NMT_Join
    {
        if (Engine_Version >= 422)
        {
            auto RequestURL = (FString*)(__int64(Connection) + 424);
            std::cout << "RequestURL: " << RequestURL << '\n';
            std::cout << "Data: " << __int64(RequestURL->Data.GetData()) << '\n';

            if (RequestURL->Data.GetData())
            {
                std::cout << RequestURL->ToString() << '\n'; // Debugging purposes
            }

            std::cout << "yee\n";

            std::cout << "dang\n";
            /* FString AAHAHAHAHAHA;
            std::string KILLME = lastURL;
            std::wstring AHAHAHA = std::wstring(KILLME.begin(), KILLME.end());
            std::wcout << L"AHAHAHA: " << AHAHAHA << '\n';
            AAHAHAHAHAHA.Set(AHAHAHA.c_str()); */
            /* auto idk = (int32_t*)(Connection + 432);

            std::cout << "idk: " << idk << '\n';

            if (idk)
                std::cout << "Idk deferefnece: " << *idk << '\n'; */

            // (*(FString*)(__int64(Connection) + 424)).Set(L"/Game/Maps/Frontend?Name=Ender0001?AuthTicket=lawinstokenlol?ASID={B06CA25A-48B1-3570-FA91-0F9FFB17984D}?Platform=WIN?AnalyticsPlat=Windows?bIsFirstServerJoin=1");
            
            // if (FnVerDouble >= 8)
                // *idk = 1;
        }
        else if (Engine_Version == 421)
        {
            auto ConnectionPC = Connection->Member<UObject*>(_("PlayerController"));
            if (!*ConnectionPC)
            {
                FURL InURL;
                /* std::cout << "Calling1!\n";
                std::wcout << L"RequestURL Data: " << Connection->RequestURL.Data << '\n';
                InURL = o_FURL_Construct(InURL, NULL, Connection->RequestURL.Data, ETravelType::TRAVEL_Absolute);
                std::cout << "Called2!\n";
                std::cout << "URL2 Map: " << InURL.Map.ToString() << '\n'; */

                // FURL InURL(NULL, Connection->RequestURL, ETravelType::TRAVEL_Absolute);

                InURL.Map.Set(L"/Game/Maps/Frontend");
                InURL.Valid = 1;
                InURL.Protocol.Set(L"unreal");
                InURL.Port = 7777;

                // ^ This was all taken from 3.5

                FString ErrorMsg;
                SpawnPlayActorDetour(World, Connection, ENetRole::ROLE_AutonomousProxy, InURL, Connection->Member<void>(_("PlayerID")), ErrorMsg, 0);

                if (!*ConnectionPC)
                {
                    std::cout << "Failed to spawn PlayerController! Error Msg: " << ErrorMsg.ToString() << "\n";
                    // TODO: Send NMT_Failure and FlushNet
                    return;
                }
                else
                {
                    std::cout << "Join succeeded!\n";
                    FString LevelName;
                    const bool bSeamless = true; // If this is true and crashes the client then u didn't remake NMT_Hello or NMT_Join correctly.

                    static auto ClientTravelFn = (*ConnectionPC)->Function(_("ClientTravel"));
                    struct {
                        const FString& URL;
                        ETravelType TravelType;
                        bool bSeamless;
                        FGuid MapPackageGuid;
                    } paramsTravel{ LevelName, ETravelType::TRAVEL_Relative, bSeamless, FGuid() };

                    std::cout << "Calling ClientTravel..\n";

                    // ClientTravel(*ConnectionPC, LevelName, ETravelType::TRAVEL_Relative, bSeamless, FGuid());

                    (*ConnectionPC)->ProcessEvent(_("ClientTravel"), &paramsTravel);

                    std::cout << "Called ClientTravel..\n";

                    *(int32_t*)(__int64(&*Connection->Member<int>(_("LastReceiveTime"))) + (sizeof(double) * 4) + (sizeof(int32_t) * 2)) = 0; // QueuedBits
                }
                return;
            }
        }
        break;
    }
    }

    return World_NotifyControlMessage(Helper::GetWorld(), Connection, MessageType, Bunch);
}

void Beacon_NotifyControlMessageDetour(UObject* Beacon, UObject* Connection, uint8_t MessageType, __int64* Bunch)
{
    return World_NotifyControlMessageDetour(Helper::GetWorld(), Connection, MessageType, Bunch);
}

char(__fastcall* NoReserve)(__int64* a1, __int64 a2, char a3, __int64* a4);

char __fastcall NoReserveDetour(__int64* a1, __int64 a2, char a3, __int64* a4)
{
    std::cout << _("No Reserve!\n");
    return 0;
}

void InitializeNetHooks()
{
    static const auto FnVerDouble = std::stod(FN_Version);

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

    // if (NetDebug)
    {
        MH_CreateHook((PVOID)NetDebugAddr, NetDebugDetour, (void**)&NetDebug);
        MH_EnableHook((PVOID)NetDebugAddr);
    }

    if (FnVerDouble >= 8)
    {
        MH_CreateHook((PVOID)HasClientLoadedCurrentWorldAddr, HasClientLoadedCurrentWorldDetour, (void**)&HasClientLoadedCurrentWorld);
        MH_EnableHook((PVOID)HasClientLoadedCurrentWorldAddr);
    }

    if (FnVerDouble >= 9)
    {
        auto a = MH_CreateHook((PVOID)NoReserveAddr, NoReserveDetour, (void**)&NoReserve);
        auto b = MH_EnableHook((PVOID)NoReserveAddr);
        std::cout << "CreateHook: " << MH_StatusToString(a) << '\n';
        std::cout << "EnableHook: " << MH_StatusToString(b) << '\n';

        a = MH_CreateHook((PVOID)ValidationFailureAddr, ValidationFailureDetour, (void**)&ValidationFailure);
        b = MH_EnableHook((PVOID)ValidationFailureAddr);

        std::cout << "CreateHook 1: " << MH_StatusToString(a) << '\n';
        std::cout << "EnableHook 1: " << MH_StatusToString(b) << '\n';
    }
}