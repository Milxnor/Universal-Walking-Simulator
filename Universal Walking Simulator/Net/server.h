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
        PauseBeaconRequests(BeaconHost, false);
    }

    if (NetDriver)
    {
        *NetDriver->Member<UObject*>(_("World")) = World;

        if (SetWorld)
            SetWorld(NetDriver, World);
        else
            std::cout << _("Invalid SetWorld!\n");
    }
}

DWORD WINAPI MapLoadThread(LPVOID) // DOES NOT WORK
{
    serverStatus = EServerStatus::Loading;

    static const auto World = Helper::GetWorld();

    // rythmm

    auto StreamingLevels = World->Member<TArray<UObject*>>(_("StreamingLevels")); // ULevelStreaming*

    for (int i = 0; i < StreamingLevels->Num(); i++)
    {
        auto StreamingLevel = StreamingLevels->At(i);

        if (!StreamingLevel)
            continue;

        static auto IsLevelLoaded = StreamingLevel->Function(_("IsLevelLoaded"));
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
        AllowConnections(*BeaconHost->Member<UObject*>(_("NetDriver")));
    }

    else
    {
    }

    std::cout << _("Players can now join!\n");
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
        static UObject* BeaconHostClass = FindObject(_("Class /Script/OnlineSubsystemUtils.OnlineBeaconHost"));

        if (!BeaconHostClass)
        {
            std::cout << dye::red(_("[ERROR] ")) << _("Unable to find BeaconClass!\n");
            return;
        }

        std::cout << _("Spawning Beacon!\n");

        BeaconHost = Easy::SpawnActor(BeaconHostClass, FVector());

        if (!BeaconHost)
        {
            std::cout << dye::red(_("[ERROR] ")) << _("Beacon failed to spawn!\n");
            return;
        }

        std::cout << _("Spawned Beacon!\n");

        *BeaconHost->Member<int>(_("ListenPort")) = Port - 1;
        bool bInitBeacon = false;

        // *BeaconHost->Member<FName>(_("NetDriverName")) = FName(282);
        // *BeaconHost->Member<FName>(_("NetDriverDefinitionName")) = FName(282);

        if (InitHost)
            bInitBeacon = InitHost(BeaconHost);
        else
        {
            std::cout << dye::red(_("[ERROR] ")) << _("No InitHost!\n");
            return;
        }

        if (!bInitBeacon)
        {
            std::cout << dye::red(_("[ERROR] ")) << _("Unable to initialize Beacon!\n");
            return;
        }

        std::cout << _("Initialized Beacon!\n");
        
        NetDriver = *BeaconHost->Member<UObject*>(_("NetDriver"));

        /* FName GameNetDriverName = FName(282);

        *BeaconHost->Member<FName>(_("NetDriverName")) = GameNetDriverName;
        NetDriver = *BeaconHost->Member<UObject*>(_("NetDriver"));
        static auto ReplicationDriverClass = FindObject(_("Class /Script/FortniteGame.FortReplicationGraph"));
        *NetDriver->Member<UObject*>(_("ReplicationDriverClass")) = ReplicationDriverClass;
        *NetDriver->Member<FName>(_("NetDriverName")) = GameNetDriverName;
        // FString string;
        // string.Set(L"GameNetDriver");
        // auto GameNetDriverName = Helper::StringToName(string);

        InitListen(NetDriver, World, InURL, true, Error);
        *NetDriver->Member<UObject*>(_("World")) = World;
        PauseBeaconRequests(BeaconHost, true);
        *NetDriver->Member<UObject*>(_("World")) = World;
        *NetDriver->Member<FName>(_("NetDriverName")) = GameNetDriverName; */
    }

    if (!NetDriver)
    {
        std::cout << _("Failed to create NetDriver!\n");
        return;
    }
    else
    {
        FString string;
        string.Set(L"GameNetDriver");
        auto GameNetDriverName = Helper::StringToName(string);
        *NetDriver->Member<FName>(_("NetDriverName")) = GameNetDriverName;
        *NetDriver->Member<UObject*>(_("World")) = World;
        *World->Member<UObject*>(_("NetDriver")) = NetDriver;

        FString Error;
        auto InURL = FURL();
        InURL.Port = 7777;

        std::cout << "InitListen: " << InitListen(NetDriver, World, InURL, false, Error) << '\n';
        SetWorld(NetDriver, World);
    }

    *NetDriver->Member<int>(_("MaxClientRate")) = *NetDriver->Member<int>(_("MaxInternetClientRate"));
    UObject** ReplicationDriver = nullptr;

    AllowConnections(NetDriver);

    serverStatus = EServerStatus::Up;

    if (NetDriver)
    {
        ReplicationDriver = NetDriver->Member<UObject*>(_("ReplicationDriver"));

        if (Engine_Version >= 424)
        {
            if (SetReplicationDriver)
            {
                if (!(*ReplicationDriver))
                {
                    if (ReplicationGraph_Enable)
                    {
                        static auto ReplicationDriverClass = FindObject(_("Class /Script/FortniteGame.FortReplicationGraph"));

                        auto Ret = Easy::SpawnObject(ReplicationDriverClass, NetDriver);
                        std::cout << "new rep graph: " << Ret << '\n';
                        SetReplicationDriver(NetDriver, Ret);
                    }
                    else
                        std::cout << _("No ReplicationGraph_Enable\n");
                }
                else
                    std::cout << dye::red(_("\n\n[WARNING] ReplicationDriver is valid, but we are trying to create it. (This is VERY good)\n\n\n"));
            }
            else
                std::cout << _("No SetReplicationDriver!\n");
        }
    }
    else
        std::cout << _("No NetDriver!\n");

    if (ReplicationDriver && *ReplicationDriver)
        RepGraph_ServerReplicateActors = decltype(RepGraph_ServerReplicateActors)((*ReplicationDriver)->VFTable[ServerReplicateActorsOffset]);
    else
        std::cout << dye::red(_("\n\n[ERROR] NO ReplicationDriver\n\n\n"));

    auto LevelCollections = World->Member<TArray<FLevelCollection>>(_("LevelCollections"));

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