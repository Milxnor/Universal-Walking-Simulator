#pragma once

#include <patterns.h>
#include <Net/funcs.h>

std::vector<UObject*> ObjectList;

int PrepConnections(UObject* NetDriver)
{
    int ReadyConnections = 0;

    auto ClientConnections = NetDriver->Member<TArray<UObject*>>(_("ClientConnections"));

    for (int ConnIdx = 0; ConnIdx < ClientConnections->Num(); ConnIdx++)
    {
        UObject* Connection = ClientConnections->At(ConnIdx);

        if (!Connection)
            continue;

        UObject* OwningActor = *Connection->Member<UObject*>(_("OwningActor"));

        if (OwningActor)
        {
            ReadyConnections++;
            UObject* DesiredViewTarget = OwningActor;

            auto PlayerController = *Connection->Member<UObject*>(_("PlayerController"));

            if (PlayerController)
            {
                static auto GetViewTarget = PlayerController->Function(_("GetViewTarget"));
                UObject* ViewTarget = nullptr;

                if (GetViewTarget)
                    PlayerController->ProcessEvent(GetViewTarget, &ViewTarget);
               
                if (ViewTarget)
                    DesiredViewTarget = ViewTarget;
            }

            *Connection->Member<UObject*>(_("ViewTarget")) = DesiredViewTarget;

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
            *Connection->Member<UObject*>(_("ViewTarget")) = nullptr;

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
    auto OpenChannels = Connection->Member<TArray<UObject*>>(_("OpenChannels"));

    for (int i = 0; i < OpenChannels->Num(); i++)
    {
        auto Channel = OpenChannels->At(i);

        if (Channel && Channel->ClassPrivate)
        {
            static UObject* ActorChannelClass = FindObject(_("Class /Script/Engine.ActorChannel"));
            if (Channel->ClassPrivate == ActorChannelClass)
            {
                if (*Channel->Member<UObject*>(_("Actor")) == Actor)
                    return Channel;
            }
        }
    }

    return nullptr;
}

void BuildConsiderList(UObject* NetDriver, std::vector<UObject*>& OutConsiderList)
{
    static auto World = *NetDriver->Member<UObject*>(_("World"));

    if (!World || !&OutConsiderList || !NetDriver)
        return;

    auto& List = ObjectList;

    for (auto& Object : List)
    {
        auto Actor = Object;

        if (!Actor || *Actor->Member<ENetRole>(_("RemoteRole")) == ENetRole::ROLE_None) // || Actor->bActorIsBeingDestroyed)
            continue;

        if (*Actor->Member<ENetDormancy>(_("NetDormancy")) == ENetDormancy::DORM_Initial && *Actor->Member<char>(_("bNetStartup")))
            continue;

        //if (!GetRelevancy(Actor))
            // continue;


        // if (Actor->Name.ComparisonIndex != 0)
        {
            CallPreReplication(Actor, NetDriver);
            OutConsiderList.push_back(Actor);
        }
    }
}


int32_t ServerReplicateActors(UObject* NetDriver)
{
#ifndef N_T
	// Supports replicationgraph

	auto ReplicationDriver = NetDriver->Member<UObject*>(_("ReplicationDriver"));

	if (ReplicationDriver && *ReplicationDriver)
	{
		if (RepGraph_ServerReplicateActors)
		{
			RepGraph_ServerReplicateActors(*ReplicationDriver);
			return 50; // pretty sure replicationdriver ticks like 50 connections max
		}
	}

	return 0;
#endif
    ++*(int32_t*)(NetDriver + 0x2C8);

    auto NumClientsToTick = PrepConnections(NetDriver);

    if (NumClientsToTick == 0)
        return NumClientsToTick;

    std::vector<UObject*> ConsiderList;
    ConsiderList.reserve(ObjectList.size());
    BuildConsiderList(NetDriver, ConsiderList);

    auto ClientConnections = NetDriver->Member<TArray<UObject*>>(_("ClientConnections"));

    for (int i = 0; i < ClientConnections->Num(); i++)
    {
        auto Connection = ClientConnections->At(i);

        if (!Connection)
            continue;

        if (i >= NumClientsToTick)
            break; // Only tick on ready connections

        else if (*Connection->Member<UObject*>(_("ViewTarget")))
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

            auto PlayerController = *Connection->Member<UObject*>(_("PlayerController"));

            if (PlayerController)
                SendClientAdjustment(PlayerController); // Sending adjustments to children is for splitscreen

            for (auto Actor : ConsiderList)
            {
                static auto PlayerControllerClass = FindObject(_("Class /Script/Engine.PlayerController"));

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

                    Channel = CreateChannel(Connection, EChannelType::CHTYPE_Actor, true, -1);
                    SetChannelActor(Channel, Actor);
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