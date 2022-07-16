#pragma once

#include <Net/funcs.h>
#include <Gameplay/helper.h>

static bool bListening = false;

void Listen(int Port = 7777)
{
    bool bUseBeacons = true; // CreateNetDriver ? false : true;
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
    }

    if (SetWorld)
    {
        std::cout << _("Setting!\n");
        SetWorld(NetDriver, World);
        std::cout << _("Set!\n");
    }

    UObject* ReplicationDriver = nullptr;

    if (NetDriver)
        ReplicationDriver = *NetDriver->Member<UObject*>(_("ReplicationDriver"));
    else
        std::cout << _("No NetDriver!\n");

    if (ReplicationDriver)
        ServerReplicateActors = decltype(ServerReplicateActors)(ReplicationDriver->VFTable[ServerReplicateActorsOffset]);
    else
        std::cout << dye::red(_("\n\n[ERROR] NO ReplicationDriver\n\n\n"));

    /* auto ClassRepNodePolicies = GetClassRepNodePolicies(BeaconHost->NetDriver->ReplicationDriver);

    for (auto&& Pair : ClassRepNodePolicies)
    {
        auto key = Pair.Key().ResolveObjectPtr();
        auto& value = Pair.Value();

        LOG_INFO("ClassRepNodePolicies: {} - {}", key->GetName(), ClassRepNodeMappingToString(value));

        if (key == AFortInventory::StaticClass())
        {
            value = EClassRepNodeMapping::RelevantAllConnections;
            LOG_INFO("Found ClassRepNodePolicy for AFortInventory! {}", (int)value);
        }

        if (key == AFortQuickBars::StaticClass())
        {
            value = EClassRepNodeMapping::RelevantAllConnections;
            LOG_INFO("Found ClassRepNodePolicy for AFortQuickBars! {}", (int)value);
        }
    } */

    *World->Member<UObject*>(_("NetDriver")) = NetDriver;
    auto LevelCollections = World->Member<TArray<FLevelCollection>>(_("LevelCollections"));
    LevelCollections->At(0).NetDriver = NetDriver;
    LevelCollections->At(1).NetDriver = NetDriver;

    // GetWorld()->AuthorityGameMode->GameSession->MaxPlayers = 100;
    bListening = true;
    // std::cout << std::format(_("Listening for connections on port {}!\n", std::to_string(Port)));
    std::cout << _("Listening for connections!\n");
}