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
            auto AuthGameMode = *Helper::GetWorld()->Member<UObject*>(("AuthorityGameMode"));
            auto bEnableReplicationGraph = AuthGameMode->Member<bool>("bEnableReplicationGraph");

            if (bEnableReplicationGraph)
                *bEnableReplicationGraph = true;

            Listen(7777);
        }

        else if (GetAsyncKeyState(VK_F9) & 1)
        {
            InitializeNetHooks();

            std::cout << ("Initialized NetHooks!\n");
        }

        else if (GetAsyncKeyState(VK_F10) & 1)
        {
            bStarted = false;
            bTraveled = false;
            bIsReadyToRestart = true;

            initStuff();

            std::cout << ("Initialized adf!\n");
        }

        Sleep(1000 / 30);
    }

    return 0;
}

char __fastcall crashmaybDetour(__int64 a1)
{
    return true;
}

__int64(__fastcall* Crashes)(__int64 a1, __int64* a2, char a3);

__int64 __fastcall CrashesDetour(__int64 a1, __int64* a2, char a3) 
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

void(__fastcall* buscrash2O)(__int64 a1, __int64 a2, unsigned int a3);

void __fastcall buscrash2Detour(__int64 a1, __int64 a2, unsigned int a3)
{
    auto v3 = *(__int64*)(a1 + 80);
    auto v6 = (char*)(v3 + 173);

    if (!v6)
    {
        std::cout << "Prevented bus crash 2!\n";
        return;
    }

    return buscrash2O(a1, a2, a3);
}

__int64(__fastcall* hookthis)(__int64 a1);

__int64 __fastcall crashTFDetour(__int64 a1) // 3.5
{
    std::cout << "crashtf!\n";

    // if (!(__int64*)(*(__int64*)(a1 + 544) + 0x4B0))
    return 0;

    auto result = *(__int64*)(a1 + 528);
    *(__int64*)(*(__int64*)(a1 + 544) + 0x4B0) = result;

    return result;
}

char __fastcall crash2Detour(__int64 a1, __int64 a2) // 3.5
{
    // std::cout << "crash2!\n";
    return true;
}

__int64 __fastcall hookthisDetour(__int64 a1)
{
    std::cout << "hookthis!\n";
    return 0;

    auto result = (*(__int64(__fastcall**)(__int64))(*(__int64*)a1 + 320))(a1);
    auto v3 = result;
    auto v8 = *(__int64*)(*(__int64*)(v3 + 56) + 136);

    if (!(int*)(v8 + 0x484))
        return result;

    return hookthis(a1);
}

char __fastcall CRASHMFDetour(__int64 a1, __int64 a2, __int64 a3, char* a4)
{
    std::cout << "stupid crash!\n";
    return false;
}

__int64 (*idkO)(UObject* BuildingActor);

__int64 idkDetour(UObject* BuildingActor)
{
    std::cout << "A1: " << BuildingActor << '\n';

    if (BuildingActor)
        std::cout << "A1 Name: " << BuildingActor->GetFullName() << '\n';

    return idkO(BuildingActor);
}

__int64 __fastcall duplicateplayerdetour(__int64* a1, __int64 a2) 
{
    std::cout << "duplicate!\n";
    return 0;
}

void (__fastcall* sub_7FF700EF5E20O)(__int64 a1, int a2);

void __fastcall sub_7FF700EF5E20Detour(__int64 a1, int a2)
{
    auto v5 = (__int64*)(a1 + 256);

    if (!v5)
        return;

    return sub_7FF700EF5E20O(a1, a2);
}

void(__fastcall* CALLMEIMCOOLO)(UObject* GameMode, __int64 a2, char setmetofalse, char a4);

void __fastcall CALLMEIMCOOLDETOUR(UObject* GameMode, __int64 a2, char setmetofalse, char a4)
{
    std::cout << "GameMode: " << GameMode << '\n';
    std::cout << "a2: " << a2 << '\n';
    std::cout << "setmetofalse: " << setmetofalse << '\n';
    std::cout << "a4: " << a4 << '\n';

    return CALLMEIMCOOLO(GameMode, a2, false, a4);
}

char(__fastcall* stu8pduficnO)(__int64 a1, __int64 a2, __int64 a3, bool* a4);

char __fastcall stu8pduficnDetour(__int64 a1, __int64 a2, __int64 a3, bool* a4)
{
    std::cout << "ur skiddeD!\n";

    return true;
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

    std::cout << "f: " << FnVerDouble << '\n';
    std::cout << "e: " << Engine_Version << '\n';

    /* static auto GameViewportOffset = GetOffset(FindObjectOld("FortEngine_"), "GameViewport");
    auto GameViewport = *(UObject**)(__int64(FindObjectOld("FortEngine_")) + GameViewportOffset);

    bInjectedOnStartup = !GameViewport;

    if (bInjectedOnStartup)
    {
        while (true)
        {
            if (GameViewport = *(UObject**)(__int64(FindObjectOld("FortEngine_")) + GameViewportOffset))
            {
                std::cout << "Found Viewport!\n";
                Sleep(6000);
                break;
            }

            Sleep(1000);
        }
    }
    else
        std::cout << "DID NOT INJECT ON STARTUP!\n"; */

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
    // InitializeHarvestingHooks();

    FinishInitializeUHooks();

    InitializeHooks();

    // CreateThread(0, 0, InputThread, 0, 0, 0);
    CreateThread(0, 0, GuiThread, 0, 0, 0);
    CreateThread(0, 0, Helper::Console::Setup , 0, 0, 0);
#ifndef DPP_DISABLED
    CreateThread(0, 0, BotThread, 0, 0, 0);
#endif

    SetConsoleTitleA(("Project Reboot Server"));

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

    std::cout << "StaticLoadObject: " << StaticLoadObjectO << '\n';
    std::cout << "SetWorld: " << SetWorld << '\n';
    std::cout << "ReplicateActor: " << ReplicateActor << '\n';
    std::cout << "SetChannelActor: " << SetChannelActor << '\n';
    std::cout << "CreateChannel: " << CreateChannel << '\n';
    std::cout << "SendClientAdjustment: " << SendClientAdjustment << '\n';
    std::cout << "KickPlayer: " << KickPlayer << '\n';
    std::cout << "GiveAbilityOLDDD: " << GiveAbilityOLDDD << '\n';
    std::cout << "LP_SpawnPlayActorAddr: " << LP_SpawnPlayActorAddr << '\n';

    if (FnVerDouble == 3.5)
    {
        static auto ahhhaddr = FindPattern("48 8B 91 ? ? ? ? 48 8B 81 ? ? ? ? 48 89 82 ? ? ? ? C3");

        MH_CreateHook((PVOID)ahhhaddr, crashTFDetour, nullptr);
        MH_EnableHook((PVOID)ahhhaddr);

        static auto heloaddr = FindPattern("48 8B C4 57 48 81 EC ? ? ? ? 4C 8B 82 ? ? ? ? 48 8B F9 0F 29 70 E8 0F 29 78 D8 44 0F 29 40 ? F3 45 0F 10 80 ? ? ? ? F3 41 0F 10 B0 ? ? ? ? F3");

        MH_CreateHook((PVOID)heloaddr, crash2Detour, nullptr);
        MH_EnableHook((PVOID)heloaddr);
    }

    static auto dupciateplayetraddy = FindPattern("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 70 48 8B 01 48 8B DA 48 8B F9 FF 90 ? ? ? ? 48 8B 53 08 48 8B C8 E8");

    std::cout << "dupciateplayetraddy: " << dupciateplayetraddy << '\n';

    if (FnVerDouble == 6.21 || Engine_Version <= 419)
    {
        MH_CreateHook((PVOID)dupciateplayetraddy, duplicateplayerdetour, nullptr); // stupid
        MH_EnableHook((PVOID)dupciateplayetraddy);
    }

    if (FnVerDouble == 7.30)
    {
        static auto hookthisaddr = FindPattern("48 89 5C 24 ? 57 48 83 EC 30 48 8B 01 48 8B F9 FF 90 ? ? ? ? 48 8B D8 48 85 C0 0F 84 ? ? ? ? 48 83 78 ? ?");

        MH_CreateHook((PVOID)hookthisaddr, hookthisDetour, (PVOID*)&hookthis);
        MH_EnableHook((PVOID)hookthisaddr);
    }

    if (FnVerDouble >= 19.00)
    {
        static auto craswhiffkaddr = FindPattern("40 55 41 54 41 55 41 56 41 57 48 83 EC 60 48 8D 6C 24 ? 48 89 5D 60 48 89 75 68 48 89 7D 70 48 8B 05 ? ? ? ? 48 33 C5 48 89 45 28 33 F6 4C 89 4D 18 41");
     
        MH_CreateHook((PVOID)craswhiffkaddr, CRASHMFDetour, nullptr);
        MH_EnableHook((PVOID)craswhiffkaddr);
    }

    NoMcpAddr = FindPattern("E8 ? ? ? ? 84 C0 75 CE", true, 1);

    if (Engine_Version >= 424)
        NoMcpAddr = FindPattern("E8 ? ? ? ? 84 C0 75 C1", true, 1);

    if (Engine_Version <= 420)
        NoMcpAddr = FindPattern("E8 ? ? ? ? 90 EB E7", true, 0x6E);

    if (!NoMcpAddr)
        NoMcpAddr = FindPattern("E8 ? ? ? ? 84 C0 75 C0", true, 1); // 10.40

    if (!NoMcpAddr)
        NoMcpAddr = FindPattern("E8 ? ? ? ? 90 EB E1", true, 1); // 2.4.2

    // 48 83 EC 28 65 48 8B 04 25 ? ? ? ? 8B 0D ? ? ? ? BA ? ? ? ? 48 8B 0C C8 8B 04 0A 39 05 ? ? ? ? 7F 0C 0F B6 05 ? ? ? ? 48 83 C4 28 C3 48 8D 0D ? ? ? ? E8 ? ? ? ? 83 3D ? ? ? ? ? 75 DF E8 ? ? ? ? 48 8B C8 48 8D 15 ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? 88 05 ? ? ? ? E8 ? ? ? ? EB B7

    funnyaddry = FindPattern("40 53 48 81 EC ? ? ? ? 48 83 79 ? ? 48 8B D9 74 0E B8 ? ? ? ? 48 81 C4 ? ? ? ? 5B C3 48 8B 89 ? ? ? ? 48 85 C9 74 0D 48 81 C4 ? ? ? ? 5B E9 ? ? ? ? 48 8B 0D ? ? ? ?");

    if (!funnyaddry)
        funnyaddry = FindPattern("48 89 5C 24 ? 57 48 81 EC ? ? ? ? 48 8B D9 48 8B 0D ? ? ? ? 48 85 C9 0F 84 ? ? ? ? 48 8B D3 E8 ? ? ? ? 48");

    constexpr bool idkai3 = false;

    if (idkai3 && !funnyaddry)
        funnyaddry = FindPattern("48 83 79 ? ? 75 19 48 8B 81 ? ? ? ? 48 85 C0 74 08 48 8B C8 E9 ? ? ? ? E9 ? ? ? ? B8"); // 19.10 */

    std::cout << "funnyaddry: " << funnyaddry << '\n';
    std::cout << "NoMcpAddr: " << NoMcpAddr << '\n';

    bool bafuaqeu = false;

    if (NoMcpAddr && funnyaddry)
    {
        if (Engine_Version < 426) // IsNoMCP leads to wrong addr (atleast for S13)
        {
            MH_CreateHook((PVOID)NoMcpAddr, IsNoMCPDetour, nullptr);
            MH_EnableHook((PVOID)NoMcpAddr);

            MH_CreateHook((PVOID)funnyaddry, getnetmodedetour, (void**)&getnetmodeO);
            MH_EnableHook((PVOID)funnyaddry);

            bafuaqeu = true;
        }
    }
    
    if (!bafuaqeu)
    {
        std::cout << "[WARNING] Will not be able to apply magical fix!\n";
        NoMcpAddr = 0;
        funnyaddry = 0;
    }

    {
        MH_CreateHook((PVOID)SpawnActorAddr, SpawnActorDetour, (void**)&SpawnActorO);
        MH_EnableHook((PVOID)SpawnActorAddr);
    }

    if (idkai3)
    {
        auto CallsReinitializeALlProfilesAddr = FindPattern("");

        MH_CreateHook((PVOID)CallsReinitializeALlProfilesAddr, CallsReinitializeALlProfilesDetour, nullptr);
        MH_EnableHook((PVOID)CallsReinitializeALlProfilesAddr);
    }

   /* auto agiuigf1 = FindPattern("48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 41 56 48 83 EC 30 48 8B B1 ? ? ? ? 41 0F B6 E9 45 0F B6 F0 48 8B FA");

    if (!agiuigf1)
        agiuigf1 = FindPattern("48 89 5C 24 ? 48 89 74 24 ? 57 41 54 41 57 48 83 EC 30 48 89 6C 24 ? 33 DB 48 8B A9 ? ? ? ? 45 0F B6 F9 45 0F B6 E0 48 8B F2 48 8B");

    std::cout << "agiuigf1: " << agiuigf1 << '\n';

    MH_CreateHook((PVOID)agiuigf1, CALLMEIMCOOLDETOUR, (void**)&CALLMEIMCOOLO);
    MH_EnableHook((PVOID)agiuigf1); */

    if (FnVerDouble == 7.40 && bEmotingEnabled)
    {
        auto addy35125 = FindObject("FortAbilitySystemComponentAthena /Script/FortniteGame.Default__FortAbilitySystemComponentAthena")->VFTable[243];

        CreateNewInstanceOfAbilityO = decltype(CreateNewInstanceOfAbilityO)(addy35125);

        MH_CreateHook((PVOID)addy35125, CreateNewInstanceOfAbilityDetour, (void**)&CreateNewInstanceOfAbilityO);
        MH_EnableHook((PVOID)addy35125);
    }

    // auto Mappings = FindObject("FortDeathCauseFromTagMapping /Game/Balance/DeathTagToEnumMapping.DeathTagToEnumMapping");

    auto CanBuildAddr = FindPattern("48 89 5C 24 10 48 89 6C 24 18 48 89 74 24 20 41 56 48 83 EC ? 49 8B E9 4D 8B F0");

    if (!CanBuildAddr)
        CanBuildAddr = FindPattern("48 89 54 24 ? 55 56 41 56 48 83 EC 50");

    if (!CanBuildAddr)
        CanBuildAddr = FindPattern("E8 ? ? ? ? 85 C0 0F 85 ? ? ? ? 80 65 28 F0", true, 1); // S19

    if (Engine_Version != 426)
        CanBuild = decltype(CanBuild)(CanBuildAddr);

    std::cout << "CanBuildAddr: " << CanBuildAddr << '\n';

    if (false) // 21.00
    {
        auto CreatereplicationadfiAd = FindPattern("48 89 5C 24 ? 48 89 6C 24 ? 56 57 41 54 41 56 41 57 48 83 EC 40 49 8B F8 48 8B F2 4C 8B F9 E8 ? ? ? ? 33 DB 39 58 08 74 36 48 8B 08");

        MH_CreateHook((PVOID)CreatereplicationadfiAd, CreateReplicationDriverDetour, nullptr);
        MH_EnableHook((PVOID)CreatereplicationadfiAd);

        auto logdfunc = FindPattern("83 7A 08 00 4C 8D 05 ? ? ? ? 74 05 4C 8B 0A EB 03 4D 8B C8 83 79 18 00 74 04 4C 8B 41 10 48 8B 09 48 8D 15 ? ? ? ? E9 ? ? ? ?");

        MH_CreateHook((PVOID)logdfunc, FReplicationGraphDebugInfo_LogDetour, nullptr);
        MH_EnableHook((PVOID)logdfunc);
    }

    if (bEmotingEnabled && false && FnVerDouble == 7.40)
    {
        auto getewaoe = FindPattern("44 8B 41 04 45 85 C0 74 4E 8B 01 85 C0 78 48 3B 05 ? ? ? ? 7D 40 99 0F B7 D2 03 C2 8B C8 0F B7 C0 2B C2 C1 F9 10 48 98 48 63 C9 48 8D");

        MH_CreateHook((PVOID)getewaoe, FWeakObjectPtr_GetDetour, (PVOID*)&FWeakObjectPtr_GetO);
        MH_EnableHook((PVOID)getewaoe);

        auto getewaoe2 = FindPattern("44 8B 49 04 44 0F B6 D2 45 85 C9 74 63 8B 01 85 C0 78 5D 3B 05 ? ? ? ? 7D 55 99 0F B7 D2 03 C2 8B C8 0F B7 C0 2B C2 C1 F9 10 48 98 48");

        MH_CreateHook((PVOID)getewaoe2, FWeakObjectPtr_Get2Detour, (PVOID*)&FWeakObjectPtr_Get2O);
        MH_EnableHook((PVOID)getewaoe2);
    }

    GlobalPickaxeDefObject = FindObject(PickaxeDef);
    bRandomCosmetics = !(std::floor(FnVerDouble) <= 4);

    if (false)
    {
        if (FnVerDouble == 19.10)
        {
            auto sigafinn = FindPattern("0F B6 CA B0 01");

            std::cout << "sigafinn: " << sigafinn << '\n';

            MH_CreateHook((PVOID)sigafinn, ShouldStartZoneOrSomethingDetour, (PVOID*)&ShouldStartZoneOrSomethingO);
            MH_EnableHook((PVOID)sigafinn);
        }
    }

    return 0;
}

DWORD WINAPI MainTest(LPVOID)
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

    auto Base = (uintptr_t)GetModuleHandleW(0);
    std::cout << dye::aqua(("[Base Address] ")) << std::format("0x{:x}\n", Base);

    std::cout << "Default__FortPlayerControllerAthena VTABLE: " << __int64(FindObject("FortPlayerControllerAthena /Script/FortniteGame.Default__FortPlayerControllerAthena")->VFTable) - Base << '\n';
    std::cout << "Default__FortPlayerPawnAthena VTABLE: " << __int64(FindObject("FortPlayerPawnAthena /Script/FortniteGame.Default__FortPlayerPawnAthena")->VFTable) - Base << '\n';
    std::cout << "Default__FortPlayerStateAthena VTABLE: " << __int64(FindObject("FortPlayerStateAthena /Script/FortniteGame.Default__FortPlayerStateAthena")->VFTable) - Base << '\n';
    std::cout << "Default__FortGameModeAthena VTABLE: " << __int64(FindObject("FortGameModeAthena /Script/FortniteGame.Default__FortGameModeAthena")->VFTable) - Base << '\n';
    std::cout << "Default__BuildingSMActor VTABLE: " << __int64(FindObject("BuildingSMActor /Script/FortniteGame.Default__BuildingSMActor")->VFTable) - Base << '\n';
    std::cout << "Default__FortWeapon VTABLE: " << __int64(FindObject("FortWeapon /Script/FortniteGame.Default__FortWeapon")->VFTable) - Base << '\n';
    std::cout << "Default__FortAmmoItemDefinition VTABLE: " << __int64(FindObject("FortAmmoItemDefinition /Script/FortniteGame.Default__FortAmmoItemDefinition")->VFTable) - Base << '\n';
    std::cout << "Default__FortPickup VTABLE: " << __int64(FindObject("FortPickup /Script/FortniteGame.Default__FortPickup")->VFTable) - Base << '\n';

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
        // CreateThread(0, 0, MainTest, 0, 0, 0);
        break;
    case DLL_PROCESS_DETACH:
        std::cout << ("Disabling all Hooks!");
        MH_DisableHook(MH_ALL_HOOKS);
        SendDiscordEnd();
        break;
    }
    return TRUE;
}

