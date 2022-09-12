#pragma once
#include "UE/structs.h"

namespace Item
{
    // Default items and quantities
    static std::pair<std::string, int> StartingSlot1 = { "FortWeaponRangedItemDefinition /Game/Athena/Items/Weapons/WID_Assault_AutoHigh_Athena_SR_Ore_T03.WID_Assault_AutoHigh_Athena_SR_Ore_T03", 1 };
    static std::pair<std::string, int> StartingSlot2 = { "FortWeaponRangedItemDefinition /Game/Athena/Items/Weapons/WID_Shotgun_Standard_Athena_UC_Ore_T03.WID_Shotgun_Standard_Athena_UC_Ore_T03", 1 };
    static std::pair<std::string, int> StartingSlot3 = { "", 0 };
    static std::pair<std::string, int> StartingSlot4 = { "FortWeaponRangedItemDefinition /Game/Athena/Items/Consumables/ShieldSmall/Athena_ShieldSmall.Athena_ShieldSmall", 3 };
    static std::pair<std::string, int> StartingSlot5 = { "FortWeaponRangedItemDefinition /Game/Athena/Items/Consumables/PurpleStuff/Athena_PurpleStuff.Athena_PurpleStuff", 1 };

    // Pickaxe
    static const auto Pickaxe = FindObject("FortWeaponMeleeItemDefinition /Game/Athena/Items/Weapons/WID_Harvest_Pickaxe_Athena_C_T01.WID_Harvest_Pickaxe_Athena_C_T01");
    
    // All building items available in Battle Royal
    static const auto WallBuild = FindObject(
        "FortBuildingItemDefinition /Game/Items/Weapons/BuildingTools/BuildingItemData_Wall.BuildingItemData_Wall");
    static const auto FloorBuild = FindObject(
        "FortBuildingItemDefinition /Game/Items/Weapons/BuildingTools/BuildingItemData_Floor.BuildingItemData_Floor");
    static const auto StairBuild = FindObject(
        "FortBuildingItemDefinition /Game/Items/Weapons/BuildingTools/BuildingItemData_Stair_W.BuildingItemData_Stair_W");
    static const auto RoofBuild = FindObject(
        "FortBuildingItemDefinition /Game/Items/Weapons/BuildingTools/BuildingItemData_RoofS.BuildingItemData_RoofS");

    // All materials available in Battle Royal
    static const auto WoodMaterial = FindObject(
        "FortResourceItemDefinition /Game/Items/ResourcePickups/WoodItemData.WoodItemData");
    static const auto StoneMaterial = FindObject(
        "FortResourceItemDefinition /Game/Items/ResourcePickups/StoneItemData.StoneItemData");
    static const auto MetalMaterial = FindObject(
        "FortResourceItemDefinition /Game/Items/ResourcePickups/MetalItemData.MetalItemData");

    // All ammunition available in battle royal
    static const auto RocketBullet = FindObject(EngineVersion < 420 ? "FortAmmoItemDefinition /Game/Athena/Items/Ammo/AthenaAmmoDataRockets.AthenaAmmoDataRockets" : "FortAmmoItemDefinition /Game/Athena/Items/Ammo/AmmoDataRockets.AmmoDataRockets");
    static const auto ShellBullet = FindObject(EngineVersion < 420 ? "FortAmmoItemDefinition /Game/Items/Ammo/AthenaAmmoDataShells.AthenaAmmoDataShells" : "FortAmmoItemDefinition /Game/Athena/Items/Ammo/AthenaAmmoDataShells.AthenaAmmoDataShells");
    static const auto MediumBullet = FindObject(EngineVersion < 420 ? "FortAmmoItemDefinition /Game/Items/Ammo/AthenaAmmoDataBulletsMedium.AthenaAmmoDataBulletsMedium" : "FortAmmoItemDefinition /Game/Athena/Items/Ammo/AthenaAmmoDataBulletsMedium.AthenaAmmoDataBulletsMedium");
    static const auto LightBullet = FindObject(EngineVersion < 420 ? "FortAmmoItemDefinition /Game/Items/Ammo/AthenaAmmoDataBulletsLight.AthenaAmmoDataBulletsLight" : "FortAmmoItemDefinition /Game/Athena/Items/Ammo/AthenaAmmoDataBulletsMedium.AthenaAmmoDataBulletsMedium");
    static const auto HeavyBullet = FindObject(EngineVersion < 420 ? "FortAmmoItemDefinition /Game/Items/Ammo/AthenaAmmoDataBulletsHeavy.AthenaAmmoDataBulletsHeavy" : "FortAmmoItemDefinition /Game/Athena/Items/Ammo/AthenaAmmoDataBulletsHeavy.AthenaAmmoDataBulletsHeavy");
    static const auto GrapplerBullet = FindObject("FortAmmoItemDefinition /Game/Athena/Items/Ammo/AthenaAmmoDataHooks.AthenaAmmoDataHooks");
    
    // Edit tool
    static const auto EditTool = FindObject("FortEditToolItemDefinition /Game/Items/Weapons/BuildingTools/EditTool.EditTool");
    
    // TODO: Add all item definitions for quick access
    static const auto ResourceItemDefinition = FindObject("Class /Script/FortniteGame.FortResourceItemDefinition");
    static const auto WeaponItemDefinition = FindObject("Class /Script/FortniteGame.FortWeaponItemDefinition");

    // TODO: Code maxes in, for version and arena
    static constexpr int MaxMaterials = 999;

    inline auto GetMaterial(const EFortResourceType Type) -> UObject*
    {
        switch (Type)
        {
        case EFortResourceType::Wood:
            return WoodMaterial;
        case EFortResourceType::Stone:
            return StoneMaterial;
        case EFortResourceType::Metal:
            return MetalMaterial;
        default:
            return nullptr;
        }
    }

    inline auto GetCount(long long* Entry) -> int*
    {
        static auto CountOffset = FindOffsetStruct("ScriptStruct /Script/FortniteGame.FortItemEntry", "Count");
        return reinterpret_cast<int*>(Entry + CountOffset);
    }

    inline auto GetCount(UObject* Entry) -> int*
    {
        if (!Entry)
        {
            return nullptr;
        }
        
        return GetCount(reinterpret_cast<long long*>(Entry));
    }

    inline auto GetLoadedAmmo(UObject* Entry) -> int*
    {
        if (!Entry)
        {
            return nullptr;
        }

        static auto LoadedAmmoOffset = Entry->Function("GetLoadedAmmo");
        return static_cast<int*>(Entry->ProcessEvent(LoadedAmmoOffset));
    }

    inline auto GetGuid(UObject* ItemInstance)
    {
        static UObject* GetItemGuidFn = FindObject("Function /Script/FortniteGame.FortItem.GetItemGuid");
        FGuid Guid;
        if (ItemInstance && GetItemGuidFn)
        {
            ItemInstance->ProcessEvent(GetItemGuidFn, &Guid);
        }

        return Guid;
    }

    inline auto GetDefinition(UObject* Entry) -> UObject*
    {
        if (!Entry)
        {
            return nullptr;
        }

        static auto Function = Entry->Function("GetItemDefinitionBP");
        return static_cast<UObject*>(Entry->ProcessEvent(Function));
    }

    inline auto GetDefinition(long long* Entry) -> UObject*
    {
        if (!Entry)
        {
            return nullptr;
        }

        static auto ItemDefinitionOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortItemEntry"), ("ItemDefinition"));
        return *reinterpret_cast<UObject**>(reinterpret_cast<long long>(&*Entry) + ItemDefinitionOffset);
    }
    
    inline auto GetEntry(UObject* Instance) -> UObject*
    {
        if (!Instance)
        {
            return nullptr;
        }
        
        static auto ItemEntryOffset = GetOffset(Instance, "ItemEntry");
        return reinterpret_cast<UObject*>(reinterpret_cast<long long>(Instance) + ItemEntryOffset);
    }

    inline auto IsResource(const UObject* Object)
    {
        return Object->IsA(ResourceItemDefinition);
    }

    inline auto IsWeapon(const UObject* Object)
    {
        return Object->IsA(WeaponItemDefinition);
    }
    
    inline auto GetQuickBar(const UObject* Definition)
    {
        return IsWeapon(Definition) ? EFortQuickBars::Primary : EFortQuickBars::Secondary;
    }

    // TODO: Use isA to determine if item is a gadget
    inline auto IsGadget(UObject* Definition)
    {
        const auto FullName = Definition->GetFullName();
        return FullName.contains("AthenaGadgetItemDefinition ")
            || FullName.contains("FortGadgetItemDefinition ");
    }
}
