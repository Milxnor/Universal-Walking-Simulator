#pragma once

#include <Net/funcs.h>
#include <Gameplay/helper.h>
#include "discord.h"

static bool bListening = false;

static UObject* BeaconHost = nullptr;

void AllowConnections(UObject* NetDriver)
{
    static const auto World = Helper::GetWorld();

    if (BeaconHost)
    {
        if (Engine_Version == 421) // idfk
            *(int*)(BeaconHost + 0x340) = 0;
        else
            PauseBeaconRequests(BeaconHost, false);
    }
    else
        std::cout << "No BeaconHost!\n";

    if (NetDriver)
    {
        *NetDriver->Member<UObject*>(("World")) = World;

        if (SetWorld)
            SetWorld(NetDriver, World);
        else
            std::cout << ("Invalid SetWorld!\n");
    }
}

DWORD WINAPI MapLoadThread(LPVOID) // DOES NOT WORK
{
    serverStatus = EServerStatus::Loading;

    static const auto World = Helper::GetWorld();

    // rythmm

    auto StreamingLevels = World->Member<TArray<UObject*>>(("StreamingLevels")); // ULevelStreaming*

    for (int i = 0; i < StreamingLevels->Num(); i++)
    {
        auto StreamingLevel = StreamingLevels->At(i);

        if (!StreamingLevel)
            continue;

        static auto IsLevelLoaded = StreamingLevel->Function(("IsLevelLoaded"));
        bool bIsLevelLoaded = false;

        if (IsLevelLoaded)
            StreamingLevel->ProcessEvent(IsLevelLoaded, &bIsLevelLoaded);

        if (bIsLevelLoaded)
            continue;

        Sleep(1000);
    }

    // now the map is fully loaded

    if (BeaconHost)
    {
        AllowConnections(*BeaconHost->Member<UObject*>(("NetDriver")));
    }

    else
    {
    }

    std::cout << ("Players can now join!\n");
    serverStatus = EServerStatus::Up;

#ifndef DPP_DISABLED
    while (!bIsBotRunning) {}
    if (bIsBotRunning)
        SendDiscordStart();
#endif

    return 0;
}

void Listen(int Port = 7777)
{
    static const auto World = Helper::GetWorld();

    UObject* NetDriver = nullptr;

    FString Error;
    auto InURL = FURL();
    InURL.Port = Port;

    if (bUseBeacons)
    {
        static UObject* BeaconHostClass = FindObject(("Class /Script/OnlineSubsystemUtils.OnlineBeaconHost"));

        if (!BeaconHostClass)
        {
            std::cout << dye::red(("[ERROR] ")) << ("Unable to find BeaconClass!\n");
            return;
        }

        std::cout << ("Spawning Beacon!\n");

        BeaconHost = Easy::SpawnActor(BeaconHostClass, FVector());

        if (!BeaconHost)
        {
            std::cout << dye::red(("[ERROR] ")) << ("Beacon failed to spawn!\n");
            return;
        }

        std::cout << ("Spawned Beacon!\n");

        if (Engine_Version < 426)
            *BeaconHost->Member<int>(("ListenPort")) = Port - 1;
        else
            *BeaconHost->Member<int>(("ListenPort")) = Port;

        bool bInitBeacon = false;

        // *BeaconHost->Member<FName>(("NetDriverName")) = FName(282);
        // *BeaconHost->Member<FName>(("NetDriverDefinitionName")) = FName(282);

        if (InitHost)
            bInitBeacon = InitHost(BeaconHost);
        else
        {
            std::cout << dye::red(("[ERROR] ")) << ("No InitHost!\n");
            return;
        }

        if (!bInitBeacon)
        {
            std::cout << dye::red(("[ERROR] ")) << ("Unable to initialize Beacon!\n");
            return;
        }

        std::cout << ("Initialized Beacon!\n");
        
        NetDriver = *BeaconHost->Member<UObject*>(("NetDriver"));

        /* FName GameNetDriverName = FName(282);

        *BeaconHost->Member<FName>(("NetDriverName")) = GameNetDriverName;
        NetDriver = *BeaconHost->Member<UObject*>(("NetDriver"));
        static auto ReplicationDriverClass = FindObject(("Class /Script/FortniteGame.FortReplicationGraph"));
        *NetDriver->Member<UObject*>(("ReplicationDriverClass")) = ReplicationDriverClass;
        *NetDriver->Member<FName>(("NetDriverName")) = GameNetDriverName;
        // FString string;
        // string.Set(L"GameNetDriver");
        // auto GameNetDriverName = Helper::StringToName(string);

        InitListen(NetDriver, World, InURL, true, Error);
        *NetDriver->Member<UObject*>(("World")) = World;
        PauseBeaconRequests(BeaconHost, true);
        *NetDriver->Member<UObject*>(("World")) = World;
        *NetDriver->Member<FName>(("NetDriverName")) = GameNetDriverName; */
    }

    if (!NetDriver)
    {
        std::cout << ("Failed to create NetDriver!\n");
        return;
    }
    else
    {
        FString string;
        string.Set(L"GameNetDriver");
        auto GameNetDriverName = Helper::StringToName(string);
        *NetDriver->Member<FName>(("NetDriverName")) = GameNetDriverName;
        *NetDriver->Member<UObject*>(("World")) = World;
        *World->Member<UObject*>(("NetDriver")) = NetDriver;

        FString Error;
        auto InURL = FURL();
        InURL.Port = Port;

        std::cout << "InitListen: " << InitListen(NetDriver, World, InURL, false, Error) << '\n';
        if (Engine_Version < 426)
        {
            if (SetWorld)
                SetWorld(NetDriver, World);
        }
        else
        {
            auto SetWorldAAddr = NetDriver->VFTable[0x72];
            SetWorld = decltype(SetWorld)(SetWorldAAddr);
            std::cout << "SetWorld Sig: " << GetBytes(__int64(SetWorldAAddr), 50) << '\n';
            SetWorld(NetDriver, World);

            /* std::cout << "Calling!\n";
            (*(void(__fastcall**)(__int64, UObject*))(**(__int64**)(BeaconHost + 560) + 912))(*(__int64*)(NetDriver + 560), World);
            std::cout << "Called!\n"; */
        }
    }

    *NetDriver->Member<int>(("MaxClientRate")) = *NetDriver->Member<int>(("MaxInternetClientRate"));
    UObject** ReplicationDriver = nullptr;

    AllowConnections(NetDriver);

    serverStatus = EServerStatus::Up;

    if (NetDriver)
    {
        ReplicationDriver = NetDriver->Member<UObject*>(("ReplicationDriver"));

        if (Engine_Version >= 424)
        {
            if (SetReplicationDriver)
            {
                if (!(*ReplicationDriver))
                {
                    if (ReplicationGraph_Enable)
                    {
                        static auto ReplicationDriverClass = FindObject(("Class /Script/FortniteGame.FortReplicationGraph"));

                        auto Ret = Easy::SpawnObject(ReplicationDriverClass, NetDriver);
                        std::cout << "new rep graph: " << Ret << '\n';
                        SetReplicationDriver(NetDriver, Ret);
                    }
                    else
                        std::cout << ("No ReplicationGraph_Enable\n");
                }
                else
                    std::cout << dye::red(("\n\n[WARNING] ReplicationDriver is valid, but we are trying to create it. (This is VERY good)\n\n\n"));
            }
            else
                std::cout << ("No SetReplicationDriver!\n");
        }
    }
    else
        std::cout << ("No NetDriver!\n");

    if (ReplicationDriver && *ReplicationDriver)
        RepGraph_ServerReplicateActors = decltype(RepGraph_ServerReplicateActors)((*ReplicationDriver)->VFTable[ServerReplicateActorsOffset]);
    else
        std::cout << dye::red(("\n\n[ERROR] NO ReplicationDriver\n\n\n"));

    auto LevelCollections = World->Member<TArray<FLevelCollection>>(("LevelCollections"));

    if (LevelCollections)
    {
        LevelCollections->At(0).NetDriver = NetDriver;
        LevelCollections->At(1).NetDriver = NetDriver;
    }

    bListening = true;
    std::cout << std::format(("Listening for connections on port {}!\n"), std::to_string(Port));
#if 0
    CreateThread(0, 0, MapLoadThread, 0, 0, 0);
#else
#ifndef DPP_DISABLED
    if (bIsBotRunning)
        SendDiscordStart();
#endif
#endif
}