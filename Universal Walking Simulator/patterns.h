#pragma once

// 3.5 Patterns are from raider, some patterns are not mine.

#include "Net/funcs.h"
#include "finder.h"

std::string RequestExitWSSig = "48 89 5C 24 ? 57 48 83 EC 40 41 B9 ? ? ? ? 0F B6 F9 44 38 0D ? ? ? ? 0F B6 DA 72 24 89 5C 24 30 48 8D 05 ? ? ? ? 89 7C 24 28 4C 8D 05 ? ? ? ? 33 D2 48 89 44 24 ?  33 C9 E8";

void InitializePatterns()
{
    // { // 4.21 patterns by default
    std::string MallocSig = ("48 83 EC ? 48 8B 0D ? ? ? ? 48 85 C9 75 ? 33 D2 B9 ? ? ? ? E8 ? ? ? ? 48 8B 4C 24 58");
    std::string ReallocSig = ("48 89 5C 24 08 48 89 74 24 10 57 48 83 EC ? 48 8B F1 41 8B D8 48 8B 0D ? ? ? ?");
    std::string FNameToStringSig = ("48 89 5C 24 08 57 48 83 EC ? 83 79 04 ?");
    std::string TickFlushSig = ("4C 8B DC 55 49 8D AB 98 FE FF FF 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 00 01 00 00 49 89 5B 18 48 8D 05 ? ? ? ?");
    std::string CreateNetDriverSig = ("48 89 5C 24 ? 57 48 83 EC 30 48 8B 81 ? ? ? ? 49 8B D8 4C 63 81 ? ? ? ? 4C 8B D2 48 8B F9 4E 8D 0C C0 49 3B C1 74 1B");
    std::string SetWorldSig = ("48 89 5C 24 ? 57 48 83 EC 20 48 8B FA 48 8B D9 48 8B 91 ? ? ? ? 48 85 D2 74 28 E8 ? ? ? ? 48 8B 8B ? ? ? ?");
    std::string InitListenSig = ("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 50 48 8B BC 24 ? ? ? ? 49 8B F0");
    std::string WelcomePlayerSig = ("48 89 5C 24 ? 48 89 74 24 ? 55 57 41 56 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B 59 30 48 8B F1 48 8B CA 48 8B FA E8 ? ? ? ? 48 8B CB E8 ? ? ? ? 48 8D 54 24 ? 48 8D 8D ? ? ? ? 4C 8B");
    std::string World_NotifyControlMessageSig = ("48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 48 89 4C 24 ? 55 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 33 FF 49 8B D9 89 7C 24 54");
    std::string SpawnPlayActorSig = ("48 8B C4 4C 89 48 20 44 89 40 18 48 89 50 10 48 89 48 08 55 56");
    std::string ReceiveUniqueIdReplSig = ("48 89 5C 24 ? 55 56 57 48 8B EC 48 83 EC 40 F6 41 28 40 48 8B FA 48 8B D9 0F 84 ? ? ? ? F6 41 2B 02");
    std::string ReceiveFStringSig = ("48 89 5C 24 18 55 56 57 41 56 41 57 48 8D 6C 24 C9 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 27 0F B6 41 28");
    std::string KickPlayerSig = ("48 89 5C 24 08 48 89 74 24 10 57 48 83 EC ? 49 8B F0 48 8B DA 48 85 D2");
    std::string GetNetModeSig = ("48 89 5C 24 ? 57 48 83 EC 20 48 8B 01 48 8B D9 FF 90 ? ? ? ? 4C 8B 83 ? ? ? ? 48 8B F8 33 C0 48 C7 44 24");
    std::string LocalPlayerSpawnPlayActorSig = ("40 55 53 56 57 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 40 48 8B D9 4D 8B F1 49 8B C9 4D 8B F8 48 8B F2");
    std::string CollectGarbageSig = ("48 89 5C 24 08 48 89 6C 24 10 56 57 41 56 48 81 EC ? ? ? ? 4C 8B 05");
    std::string NetDebugSig = ("40 55 56 41 56 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 8B 01 48 8B F1 FF 90 ? ? ? ? 4C 8B F0 48 85 C0 0F 84 ? ? ? ? 48 83 78");
    std::string SetReplicationDriverSig = ("40 55 41 57 48 83 EC 28 48 8B E9 4C 8B FA 48 8B 89 ? ? ? ? 48 85 C9 0F 84 ? ? ? ? 48 8B 01 48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 4C 89 64 24 ? 4C 89 74 24 ? FF");
    std::string GetPlayerViewpointSig = ("48 89 5C 24 ? 48 89 74 24 ? 55 41 56 41 57 48 8B EC 48 83 EC 40 48 8B F2 48 C7 45 ? ? ? ? ? 48 8B 55 38 4D 8B F0 48 8B D9 45 33 FF E8 ? ? ? ? 84 C0 74 4A 80 BB ? ? ? ? ? 75 41 48 8B 03 48 8B CB");
    std::string IdkfSig = ("49 63 C0 48 FF C8 48 85 C0 7E 37 4C 8B CA 4C 8D 04 10 49 F7 D9");
    std::string SendChallengeSig = ("48 89 5C 24 ? 55 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B DA 48 85 D2 0F 84 ? ? ? ? 83 BA ? ? ? ? ? 0F 86 ? ? ? ? 48 83 7A ? ? 0F 84 ? ? ? ? 48 8D 4D 78 48 89 BC 24 ? ? ? ? FF 15 ? ? ? ? 44 8B 45 78 48 8D 15 ? ? ? ? 48 8D 4C 24 ? E8 ? ? ? ? 48 8D 44 24 ? 48 8D BB ? ? ? ? 48 3B F8 74 25 48 8B 0F 48 85 C9 74 05 E8 ? ? ? ? 48 8B 44 24 ? 48 89 07 8B 44 24 38 89 47 08 8B 44 24 3C 89 47 0C EB 0F 48 8B 4C 24 ? 48 85 C9 74 05 E8 ? ? ? ? B2 05 48 8B CB E8 ? ? ? ? 48 8B 83 ? ? ? ? 48 8B 10 48 85 D2 0F 84 ? ? ? ? F6 42 30 02 0F 85 ? ? ? ? 45 33 C0 48 8D 4C 24 ? E8 ? ? ? ?");

    // paddin

    std::string CreateNamedNetDriverSig = ("");
    std::string HandleReloadCostSig = ("89 54 24 10 55 41 56 48 8D 6C 24 ? 48 81 EC ? ? ? ? 80 B9 ? ? ? ? ? 4C 8B F1 0F 85");
    std::string CanActivateAbilitySig = ("4C 89 4C 24 20 55 56 57 41 56 48 8D 6C 24 D1");
    std::string StaticFindObjectSig = ("48 89 5C 24 ? 48 89 74 24 ? 55 57 41 54 41 56 41 57 48 8B EC 48 83 EC 60 80 3D ? ? ? ? ? 45 0F B6 F1 49 8B F8 48 8B DA 4C 8B F9 74 4E 48 8B 05 ? ? ? ? 48 8D 35 ? ? ? ?");
    std::string InitHostSig = ("48 8B C4 48 81 EC ? ? ? ? 48 89 58 18 4C 8D 05 ? ? ? ?");
    std::string PauseBeaconRequestsSig = ("40 53 48 83 EC 30 48 8B 99 ? ? ? ? 48 85 DB 0F 84 ? ? ? ? 84 D2 74 68 80 3D ? ? ? ? ? 72 2C 48 8B 05 ? ? ? ? 4C 8D 44 24 ? 48 89 44 24 ? 41 B9");
    std::string Beacon_NotifyControlMessageSig = ("4C 8B DC 49 89 5B 18 49 89 7B 20 49 89 4B 08");
    std::string HasClientLoadedCurrentWorldSig = ("48 89 5C 24 ? 57 48 83 EC 20 48 8B 99 ? ? ? ? 48 8B F9 48 85 DB 74 23 E8 ? ? ? ? 48 8B 53 10 4C 8D 40 30 48 63 40 38 3B 42 38 7F 0D 48 8B C8 48 8B 42 30 4C 39 04 C8 74 17 48 8B 1D ? ? ? ? 48 85 DB");
    std::string malformedSig = ("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 50 40 32 F6 0F 29 7C 24 ? 48 8B FA 44 0F 29 44 24 ? 48 8B D9 40 38 72 28 7C 51 4C 8B 92 ? ? ? ? 4C 3B 92 ? ? ? ? 7C 0F");
    std::string FixCrashSig = ("40 55 56 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 48 8D 6C 24 ? 48 89 9D ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C5 48 89 85 ? ? ? ? 8B 41 0C 45 33 F6");
    std::string ValidationFailureSig = ("48 89 4C 24 ? 55 53 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 33 DB 48 8B F2");
    std::string GiveAbilitySig = ("48 89 5C 24 ? 48 89 6C 24 ? 48 89 7C 24 ? 41 56 48 83 EC 20 83 B9 ? ? ? ? ? 49 8B E8 4C 8B F2 48 8B F9 7E 56 48 63 9F ? ? ? ? 48 81 C7 ? ? ? ? 8D 43 01 89 87 ? ? ? ? 3B 87 ? ? ? ? 7E");
    std::string InternalTryActivateAbilitySig = ("4C 89 4C 24 20 4C 89 44 24 18 89 54 24 10 55 53 56 57 41 54");
    std::string MarkAbilitySpecDirtySig = ("48 89 5C 24 ? 48 89 7C 24 ? 41 56 48 83 EC 20 48 8B 01 41 0F B6 D8 4C 8B F2 48 8B F9 FF 90 ? ? ? ? 84 C0 0F 84 ? ? ? ? 49 8B 46 10 48 89 6C 24 ?");
    std::string CreateNetDriver_LocalSig = ("4C 89 44 24 ? 53 56 57 41 56 41 57 48 83 EC 70 48 8B B9 ? ? ? ? 48 8D 1D ? ? ? ? 48 63 81 ? ? ? ? 45 33 FF 4C 8B F2 48 8D 0C 40 4C 8D 0C CF 49 3B F9 74 22 44 8B 94 24 ? ? ? ? 44 39 57 04 0F 94 C1 44 39 07 0F 94 C0");
    std::string NoReserveSig = "";
    std::string ClientTravelSig = "";
    std::string PlayMontageSig = "";
    std::string CrashPatchSig = "";
    std::string Beacon_NotifyAcceptingConnectionSig = "";
    std::string World_NotifyAcceptingConnectionSig = "";
    std::string PostRenderSig = "";
    std::string ReplicationGraph_EnableSig = "";
    std::string ValidENameSig = "";
    std::string StaticLoadObjectSig = "4C 89 4C 24 ? 48 89 54 24 ? 48 89 4C 24 ? 55 53 56 57 41 54 41 55 41 56 41 57 48 8B EC 48 83 EC 78 45 33 F6 48 8D 05 ? ? ? ?";
    std::string ReplicateActorSig = "40 55 56 41 54 41 55 41 56 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 4C 8B E9 48 8B 49 68 48 8B 01 FF 90 ? ? ? ? 41 8B 4D 30 48 8D 35 ? ? ? ? 4C 8B F0 0F BA E1 08 0F 83 ? ? ? ? 41 83 7D ? ? 7E 12 32 C0 48 81 C4 ? ? ? ? 41 5E 41 5D 41 5C"; // ud
    std::string CreateChannelSig = "40 56 57 41 54 41 55 41 57 48 83 EC 60 48 8B 01 41 8B F9 45 0F B6 E0 4C 63 FA 48 8B F1 FF 90 ? ? ? ? 45 33 ED 83 FF FF 0F 85 ? ? ? ? 4C 63 8E ? ? ? ? 41 83 FF 01 41 8B FD B8"; // ud
    std::string SetChannelActorSig = "48 8B C4 55 53 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 48 89 70 E8 48 8B D9 48 89 78 E0 48 8D 35 ? ? ? ? 4C 89 60 D8 48 8B FA 45 33 E4 4C 89 78 C8 44 89 A5 ? ? ? ? 45 8B FC 48 8B 41 28 48 8B 48 58 48 85 C9 0F 84 ? ? ? ?"; // ud
    std::string SendClientAdjustmentSig = "40 53 48 83 EC 20 48 8B 99 ? ? ? ? 48 39 99 ? ? ? ? 74 0A 48 83 B9 ? ? ? ? ? 74 78 48 85 DB 75 0C 48 8B 99 ? ? ? ? 48 85 DB 74 67 80 BB ? ? ? ? ? 75 5E 48 8B 81 ? ? ? ? 33 D2 48 89 54 24 ? 48 3B C2 74 09"; // ud    // }
    std::string CallPreReplicationSig = "";
    std::string IsNetRelevantForSig = "";
    std::string ActorChannelCloseSig = "";

    if (Engine_Version == 419)
    {
        ActorChannelCloseSig = "48 89 5C 24 ? 48 89 7C 24 ? 41 56 48 83 EC 60 33 FF";
        ReallocSig = "4C 8B D1 48 8B 0D ? ? ? ? 48 85 C9 75 08 49 8B CA E9 ? ? ? ? 48 8B 01 45 8B C8 4C 8B C2 49 8B D2 48 FF 60 18";// ud
        TickFlushSig = "4C 8B DC 55 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 45 0F 29 43 ? 45 0F 29 4B ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 49 89 5B 18 49 89 7B E8 48 8B F9 4D 89 6B D8 45 33 ED 4D 89 7B C8"; // ud
        World_NotifyControlMessageSig = "40 55 53 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 33 FF 48 89 4D D0 89 7C 24 58 49 8B D9";// ud
        SpawnPlayActorSig = "44 89 44 24 ? 48 89 54 24 ? 48 89 4C 24 ? 55 53 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 33 F6 48 8D 05 ? ? ? ? 89 75 67 4D 8B E9 4C 8B 65 77 49 39 04 24 74"; // ud
        KickPlayerSig = "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 49 8B F0 48 8B DA 48 85 D2 74 78 48 8B BA ? ? ? ? 48 85 FF 74 6C E8 ? ? ? ? 48 8B 57 10 4C 8D 88";// ud
        Beacon_NotifyControlMessageSig = "40 55 53 56 57 41 54 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 33 FF 48 89 4C 24 ? 89 7C 24 60 4D 8B F1 48 8B 41 10 45 0F"; // ud
        InitHostSig = "48 8B C4 48 81 EC ? ? ? ? 48 89 58 18 4C 8D 05 ? ? ? ? 48 8B D9 48 89 78 F8 48 8D 48 88 45 33 C9 33 D2 E8 ? ? ? ? E8 ? ? ? ? 48 8B C8 4C 8D 84 24 ? ? ? ? 48 8D 15 ? ? ? ? E8 ? ? ? ?"; // ud
        PauseBeaconRequestsSig = "40 53 48 83 EC 30 48 8B D9 84 D2 74 68 80 3D ? ? ? ? ? 72 2C 48 8B 05 ? ? ? ? 4C 8D 44 24 ? 48 89 44 24 ? 41 B9 ? ? ? ? 48 8D 05 ? ? ? ? 33 D2 33 C9 48 89 44 24 ? E8 ? ? ? ?"; // ud
        StaticFindObjectSig = "48 89 5C 24 ? 48 89 74 24 ? 55 57 41 54 41 56 41 57 48 8B EC 48 83 EC 60 80 3D ? ? ? ? ? 45 0F B6 F1 49 8B F8 48 8B DA 4C 8B F9 74 52 48 8B 05 ? ? ? ? 4C 8D 45 38 48"; // ud
        IsNetRelevantForSig = "48 89 5C 24 ? 56 57 41 56 48 83 EC 70 48 8B FA 4D 8B F1 0F B6 91 ? ? ? ? 49 8B D8 48 8B F1 F6 C2 10 0F 85 ? ? ? ? 48";

        StaticLoadObjectSig = "40 55 53 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 8B 85 ? ? ? ? 33 F6 4C 8B BD ? ? ? ? 49 8B F9 44 0F B6 A5 ? ? ? ? 4D 8B F0 89 74 24 50 48 8B DA";
        NetDebugSig = "40 55 56 41 56 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 8B 01"; // ud

        SetWorldSig = "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 48 8B B1 ? ? ? ? 48 8B FA 48 8B D9 48 85 F6 74 5C 48 8B 93 ? ? ? ? 48 8D 8E ? ? ? ? E8 ? ? ? ? 48 8B 93 ? ? ? ? 48 8D 8E ? ? ? ? E8"; // ud
        InitListenSig = "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 50 48 8B BC 24 ? ? ? ? 49 8B F0";// ud
        CreateNetDriverSig = "4C 89 44 24 ? 53 48 83 EC 20 48 8B D9 E8 ? ? ? ? 4C 8B 44 24 ? 48 8B D0 48 8B CB E8 ? ? ? ? 48 83 C4 20 5B C3"; // ud
        CallPreReplicationSig = "48 85 D2 0F 84 ? ? ? ? 48 8B C4 55 57 41 54 48 8D 68 A1 48 81 EC ? ? ? ? 48 89 58 08 4C 8B E2 48 89";
        SendClientAdjustmentSig = "40 53 48 83 EC 20 48 8B 99 ? ? ? ? 48 39 99 ? ? ? ? 74 0A 48 83 B9 ? ? ? ? ? 74 78 48 85 DB 75 0C 48 8B 99 ? ? ? ? 48 85 DB 74 67 80 BB ? ? ? ? ? 75 5E";
        GiveAbilitySig = "48 89 5C 24 ? 56 57 41 56 48 83 EC 20 83 B9 ? ? ? ? ? 49 8B F0 4C 8B F2 48 8B D9 7E 5E 48 63 BB ? ? ? ? 48 81 C3";
        InternalTryActivateAbilitySig = "4C 89 4C 24 ? 4C 89 44 24 ? 89 54 24 10 55 53 56 41 54 41 55 41";

        // paddin
                   
        NoReserveSig = "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 56 41 57 48 81 EC ? ? ? ? 48 8B 01 49 8B D9 45 0F B6 F0 48 8B F2 48 8B F9 45 32 FF FF 90 ? ? ? ? 48 8B 0D ? ? ? ?";
        WelcomePlayerSig = "48 8B C4 55 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 48 89 58 18 48 8B 59 30 48 89 78 E8 48 8B FA 4C 89 68 D8";//
        ReceiveUniqueIdReplSig = "48 89 5C 24 ? 55 56 57 48 8B EC 48 83 EC 40 F6 41 28 40 48 8B FA 48 8B D9 0F 84 ? ? ? ? F6 41 2B 02";//
        ReceiveFStringSig = "48 89 5C 24 18 55 56 57 41 56 41 57 48 8D 6C 24 C9 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 27 0F B6 41 28";//
        GetNetModeSig = "48 89 5C 24 08 57 48 83 EC ? 48 8B 01 48 8B D9 FF 90 40 01 00 00 4C 8B 83 10 01 00 00";//
        LocalPlayerSpawnPlayActorSig = "48 89 5C 24 08 48 89 74 24 10 48 89 7C 24 18 55 41 56 41 57 48 8D 6C 24 F0 48 81 EC ? ? ? ? 48 8B D9";//
        ValidationFailureSig = "40 53 41 56 48 81 EC ? ? ? ? 48 8B 01 48 8B DA 4C 8B F1 FF 90 ? ? ? ? 48 8B 0D ? ? ? ? 48 8B D0 E8 ? ? ? ? 85 C0 0F 84 ? ? ? ? 48 85 DB 74 75 48 8B 83 ? ? ? ? 48 8B 88 ? ? ? ? 48 85 C9";
        ClientTravelSig = "48 83 EC 48 45 84 C9 74 0D 41 83 F8 02 75 07 66 FF 81 ? ? ? ? 48 8B 44 24 ? 0F 10 00";
        HasClientLoadedCurrentWorldSig = "48 89 5C 24 ? 57 48 83 EC 20 48 8B 99 ? ? ? ? 48 8B F9 48 85 DB 74 23 E8 ? ? ? ? 48 8B 53 10 4C 8D 40 30 48 63 40 38 3B 42 38 7F 0D 48 8B C8 48 8B 42 30 4C 39 04 C8 74 17 48 8B 1D ? ? ? ? 48 85 DB";
        malformedSig = "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 50 40 32 F6 0F 29 7C 24 ? 48 8B FA 44 0F 29 44 24 ? 48 8B D9 40 38 72 28 7C 51 4C 8B 92 ? ? ? ? 4C 3B 92 ? ? ? ? 7C 0F";
        MarkAbilitySpecDirtySig = "48 89 5C 24 ? 48 89 7C 24 ? 41 56 48 83 EC 20 48 8B 01 41 0F B6 D8 4C 8B F2 48 8B F9 FF 90 ? ? ? ? 84 C0 0F 84 ? ? ? ? 49 8B 46 10 48 89 6C 24 ?";
        GetPlayerViewpointSig = "48 89 74 24 ? 55 57 41 56 48 8B EC 48 83 EC 40 48 8B 81 ? ? ? ? 4D 8B F0 48 8B F2 48 8B F9 48 85 C0 0F 84 ? ? ? ? F6 80 ? ? ? ? ? 0F 84 ? ? ? ? 48 8B 88 ? ? ? ? 48 85 C9 74 23 0F 10 89 ? ? ? ? 0F 28 C1";
        CollectGarbageSig = "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 56 41 57 48 83 EC 30 48 8B 35";
        HandleReloadCostSig = "89 54 24 10 55 41 56 48 8D 6C 24 ? 48 81 EC ? ? ? ? 80 B9 ? ? ? ? ? 4C 8B F1 0F 85";
        CanActivateAbilitySig = "4C 89 4C 24 20 55 56 57 41 56 48 8D 6C 24 D1";
        FixCrashSig = "40 55 56 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 48 8D 6C 24 ? 48 89 9D ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C5 48 89 85 ? ? ? ? 8B 41 0C 45 33 F6";
        SetReplicationDriverSig = "40 55 41 57 48 83 EC 28 48 8B E9 4C 8B FA 48 8B 89 ? ? ? ? 48 85 C9 0F 84 ? ? ? ? 48 8B 01 48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 4C 89 64 24 ? 4C 89 74 24 ? FF 90";
        CreateNetDriver_LocalSig = "4C 89 44 24 ? 53 56 57 41 56 41 57 48 83 EC 70 48 8B B9 ? ? ? ? 48 8D 1D ? ? ? ? 48 63 81 ? ? ? ? 45 33 FF 4C 8B F2 48 8D 0C 40 4C 8D 0C CF 49 3B F9 74 22 44 8B 94 24 ? ? ? ? 44 39 57 04 0F 94 C1 44 39 07 0F 94 C0";
    }

    if (Engine_Version == 416)
    {
        TickFlushSig = "4C 8B DC 55 53 56 57 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 41 0F 29 7B ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 70 48 8B 01 48 8B F1";
        World_NotifyControlMessageSig = "40 55 53 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 45 33 F6 48 89 4D 90 44 89 74 24 ? 49 8B D9 48 8B 41 10 45 0F B6 E0 48 8B F2 4C 8B E9";
        SpawnPlayActorSig = "44 89 44 24 ? 48 89 54 24 ? 48 89 4C 24 ? 55 53 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 33 F6 48 8D 05 ? ? ? ? 89 75 67 4D 8B E9 4C 8B 65 77 49 39 04";
        PauseBeaconRequestsSig = "40 53 48 83 EC 30 48 8B D9 84 D2 74 6F 80 3D ? ? ? ? ? 72 33 48 8B 05 ? ? ? ? 4C 8D 44 24 ? 41 B9 ? ? ? ? 48 89 44 24 ? 48 8D 05 ? ? ? ? 48 8D";
        Beacon_NotifyControlMessageSig = "40 55 53 56 57 41 54 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 33 FF 48 89 4C";
        SetChannelActorSig = "4C 8B DC 55 53 57 41 54 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 45 33 E4 4D 89 73 D0 44 89 A5 ? ? ? ? 4C 8D 35 ? ? ? ? 48 8B 41 28 48 8B D9 4D";
        CreateChannelSig = "40 56 57 41 54 41 55 41 57 48 83 EC 60 48 8B 01 41 8B F9 45 0F B6 E0 4C 63 FA 48 8B F1 FF 90 ? ? ? ? 45 33 ED 83 FF FF 75 52 41 8B FD 48 8D 8E ? ? ? ? 41 83 FF 01 B8";
        ReplicateActorSig = "40 55 53 57 41 56 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8D 59 68 4C 8B F1 48 8B 0B 48 8B 01 FF 90 ? ? ? ? 41 8B 4E 30 48 8D 3D";
        SetWorldSig = "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 48 8B B1 ? ? ? ? 48 8B FA 48 8B D9 48 85 F6 74 5C 48 8B 93";
        SendClientAdjustmentSig = "40 53 48 83 EC 20 48 8B 99 ? ? ? ? 48 39 99 ? ? ? ? 74 0A 48 83 B9 ? ? ? ? ? 74 78 48 85 DB 75 0C 48 8B 99 ? ? ? ? 48 85 DB 74 67 80 BB ? ? ? ? ? 75 5E";
        StaticFindObjectSig = "4C 8B DC 57 48 81 EC ? ? ? ? 80 3D ? ? ? ? ? 49 89 6B F0 49 89 73 E8 49 8B F0";
        NoReserveSig = "48 89 5C 24 ? 48 89 6C 24 ? 57 41 56 41 57 48 81 EC ? ? ? ? 48 8B 01 49 8B E9 45 0F";
    }

    if (FnVerDouble >= 2.5 && FnVerDouble <= 3.3)
    {
        CreateChannelSig = "40 56 57 41 54 41 55 41 57 48 83 EC 60 48 8B 01 41 8B F9 45 0F B6 E0 4C 63 FA 48 8B F1";
        CallPreReplicationSig = "48 85 D2 0F 84 ? ? ? ? 56 41 56 48 83 EC 38 4C 8B F2 48 89 5C 24 ? 48 89 6C 24 ? 48 8D 54 24 ? 48 8B F1";
        ReplicateActorSig = "40 55 56 41 54 41 55 41 56 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 4C 8B E9 48 8B 49 68";
        SetChannelActorSig = "48 8B C4 55 53 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 48 89 70 ? 48 8B D9 48 89 78";
        SendClientAdjustmentSig = "40 53 48 83 EC 20 48 8B 99 ? ? ? ? 48 39 99 ? ? ? ?";
    }

    if (Engine_Version == 420)
    {
        CrashPatchSig = ("0F 29 78 D8 44 0F 29 40 ? F3 45 0F 10 80 ? ? ? ? F3 41 0F 10 B0 ? ? ? ? F3 41 0F 10 88 ? ? ? ? F3 41 0F 10 A0 ? ? ? ? 44 0F 29 58 ? 44 0F 29 60 ? 45 0F 57 E4 48 8B 81 ? ? ? ? 48 05 ? ? ? ? F3 0F 58 48 ? F3 44 0F 10 18 F3 0F 10 78 ? 41 0F 28 D3 F3 0F 10 68 ? F3 41 0F 5C D0 0F 28 DF 0F 28 C5 F3 0F 58 0D E0 FA 75 02"); //4.5 Only

        KickPlayerSig = ("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 49 8B F0 48 8B DA 48 85 D2 74 ? 48 8B BA ? ? ? ? 48");
        SetWorldSig = ("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 48 8B B1 ? ? ? ? 48 8B FA 48 8B D9 48 85 F6 74 5C");

        MallocSig = ("4C 8B C9 48 8B 0D ? ? ? ? 48 85 C9 75 08 49 8B C9 E9 ? ? ? ?");
        ReallocSig = ("4C 8B D1 48 8B 0D ? ? ? ? 48 85 C9 75 08 49 8B CA E9 ? ? ? ? 48 8B 01 45 8B C8 4C 8B C2 49 8B D2 48 FF 60 18");

        ReceiveFStringSig = ("40 55 53 56 57 41 56 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 27 0F B6 41 28");
        ReceiveUniqueIdReplSig = ("48 89 5C 24 ? 55 56 57 48 8B EC 48 83 EC 40 F6 41 28 40 48 8B FA 48 8B D9 0F 84 ? ? ? ? F6 41 2B 02");

        TickFlushSig = ("4C 8B DC 55 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 49 89 5B 18 48 8D 05 ? ? ? ? 49 89 7B E8 48 8B F9 4D 89 63 E0 45 33 E4");
        PauseBeaconRequestsSig = ("40 53 48 83 EC 30 48 8B D9 84 D2 74 68 80 3D ? ? ? ? ? 72 2C 48 8B 05 ? ? ? ? 4C 8D 44 24 ? 48 89 44 24 ?");
        Beacon_NotifyAcceptingConnectionSig = ("48 83 EC 48 48 8B 41 10 48 83 78 ? ?");
        InitHostSig = ("48 8B C4 48 81 EC ? ? ? ? 48 89 58 18 4C 8D 05 ? ? ? ?");
        Beacon_NotifyControlMessageSig = ("40 55 53 56 57 41 54 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 33 FF 48 89 4C 24 ? 89 7C 24 60 49 8B F1 48 8B 41 10 45 0F B6 E0 4C 8B F2 48 8B D9 44 8B FF 48 39 78 78 0F 85 ? ? ? ? 80 3D ? ? ? ? ?");
        WelcomePlayerSig = ("48 8B C4 55 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 48 89 70 20");
        World_NotifyControlMessageSig = ("40 55 53 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 45 33 F6 49 8B D9 44 89 74 24 ? 45 8B E6 48 8B 41 10 45 0F B6 F8 48 8B FA 4C 8B E9 4C 39 60 78");
        SpawnPlayActorSig = ("44 89 44 24 ? 48 89 54 24 ? 48 89 4C 24 ? 55 53 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 33 F6 48 8D 05 ? ? ? ? 89 75 67 4D 8B E9 4C 8B 65 77 49 39 04 24 74 2A 41 89 74 24 ? 41 39 74 24 ?");
        World_NotifyAcceptingConnectionSig = ("40 55 48 83 EC 50 48 8B 41 10 48 8B E9 48 83 78 ? ? 74 45 80 3D ? ? ? ? ? 72 34 48 8B 05 ? ? ? ? 4C 8D 44 24 ? 48 89 44 24 ? 48 8D 0D ? ? ? ? 48 8D 05 ? ? ? ? 41 B9 ? ? ? ? BA ? ? ? ? 48 89 44 24 ?");
        GetNetModeSig = ("48 89 5C 24 ? 57 48 83 EC 20 48 8B 01 48 8B D9 FF 90 ? ? ? ? 4C 8B 83 ? ? ? ? 48 8B F8 33 C0 48 C7 44 24");
        GiveAbilitySig = ("48 89 5C 24 ? 56 57 41 56 48 83 EC 20 83 B9 ? ? ? ? ? 49 8B F0 4C 8B F2 48 8B D9 7E 61");
        InternalTryActivateAbilitySig = ("4C 89 4C 24 ? 4C 89 44 24 ? 89 54 24 10 55 53 56");
        MarkAbilitySpecDirtySig = ("48 89 5C 24 ? 57 48 83 EC 20 80 B9 ? ? ? ? ? 48 8B FA 48 8B D9 75 4A C6 81");
        LocalPlayerSpawnPlayActorSig = ("40 55 53 56 57 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 40 48 8B D9 4D 8B F1 49 8B C9 4D 8B F8 48 8B F2");

        InitListenSig = ("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 50 48 8B BC 24 ? ? ? ? 49 8B F0");
        PostRenderSig = ("48 89 74 24 ? 57 48 81 EC ? ? ? ? 80 B9 ? ? ? ? ? 48 8B F2");

        StaticFindObjectSig = ("48 89 5C 24 ? 48 89 74 24 ? 55 57 41 54 41 56 41 57 48 8B EC 48 83 EC 60 80 3D ? ? ? ? ? 45 0F B6 F1 49 8B F8 48 8B DA 4C 8B F9 74 52 48 8B 05");
        HandleReloadCostSig = ("89 54 24 10 55 41 56 48 8D 6C 24 ? 48 81 EC ? ? ? ? 80 B9 ? ? ? ? ? 4C 8B F1 0F 85");

        CollectGarbageSig = ("40 53 48 83 EC 40 48 8B D9 FF 15 ? ? ? ? 84 C0 75 76 33 D2 33 C9 E8 ? ? ? ? 84 C0 74 69 33 C0 88 44 24 58 48 8D 44 24 ?");
        NetDebugSig = ("40 55 56 41 56 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 8B 01 48 8B F1 FF 90 ? ? ? ? 4C 8B F0 48 85 C0 0F");
        GetPlayerViewpointSig = ("48 89 5C 24 ? 48 89 74 24 ? 55 41 56 41 57 48 8B EC 48 83 EC 50 48 8B F2 48 C7 45 ? ? ? ? ? 48 8B 55 D0 4D 8B F0 48 8B D9 45 33 FF E8 ? ? ? ? 84 C0 74 4A 80 BB ? ? ? ? ? 75 41");

        // paddin
        CanActivateAbilitySig = ("4C 89 4C 24 20 55 56 57 41 56 48 8D 6C 24 D1");
        HasClientLoadedCurrentWorldSig = ("48 89 5C 24 ? 57 48 83 EC 20 48 8B 99 ? ? ? ? 48 8B F9 48 85 DB 74 23 E8 ? ? ? ? 48 8B 53 10 4C 8D 40 30 48 63 40 38 3B 42 38 7F 0D 48 8B C8 48 8B 42 30 4C 39 04 C8 74 17 48 8B 1D ? ? ? ? 48 85 DB");
        ClientTravelSig = ("48 83 EC 48 45 84 C9 74 0D 41 83 F8 02 75 07 66 FF 81 ? ? ? ? 48 8B 44 24 ? 0F 10 00");
        CreateNetDriverSig = ("48 89 5C 24 08 57 48 83 EC ? 49 8B D8 48 8B F9 E8 ? ? ? ? 48 8B D0");
        malformedSig = ("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 50 40 32 F6 0F 29 7C 24 ? 48 8B FA 44 0F 29 44 24 ? 48 8B D9 40 38 72 28 7C 51 4C 8B 92 ? ? ? ? 4C 3B 92 ? ? ? ? 7C 0F");
        SetReplicationDriverSig = ("40 56 41 56 48 83 EC 28 48 8B F1 4C 8B F2 48 8B 89 ? ? ? ? 48 85 C9 0F 84 ? ? ? ? 48 8B 01 48 89 5C 24 ? 48 89 6C 24 ? 4C 89 7C 24 ? FF 90 ? ? ? ? 48 8B 9E ? ? ? ? 45 33 FF");
        FixCrashSig = ("40 55 56 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 48 8D 6C 24 ? 48 89 9D ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C5 48 89 85 ? ? ? ? 8B 41 0C 45 33 F6");
        StaticLoadObjectSig = "4C 89 4C 24 ? 48 89 54 24 ? 48 89 4C 24 ? 55 53 56 57 48 8B EC";
        NoReserveSig = ("aaaaa");
        ValidationFailureSig = ("bbbb");
    }

    else if (Engine_Version >= 422 && Engine_Version < 424)
    {
        ReallocSig = ("48 89 5C 24 08 48 89 74 24 10 57 48 83 EC ? 48 8B F1 41 8B D8 48 8B 0D");
        TickFlushSig = ("4C 8B DC 55 49 8D AB 78 FE FF FF 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 00 01 00 00 49 89 5B 18");
        InitListenSig = ("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 50 48 8B BC 24 ? ? ? ? 49 8B F0");
        WelcomePlayerSig = ("48 8B C4 55 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 48 89 58 18 48 8B 59 30 48 89 78 E8 48 8B FA 4C 89 68 D8");
        World_NotifyControlMessageSig = ("48 89 5C 24 10 48 89 74 24 18 48 89 7C 24 20 48 89 4C 24 08 55 41 54 41 55 41 56 41 57 48 8D AC 24 D0 F9 FF FF");
        SpawnPlayActorSig = ("48 8B C4 4C 89 48 20 44 89 40 18 48 89 50 10 48 89 48 08 55 56");
        ReceiveUniqueIdReplSig = ("48 89 5C 24 ? 55 56 57 48 8B EC 48 83 EC 40 F6 41 28 40 48 8B FA 48 8B D9 0F 84 ? ? ? ? F6 41 2B 02");
        ReceiveFStringSig = ("48 89 5C 24 18 55 56 57 41 56 41 57 48 8D 6C 24 C9 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 27 0F B6 41 28");
        KickPlayerSig = ("48 89 5C 24 08 48 89 74 24 10 57 48 83 EC ? 49 8B F0 48 8B DA 48 85 D2");
        GetNetModeSig = ("48 89 5C 24 08 57 48 83 EC ? 48 8B 01 48 8B D9 FF 90 40 01 00 00 4C 8B 83 10 01 00 00");
        LocalPlayerSpawnPlayActorSig = ("48 89 5C 24 08 48 89 74 24 10 48 89 7C 24 18 55 41 56 41 57 48 8D 6C 24 F0 48 81 EC ? ? ? ? 48 8B D9");
        Beacon_NotifyControlMessageSig = ("4C 8B DC 49 89 5B 18 49 89 7B 20 49 89 4B 08");
        InitHostSig = ("48 8B C4 48 81 EC ? ? ? ? 48 89 58 18 4C 8D 05 ? ? ? ?");
        PauseBeaconRequestsSig = ("40 53 48 83 EC 30 48 8B D9 84 D2 74 68 80 3D ? ? ? ? ? 72 2C 48 8B 05 ? ? ? ? 4C 8D 44 24 ? 48 89 44 24 ? 41 B9 ? ? ? ?");
        SetWorldSig = ("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 48 8B 99 ? ? ? ? 48 8B F2 48 8B F9 48 85 DB 0F 84 ? ? ? ? 48 8B 97 ? ? ? ? 48 8D 8B ? ? ? ? E8 ? ? ? ?");
        NoReserveSig = ("48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 56 41 57 48 81 EC ? ? ? ? 48 8B 01 49 8B D9 45 0F B6 F0 48 8B F2 48 8B F9 45 32 FF FF 90 ? ? ? ? 48 8B 0D ? ? ? ?");
        ValidationFailureSig = ("40 53 41 56 48 81 EC ? ? ? ? 48 8B 01 48 8B DA 4C 8B F1 FF 90 ? ? ? ? 48 8B 0D ? ? ? ? 48 8B D0 E8 ? ? ? ? 85 C0 0F 84 ? ? ? ? 48 85 DB 74 75 48 8B 83 ? ? ? ? 48 8B 88 ? ? ? ? 48 85 C9");
        ClientTravelSig = ("48 83 EC 48 45 84 C9 74 0D 41 83 F8 02 75 07 66 FF 81 ? ? ? ? 48 8B 44 24 ? 0F 10 00");
        CreateNetDriverSig = ("48 89 5C 24 08 57 48 83 EC ? 49 8B D8 48 8B F9 E8 ? ? ? ? 48 8B D0");
        HasClientLoadedCurrentWorldSig = ("48 89 5C 24 ? 57 48 83 EC 20 48 8B 99 ? ? ? ? 48 8B F9 48 85 DB 74 23 E8 ? ? ? ? 48 8B 53 10 4C 8D 40 30 48 63 40 38 3B 42 38 7F 0D 48 8B C8 48 8B 42 30 4C 39 04 C8 74 17 48 8B 1D ? ? ? ? 48 85 DB");
        malformedSig = ("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 50 40 32 F6 0F 29 7C 24 ? 48 8B FA 44 0F 29 44 24 ? 48 8B D9 40 38 72 28 7C 51 4C 8B 92 ? ? ? ? 4C 3B 92 ? ? ? ? 7C 0F");
        GiveAbilitySig = ("48 89 5C 24 ? 48 89 6C 24 ? 48 89 7C 24 ? 41 56 48 83 EC 20 83 B9 ? ? ? ? ? 49 8B E8 4C 8B F2 48 8B F9 7E 56 48 63 9F ? ? ? ? 48 81 C7 ? ? ? ? 8D 43 01 89 87 ? ? ? ? 3B 87 ? ? ? ? 7E");
        InternalTryActivateAbilitySig = ("4C 89 4C 24 20 4C 89 44 24 18 89 54 24 10 55 53 56 57 41 54");
        MarkAbilitySpecDirtySig = ("48 89 5C 24 ? 48 89 7C 24 ? 41 56 48 83 EC 20 48 8B 01 41 0F B6 D8 4C 8B F2 48 8B F9 FF 90 ? ? ? ? 84 C0 0F 84 ? ? ? ? 49 8B 46 10 48 89 6C 24 ?");
        GetPlayerViewpointSig = ("48 89 74 24 ? 55 57 41 56 48 8B EC 48 83 EC 40 48 8B 81 ? ? ? ? 4D 8B F0 48 8B F2 48 8B F9 48 85 C0 0F 84 ? ? ? ? F6 80 ? ? ? ? ? 0F 84 ? ? ? ? 48 8B 88 ? ? ? ? 48 85 C9 74 23 0F 10 89 ? ? ? ? 0F 28 C1");
        CollectGarbageSig = ("48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 56 41 57 48 83 EC 30 48 8B 35");
        StaticFindObjectSig = ("48 89 5C 24 ? 48 89 74 24 ? 55 57 41 54 41 56 41 57 48 8B EC 48 83 EC 60 80 3D ? ? ? ? ? 45 0F B6 F1 49 8B F8 48 8B DA 4C 8B F9 74 4E 48 8B 05 ? ? ? ? 48 8D 35 ? ? ? ?");
        HandleReloadCostSig = ("89 54 24 10 55 41 56 48 8D 6C 24 ? 48 81 EC ? ? ? ? 80 B9 ? ? ? ? ? 4C 8B F1 0F 85");
        CanActivateAbilitySig = ("4C 89 4C 24 20 55 56 57 41 56 48 8D 6C 24 D1");
        PlayMontageSig = ("40 55 56 41 54 41 56 48 8D 6C 24 D1");
        StaticLoadObjectSig = "40 55 53 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 8B 85 ? ? ? ? 33 FF 89 44 24 54 4C 8B FA 48 8B 85 ? ? ? ? 4C 8B E9 48 89 45 A8 48 8D 4D 30 0F B6 85 ? ? ? ? 8B F7 88 44 24 51 4D";

        // paddin

        NetDebugSig = ("40 55 56 41 56 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 8B 01");
        FixCrashSig = ("40 55 56 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 48 8D 6C 24 ? 48 89 9D ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C5 48 89 85 ? ? ? ? 8B 41 0C 45 33 F6");
        SetReplicationDriverSig = ("40 55 41 57 48 83 EC 28 48 8B E9 4C 8B FA 48 8B 89 ? ? ? ? 48 85 C9 0F 84 ? ? ? ? 48 8B 01 48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 4C 89 64 24 ? 4C 89 74 24 ? FF 90");
        CreateNetDriver_LocalSig = ("4C 89 44 24 ? 53 56 57 41 56 41 57 48 83 EC 70 48 8B B9 ? ? ? ? 48 8D 1D ? ? ? ? 48 63 81 ? ? ? ? 45 33 FF 4C 8B F2 48 8D 0C 40 4C 8D 0C CF 49 3B F9 74 22 44 8B 94 24 ? ? ? ? 44 39 57 04 0F 94 C1 44 39 07 0F 94 C0");
    }

    else if (Engine_Version == 424)
    {
        TickFlushSig = ("4C 8B DC 55 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 49 89 5B 18 49 89 73 F0 48 8B F1 49 89 7B E8 48 8D 0D ? ? ? ? 4D 89 73 D0");
        World_NotifyControlMessageSig = ("48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 48 89 4C 24 ? 55 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 33 FF");
        SpawnPlayActorSig = ("48 8B C4 4C 89 48 20 44 89 40 18 48 89 50 10 48 89 48 08 55 56 48 8D 68 B9 48 81 EC ? ? ? ? 48 89 78 E0 33 F6 4C 89 60 D8 49 8B F9 4C 89 68 D0 4C 8D 2D ? ? ? ? 89 75 AF 4C 8B 65 77");
        ReceiveUniqueIdReplSig = ("48 89 5C 24 ? 55 56 57 48 8B EC 48 83 EC 40 F6 41 28 40 48 8B FA 48 8B D9 0F 84 ? ? ? ? F6 41 2B 04 0F 85 ? ? ? ? 48 8B 4A 08 33 F6 48 85 C9 74 16 48 8B 01 FF 50 20 84 C0 74 0C");
        ReceiveFStringSig = ("48 89 5C 24 ? 55 56 57 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 27 F6 41 28 01 48 8B F2 48 8B F9 0F 84 ? ? ? ? 48");
        LocalPlayerSpawnPlayActorSig = ("48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 55 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B D9 4D 8B F1 49 8B C9 4D 8B F8 48 8B F2");
        Beacon_NotifyControlMessageSig = ("4C 8B DC 49 89 5B 18 49 89 73 20 49 89 4B 08 55 57 41 55 41 56 41 57 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 33 FF 49 8B F1 89 7C 24 50 45 0F B6 E8 48 8B 41 10 4C 8B F2");
        InitHostSig = ("48 8B C4 48 81 EC ? ? ? ? 48 89 58 18 4C 8D 05 ? ? ? ? 48 8B D9 48 89 78 F8 48 8D 48 88 45 33 C9 33 D2 E8 ? ? ? ? E8 ? ? ? ? 48 8B C8 4C 8D 84 24 ? ? ? ? 48 8D 15 ? ? ? ? E8 ? ? ? ? 84 C0 74 11 8B");
        PauseBeaconRequestsSig = ("40 53 48 83 EC 30 48 8B D9 84 D2 74 68 80 3D ? ? ? ? ? 72 2C 48 8B 05 ? ? ? ? 4C 8D 44 24 ? 48 89 44 24 ? 41 B9 ? ? ? ? 48 8D 05 ? ? ? ? 33 D2 33 C9");
        WelcomePlayerSig = ("48 8B C4 55 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 48 89 58 18 48 8B 59 30 48 89 78 E8 48 8B FA 4C 89 68 D8 48 8B D1 4C 89 78 C8 45 33 ED 4C 8B F9 4C 89 6C 24 ?");
        NetDebugSig = ("40 55 57 41 56 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 8B 01 48 8B F9 FF 90 ? ? ? ? 4C 8B F0 48 85 C0 0F 84 ? ? ? ? 48 83 78 ? ? 0F 84 ? ? ? ? 48 8B 07 48 8D 0D ? ? ? ?");
        SetReplicationDriverSig = ("40 55 41 57 48 83 EC 28 48 8B E9 4C 8B FA 48 8B 89 ? ? ? ? 48 85 C9 0F 84 ? ? ? ? 48 8B 01 48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 4C 89 64 24 ? 4C 89 74 24 ? FF 90 ? ? ? ? 48 63");
        ValidationFailureSig = ("40 55 53 56 41 54 41 55 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 33 F6 48 8B DA 89 B5 ? ? ? ? 4C 8B E1 E8 ? ? ? ? 49 8B 04 24 49 8B CC FF 90 ? ? ? ? 48 8B 0D ? ? ? ? 48 8B D0 4C 8B E8 E8 ? ? ?");
        InitListenSig = ("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 50 48 8B BC 24 ? ? ? ? 49 8B F0");
        CreateNetDriverSig = ("48 89 5C 24 ? 57 48 83 EC 20 49 8B D8 48 8B F9 E8 ? ? ? ? 48 8B D0 4C 8B C3 48 8B CF 48 8B 5C 24 ? 48 83 C4 20 5F E9 ? ? ? ?");
        SetWorldSig = ("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 48 8B 99 ? ? ? ? 48 8B F2 48 8B F9 48 85 DB 0F 84 ? ? ? ? 48 8B 97 ? ? ? ? 48 8D 8B ? ? ? ? E8 ? ? ? ? 48 8B 97");
        StaticFindObjectSig = ("48 89 5C 24 ? 48 89 74 24 ? 55 57 41 54 41 56 41 57 48 8B EC 48 83 EC 60 80 3D ? ? ? ? ? 45 0F B6 F1 49 8B F8 48 8B DA 4C 8B F9 74 4E 48 8B 05 ? ? ? ? 48 8D 35 ? ? ? ? 41 B9 ? ? ? ?");
        GetPlayerViewpointSig = ("48 89 5C 24 ? 55 56 41 56 48 8B EC 48 83 EC 40 48 8B F2 48 8B D9 BA ? ? ? ? 48 8D 4D 38 4D 8B F0 E8 ? ? ? ? 48 8B CB 48 8B 10 E8 ? ? ? ? 84 C0 74 53");
        CreateNamedNetDriverSig = ("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 49 8B D9 49 8B F8 48 8B F1 E8 ? ? ? ? 48 8B D0 4C 8B CB 4C 8B C7 48 8B CE 48 8B 5C 24 ? 48 8B 74 24 ? 48 83 C4 20 5F E9 ? ? ? ?");
        MarkAbilitySpecDirtySig = ("48 89 5C 24 ? 48 89 7C 24 ? 41 56 48 83 EC 20 48 8B 01 41 0F B6 D8 4C 8B F2 48 8B F9 FF 90 ? ? ? ? 84 C0 0F 84 ? ? ? ? 49 8B 46 10 48 89 6C 24 ? 48 89");
        GiveAbilitySig = ("48 89 5C 24 ? 48 89 6C 24 ? 48 89 7C 24 ? 41 56 48 83 EC 20 83 B9 ? ? ? ? ? 49 8B E8 4C 8B F2 48 8B F9 7E 51 48 63 9F ? ? ? ? 48 81 C7 ? ? ? ? 8D");
        InternalTryActivateAbilitySig = ("4C 89 4C 24 ? 4C 89 44 24 ? 89 54 24 10 55 53 56 57 41 54 41 55 41 56 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 8B DA 48 8B F1 E8 ? ? ? ? 4C 8D B6 ? ? ? ? 33 D2 49 8B CE 4C 8D A0 ? ? ? ? E8 ? ? ? ?");

        /*

        ReplicateActorSig = ("48 8B C4 48 89 48 08 55 53 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 48 89 70 E8 33 F6 40 38 35 ? ? ? ? 48 89 78 E0 4C 89 60 D8 4C 8B E1 4C 89 70 C8 48");
        SetChannelActorSig = ("48 89 5C 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 45 33 ED 4C 8D 35 ? ? ? ? 44 89 AD ? ? ? ? 48 8B F9 48 8B 41 28 45 8B E0");
        CreateChannelByNameSig = ("40 55 53 56 57 41 54 41 56 41 57 48 8B EC 48 83 EC 60 48 8B 01 41 8B D9 45 8B F8 4C 8B E2 48 8B F9 FF 90 ? ? ? ? 33 F6");
        CallPreReplicationSig = ("48 85 D2 0F 84 ? ? ? ? 53 55 48 83 EC 38 48 8B EA 48 89 74 24 ? 48 89 7C 24 ? 48 8D 54 24 ? 48 8B D9 4C 89 74 24 ? 4C 8B C1 4C 89 7C 24 ? 48 8B CD E8 ? ? ? ?");
        
        */
        
        ValidENameSig = ("48 89 5C 24 ? 57 48 83 EC 20 80 3D ? ? ? ? ? 48 8B D9 48 8D 0D ? ? ? ? 48 63 FA 75 0F E8 ? ? ? ? 48 8B C8 C6 05 ? ? ? ? ? 8B 8C B9 ? ? ? ? 48 8B C3 89 0B C7 43 ? ? ? ? ? 48 8B 5C 24 ? 48 83 C4 20 5F C3");
        ReplicationGraph_EnableSig = ("48 89 5C 24 ? 48 89 74 24 ? 55 57 41 56 48 8B EC 48 83 EC 70 8B 99 ? ? ? ? 48 8B F2 4C 8B F1 BA ? ? ? ? 8B 89 ? ? ? ? E8 ? ? ? ? 85 DB 41");
    }

    else if (Engine_Version >= 425)
    {
        TickFlushSig = ("4C 8B DC 55 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 49 89 5B 18 49 89 73 F0 49 89 7B E8 48 8B F9");
        World_NotifyControlMessageSig = ("48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 48 89 4C 24 ? 55 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 33 FF 45 0F B6 E0 89 7C 24 74");
        SpawnPlayActorSig = ("48 8B C4 44 89 40 18 48 89 50 10 48 89 48 08 55 57 48 8D 68 B9 48 81 EC ? ? ? ? 48 89 70 E0 33 FF 4C 89 60 D8 49 8B F1 4C 89 68 D0");
        ReceiveUniqueIdReplSig = ("48 89 5C 24 ? 55 56 57 48 8B EC 48 83 EC 40 80 79 28 00 48 8B FA 48 8B D9 0F 8D ? ? ? ? F6 41 2B 08 0F 85 ? ? ? ? 48 8B 4A 08 33 F6 48 85 C9");
        ReceiveFStringSig = ("48 89 5C 24 ? 55 56 57 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 00 F6 41 28 01 4C 8B F2 48 8B F9 0F 84 ? ? ? ? 48 8B 49 08");
        KickPlayerSig = ("48 8B C4 53 48 83 EC 70 48 89 68 08 48 8B EA 48 89 70 10 4C 89 60 20 45 33 E4 4C 89 70 F0 4D 8B F0 4C 89 78 E8 4C 8B F9 48 8D 48 C8"); // NOTE: This is a different kickplayer than the other versions.
        LocalPlayerSpawnPlayActorSig = ("48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 55 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B D9 4D 8B F1 49 8B C9 4D 8B F8 48 8B F2");
        Beacon_NotifyControlMessageSig = ("4C 8B DC 49 89 5B 18 49 89 73 20 49 89 4B 08 55 57 41 54 41 55 41 57 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 33 F6 49 8B F9");
        InitHostSig = ("48 8B C4 48 81 EC ? ? ? ? 48 89 58 10 4C 8D 05 ? ? ? ? 48 8B D9 48 89 78 F8 48 8D 48 88");
        PauseBeaconRequestsSig = ("40 53 48 83 EC 30 48 8B D9 84 D2 74 5E 80 3D ? ? ? ? ? 72 22 48 8D 05 ? ? ? ? 41 B9 ? ? ? ? 4C 8D 05 ? ? ? ? 48 89 44 24");
        WelcomePlayerSig = ("48 8B C4 55 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 48 89 58 18 48 8B 59 30 48 89 78 E8 48 8B FA 4C 89 68 D8 48 8B D1 4C 89 78 C8 45 33 ED 4C 8B F9 4C 89 6C 24 ?");
        NetDebugSig = ("40 55 56 41 56 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 8B 01");
        FixCrashSig = ("40 55 56 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 48 8D 6C 24 ? 48 89 9D ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C5 48 89 85 ? ? ? ? 8B 41 0C 45 33 F6");
        SetReplicationDriverSig = ("40 56 41 56 48 83 EC 28 48 8B F1 4C 8B F2 48 8B 89 ? ? ? ? 48 85 C9 0F 84 ? ? ? ? 48 8B 01 48 89 5C 24 ? 48 89 6C 24 ? 4C 89 7C 24 ? FF 90 ? ? ? ? 48 8B 9E ? ? ? ? 45 33 FF");
        ValidationFailureSig = ("48 89 4C 24 ? 55 53 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 33 DB 48 8B F2");
        CreateNetDriver_LocalSig = ("4C 89 44 24 ? 53 56 57 41 56 41 57 48 83 EC 70 48 8B B9 ? ? ? ? 48 8D 1D ? ? ? ? 48 63 81 ? ? ? ? 45 33 FF 4C 8B F2 48 8D 0C 40 4C 8D 0C CF 49 3B F9 74 22 44 8B 94 24 ? ? ? ? 44 39 57 04 0F 94 C1 44 39 07 0F 94 C0");
        CreateNetDriverSig = ("48 89 5C 24 08 57 48 83 EC ? 49 8B D8 48 8B F9 E8 ? ? ? ? 48 8B D0");
        SetWorldSig = ("48 89 5C 24 ? 48 89 74 24 ? 55 57 41 56 48 8B EC 48 83 EC 30 48 8B 99 ? ? ? ? 45 33 F6 48 8B F2 48 8B F9 48 85 DB 0F 84 ? ? ? ? 48 8B 97 ? ? ? ? 48 8D 8B ? ? ? ? E8 ? ? ? ? 48 8B 97 ? ? ? ? 48 8D 8B ? ? ? ? E8 ? ? ? ? 48 8B 97 ? ? ? ?");
        StaticFindObjectSig = ("48 89 5C 24 ? 48 89 74 24 ? 55 57 41 54 41 56 41 57 48 8B EC 48 83 EC 60 80 3D ? ? ? ? ? 45 0F B6 F1 49 8B F8 48 8B DA 4C 8B F9 74 2F 48 8D 05 ? ? ? ? 41 B9 ? ? ? ? 4C 8D 05 ? ? ? ? 48 89 44 24 ? BA ? ? ? ? 48 8D 0D");
        GetPlayerViewpointSig = ("40 55 56 57 41 57 48 8B EC 48 83 EC 48 48 8B 81 ? ? ? ? 4D 8B F8 48 8B F2 48 8B F9 48 85 C0 0F 84 ? ? ? ? F6 80 ? ? ? ? ? 0F 84 ? ? ? ? 48 8B 88 ? ? ? ? 48 85 C9 74 23 0F 10 89 ? ? ? ? 0F 28 C1 F3 0F 11 4D ? 0F C6 C1 55 0F C6 C9 AA");
        StaticLoadObjectSig = "40 55 53 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 8B 85 ? ? ? ? 4C 8B FA 89 45 88 4C 8B E9 48 8B 85 ? ? ? ? 48 8D";
        PlayMontageSig = "40 55 56 41 54 41 56 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B F1 0F 29 7C 24 ? 48 8B 89 ? ? ? ? 4D 8B F1 F3 0F 10 3D ? ? ? ? 4C 8B E2 48 85 C9 0F 84 ? ? ? ? 4C 89 BC 24 ? ? ? ? E8 ? ? ? ? 4C 8B F8 48 85 C0";
        CanActivateAbilitySig = "48 89 5C 24 ? 4C 89 4C 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 49 8B F0";
    }

    if (Engine_Version >= 426)
    {
        TickFlushSig = ("4C 8B DC 55 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 49 89 5B 18 49 89 73 F0 48 8B F1 49 89 7B E8 48 8D 0D ? ? ? ? 4D 89 7B C8");
        World_NotifyControlMessageSig = ("40 55 53 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 33 FF 45 0F B6 E0");
        SpawnPlayActorSig = ("48 8B C4 44 89 40 18 48 89 50 10 48 89 48 08 55 57 48 8D 68 B9 48 81 EC ? ? ? ? 48 89 70 E0 33 FF 4C 89 60 D8 49 8B F1 4C 89 68 D0 4C 8D 25 ? ? ? ? 4C 89 70 C8 45 8B E8 4C");
        // LocalPlayerSpawnPlayActorSig = ("48 8B C4 48 89 58 20 44 89 40 18 48 89 50 10 55 56 57 41 54 41 55 41 56 41 57 48 8D 68 B9 48 81 EC ? ? ? ? 33 DB 4C 8D 35 ? ? ? ? 89 5D 4F 49 8B F9 48 8B 75 77 4C 8B E9 4C 39 36 74 24 8B 46 0C 8B CB 89 5E 08 85 C0 74 10 33 D2 48 8B CE");
        InitHostSig = ("48 8B C4 48 81 EC ? ? ? ? 48 89 58 10 4C 8D 05 ? ? ? ?");
        InitListenSig = ("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 50 48 8B BC 24 ? ? ? ? 49 8B F0 48 8B 01 48 8B D9 48 89 7C 24 ? 44 88 4C 24 ? 4D 8B C8 4C 8B C2 33 D2 FF 90 ? ? ? ? 84 C0 0F 85 ? ? ? ? 80 3D ? ? ? ? ? 72 69");
        GiveAbilitySig = ("48 89 5C 24 ? 48 89 6C 24 ? 48 89 7C 24 ? 41 56 48 83 EC 20 8B 81 ? ? ? ? 49 8B E8 4C 8B F2 48 8B F9 85 C0 7E 54 48 63 9F ? ? ? ? 48");
        InternalTryActivateAbilitySig = ("4C 89 4C 24 ? 4C 89 44 24 ? 89 54 24 10 55 53 56 57 41 54 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 8B DA 4C 8B F1 E8 ? ? ? ? 4D");
        MarkAbilitySpecDirtySig = ("48 89 5C 24 ? 48 89 7C 24 ? 41 56 48 83 EC 20 48 8B 01 41 0F B6 F8 4C 8B F2 48 8B D9 FF 90 ? ? ? ? 84 C0 0F 84 ? ? ? ? 49");
        GetPlayerViewpointSig = ("48 89 5C 24 ? 48 89 74 24 ? 55 41 56 41 57 48 8B EC 48 83 EC 40 48 8B F2");
        CreateNetDriverSig = ("48 89 5C 24 ? 57 48 83 EC 20 49 8B D8 48 8B F9 E8 ? ? ? ? 48 8B D0 4C 8B C3 48 8B CF 48 8B 5C 24 ? 48 83 C4 20 5F E9 ? ? ? ?");
        PauseBeaconRequestsSig = ("40 57 48 83 EC 30 48 8B F9 84 D2 74 62 80 3D ? ? ? ? ? 72 22 48 8D 05 ? ? ? ? 41 B9 ? ? ? ? 4C 8D 05 ? ? ? ? 48 89 44 24 ? 33 D2");
        Beacon_NotifyControlMessageSig = ("4C 8B DC 49 89 5B 18 49 89 73 20 55 57 41 54 41 55 41 57 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 33 F6 49 8B D9 89 74 24 44 41 0F B6 F8 48 8B 41 10");
        CollectGarbageSig = "40 55 57 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 80 3D ? ? ? ? ? 0F B6 FA 44 8B F9 74 3B 80 3D ? ? ? ? ? 0F 82 ? ? ? ? 48 8D";
    }

    if (FnVerDouble >= 16.00)
    {
        TickFlushSig = ("48 8B C4 48 89 58 18 55 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 70 B8 0F 29 78 A8 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 8B F9 48 89 4D 38 48");
        World_NotifyControlMessageSig = "48 8B C4 48 89 58 10 48 89 70 18 48 89 78 20 48 89 48 08 55 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 45 33 ED 45 0F B6 F0 44 89";
        ReceiveFStringSig = "48 89 5C 24 ? 55 56 57 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 00 F6 41 28 01 48 8B F2 48 8B F9 0F 84";
        ReceiveUniqueIdReplSig = "40 55 53 57 48 8B EC 48 83 EC 40 80 79 28 00 48 8B FA 48 8B D9 7C 1A 4C 8D 4D 20 48 8B D3 48 8B CF E8 ? ? ? ? 48 8B C3 48 83 C4 40 5F 5B 5D C3 F6 41 2B 08 75 E0 48 8B CF E8 ? ? ? ? 84 C0 0F 85 ? ? ? ?";
        WelcomePlayerSig = "48 89 5C 24 ? 48 89 74 24 ? 55 57 41 56 48 8B EC 48 83 EC 60 48 8B 71 30";
        SpawnPlayActorSig = "48 8B C4 48 89 58 20 44 89 40 18 48 89 50 10 55 56 57 41 54 41 55 41 56 41 57 48 8D 68 B9 48 81 EC ? ? ? ? 33 DB 4C 8D 35 ? ? ? ? 89 5D";
        NetDebugSig = "48 8B C4 48 89 58 10 48 89 70 18 48 89 78 20 55 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 8B 01 48 8B F9 FF 90 ? ? ? ? 45 33 E4 48 8B F0 48 85 C0 0F 84 ? ? ? ? 4C 39 60 38 0F 84 ? ? ? ? 44 38";
        GetPlayerViewpointSig = "48 89 5C 24 ? 48 89 74 24 ? 55 57 41 56 48 8B EC 48 83 EC 40 49 8B F0 4C 8B F2 48 8B D9 E8 ? ? ? ? 83 65 24 00 44 8B 88 ? ? ? ? 44 89 4D 20 48 8B 45 20 48 39 83 ? ? ? ? 75 1D 80 BB ? ? ? ? ? 75 14 48 8B 03 48 8B CB";
        PauseBeaconRequestsSig = "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 30 33 F6 48 8B F9 84 D2 74 42 80 3D ? ? ? ? ? 72 20 48 8D 05 ? ? ? ? 33 D2 44 8D 4E 06 48 89 44 24 ? 4C 8D 05";
        InitHostSig = "48 89 5C 24 ? 48 89 74 24 ? 55 57 41 56 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B F1 4C 8D 05 ? ? ? ? 48 8D 4D D7 45 33 C9 33 D2 E8 ? ? ? ? E8 ? ? ? ? 48 8B C8 4C 8D 45 67 48 8D 15 ? ? ? ? E8 ? ? ? ? 33 FF 84 C0 74 0D 8B 45 67 85 C0 74 06 89 86 ? ? ? ?";
        Beacon_NotifyControlMessageSig = "48 8B C4 48 89 58 10 48 89 70 18 48 89 78 20 48 89 48 08 55 41 54 41 55 41 56 41 57 48 8D 68 88 48 81 EC ? ? ? ? 45 33 E4 49 8B D9 44 89 64 24 ? 41 8A F0 48 8B 41 10 48 8B";
        NoReserveSig = "48 8B C4 48 89 58 08 48 89 70 10 48 89 78 18 4C 89 60 20 55 41 56 41 57 48 8B EC 48 83 EC 60 49 8B D9 45 8A F8 4C 8B F2 48 8B F9 45 32 E4 E8 ? ? ? ? 48 8B";
        KickPlayerSig = "48 8B C4 48 89 58 08 48 89 70 10 48 89 78 18 4C 89 60 20 55 41 56 41 57 48 8B EC 48 83 EC 60 48 83 65 ? ? 4C 8B F2 83 65 E8 00 4C 8B E1 83 65 EC 00 48 8D 4D E0";
        GiveAbilitySig = "48 89 5C 24 ? 48 89 6C 24 ? 56 57 41 56 48 83 EC 20 8B 81 ? ? ? ? 49 8B E8 4C 8B F2 48 8B F9 85 C0 0F 8F ? ? ? ? FF C0 48 89 4C 24 ? 89 81 ? ? ? ? 48 8D B1 ? ? ? ? 48 63";
        InternalTryActivateAbilitySig = "4C 89 4C 24 ? 4C 89 44 24 ? 89 54 24 10 55 53 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 8B DA 48 8B F1 E8 ? ? ? ? 48 8D 8E ? ? ? ? 33 D2 48 8D B8";
        StaticFindObjectSig = "40 55 53 57 41 54 41 55 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 80 3D ? ? ? ? ? 45 0F B6 E1 49 8B F8 48 8B DA 4C 8B F9 0F 85 ? ? ? ? 45 33 ED";
    }

    if (FnVerDouble >= 17.00)
    {
        TickFlushSig = "48 8B C4 48 89 58 18 55 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 70 B8 0F 29 78 A8 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 8A 81 ? ? ? ? 48";
        GetPlayerViewpointSig = "48 8B C4 48 89 58 10 48 89 70 18 55 57 41 54 41 56 41 57 48 8D 68 A1 48 81 EC ? ? ? ? 0F 29 70 C8 45 33 E4 0F 29 78 B8 49 8B F0 44 0F 29 40 ? 48 8B FA 44 0F 29 48";
        Beacon_NotifyControlMessageSig = "48 8B C4 48 89 58 10 48 89 70 18 48 89 78 20 48 89 48 08 55 41 54 41 55 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 45 33 ED 4D 8B F1 44 89 6C 24 ? 41 8A F0 48 8B 41 10";
        GiveAbilitySig = "48 89 5C 24 ? 48 89 6C 24 ? 56 57 41 56 48 83 EC 20 8B 81 ? ? ? ? 49 8B E8 4C 8B F2 48 8B F9 85 C0 7E 54 48 63 9F ? ? ? ? 48 81 C7 ? ? ? ? 8D 43 01 89 87 ? ? ? ? 3B 87 ? ? ? ? 7E 0A 8B";
        InternalTryActivateAbilitySig = "48 8B C4 4C 89 48 20 4C 89 40 18 89 50 10 55 53 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 8B DA 4C 8B F1 E8 ? ? ? ? 49 8D 8E ? ? ? ? 33 D2 48 8D";
        MarkAbilitySpecDirtySig = "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 48 8B 01 41 8A F0 48 8B FA 48 8B D9 FF 90 ? ? ? ? 84 C0 74 3E 48 8B 47 10 48 85 C0 74 0E 80 B8 ? ? ? ? ? 75 05 40 84 F6 74 16 80 8B";
        StaticFindObjectSig = "40 55 53 57 41 54 41 55 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 80 3D ? ? ? ? ? 45 0F B6 E1 49 8B F8 48 8B DA 4C 8B F9";
    }

    if (FnVerDouble >= 18.00)
    {
        TickFlushSig = "48 8B C4 48 89 58 18 55 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 70 B8 0F 29 78 A8 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 44 0F B6 A1";
        InitListenSig = "4C 8B DC 49 89 5B 10 49 89 73 18 57 48 83 EC 50 48 8B BC 24 ? ? ? ? 49 8B F0 48 8B 01 48 8B D9 49 89 7B D0 45 88 4B C8 4D 8B C8 4C 8B C2 33 D2 FF 90 ? ? ? ? 84 C0";
        World_NotifyControlMessageSig = "48 8B C4 48 89 58 10 48 89 70 18 48 89 78 20 48 89 48 08 55 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 45 33 ED 45 0F B6 F0 44 89 6C 24 ? 49 8B D9";
        SpawnPlayActorSig = "48 89 5C 24 ? 44 89 44 24 ? 48 89 54 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 8B EC 48 81 EC ? ? ? ? 48 8B 5D 68 4C 8D 35 ? ? ? ? 33 FF 49 8B F1 4C 8B E9 89 7D 40 4C 39 33 74 24 8B 43 0C 8B CF";
        PauseBeaconRequestsSig = "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 30 33 F6 48 8B F9 84 D2 74 42 80 3D ? ? ? ? ? 72 20 48 8D 05 ? ? ? ? 33 D2 44 8D 4E 06 48 89 44 24 ? 4C 8D 05 ? ? ? ? 33 C9 E8 ? ? ? ? 48 8B 8F ? ? ?";
        InitHostSig = "48 8B C4 48 89 58 10 48 89 70 18 48 89 78 20 55 41 56 41 57 48 8D 68 A1 48 81 EC ? ? ? ? 48 8B F1 4C 8D 35 ? ? ? ? 4D 8B C6 48 8D 4D D7 45 33 C9 33 D2 E8 ? ? ? ? E8 ? ? ? ? 48 8B C8 4C 8D 45 67 48 8D 15 ? ? ? ? E8 ? ? ? ? 33 FF 84 C0 74 0D 8B 45 67 85 C0";
        Beacon_NotifyControlMessageSig = "48 89 5C 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 45 33 FF 41 0F B6 D8 4C 8D A9";
        ReceiveFStringSig = "48 89 5C 24 ? 55 56 57 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 00 F6 41 28 01 48 8B F2 48 8B F9 0F 84 ? ? ? ? 48 8B 49";
        ReceiveUniqueIdReplSig = "40 55 53 57 48 8B EC 48 83 EC 40 80 79 28 00 48 8B FA 48 8B D9 7C 1A 4C 8D 4D 20 48 8B D3 48 8B CF E8 ? ? ? ? 48 8B C3 48 83 C4 40 5F 5B 5D C3 F6 41 2B 08";
        KickPlayerSig = "48 8B C4 48 89 58 08 48 89 70 10 48 89 78 18 4C 89 60 20 55 41 56 41 57 48 8B EC 48 83 EC 60 48 83 65 ? ? 4C 8B F2 83 65 E8 00 4C 8B E1 83 65 EC 00 48 8D 4D E0 BA ? ? ? ? 4D 8B F8 E8 ? ? ? ?";
        GiveAbilitySig = "48 89 5C 24 ? 48 89 6C 24 ? 56 57 41 56 48 83 EC 20 8B 81 ? ? ? ? 49 8B E8 4C 8B F2 48 8B F9 85 C0 7E 54 48 63 9F ? ? ? ? 48 81 C7 ? ? ? ? 8D 43 01 89 87 ? ? ? ? 3B";
        MarkAbilitySpecDirtySig = "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 48 8B 01 41 8A F0 48 8B FA 48 8B D9 FF 90 ? ? ? ? 84 C0 74 3E 48 8B 47 10 48 85 C0 74 0E 80 B8 ? ? ? ? ? 75 05 40 84 F6 74 16 80 8B ? ? ? ? ?";
        InternalTryActivateAbilitySig = "4C 89 4C 24 ? 4C 89 44 24 ? 89 54 24 10 55 53 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 8B DA 48 8B F1 E8 ? ? ? ? 48 8D 8E";
        StaticFindObjectSig = "48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 55 41 54 41 55 41 56 41 57 48 8B EC 48 83 EC 60 45 33 ED 45 8A F9 44 38 2D ? ? ? ? 49 8B F8 48 8B F2 4C 8B E1 0F 85 ? ? ? ? 44";
    }

    if (FnVerDouble >= 19.00)
    {
        ReceiveUniqueIdReplSig = "40 55 53 57 48 8B EC 48 83 EC 40 80 79 28 00 48 8B FA 48 8B D9 7C 1A 4C 8D 4D 20 48 8B D3 48 8B CF E8 ? ? ? ? 48 8B C3 48 83 C4 40 5F 5B 5D C3 F6 41 2B 10 75 E0 48 8B CF";
        World_NotifyControlMessageSig = "48 8B C4 48 89 58 10 48 89 70 18 48 89 78 20 55 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 45 33 FF 41 0F B6 F8 44 89 BD ? ? ? ? 49 8B D9 48 8B 41 10 48 8B F2 4C 8B E9 45 8B E7 4C 39 B8 ? ? ? ? 0F";
        SpawnPlayActorSig = "48 89 5C 24 ? 44 89 44 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 4C 8B 75 77 48 8D 3D ? ? ? ? 4C 8B EA 4C 8B F9 33 DB 48";
        PauseBeaconRequestsSig = "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC 20 33 ED 48 8B F1 84 D2 74 27 80 3D ? ? ? ? ? 72 05 E8 ? ? ? ? 48 8B 8E ? ? ? ? 33 D2 48 8B 01";
        InitHostSig = "48 8B C4 48 89 58 10 48 89 70 18 48 89 78 20 55 41 56 41 57 48 8D 68 A1 48 81 EC ? ? ? ? 4C 8B F1 48 8D 3D ? ? ? ? 4C 8B C7 48 8D 4D D7 45 33 C9 33 D2 E8 ? ? ? ? E8";
        Beacon_NotifyControlMessageSig = "48 89 5C 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 45 33 ED 41 0F B6 D8 4C 8D B9 ? ? ? ? 44";
        ReceiveFStringSig = "48 89 5C 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 17 41 BD ? ? ? ? 4C 8B F2 48 8B F1 44 84 69 28 0F 84 ? ? ? ? 48 8B 49";
        TickFlushSig = "48 8B C4 48 89 58 18 55 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 70 B8 0F 29 78 A8 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 44 8A A1 ? ? ? ? 48 8D 15 ? ? ? ? 48 8B F9 44 88 64 24 ?";
        KickPlayerSig = "48 89 5C 24 ? 48 89 74 24 ? 55 57 41 56 48 8B EC 48 83 EC 50 48 8B DA 4C 8B F1 48 8D 15 ? ? ? ? 49 8B F0 48 8D 4D E0 E8 ? ? ? ? 48 8B 8B";
        
        GiveAbilitySig = "48 89 5C 24 ? 48 89 6C 24 ? 56 57 41 56 48 83 EC 20 8B 81 ? ? ? ? 49 8B E8 4C 8B F2 48 8B F9 85 C0 0F 8F ? ? ? ? FF C0";
        InternalTryActivateAbilitySig = "48 89 5C 24 ? 48 89 6C 24 ? 56 57 41 56 48 83 EC 20 8B 81 ? ? ? ? 49 8B E8 4C 8B F2 48 8B F9 85 C0 0F 8F ? ? ? ? FF C0 48 89 4C 24 ? 89 81 ? ? ? ? 48 8D B1 ? ? ? ?";
    }

    if (FnVerDouble >= 19.10)
    {
        KickPlayerSig = "48 89 5C 24 ? 48 89 7C 24 ? 55 41 56 41 57 48 8B EC 48 83 EC 50 48 8B DA 4C 8B F9 48 8D 15 ? ? ? ? 4D 8B F0 48 8D 4D E0 E8 ? ? ? ? 48 8B 8B ? ? ? ? 48 85 C9 74 2A 48 81 C1 ? ? ? ? 48 8D 55 F0 E8";
        World_NotifyControlMessageSig = "48 8B C4 48 89 58 10 48 89 70 18 48 89 78 20 48 89 48 08 55 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 45 33 FF 41 0F B6 F8 44 89 7C 24 ? 49 8B D9 48 8B 41 10 4C 8B F2 4C 8B E9";
        SpawnPlayActorSig = "44 89 44 24 ? 48 89 54 24 ? 55 53 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 4C 8B 7D 77 48 8D 3D ? ? ? ? 4C 8B F1 33 DB";
        Beacon_NotifyControlMessageSig = "48 89 5C 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 45 33 E4 41 0F B6 D8 4C";
    }

    if (FnVerDouble >= 20.00)
    {
        ReceiveUniqueIdReplSig = "40 55 53 57 48 8B EC 48 83 EC 30 80 79 28 00 48 8B FA 48 8B D9 7C 1A 4C 8D 4D 20 48 8B D3 48 8B CF E8 ? ? ? ? 48 8B C3 48 83 C4 30 5F 5B 5D C3 F6 41 2B 10";
        WelcomePlayerSig = "48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 55 41 56 41 57 48 8B EC 48 83 EC 70 48 8B 71 30 48 8B DA 45 33 FF 48 8B D1 4C 89 7D B0 48 8B F9 4C 89 7D B8 E8 ? ? ? ? 44 38 B8";
        World_NotifyControlMessageSig = "48 8B C4 48 89 58 10 48 89 70 18 48 89 78 20 48 89 48 08 55 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 45 33 ED 45 0F B6 F0 44 89 6C 24 ? 49 8B F1 48 8B 41 10";
        Beacon_NotifyControlMessageSig = "48 89 5C 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 45 33 E4 41 0F B6 D8 4C 8D";
        KickPlayerSig = "48 89 5C 24 ? 48 89 74 24 ? 55 57 41 54 41 56 41 57 48 8B EC 48 83 EC 50 48 83 65 ? ? 4C 8B F2 83 65 E8 00 4C 8B E1 83 65 EC 00 48 8D";
    }

    static auto ReallocAddr = FindPattern(ReallocSig);
    // FMemory::Realloc = decltype(FMemory::Realloc)(ReallocAddr); // we don't need this I think
    CheckPattern(("FMemory::Realloc"), ReallocAddr, &FMemory::Realloc);

    static auto StaticFindObjectAddr = FindPattern(StaticFindObjectSig);

    if (!StaticFindObjectAddr)
    {
        if (!StaticFindObjectAddr)
        {
            StaticFindObjectAddr = FindPattern(("48 89 5C 24 ? 48 89 74 24 ? 55 57 41 54 41 56 41 57 48 8B EC 48 83 EC 60 80 3D ? ? ? ? ? 45 0F B6 F1 49 8B F8 48 8B DA 4C 8B F9 74 52 48 8B 05")); // S7

            if (!StaticFindObjectAddr)
                StaticFindObjectAddr = FindPattern("4C 8B DC 49 89 5B 08 49 89 6B 18 49 89 73 20 57 41 56 41 57 48 83 EC 60 80 3D ? ? ? ? ? 41 0F B6 E9 49 8B F8 48 8B DA 4C 8B F1 74 51 48 8B 05 ? ? ? ? 4D");

            if (!StaticFindObjectAddr)
                std::cout << ("[WARNING] You will not be able to utilize the fast speeds of StaticFindObject!\n");
        }
    }

    StaticFindObjectO = decltype(StaticFindObjectO)(StaticFindObjectAddr);

    if (Engine_Version < 424)
    {
        GetNetModeAddr = FindPattern(GetNetModeSig);

        if (!GetNetModeAddr)
            GetNetModeAddr = FindPattern(("48 89 5C 24 ? 57 48 83 EC 20 48 8B 01 48 8B D9 FF 90 ? ? ? ? 4C 8B 83 ? ? ? ? 48 8B F8 49 8B C8 48 C1 E9 20 85 C9 0F 94 C2 41 81 F8 ? ? ? ?"));

        if (!GetNetModeAddr)
            std::cout << ("[WARNING] Pawns will probably get desynced!\n");

        // CheckPattern(("GetNetMode"), GetNetModeAddr, &GetNetMode);
    }

    LP_SpawnPlayActorAddr = FindPattern(LocalPlayerSpawnPlayActorSig);

    if (!LP_SpawnPlayActorAddr)
        std::cout << ("[WARNING] Will not be able to prevent the creation of the player controller! Must be logged in to Fortnite to host.\n");

    // CheckPattern(("LocalPlayer::SpawnPlayActor"), LP_SpawnPlayActorAddr, &LP_SpawnPlayActor);

    CollectGarbageAddr = FindPattern(CollectGarbageSig);

    if (!CollectGarbageAddr)
        CollectGarbageAddr = FindPattern("48 89 5C 24 ? 55 56 57 48 81 EC ? ? ? ? 0F B6 FA 0F 29 74 24 ? 8B E9 48 8D 15 ? ? ? ? 33 C9 E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 4C 8B 05 ? ? ? ? 33 F6 8B C6");

    if (!CollectGarbageAddr)
        std::cout << ("[WARNING] Could not find CollectGarbage! Probably going to crash down the line.\n");
    else
        CollectGarbage = decltype(CollectGarbage)(CollectGarbageAddr);

    InitListenAddr = FindPattern(InitListenSig);

    if (!InitListenAddr)
        InitListenAddr = FindPattern(("4C 8B DC 49 89 5B 08 49 89 73 10 57 48 83 EC 50 48 8B BC 24 ? ? ? ? 49 8B F0"));

    CheckPattern(("InitListen"), InitListenAddr, &InitListen);

    SendClientAdjustment = decltype(SendClientAdjustment)(FindPattern(SendClientAdjustmentSig));

    NoReserveAddr = FindPattern(NoReserveSig);

    if (!NoReserveAddr)
    {
        NoReserveAddr = FindPattern("48 89 5C 24 ? 48 89 6C 24 ? 57 41 56 41 57 48 81 EC ? ? ? ? 48 8B 01 49 8B E9 45 0F B6 F8 4C 8B F2 48 8B F9 B3 01 FF 90 ? ? ? ? 48");

        if (!NoReserveAddr && Engine_Version >= 424)
            std::cout << ("[WARNING] Unable to find No Reserve! Players will probably get kicked on join!\n");
    }
    
    NoReserve = decltype(NoReserve)(NoReserveAddr);

    TickFlushAddr = FindPattern(TickFlushSig);

    if (!TickFlushAddr)
        TickFlushAddr = FindPattern(("4C 8B DC 55 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 49 89 5B 18 48 8D 05 ? ? ? ? 49 89 73 F0 33 F6 40 38 35 ? ? ? ? 49 89 7B E8 48 8B F9 48 0F 45 C6"));

    if (!TickFlushAddr)
        TickFlushAddr = FindPattern(("4C 8B DC 55 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 49 89 5B 18 49 89 73 F0 48 8B F1 49 89 7B E8 48 8D 0D ? ? ? ? 4D 89 73 D0"));

    if (!TickFlushAddr)
        TickFlushAddr = FindPattern("4C 8B DC 55 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 45 0F 29 43 ? 45 0F 29 4B ? 48 8B 05 ? ? ? ? 48 33 C4 48 89"); // 1.11-2.5

    if (!TickFlushAddr && ((Engine_Version > 424 && FnVerDouble < 18.00)))
    {
        Finder::Functions::GetTickFlush(35, &TickFlushAddr);
    }

    CheckPattern(("TickFlush"), TickFlushAddr, &TickFlush);

    if (Engine_Version >= 420)
    {
         ReceiveFStringAddr = FindPattern(ReceiveFStringSig);

        if (!ReceiveFStringAddr)
            ReceiveFStringAddr = FindPattern(("48 89 5C 24 ? 55 56 57 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 27 F6 41 28 01 48 8B F2 48 8B F9 0F 84 ? ? ? ? 48 8B 49"));

        if (!ReceiveFStringAddr)
            ReceiveFStringAddr = FindPattern("48 89 5C 24 ? 55 56 57 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 27 0F B6 41 28 48 8B F2 48 8B F9 A8 01 0F 84 ? ? ? ? 48 8B 49 08 48 8B 11 48");

        CheckPattern(("ReceiveFString"), ReceiveFStringAddr, &ReceiveFString); 

        ReceiveUniqueIdReplAddr = FindPattern(ReceiveUniqueIdReplSig);

        if (!ReceiveUniqueIdReplAddr)
            ReceiveUniqueIdReplAddr = FindPattern(("48 89 5C 24 ? 55 56 57 48 8B EC 48 83 EC 40 F6 41 28 40 48 8B FA 48 8B D9"));

        if (!ReceiveUniqueIdReplAddr)
            ReceiveUniqueIdReplAddr = FindPattern("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 50 48 8B F9 33 F6 48 8B 4A 08 48 8B DA 48 85 C9 74 1A 48 8B 01 FF 50 18"); // 2.5

        CheckPattern(("ReceiveUniqueIdRepl"), ReceiveUniqueIdReplAddr, &ReceiveUniqueIdRepl);

        WelcomePlayerAddr = FindPattern(WelcomePlayerSig);
    }

    CanActivateAbilityAddr = FindPattern(CanActivateAbilitySig);

    if (!CanActivateAbilityAddr)
    {
        CanActivateAbilityAddr = FindPattern(("48 89 5C 24 ? 4C 89 4C 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 49 8B F0 8B DA 48 8B F9 4D 85 C0 0F 84 ? ? ? ? 49 8D 48 10 E8 ? ? ? ? 48 85"));

        if (!CanActivateAbilityAddr)
            CanActivateAbilityAddr = FindPattern("48 89 5C 24 ? 4C 89 4C 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 49 8B F0 8B DA");

        if (!CanActivateAbilityAddr && Engine_Version > 420)
            std::cout << ("[WARNING] Abilities may fail!\n");
    }
    
    o_CanActivateAbility = decltype(o_CanActivateAbility)(CanActivateAbilityAddr);

    if (!WelcomePlayerAddr) // s6
    {
        WelcomePlayerAddr = FindPattern(("48 8B C4 55 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 48 89 70 20 48 8B F1 48 89 78 F0 48 8B CA 4C 89 70 E8 48 8B FA"));

        if (!WelcomePlayerAddr)
            WelcomePlayerAddr = FindPattern(("48 8B C4 55 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 89 70 20 48 8B F1 48 89 78 F0 48 8B CA 4C 89 70 E8 48 8B FA E8 ? ? ? ? 48 8B 8E ? ? ? ? E8 ? ? ? ? 48 8D 54 24 ? 48 8D 8D ? ? ? ? 4C 8B 40 18"));
    }

    CheckPattern(("WelcomePlayer"), WelcomePlayerAddr, &WelcomePlayer);

    World_NotifyControlMessageAddr = FindPattern(World_NotifyControlMessageSig);

    if (!World_NotifyControlMessageAddr)
        World_NotifyControlMessageAddr = FindPattern(("40 55 53 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 45 33 F6 49 8B D9"));

    if (!World_NotifyControlMessageAddr)
        World_NotifyControlMessageAddr = FindPattern(("48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 48 89 4C 24 ? 55 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 33 FF 49 8B D9 89 7C 24 60"));

    CheckPattern(("World_NotifyControlMessage"), World_NotifyControlMessageAddr, &World_NotifyControlMessage);

    SpawnPlayActorAddr = FindPattern(SpawnPlayActorSig);

    if (!SpawnPlayActorAddr)
        SpawnPlayActorAddr = FindPattern(("48 8B C4 44 89 40 18 48 89 50 10 48 89 48 08 55 56 48 8D 68 B9 48 81 EC ? ? ? ?"));

    CheckPattern(("SpawnPlayActor"), SpawnPlayActorAddr, &SpawnPlayActor);

    NetDebugAddr = FindPattern(NetDebugSig);

    if (Engine_Version >= 419 && FnVerDouble < 17.00)
        CheckPattern(("NetDebug"), NetDebugAddr, &NetDebug);

    // if (Engine_Version >= 420) // && FnVerDouble < 18.00)
    {
        GiveAbilityAddr = FindPattern(GiveAbilitySig);

        if (!GiveAbilityAddr)
        {
            GiveAbilityAddr = FindPattern(("48 89 5C 24 10 48 89 6C 24 18 48 89 7C 24 20 41 56 48 83 EC ? 83 B9 60 05"));

            if (!GiveAbilityAddr)
            {
                GiveAbilityAddr = FindPattern(("48 89 5C 24 ? 48 89 6C 24 ? 48 89 7C 24 ? 41 56 48 83 EC 20 83 B9 ? ? ? ? ? 49 8B E8 4C 8B F2 48 8B F9 7E 51 48 63 9F ? ? ? ? 48 81 C7 ? ? ? ? 8D 43 01"));

                if (!GiveAbilityAddr)
                    GiveAbilityAddr = FindPattern(("48 89 5C 24 ? 48 89 6C 24 ? 48 89 7C 24 ? 41 56 48 83 EC 20 83 B9 ? ? ? ? ? 49 8B E8 4C 8B F2 48 8B F9 7E 52 48 63 9F"));
            }
        }

        if (!GiveAbilityAddr)
        {
            std::cout << ("[WARNING] Failed to find GiveAbility, abilities will be disabled!\n");
        }
        // else if (FnVerDouble < 19.00)
        else if (Engine_Version >= 417)
        {
            if (Engine_Version < 426 && Engine_Version >= 420)
                CheckPattern(("GiveAbility"), GiveAbilityAddr, &GiveAbility);
            else if (Engine_Version < 420)
                CheckPattern("GiveAbility", GiveAbilityAddr, &GiveAbilityOLDDD);
            else if (std::floor(FnVerDouble) == 14 || std::floor(FnVerDouble) == 15)
                CheckPattern(("GiveAbility"), GiveAbilityAddr, &GiveAbilityS14ANDS15);
            else if (std::floor(FnVerDouble) == 16)
                CheckPattern(("GiveAbility"), GiveAbilityAddr, &GiveAbilityS16);
            else if (Engine_Version == 426)
                CheckPattern(("GiveAbility"), GiveAbilityAddr, &GiveAbilityFTS);
            else
                CheckPattern(("GiveAbility"), GiveAbilityAddr, &GiveAbilityNewer);

            InternalTryActivateAbilityAddr = FindPattern(InternalTryActivateAbilitySig);
            
            if (!InternalTryActivateAbilityAddr)
                InternalTryActivateAbilityAddr = FindPattern(("4C 89 4C 24 ? 4C 89 44 24 ? 89 54 24 10 55 53 56 57 41 54 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 8B DA 4C 8B F1 E8 ? ? ? ? 4D 8D BE ? ? ? ? 33 D2 49"));

            if (!InternalTryActivateAbilityAddr)
                InternalTryActivateAbilityAddr = FindPattern("4C 89 4C 24 ? 4C 89 44 24 ? 89 54 24 10 55 53 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 45 33 FF 48 8D 05 ? ? ? ? 44 38 3D ? ? ? ? 8B"); // 5.10

            if (!InternalTryActivateAbilityAddr)
                InternalTryActivateAbilityAddr = FindPattern("4C 89 4C 24 ? 4C 89 44 24 ? 89 54 24 10 55 53 56 57 41 54 41 56 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 8B DA 48 8B F1 E8 ? ? ? ? 4C 8D B6 ? ? ? ? 33 D2 49 8B CE");

            if (Engine_Version < 426)
                CheckPattern(("InternalTryActivateAbility"), InternalTryActivateAbilityAddr, &InternalTryActivateAbility);
            else if (FnVerDouble < 17.00)
                CheckPattern(("InternalTryActivateAbility"), InternalTryActivateAbilityAddr, &InternalTryActivateAbilityFTS);
            else
                CheckPattern(("InternalTryActivateAbility"), InternalTryActivateAbilityAddr, &InternalTryActivateAbilityNewer);

            /* MarkAbilitySpecDirtyAddr = FindPattern(MarkAbilitySpecDirtySig);

            if (!MarkAbilitySpecDirtyAddr)
                MarkAbilitySpecDirtyAddr = FindPattern(("48 89 5C 24 ? 48 89 7C 24 ? 41 56 48 83 EC 20 48 8B 01 41 0F B6 D8 4C 8B F2 48 8B F9 FF 90 ? ? ? ? 84 C0 0F 84 ? ? ? ? 49 8B 46 10 48 89 6C 24 ? 48 89 74"));

            if (Engine_Version < 426)
                CheckPattern(("MarkAbilitySpecDirty"), MarkAbilitySpecDirtyAddr, &MarkAbilitySpecDirtyOld);
            else
                CheckPattern(("MarkAbilitySpecDirty"), MarkAbilitySpecDirtyAddr, &MarkAbilitySpecDirtyFTS); */
        }
    }

    if (Engine_Version >= 420)
    {
        GetPlayerViewpointAddr = FindPattern(GetPlayerViewpointSig);

        if (!GetPlayerViewpointAddr)
            GetPlayerViewpointAddr = FindPattern(("48 89 5C 24 ? 48 89 74 24 ? 55 41 56 41 57 48 8B EC 48 83 EC 40 48 8B F2 48 C7 45 ? ? ? ? ? 48 8B 55 38 4D 8B F0 48 8B D9 45 33 FF E8 ? ? ? ? 84 C0 74 4A 80 BB"));

        if (FnVerDouble < 17.00)
            CheckPattern(("GetPlayerViewPoint"), GetPlayerViewpointAddr, &GetPlayerViewPoint);
    }

    if (Engine_Version >= 421 && Engine_Version <= 423)
    { 
        if (FnVerDouble >= 5 && FnVerDouble < 7)
        {
            IdkfAddr = FindPattern(IdkfSig);
            CheckPattern(("Idkf"), IdkfAddr, &Idkf);

            SendChallengeAddr = FindPattern(SendChallengeSig);

            if (!SendChallengeAddr)
                SendChallengeAddr = FindPattern(("48 89 5C 24 ? 55 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B DA 48 85 D2 0F 84 ? ? ? ? 83 BA ? ? ? ? ? 0F 86 ? ? ? ? 48 83 7A ? ? 0F 84 ? ? ? ? 48 8D 4D 68 48 89 BC 24 ? ? ? ? FF 15 ? ? ? ? 44 8B 45 68 48 8D 15 ? ? ? ? 48 8D 4C 24 ? E8 ? ? ? ? 48 8D 44 24 ? 48 8D BB ? ? ? ? 48 3B F8 74 25 48 8B 0F 48 85 C9 74 05 E8 ? ? ? ? 48 8B 44 24 ? 48 89 07 8B 44 24 38 89 47 08 8B 44 24 3C 89 47 0C EB 0F 48 8B 4C 24 ? 48 85 C9 74 05 E8 ? ? ? ? B2 05 48 8B CB E8 ? ? ? ? 48 8B 83 ? ? ? ?"));

            CheckPattern(("SendChallenge"), SendChallengeAddr, &SendChallenge);
        } 

        if (Engine_Version == 423)
        {
            HasClientLoadedCurrentWorldAddr = FindPattern(HasClientLoadedCurrentWorldSig);
            CheckPattern(("HasClientLoadedCurrentWorld"), HasClientLoadedCurrentWorldAddr, &HasClientLoadedCurrentWorld);

            malformedAddr = FindPattern(malformedSig);
            CheckPattern(("malformed"), malformedAddr, &malformed);
        }
    }

    if (Engine_Version >= 424 || Engine_Version < 420 || Engine_Version == 421)
    {
        ValidationFailureAddr = FindPattern(ValidationFailureSig);

        if (ValidationFailureAddr)
            ValidationFailure = decltype(ValidationFailure)(ValidationFailureAddr);

        // CheckPattern(("ValidationFailure"), ValidationFailureAddr, &ValidationFailure);
    }

    if (Engine_Version < 426) // it becomes virtual bruh
    {
        SetWorldAddr = FindPattern(SetWorldSig);

        if (!SetWorldAddr)
            SetWorldAddr = FindPattern(("48 89 5C 24 ? 57 48 83 EC 20 48 8B FA 48 8B D9 48 8B 91 ? ? ? ? 48 85 D2 74 28 E8 ? ? ? ? 48 8B 8B ? ? ? ? 33 C0 48 89 83 ? ? ? ? 48 89 83 ? ? ? ? 48 89 83"));

        if (!SetWorldAddr)
            SetWorldAddr = FindPattern("48 89 74 24 ? 57 48 83 EC 20 48 8B F2 48 8B F9 48 8B 91 ? ? ? ? 48 85 D2 74 5E 48 89 5C 24 ? E8");

        // if (Engine_Version >= 419)
        CheckPattern(("SetWorld"), SetWorldAddr, &SetWorld);
    }

    {
        PauseBeaconRequestsAddr = FindPattern(PauseBeaconRequestsSig);

        if (!PauseBeaconRequestsAddr)
            PauseBeaconRequestsAddr = FindPattern("40 53 48 83 EC 30 48 8B D9 84 D2 74 6F 80 3D ? ? ? ? ? 72 33 48 8B 05 ? ? ? ? 4C 8D 44 24 ? 41 B9 ? ? ? ? 48 89 44 24 ? 48 8D 05 ? ? ? ? 48 8D");

        CheckPattern(("PauseBeaconRequests"), PauseBeaconRequestsAddr, &PauseBeaconRequests);

        InitHostAddr = FindPattern(InitHostSig);
        CheckPattern(("InitHost"), InitHostAddr, &InitHost);

        Beacon_NotifyControlMessageAddr = FindPattern(Beacon_NotifyControlMessageSig);

        if (!Beacon_NotifyControlMessageAddr)
            Beacon_NotifyControlMessageAddr = FindPattern(("40 55 53 56 57 41 54 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 33 FF 48 89 4C 24 ?"));

        if (!Beacon_NotifyControlMessageAddr)
            Beacon_NotifyControlMessageAddr = FindPattern(("4C 8B DC 49 89 5B 18 49 89 73 20 49 89 4B 08 55 57 41 54 41 56 41 57 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 33 FF 49 8B F1"));

        if (!Beacon_NotifyControlMessageAddr)
            Beacon_NotifyControlMessageAddr = FindPattern(("4C 8B DC 49 89 5B 18 49 89 73 20 55 57 41 54 41 55 41 57 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 33 F6 49 8B F9 8B DE 45 0F B6 E0 89 5C 24 68"));

        if (!Beacon_NotifyControlMessageAddr)
        {
            // auto NCMSig = Finder::Functions::GetBeaconNotifyControlMessage(35, &Beacon_NotifyControlMessageAddr);
        }

        if (FnVerDouble < 20.00)
            CheckPattern(("Beacon_NotifyControlMessage"), Beacon_NotifyControlMessageAddr, &Beacon_NotifyControlMessage);
    }

    HandleReloadCostAddr = FindPattern(HandleReloadCostSig);

    if (!HandleReloadCostAddr)
        std::cout << ("[WARNING] Unable to find HandleReloadCost! Will not be able to deplete ammo.\n");
    else
        HandleReloadCost = decltype(HandleReloadCost)(HandleReloadCostAddr);
    
    StaticLoadObjectAddr = FindPattern(StaticLoadObjectSig);

    if (!StaticLoadObjectAddr) {
        StaticLoadObjectAddr = FindPattern("4C 89 4C 24 ? 48 89 54 24 ? 48 89 4C 24 ? 55 53 56 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 33 D2");
    }

    // 4C 89 4C 24 ? 48 89 54 24 ? 48 89 4C 24 ? 55 53 56 57 ??

    if (!StaticLoadObjectAddr)
    {
        Finder::Functions::GetStaticLoadObject(35, &StaticLoadObjectAddr);
    }

    CheckPattern(("StaticLoadObject"), StaticLoadObjectAddr, &StaticLoadObjectO);

    if (Engine_Version < 424 || FnVerDouble >= 16.00)
    {
        KickPlayerAddr = FindPattern(KickPlayerSig);
        CheckPattern(("KickPlayer"), KickPlayerAddr, &KickPlayer);
    }

    /* if (Engine_Version == 424)
    {
        CreateChannelAddr = FindPattern(CreateChannelByName);
        CheckPattern(("CreateChannelByName"), CreateChannelAddr, &CreateChannelByName);
    } */

    if (FnVerDouble <= 3.3)
    {
        IsNetRelevantForAddr = FindPattern(IsNetRelevantForSig);

        if (!IsNetRelevantForAddr)
            std::cout << "[WARNING] Will not have relevancy!\n";

        IsNetRelevantFor = decltype(IsNetRelevantFor)(IsNetRelevantForAddr);

        ActorChannelCloseAddr = FindPattern(ActorChannelCloseSig);

        if (!ActorChannelCloseAddr)
            std::cout << "[WARNING] Will not have relevancy!\n";

        UActorChannel_Close = decltype(UActorChannel_Close)(ActorChannelCloseAddr);

        CreateChannelAddr = FindPattern(CreateChannelSig);
        CheckPattern(("CreateChannel"), CreateChannelAddr, &CreateChannel);

        ReplicateActorAddr = FindPattern(ReplicateActorSig);

        if (!ReplicateActorAddr)
            ReplicateActorAddr = FindPattern("40 55 53 57 41 56 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8D 59 68 4C 8B F1 48 8B 0B 48");

        CheckPattern(("ReplicateActor"), ReplicateActorAddr, &ReplicateActor);

        SetChannelActorAddr = FindPattern(SetChannelActorSig);

        if (!SetChannelActorAddr)
            SetChannelActorAddr = FindPattern("48 8B C4 55 53 57 41 54 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 45 33 E4 48 89 70 D8 44 89 A5 ? ? ? ? 48 8D 35 ? ? ? ? 4C 89 78 C8 48 8B D9 48 8B 41 28 48 8B FA 45 8B FC 48");

        if (!SetChannelActorAddr)
            SetChannelActorAddr = FindPattern("4C 8B DC 55 53 57 41 54 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 45 33 E4 4D 89 73 D0 44 89 A5");

        CheckPattern(("SetChannelActor"), SetChannelActorAddr, &SetChannelActor);

        CallPreReplicationAddr = FindPattern(CallPreReplicationSig);

        if (!CallPreReplicationAddr)
            CallPreReplicationAddr = FindPattern("48 85 D2 0F 84 ? ? ? ? 48 8B C4 55 57 41 54 48 8D 68 A1 48 81 EC ? ? ? ? 48 89 58 08 4C 8B E2 48 89 70 10 48 8D 55 D7 48 8B");

        if (!CallPreReplicationAddr)
            CallPreReplicationAddr = FindPattern("48 85 D2 0F 84 ? ? ? ? 48 8B C4 55 57 41 57 48 8D 68 A1 48");

        CheckPattern(("CallPreReplication"), CallPreReplicationAddr, &CallPreReplication);
    }

    if (FnVerDouble == 4.5)
    {
        uintptr_t CrashFix = FindPattern(CrashPatchSig);
        if (CrashFix) {
            std::cout << "Applying crash fix!\n";
            *reinterpret_cast<char*>(CrashFix) = 0xE9;
            *reinterpret_cast<char*>(CrashFix + 1) = 0x39;
            *reinterpret_cast<char*>(CrashFix + 2) = 0x02;
            *reinterpret_cast<char*>(CrashFix + 3) = 0x00;
            *reinterpret_cast<char*>(CrashFix + 4) = 0x00;
        }
    }
}
