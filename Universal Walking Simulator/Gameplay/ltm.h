#pragma once
#include <UE/structs.h>
#include <Gameplay/helper.h>
#include <Gameplay/ability.h>

#include "UE/hooks.h"

namespace Ltm
{
    namespace Thanos
    {
        static bool Spawned = false;

        inline void HandleGauntlet(UObject* Controller)
        {
            if (!Controller || FortniteVersion >= 11.0)
            {
                return;
            }

            const auto Pawn = Helper::GetPawnFromController(Controller);

            const auto Body = FindObject(
                "CustomCharacterPart /Game/Athena/Heroes/Meshes/Bodies/Dev_TestAsset_Body_M_XL.Dev_TestAsset_Body_M_XL");
            Helper::ChoosePart(Pawn, EFortCustomPartType::Body, Body);

            const auto Head = FindObject(
                "CustomCharacterPart /Game/Athena/Heroes/Meshes/Heads/Dev_TestAsset_Head_M_XL.Dev_TestAsset_Head_M_XL");
            Helper::ChoosePart(Pawn, EFortCustomPartType::Head, Head);

            const auto Animation = FindObject(
                "AnimBlueprintGeneratedClass /Game/Characters/Player/Male/Male_Avg_Base/Gauntlet_Player_AnimBlueprint.Gauntlet_Player_AnimBlueprint_C",
                true);
            *Pawn->Member<UObject*>("AnimBPOverride") = Animation;


            const auto CarminePack = FindObject(
                "FortAbilitySet /Game/Athena/Items/Gameplay/BackPacks/CarminePack/AS_CarminePack.AS_CarminePack");
            const auto GrantedAbilities = CarminePack->Member<TArray<UObject*>>("GameplayAbilities");
            for (int i = 0; i < GrantedAbilities->Num(); i++)
            {
                Ability::EnableGameplayAbility(Pawn, GrantedAbilities->At(i));
            }

            const auto Speed = FindObject(
                "BlueprintGeneratedClass /Game/Athena/Items/Gameplay/BackPacks/CarminePack/GE_Carmine_Speed.GE_Carmine_Speed_C");
            Ability::ApplyGameplayEffect(*Pawn->Member<UObject*>("AbilitySystemComponent"), Speed);

            const auto FallDamage = FindObject(
                "BlueprintGeneratedClass /Game/Athena/Items/Gameplay/BackPacks/CarminePack/GE_Carmine_FallDamageImmune.GE_Carmine_FallDamageImmune_C");
            Ability::ApplyGameplayEffect(*Pawn->Member<UObject*>("AbilitySystemComponent"), FallDamage);

            const auto NoCrouch = FindObject(
                "BlueprintGeneratedClass /Game/Athena/Items/Gameplay/BackPacks/CarminePack/GE_Carmine_DisableCrouch.GE_Carmine_DisableCrouch_C");
            Ability::ApplyGameplayEffect(*Pawn->Member<UObject*>("AbilitySystemComponent"), NoCrouch);

            const auto Equipped = FindObject(
                "BlueprintGeneratedClass /Game/Athena/Items/Gameplay/BackPacks/CarminePack/GE_Carmine_Equipped.GE_Carmine_Equipped_C");
            Ability::ApplyGameplayEffect(*Pawn->Member<UObject*>("AbilitySystemComponent"), Equipped);
        }

        inline bool OnGauntletProjectileStopHook(UObject* Object, [[maybe_unused]] UFunction* Func, void* Params)
        {
            const auto LocOffset = FindOffsetStruct("ScriptStruct /Script/Engine.HitResult", "Location");
            const auto Loc = reinterpret_cast<FVector*>(reinterpret_cast<long long>(Params) + LocOffset);
            if (!Object->GetName().contains("AthenaSupplyDrop_Meteor_Gauntlet") || Spawned != false)
            {
                return false;
            }

            const auto Gauntlet = FindObject(
                "AthenaGadgetItemDefinition /Game/Athena/Items/Gameplay/BackPacks/CarminePack/AGID_CarminePack.AGID_CarminePack");
            Helper::SummonPickup(nullptr, Gauntlet, *Loc, EFortPickupSourceTypeFlag::Other,
                                 EFortPickupSpawnSource::Unset);
            Spawned = true;
            return false;
        }

        inline bool OnGemProjectileStopHook(UObject* Object, UFunction* Func, void* Params)
        {
            const auto LocOffset = FindOffsetStruct("ScriptStruct /Script/Engine.HitResult", "Location");
            const auto Loc = reinterpret_cast<FVector*>(long long(Params) + LocOffset);
            const auto ItemToSpawn = Object->Member<UObject*>("ItemDefToSpawn");
            if (!ItemToSpawn || !*ItemToSpawn)
            {
                return false;
            }

            Helper::SummonPickup(nullptr, *ItemToSpawn, *Loc, EFortPickupSourceTypeFlag::Other,
                                 EFortPickupSpawnSource::Unset);
            return false;
        }

        inline bool ReceiveBeginPlayHook(UObject* Object, [[maybe_unused]] UFunction* Func,
                                         [[maybe_unused]] void* Params)
        {
            if (!Spawned)
            {
                return false;
            }

            Helper::DestroyActor(Object);
            return true;
        }

        inline void InitHooks(const bool Gems)
        {
            Hooks::Add(
                "Function /Game/Athena/SupplyDrops/AthenaSupplyDrop.AthenaSupplyDrop_C.BndEvt__ProjectileMovement_K2Node_ComponentBoundEvent_0_OnProjectileStopDelegate__DelegateSignature",
                OnGauntletProjectileStopHook);
            if(!Gems)
            {
                return;
            }
            
            Hooks::Add(
                "Function /Game/Athena/Playlists/Carmine/MeteorDrop/AthenaSupplyDrop_Meteor_Gauntlet.AthenaSupplyDrop_Meteor_Gauntlet_C.UserConstructionScript",
                ReceiveBeginPlayHook);
        }

        // TODO: Make spawning random like random stone
        inline void SpawnGauntlet()
        {
            const auto Class = FindObject(
                "/Game/Athena/Playlists/Carmine/MeteorDrop/AthenaSupplyDrop_Meteor_Gauntlet.AthenaSupplyDrop_Meteor_Gauntlet_C",
                true);
            const auto PseudoRandomLocation = FVector{1250, 1818, 6000};
            Easy::SpawnActor(Class, PseudoRandomLocation, {});
        }

        inline void SpawnRandomStone()
        {
            const auto Class = FindObject(
                "/Game/Athena/Playlists/Ashton/Rocks/AthenaSupplyDrop_Rock_P.AthenaSupplyDrop_Rock_P_C", true);

            FVector RandLocation;
            std::random_device rd;
            std::mt19937 gen(rd());

            std::uniform_int_distribution<> AbscissaDistribution(-40000, 128000);
            std::uniform_int_distribution<> OrdinateDistribution(-90000, 70000);

            RandLocation.X = static_cast<float>(AbscissaDistribution(gen));
            RandLocation.Y = static_cast<float>(OrdinateDistribution(gen));
            RandLocation.Z = 10000.0f;

            Easy::SpawnActor(Class, RandLocation, {});
        }
    }
}
