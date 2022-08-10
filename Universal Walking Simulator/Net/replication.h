#pragma once

#include <patterns.h>
#include <Net/funcs.h>

std::vector<UObject*> ObjectList;

int PrepConnections(UObject* NetDriver)
{
    int ReadyConnections = 0;

    auto ClientConnections = NetDriver->Member<TArray<UObject*>>(("ClientConnections"));

    for (int ConnIdx = 0; ConnIdx < ClientConnections->Num(); ConnIdx++)
    {
        UObject* Connection = ClientConnections->At(ConnIdx);

        if (!Connection)
            continue;

        UObject* OwningActor = *Connection->Member<UObject*>(("OwningActor"));

        if (OwningActor)
        {
            ReadyConnections++;
            UObject* DesiredViewTarget = OwningActor;

            auto PlayerController = *Connection->Member<UObject*>(("PlayerController"));

            if (PlayerController)
            {
                static auto GetViewTarget = PlayerController->Function(("GetViewTarget"));
                UObject* ViewTarget = nullptr;

                if (GetViewTarget)
                    PlayerController->ProcessEvent(GetViewTarget, &ViewTarget);
               
                if (ViewTarget)
                    DesiredViewTarget = ViewTarget;
            }

            *Connection->Member<UObject*>(("ViewTarget")) = DesiredViewTarget;

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
            *Connection->Member<UObject*>(("ViewTarget")) = nullptr;

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
    auto OpenChannels = Connection->Member<TArray<UObject*>>(("OpenChannels"));

    for (int i = 0; i < OpenChannels->Num(); i++)
    {
        auto Channel = OpenChannels->At(i);

        if (Channel && Channel->ClassPrivate)
        {
            static UObject* ActorChannelClass = FindObject(("Class /Script/Engine.ActorChannel"));
            if (Channel->ClassPrivate == ActorChannelClass)
            {
                if (*Channel->Member<UObject*>(("Actor")) == Actor)
                    return Channel;
            }
        }
    }

    return nullptr;
}

TArray<UObject*> GetAllActors(UObject* World)
{
    /* if (!World) return nullptr;

    auto Level = World->Member<UObject*>(("PersistentLevel"));

    if (!*Level || !Level) return nullptr;

    auto OwningWorld = (*Level)->Member<UObject*>(("OwningWorld"));

    if (!*OwningWorld || !OwningWorld) return nullptr;

    return (TArray<UObject*>*)(&*OwningWorld - 2); */

    static auto ActorsClass = FindObject(("Class /Script/Engine.Actor"));
    auto AllActors = Helper::GetAllActorsOfClass(ActorsClass);
    return AllActors;
}

void BuildConsiderList(UObject* NetDriver, std::vector<UObject*>& OutConsiderList)
{
    static auto World = *NetDriver->Member<UObject*>(("World"));

    if (!World || !NetDriver)
        return;

    // auto& List = ObjectList;

    TArray<UObject*> Actors = GetAllActors(World);

    std::cout << ("NumActors: ") << Actors.Num() << '\n';

    for (int j = 0; j < Actors.Num(); j++)
    {
        auto Actor = Actors.At(j);

        if (!Actor || *Actor->Member<ENetRole>(("RemoteRole")) == ENetRole::ROLE_None)
            continue;

        // TODO: Fix because bitfields makes it replicate A LOT of actors that should NOT be replicated
        if (*Actor->Member<ENetDormancy>(("NetDormancy")) == ENetDormancy::DORM_Initial && *Actor->Member<char>(("bNetStartup")))
            continue;
        {
            CallPreReplication(Actor, NetDriver);
            OutConsiderList.push_back(Actor);
        }
    }
}


int32_t ServerReplicateActors(UObject* NetDriver)
{
    if (!NetDriver)
        return -1;

	// Supports replicationgraph

    if (Engine_Version >= 420)
    {
        auto ReplicationDriver = NetDriver->Member<UObject*>(("ReplicationDriver"));

        if (ReplicationDriver && *ReplicationDriver)
        {
            if (RepGraph_ServerReplicateActors)
            {
                RepGraph_ServerReplicateActors(*ReplicationDriver);
                return 50; // pretty sure replicationdriver ticks like 50 connections max
            }
        }

        return -1;
    }

    // ReplicationFrame
#ifdef N_T
    ++*(int32_t*)(NetDriver + 0x2C8);
#endif
#ifdef F_TF
    ++*(int32_t*)(NetDriver + 0x410);
#endif

    auto NumClientsToTick = PrepConnections(NetDriver);

    if (NumClientsToTick == 0)
        return NumClientsToTick;

    std::vector<UObject*> ConsiderList;
    // ConsiderList.reserve(ObjectList.size());
    BuildConsiderList(NetDriver, ConsiderList);

    std::cout << ("Considering: ") << ConsiderList.size() << '\n';

    auto ClientConnections = NetDriver->Member<TArray<UObject*>>(("ClientConnections"));

    for (int i = 0; i < ClientConnections->Num(); i++)
    {
        auto Connection = ClientConnections->At(i);

        if (!Connection)
            continue;

        if (i >= NumClientsToTick)
            break; // Only tick on ready connections

        else if (*Connection->Member<UObject*>(("ViewTarget")))
        {
            /*
            static auto ConnectionViewers = GetWorld()->PersistentLevel->WorldSettings->ReplicationViewers;
            ConnectionViewers.Reset();
            ConnectionViewers.Add(FNetViewer(Connection));

            for (int32 ViewerIndex = 0; ViewerIndex < Connection->Children.Num(); ViewerIndex++)
            {
                if (Connection->Children[ViewerIndex]->ViewTarget)
                {
                    ConnectionViewers.Add(FNetViewer(Connection->Children[ViewerIndex]));
                }
            }
            */

            auto PlayerController = *Connection->Member<UObject*>(("PlayerController"));

            if (SendClientAdjustment && PlayerController)
                SendClientAdjustment(PlayerController); // Sending adjustments to children is for splitscreen

            for (auto Actor : ConsiderList)
            {
                static auto PlayerControllerClass = FindObject(("Class /Script/Engine.PlayerController"));

                if (Actor->IsA(PlayerControllerClass) && Actor != PlayerController)
                    continue;

                auto Channel = FindChannel(Actor, Connection);

                if (!Channel)
                {
                    /*
                    if (!IsActorRelevantToConnection(Actor, ConnectionViewers) && !Actor->bAlwaysRelevant)
                    {
                        // If not relevant (and we don't have a channel), skip
                        continue;
                    }
                    */

#ifdef N_T
                    Channel = CreateChannel(Connection, EChannelType::CHTYPE_Actor, true, -1);
#else
                    // EName ActorEName = EName::Actor;

                    // FNameEntryId ActorEntryId = FromValidEName(ActorEName);

                   //  FName ActorName = FName(ActorEName);

                    FName ActorName(102);

                    std::cout << ("Comparison Index: ") << ActorName.ComparisonIndex << '\n';
                    std::cout << ("Number: ") << ActorName.Number << '\n';

                    Channel = CreateChannelByName(Connection, &ActorName, EChannelCreateFlags::OpenedLocally, -1);
#endif
                    if (Channel)
                    {
                        SetChannelActor(Channel, Actor);
                        std::cout << ("Created Channel for Actor => ") << Actor->GetFullName() << '\n';
                    }
                    else
                    {
                        std::cout << ("Unable to Create Channel!\n");
                    }
                }

                if (Channel)
                {
                    // if (IsActorRelevantToConnection(Actor, ConnectionViewers) || Actor->bAlwaysRelevant) // temporary
                    {
                        ReplicateActor(Channel);
                    }
                    // else // techinally we should wait like 5 seconds but whatever.
                    {
                        // todo get pattern
                        // Native::ActorChannel::Close(Channel);
                    }
                }
            }
        }
    }

    return NumClientsToTick;
}