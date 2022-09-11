#pragma once

#include <Gameplay/helper.h>

#include "Gameplay/gamemode.h"
#include "Misc/discord.h"

namespace Server
{
    enum StatusType
    {
        Up,
        Down,
        Loading,
        Restarting
    };
    
    inline bool Listening = false;

    inline UObject* BeaconHost = nullptr;
    
    inline auto Status = Down;
    
    inline void AllowConnections(UObject* NetDriver)
    {
        static const auto World = Helper::GetWorld();
        if (BeaconHost)
        {
            if (EngineVersion == 421)
            {
                *reinterpret_cast<int*>(BeaconHost + 0x340) = 0;
            }
            else
            {
                PauseBeaconRequests(BeaconHost, false);
            }
        }

        if (NetDriver)
        {
            *NetDriver->Member<UObject*>("World") = World;

            if (SetWorld)
            {
                SetWorld(NetDriver, World);
            }
        }
    }

    inline auto CreateNetDriver(const int Port) -> UObject*
    {
        if (!GameMode::UseBeacons)
        {
            return nullptr;
        }

        static const auto BeaconHostClass = FindObjectOld("Class /Script/OnlineSubsystemUtils.OnlineBeaconHost", true);
        if (!BeaconHostClass)
        {
            return nullptr;
        }

        BeaconHost = Easy::SpawnActor(BeaconHostClass, FVector());
        if (!BeaconHost)
        {
            return nullptr;
        }


        if (EngineVersion < 426)
        {
            *BeaconHost->Member<int>("ListenPort") = Port - 1;
        }
        else
        {
            *BeaconHost->Member<int>("ListenPort") = Port;
        }

        const auto InitBeacon = InitHost && InitHost(BeaconHost);
        if (!InitBeacon)
        {
            return nullptr;
        }

        return *BeaconHost->Member<UObject*>("NetDriver");
    }

    inline void Listen(const int Port = 7777)
    {
        static const auto World = Helper::GetWorld();
        const auto NetDriver = CreateNetDriver(Port);
        if (!NetDriver)
        {
            return;
        }

        FString string;
        string.Set(L"GameNetDriver");

        const auto GameNetDriverName = Helper::StringToName(string);
        *NetDriver->Member<FName>("NetDriverName") = GameNetDriverName;
        *NetDriver->Member<UObject*>("World") = World;
        *World->Member<UObject*>("NetDriver") = NetDriver;

        if (EngineVersion < 426)
        {
            if (SetWorld)
            {
                SetWorld(NetDriver, World);
            }
        }
        else
        {
            const auto VTableIndex = FortniteVersion >= 20.00 ? 0x7B : FortniteVersion < 19.00 ? 0x72 : 0x7A;
            const auto SetWorldAddress = NetDriver->VFTable[VTableIndex];
            SetWorld = static_cast<decltype(SetWorld)>(SetWorldAddress);
            SetWorld(NetDriver, World);
        }

        *NetDriver->Member<int>(("MaxClientRate")) = *NetDriver->Member<int>("MaxInternetClientRate");
        UObject** ReplicationDriver = nullptr;

        AllowConnections(NetDriver);

        Status = Up;

        if (NetDriver && EngineVersion >= 420)
        {
            ReplicationDriver = NetDriver->Member<UObject*>("ReplicationDriver");

            if (EngineVersion >= 424)
            {
                if (!*ReplicationDriver)
                {
                    if (EnableReplicationGraph)
                    {
                        static const auto ReplicationDriverClass = FindObject(
                            ("Class /Script/FortniteGame.FortReplicationGraph"));
                        const auto Ret = Easy::SpawnObject(ReplicationDriverClass, NetDriver);
                        SetReplicationDriver(NetDriver, Ret);
                    }
                }
            }
        }

        if (ReplicationDriver && *ReplicationDriver)
        {
            RepGraphServerReplicateActors = static_cast<decltype(RepGraphServerReplicateActors)>((*ReplicationDriver)->
                VFTable[ServerReplicateActorsOffset]);
        }

        const auto LevelCollections = World->Member<TArray<__int64>>("LevelCollections");
        static auto LevelCollectionsSize = GetSizeOfStruct(FindObject("ScriptStruct /Script/Engine.LevelCollection"));
        static auto NetDriverOffset = FindOffsetStruct("ScriptStruct /Script/Engine.LevelCollection", "NetDriver");
        if (LevelCollections)
        {
            typedef __int64 TheLevelCollection;
            *reinterpret_cast<UObject**>(reinterpret_cast<long long>(reinterpret_cast<TheLevelCollection*>(
                    reinterpret_cast<long long>(LevelCollections->GetData()) + LevelCollectionsSize * 0)) +
                NetDriverOffset) = NetDriver;
            *reinterpret_cast<UObject**>(reinterpret_cast<long long>(reinterpret_cast<TheLevelCollection*>(
                    reinterpret_cast<long long>(LevelCollections->GetData()) + LevelCollectionsSize * 1)) +
                NetDriverOffset) = NetDriver;
        }

        *World->Member<UObject*>("NetDriver") = NetDriver;
        Listening = true;

        #ifndef DPP_DISABLED
        if (IsBotRunning)
        {
            SendDiscordStart();   
        }
        #endif
    }
}
