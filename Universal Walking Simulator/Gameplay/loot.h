#pragma once
#include <vector>
#include <random>

#include <Gameplay/helper.h>

#include "item.h"

namespace Looting
{
    inline std::vector<std::vector<DefinitionInRow>> Items;
    inline std::vector<std::vector<float>> Weights;
    inline bool Initialized = false;

    static constexpr auto LootRow = 0;
    static constexpr auto SupplyDropRow = 1;
    static constexpr auto LamaRow = 2;

    inline auto GetLootPackageName(TSoftObjectPtr* LootPackagesSoft)
    {
        if (LootPackagesSoft && LootPackagesSoft->ObjectID.AssetPathName.ComparisonIndex)
        {
            const auto LootPackagesName = LootPackagesSoft->ObjectID.AssetPathName.ToString();
            return LootPackagesSoft->ObjectID.SubPathString.ToString();
        }

        return "/Game/Athena/Playlists/Playground/AthenaLootPackages_Client.AthenaLootPackages_Client";
    }

    inline auto GetLootPackages() -> UObject*
    {
        if (EngineVersion < 420)
        {
            return nullptr;
        }

        const auto Playlist = Helper::GetPlaylist();
        if (!Playlist)
        {
            return nullptr;
        }

        const auto LootPackagesSoft = Playlist->Member<TSoftObjectPtr>("LootPackages");
        const auto LootPackagesName = GetLootPackageName(LootPackagesSoft);

        static auto ClassToUse = LootPackagesName.ends_with("_Client")
                                     ? FindObject("Class /Script/Engine.DataTable")
                                     : FindObject("Class /Script/Engine.CompositeDataTable");
        const auto LootPackages = StaticLoadObject(ClassToUse, nullptr, LootPackagesName);
        return LootPackages;
    }

    inline void AddItemAndWeight(const int Index, const DefinitionInRow& Item, const float Weight)
    {
        if (Index > Items.size() || Index > Weights.size())
        {
            return;
        }

        Items[Index].push_back(Item);
        Weights[Index].push_back(Weight);
    }

    inline auto GetRowMap(UObject* DataTable)
    {
        static auto RowStructOffset = GetOffset(DataTable, "RowStruct");
        return *reinterpret_cast<TMap<FName, uint8_t*>*>(reinterpret_cast<long long>(DataTable) + (RowStructOffset +
            sizeof(UObject*)));
    }

    // TODO: Handle lama
    inline auto GetLootIndex(const std::string& RowName)
    {
        if (RowName.starts_with("WorldList.AthenaLoot"))
        {
            return LootRow;
        }

        if (RowName.starts_with("WorldPKG.AthenaSupplyDrop."))
        {
            return SupplyDropRow;
        }

        return -1;
    }

    inline auto GetItemTypeByAsset(const std::string& DefinitionString)
    {
        if (DefinitionString.contains("Weapon"))
        {
            return Item::Type::Weapon;
        }

        if (DefinitionString.contains("Consumable"))
        {
            return Item::Type::Consumable;
        }

        if (DefinitionString.contains("Ammo"))
        {
            return Item::Type::Ammo;
        }

        return Item::Type::None;
    }

    inline void Init()
    {
        if (Initialized)
        {
            return;
        }

        if (!StaticFindObjectO)
        {
            return;
        }

        Initialized = true;

        for (int i = 0; i < 2; i++)
        {
            Items.emplace_back(std::vector<DefinitionInRow>());
        }

        for (int i = 0; i < 2; i++)
        {
            Weights.emplace_back(std::vector<float>());
        }


        const auto LootPackages = GetLootPackages();
        if (!LootPackages)
        {
            return;
        }

        const auto [Pairs] = GetRowMap(LootPackages);
        const auto Elements = Pairs.Elements.Data;
        for (auto i = 0; i < Elements.Num(); i++)
        {
            const auto Man = Elements.At(i);
            auto Pair = Man.ElementData.Value;
            if (!Pair.First.ComparisonIndex)
            {
                continue;
            }


            auto RowName = Pair.First.ToString();
            const auto LootPackageDataOfRow = Pair.Second; // ScriptStruct FortniteGame.FortLootPackageData
            if (!LootPackageDataOfRow)
            {
                continue;
            }

            const auto Index = GetLootIndex(RowName);

            if (Index == -1)
            {
                continue;
            }

            static auto off = FindOffsetStruct("ScriptStruct /Script/FortniteGame.FortLootPackageData",
                                               "ItemDefinition");
            static auto countOff = FindOffsetStruct("ScriptStruct /Script/FortniteGame.FortLootPackageData",
                                                    "Count");
            static auto weightOff = FindOffsetStruct("ScriptStruct /Script/FortniteGame.FortLootPackageData",
                                                     "Weight");

            const auto ItemDef = reinterpret_cast<TSoftObjectPtr*>(reinterpret_cast<long long>(LootPackageDataOfRow) +
                off);
            if (!ItemDef)
            {
                continue;
            }

            DefinitionInRow currentItem;

            currentItem.RowName = RowName;

            const auto DefinitionString = ItemDef->ObjectID.AssetPathName.ToString();
            currentItem.Definition = FindObject(DefinitionString);

            currentItem.Type = GetItemTypeByAsset(DefinitionString);

            if (const auto Count = reinterpret_cast<int*>(reinterpret_cast<long long>(LootPackageDataOfRow) + countOff))
            {
                currentItem.DropCount = *Count;
            }

            if (const auto Weight = reinterpret_cast<float*>(reinterpret_cast<long long>(LootPackageDataOfRow) +
                weightOff))
            {
                currentItem.Weight = *Weight;
                AddItemAndWeight(Index, currentItem, *Weight);
            }
        }
    }

    inline auto RandomBoolWithWeight(const float Weight)
    {
        if (Weight <= 0.0f)
        {
            return false;
        }

        std::random_device RandomDevice;
        std::mt19937 Generator(RandomDevice());

        std::uniform_int_distribution<> Distribution(0.0f, 1.0f);
        return Weight >= Distribution(Generator);
    }

    // TODO: Optimize
    inline auto GetRandomItem(const Item::Type Type, const int Row = LootRow)
    {
        if (Row <= Items.size() ? Items[Row].empty() : true)
        {
            return DefinitionInRow();
        }

        const auto TableToUse = Items[Row];
        auto current = 0;
        while (current < 5000)
        {
            auto Item = TableToUse[rand() % TableToUse.size()];
            if (!RandomBoolWithWeight(Item.Weight) || Item.Type != Type || !Item.Definition)
            {
                current++;
                continue;
            }

            return Item;
        }

        return DefinitionInRow();
    }

    // TODO: Optimize
    // DOCUMENTATION: https://ibb.co/Ny7yHjF
    inline auto SummonFloorLoot(LPVOID) -> DWORD
    {
        static auto BuildingContainerClass = FindObject("Class /Script/FortniteGame.BuildingContainer");

        const auto world = Helper::GetWorld();
        if (!BuildingContainerClass)
        {
            return 0;
        }

        auto BuildingContainers = Helper::GetAllActorsOfClass(BuildingContainerClass);
        for (int i = 0; i < BuildingContainers.Num(); i++)
        {
            const auto BuildingContainer = BuildingContainers.At(i);

            if (!BuildingContainer || !BuildingContainer->GetFullName().contains("Tiered_Athena_FloorLoot_"))
            {
                continue;
            }

            if (EngineVersion == 422 || EngineVersion == 423)
            {
                (reinterpret_cast<FixFloorLootCrash*>(world) + 0x10C)->IsRunningConstructionScript = false;
            }

            if (const auto ShouldSpawn = RandomBoolWithWeight(0.7f); !ShouldSpawn)
            {
                Sleep(10);
                continue;
            }

            if (RandomBoolWithWeight(0.35f))
            {
                Helper::SummonPickup(nullptr, GetRandomItem(Item::Type::Consumable).Definition,
                                     Helper::GetCorrectLocation(BuildingContainer),
                                     EFortPickupSourceTypeFlag::FloorLoot,
                                     EFortPickupSpawnSource::Unset);
                Sleep(10);
                continue;
            }

            const auto [Definition, Weight, DropCount, RowName, Type] = GetRandomItem(Item::Type::Weapon);

            static auto GetAmmoWorldItemDefinitionBp = Definition->Function("GetAmmoWorldItemDefinition_BP");
            const auto WeaponPickup = Helper::SummonPickup(nullptr, Definition,
                                                           Helper::GetCorrectLocation(BuildingContainer),
                                                           EFortPickupSourceTypeFlag::FloorLoot,
                                                           EFortPickupSpawnSource::Unset);
            if (!GetAmmoWorldItemDefinitionBp || !WeaponPickup)
            {
                Sleep(10);
                continue;
            }


            GetAmmoWorldItemDefinitionParameters Parameters{};
            Definition->ProcessEvent(GetAmmoWorldItemDefinitionBp, &Parameters);
            const auto AmmoDef = Parameters.AmmoDefinition;
            Helper::SummonPickup(nullptr, AmmoDef, Helper::GetCorrectLocation(BuildingContainer),
                                 EFortPickupSourceTypeFlag::FloorLoot,
                                 EFortPickupSpawnSource::Unset, *AmmoDef->Member<int>("DropCount"));
            Sleep(10);
        }

        BuildingContainers.Free();
        return 0;
    }

    inline auto SummonLamas(LPVOID) -> DWORD
    {
        static auto const LamaClass = FindObject(("BlueprintGeneratedClass /Game/Athena/SupplyDrops/Llama/AthenaSupplyDrop_Llama.AthenaSupplyDrop_Llama_C"));
        if (!LamaClass)
        {
            return 0;
        }
        
        for (int i = 0; i < 3; i++)
        {
            FVector RandLocation;

            std::random_device RandomDevice;
            std::mt19937 Generator(RandomDevice());

            std::uniform_int_distribution<> AbscissaDistribution(-40000, 128000);
            std::uniform_int_distribution<> OrdinateDistribution(-90000, 70000);
            std::uniform_int_distribution<> OtherDistribution(-40000, 30000); 

            RandLocation.X = AbscissaDistribution(Generator);
            RandLocation.Y = OrdinateDistribution(Generator);
            RandLocation.Z = OtherDistribution(Generator);

            FRotator RandRotation;
            Easy::SpawnActor(LamaClass, RandLocation, RandRotation);
        }

        return 0;
    }

    inline void HandleChestSearch(UObject* BuildingContainer)
    {
        const auto [WeaponDef, WeaponWeight, WeaponCount, WeaponRow, WeaponType] = GetRandomItem(Item::Type::Weapon);
        const auto GetAmmoWorldItemDefinitionBp = WeaponDef->Function("GetAmmoWorldItemDefinition_BP");
        if (!GetAmmoWorldItemDefinitionBp)
        {
            return;
        }

        GetAmmoWorldItemDefinitionParameters Parameters{};
        WeaponDef->ProcessEvent(GetAmmoWorldItemDefinitionBp, &Parameters);
        const auto Location = Helper::GetCorrectLocation(BuildingContainer);

        if (WeaponDef)
        {
            Helper::SummonPickup(nullptr, WeaponDef, Location, EFortPickupSourceTypeFlag::Container,
                                 EFortPickupSpawnSource::Chest, WeaponCount);
        }

        if (Parameters.AmmoDefinition)
        {
            const auto Count = *Parameters.AmmoDefinition->Member<int>("DropCount");
            Helper::SummonPickup(nullptr, WeaponDef, Location, EFortPickupSourceTypeFlag::Container,
                                 EFortPickupSpawnSource::Chest, Count);
        }

        if (const auto [Definition, Weight, DropCount, RowName, Type] = GetRandomItem(Item::Type::Consumable);
            Definition)
        {
            Helper::SummonPickup(nullptr, Definition, Location,
                                 EFortPickupSourceTypeFlag::Container,
                                 EFortPickupSpawnSource::Chest, DropCount);
        }
    }

    inline void HandleAmmoSearch(UObject* BuildingContainer)
    {
        const auto [Definition, Weight, DropCount, RowName, Type] = GetRandomItem(Item::Type::Ammo);
        const auto AmmoDef = Definition;
        if (!AmmoDef)
        {
            return;
        }

        const auto Location = Helper::GetCorrectLocation(BuildingContainer);
        Helper::SummonPickup(nullptr, AmmoDef, Location, EFortPickupSourceTypeFlag::Container,
                             EFortPickupSpawnSource::AmmoBox, DropCount);
    }

    // TODO: Take into account different amounts of mats for different fortnite versions
    inline void HandleLamaSearch(UObject* BuildingContainer)
    {
        const auto Location = Helper::GetCorrectLocation(BuildingContainer);

        static auto Minis = FindObject(
            ("FortWeaponRangedItemDefinition /Game/Athena/Items/Consumables/ShieldSmall/Athena_ShieldSmall.Athena_ShieldSmall"));

        Helper::SummonPickup(nullptr, Item::WoodMaterial, Location, EFortPickupSourceTypeFlag::Container,
                             EFortPickupSpawnSource::SupplyDrop, 200);
        Helper::SummonPickup(nullptr, Item::StoneMaterial, Location, EFortPickupSourceTypeFlag::Container,
                             EFortPickupSpawnSource::SupplyDrop, 200);
        Helper::SummonPickup(nullptr, Item::MetalMaterial, Location, EFortPickupSourceTypeFlag::Container,
                             EFortPickupSpawnSource::SupplyDrop, 200);
        Helper::SummonPickup(nullptr, Minis, Location, EFortPickupSourceTypeFlag::Container,
                             EFortPickupSpawnSource::SupplyDrop, 6);

        // OnRep_Looted
        // SpawnLoot
    }

    inline void HandleSupplyDropSearch(UObject* BuildingContainer)
    {
        for (int i = 0; i < 5; i++)
        {
            Helper::SummonPickup(nullptr, GetRandomItem(Item::Type::Weapon, SupplyDropRow).Definition,
                                 Helper::GetCorrectLocation(BuildingContainer),
                                 EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, 1);
        }
    }

    // TODO: Make amount random
    // TODO: Handle all fishing rods
    inline void HandleFishingRodSearch(UObject* BuildingContainer)
    {
        static auto FishingRodWid = FindObject(
            "FortWeaponRangedItemDefinition /Game/Athena/Items/Consumables/FloppingRabbit/WID_Athena_FloppingRabbit.WID_Athena_FloppingRabbit");
        if (!FishingRodWid)
        {
            return;
        }

        Helper::SummonPickup(nullptr, FishingRodWid, Helper::GetActorLocation(BuildingContainer),
                             EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Unset, 1);
        Helper::SummonPickup(nullptr, FishingRodWid, Helper::GetActorLocation(BuildingContainer),
                             EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Unset, 1);
        Helper::DestroyActor(BuildingContainer);
    }

    // TODO: Handle FactionChest
    inline void HandleSearch(UObject* BuildingContainer)
    {
        if (!BuildingContainer)
        {
            return;
        }

        const auto BuildingContainerName = BuildingContainer->GetName();
        if (BuildingContainerName.contains("Tiered_Chest"))
        {
            HandleChestSearch(BuildingContainer);
        }
        else if (BuildingContainerName.contains("Ammo"))
        {
            HandleAmmoSearch(BuildingContainer);
        }
        else if (BuildingContainerName.contains("AthenaSupplyDrop_Llama_C"))
        {
            HandleLamaSearch(BuildingContainer);
        }
        else if (BuildingContainerName.contains("AthenaSupplyDrop_C"))
        {
            HandleSupplyDropSearch(BuildingContainer);
        }

        if (EngineVersion < 424)
        {
            return;
        }

        if (BuildingContainerName.contains("Barrel") && BuildingContainerName.contains("Rod"))
        {
            HandleFishingRodSearch(BuildingContainer);
        }
        else if (BuildingContainerName.contains("FactionChest"))
        {
            // Missing
        }
    }

    // TODO: Refactor this method and optimize it
    // It's kind of hard to not mess it up so it's staying like this for now
    inline void SetWeaponInVendingMachine(const TArray<long long>* const ItemCollections, const int Index, UObject* const& CollectorUnitInfoClass, const int& OutputItemOffset)
    {
        *reinterpret_cast<UObject**>(reinterpret_cast<long long>(reinterpret_cast<long long*>((
                static_cast<long long>(GetSizeOfStruct(CollectorUnitInfoClass)) * Index + reinterpret_cast<long long>(
                    ItemCollections->
                    GetData()))))
            + OutputItemOffset) = GetRandomItem(Item::Type::Weapon).Definition;
    }

    inline void FillVendingMachines(LPVOID) -> DWORD
    {
        static auto BuildingItemCollectorClass = FindObject("Class /Script/FortniteGame.BuildingItemCollectorActor");
        if (!BuildingItemCollectorClass)
        {
            return 0;
        }
        
        auto Actors = Helper::GetAllActorsOfClass(BuildingItemCollectorClass);
        for (int i = 0; i < Actors.Num(); i++)
        {
            const auto Actor = Actors[i];

            if (!Actor)
            {
                continue;
            }

            const auto ItemCollections = Actor->Member<TArray<long long>>(("ItemCollections"));
            if (!ItemCollections)
            {
                continue;
            }


            static const auto CollectorUnitInfoClass = FindObject("ScriptStruct /Script/FortniteGame.CollectorUnitInfo");
            if (!CollectorUnitInfoClass)
            {
                continue;
            }

            static const auto OutputItemOffset = FindOffsetStruct("ScriptStruct /Script/FortniteGame.CollectorUnitInfo", "OutputItem");
            if (!OutputItemOffset)
            {
                continue;
            }
            
            SetWeaponInVendingMachine(ItemCollections, 0, CollectorUnitInfoClass, OutputItemOffset);
            SetWeaponInVendingMachine(ItemCollections, 1, CollectorUnitInfoClass, OutputItemOffset);
            SetWeaponInVendingMachine(ItemCollections, 2, CollectorUnitInfoClass, OutputItemOffset);
        }

        Actors.Free();
        return 0;
    }
}
