#include <Windows.h>
#include <iostream>

#include <Net/nethooks.h>
#include <MinHook/MinHook.h>

#include "patterns.h"
#include <hooks.h>

void InitializePatterns()
{
    static auto ReallocAddr = FindPattern(Patterns::Realloc);
    CheckPattern(_("FMemory::Realloc"), ReallocAddr, &FMemory::Realloc);
    GetNetModeAddr = FindPattern(Patterns::GetNetMode);
    GetNetMode = decltype(GetNetMode)(GetNetModeAddr);
    LP_SpawnPlayActorAddr = FindPattern(Patterns::LocalPlayerSpawnPlayActor);
    LP_SpawnPlayActor = decltype(LP_SpawnPlayActor)(LP_SpawnPlayActorAddr);
    CollectGarbage = decltype(CollectGarbage)(FindPattern(Patterns::CollectGarbage));
    SetWorld = decltype(SetWorld)(FindPattern(Patterns::SetWorld));
    InitListen = decltype(InitListen)(FindPattern(Patterns::InitListen));
    TickFlushAddr = FindPattern(Patterns::TickFlush);
    TickFlush = decltype(TickFlush)(TickFlushAddr);
    ReceiveFString = decltype(ReceiveFString)(FindPattern(Patterns::ReceiveFString));
    ReceiveUniqueIdRepl = decltype(ReceiveUniqueIdRepl)(FindPattern(Patterns::ReceiveUniqueIdRepl));
    WelcomePlayer = decltype(WelcomePlayer)(FindPattern(Patterns::WelcomePlayer));
    World_NotifyControlMessageAddr = FindPattern(Patterns::World_NotifyControlMessage);
    World_NotifyControlMessage = decltype(World_NotifyControlMessage)(World_NotifyControlMessageAddr);
    SpawnPlayActorAddr = FindPattern(Patterns::SpawnPlayActor);
    SpawnPlayActor = decltype(SpawnPlayActor)(SpawnPlayActorAddr);
    InitHost = decltype(InitHost)(FindPattern(Patterns::InitHost));
    Beacon_NotifyControlMessageAddr = FindPattern(Patterns::Beacon_NotifyControlMessage);
    Beacon_NotifyControlMessage = decltype(Beacon_NotifyControlMessage)(Beacon_NotifyControlMessageAddr);
    PauseBeaconRequests = decltype(PauseBeaconRequests)(FindPattern(Patterns::PauseBeaconRequests));
    NetDebug = decltype(NetDebug)(FindPattern(Patterns::NetDebug));
    KickPlayerAddr = FindPattern(Patterns::KickPlayer);
    KickPlayer = decltype(KickPlayer)(KickPlayerAddr);
}

DWORD WINAPI Input(LPVOID)
{
    while (1)
    {
        if (GetAsyncKeyState(VK_F6) & 1)
        {
            initStuff();
        }

        Sleep(1000 / 30);
    }

    return 0;
}

DWORD WINAPI Main(LPVOID)
{
    AllocConsole();

    FILE* fptr;
    freopen_s(&fptr, _("CONOUT$"), _("w"), stdout);

    auto stat = MH_Initialize();

    if (stat != MH_OK)
    {
        std::cout << std::format(_("Failed to initialize MinHook! Error: {}\n"), MH_StatusToString(stat));
        return 1;
    }

    if (!Setup())
    {
        std::cout << _("Failed setup!\n");
        return 1;
    }

    InitializePatterns();

    std::cout << _("Initialized Patterns!\n");

    InitializeNetUHooks();
    FinishInitializeHooks();

    MH_CreateHook((PVOID)ProcessEventAddr,  ProcessEventDetour, (void**)&ProcessEventO);
    MH_EnableHook((PVOID)ProcessEventAddr);

    CreateThread(0, 0, Input, 0, 0, 0);

    std::cout << _("Press play button to host!\n");

    std::cout << dye::aqua("[Base Address] ") << std::format("0x{:x}\n", (uintptr_t)GetModuleHandleW(0));

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
        std::cout << _("Disabling all Hooks!");
        MH_DisableHook(MH_ALL_HOOKS); // Untested
        break;
    }
    return TRUE;
}

