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
        static UObject* BeaconHostClass = nullptr;

        if (Engine_Version >= 420)
            BeaconHostClass = FindObject("Class /Script/FortniteGame.FortOnlineBeaconHost");
        else
            BeaconHostClass = FindObjectOld("Class /Script/OnlineSubsystemUtils.OnlineBeaconHost", true);

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
        
        FName GameNetDriverName = FName(282);

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
        *NetDriver->Member<FName>(_("NetDriverName")) = GameNetDriverName;
    }
    else
    {
        FName GameNetDriverName = FName();
        GameNetDriverName.ComparisonIndex = 282;

        NetDriver = CreateNetDriver(GetEngine(), World, GameNetDriverName);

        // static auto IpNetDriverClass = FindObject(_("Class /Script/OnlineSubsystemUtils.IpNetDriver"));
        // static auto dababy = FindObject(_("Package /Engine/Transient"));
        // NetDriver = Easy::SpawnObject(IpNetDriverClass, dababy);
        
        std::cout << "NetDriver Address: " << NetDriver << '\n';

        if (NetDriver)
        {
            std::cout << "NetDriver Name: " << NetDriver->GetFullName() << '\n';

            *NetDriver->Member<FName>(_("NetDriverName")) = GameNetDriverName;
            *NetDriver->Member<UObject*>(_("World")) = World;
            InitListen(NetDriver, World, InURL, true, Error);
            *NetDriver->Member<FName>(_("NetDriverName")) = GameNetDriverName;
        }

        /* NetDriver = CreateNetDriver_Local(GetEngine(), World, FName(282));
        // std::cout << _("Created NetDriver: ") << NetDriver << '\n';
        //static auto IpNetDriverClass = FindObject(_("Class /Script/OnlineSubsystemUtils.IpNetDriver"));
        //static auto dababy = FindObject(_("Package /Engine/Transient"));
        //NetDriver = Easy::SpawnObject(IpNetDriverClass, dababy);

        FString string;
        string.Set(L"GameNetDriver");
        auto GameNetDriverName = Helper::StringToName(string);

        for (int k = 1000; k < 702 * 7; k += 7)
        {
            FName Name;
            Name.ComparisonIndex = k;
            std::cout << std::format("[{}] {}\n", k, Name.ToString());

        } */

        /* GameNetDriverName.ComparisonIndex = 282;
        std::cout << _("GameNetDriverName Name: ") << GameNetDriverName.ToString() << '\n';
        std::cout << _("GameNetDriverName Name Comp: ") << GameNetDriverName.ComparisonIndex << '\n';
        std::cout << _("GameNetDriverName Name Num: ") << GameNetDriverName.Number << '\n';
        NetDriver = CreateNetDriver(GetEngine(), World, GameNetDriverName);

        std::cout << _("Created NetDriver: ") << NetDriver << '\n';

        if (NetDriver)
        {
            std::cout << _("Driver Name: ") << NetDriver->GetFullName() << '\n';
            FString string;
            string.Set(L"GameNetDriver");
            *NetDriver->Member<FName>(_("NetDriverName")) = GameNetDriverName;
            InitListen(NetDriver, World, InURL, true, Error);
            *NetDriver->Member<FName>(_("NetDriverName")) = GameNetDriverName;
            std::cout << _("Called InitListen on the NetDriver!\n");
        }
        else
            std::cout << _("Failed to create netdriver!\n"); */
    }

    if (!NetDriver)
    {
        std::cout << _("Failed to create NetDriver!\n");
        return;
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

    *World->Member<UObject*>(_("NetDriver")) = NetDriver;
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