#pragma once

#define N_T // S2
// #define T_F // S3-S4
// #define S_F // S5-S6
// #define S_T // S7+
// #define F_TF // 4.24
// #define F_FF // UE4.25

#define BEFORE_SEASONEIGHT

// 3.5 Patterns are from raider, some patterns are not mine.

namespace Patterns
{
#ifdef N_T
    constexpr const char* Realloc = "4C 8B D1 48 8B 0D ? ? ? ? 48 85 C9 75 08 49 8B CA E9 ? ? ? ? 48 8B 01 45 8B C8 4C 8B C2 49 8B D2 48 FF 60 18";// ud
    constexpr const char* TickFlush = "4C 8B DC 55 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 45 0F 29 43 ? 45 0F 29 4B ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 49 89 5B 18 49 89 7B E8 48 8B F9 4D 89 6B D8 45 33 ED 4D 89 7B C8"; // ud
    constexpr const char* World_NotifyControlMessage = "40 55 53 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 33 FF 48 89 4D D0 89 7C 24 58 49 8B D9";// ud
    constexpr const char* SpawnPlayActor = "44 89 44 24 ? 48 89 54 24 ? 48 89 4C 24 ? 55 53 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 33 F6 48 8D 05 ? ? ? ? 89 75 67 4D 8B E9 4C 8B 65 77 49 39 04 24 74"; // ud
    constexpr const char* KickPlayer = "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 49 8B F0 48 8B DA 48 85 D2 74 78 48 8B BA ? ? ? ? 48 85 FF 74 6C E8 ? ? ? ? 48 8B 57 10 4C 8D 88";// ud
    constexpr const char* Beacon_NotifyControlMessage = "40 55 53 56 57 41 54 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 33 FF 48 89 4C 24 ? 89 7C 24 60 4D 8B F1 48 8B 41 10 45 0F"; // ud
    constexpr const char* InitHost = "48 8B C4 48 81 EC ? ? ? ? 48 89 58 18 4C 8D 05 ? ? ? ? 48 8B D9 48 89 78 F8 48 8D 48 88 45 33 C9 33 D2 E8 ? ? ? ? E8 ? ? ? ? 48 8B C8 4C 8D 84 24 ? ? ? ? 48 8D 15 ? ? ? ? E8 ? ? ? ?"; // ud
    constexpr const char* PauseBeaconRequests = "40 53 48 83 EC 30 48 8B D9 84 D2 74 68 80 3D ? ? ? ? ? 72 2C 48 8B 05 ? ? ? ? 4C 8D 44 24 ? 48 89 44 24 ? 41 B9 ? ? ? ? 48 8D 05 ? ? ? ? 33 D2 33 C9 48 89 44 24 ? E8 ? ? ? ?"; // ud
    constexpr const char* StaticFindObject = "48 89 5C 24 ? 48 89 74 24 ? 55 57 41 54 41 56 41 57 48 8B EC 48 83 EC 60 80 3D ? ? ? ? ? 45 0F B6 F1 49 8B F8 48 8B DA 4C 8B F9 74 52 48 8B 05 ? ? ? ? 4C 8D 45 38 48"; // ud
    constexpr const char* ReplicateActor = "40 55 56 41 54 41 55 41 56 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 4C 8B E9 48 8B 49 68 48 8B 01 FF 90 ? ? ? ? 41 8B 4D 30 48 8D 35 ? ? ? ? 4C 8B F0 0F BA E1 08 0F 83 ? ? ? ? 41 83 7D ? ? 7E 12 32 C0 48 81 C4 ? ? ? ? 41 5E 41 5D 41 5C"; // ud
    constexpr const char* CreateChannel = "40 56 57 41 54 41 55 41 57 48 83 EC 60 48 8B 01 41 8B F9 45 0F B6 E0 4C 63 FA 48 8B F1 FF 90 ? ? ? ? 45 33 ED 83 FF FF 0F 85 ? ? ? ? 4C 63 8E ? ? ? ? 41 83 FF 01 41 8B FD B8"; // ud
    constexpr const char* SetChannelActor = "48 8B C4 55 53 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 48 89 70 E8 48 8B D9 48 89 78 E0 48 8D 35 ? ? ? ? 4C 89 60 D8 48 8B FA 45 33 E4 4C 89 78 C8 44 89 A5 ? ? ? ? 45 8B FC 48 8B 41 28 48 8B 48 58 48 85 C9 0F 84 ? ? ? ?"; // ud
    constexpr const char* SendClientAdjustment = "40 53 48 83 EC 20 48 8B 99 ? ? ? ? 48 39 99 ? ? ? ? 74 0A 48 83 B9 ? ? ? ? ? 74 78 48 85 DB 75 0C 48 8B 99 ? ? ? ? 48 85 DB 74 67 80 BB ? ? ? ? ? 75 5E 48 8B 81 ? ? ? ? 33 D2 48 89 54 24 ? 48 3B C2 74 09"; // ud
    constexpr const char* NetDebug = "40 55 56 41 56 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 8B 01"; // ud

    constexpr const char* SetWorld = "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 48 8B B1 ? ? ? ? 48 8B FA 48 8B D9 48 85 F6 74 5C 48 8B 93 ? ? ? ? 48 8D 8E ? ? ? ? E8 ? ? ? ? 48 8B 93 ? ? ? ? 48 8D 8E ? ? ? ? E8"; // ud
    constexpr const char* InitListen = "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 50 48 8B BC 24 ? ? ? ? 49 8B F0";// ud
    constexpr const char* CreateNetDriver = "4C 89 44 24 ? 53 48 83 EC 20 48 8B D9 E8 ? ? ? ? 4C 8B 44 24 ? 48 8B D0 48 8B CB E8 ? ? ? ? 48 83 C4 20 5B C3"; // ud

    // paddin

    constexpr const char* NoReserve = "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 56 41 57 48 81 EC ? ? ? ? 48 8B 01 49 8B D9 45 0F B6 F0 48 8B F2 48 8B F9 45 32 FF FF 90 ? ? ? ? 48 8B 0D ? ? ? ?";
    constexpr const char* WelcomePlayer = "48 8B C4 55 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 48 89 58 18 48 8B 59 30 48 89 78 E8 48 8B FA 4C 89 68 D8";//
    constexpr const char* ReceiveUniqueIdRepl = "48 89 5C 24 ? 55 56 57 48 8B EC 48 83 EC 40 F6 41 28 40 48 8B FA 48 8B D9 0F 84 ? ? ? ? F6 41 2B 02";//
    constexpr const char* ReceiveFString = "48 89 5C 24 18 55 56 57 41 56 41 57 48 8D 6C 24 C9 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 27 0F B6 41 28";//
    constexpr const char* GetNetMode = "48 89 5C 24 08 57 48 83 EC ? 48 8B 01 48 8B D9 FF 90 40 01 00 00 4C 8B 83 10 01 00 00";//
    constexpr const char* LocalPlayerSpawnPlayActor = "48 89 5C 24 08 48 89 74 24 10 48 89 7C 24 18 55 41 56 41 57 48 8D 6C 24 F0 48 81 EC ? ? ? ? 48 8B D9";//
    constexpr const char* ValidationFailure = "40 53 41 56 48 81 EC ? ? ? ? 48 8B 01 48 8B DA 4C 8B F1 FF 90 ? ? ? ? 48 8B 0D ? ? ? ? 48 8B D0 E8 ? ? ? ? 85 C0 0F 84 ? ? ? ? 48 85 DB 74 75 48 8B 83 ? ? ? ? 48 8B 88 ? ? ? ? 48 85 C9";
    constexpr const char* ClientTravel = "48 83 EC 48 45 84 C9 74 0D 41 83 F8 02 75 07 66 FF 81 ? ? ? ? 48 8B 44 24 ? 0F 10 00";
    constexpr const char* HasClientLoadedCurrentWorld = "48 89 5C 24 ? 57 48 83 EC 20 48 8B 99 ? ? ? ? 48 8B F9 48 85 DB 74 23 E8 ? ? ? ? 48 8B 53 10 4C 8D 40 30 48 63 40 38 3B 42 38 7F 0D 48 8B C8 48 8B 42 30 4C 39 04 C8 74 17 48 8B 1D ? ? ? ? 48 85 DB";
    constexpr const char* malformed = "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 50 40 32 F6 0F 29 7C 24 ? 48 8B FA 44 0F 29 44 24 ? 48 8B D9 40 38 72 28 7C 51 4C 8B 92 ? ? ? ? 4C 3B 92 ? ? ? ? 7C 0F";
    constexpr const char* GiveAbility = "48 89 5C 24 ? 48 89 6C 24 ? 48 89 7C 24 ? 41 56 48 83 EC 20 83 B9 ? ? ? ? ? 49 8B E8 4C 8B F2 48 8B F9 7E 56 48 63 9F ? ? ? ? 48 81 C7 ? ? ? ? 8D 43 01 89 87 ? ? ? ? 3B 87 ? ? ? ? 7E";
    constexpr const char* InternalTryActivateAbility = "4C 89 4C 24 20 4C 89 44 24 18 89 54 24 10 55 53 56 57 41 54";
    constexpr const char* MarkAbilitySpecDirty = "48 89 5C 24 ? 48 89 7C 24 ? 41 56 48 83 EC 20 48 8B 01 41 0F B6 D8 4C 8B F2 48 8B F9 FF 90 ? ? ? ? 84 C0 0F 84 ? ? ? ? 49 8B 46 10 48 89 6C 24 ?";
    constexpr const char* GetPlayerViewpoint = "48 89 74 24 ? 55 57 41 56 48 8B EC 48 83 EC 40 48 8B 81 ? ? ? ? 4D 8B F0 48 8B F2 48 8B F9 48 85 C0 0F 84 ? ? ? ? F6 80 ? ? ? ? ? 0F 84 ? ? ? ? 48 8B 88 ? ? ? ? 48 85 C9 74 23 0F 10 89 ? ? ? ? 0F 28 C1";
    constexpr const char* CollectGarbage = "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 56 41 57 48 83 EC 30 48 8B 35";
    constexpr const char* HandleReloadCost = "89 54 24 10 55 41 56 48 8D 6C 24 ? 48 81 EC ? ? ? ? 80 B9 ? ? ? ? ? 4C 8B F1 0F 85";
    constexpr const char* CanActivateAbility = "4C 89 4C 24 20 55 56 57 41 56 48 8D 6C 24 D1";
    constexpr const char* FixCrash = "40 55 56 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 48 8D 6C 24 ? 48 89 9D ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C5 48 89 85 ? ? ? ? 8B 41 0C 45 33 F6";
    constexpr const char* SetReplicationDriver = "40 55 41 57 48 83 EC 28 48 8B E9 4C 8B FA 48 8B 89 ? ? ? ? 48 85 C9 0F 84 ? ? ? ? 48 8B 01 48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 4C 89 64 24 ? 4C 89 74 24 ? FF 90";
    constexpr const char* CreateNetDriver_Local = "4C 89 44 24 ? 53 56 57 41 56 41 57 48 83 EC 70 48 8B B9 ? ? ? ? 48 8D 1D ? ? ? ? 48 63 81 ? ? ? ? 45 33 FF 4C 8B F2 48 8D 0C 40 4C 8D 0C CF 49 3B F9 74 22 44 8B 94 24 ? ? ? ? 44 39 57 04 0F 94 C1 44 39 07 0F 94 C0";
#endif
#ifdef T_F
    constexpr const char* CrashPatch = "0F 29 78 D8 44 0F 29 40 ? F3 45 0F 10 80 ? ? ? ? F3 41 0F 10 B0 ? ? ? ? F3 41 0F 10 88 ? ? ? ? F3 41 0F 10 A0 ? ? ? ? 44 0F 29 58 ? 44 0F 29 60 ? 45 0F 57 E4 48 8B 81 ? ? ? ? 48 05 ? ? ? ? F3 0F 58 48 ? F3 44 0F 10 18 F3 0F 10 78 ? 41 0F 28 D3 F3 0F 10 68 ? F3 41 0F 5C D0 0F 28 DF 0F 28 C5 F3 0F 58 0D E0 FA 75 02"; //4.5 Only

    constexpr const char* KickPlayer = "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 49 8B F0 48 8B DA 48 85 D2 74 ? 48 8B BA ? ? ? ? 48";
    constexpr const char* SetWorld = "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 48 8B B1 ? ? ? ? 48 8B FA 48 8B D9 48 85 F6 74 5C";

    constexpr const char* Malloc = "4C 8B C9 48 8B 0D ? ? ? ? 48 85 C9 75 08 49 8B C9 E9 ? ? ? ?";
    constexpr const char* Realloc = "4C 8B D1 48 8B 0D ? ? ? ? 48 85 C9 75 08 49 8B CA E9 ? ? ? ? 48 8B 01 45 8B C8 4C 8B C2 49 8B D2 48 FF 60 18";


    constexpr const char* ReceiveFString = "40 55 53 56 57 41 56 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 27 0F B6 41 28";
    constexpr const char* ReceiveUniqueIdRepl = "48 89 5C 24 ? 55 56 57 48 8B EC 48 83 EC 40 F6 41 28 40 48 8B FA 48 8B D9 0F 84 ? ? ? ? F6 41 2B 02";

    //S4 Fixed Patterns (Tested on 4.5)
    constexpr const char* TickFlush2 = "4C 8B DC 55 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 49 89 5B 18 48 8D 05 ? ? ? ? 49 89 73 F0 33 F6 40 38 35 ? ? ? ? 49 89 7B E8 48 8B F9 48 0F 45 C6";
    constexpr const char* World_NotifyControlMessage2 = "40 55 53 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 45 33 F6 49 8B D9";
    constexpr const char* SpawnPlayActor2 = "48 8B C4 44 89 40 18 48 89 50 10 48 89 48 08 55 56 48 8D 68 B9 48 81 EC ? ? ? ?";
    constexpr const char* Beacon_NotifyControlMessage2 = "40 55 53 56 57 41 54 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 33 FF 48 89 4C 24 ?";

    constexpr const char* TickFlush = "4C 8B DC 55 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 49 89 5B 18 48 8D 05 ? ? ? ? 49 89 7B E8 48 8B F9 4D 89 63 E0 45 33 E4";
    constexpr const char* PauseBeaconRequests = "40 53 48 83 EC 30 48 8B D9 84 D2 74 68 80 3D ? ? ? ? ? 72 2C 48 8B 05 ? ? ? ? 4C 8D 44 24 ? 48 89 44 24 ?";
    constexpr const char* Beacon_NotifyAcceptingConnection = "48 83 EC 48 48 8B 41 10 48 83 78 ? ?";
    constexpr const char* InitHost = "48 8B C4 48 81 EC ? ? ? ? 48 89 58 18 4C 8D 05 ? ? ? ?";
    constexpr const char* Beacon_NotifyControlMessage = "40 55 53 56 57 41 54 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 33 FF 48 89 4C 24 ? 89 7C 24 60 49 8B F1 48 8B 41 10 45 0F B6 E0 4C 8B F2 48 8B D9 44 8B FF 48 39 78 78 0F 85 ? ? ? ? 80 3D ? ? ? ? ?";
    constexpr const char* WelcomePlayer = "48 8B C4 55 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 48 89 70 20";
    constexpr const char* World_NotifyControlMessage = "40 55 53 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 45 33 F6 49 8B D9 44 89 74 24 ? 45 8B E6 48 8B 41 10 45 0F B6 F8 48 8B FA 4C 8B E9 4C 39 60 78";
    constexpr const char* SpawnPlayActor = "44 89 44 24 ? 48 89 54 24 ? 48 89 4C 24 ? 55 53 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 33 F6 48 8D 05 ? ? ? ? 89 75 67 4D 8B E9 4C 8B 65 77 49 39 04 24 74 2A 41 89 74 24 ? 41 39 74 24 ?";
    constexpr const char* World_NotifyAcceptingConnection = "40 55 48 83 EC 50 48 8B 41 10 48 8B E9 48 83 78 ? ? 74 45 80 3D ? ? ? ? ? 72 34 48 8B 05 ? ? ? ? 4C 8D 44 24 ? 48 89 44 24 ? 48 8D 0D ? ? ? ? 48 8D 05 ? ? ? ? 41 B9 ? ? ? ? BA ? ? ? ? 48 89 44 24 ?";
    constexpr const char* GetNetMode = "48 89 5C 24 ? 57 48 83 EC 20 48 8B 01 48 8B D9 FF 90 ? ? ? ? 4C 8B 83 ? ? ? ? 48 8B F8 33 C0 48 C7 44 24";
    constexpr const char* GiveAbility = "48 89 5C 24 ? 56 57 41 56 48 83 EC 20 83 B9 ? ? ? ? ? 49 8B F0 4C 8B F2 48 8B D9 7E 61";
    constexpr const char* InternalTryActivateAbility = "4C 89 4C 24 ? 4C 89 44 24 ? 89 54 24 10 55 53 56 57 41 54 41 57 48 8D AC 24";
    constexpr const char* MarkAbilitySpecDirty = "48 89 5C 24 ? 57 48 83 EC 20 80 B9 ? ? ? ? ? 48 8B FA 48 8B D9 75 4A C6 81";
    constexpr const char* LocalPlayerSpawnPlayActor = "40 55 53 56 57 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 40 48 8B D9 4D 8B F1 49 8B C9 4D 8B F8 48 8B F2";

    constexpr const char* InitListen = "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 50 48 8B BC 24 ? ? ? ? 49 8B F0";
    constexpr const char* PostRender = "48 89 74 24 ? 57 48 81 EC ? ? ? ? 80 B9 ? ? ? ? ? 48 8B F2";

    constexpr const char* StaticFindObject = "48 89 5C 24 ? 48 89 74 24 ? 55 57 41 54 41 56 41 57 48 8B EC 48 83 EC 60 80 3D ? ? ? ? ? 45 0F B6 F1 49 8B F8 48 8B DA 4C 8B F9 74 52 48 8B 05";
    constexpr const char* HandleReloadCost = "89 54 24 10 55 41 56 48 8D 6C 24 ? 48 81 EC ? ? ? ? 80 B9 ? ? ? ? ? 4C 8B F1 0F 85";


    constexpr const char* CollectGarbage = "40 53 48 83 EC 40 48 8B D9 FF 15 ? ? ? ? 84 C0 75 76 33 D2 33 C9 E8 ? ? ? ? 84 C0 74 69 33 C0 88 44 24 58 48 8D 44 24 ?";
    constexpr const char* NetDebug = "40 55 56 41 56 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 8B 01 48 8B F1 FF 90 ? ? ? ? 4C 8B F0 48 85 C0 0F";
    constexpr const char* GetPlayerViewpoint = "48 89 5C 24 ? 48 89 74 24 ? 55 41 56 41 57 48 8B EC 48 83 EC 50 48 8B F2 48 C7 45 ? ? ? ? ? 48 8B 55 D0 4D 8B F0 48 8B D9 45 33 FF E8 ? ? ? ? 84 C0 74 4A 80 BB ? ? ? ? ? 75 41";

    // paddin
    constexpr const char* HasClientLoadedCurrentWorld = "48 89 5C 24 ? 57 48 83 EC 20 48 8B 99 ? ? ? ? 48 8B F9 48 85 DB 74 23 E8 ? ? ? ? 48 8B 53 10 4C 8D 40 30 48 63 40 38 3B 42 38 7F 0D 48 8B C8 48 8B 42 30 4C 39 04 C8 74 17 48 8B 1D ? ? ? ? 48 85 DB";
    constexpr const char* ClientTravel = "48 83 EC 48 45 84 C9 74 0D 41 83 F8 02 75 07 66 FF 81 ? ? ? ? 48 8B 44 24 ? 0F 10 00";
    constexpr const char* CreateNetDriver = "48 89 5C 24 08 57 48 83 EC ? 49 8B D8 48 8B F9 E8 ? ? ? ? 48 8B D0";
    constexpr const char* malformed = "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 50 40 32 F6 0F 29 7C 24 ? 48 8B FA 44 0F 29 44 24 ? 48 8B D9 40 38 72 28 7C 51 4C 8B 92 ? ? ? ? 4C 3B 92 ? ? ? ? 7C 0F";
    constexpr const char* SetReplicationDriver = "40 56 41 56 48 83 EC 28 48 8B F1 4C 8B F2 48 8B 89 ? ? ? ? 48 85 C9 0F 84 ? ? ? ? 48 8B 01 48 89 5C 24 ? 48 89 6C 24 ? 4C 89 7C 24 ? FF 90 ? ? ? ? 48 8B 9E ? ? ? ? 45 33 FF";
    constexpr const char* FixCrash = "40 55 56 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 48 8D 6C 24 ? 48 89 9D ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C5 48 89 85 ? ? ? ? 8B 41 0C 45 33 F6";
    constexpr const char* NoReserve = "aaaaa";
    constexpr const char* ValidationFailure = "bbbb";
#endif
    constexpr const char* Idkf = "49 63 C0 48 FF C8 48 85 C0 7E 37 4C 8B CA 4C 8D 04 10 49 F7 D9";
    constexpr const char* SendChallenge = "48 89 5C 24 ? 55 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B DA 48 85 D2 0F 84 ? ? ? ? 83 BA ? ? ? ? ? 0F 86 ? ? ? ? 48 83 7A ? ? 0F 84 ? ? ? ? 48 8D 4D 78 48 89 BC 24 ? ? ? ? FF 15 ? ? ? ? 44 8B 45 78 48 8D 15 ? ? ? ? 48 8D 4C 24 ? E8 ? ? ? ? 48 8D 44 24 ? 48 8D BB ? ? ? ? 48 3B F8 74 25 48 8B 0F 48 85 C9 74 05 E8 ? ? ? ? 48 8B 44 24 ? 48 89 07 8B 44 24 38 89 47 08 8B 44 24 3C 89 47 0C EB 0F 48 8B 4C 24 ? 48 85 C9 74 05 E8 ? ? ? ? B2 05 48 8B CB E8 ? ? ? ? 48 8B 83 ? ? ? ? 48 8B 10 48 85 D2 0F 84 ? ? ? ? F6 42 30 02 0F 85 ? ? ? ? 45 33 C0 48 8D 4C 24 ? E8 ? ? ? ?";
#ifdef S_F
    constexpr const char* Malloc = "48 83 EC ? 48 8B 0D ? ? ? ? 48 85 C9 75 ? 33 D2 B9 ? ? ? ? E8 ? ? ? ? 48 8B 4C 24 58"; // updated
    constexpr const char* Realloc = "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC ? 48 8B F1 41 8B D8 48 8B 0D ? ? ? ?";// updated
    constexpr const char* FNameToString = "48 89 5C 24 08 57 48 83 EC ? 83 79 04 ?";// updated
    constexpr const char* TickFlush = "4C 8B DC 55 49 8D AB 98 FE FF FF 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 00 01 00 00 49 89 5B 18 48 8D 05 ? ? ? ?";// updated
    constexpr const char* CreateNetDriver = "48 89 5C 24 ? 57 48 83 EC 30 48 8B 81 ? ? ? ? 49 8B D8 4C 63 81 ? ? ? ? 4C 8B D2 48 8B F9 4E 8D 0C C0 49 3B C1 74 1B";
    constexpr const char* SetWorld = "48 89 5C 24 ? 57 48 83 EC 20 48 8B FA 48 8B D9 48 8B 91 ? ? ? ? 48 85 D2 74 28 E8 ? ? ? ? 48 8B 8B ? ? ? ? 33 C0";
    constexpr const char* InitListen = "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 50 48 8B BC 24 ? ? ? ? 49 8B F0";// updated
    constexpr const char* WelcomePlayer = "48 89 5C 24 ? 48 89 74 24 ? 55 57 41 56 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B 59 30 48 8B F1 48 8B CA 48 8B FA E8 ? ? ? ? 48 8B CB E8 ? ? ? ? 48 8D 54 24 ? 48 8D 8D ? ? ? ? 4C 8B";// updated
    constexpr const char* World_NotifyControlMessage = "48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 48 89 4C 24 ? 55 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 33 FF 49 8B D9 89 7C 24 54";// updatedHM
    constexpr const char* SpawnPlayActor = "48 8B C4 4C 89 48 20 44 89 40 18 48 89 50 10 48 89 48 08 55 56";// updated -> ps: � un p� cambiata come parametri ma � questa
    constexpr const char* ReceiveUniqueIdRepl = "48 89 5C 24 ? 55 56 57 48 8B EC 48 83 EC 40 F6 41 28 40 48 8B FA 48 8B D9 0F 84 ? ? ? ? F6 41 2B 02";// updated
    constexpr const char* ReceiveFString = "48 89 5C 24 18 55 56 57 41 56 41 57 48 8D 6C 24 C9 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 27 0F B6 41 28";// updated
    constexpr const char* KickPlayer = "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC ? 49 8B F0 48 8B DA 48 85 D2";// updated
    constexpr const char* GetNetMode = "48 89 5C 24 ? 57 48 83 EC 20 48 8B 01 48 8B D9 FF 90 ? ? ? ? 4C 8B 83 ? ? ? ? 48 8B F8 33 C0 48 C7 44 24";// updated
    constexpr const char* LocalPlayerSpawnPlayActor = "40 55 53 56 57 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 40 48 8B D9 4D 8B F1 49 8B C9 4D 8B F8 48 8B F2"; // updated
    constexpr const char* CollectGarbage = "48 89 5C 24 08 48 89 6C 24 10 56 57 41 56 48 81 EC ? ? ? ? 4C 8B 05"; // NON ESISTE PIU'
    constexpr const char* NetDebug = "40 55 56 41 56 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 8B 01 48 8B F1 FF 90 ? ? ? ? 4C 8B F0 48 85 C0 0F 84 ? ? ? ? 48 83 78";
    constexpr const char* SetReplicationDriver = "40 55 41 57 48 83 EC 28 48 8B E9 4C 8B FA 48 8B 89 ? ? ? ? 48 85 C9 0F 84 ? ? ? ? 48 8B 01 48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 4C 89 64 24 ? 4C 89 74 24 ? FF";
    constexpr const char* GetPlayerViewpoint = "48 89 5C 24 ? 48 89 74 24 ? 55 41 56 41 57 48 8B EC 48 83 EC 40 48 8B F2 48 C7 45 ? ? ? ? ? 48 8B 55 38 4D 8B F0 48 8B D9 45 33 FF E8 ? ? ? ? 84 C0 74 4A 80 BB ? ? ? ? ? 75 41 48 8B 03 48 8B CB";

    // paddin
    constexpr const char* InitHost = "48 8B C4 48 81 EC ? ? ? ? 48 89 58 18 4C 8D 05 ? ? ? ?";
    constexpr const char* PauseBeaconRequests = "40 53 48 83 EC 30 48 8B 99 ? ? ? ? 48 85 DB 0F 84 ? ? ? ? 84 D2 74 68 80 3D ? ? ? ? ? 72 2C 48 8B 05 ? ? ? ? 4C 8D 44 24 ? 48 89 44 24 ? 41 B9";
    constexpr const char* Beacon_NotifyControlMessage = "4C 8B DC 49 89 5B 18 49 89 7B 20 49 89 4B 08";
    constexpr const char* HasClientLoadedCurrentWorld = "48 89 5C 24 ? 57 48 83 EC 20 48 8B 99 ? ? ? ? 48 8B F9 48 85 DB 74 23 E8 ? ? ? ? 48 8B 53 10 4C 8D 40 30 48 63 40 38 3B 42 38 7F 0D 48 8B C8 48 8B 42 30 4C 39 04 C8 74 17 48 8B 1D ? ? ? ? 48 85 DB";
    constexpr const char* malformed = "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 50 40 32 F6 0F 29 7C 24 ? 48 8B FA 44 0F 29 44 24 ? 48 8B D9 40 38 72 28 7C 51 4C 8B 92 ? ? ? ? 4C 3B 92 ? ? ? ? 7C 0F";
    constexpr const char* FixCrash = "40 55 56 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 48 8D 6C 24 ? 48 89 9D ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C5 48 89 85 ? ? ? ? 8B 41 0C 45 33 F6";
    constexpr const char* ValidationFailure = "48 89 4C 24 ? 55 53 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 33 DB 48 8B F2";
    constexpr const char* GiveAbility = "48 89 5C 24 ? 48 89 6C 24 ? 48 89 7C 24 ? 41 56 48 83 EC 20 83 B9 ? ? ? ? ? 49 8B E8 4C 8B F2 48 8B F9 7E 56 48 63 9F ? ? ? ? 48 81 C7 ? ? ? ? 8D 43 01 89 87 ? ? ? ? 3B 87 ? ? ? ? 7E";
    constexpr const char* InternalTryActivateAbility = "4C 89 4C 24 20 4C 89 44 24 18 89 54 24 10 55 53 56 57 41 54";
    constexpr const char* MarkAbilitySpecDirty = "48 89 5C 24 ? 48 89 7C 24 ? 41 56 48 83 EC 20 48 8B 01 41 0F B6 D8 4C 8B F2 48 8B F9 FF 90 ? ? ? ? 84 C0 0F 84 ? ? ? ? 49 8B 46 10 48 89 6C 24 ?";
    constexpr const char* CreateNetDriver_Local = "4C 89 44 24 ? 53 56 57 41 56 41 57 48 83 EC 70 48 8B B9 ? ? ? ? 48 8D 1D ? ? ? ? 48 63 81 ? ? ? ? 45 33 FF 4C 8B F2 48 8D 0C 40 4C 8D 0C CF 49 3B F9 74 22 44 8B 94 24 ? ? ? ? 44 39 57 04 0F 94 C1 44 39 07 0F 94 C0";
#endif
#ifdef S_T
    constexpr const char* Realloc = "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC ? 48 8B F1 41 8B D8 48 8B 0D";//
    constexpr const char* TickFlush = "4C 8B DC 55 49 8D AB 78 FE FF FF 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 00 01 00 00 49 89 5B 18";//
    constexpr const char* InitListen = "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 50 48 8B BC 24 ? ? ? ? 49 8B F0";//
    constexpr const char* WelcomePlayer = "48 8B C4 55 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 48 89 58 18 48 8B 59 30 48 89 78 E8 48 8B FA 4C 89 68 D8";//
    constexpr const char* World_NotifyControlMessage = "48 89 5C 24 10 48 89 74 24 18 48 89 7C 24 20 48 89 4C 24 08 55 41 54 41 55 41 56 41 57 48 8D AC 24 D0 F9 FF FF";//
    constexpr const char* SpawnPlayActor = "48 8B C4 4C 89 48 20 44 89 40 18 48 89 50 10 48 89 48 08 55 56";//
    constexpr const char* ReceiveUniqueIdRepl = "48 89 5C 24 ? 55 56 57 48 8B EC 48 83 EC 40 F6 41 28 40 48 8B FA 48 8B D9 0F 84 ? ? ? ? F6 41 2B 02";//
    constexpr const char* ReceiveFString = "48 89 5C 24 18 55 56 57 41 56 41 57 48 8D 6C 24 C9 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 27 0F B6 41 28";//
    constexpr const char* KickPlayer = "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC ? 49 8B F0 48 8B DA 48 85 D2";//
    constexpr const char* GetNetMode = "48 89 5C 24 08 57 48 83 EC ? 48 8B 01 48 8B D9 FF 90 40 01 00 00 4C 8B 83 10 01 00 00";//
    constexpr const char* LocalPlayerSpawnPlayActor = "48 89 5C 24 08 48 89 74 24 10 48 89 7C 24 18 55 41 56 41 57 48 8D 6C 24 F0 48 81 EC ? ? ? ? 48 8B D9";//
    constexpr const char* Beacon_NotifyControlMessage = "4C 8B DC 49 89 5B 18 49 89 7B 20 49 89 4B 08";
    constexpr const char* InitHost = "48 8B C4 48 81 EC ? ? ? ? 48 89 58 18 4C 8D 05 ? ? ? ?";
    constexpr const char* PauseBeaconRequests = "40 53 48 83 EC 30 48 8B D9 84 D2 74 68 80 3D ? ? ? ? ? 72 2C 48 8B 05 ? ? ? ? 4C 8D 44 24 ? 48 89 44 24 ? 41 B9 ? ? ? ?";
    constexpr const char* SetWorld = "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 48 8B 99 ? ? ? ? 48 8B F2 48 8B F9 48 85 DB 0F 84 ? ? ? ? 48 8B 97 ? ? ? ? 48 8D 8B ? ? ? ? E8 ? ? ? ?";
    constexpr const char* NoReserve = "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 56 41 57 48 81 EC ? ? ? ? 48 8B 01 49 8B D9 45 0F B6 F0 48 8B F2 48 8B F9 45 32 FF FF 90 ? ? ? ? 48 8B 0D ? ? ? ?";
    constexpr const char* ValidationFailure = "40 53 41 56 48 81 EC ? ? ? ? 48 8B 01 48 8B DA 4C 8B F1 FF 90 ? ? ? ? 48 8B 0D ? ? ? ? 48 8B D0 E8 ? ? ? ? 85 C0 0F 84 ? ? ? ? 48 85 DB 74 75 48 8B 83 ? ? ? ? 48 8B 88 ? ? ? ? 48 85 C9";
    constexpr const char* ClientTravel = "48 83 EC 48 45 84 C9 74 0D 41 83 F8 02 75 07 66 FF 81 ? ? ? ? 48 8B 44 24 ? 0F 10 00";
    constexpr const char* CreateNetDriver = "48 89 5C 24 08 57 48 83 EC ? 49 8B D8 48 8B F9 E8 ? ? ? ? 48 8B D0";
    constexpr const char* HasClientLoadedCurrentWorld = "48 89 5C 24 ? 57 48 83 EC 20 48 8B 99 ? ? ? ? 48 8B F9 48 85 DB 74 23 E8 ? ? ? ? 48 8B 53 10 4C 8D 40 30 48 63 40 38 3B 42 38 7F 0D 48 8B C8 48 8B 42 30 4C 39 04 C8 74 17 48 8B 1D ? ? ? ? 48 85 DB";
    constexpr const char* malformed = "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 50 40 32 F6 0F 29 7C 24 ? 48 8B FA 44 0F 29 44 24 ? 48 8B D9 40 38 72 28 7C 51 4C 8B 92 ? ? ? ? 4C 3B 92 ? ? ? ? 7C 0F";
    constexpr const char* GiveAbility = "48 89 5C 24 ? 48 89 6C 24 ? 48 89 7C 24 ? 41 56 48 83 EC 20 83 B9 ? ? ? ? ? 49 8B E8 4C 8B F2 48 8B F9 7E 56 48 63 9F ? ? ? ? 48 81 C7 ? ? ? ? 8D 43 01 89 87 ? ? ? ? 3B 87 ? ? ? ? 7E";
    constexpr const char* InternalTryActivateAbility = "4C 89 4C 24 20 4C 89 44 24 18 89 54 24 10 55 53 56 57 41 54";
    constexpr const char* MarkAbilitySpecDirty = "48 89 5C 24 ? 48 89 7C 24 ? 41 56 48 83 EC 20 48 8B 01 41 0F B6 D8 4C 8B F2 48 8B F9 FF 90 ? ? ? ? 84 C0 0F 84 ? ? ? ? 49 8B 46 10 48 89 6C 24 ?";
    constexpr const char* GetPlayerViewpoint = "48 89 74 24 ? 55 57 41 56 48 8B EC 48 83 EC 40 48 8B 81 ? ? ? ? 4D 8B F0 48 8B F2 48 8B F9 48 85 C0 0F 84 ? ? ? ? F6 80 ? ? ? ? ? 0F 84 ? ? ? ? 48 8B 88 ? ? ? ? 48 85 C9 74 23 0F 10 89 ? ? ? ? 0F 28 C1";
    constexpr const char* CollectGarbage = "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 56 41 57 48 83 EC 30 48 8B 35"; // THIS IS DIFFERENT!
    constexpr const char* StaticFindObject = "48 89 5C 24 ? 48 89 74 24 ? 55 57 41 54 41 56 41 57 48 8B EC 48 83 EC 60 80 3D ? ? ? ? ? 45 0F B6 F1 49 8B F8 48 8B DA 4C 8B F9 74 4E 48 8B 05 ? ? ? ? 48 8D 35 ? ? ? ?";
    constexpr const char* HandleReloadCost = "89 54 24 10 55 41 56 48 8D 6C 24 ? 48 81 EC ? ? ? ? 80 B9 ? ? ? ? ? 4C 8B F1 0F 85";
    constexpr const char* CanActivateAbility = "4C 89 4C 24 20 55 56 57 41 56 48 8D 6C 24 D1";

    // paddin

    constexpr const char* NetDebug = "40 55 56 41 56 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 8B 01";
    constexpr const char* FixCrash = "40 55 56 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 48 8D 6C 24 ? 48 89 9D ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C5 48 89 85 ? ? ? ? 8B 41 0C 45 33 F6";
    constexpr const char* SetReplicationDriver = "40 55 41 57 48 83 EC 28 48 8B E9 4C 8B FA 48 8B 89 ? ? ? ? 48 85 C9 0F 84 ? ? ? ? 48 8B 01 48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 4C 89 64 24 ? 4C 89 74 24 ? FF 90";
    constexpr const char* CreateNetDriver_Local = "4C 89 44 24 ? 53 56 57 41 56 41 57 48 83 EC 70 48 8B B9 ? ? ? ? 48 8D 1D ? ? ? ? 48 63 81 ? ? ? ? 45 33 FF 4C 8B F2 48 8D 0C 40 4C 8D 0C CF 49 3B F9 74 22 44 8B 94 24 ? ? ? ? 44 39 57 04 0F 94 C1 44 39 07 0F 94 C0";
#endif
#ifdef F_TF
    constexpr const char* TickFlush = "4C 8B DC 55 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 49 89 5B 18 49 89 73 F0 48 8B F1 49 89 7B E8 48 8D 0D ? ? ? ? 4D 89 73 D0";
    constexpr const char* World_NotifyControlMessage = "48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 48 89 4C 24 ? 55 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 33 FF";//
    constexpr const char* SpawnPlayActor = "48 8B C4 4C 89 48 20 44 89 40 18 48 89 50 10 48 89 48 08 55 56 48 8D 68 B9 48 81 EC ? ? ? ? 48 89 78 E0 33 F6 4C 89 60 D8 49 8B F9 4C 89 68 D0 4C 8D 2D ? ? ? ? 89 75 AF 4C 8B 65 77";//
    constexpr const char* ReceiveUniqueIdRepl = "48 89 5C 24 ? 55 56 57 48 8B EC 48 83 EC 40 F6 41 28 40 48 8B FA 48 8B D9 0F 84 ? ? ? ? F6 41 2B 04 0F 85 ? ? ? ? 48 8B 4A 08 33 F6 48 85 C9 74 16 48 8B 01 FF 50 20 84 C0 74 0C";//
    constexpr const char* ReceiveFString = "48 89 5C 24 ? 55 56 57 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 27 F6 41 28 01 48 8B F2 48 8B F9 0F 84 ? ? ? ? 48";//
    constexpr const char* KickPlayer = ""; // NOTE: This is a different kickplayer than the other versions.
    constexpr const char* LocalPlayerSpawnPlayActor = "48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 55 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B D9 4D 8B F1 49 8B C9 4D 8B F8 48 8B F2";//
    constexpr const char* Beacon_NotifyControlMessage = "4C 8B DC 49 89 5B 18 49 89 73 20 49 89 4B 08 55 57 41 55 41 56 41 57 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 33 FF 49 8B F1 89 7C 24 50 45 0F B6 E8 48 8B 41 10 4C 8B F2";
    constexpr const char* InitHost = "48 8B C4 48 81 EC ? ? ? ? 48 89 58 18 4C 8D 05 ? ? ? ? 48 8B D9 48 89 78 F8 48 8D 48 88 45 33 C9 33 D2 E8 ? ? ? ? E8 ? ? ? ? 48 8B C8 4C 8D 84 24 ? ? ? ? 48 8D 15 ? ? ? ? E8 ? ? ? ? 84 C0 74 11 8B";
    constexpr const char* PauseBeaconRequests = "40 53 48 83 EC 30 48 8B D9 84 D2 74 68 80 3D ? ? ? ? ? 72 2C 48 8B 05 ? ? ? ? 4C 8D 44 24 ? 48 89 44 24 ? 41 B9 ? ? ? ? 48 8D 05 ? ? ? ? 33 D2 33 C9";
    constexpr const char* WelcomePlayer = "48 8B C4 55 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 48 89 58 18 48 8B 59 30 48 89 78 E8 48 8B FA 4C 89 68 D8 48 8B D1 4C 89 78 C8 45 33 ED 4C 8B F9 4C 89 6C 24 ?";//
    constexpr const char* NetDebug = "40 55 57 41 56 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 8B 01 48 8B F9 FF 90 ? ? ? ? 4C 8B F0 48 85 C0 0F 84 ? ? ? ? 48 83 78 ? ? 0F 84 ? ? ? ? 48 8B 07 48 8D 0D ? ? ? ?";
    constexpr const char* FixCrash = "";
    constexpr const char* SetReplicationDriver = "40 55 41 57 48 83 EC 28 48 8B E9 4C 8B FA 48 8B 89 ? ? ? ? 48 85 C9 0F 84 ? ? ? ? 48 8B 01 48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 4C 89 64 24 ? 4C 89 74 24 ? FF 90 ? ? ? ? 48 63";
    constexpr const char* ValidationFailure = "40 55 53 56 41 54 41 55 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 33 F6 48 8B DA 89 B5 ? ? ? ? 4C 8B E1 E8 ? ? ? ? 49 8B 04 24 49 8B CC FF 90 ? ? ? ? 48 8B 0D ? ? ? ? 48 8B D0 4C 8B E8 E8 ? ? ?";
    constexpr const char* CreateNetDriver_Local = "";
    constexpr const char* InitListen = "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 50 48 8B BC 24 ? ? ? ? 49 8B F0";
    constexpr const char* CreateNetDriver = "48 89 5C 24 08 57 48 83 EC ? 49 8B D8 48 8B F9 E8 ? ? ? ? 48 8B D0";
    constexpr const char* SetWorld = "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 48 8B 99 ? ? ? ? 48 8B F2 48 8B F9 48 85 DB 0F 84 ? ? ? ? 48 8B 97 ? ? ? ? 48 8D 8B ? ? ? ? E8 ? ? ? ? 48 8B 97";
    constexpr const char* StaticFindObject = "48 89 5C 24 ? 48 89 74 24 ? 55 57 41 54 41 56 41 57 48 8B EC 48 83 EC 60 80 3D ? ? ? ? ? 45 0F B6 F1 49 8B F8 48 8B DA 4C 8B F9 74 4E 48 8B 05 ? ? ? ? 48 8D 35 ? ? ? ? 41 B9 ? ? ? ?";
    constexpr const char* GetPlayerViewpoint = "48 89 5C 24 ? 55 56 41 56 48 8B EC 48 83 EC 40 48 8B F2 48 8B D9 BA ? ? ? ? 48 8D 4D 38 4D 8B F0 E8 ? ? ? ? 48 8B CB 48 8B 10 E8 ? ? ? ? 84 C0 74 53";

    // paddin (aka patterns which arent right but we ned them to build)

    constexpr const char* GetNetMode = "48 89 5C 24 08 57 48 83 EC ? 48 8B 01 48 8B D9 FF 90 40 01 00 00 4C 8B 83 10 01 00 00";//
    constexpr const char* Realloc = "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC ? 48 8B F1 41 8B D8 48 8B 0D";//
    constexpr const char* NoReserve = "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 56 41 57 48 81 EC ? ? ? ? 48 8B 01 49 8B D9 45 0F B6 F0 48 8B F2 48 8B F9 45 32 FF FF 90 ? ? ? ? 48 8B 0D ? ? ? ?";
    constexpr const char* ClientTravel = "48 83 EC 48 45 84 C9 74 0D 41 83 F8 02 75 07 66 FF 81 ? ? ? ? 48 8B 44 24 ? 0F 10 00";
    constexpr const char* HasClientLoadedCurrentWorld = "48 89 5C 24 ? 57 48 83 EC 20 48 8B 99 ? ? ? ? 48 8B F9 48 85 DB 74 23 E8 ? ? ? ? 48 8B 53 10 4C 8D 40 30 48 63 40 38 3B 42 38 7F 0D 48 8B C8 48 8B 42 30 4C 39 04 C8 74 17 48 8B 1D ? ? ? ? 48 85 DB";
    constexpr const char* malformed = "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 50 40 32 F6 0F 29 7C 24 ? 48 8B FA 44 0F 29 44 24 ? 48 8B D9 40 38 72 28 7C 51 4C 8B 92 ? ? ? ? 4C 3B 92 ? ? ? ? 7C 0F";
    constexpr const char* GiveAbility = "48 89 5C 24 ? 48 89 6C 24 ? 48 89 7C 24 ? 41 56 48 83 EC 20 83 B9 ? ? ? ? ? 49 8B E8 4C 8B F2 48 8B F9 7E 56 48 63 9F ? ? ? ? 48 81 C7 ? ? ? ? 8D 43 01 89 87 ? ? ? ? 3B 87 ? ? ? ? 7E";
    constexpr const char* InternalTryActivateAbility = "4C 89 4C 24 20 4C 89 44 24 18 89 54 24 10 55 53 56 57 41 54";
    constexpr const char* MarkAbilitySpecDirty = "48 89 5C 24 ? 48 89 7C 24 ? 41 56 48 83 EC 20 48 8B 01 41 0F B6 D8 4C 8B F2 48 8B F9 FF 90 ? ? ? ? 84 C0 0F 84 ? ? ? ? 49 8B 46 10 48 89 6C 24 ?";
    constexpr const char* CollectGarbage = "48 89 5C 24 08 48 89 6C 24 10 56 57 41 56 48 81 EC ? ? ? ? 4C 8B 05"; // NON ESISTE PIU'
#endif
#ifdef F_FF
    constexpr const char* TickFlush = "4C 8B DC 55 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 49 89 5B 18 49 89 73 F0 49 89 7B E8 48 8B F9";
    constexpr const char* World_NotifyControlMessage = "48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 48 89 4C 24 ? 55 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 33 FF 45 0F B6 E0 89 7C 24 74";//
    constexpr const char* SpawnPlayActor = "48 8B C4 44 89 40 18 48 89 50 10 48 89 48 08 55 57 48 8D 68 B9 48 81 EC ? ? ? ? 48 89 70 E0 33 FF 4C 89 60 D8 49 8B F1 4C 89 68 D0";//
    constexpr const char* ReceiveUniqueIdRepl = "48 89 5C 24 ? 55 56 57 48 8B EC 48 83 EC 40 80 79 28 00 48 8B FA 48 8B D9 0F 8D ? ? ? ? F6 41 2B 08 0F 85 ? ? ? ? 48 8B 4A 08 33 F6 48 85 C9";//
    constexpr const char* ReceiveFString = "48 89 5C 24 ? 55 56 57 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 00 F6 41 28 01 4C 8B F2 48 8B F9 0F 84 ? ? ? ? 48 8B 49 08";//
    constexpr const char* KickPlayer = "48 8B C4 53 48 83 EC 70 48 89 68 08 48 8B EA 48 89 70 10 4C 89 60 20 45 33 E4 4C 89 70 F0 4D 8B F0 4C 89 78 E8 4C 8B F9 48 8D 48 C8"; // NOTE: This is a different kickplayer than the other versions.
    constexpr const char* LocalPlayerSpawnPlayActor = "48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 55 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B D9 4D 8B F1 49 8B C9 4D 8B F8 48 8B F2";//
    constexpr const char* Beacon_NotifyControlMessage = "4C 8B DC 49 89 5B 18 49 89 73 20 49 89 4B 08 55 57 41 54 41 55 41 57 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 33 F6 49 8B F9";
    constexpr const char* InitHost = "48 8B C4 48 81 EC ? ? ? ? 48 89 58 10 4C 8D 05 ? ? ? ? 48 8B D9 48 89 78 F8 48 8D 48 88";
    constexpr const char* PauseBeaconRequests = "40 53 48 83 EC 30 48 8B D9 84 D2 74 5E 80 3D ? ? ? ? ? 72 22 48 8D 05 ? ? ? ? 41 B9 ? ? ? ? 4C 8D 05 ? ? ? ? 48 89 44 24";
    constexpr const char* WelcomePlayer = "48 8B C4 55 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 48 89 58 18 48 8B 59 30 48 89 78 E8 48 8B FA 4C 89 68 D8 48 8B D1 4C 89 78 C8 45 33 ED 4C 8B F9 4C 89 6C 24 ?";//
    constexpr const char* NetDebug = "40 55 56 41 56 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 8B 01";
    constexpr const char* FixCrash = "40 55 56 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 48 8D 6C 24 ? 48 89 9D ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C5 48 89 85 ? ? ? ? 8B 41 0C 45 33 F6";
    constexpr const char* SetReplicationDriver = "40 56 41 56 48 83 EC 28 48 8B F1 4C 8B F2 48 8B 89 ? ? ? ? 48 85 C9 0F 84 ? ? ? ? 48 8B 01 48 89 5C 24 ? 48 89 6C 24 ? 4C 89 7C 24 ? FF 90 ? ? ? ? 48 8B 9E ? ? ? ? 45 33 FF";
    constexpr const char* ValidationFailure = "48 89 4C 24 ? 55 53 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 33 DB 48 8B F2";
    constexpr const char* CreateNetDriver_Local = "4C 89 44 24 ? 53 56 57 41 56 41 57 48 83 EC 70 48 8B B9 ? ? ? ? 48 8D 1D ? ? ? ? 48 63 81 ? ? ? ? 45 33 FF 4C 8B F2 48 8D 0C 40 4C 8D 0C CF 49 3B F9 74 22 44 8B 94 24 ? ? ? ? 44 39 57 04 0F 94 C1 44 39 07 0F 94 C0";
    constexpr const char* InitListen = "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 50 48 8B BC 24 ? ? ? ? 49 8B F0";
    constexpr const char* CreateNetDriver = "48 89 5C 24 08 57 48 83 EC ? 49 8B D8 48 8B F9 E8 ? ? ? ? 48 8B D0";
    constexpr const char* SetWorld = "48 89 5C 24 ? 48 89 74 24 ? 55 57 41 56 48 8B EC 48 83 EC 30 48 8B 99 ? ? ? ? 45 33 F6 48 8B F2 48 8B F9 48 85 DB 0F 84 ? ? ? ? 48 8B 97 ? ? ? ? 48 8D 8B ? ? ? ? E8 ? ? ? ? 48 8B 97 ? ? ? ? 48 8D 8B ? ? ? ? E8 ? ? ? ? 48 8B 97 ? ? ? ?";
    constexpr const char* StaticFindObject = "48 89 5C 24 ? 48 89 74 24 ? 55 57 41 54 41 56 41 57 48 8B EC 48 83 EC 60 80 3D ? ? ? ? ? 45 0F B6 F1 49 8B F8 48 8B DA 4C 8B F9 74 2F 48 8D 05 ? ? ? ? 41 B9 ? ? ? ? 4C 8D 05 ? ? ? ? 48 89 44 24 ? BA ? ? ? ? 48 8D 0D";
    constexpr const char* GetPlayerViewpoint = "40 55 56 57 41 57 48 8B EC 48 83 EC 48 48 8B 81 ? ? ? ? 4D 8B F8 48 8B F2 48 8B F9 48 85 C0 0F 84 ? ? ? ? F6 80 ? ? ? ? ? 0F 84 ? ? ? ? 48 8B 88 ? ? ? ? 48 85 C9 74 23 0F 10 89 ? ? ? ? 0F 28 C1 F3 0F 11 4D ? 0F C6 C1 55 0F C6 C9 AA";

    // paddin (aka patterns which arent right but we ned them to build)

    constexpr const char* GetNetMode = "48 89 5C 24 08 57 48 83 EC ? 48 8B 01 48 8B D9 FF 90 40 01 00 00 4C 8B 83 10 01 00 00";//
    constexpr const char* Realloc = "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC ? 48 8B F1 41 8B D8 48 8B 0D";//
    constexpr const char* NoReserve = "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 56 41 57 48 81 EC ? ? ? ? 48 8B 01 49 8B D9 45 0F B6 F0 48 8B F2 48 8B F9 45 32 FF FF 90 ? ? ? ? 48 8B 0D ? ? ? ?";
    constexpr const char* ClientTravel = "48 83 EC 48 45 84 C9 74 0D 41 83 F8 02 75 07 66 FF 81 ? ? ? ? 48 8B 44 24 ? 0F 10 00";
    constexpr const char* HasClientLoadedCurrentWorld = "48 89 5C 24 ? 57 48 83 EC 20 48 8B 99 ? ? ? ? 48 8B F9 48 85 DB 74 23 E8 ? ? ? ? 48 8B 53 10 4C 8D 40 30 48 63 40 38 3B 42 38 7F 0D 48 8B C8 48 8B 42 30 4C 39 04 C8 74 17 48 8B 1D ? ? ? ? 48 85 DB";
    constexpr const char* malformed = "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 50 40 32 F6 0F 29 7C 24 ? 48 8B FA 44 0F 29 44 24 ? 48 8B D9 40 38 72 28 7C 51 4C 8B 92 ? ? ? ? 4C 3B 92 ? ? ? ? 7C 0F";
    constexpr const char* GiveAbility = "48 89 5C 24 ? 48 89 6C 24 ? 48 89 7C 24 ? 41 56 48 83 EC 20 83 B9 ? ? ? ? ? 49 8B E8 4C 8B F2 48 8B F9 7E 56 48 63 9F ? ? ? ? 48 81 C7 ? ? ? ? 8D 43 01 89 87 ? ? ? ? 3B 87 ? ? ? ? 7E";
    constexpr const char* InternalTryActivateAbility = "4C 89 4C 24 20 4C 89 44 24 18 89 54 24 10 55 53 56 57 41 54";
    constexpr const char* MarkAbilitySpecDirty = "48 89 5C 24 ? 48 89 7C 24 ? 41 56 48 83 EC 20 48 8B 01 41 0F B6 D8 4C 8B F2 48 8B F9 FF 90 ? ? ? ? 84 C0 0F 84 ? ? ? ? 49 8B 46 10 48 89 6C 24 ?";
    constexpr const char* CollectGarbage = "48 89 5C 24 08 48 89 6C 24 10 56 57 41 56 48 81 EC ? ? ? ? 4C 8B 05"; // NON ESISTE PIU'
#endif
}