#pragma once

#include <set>
#include <UE/structs.h>
#include <Gameplay/helper.h>

#include "inventory.h"

namespace Event
{
    static constexpr auto MeteorEvent = 3.0; // Exact build is unknown
    static constexpr auto RocketEvent = 4.5;
    static constexpr auto CubeVersion = 5.30;
    static constexpr auto ButterflyVersion = 6.21;
    static constexpr auto IceKingVersion = 7.20;
    static constexpr auto MarshmallowVersion = 7.30;
    static constexpr auto UnvaultVersion = 8.51;
    static constexpr auto FinalShutdownVersion = 9.40;
    static constexpr auto TheEndVersion = 10.40;
    static constexpr auto FirstDeviceEvent = 12.41;
    static constexpr auto SecondDeviceEvent = 12.61;
    static constexpr auto GalactusEvent = 24.0; // Exact build is unknown

    inline void ApplyToPlayers(const std::function<void(UObject*, int)>& Lambda)
    {
        const auto World = Helper::GetWorld();
        if (!World)
        {
            return;
        }

        const auto NetDriver = *World->Member<UObject*>("NetDriver");
        if (!NetDriver)
        {
            return;
        }


        const auto ClientConnections = NetDriver->Member<TArray<UObject*>>("ClientConnections");
        if (!ClientConnections)
        {
            return;
        }

        for (int i = 0; i < ClientConnections->Num(); i++)
        {
            const auto Connection = ClientConnections->At(i);
            if (!Connection)
            {
                continue;
            }

            const auto Controller = *Connection->Member<UObject*>("PlayerController");
            if (!Controller)
            {
                continue;
            }

            Lambda(Controller, i);
        }
    }

    namespace Butterfly
    {
        inline std::string ItemToUnvault = "DrumGun";

        // DOCUMENTATION: Teleports all players to the event
        inline void Start()
        {
            static auto scripting = FindObject(
                "BP_IslandScripting_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.BP_IslandScripting3",
                true);
            if (!scripting)
            {
                return;
            }

            const auto ButterflyBp = *scripting->Member<UObject*>("ButterflyBP");
            const auto Locations = ButterflyBp->Member<TArray<FTransform>>("PlayerLocations");

            ApplyToPlayers([Locations](UObject* Controller, auto Index)
            {
                const auto NewPawn = *Controller->Member<UObject*>("Pawn");
                Helper::SetActorLocation(NewPawn, Locations->At(Index).Translation);
            });
        }

        // DOCUMENTATION: Finishes the event by unvaulting the correct item
        inline void Finish(FName ItemName)
        {
            const auto Bss = FindObject(
                "BP_SnowScripting_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.BP_SnowScripting_2");
            const auto Func = Bss->Function("PillarsConcluded");
            Bss->ProcessEvent(Func, &ItemName);
        }
    }

    namespace Travis
    {
        inline void Start()
        {
            TArray<UObject*> Pawns;

            ApplyToPlayers([&Pawns](UObject* Controller, auto)
            {
                const auto NewPawn = *Controller->Member<UObject*>("Pawn");
                if (!NewPawn)
                {
                    return;
                }

                Pawns.Add(NewPawn);
            });

            static const auto JerkyBpLoader = FindObject(
                "BP_Jerky_Scripting_C /CycloneJerky/Levels/JerkySequenceMap_LevelInstance_1.JerkySequenceMap.PersistentLevel.BP_Jerky_Scripting_2");
            const auto Function = JerkyBpLoader->Function("PawnsAvailableFlyUp");
            const auto DisableGliders = JerkyBpLoader->Function("ToggleParachute");
            JerkyBpLoader->ProcessEvent(Function, &Pawns);
            auto Parameters = true;
            JerkyBpLoader->ProcessEvent(DisableGliders, &Parameters);
        }
    }

    namespace LootLake
    {
        // EXPERIMENTAL
        inline void ShowLake(const bool Show)
        {
            static auto FloatingIsland = FindObject(
                "LF_Athena_POI_15x15_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.LF_FloatingIsland");
            static auto Lake = FindObject(
                "LF_Athena_POI_75x75_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.LF_Lake1");
            static auto Lake2 = FindObject(
                "LF_Athena_POI_75x75_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.LF_Lake2");

            Helper::ShowBuilding(FloatingIsland, Show);
            Helper::ShowBuilding(Lake, Show);
            Helper::ShowBuilding(Lake2, !Show);

            // OnRep_ShowLakeRainbow
        }

        // EXPERIMENTAL
        inline void Start()
        {
            ShowLake(true);
        }

        // EXPERIMENTAL
        inline void Finish()
        {
            ShowLake(false);
        }
    }

    inline bool IsSupported()
    {
        const auto Set = std::set{
            MeteorEvent, RocketEvent, CubeVersion, ButterflyVersion, IceKingVersion, MarshmallowVersion, UnvaultVersion,
            FinalShutdownVersion, TheEndVersion, FirstDeviceEvent, SecondDeviceEvent, GalactusEvent
        };
        return Set.contains(FortniteVersion);
    }
    
    inline void Load()
    {
        if (FortniteVersion == CubeVersion)
        {
            static const auto Aec = FindObject(
                "BP_Athena_Event_Components_C /Game/Athena/Maps/Streaming/Athena_GameplayActors.Athena_GameplayActors.PersistentLevel.BP_Athena_Event_Components_54");

            const auto Func = Aec->Function("OnRep_CrackProgression");
            const auto Func2 = Aec->Function("OnRep_Corruption");

            *Aec->Member<float>("CrackOpacity") = 0.0f; // Hide the initial crack
            Aec->ProcessEvent(Func);
            *Aec->Member<float>("Corruption") = 1.0f; // Show the smaller purple crack
            Aec->ProcessEvent(Func2);
        }

        if (FortniteVersion == ButterflyVersion)
        {
            static const auto Bf = FindObject(
                "BP_Butterfly_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.BP_Butterfly_4");
            const auto Func = Bf->Function("LoadButterflySublevel");
            Bf->ProcessEvent(Func);

            static auto scripting = FindObjectOld(
                "BP_IslandScripting_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.BP_IslandScripting3",
                true);
            scripting->ProcessEvent("LoadDynamicLevels");

            scripting->ProcessEvent("OnRep_CachedTime");
            scripting->ProcessEvent("TrySetIslandLocation");
        }

        if (FortniteVersion == IceKingVersion)
        {
            static const auto Ml = FindObject(
                "BP_MooneyLoader_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.BP_MooneyLoader_2");
            const auto Func = Ml->Function("LoadSequence");
            const auto Func2 = Ml->Function("LoadMap");
            Ml->ProcessEvent(Func2);
            Ml->ProcessEvent(Func);
        }

        if (FortniteVersion == FinalShutdownVersion)
        {
            static const auto Cd = FindObject(
                "BP_CattusDoggus_Scripting_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.BP_CattusDoggus_Scripting_2");
            const auto Func = Cd->Function("LoadCattusLevel");
            auto Parameters = true;
            Cd->ProcessEvent(Func, &Parameters);
        }

        if (FortniteVersion == TheEndVersion)
        {
            static const auto Nn = FindObject(
                "BP_NightNight_Scripting_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.BP_NightNight_Scripting_2");
            const auto Func = Nn->Function("LoadNightNightLevel");
            auto Parameters = true;
            Nn->ProcessEvent(Func, &Parameters);
        }

        if (FortniteVersion == FirstDeviceEvent)
        {
            static const auto JerkyLevel = FindObject(
                "BP_Jerky_Loader_C /CycloneJerky/Levels/JerkyLoaderLevel.JerkyLoaderLevel.PersistentLevel.BP_Jerky_Loader_2");
            if (!JerkyLevel)
            {
                return;
            }

            const auto Func = JerkyLevel->Function("LoadJerkyLevel");
            auto Parameters = true;
            Func->ProcessEvent(Func, &Parameters);
        }

        if (FortniteVersion == SecondDeviceEvent)
        {
            static const auto FritterLoader = FindObject(
                "BP_Fritter_Loader_C /Fritter/Level/FritterLoaderLevel.FritterLoaderLevel.PersistentLevel.BP_Fritter_Loader_0");
            if (!FritterLoader)
            {
                return;
            }

            const auto LoadFritterLevel = FindObject(
                "Function /Fritter/BP_Fritter_Loader.BP_Fritter_Loader_C.LoadFritterLevel");
            auto Parameters = true;
            FritterLoader->ProcessEvent(LoadFritterLevel, &Parameters);
        }
    }

    inline void Start()
    {
        if (FortniteVersion == RocketEvent)
        {
            static const auto Lr = FindObject(
                "LevelSequencePlayer /Game/Athena/Maps/Test/Events/Athena_Gameplay_Geode.Athena_Gameplay_Geode.PersistentLevel.LevelSequence_LaunchRocket.AnimationPlayer");
            auto Func = Lr->Function("Play");
            Lr->ProcessEvent(Func);
            return;
        }

        if (FortniteVersion == CubeVersion)
        {
            static const auto Aec = FindObject(
                "BP_Athena_Event_Components_C /Game/Athena/Maps/Streaming/Athena_GameplayActors.Athena_GameplayActors.PersistentLevel.BP_Athena_Event_Components_54");
            const auto Func = Aec->Function("Final");
            Aec->ProcessEvent(Func);

            static const auto Cube =
                FindObject("CUBE_C /Game/Athena/Maps/Test/Level_CUBE.Level_CUBE.PersistentLevel.CUBE_2");
            const auto Func2 = Cube->Function("Final");
            Cube->ProcessEvent(Func2);
            return;
        }

        if (FortniteVersion == ButterflyVersion)
        {
            static const auto Bf = FindObject(
                "BP_Butterfly_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.BP_Butterfly_4");
            const auto Func = Bf->Function("ButterflySequence");
            Bf->ProcessEvent(Func);
            return;
        }

        if (FortniteVersion == IceKingVersion)
        {
            static const auto Ms = FindObject(
                "LevelSequencePlayer /Game/Athena/Maps/Test/S7/MooneySequenceMap.MooneySequenceMap.PersistentLevel.MooneySequence.AnimationPlayer");
            const auto Func = Ms->Function("Play");
            Ms->ProcessEvent(Func);
            return;
        }

        if (FortniteVersion == MarshmallowVersion)
        {
            static const auto Fs = FindObject(".PersistentLevel.FestivusSequence_01_2.AnimationPlayer2");
            const auto Func = Fs->Function("Play");
            Fs->ProcessEvent(Func);
            // TODO: Fix audio
            return;
        }

        if (FortniteVersion == UnvaultVersion)
        {
            static const auto Bss = FindObject(
                "BP_SnowScripting_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.BP_SnowScripting_2");
            const auto Func = Bss->Function("FinalSequence");
            Bss->ProcessEvent(Func);
            // TODO: Teleport back after unvaulting part is done
            return;
        }

        if (FortniteVersion == FinalShutdownVersion)
        {
            static const auto Cd = FindObject(
                "BP_CattusDoggus_Scripting_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.BP_CattusDoggus_Scripting_2");
            const auto Func = Cd->Function("startevent");
            Cd->ProcessEvent(Func);
            return;
        }

        if (FortniteVersion == TheEndVersion)
        {
            static const auto Nn = FindObject(
                "BP_NightNight_Scripting_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.BP_NightNight_Scripting_2");
            const auto Func = Nn->Function("startevent");
            Nn->ProcessEvent(Func);
            return;
        }

        if (FortniteVersion == FirstDeviceEvent)
        {
            static const auto JerkyPlayerInteraction = FindObject(
                "BP_Jerky_PlayerInteraction_C /CycloneJerky/Levels/JerkySequenceMap_LevelInstance_1.JerkySequenceMap.PersistentLevel.BP_Jerky_PlayerInteraction_2");
            if (!JerkyPlayerInteraction)
            {
                return;
            }

            static const auto JerkyBpLoader = FindObject(
                "BP_Jerky_Scripting_C /CycloneJerky/Levels/JerkySequenceMap_LevelInstance_1.JerkySequenceMap.PersistentLevel.BP_Jerky_Scripting_2");
            if (!JerkyBpLoader)
            {
                return;
            }

            const auto TeleportDistant = JerkyBpLoader->Function("TeleportDistantPlayers");
            const auto DebugStartSequence = JerkyBpLoader->Function("DebugStartSequence");

            JerkyBpLoader->ProcessEvent(TeleportDistant);
            auto Parameters = 0.0f;
            JerkyBpLoader->ProcessEvent(DebugStartSequence, &Parameters);
            Travis::Start();
            ApplyToPlayers([](auto Controller, auto)
            {
                Inventory::Clear(Controller, true);
            });
        }
    }
}
