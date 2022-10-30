#pragma once

#include <Net/funcs.h>
#include <Gameplay/helper.h>
#include "discord.h"

static bool bListening = false;

static UObject* BeaconHost = nullptr;

void setWorld(UObject* netdriver, UObject* world)
{
    if (Engine_Version < 426)
    {
        if (SetWorld)
            SetWorld(netdriver, world);
        else
            std::cout << "Invalid SetWorld!\n";
    }
    else
    {
        int VTableIndex = FnVerDouble < 19.00 ? 0x72 : 0x7A;

        if (FnVerDouble >= 20.00)
            VTableIndex = 0x7B;

        if (FnVerDouble >= 21.00)
            VTableIndex = 0x7C;

        auto SetWorldAAddr = netdriver->VFTable[VTableIndex];
        SetWorld = decltype(SetWorld)(SetWorldAAddr);
        // std::cout << "SetWorld Sig: " << GetBytes(__int64(SetWorldAAddr), 50) << '\n';
        SetWorld(netdriver, world);
    }
}

void AllowConnections(UObject* NetDriver)
{
    const auto World = Helper::GetWorld();

    if (BeaconHost)
    {
        /* if (Engine_Version == 421) // idfk
        {
            auto ahh = (int*)(BeaconHost + 0x340);

            if (ahh)
                *ahh = 0;
            else
                std::cout << "fix yo game!\n";
        }
        else */
        PauseBeaconRequests(BeaconHost, false);
    }
    else
        std::cout << "No BeaconHost!\n";

    if (NetDriver)
    {
        static auto NetDriver_WorldOffset = GetOffset(NetDriver, "World");
        *(UObject**)(__int64(NetDriver) + NetDriver_WorldOffset) = World;

        if (SetWorld)
            SetWorld(NetDriver, World);
        else
            std::cout << ("Invalid SetWorld!\n");
    }
}

DWORD WINAPI MapLoadThread(LPVOID) // DOES NOT WORK
{
    serverStatus = EServerStatus::Loading;

    const auto World = Helper::GetWorld();

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
    if (bRestarting)
        Port -= AmountOfRestarts; // why? don't ask me

    const auto World = Helper::GetWorld();

    UObject* NetDriver = nullptr;

    FString Error;
    auto InURL = FURL();
    InURL.Port = Port;

    if (bUseBeacons)
    {
        static UObject* BeaconHostClass = FindObjectOld("Class /Script/OnlineSubsystemUtils.OnlineBeaconHost", true);

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

        static auto ListenPortOffset = GetOffset(BeaconHost, "ListenPort");

        // if (FnVerDouble < 16.00)
        {
            if (Engine_Version < 426)
                *(int*)(__int64(BeaconHost) + ListenPortOffset) = Port - 1;
            else
                *(int*)(__int64(BeaconHost) + ListenPortOffset) = Port;
        }

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
        else
        {
            std::cout << ("Initialized Beacon!\n");
            static auto Beacon_NetDriverOffset = GetOffset(BeaconHost, "NetDriver");
            NetDriver = *(UObject**)(__int64(BeaconHost) + Beacon_NetDriverOffset);
        }
    }

    static auto World_NetDriverOffset = GetOffset(World, "NetDriver");

    if (!NetDriver)
    {
        std::cout << ("Failed to create NetDriver!\n");
        return;
    }
    else
    {
        static auto ReplicationDriverClass = FindObject(("Class /Script/FortniteGame.FortReplicationGraph"));

        // *NetDriver->Member<UObject*>("ReplicationDriverClass") = ReplicationDriverClass;

        FString string;
        string.Set(L"GameNetDriver");
        auto GameNetDriverName = Helper::StringToName(string);

        static auto NetDriverNameOffset = GetOffset(NetDriver, "NetDriverName");
        *(FName*)(__int64(NetDriver) + NetDriverNameOffset) = GameNetDriverName;

        static auto NetDriver_WorldOffset = GetOffset(NetDriver, "World");
        *(UObject**)(__int64(NetDriver) + NetDriver_WorldOffset) = World;

        *(UObject**)(__int64(World) + World_NetDriverOffset) = NetDriver;

        FString Error;
        auto InURL = FURL();
        InURL.Port = Port;

        setWorld(NetDriver, World);

        std::cout << "InitListen: " << InitListen(NetDriver, World, InURL, false, Error) << '\n';

        setWorld(NetDriver, World);
    }

    // *NetDriver->Member<int>(("MaxClientRate")) = *NetDriver->Member<int>(("MaxInternetClientRate"));
    UObject** ReplicationDriver = nullptr;

    AllowConnections(NetDriver);

    setWorld(NetDriver, World);

    serverStatus = EServerStatus::Up;

    if (NetDriver && Engine_Version >= 420)
    {
        static auto ReplicationDriverOffset = GetOffset(NetDriver, "ReplicationDriver");
        ReplicationDriver = (UObject**)(__int64(NetDriver) + ReplicationDriverOffset);

        if (Engine_Version >= 424)
        {
            if (true) // SetReplicationDriver)
            {
                if (!(*ReplicationDriver))
                {
                }
                else
                    std::cout << dye::green(("\n\n[WARNING] ReplicationDriver is valid, but we are trying to create it. (This is VERY good)\n\n\n"));
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

    static auto LevelCollectionsOffset = GetOffset(World, "LevelCollections");
    auto LevelCollections = (TArray<__int64>*)(__int64(World) + LevelCollectionsOffset);

    static auto LevelCollectionsSize = GetSizeOfStruct(FindObject("ScriptStruct /Script/Engine.LevelCollection"));
    static auto NetDriverOffset = FindOffsetStruct("ScriptStruct /Script/Engine.LevelCollection", "NetDriver");

    if (LevelCollections)
    {
        typedef __int64 TheLevelCollection;
        // std::cout << "OUrs: " << __int64((UObject**)(__int64((TheLevelCollection*)(__int64(LevelCollections->GetData()) + (LevelCollectionsSize * 0))) + NetDriverOffset));
        // std::cout << "Acutal: " << __int64(&LevelCollectionsDa->At(0).NetDriver) << '\n';
        *(UObject**)(__int64((TheLevelCollection*)(__int64(LevelCollections->GetData()) + (LevelCollectionsSize * 0))) + NetDriverOffset) = NetDriver;
        *(UObject**)(__int64((TheLevelCollection*)(__int64(LevelCollections->GetData()) + (LevelCollectionsSize * 1))) + NetDriverOffset) = NetDriver;
    }

    *(UObject**)(__int64(World) + World_NetDriverOffset) = NetDriver;

    bListening = true;
    std::cout << std::format(("Listening for connections on port {}!\n"), bRestarting ? std::to_string(Port + AmountOfRestarts) : std::to_string(Port));
#if 0
    CreateThread(0, 0, MapLoadThread, 0, 0, 0);
#else
#ifndef DPP_DISABLED
    if (bIsBotRunning)
        SendDiscordStart();
#endif
#endif

    // if (FnVerDouble == 14.60)
        // *(__int64**)(__int64(NetDriver) + 0x208) = (__int64*)World; // notify
}