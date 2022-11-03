#pragma once

#include <patterns.h>
#include <Net/funcs.h>

std::vector<UObject*> ObjectList;

int PrepConnections(UObject* NetDriver)
{
    int ReadyConnections = 0;

    static auto ClientConnectionsOffset = GetOffset(NetDriver, "ClientConnections");
    auto ClientConnections = (TArray<UObject*>*)(__int64(NetDriver) + ClientConnectionsOffset);

    for (int ConnIdx = 0; ConnIdx < ClientConnections->Num(); ConnIdx++)
    {
        UObject* Connection = ClientConnections->At(ConnIdx);

        if (!Connection)
            continue;

        static auto Connection_ViewTargetOffset = GetOffset(Connection, "ViewTarget");
        auto ViewTarget = (UObject**)(__int64(Connection) + Connection_ViewTargetOffset);

        static auto OwningActorOffset = GetOffset(Connection, "OwningActor");
        UObject* OwningActor = *(UObject**)(__int64(Connection) + OwningActorOffset);

        if (OwningActor)
        {
            ReadyConnections++;
            UObject* DesiredViewTarget = OwningActor;
            
            static auto Connection_PCOffset = GetOffset(Connection, "PlayerController");
            auto PlayerController = *(UObject**)(__int64(Connection) + Connection_PCOffset);

            if (PlayerController)
            {
                static auto GetViewTarget = PlayerController->Function(("GetViewTarget"));
                UObject* ViewTarget = nullptr;

                if (GetViewTarget)
                    PlayerController->ProcessEvent(GetViewTarget, &ViewTarget);
               
                if (ViewTarget)
                    DesiredViewTarget = ViewTarget;
            }

            *ViewTarget = DesiredViewTarget;

            /* for(int ChildIdx = 0; ChildIdx < Connection->Children.Num(); ++ChildIdx)
            {
                UNetConnection* ChildConnection = Connection->Children[ChildIdx];
                if (ChildConnection && ChildConnection->PlayerController && ChildConnection->ViewTarget)
                {
                    ChildConnection->ViewTarget = DesiredViewTarget;
                }
            } */
        }
        else
        {
            *ViewTarget = nullptr;

            /* for (int ChildIdx = 0; ChildIdx < Connection->Children.Num(); ++ChildIdx)
            {
                UNetConnection* ChildConnection = Connection->Children[ChildIdx];
                if (ChildConnection && ChildConnection->PlayerController && ChildConnection->ViewTarget)
                {
                    ChildConnection->ViewTarget = nullptr;
                }
            } */
        }
    }

    return ReadyConnections;
}

UObject* FindChannel(UObject* Actor, UObject* Connection)
{
    static auto OpenChannelsOffset = GetOffset(Connection, "OpenChannels");
    auto OpenChannels = (TArray<UObject*>*)(__int64(Connection) + OpenChannelsOffset);

    for (int i = 0; i < OpenChannels->Num(); i++)
    {
        auto Channel = OpenChannels->At(i);

        if (Channel && Channel->ClassPrivate)
        {
            static UObject* ActorChannelClass = FindObject(("Class /Script/Engine.ActorChannel"));
            if (Channel->ClassPrivate == ActorChannelClass)
            {
                static auto ActorOffset = GetOffset(Channel, "Actor");
                if (*(UObject**)(__int64(Channel) + ActorOffset) == Actor)
                    return Channel;
            }
        }
    }

    return nullptr;
}

TArray<UObject*> GetAllActors(UObject* World)
{
    static auto ActorsClass = FindObjectOld("Class /Script/Engine.Actor", true);

    auto AllActors = Helper::GetAllActorsOfClass(ActorsClass, World);

    return AllActors;
}

auto GetNetworkObjectList(UObject* NetDriver)
{
    auto Offset = 0;

    if (Engine_Version == 419) // got on 2.4.2
        Offset = 0x490;

    /* if (Engine_Version < 420)
        return ((TSharedPtr<FNetworkObjectList<TSharedPtr<FNetworkObjectInfo>>>*)(__int64(NetDriver) + Offset))->Get();
    else */
        return ((TSharedPtrOld<FNetworkObjectList<TSharedPtrOld<FNetworkObjectInfo>>>*)(__int64(NetDriver) + Offset))->Get();
}

struct Bitfield_242
{
    unsigned char                                      bHidden : 1;                                              // 0x0084(0x0001) (Edit, BlueprintVisible, BlueprintReadOnly, Net)
    unsigned char                                      bNetTemporary : 1;                                        // 0x0084(0x0001)
    unsigned char                                      bNetStartup : 1;                                          // 0x0084(0x0001)
    unsigned char                                      bOnlyRelevantToOwner : 1;                                 // 0x0084(0x0001) (Edit, BlueprintVisible, BlueprintReadOnly, DisableEditOnInstance)
    unsigned char                                      bAlwaysRelevant : 1;                                      // 0x0084(0x0001) (Edit, BlueprintVisible, DisableEditOnInstance)
    unsigned char                                      bReplicateMovement : 1;                                   // 0x0084(0x0001) (Edit, Net, DisableEditOnInstance)
    unsigned char                                      bTearOff : 1;                                             // 0x0084(0x0001) (Net)
    unsigned char                                      bExchangedRoles : 1;                                      // 0x0084(0x0001) (Transient)
};

struct Bitfield2_242
{
    unsigned char                                      AutoDestroyWhenFinished : 1;                             // 0x013C(0x0001) (BlueprintVisible)
    unsigned char                                      bCanBeDamaged : 1;                                        // 0x013C(0x0001) (Edit, BlueprintVisible, Net, SaveGame)
    unsigned char                                      bActorIsBeingDestroyed : 1;                               // 0x013C(0x0001) (Transient, DuplicateTransient)
    unsigned char                                      bCollideWhenPlacing : 1;                                  // 0x013C(0x0001)
    unsigned char                                      bFindCameraComponentWhenViewTarget : 1;                   // 0x013C(0x0001) (Edit, BlueprintVisible)
    unsigned char                                      bRelevantForNetworkReplays : 1;                           // 0x013C(0x0001)
    unsigned char                                      bGenerateOverlapEventsDuringLevelStreaming : 1;           // 0x013C(0x0001) (Edit, BlueprintVisible)
    unsigned char                                      bCanBeInCluster : 1;
};

void BuildConsiderList(UObject* NetDriver, std::vector<FNetworkObjectInfo*>& OutConsiderList, UObject* World = nullptr)
{
    TArray<UObject*> Actors = GetAllActors(World);

    for (int i = 0; i < Actors.Num(); i++)
    {
        auto Actor = Actors.At(i);

        if (!Actor)
            continue;

        /* static auto bActorIsBeingDestroyedOffset = GetOffset(Actor, "bActorIsBeingDestroyed");
        static auto bActorIsBeingDestroyedBI = GetBitIndex(GetProperty(Actor, "bActorIsBeingDestroyed"));

        std::cout << bActorIsBeingDestroyedOffset << '\n';

        // if (readd((uint8_t*)(__int64(Actor) + bActorIsBeingDestroyedOffset), bActorIsBeingDestroyedBI))
        if (((die*)(__int64(Actor) + bActorIsBeingDestroyedOffset))->bActorIsBeingDestroyed)
        {
            std::cout << "bugha!\n";
            continue;
        } */

        static auto bActorIsBeingDestroyedOffset = GetOffset(Actor, "bActorIsBeingDestroyed");

        if ((((Bitfield2_242*)(__int64(Actor) + bActorIsBeingDestroyedOffset))->bActorIsBeingDestroyed))
        {
            // std::cout << "bActorIsBeingDestroyed!\n";
            continue;
        }

        static auto RemoteRoleOffset = GetOffset(Actor, "RemoteRole");

        if ((*(ENetRole*)(__int64(Actor) + RemoteRoleOffset)) == ENetRole::ROLE_None)
        {
            // std::cout << "skidder!\n";
            continue;
        }

        static auto NetDormancyOffset = GetOffset(Actor, "NetDormancy");
        static auto bNetStartupOffset = GetOffset(Actor, "bNetStartup");
        /* static auto bNetStartupBI = GetBitIndex(GetProperty(Actor, "bNetStartup"));

        if ((*(ENetDormancy*)(__int64(Actor) + NetDormancyOffset) == ENetDormancy::DORM_Initial) && readd((uint8_t*)(__int64(Actor) + bNetStartupOffset), bNetStartupBI))
        {
            // std::cout << "daick1!\n";
            continue;
        } */

        if ((*(ENetDormancy*)(__int64(Actor) + NetDormancyOffset) == ENetDormancy::DORM_Initial) && ((Bitfield_242*)(__int64(Actor) + bNetStartupOffset))->bNetStartup)
        {
            // std::cout << "daick1!\n";
            continue;
        }

        if (Actor->NamePrivate.ComparisonIndex != 0)
        {
            CallPreReplication(Actor, NetDriver);

            auto Info = new FNetworkObjectInfo();
            Info->Actor = Actor;

            OutConsiderList.push_back(Info);
        }
    }

    Actors.Free();
}

void ReplicateActors(UObject* NetDriver, UObject* World = nullptr)
{
    // ReplicationFrame
    if (Engine_Version == 416)
        ++* (int32_t*)(NetDriver + 0x288);
    if (Engine_Version == 419)
        ++*(int32_t*)(NetDriver + 0x2C8);
    if (Engine_Version == 420)
        ++* (int32_t*)(NetDriver + 0x330);
    if (Engine_Version == 424)
        ++*(int32_t*)(NetDriver + 0x410);

    auto NumClientsToTick = PrepConnections(NetDriver);

    if (NumClientsToTick == 0)
        return;

    // std::cout << "Replicating!\n";

    std::vector<FNetworkObjectInfo*> ConsiderList;
    BuildConsiderList(NetDriver, ConsiderList, World);

    static auto ClientConnectionsOffset = GetOffset(NetDriver, "ClientConnections");
    // auto& ClientConnections = *NetDriver->CachedMember<TArray<UObject*>>(("ClientConnections"));

    auto ClientConnections = (TArray<UObject*>*)(__int64(NetDriver) + ClientConnectionsOffset);

    // std::cout << "Consider list size: " << ConsiderList.size() << '\n';
    
    for (int i = 0; i < ClientConnections->Num(); i++)
    {
        auto Connection = ClientConnections->At(i);

        if (!Connection)
            continue;

        if (i >= NumClientsToTick)
            continue;

        static auto Connection_ViewTargetOffset = GetOffset(Connection, "ViewTarget");

        auto ViewTarget = *(UObject**)(__int64(Connection) + Connection_ViewTargetOffset);

        if (!ViewTarget)
            continue;

        static auto Connection_PCOffset = GetOffset(Connection, "PlayerController");
        auto PC = *(UObject**)(__int64(Connection) + Connection_PCOffset);

        if (PC && SendClientAdjustment)
        {
            SendClientAdjustment(PC);
        }

        /* for (int32_t ChildIdx = 0; ChildIdx < Connection->Children.Num(); ChildIdx++)
        {
            if (Connection->Children[ChildIdx]->PlayerController != NULL)
            {
                SendClientAdjustment(Connection->Children[ChildIdx]->PlayerController);
            }
        } */

        for (int i = 0; i < ConsiderList.size(); i++)
        {
            // if (!ActorInfo)
               // continue;

            auto ActorInfo = ConsiderList.at(i);

            // std::cout << "Consider list size: " << ConsiderList.size() << '\n';

            auto Actor = ActorInfo->Actor;

            if (!Actor)
            {
                continue;
            }

            static auto PlayerControllerClass = FindObject(("Class /Script/Engine.PlayerController"));

            if (Actor->IsA(PlayerControllerClass) && Actor != PC)
                continue;

            // std::cout << "Considering: " << Actor->GetFullName() << '\n';

            auto Channel = FindChannel(Actor, Connection);

            /* static auto bAlwaysRelevantOffset = GetOffset(Actor, "bAlwaysRelevant");
            static auto bAlwaysRelevantBI = GetBitIndex(GetProperty(Actor, "bAlwaysRelevant"));

            static auto bNetUseOwnerRelevancyOffset = GetOffset(Actor, "bNetUseOwnerRelevancy");
            static auto bNetUseOwnerRelevancyBI = GetBitIndex(GetProperty(Actor, "bNetUseOwnerRelevancy"));

            static auto bOnlyRelevantToOwnerOffset = GetOffset(Actor, "bOnlyRelevantToOwner");
            static auto bOnlyRelevantToOwnerBI = GetBitIndex(GetProperty(Actor, "bOnlyRelevantToOwner"));

            if (!readd((uint8_t*)(__int64(Actor) + bAlwaysRelevantOffset), bAlwaysRelevantBI)
                && !readd((uint8_t*)(__int64(Actor) + bNetUseOwnerRelevancyOffset), bNetUseOwnerRelevancyBI)
                && !readd((uint8_t*)(__int64(Actor) + bOnlyRelevantToOwnerOffset), bOnlyRelevantToOwnerBI))
            {
                if (Connection && ViewTarget)
                {
                    auto Loc = Helper::GetActorLocation(ViewTarget);
                    if (!IsNetRelevantFor(Actor, ViewTarget, ViewTarget, &Loc))
                    {
                        if (Channel)
                            UActorChannel_Close(Channel);

                        continue;
                    }
                }
            } */

            if (!Channel)
            {
                Channel = CreateChannel(Connection, EChannelType::CHTYPE_Actor, true, -1);

                if (Channel) {
                    SetChannelActor(Channel, Actor);
                    // *Channel->Member<UObject*>("Connection") = Connection;
                }
            }

            if (Channel)
                ReplicateActor(Channel);
        }
    }

    ConsiderList.clear();
}