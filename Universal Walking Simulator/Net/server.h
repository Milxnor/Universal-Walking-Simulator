#pragma once

#include <Net/funcs.h>
#include <Gameplay/helper.h>

static bool bListening = false;
UObject* BeaconHost = nullptr;

void Listen(int Port = 7777)
{
    static auto BeaconHostClass = FindObject("Class /Script/FortniteGame.FortOnlineBeaconHost");

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
    auto bInitBeacon = InitHost(BeaconHost);

    if (!bInitBeacon)
    {
        std::cout << dye::red(_("[ERROR] ")) << _("Unable to initialize Beacon!\n");
        return;
    }

    std::cout << _("Initialized Beacon!\n");

    *BeaconHost->Member<FName>(_("NetDriverName")) = FName(282); // REGISTER_NAME(282,GameNetDriver)
    auto& BeaconNetDriver = *BeaconHost->Member<UObject*>(_("NetDriver"));
    *BeaconNetDriver->Member<FName>(_("NetDriverName")) = FName(282); // REGISTER_NAME(282,GameNetDriver)
    // BeaconNetDriver->World = GetWorld();
    FString Error;
    auto InURL = FURL();
    InURL.Port = Port;

    InitListen(BeaconNetDriver, Helper::GetWorld(), InURL, true, Error);

    // SetWorld(BeaconNetDriver, Helper::GetWorld());

    auto& ReplicationDriver = *BeaconNetDriver->Member<UObject*>(_("ReplicationDriver"));

    ServerReplicateActors = decltype(ServerReplicateActors)(ReplicationDriver->VFTable[ServerReplicateActorsOffset]);

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

    *Helper::GetWorld()->Member<UObject*>(_("NetDriver")) = BeaconNetDriver;
    auto LevelCollections = Helper::GetWorld()->Member<TArray<FLevelCollection>>(_("LevelCollections"));
    LevelCollections->At(0).NetDriver = BeaconNetDriver;
    LevelCollections->At(1).NetDriver = BeaconNetDriver;

    // GetWorld()->AuthorityGameMode->GameSession->MaxPlayers = 100;

    PauseBeaconRequests(BeaconHost, false);
    bListening = true;
    // std::cout << std::format(_("Listening for connections on port {}!\n", std::to_string(Port)));
    std::cout << _("Listening for connections!\n");
}