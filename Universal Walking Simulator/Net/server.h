#pragma once

#include <Net/funcs.h>
#include <Gameplay/helper.h>
#include "discord.h"

static bool bListening = false;

void Listen(int Port = 7777)
{
    bool bUseBeacons = true;//(Engine_Version >= 425) ? false : true; // CreateNetDriver ? false : true;
    static const auto World = Helper::GetWorld();

    UObject* NetDriver = nullptr;

    FString Error;
    auto InURL = FURL();
    InURL.Port = Port;

    if (bUseBeacons)
    {
        static auto BeaconHostClass = FindObject("Class /Script/FortniteGame.FortOnlineBeaconHost");

        if (!BeaconHostClass)
        {
            std::cout << dye::red(_("[ERROR] ")) << _("Unable to find BeaconClass!\n");
            return;
        }

        std::cout << _("Spawning Beacon!\n");

        UObject* BeaconHost = Easy::SpawnActor(BeaconHostClass, FVector());

        if (!BeaconHost)
        {
            std::cout << dye::red(_("[ERROR] ")) << _("Beacon failed to spawn!\n");
            return;
        }

        std::cout << _("Spawned Beacon!\n");

        *BeaconHost->Member<int>(_("ListenPort")) = Port - 1;
        auto bInitBeacon = InitHost(BeaconHost);

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
        PauseBeaconRequests(BeaconHost, false);
        *NetDriver->Member<UObject*>(_("World")) = World;
    }
    else
    {
        NetDriver = CreateNetDriver(GetEngine(), World, FName(282));
        std::cout << _("Created NetDriver!\n");
        InitListen(NetDriver, Helper::GetWorld(), InURL, true, Error);
        std::cout << _("Called InitListen on the NetDriver!\n");
    }

    if (SetWorld && NetDriver)
    {
        std::cout << _("Setting!\n");
        SetWorld(NetDriver, World);
        std::cout << _("Set!\n");
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
                OurReplicationDriver = params.ReturnValue;
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
    // std::cout << std::format(_("Listening for connections on port {}!\n", std::to_string(Port)));
    std::cout << _("Listening for connections!\n");
    SendDiscordStart();
    // CreateThread(0, 0, ReplicationThread, 0, 0, 0);
}

DWORD WINAPI MapLoadThread(LPVOID)
{
    // Sleep(10 * 1000);

    Listen(7777);

    return 0;
}