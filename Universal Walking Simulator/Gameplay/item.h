#pragma once
#include "UE/structs.h"

namespace Item
{
    // Default items and quantities
    static std::pair<std::string, int> StartingSlot1 = {
        "FortWeaponRangedItemDefinition /Game/Athena/Items/Weapons/WID_Assault_AutoHigh_Athena_SR_Ore_T03.WID_Assault_AutoHigh_Athena_SR_Ore_T03",
        1
    };
    static std::pair<std::string, int> StartingSlot2 = {
        "FortWeaponRangedItemDefinition /Game/Athena/Items/Weapons/WID_Shotgun_Standard_Athena_UC_Ore_T03.WID_Shotgun_Standard_Athena_UC_Ore_T03",
        1
    };
    static std::pair<std::string, int> StartingSlot3 = {"", 0};
    static std::pair<std::string, int> StartingSlot4 = {
        "FortWeaponRangedItemDefinition /Game/Athena/Items/Consumables/ShieldSmall/Athena_ShieldSmall.Athena_ShieldSmall",
        3
    };
    static std::pair<std::string, int> StartingSlot5 = {
        "FortWeaponRangedItemDefinition /Game/Athena/Items/Consumables/PurpleStuff/Athena_PurpleStuff.Athena_PurpleStuff",
        1
    };

    // Pickaxe
    inline UObject* Pickaxe;

    // Edit tool
    inline UObject* EditTool;

    // All building items available in Battle Royal
    inline UObject* WallBuild;
    inline UObject* FloorBuild;
    inline UObject* StairBuild;
    inline UObject* RoofBuild;

    // All materials available in Battle Royal
    inline UObject* WoodMaterial;
    inline UObject* StoneMaterial;
    inline UObject* MetalMaterial;

    // All ammunition available in battle royal
    inline UObject* RocketBullet;
    inline UObject* ShellBullet;
    inline UObject* MediumBullet;
    inline UObject* LightBullet;
    inline UObject* HeavyBullet;
    inline UObject* GrapplerBullet;

    // TODO: Add all item definitions for quick access
    inline UObject* ResourceItemDefinition;
    inline UObject* WeaponItemDefinition;

    // Max constants
    inline int MaxMaterials;

    inline void Init()
    {
        Pickaxe = FindObject(
            "FortWeaponMeleeItemDefinition /Game/Athena/Items/Weapons/WID_Harvest_Pickaxe_Athena_C_T01.WID_Harvest_Pickaxe_Athena_C_T01");
        WallBuild = FindObject(
            "FortBuildingItemDefinition /Game/Items/Weapons/BuildingTools/BuildingItemData_Wall.BuildingItemData_Wall");
        FloorBuild = FindObject(
            "FortBuildingItemDefinition /Game/Items/Weapons/BuildingTools/BuildingItemData_Floor.BuildingItemData_Floor");
        StairBuild = FindObject(
            "FortBuildingItemDefinition /Game/Items/Weapons/BuildingTools/BuildingItemData_Stair_W.BuildingItemData_Stair_W");
        RoofBuild = FindObject(
            "FortBuildingItemDefinition /Game/Items/Weapons/BuildingTools/BuildingItemData_RoofS.BuildingItemData_RoofS");
        WoodMaterial = FindObject(
            "FortResourceItemDefinition /Game/Items/ResourcePickups/WoodItemData.WoodItemData");
        StoneMaterial = FindObject(
            "FortResourceItemDefinition /Game/Items/ResourcePickups/StoneItemData.StoneItemData");
        MetalMaterial = FindObject(
            "FortResourceItemDefinition /Game/Items/ResourcePickups/MetalItemData.MetalItemData");
        RocketBullet = FindObject(EngineVersion < 420
                                      ? "FortAmmoItemDefinition /Game/Athena/Items/Ammo/AthenaAmmoDataRockets.AthenaAmmoDataRockets"
                                      : "FortAmmoItemDefinition /Game/Athena/Items/Ammo/AmmoDataRockets.AmmoDataRockets");
        ShellBullet = FindObject(EngineVersion < 420
                                     ? "FortAmmoItemDefinition /Game/Items/Ammo/AthenaAmmoDataShells.AthenaAmmoDataShells"
                                     : "FortAmmoItemDefinition /Game/Athena/Items/Ammo/AthenaAmmoDataShells.AthenaAmmoDataShells");
        MediumBullet = FindObject(EngineVersion < 420
                                      ? "FortAmmoItemDefinition /Game/Items/Ammo/AthenaAmmoDataBulletsMedium.AthenaAmmoDataBulletsMedium"
                                      : "FortAmmoItemDefinition /Game/Athena/Items/Ammo/AthenaAmmoDataBulletsMedium.AthenaAmmoDataBulletsMedium");
        LightBullet = FindObject(EngineVersion < 420
                                     ? "FortAmmoItemDefinition /Game/Items/Ammo/AthenaAmmoDataBulletsLight.AthenaAmmoDataBulletsLight"
                                     : "FortAmmoItemDefinition /Game/Athena/Items/Ammo/AthenaAmmoDataBulletsMedium.AthenaAmmoDataBulletsMedium");
        HeavyBullet = FindObject(EngineVersion < 420
                                     ? "FortAmmoItemDefinition /Game/Items/Ammo/AthenaAmmoDataBulletsHeavy.AthenaAmmoDataBulletsHeavy"
                                     : "FortAmmoItemDefinition /Game/Athena/Items/Ammo/AthenaAmmoDataBulletsHeavy.AthenaAmmoDataBulletsHeavy");
        GrapplerBullet = FindObject(
            "FortAmmoItemDefinition /Game/Athena/Items/Ammo/AthenaAmmoDataHooks.AthenaAmmoDataHooks");

        EditTool = FindObject("FortEditToolItemDefinition /Game/Items/Weapons/BuildingTools/EditTool.EditTool");

        ResourceItemDefinition = FindObject("Class /Script/FortniteGame.FortResourceItemDefinition");
        WeaponItemDefinition = FindObject("Class /Script/FortniteGame.FortWeaponItemDefinition");

        // TODO: Code maxes in, for version and arena
        MaxMaterials = 999;
    }

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

        static auto ItemDefinitionOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortItemEntry"),
                                                            ("ItemDefinition"));
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
