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

    return (TArray<UObject*>*)(__int64(&*OwningWorld) - 32); */

    static auto ActorsClass = FindObjectOld("Class /Script/Engine.Actor", true);
    // auto AllActors = Helper::GetAllActorsOfClass(ActorsClass, World);

    TArray<UObject*> AllActors;

    for (int32_t i = 0; i < (ObjObjects ? ObjObjects->Num() : OldObjects->Num()); i++)
    {
        auto Object = GetByIndex(i);

        if (Object->IsA(ActorsClass))
        {
            AllActors.Add(Object);
        }
    }

    return AllActors;
}

FNetworkObjectList* GetNetworkObjectList(UObject* NetDriver)
{
    auto Offset = 0;

    if (Engine_Version == 419) // got on 2.4.2
        Offset = 0x490;

    if (Engine_Version < 420)
        return ((TSharedPtr<FNetworkObjectList>*)(__int64(NetDriver) + Offset))->Get();
    else
        return ((TSharedPtrOld<FNetworkObjectList>*)(__int64(NetDriver) + Offset))->Get();
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

void BuildConsiderList(UObject* NetDriver, std::vector<UObject*>& OutConsiderList)
{
    static auto WorldOffset = GetOffset(NetDriver, "World");
    auto World = *(UObject**)(__int64(NetDriver) + WorldOffset);

    if (!World)
        return;

    /* auto ObjectList = GetNetworkObjectList(NetDriver);
    auto ActiveNetworkObjects = ObjectList->ActiveNetworkObjects;

    std::cout << ("NumActors: ") << ActiveNetworkObjects.Elements.Data.Num() << '\n';

    for (int j = 0; j < ActiveNetworkObjects.Elements.Data.Num(); j++)
    {
        auto& ActorIdk = ActiveNetworkObjects.Elements.Data.At(j);
        auto ActorInfo = ActorIdk.ElementData.Value.Get();
        auto Actor = ActorInfo->Actor; */

    TArray<UObject*> Actors = GetAllActors(World);

    std::cout << ("NumActors: ") << Actors.Num() << '\n';

    for (int i = 0; i < Actors.Num(); i++)
    {
        auto Actor = Actors.At(i);

        if (!Actor)
            continue;

        // std::cout << "RemoteRoleOffset: " << RemoteRoleOffset << '\n';

        static auto RemoteRoleOffset = GetOffset(Actor, "RemoteRole");

        if (((TEnumAsByte<ENetRole>*)(__int64(Actor) + RemoteRoleOffset))->Get() == ENetRole::ROLE_None)
        {
            // std::cout << "actor: " << Actor << '\n';
            continue;
        }

        static auto bActorIsBeingDestroyedOffset = GetOffset(Actor, "bActorIsBeingDestroyed");

        if ((((Bitfield2_242*)(__int64(Actor) + bActorIsBeingDestroyedOffset))->bActorIsBeingDestroyed))
        {
            std::cout << "bActorIsBeingDestroyed!\n";
            continue;
        }

        static auto NetDormancyOffset = GetOffset(Actor, "NetDormancy");
        static auto bNetStartupOffset = GetOffset(Actor, "bNetStartup");

        if ((*(ENetDormancy*)(__int64(Actor) + NetDormancyOffset) == ENetDormancy::DORM_Initial) && ((Bitfield_242*)(__int64(Actor) + bNetStartupOffset))->bNetStartup)
        {
            continue;
        }

        if (Actor->NamePrivate.ComparisonIndex != 0)
        {
            CallPreReplication(Actor, NetDriver);
            OutConsiderList.push_back(Actor);
        }
    }

    Actors.Free();
}

static bool bInThing = false;

int32_t ServerReplicateActors(UObject* NetDriver)
{
    bInThing = true;

    if (!NetDriver)
    {
        bInThing = false;
        return -1;
    }

	// Supports replicationgraph

    if (Engine_Version >= 420)
    {
        static auto ReplicationDriverOffset = GetOffset(NetDriver, "ReplicationDriver");
        auto ReplicationDriver = (UObject**)(__int64(NetDriver) + ReplicationDriverOffset);

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
    if (Engine_Version == 419)
        ++*(int32_t*)(NetDriver + 0x2C8);
    if (Engine_Version == 424)
        ++*(int32_t*)(NetDriver + 0x410);

    auto NumClientsToTick = PrepConnections(NetDriver);

    if (NumClientsToTick == 0)
        return NumClientsToTick;

    // std::vector<UObject*> ConsiderList;
    std::vector<UObject*> ConsiderList;
    ConsiderList.reserve(2000);
    // ConsiderList.reserve(ObjectList.size());
    // BuildConsiderList(NetDriver, ConsiderList);

    std::cout << ("Considering: ") << ConsiderList.size() << '\n';

    auto ClientConnections = NetDriver->Member<TArray<UObject*>>(("ClientConnections"));

    for (int i = 0; i < ClientConnections->Num(); i++)
    {
        auto Connection = ClientConnections->At(i);

        if (!Connection)
            continue;

        static auto ViewTargetOffset = GetOffset(Connection, "ViewTarget");
        auto ViewTarget = (UObject**)(__int64(Connection) + ViewTargetOffset);

        if (i >= NumClientsToTick)
            break; // Only tick on ready connections

        else if (ViewTarget && *ViewTarget)
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

            static auto PlayerControllerOffset = GetOffset(Connection, "PlayerController");
            auto PlayerControllerR = (UObject**)(__int64(Connection) + PlayerControllerOffset);

            if (!(PlayerControllerR && *PlayerControllerR))
                continue;
            
            auto PlayerController = *PlayerControllerR;

            if (SendClientAdjustment && PlayerController)
                SendClientAdjustment(PlayerController); // Sending adjustments to children is for splitscreen

            std::cout << "a\n";

            int j = 0;

            TArray<UObject*> Actors = GetAllActors(Helper::GetWorld());

            if (!Actors.GetData())
                continue;

            std::cout << ("NumActors: ") << Actors.Num() << '\n';

            for (int i = 0; i < Actors.Num(); i++)
            {
                auto Actor = Actors.At(i);

                if (!Actor)
                    continue;

                // std::cout << "RemoteRoleOffset: " << RemoteRoleOffset << '\n';

                static auto RemoteRoleOffset = GetOffset(Actor, "RemoteRole");

                if (((TEnumAsByte<ENetRole>*)(__int64(Actor) + RemoteRoleOffset))->Get() == ENetRole::ROLE_None)
                {
                    // std::cout << "actor: " << Actor << '\n';
                    continue;
                }

                static auto bActorIsBeingDestroyedOffset = GetOffset(Actor, "bActorIsBeingDestroyed");

                if ((((Bitfield2_242*)(__int64(Actor) + bActorIsBeingDestroyedOffset))->bActorIsBeingDestroyed))
                {
                    std::cout << "bActorIsBeingDestroyed!\n";
                    continue;
                }

                static auto NetDormancyOffset = GetOffset(Actor, "NetDormancy");
                static auto bNetStartupOffset = GetOffset(Actor, "bNetStartup");

                if ((*(ENetDormancy*)(__int64(Actor) + NetDormancyOffset) == ENetDormancy::DORM_Initial) && ((Bitfield_242*)(__int64(Actor) + bNetStartupOffset))->bNetStartup)
                {
                    continue;
                }

                if (Actor->NamePrivate.ComparisonIndex != 0)
                {
                    CallPreReplication(Actor, NetDriver);

                    std::cout << "b\n";

                    // auto Actor = ConsiderList.at(j);

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

                        // EName ActorEName = EName::Actor;

                        // FNameEntryId ActorEntryId = FromValidEName(ActorEName);

                       //  FName ActorName = FName(ActorEName);

                        FName ActorName(102); // Helper::StringToName(idk)

                        // std::cout << ("Comparison Index: ") << ActorName.ComparisonIndex << '\n';
                        // std::cout << ("Number: ") << ActorName.Number << '\n';

                        if (Engine_Version >= 422)
                            Channel = CreateChannelByName(Connection, &ActorName, EChannelCreateFlags::OpenedLocally, -1);
                        else
                            Channel = CreateChannel(Connection, EChannelType::CHTYPE_Actor, true, -1);

                        if (Channel)
                        {
                            SetChannelActor(Channel, Actor);
                            // std::cout << ("Created Channel for Actor => ") << Actor->GetFullName() << '\n';
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
                            // ReplicateActor(Channel);
                        }
                        // else // techinally we should wait like 5 seconds but whatever.
                        {
                            // todo get pattern
                            // Native::ActorChannel::Close(Channel);
                        }
                    }
                }
            }
            /*
            while(j < ConsiderList.size())
            {
                std::cout << "b\n";
                
                auto Actor = ConsiderList.at(j);

                static auto PlayerControllerClass = FindObject(("Class /Script/Engine.PlayerController"));

                if (Actor->IsA(PlayerControllerClass) && Actor != PlayerController)
                    continue;

                auto Channel = FindChannel(Actor, Connection);

                if (!Channel)
                {

                    // EName ActorEName = EName::Actor;

                    // FNameEntryId ActorEntryId = FromValidEName(ActorEName);

                   //  FName ActorName = FName(ActorEName);

                    FName ActorName(102); // Helper::StringToName(idk)

                    // std::cout << ("Comparison Index: ") << ActorName.ComparisonIndex << '\n';
                    // std::cout << ("Number: ") << ActorName.Number << '\n';

                    if (Engine_Version >= 422)
                        Channel = CreateChannelByName(Connection, &ActorName, EChannelCreateFlags::OpenedLocally, -1);
                    else
                        Channel = CreateChannel(Connection, EChannelType::CHTYPE_Actor, true, -1);

                    if (Channel)
                    {
                        SetChannelActor(Channel, Actor);
                        // std::cout << ("Created Channel for Actor => ") << Actor->GetFullName() << '\n';
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

                j++;
            } */
        }
    }

    bInThing = false;

    return NumClientsToTick;
}