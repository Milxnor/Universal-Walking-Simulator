#pragma once

#include "functions.h"

namespace Replication
{
    inline auto GetSize()
    {
        if (FortniteVersion >= 3 && FortniteVersion < 4)
        {
            return 0xC8;
        }
        
        if (FortniteVersion > 3 && FortniteVersion < 7.40)
        {
            return 0xD0;
        }

        if (FortniteVersion >= 7.40 && EngineVersion <= 424)
        {
            return 0x120;
        }

        if (EngineVersion == 425)
        {
            return 0x150;
        }

        if (EngineVersion >= 426 && FortniteVersion < 14.60)
        {
            return 0x160;
        }
        
        if (FortniteVersion == 14.60) {
            return 0x180;
        }
        
        if (FortniteVersion >= 15 && FortniteVersion < 18)
        {
            return 0x190;
        }
        
        if (FortniteVersion >= 18)
        {
            return 0x1A0;
        }

        return -1;
    }

    
    inline auto PrepConnections(UObject* NetDriver)
    {
        auto ReadyConnections = 0;
        const auto ClientConnections = NetDriver->Member<TArray<UObject*>>("ClientConnections");
        for (auto ConnIdx = 0; ConnIdx < ClientConnections->Num(); ConnIdx++)
        {
            const auto Connection = ClientConnections->At(ConnIdx);
            if (!Connection)
            {
                continue;
            }

            UObject* OwningActor = *Connection->Member<UObject*>(("OwningActor"));

            if (!OwningActor)
            {
                *Connection->Member<UObject*>(("ViewTarget")) = nullptr;
                continue;
            }

            ReadyConnections++;
            UObject* DesiredViewTarget = OwningActor;

            const auto PlayerController = *Connection->Member<UObject*>(("PlayerController"));
            if (!PlayerController)
            {
                *Connection->Member<UObject*>(("ViewTarget")) = DesiredViewTarget;
                continue;
            }

            UObject* ViewTarget = nullptr;
            if (static const auto GetViewTarget = PlayerController->Function("GetViewTarget"))
            {
                PlayerController->ProcessEvent(GetViewTarget, &ViewTarget);
            }

            if (ViewTarget)
            {
                DesiredViewTarget = ViewTarget;
            }
            *Connection->Member<UObject*>(("ViewTarget")) = DesiredViewTarget;
        }

        return ReadyConnections;
    }

    inline auto FindChannel(const UObject* Actor, UObject* Connection) -> UObject*
    {
        const auto OpenChannels = Connection->Member<TArray<UObject*>>("OpenChannels");

        for (auto Index = 0; Index < OpenChannels->Num(); Index++)
        {
            const auto Channel = OpenChannels->At(Index);
            if (!Channel || !Channel->ClassPrivate)
            {
                continue;
            }

            if (static const auto ActorChannelClass = FindObject("Class /Script/Engine.ActorChannel"); Channel->
                ClassPrivate != ActorChannelClass)
            {
                continue;
            }

            if (*Channel->Member<UObject*>("Actor") != Actor)
            {
                continue;
            }

            return Channel;
        }

        return nullptr;
    }

    inline auto GetNetworkObjectList(UObject* NetDriver)
    {
        auto Offset = 0;
        if (EngineVersion == 419)
        {
            Offset = 0x490;
        }

        return EngineVersion < 420
                   ? reinterpret_cast<TSharedPtr<FNetworkObjectList>*>(reinterpret_cast<long long>(NetDriver) + Offset)
                   ->Get()
                   : reinterpret_cast<TSharedPtrOld<FNetworkObjectList>*>(reinterpret_cast<long long>(NetDriver) +
                       Offset)->Get();
    }

    // TODO: Check NetDriver Name
    inline void ServerReplicateActorsBuildConsiderList(UObject* NetDriver, TArray<FNetworkObjectInfo*>& OutConsiderList)
    {
        static auto WorldOffset = GetOffset(NetDriver, "World");
        if (const auto World = *reinterpret_cast<UObject**>(reinterpret_cast<long long>(NetDriver) + WorldOffset); !
            World)
        {
            return;
        }

        const auto ObjectList = GetNetworkObjectList(NetDriver);
        const auto [Elements, Hash, HashSize] = ObjectList->ActiveNetworkObjects;

        for (int j = 0; j < Elements.Data.Num(); j++)
        {
            const auto ActorIdk = Elements.Data.At(j);
            const auto ActorInfo = ActorIdk.ElementData.Value.Get();
            if (!ActorInfo)
            {
                continue;
            }

            auto Actor = ActorInfo->Actor;
            if (!Actor)
            {
                continue;
            }

            static const auto RemoteRoleOffset = GetOffset(Actor, "RemoteRole");

            if (reinterpret_cast<TEnumAsByte<ENetRole>*>(reinterpret_cast<long long>(Actor) + RemoteRoleOffset)->Get()
                == ENetRole::ROLE_None)
            {
                continue;
            }

            if (static auto ActorIsBeingDestroyedOffset = GetOffset(Actor, "bActorIsBeingDestroyed"); reinterpret_cast<
                    Bitfield2_242*>(reinterpret_cast<long long>(Actor) + ActorIsBeingDestroyedOffset)->
                bActorIsBeingDestroyed)
            {
                continue;
            }

            static const auto NetDormancyOffset = GetOffset(Actor, "NetDormancy");
            if (static const auto NetStartupOffset = GetOffset(Actor, "bNetStartup");
                *reinterpret_cast<ENetDormancy*>(reinterpret_cast<long long>(Actor) + NetDormancyOffset) ==
                ENetDormancy::DORM_Initial
                && reinterpret_cast<Bitfield_242*>(reinterpret_cast<long long>(Actor) + NetStartupOffset)->bNetStartup)
            {
                continue;
            }

            CallPreReplication(Actor, NetDriver);
            OutConsiderList.Add(ActorInfo);
        }
    }

    inline int32_t ServerReplicateActors(UObject* NetDriver)
    {
        if (!NetDriver)
        {
            return -1;
        }

        if (EngineVersion >= 420)
        {
            static const auto ReplicationDriverOffset = GetOffset(NetDriver, "ReplicationDriver");
            if (const auto ReplicationDriver = reinterpret_cast<UObject**>(reinterpret_cast<long long>(NetDriver) +
                ReplicationDriverOffset); ReplicationDriver && *ReplicationDriver)
            {
                if (RepGraphServerReplicateActors)
                {
                    RepGraphServerReplicateActors(*ReplicationDriver);
                    return 50;
                }
            }

            return -1;
        }

        if (EngineVersion == 419)
        {
            ++*reinterpret_cast<int32_t*>(NetDriver + 0x2C8);
        }
        else if (EngineVersion == 424)
        {
            ++*reinterpret_cast<int32_t*>(NetDriver + 0x410);
        }

        const auto NumClientsToTick = PrepConnections(NetDriver);

        if (NumClientsToTick == 0)
        {
            return NumClientsToTick;
        }

        TArray<FNetworkObjectInfo*> ConsiderList;
        ConsiderList.Reserve(GetNetworkObjectList(NetDriver)->ActiveNetworkObjects.Elements.Data.Num());

        ServerReplicateActorsBuildConsiderList(NetDriver, ConsiderList);

        const auto ClientConnections = NetDriver->Member<TArray<UObject*>>("ClientConnections");
        for (int i = 0; i < ClientConnections->Num(); i++)
        {
            const auto Connection = ClientConnections->At(i);
            if (!Connection)
            {
                continue;
            }

            static auto ViewTargetOffset = GetOffset(Connection, "ViewTarget");
            const auto ViewTarget = reinterpret_cast<UObject**>(reinterpret_cast<long long>(Connection) +
                ViewTargetOffset);
            if (i >= NumClientsToTick)
            {
                break;
            }

            if (!ViewTarget || !*ViewTarget)
            {
                continue;
            }

            static auto PlayerControllerOffset = GetOffset(Connection, "PlayerController");
            const auto PlayerControllerR = reinterpret_cast<UObject**>(reinterpret_cast<long long>(Connection) +
                PlayerControllerOffset);

            if (!PlayerControllerR || !*PlayerControllerR)
            {
                continue;
            }

            const auto PlayerController = *PlayerControllerR;
            if (SendClientAdjustment && PlayerController)
            {
                SendClientAdjustment(PlayerController);
            }

            auto j = 0;
            while (j < ConsiderList.Num())
            {
                const auto ActorInfo = ConsiderList.At(j);
                if (!ActorInfo)
                {
                    continue;
                }

                const auto Actor = ActorInfo->Actor;
                if (static auto PlayerControllerClass = FindObject("Class /Script/Engine.PlayerController"); Actor->
                    IsA(PlayerControllerClass) && Actor != PlayerController)
                {
                    continue;
                }

                auto Channel = FindChannel(Actor, Connection);
                if (!Channel)
                {
                    FName ActorName(102);
                    Channel = EngineVersion >= 422
                                  ? CreateChannelByName(Connection, &ActorName, EChannelCreateFlags::OpenedLocally, -1)
                                  : CreateChannel(Connection, CHTYPE_Actor, true, -1);
                    if (Channel)
                    {
                        SetChannelActor(Channel, Actor);
                    }
                }

                if (Channel)
                {
                    ReplicateActor(Channel);
                }

                j++;
            }
        }

        return NumClientsToTick;
    }
}
