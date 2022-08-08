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

        else if (GetAsyncKeyState(VK_F9) & 1)
        {
            InitializeNetHooks();

            std::cout << ("Initialized NetHooks!\n");
        }

        Sleep(1000 / 30);
    }

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
#ifdef T_F
    //For Server Only.
    uintptr_t CrashFix = FindPattern(Patterns::CrashPatch);
    if (CrashFix) {
        printf("Applying 4.5 Fix!\n");
        *reinterpret_cast<char*>(CrashFix) = 0xE9;
        *reinterpret_cast<char*>(CrashFix + 1) = 0x39;
        *reinterpret_cast<char*>(CrashFix + 2) = 0x02;
        *reinterpret_cast<char*>(CrashFix + 3) = 0x00;
        *reinterpret_cast<char*>(CrashFix + 4) = 0x00;
    }
#endif

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
        MH_DisableHook(MH_ALL_HOOKS); // Untested
        SendDiscordEnd();
        break;
    }
    return TRUE;
}

