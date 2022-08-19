#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <iostream>

#include <Net/nethooks.h>
#include <MinHook/MinHook.h>

#include "patterns.h"
#include "gui.h"
#include <hooks.h>
#include <Gameplay/abilities.h>
#include <Gameplay/build.h>
#include <Gameplay/harvesting.h>

DWORD WINAPI InputThread(LPVOID)
{
    while (1)
    {
        if (GetAsyncKeyState(VK_F5) & 1)
        {
            LoadInMatch();
        }

        else if (GetAsyncKeyState(VK_F6) & 1)
        {
            initStuff();
        }

        else if (GetAsyncKeyState(VK_F7) & 1)
        {
            // if (MyPawn)
            {
                // Helper::TeleportTo(MyPawn, Helper::GetPlayerStart());
            }
        }

        else if (GetAsyncKeyState(VK_F8) & 1)
        {
            Listen(7777);
        }

        else if (GetAsyncKeyState(VK_F9) & 1)
        {
            InitializeNetHooks();

            std::cout << ("Initialized NetHooks!\n");
        }

        Sleep(1000 / 30);
    }

    return 0;
}

UObject* __fastcall SpawnsAircraftDetour(__int64 a1, int a2)
{
    std::cout << "Spawns aircraft called!\n";
    static auto aircraftClass = FindObject("BlueprintGeneratedClass /Game/Athena/Aircraft/AthenaAircraft.AthenaAircraft_C");
    auto Aircraft = Easy::SpawnActor(aircraftClass, AircraftLocationToUse);

    auto World = Helper::GetWorld();
    if (World)
    {
        auto NetDriver = *World->Member<UObject*>(("NetDriver"));
        if (NetDriver)
        {
            auto ClientConnections = NetDriver->Member<TArray<UObject*>>(("ClientConnections"));

            if (ClientConnections)
            {
                for (int i = 0; i < ClientConnections->Num(); i++)
                {
                    auto Connection = ClientConnections->At(i);

                    if (!Connection)
                        continue;

                    auto Controller = *Connection->Member<UObject*>(("PlayerController"));

                    if (Controller)
                    {
                        struct
                        {
                        public:
                            UObject* A;                                                 // 0x0(0x8)(Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
                            char          TransitionParams[0x10];                                  // 0x8(0x10)(Parm, NoDestructor, NativeAccessSpecifierPublic)
                        }APlayerController_ClientSetViewTarget_Params{ Aircraft };

                        static auto ClientSetViewTarget = Controller->Function("ClientSetViewTarget");

                        if (ClientSetViewTarget)
                            Controller->ProcessEvent(ClientSetViewTarget, &APlayerController_ClientSetViewTarget_Params);
                        else
                            std::cout << "No ClientSetViewTarget!\n";
                    }
                }
            }
        }
    }

    return Aircraft;
}

char __fastcall crashmaybDetour(__int64 a1)
{
    return true;
}

__int64(__fastcall* Crashes)(__int64 a1, __int64 a2);

__int64 __fastcall CrashesDetour(__int64 a1, __int64 a2)
{
    std::cout << "Crashes called!\n";

    /*
    auto ClassPrivateMaybe = *(__int64*)(a1 + 16);

    if (!(__int64*)(ClassPrivateMaybe + 0x88))
        return 0;

    return Crashes(a1, a2);
    */

    return 0;
}

DWORD WINAPI Main(LPVOID)
{
    AllocConsole();

    FILE* fptr;
    freopen_s(&fptr, ("CONOUT$"), ("w"), stdout);

    auto stat = MH_Initialize();

    if (stat != MH_OK)
    {
        std::cout << std::format("Failed to initialize MinHook! Error: {}\n", MH_StatusToString(stat));
        return 1;
    }

    if (!Setup())
    {
        std::cout << ("Failed setup!\n");
        return 1;
    }

    if (Engine_Version >= 425)
    {
        RequestExitWSAddr = FindPattern(RequestExitWSSig);

        if (!RequestExitWSAddr)
        {
            if (Engine_Version >= 426)
                std::cout << ("[WARNING] Could not find RequestExitWithStatus, game will probably close!\n");
        }
        else
        {
            CheckPattern(("RequestExitWithStatus"), RequestExitWSAddr, &RequestExitWithStatus);

            if (RequestExitWSAddr)
            {
                MH_CreateHook((PVOID)RequestExitWSAddr, RequestExitWithStatusHook, (void**)&RequestExitWithStatus);
                MH_EnableHook((PVOID)RequestExitWSAddr);
            }
        }
    }

    InitializePatterns();

    std::cout << ("Initialized Patterns!\n");

    InitializeNetUHooks();

    if (GiveAbilityAddr)
        InitializeAbilityHooks();

    InitializeInventoryHooks();
    InitializeBuildHooks();
    InitializeHarvestingHooks();

    FinishInitializeUHooks();

    InitializeHooks();

    CreateThread(0, 0, InputThread, 0, 0, 0);
    CreateThread(0, 0, GuiThread, 0, 0, 0);
    CreateThread(0, 0, Helper::Console::Setup , 0, 0, 0);
#ifndef DPP_DISABLED
    CreateThread(0, 0, BotThread, 0, 0, 0);
#endif

    if (FnVerDouble < 18.00)
        Looting::Tables::Init(nullptr);

    SetConsoleTitleA(("Project Reboot Server"));
    std::cout << ("Found all loot!\n");

    if (Engine_Version < 422)
        std::cout << ("Press play button to host!\n");
    else
        std::cout << ("Press F5 to host!\n");

    std::cout << dye::aqua(("[Base Address] ")) << std::format("0x{:x}\n", (uintptr_t)GetModuleHandleW(0));
    std::cout << dye::green(("[ServerReplicateActors] ")) << std::format("0x{:x}\n", ServerReplicateActorsOffset);
    
    std::cout << dye::blue(("[DEBUG] ")) << std::format("Size of Entry: 0x{:x}.\n", GetEntrySize());
    std::cout << dye::blue(("[DEBUG] ")) << std::format("ReplicatedEntries Offset: 0x{:x}.\n", FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortItemList"), ("ReplicatedEntries")));
    std::cout << dye::blue(("[DEBUG] ")) << std::format("ItemInstances Offset: 0x{:x}.\n", FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortItemList"), ("ItemInstances")));

    TestAbilitySizeDifference();

    // std::cout << "FUnny offset: " << FindOffsetStruct("ScriptStruct /Script/Engine.FastArraySerializer", "DeltaFlags") << '\n'; // 256 12.41
    // std::cout << "FUnny offset: " << FindOffsetStruct("ScriptStruct /Script/Engine.FastArraySerializer", "ArrayReplicationKey") << '\n'; // 84 12.41

    /* uint8_t* BusCrashAddr = (uint8_t*)FindPattern("0F 10 4C D1 ? 0F 11 88 ? ? ? ? F2 0F 10 44 D1 ? 48 8D 88 ? ? ? ? 48 8D 54 24 ? F2 0F 11 80 ? ? ? ? E8 ? ? ? ? 0F 28 44 24 ? 48 8D 54 24 ? 0F 29 44 24 ? 0F 57 C9 F3 0F 10 83 ? ? ? ?");

    for (int i = 0; i < 11; i++)
    {
        BusCrashAddr[i] = 0;
    } */

    /* uintptr_t BusCrashAddr = FindPattern("0F 10 4C D1 ? 0F 11 88 ? ? ? ? F2 0F 10 44 D1 ? 48 8D 88 ? ? ? ? 48 8D 54 24 ? F2 0F 11 80 ? ? ? ? E8 ? ? ? ? 0F 28 44 24 ? 48 8D 54 24 ? 0F 29 44 24 ? 0F 57 C9 F3 0F 10 83 ? ? ? ?");
    
    if (BusCrashAddr) {
        std::cout << "Applying bus crash fix!\n";

        for (int i = 0; i < 11; i++)
        {
            *reinterpret_cast<char*>(BusCrashAddr + i) = 0x00;
        }
    }
    else
        std::cout << "No BusCrashAddr!\n"; */

    /* auto SpawnAircraftAddr = FindPattern("48 89 74 24 ? 57 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 ? ? ? ? 48 8B B9 ? ? ? ? 48 63 F2 48 85 FF 74 2D E8 ? ? ? ? 4C 8B 47 10 4C 8D 48 30 48 63 40 38 41 3B 40 38 7F 16 48 8B C8 49 8B 40 30");

    if (SpawnAircraftAddr)
    {
        MH_CreateHook((PVOID)SpawnAircraftAddr, SpawnsAircraftDetour, nullptr);
        MH_EnableHook((PVOID)SpawnAircraftAddr);
    }
    else
        std::cout << "No SpawnAircraftAddr!\n"; */

    /* auto crashAddr = FindPattern("48 89 5C 24 ? 48 89 7C 24 ? 4C 89 74 24 ? 55 48 8B EC 48 83 EC 30 48 8B 79 10 4C 8B C1 48 83 BF ? ? ? ? ? 74 15 80 3D ? ? ? ? ? 72 05 E8 ? ? ? ? 33 C0");

    if (crashAddr)
    {
        Crashes = decltype(Crashes)(crashAddr);
        MH_CreateHook((PVOID)crashAddr, CrashesDetour, (PVOID*)&Crashes);
        MH_EnableHook((PVOID)crashAddr);
    }
    else
        std::cout << "No crashAddr!\n"; */

    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(0, 0, Main, 0, 0, 0);
        break;
    case DLL_PROCESS_DETACH:
        std::cout << ("Disabling all Hooks!");
        MH_DisableHook(MH_ALL_HOOKS);
        SendDiscordEnd();
        break;
    }
    return TRUE;
}

