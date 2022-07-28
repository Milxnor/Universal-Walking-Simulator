#pragma once

#include <Net/funcs.h>
#include <Gameplay/helper.h>
#include "discord.h"

static bool bListening = false;

static UObject* BeaconHost = nullptr;

DWORD WINAPI MapLoadThread(LPVOID)
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

        StreamingLevel->ProcessEvent(IsLevelLoaded, &bIsLevelLoaded);

        if (bIsLevelLoaded)
            continue;

        Sleep(1000);
    }

    // now the map is fully loaded

    if (BeaconHost)
    {
        PauseBeaconRequests(BeaconHost, false);
        auto NetDriver = *BeaconHost->Member<UObject*>(_("NetDriver"));
        *NetDriver->Member<UObject*>(_("World")) = World;

        if (SetWorld && NetDriver)
            SetWorld(NetDriver, World);

        std::cout << _("Players can now join!\n");
        serverStatus = EServerStatus::Up;
        SendDiscordStart();
    }

    return 0;
}

void Listen(int Port = 7777)
{
    bool bUseBeacons = true;//(Engine_Version >= 425) ? false : true; // CreateNetDriver ? false : true;

    if (!bUseBeacons)
        bUseBeacons = (CreateNetDriver ? false : true);

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

        *BeaconHost->Member<FName>(_("NetDriverName")) = FName(282); // REGISTER_NAME(282,GameNetDriver)
        NetDriver = *BeaconHost->Member<UObject*>(_("NetDriver"));
        *NetDriver->Member<FName>(_("NetDriverName")) = FName(282); // REGISTER_NAME(282,GameNetDriver)

        InitListen(NetDriver, Helper::GetWorld(), InURL, true, Error);
        *NetDriver->Member<UObject*>(_("World")) = World;
        PauseBeaconRequests(BeaconHost, true);
        *NetDriver->Member<UObject*>(_("World")) = World;
    }
    else
    {
        NetDriver = CreateNetDriver(GetEngine(), World, FName(282));
        std::cout << _("Created NetDriver!\n");
        InitListen(NetDriver, World, InURL, true, Error);
        std::cout << _("Called InitListen on the NetDriver!\n");
    }

    *NetDriver->Member<int>(_("MaxClientRate")) = *NetDriver->Member<int>(_("MaxInternetClientRate"));
    UObject* ReplicationDriver = nullptr;

    if (NetDriver)
    {
        // if (!bUseBeacons)
        if (Engine_Version >= 424)
        {
            if (SetReplicationDriver)
            {
                static auto ReplicationDriverClass = FindObject(_("Class /Script/FortniteGame.FortReplicationGraph"));

                struct {
                    UObject* ObjectClass;
                    UObject* Outer;
                    UObject* ReturnValue;
                } params{ ReplicationDriverClass , NetDriver };

                static auto GSC = FindObject(_("GameplayStatics /Script/Engine.Default__GameplayStatics"));
                static auto fn = GSC->Function(_("SpawnObject"));
                // static auto fn = FindObject(_("Function /Script/Engine.GameplayStatics.SpawnObject"));
                std::cout << "Creating graph\n";
                GSC->ProcessEvent(fn, &params);
                std::cout << "new rep graph: " << params.ReturnValue << '\n';
                SetReplicationDriver(NetDriver, params.ReturnValue);
            }
            else
                std::cout << _("No SetReplicationDriver!\n");
        }

        ReplicationDriver = *NetDriver->Member<UObject*>(_("ReplicationDriver"));
    }
    else
        std::cout << _("No NetDriver!\n");

    if (ReplicationDriver)
        RepGraph_ServerReplicateActors = decltype(RepGraph_ServerReplicateActors)(ReplicationDriver->VFTable[ServerReplicateActorsOffset]);
    else
        std::cout << dye::red(_("\n\n[ERROR] NO ReplicationDriver\n\n\n"));

    *World->Member<UObject*>(_("NetDriver")) = NetDriver;
    auto LevelCollections = World->Member<TArray<FLevelCollection>>(_("LevelCollections"));

    if (LevelCollections)
    {
        LevelCollections->At(0).NetDriver = NetDriver;
        LevelCollections->At(1).NetDriver = NetDriver;
    }

    // GetWorld()->AuthorityGameMode->GameSession->MaxPlayers = 100;
    bListening = true;
    std::cout << std::format(("Listening for connections on port {}!\n"), std::to_string(Port));
    CreateThread(0, 0, MapLoadThread, 0, 0, 0);
}