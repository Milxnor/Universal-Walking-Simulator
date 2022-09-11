#pragma once

#include <Gameplay/helper.h>
#include <Gameplay/inventory.h>

namespace Harvesting
{
    inline int GetResourcesAmount(UObject* BuildingActor)
    {
        std::random_device RandomDevice;
        std::mt19937 Generator(RandomDevice());
        const auto MaxResourcesToSpawn = *BuildingActor->Member<int>("MaxResourcesToSpawn");
        std::uniform_int_distribution Distribution(MaxResourcesToSpawn / 2, MaxResourcesToSpawn);
        return Distribution(Generator);
    }

    inline void HarvestItem(UObject* Controller, UObject* BuildingActor, const float Damage = 0.f)
    {
        // Player placed replacement
        if (const auto BuildingResourceAmountOverride = BuildingActor->Member<CurveTableRowHandle>(
                "BuildingResourceAmountOverride");
            !BuildingResourceAmountOverride->RowName.ComparisonIndex)
        {
            return;
        }

        const auto MaxDamage = Damage == 100.f;
        const auto Amount = MaxDamage
                                ? GetResourcesAmount(BuildingActor) + GetResourcesAmount(BuildingActor)
                                : GetResourcesAmount(BuildingActor);
        const auto Type = *BuildingActor->Member<TEnumAsByte<EFortResourceType>>("ResourceType");
        ReportDamagedResourceBuildingParameters Parameters{BuildingActor, Type, Amount, false, MaxDamage};

        static auto ClientReportDamagedResourceBuilding = Controller->Function("ClientReportDamagedResourceBuilding");
        if (!ClientReportDamagedResourceBuilding)
        {
            return;
        }

        Controller->ProcessEvent(ClientReportDamagedResourceBuilding, &Parameters);

        const auto Pawn = *Controller->Member<UObject*>("Pawn");
        const auto ItemDef = Item::GetMaterial(Parameters.PotentialResourceType.Get());
        const auto ItemInstance = Inventory::FindItem(Controller, ItemDef);
        const auto AmountToGive = Parameters.PotentialResourceCount;

        // TODO: Take into account weakspot to determine quantity of loot
        if (ItemInstance && Pawn)
        {
            if (const auto Entry = ItemInstance->Member<long long>("ItemEntry"); *Item::GetCount(Entry) >=
                999)
            {
                // BUG: You lose some mats if you have like 998
                Helper::SummonPickup(Pawn, ItemDef, Helper::GetActorLocation(Pawn),
                                     EFortPickupSourceTypeFlag::Other, EFortPickupSpawnSource::Unset, AmountToGive);
                return;
            }
        }

        Inventory::GiveItem(Controller, ItemDef, EFortQuickBars::Secondary, 1, AmountToGive);
    }

    // TODO: Not hardcode the PickaxeDef, do like slot 0 or something
    inline bool OnDamageServerHook(UObject* BuildingActor, [[maybe_unused]] UFunction* Function, void* Parameters)
    {
        static const auto BuildingSMActorClass = FindObject("Class /Script/FortniteGame.BuildingSMActor");
        if (!BuildingActor->IsA(BuildingSMActorClass))
        {
            return false;
        }

        static const auto FortPlayerControllerAthenaClass = FindObject(
            "Class /Script/FortniteGame.FortPlayerControllerAthena");
        const auto InstigatedByOffset = FindOffsetStruct("Function /Script/FortniteGame.BuildingActor.OnDamageServer",
                                                         "InstigatedBy");
        const auto InstigatedBy = *reinterpret_cast<UObject**>(reinterpret_cast<long long>(Parameters) +
            InstigatedByOffset);
        if (!InstigatedBy || !InstigatedBy->IsA(FortPlayerControllerAthenaClass))
        {
            return false;
        }

        const auto DamageCauserOffset = FindOffsetStruct("Function /Script/FortniteGame.BuildingActor.OnDamageServer",
                                                         "DamageCauser");
        if (const auto DamageCauser = *reinterpret_cast<UObject**>(reinterpret_cast<long long>(Parameters) +
                DamageCauserOffset);
            !DamageCauser->GetFullName().contains("B_Melee_Impact_Pickaxe_Athena_C"))
        {
            return false;
        }

        const auto DamageOffset = FindOffsetStruct("Function /Script/FortniteGame.BuildingActor.OnDamageServer",
                                                   "Damage");
        const auto Damage = reinterpret_cast<float*>(reinterpret_cast<long long>(Parameters) + DamageOffset);
        if (const auto CurrentWeapon = *(*InstigatedBy->Member<UObject*>("MyFortPawn"))->Member<UObject*>(
                "CurrentWeapon");
            !CurrentWeapon || *CurrentWeapon->Member<UObject*>("WeaponData") != Item::Pickaxe)
        {
            return false;
        }

        HarvestItem(InstigatedBy, BuildingActor, *Damage);
        return false;
    }

    // TODO: Not hardcode the PickaxeDef, do like slot 0 or something
    inline bool BlueprintCanAttemptGenerateResourcesHook(UObject* BuildingActor, UFunction* Function, void* Parameters)
    {
        const auto Params = static_cast<InstigatorParameters*>(Parameters);
        ProcessEventO(BuildingActor, Function, Params);
        if (!Params || !Params->Ret)
        {
        }

        const auto Pawn = *Params->InstigatorController->Member<UObject*>("MyFortPawn");
        if (const auto CurrentWeapon = *Pawn->Member<UObject*>("CurrentWeapon"); !CurrentWeapon || *CurrentWeapon->
            Member<UObject*>("WeaponData") != Item::Pickaxe)
        {
            return false;
        }

        HarvestItem(Params->InstigatorController, BuildingActor);
        return false;
    }

    inline void InitHooks()
    {
        Hooks::Add("Function /Script/FortniteGame.BuildingActor.OnDamageServer", OnDamageServerHook);
        if (EngineVersion > 424)
        {
            Hooks::Add("Function /Script/FortniteGame.BuildingSMActor.BlueprintCanAttemptGenerateResources", BlueprintCanAttemptGenerateResourcesHook);
        }
    }
}
