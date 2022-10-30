#pragma once
#include <vector>
#include <random>

#include <Gameplay/helper.h>
#include <UE/DataTables.h>

enum class ItemType
{
	None,
	Weapon,
	Consumable,
	Ammo,
	Resource,
	Trap
};

std::string ItemTypeToString(ItemType type)
{
	switch (type)
	{
		using enum ItemType;
	case Weapon:
		return "Weapon";
	case Consumable:
		return "Consumable";
	case Ammo:
		return "Ammo";
	case Resource:
		return "Resource";
	case Trap:
		return "Trap";
	default:
		return "NULL ItemType";
	}
}

struct DefinitionInRow // 50 bytes
{
	UObject* Definition = nullptr;
	float Weight;
	int DropCount = 1;
	std::string RowName;
	ItemType Type = ItemType::None;

	std::string Describe(bool bNewLines = false)
	{
		if (bNewLines)
			return std::format("{}\n{}\n{}\n{}\n{}\n", Definition ? Definition->GetFullName() : "NULL Definition", std::to_string(Weight), std::to_string(DropCount), RowName, ItemTypeToString(Type));
		else
			return std::format("{} {} {} {} {}", Definition ? Definition->GetFullName() : "NULL Definition", std::to_string(Weight), std::to_string(DropCount), RowName, ItemTypeToString(Type));
	}
};

const DefinitionInRow* cumulative_weighted_choice(const std::vector<float>& weights, const std::vector<DefinitionInRow>& values) {
	static std::random_device hardware_seed;
	static std::mt19937_64 engine{ hardware_seed() };

	if (weights.size() != values.size())
	{
		MessageBoxA(0, "Weights and values size mismatch!", "Reboot", MB_ICONERROR);
		return nullptr;
	}

	assert(weights.size() == values.size());
	const auto max_weight{ weights.back() };
	std::uniform_real_distribution < float > distribution{ 0.0, max_weight };
	const auto raw_weight{ distribution(engine) };
	const auto valid_weight{ std::lower_bound(weights.cbegin(), weights.cend(), raw_weight) };
	const auto result_idx{ std::distance(weights.cbegin(), valid_weight) };
	return &values[result_idx];
}

bool RandomBoolWithWeight(float Weight, float Min = 0.f, float Max = 1.f)
{
	//If the Weight equals to 0.0f then always return false
	if (Weight <= 0.0f)
	{
		return false;
	}
	else
	{
		//If the Weight is higher or equal to the random number then return true
		std::random_device rd; // obtain a random number from hardware
		std::mt19937 gen(rd()); // seed the generator

		std::uniform_int_distribution<> distr(Min, Max);
		return Weight >= distr(gen);
	}
}

namespace LootingV2
{
	// static std::vector<std::map<std::vector<DefinitionInRow>, std::vector<float>>> Items; // best way? Probably not
	static std::vector<std::vector<DefinitionInRow>> Items; // best way? Probably not
	static std::vector<std::vector<float>> Weights;
	static bool bInitialized = false;

	// Position in Items
	int LootItems = 0;
	int SupplyDropItems = 1;
	int LlamaItems = 2;
	int FactionLootItems = 3;

	void AddItemAndWeight(int Index, const DefinitionInRow& Item, float Weight)
	{
		if (Index > Items.size() || Index > Weights.size())
			return;

		Items[Index].push_back(Item);
		Weights[Index].push_back(Weight);
	}

	static DWORD WINAPI InitializeWeapons(LPVOID)
	{
		/* if (bRestarting)
		{
			Items.empty();
			Weights.empty();
			bInitialized = false;
		} */

		if (LootingV2::bInitialized)
		{
			std::cout << "[WARNING] Loot is already initialized!\n";
			return 0;
		}

		if (!StaticFindObjectO)
		{
			std::cout << "Not going to initialize looting because it will be too slow!\n";
			return 0;
		}

		LootingV2::bInitialized = true;

		for (int i = 0; i < 2; i++)
		{
			Items.push_back(std::vector<DefinitionInRow>());
		}

		for (int i = 0; i < 2; i++)
		{
			Weights.push_back(std::vector<float>());
		}

		// Items.reserve(2);

		auto LootPackages = GetLootPackages();

		if (!LootPackages)
		{
			std::cout << "Failed to get LootPackages!\n";
			return 1;
		}

		auto LootPackagesRowMap = GetRowMap(LootPackages);

		auto fortnite = LootPackagesRowMap.Pairs.Elements.Data;

		std::cout << "Amount of rows: " << fortnite.Num() << '\n';

		for (int i = 0; i < fortnite.Num() - 1; i++)
		{
			auto& Man = fortnite.At(i);
			auto& Pair = Man.ElementData.Value;
			auto RowFName = Pair.First;

			if (!RowFName.ComparisonIndex)
				continue;

			auto RowName = RowFName.ToString();
			auto LootPackageDataOfRow = Pair.Second; // ScriptStruct FortniteGame.FortLootPackageData

			if (LootPackageDataOfRow) // pretty sure this is wrong // todo: check rowisa athenaloot package or something io think
			{
				int Index = -1;

				if (RowName.starts_with("WorldList.AthenaLoot"))
					Index = LootItems;
				else if (RowName.starts_with("WorldPKG.AthenaSupplyDrop")) // wrong
					Index = SupplyDropItems;
				else if (RowName.starts_with("WorldList.FactionLoot"))
					Index = FactionLootItems;
				// else if (RowName.starts_with("WorldList.AthenaLlama"))
					// Index = LlamaItems;

				if (Index == -1)
					continue;

				static auto off = FindOffsetStruct("ScriptStruct /Script/FortniteGame.FortLootPackageData", "ItemDefinition");
				static auto countOff = FindOffsetStruct("ScriptStruct /Script/FortniteGame.FortLootPackageData", "Count");
				static auto weightOff = FindOffsetStruct("ScriptStruct /Script/FortniteGame.FortLootPackageData", "Weight");

				auto ItemDef = (TSoftObjectPtr*)(__int64(LootPackageDataOfRow) + off);
				auto Count = (int*)(__int64(LootPackageDataOfRow) + countOff);
				auto Weight = (float*)(__int64(LootPackageDataOfRow) + weightOff);

				// std::cout << std::format("Count: {} ItemDef: {}\n", *Count, ItemDef->ObjectID.AssetPathName.ToString());

				DefinitionInRow currentItem;

				if (ItemDef && ItemDef->ObjectID.AssetPathName.ComparisonIndex)
				{
					auto DefinitionString = ItemDef->ObjectID.AssetPathName.ToString();
					currentItem.Definition = FindObject(DefinitionString);

					if (Count)
						currentItem.DropCount = *Count;

					if (Weight)
						currentItem.Weight = *Weight;

					currentItem.RowName = RowName;

					// brain damage

					constexpr bool bAllowContextTraps = false;

					if (DefinitionString.contains("Weapon"))
						currentItem.Type = ItemType::Weapon;
					else if (DefinitionString.contains("Consumable"))
						currentItem.Type = ItemType::Consumable;
					else if (DefinitionString.contains("Ammo"))
						currentItem.Type = ItemType::Ammo;
					else if (DefinitionString.contains("ResourcePickups"))
						currentItem.Type = ItemType::Resource;
					else if (DefinitionString.contains("TID") && bAllowContextTraps ? true : !DefinitionString.contains("Context"))
						currentItem.Type = ItemType::Trap;

					if (Weight)
						AddItemAndWeight(Index, currentItem, *Weight);
				}
			}
		}

		std::cout << "Initialized Looting V2!\n";

		return 0;
	}

	static const DefinitionInRow GetRandomItem(ItemType Type, int LootType = LootItems)
	{
		if (LootType <= Items.size() && Items.size() >= 1 ? Items[LootType].empty() : true)
		{
			std::cout << std::format("[WARNING] Tried getting a {} with loot type {} but the table is null!\n", ItemTypeToString(Type), std::to_string(LootType));
			return DefinitionInRow();
		}

		auto& TableToUse = Items[LootType];

		// auto Item = cumulative_weighted_choice(Weights[LootType], TableToUse);

		int current = 0;

		while (current < 5000) // it shouldnt even be this much
		{
			auto& Item = TableToUse[rand() % (TableToUse.size())];

			if (Item.Type == Type && RandomBoolWithWeight(Item.Weight) && Item.Definition)
				return Item;

			current++;
		}

		return DefinitionInRow();
	}
	
	int SpawnFloorLoot(UObject* Class, int SleepTimer = 0)
	{
		if (!Class)
			return 0;

		auto ClassActors = Helper::GetAllActorsOfClass(Class);

		for (int i = 0; i < ClassActors.Num(); i++)
		{
			auto ClassActor = ClassActors.At(i);

			if (ClassActor)
			{
				constexpr bool bTossPickup = true;
				bool ShouldSpawn = RandomBoolWithWeight(0.3f);

				if (ShouldSpawn)
				{
					auto CorrectLocation = Helper::GetActorLocation(ClassActor);
					CorrectLocation.Z += 50;

					UObject* MainPickup = nullptr;

					if (RandomBoolWithWeight(6, 1, 100))
					{
						auto Ammo = GetRandomItem(ItemType::Ammo);

						MainPickup = Helper::SummonPickup(nullptr, Ammo.Definition, CorrectLocation, EFortPickupSourceTypeFlag::FloorLoot,
							EFortPickupSpawnSource::Unset, Ammo.DropCount, bTossPickup, false);
					}

					else if (RandomBoolWithWeight(5, 1, 100))
					{
						auto Trap = GetRandomItem(ItemType::Trap);

						MainPickup = Helper::SummonPickup(nullptr, Trap.Definition, CorrectLocation, EFortPickupSourceTypeFlag::FloorLoot,
							EFortPickupSpawnSource::Unset, Trap.DropCount, bTossPickup, false);
					}

					else if (RandomBoolWithWeight(26, 1, 100))
					{
						auto Consumable = GetRandomItem(ItemType::Consumable);

						MainPickup = Helper::SummonPickup(nullptr, Consumable.Definition, CorrectLocation, EFortPickupSourceTypeFlag::FloorLoot,
							EFortPickupSpawnSource::Unset, Consumable.DropCount, bTossPickup, true);
					}

					else
					{
						auto Weapon = GetRandomItem(ItemType::Weapon);

						MainPickup = Helper::SummonPickup(nullptr, Weapon.Definition, CorrectLocation, EFortPickupSourceTypeFlag::FloorLoot, EFortPickupSpawnSource::Unset, 1, bTossPickup);

						auto AmmoDef = GetAmmoForDefinition(Weapon.Definition);

						Helper::SummonPickup(nullptr, AmmoDef.first, CorrectLocation, EFortPickupSourceTypeFlag::FloorLoot,
							EFortPickupSpawnSource::Unset, AmmoDef.second, bTossPickup, false);
					}

					if (!MainPickup)
						Sleep(SleepTimer * 10);

					std::cout << "I: " << i << '\n';

					Sleep(Engine_Version >= 422 ? SleepTimer : 1);
				}
			}
		}

		int Num = ClassActors.Num();

		ClassActors.Free();

		return Num;
	}


	static DWORD WINAPI SpawnVehicles(LPVOID)
	{
		if (!StaticLoadObjectO)
		{
			std::cout << "No StaticLoadObject!\n";
			return 1;
		}

		// BlueprintGeneratedClass /Game/Athena/DrivableVehicles/Athena_QuadSpawner.Athena_QuadSpawner_C
		// BlueprintGeneratedClass /Game/Athena/DrivableVehicles/Athena_OctopusSpawner.Athena_OctopusSpawner_C
		// BlueprintGeneratedClass /Game/Athena/DrivableVehicles/Athena_BiplaneSpawner.Athena_BiplaneSpawner_C
		// BlueprintGeneratedClass /Game/Athena/DrivableVehicles/Athena_JackalSpawner.Athena_JackalSpawner_C
		// BlueprintGeneratedClass /Game/Athena/DrivableVehicles/Athena_CartSpawner.Athena_CartSpawner_C
		// World /Game/Athena/Maps/Athena_DroneSpawners.Athena_DroneSpawners

		static auto FortVehicleSpawnerClass = FindObject("Class /Script/FortniteGame.FortAthenaVehicleSpawner");
		static auto BoatSpawnerClass = FindObject("BlueprintGeneratedClass /Game/Athena/DrivableVehicles/Meatball/Athena_Meatball_L_Spawner.Athena_Meatball_L_Spawner_C"); // Boat

		auto spawnerClass = BoatSpawnerClass;

		auto Spawners = Helper::GetAllActorsOfClass(spawnerClass);

		std::cout << "Spawning: " << Spawners.Num() << " vehicles\n";

		for (int i = 0; i < Spawners.Num(); i++)
		{
			auto Spawner = Spawners[i];

			if (Spawner)
			{
				// auto VehicleClassSoft = Spawner->Member<TSoftClassPtr>("VehicleClass");

				{
					auto SpawnerLoc = Helper::GetActorLocation(Spawner);
					// std::cout << std::format("Spawning {} at {} {} {}", VehicleName, SpawnerLoc.X, SpawnerLoc.Y, SpawnerLoc.Z);
					std::cout << "Loading!\n";
					UObject* VehicleClass = LoadObject(Helper::GetBGAClass(), nullptr, "/Game/Athena/DrivableVehicles/Meatball/Meatball_Large/MeatballVehicle_L.MeatballVehicle_L");
					std::cout << "Loaded: " << VehicleClass << '\n';

					if (VehicleClass)
					{
						Easy::SpawnActor(VehicleClass, SpawnerLoc, Helper::GetActorRotation(Spawner));
					}
					else
						std::cout << "No vehicle class!\n";
				}
			}
		}

		Spawners.Free();

		std::cout << "Spawned vehicles!\n";

		return 0;
	}

	DWORD WINAPI SummonFloorLoot(LPVOID)
	{
		int amountSpawned = 0;

		/* constexpr bool dehh = true;

		if (!dehh)
		{
			static auto BuildingContainerClass = FindObject("Class /Script/FortniteGame.BuildingContainer");

			if (BuildingContainerClass)
			{
				// std::cout << "aa!\n";
				auto BuildingContainers = Helper::GetAllActorsOfClass(BuildingContainerClass);
				// std::cout << "bb!\n";

				// std::cout << "Spawning: " << BuildingContainers.Num() << '\n';

				for (int i = 0; i < BuildingContainers.Num(); i++)
				{
					auto BuildingContainer = BuildingContainers.At(i);

					if (BuildingContainer && BuildingContainer->GetFullName().contains("Tiered_Athena_FloorLoot_"))
					{
						constexpr bool bTossPickup = true;
						bool ShouldSpawn = RandomBoolWithWeight(0.3f);

						if (ShouldSpawn)
						{
							auto CorrectLocation = Helper::GetActorLocation(BuildingContainer);
							CorrectLocation.Z += 50;

							if (RandomBoolWithWeight(0.93f))
							{
								auto Ammo = GetRandomItem(ItemType::Ammo);

								Helper::SummonPickup(nullptr, Ammo.Definition, CorrectLocation, EFortPickupSourceTypeFlag::FloorLoot,
									EFortPickupSpawnSource::Unset, Ammo.DropCount, bTossPickup);
							}
							else if (RandomBoolWithWeight(0.85f))
							{
								auto Consumable = GetRandomItem(ItemType::Consumable);

								Helper::SummonPickup(nullptr, Consumable.Definition, CorrectLocation, EFortPickupSourceTypeFlag::FloorLoot,
									EFortPickupSpawnSource::Unset, Consumable.DropCount, bTossPickup);
							}
							else
							{
								auto Weapon = GetRandomItem(ItemType::Weapon);

								auto WeaponPickup = Helper::SummonPickup(nullptr, Weapon.Definition, CorrectLocation, EFortPickupSourceTypeFlag::FloorLoot, EFortPickupSpawnSource::Unset, 1, bTossPickup);

								static auto GetAmmoWorldItemDefinition_BP = Weapon.Definition->Function(("GetAmmoWorldItemDefinition_BP"));

								if (GetAmmoWorldItemDefinition_BP && WeaponPickup)
								{
									struct { UObject* AmmoDefinition; }GetAmmoWorldItemDefinition_BP_Params{};
									Weapon.Definition->ProcessEvent(GetAmmoWorldItemDefinition_BP, &GetAmmoWorldItemDefinition_BP_Params);
									auto AmmoDef = GetAmmoWorldItemDefinition_BP_Params.AmmoDefinition;
									static auto DropCountOffset = GetOffset(AmmoDef, "DropCount");

									auto DropCount = *(int*)(__int64(AmmoDef) + DropCountOffset);

									Helper::SummonPickup(nullptr, AmmoDef, CorrectLocation, EFortPickupSourceTypeFlag::FloorLoot,
										EFortPickupSpawnSource::Unset, DropCount, bTossPickup, false);
								}
							}

							std::cout << "I: " << i << '\n';

							amountSpawned++;
						}
					}
				}

				BuildingContainers.Free();
			}
		}
		else */
		{
			amountSpawned += SpawnFloorLoot(FindObject("BlueprintGeneratedClass /Game/Athena/Environments/Blueprints/Tiered_Athena_FloorLoot_Warmup.Tiered_Athena_FloorLoot_Warmup_C"), 13);
			amountSpawned += SpawnFloorLoot(FindObject("BlueprintGeneratedClass /Game/Athena/Environments/Blueprints/Tiered_Athena_FloorLoot_01.Tiered_Athena_FloorLoot_01_C"), 15); // we can take our time
		}

		std::cout << "Finished spawning " << amountSpawned << " floorloot!\n";

		return 0;
	}

	static void HandleSearch(UObject* BuildingContainer)
	{
		auto GetCorrectLocation = [BuildingContainer]() -> FVector {
			auto Location = Helper::GetActorLocation(BuildingContainer);
			auto RightVector = Helper::GetActorRightVector(BuildingContainer);

			// static auto LootSpawnLocationOffset = GetOffset(BuildingContainer, "LootSpawnLocation");

			// return Location + RightVector * 70.0f + *(FVector*)(__int64(BuildingContainer) + LootSpawnLocationOffset);

			return Location + RightVector * 70.0f + FVector{0, 0, 50};
		};

		static auto LlamaClass = FindObject("BlueprintGeneratedClass /Game/Athena/SupplyDrops/Llama/AthenaSupplyDrop_Llama.AthenaSupplyDrop_Llama_C");

		if (BuildingContainer)
		{
			auto BuildingContainerName = BuildingContainer->GetName();

			if (BuildingContainerName.contains(("Tiered_Chest")) || BuildingContainerName.contains("LCD_Chest")) //  LCD_ToolBox // die
			{
				auto DefInRow = GetRandomItem(ItemType::Weapon);
				{
					auto WeaponDef = DefInRow.Definition;

					auto Ammo = GetAmmoForDefinition(WeaponDef);

					{

						{
							auto Location = Helper::GetCorrectLocation(BuildingContainer);

							if (WeaponDef)
								Helper::SummonPickup(nullptr, WeaponDef, Location, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, DefInRow.DropCount);

							{
								Helper::SummonPickup(nullptr, Ammo.first, Location, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, Ammo.second, true, false);
							}

							auto ConsumableInRow = RandomBoolWithWeight(5, 1, 100) ? GetRandomItem(ItemType::Trap) : GetRandomItem(ItemType::Consumable);

							if (ConsumableInRow.Definition)
							{
								Helper::SummonPickup(nullptr, ConsumableInRow.Definition, Location, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, ConsumableInRow.DropCount, true, false); // *Consumable->Member<int>(("DropCount")));
							}

							static auto WoodItemData = FindObject(("FortResourceItemDefinition /Game/Items/ResourcePickups/WoodItemData.WoodItemData"));
							static auto StoneItemData = FindObject(("FortResourceItemDefinition /Game/Items/ResourcePickups/StoneItemData.StoneItemData"));
							static auto MetalItemData = FindObject(("FortResourceItemDefinition /Game/Items/ResourcePickups/MetalItemData.MetalItemData"));

							auto random = rand() % 3;

							int amountOfMaterialToDrop = GetRandomItem(ItemType::Resource, LootItems).DropCount;

							if (random == 1)
								Helper::SummonPickup(nullptr, WoodItemData, Location, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, amountOfMaterialToDrop, true, false);
							else if (random == 2)
								Helper::SummonPickup(nullptr, StoneItemData, Location, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, amountOfMaterialToDrop, true, false);
							else
								Helper::SummonPickup(nullptr, MetalItemData, Location, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, amountOfMaterialToDrop, true, false);
						}
					}
				}
			}

			else if (BuildingContainerName.contains(("Ammo")) || BuildingContainerName.contains("Tiered_Short"))
			{
				auto AmmoInRow = GetRandomItem(ItemType::Ammo);
				auto AmmoDef = AmmoInRow.Definition;

				if (AmmoDef)
				{
					auto Location = Helper::GetCorrectLocation(BuildingContainer);

					auto DropCount = AmmoInRow.DropCount; // *AmmoDef->Member<int>(("DropCount"));
					Helper::SummonPickup(nullptr, AmmoDef, Location, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::AmmoBox, DropCount, true, false);
				}
			}

			else if (BuildingContainer->IsA(LlamaClass))
			{
				static auto WoodItemData = FindObject(("FortResourceItemDefinition /Game/Items/ResourcePickups/WoodItemData.WoodItemData"));
				static auto StoneItemData = FindObject(("FortResourceItemDefinition /Game/Items/ResourcePickups/StoneItemData.StoneItemData"));
				static auto MetalItemData = FindObject(("FortResourceItemDefinition /Game/Items/ResourcePickups/MetalItemData.MetalItemData"));

				auto Location = Helper::GetCorrectLocation(BuildingContainer); // GetLootSpawnLocation

				{
					static auto Minis = FindObject(("FortWeaponRangedItemDefinition /Game/Athena/Items/Consumables/ShieldSmall/Athena_ShieldSmall.Athena_ShieldSmall"));

					auto CountToDrop = 200; // GetRandomItem(ItemType::Resource, LlamaItems).DropCount;

					Helper::SummonPickup(nullptr, WoodItemData, Location, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, CountToDrop);
					Helper::SummonPickup(nullptr, StoneItemData, Location, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, CountToDrop);
					Helper::SummonPickup(nullptr, MetalItemData, Location, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, CountToDrop);
					Helper::SummonPickup(nullptr, Minis, Location, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, CountToDrop);

					// OnRep_Looted
					// SpawnLoot
				}
			}

			else if (BuildingContainerName.contains("AthenaSupplyDrop_C"))
			{				
				auto CorrectLocation = Helper::GetCorrectLocation(BuildingContainer);

				for (int i = 0; i < 5; i++)
				{
					Helper::SummonPickup(nullptr, GetRandomItem(ItemType::Weapon).Definition, CorrectLocation, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, 1);
				}
			}

			if (Engine_Version >= 424) // chapter 2 specific stuff
			{
				if (BuildingContainerName.contains("Barrel") && BuildingContainerName.contains("Rod"))
				{
					static auto FishingRodWID = FindObject(("FortWeaponRangedItemDefinition /Game/Athena/Items/Consumables/FloppingRabbit/WID_Athena_FloppingRabbit.WID_Athena_FloppingRabbit"));

					if (FishingRodWID)
					{
						// todo: make amount random

						Helper::SummonPickup(nullptr, FishingRodWID, Helper::GetActorLocation(BuildingContainer), EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Unset, 1);
						Helper::SummonPickup(nullptr, FishingRodWID, Helper::GetActorLocation(BuildingContainer), EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Unset, 1);
						Helper::DestroyActor(BuildingContainer); // todo: not
					}
				}

				else if (BuildingContainerName.contains("FactionChest")) // IO Chests
				{

				}
				
				else if (BuildingContainerName.contains("AthenaSupplyDrop_Vault"))
				{

				}
			}

			if (!BuildingContainerName.contains(("Door")) && !BuildingContainerName.contains(("Wall")))
			{
				std::cout << ("Container: ") << BuildingContainerName << "!\n";
			}
		}
	}

	static DWORD WINAPI FillVendingMachines(LPVOID)
	{
		static auto BuildingItemCollectorClass = FindObject(("Class /Script/FortniteGame.BuildingItemCollectorActor"));

		if (BuildingItemCollectorClass)
		{
			auto Actors = Helper::GetAllActorsOfClass(BuildingItemCollectorClass);

			std::cout << ("Filling ") << Actors.Num() << (" vending machines!\n");

			for (int i = 0; i < Actors.Num(); i++)
			{
				auto Actor = Actors[i];

				if (Actor)
				{
					TArray<__int64>* ItemCollections = Actor->Member<TArray<__int64>>(("ItemCollections")); // CollectorUnitInfo

					if (ItemCollections)
					{
						static UObject* CollectorUnitInfoClass = FindObject(("ScriptStruct /Script/FortniteGame.CollectorUnitInfo"));
						static std::string CollectorUnitInfoClassName = ("ScriptStruct /Script/FortniteGame.CollectorUnitInfo");

						if (!CollectorUnitInfoClass)
						{
							CollectorUnitInfoClassName = ("ScriptStruct /Script/FortniteGame.ColletorUnitInfo"); // die fortnite
							CollectorUnitInfoClass = FindObject(CollectorUnitInfoClassName); // Wedc what this value is
						}

						static auto OutputItemOffset = FindOffsetStruct(CollectorUnitInfoClassName, ("OutputItem"));

						std::cout << ("Offset: ") << OutputItemOffset << '\n';

						// So this is equal to Array[1] + OutputItemOffset, but since the array is __int64, it doesn't calcuate it properly so we have to implement it ourselves

						auto Size = GetSizeOfStruct(CollectorUnitInfoClass);

						*TArrayAt<UObject*, __int64>(ItemCollections, 1, Size, OutputItemOffset) = LootingV2::GetRandomItem(ItemType::Weapon).Definition;
						*TArrayAt<UObject*, __int64>(ItemCollections, 2, Size, OutputItemOffset) = LootingV2::GetRandomItem(ItemType::Weapon).Definition;
						*TArrayAt<UObject*, __int64>(ItemCollections, 3, Size, OutputItemOffset) = LootingV2::GetRandomItem(ItemType::Weapon).Definition;

						// *(UObject**)(__int64((__int64*)((__int64(ItemCollections->GetData()) + (GetSizeOfStruct(CollectorUnitInfoClass) * 0)))) + OutputItemOffset) = LootingV2::GetRandomItem(ItemType::Weapon).Definition;
						// *(UObject**)(__int64((__int64*)((__int64(ItemCollections->GetData()) + (GetSizeOfStruct(CollectorUnitInfoClass) * 1)))) + OutputItemOffset) = LootingV2::GetRandomItem(ItemType::Weapon).Definition;
						// *(UObject**)(__int64((__int64*)((__int64(ItemCollections->GetData()) + (GetSizeOfStruct(CollectorUnitInfoClass) * 2)))) + OutputItemOffset) = LootingV2::GetRandomItem(ItemType::Weapon).Definition;
					}
					else
						std::cout << ("ItemCollections Invalid: ") << ItemCollections << '\n';
				}
				else
					std::cout << ("Invalid Vending Actor! Index: ") << i << '\n';
			}

			Actors.Free();
		}
		else
			std::cout << "Unable to find BuildingItemCollectorClass!\n";

		std::cout << ("Finished filling vending machines!\n");

		return 0;
	}

	void SpawnForagedItems()
	{
		static auto BGAConsumableSpawnerClass = FindObject("Class /Script/FortniteGame.BGAConsumableSpawner");

		auto AllActors = Helper::GetAllActorsOfClass(BGAConsumableSpawnerClass);

		for (int i = 0; i < AllActors.Num(); i++)
		{
			auto AllActor = AllActors.At(i);

			if (AllActor)
			{
				auto SpawnLootTierGroupFName = AllActor->Member<FName>("SpawnLootTierGroup");

				auto SpawnLootTierGroup = SpawnLootTierGroupFName->ToString();

				std::cout << "SpawnLootTierGroup: " << SpawnLootTierGroup << '\n';

				auto Location = Helper::GetActorLocation(AllActor);

				continue;
	
				// MF LOADOBJECT WORKS 1/100 TIMES

				if (SpawnLootTierGroup == "Loot_ForagedItem_AthenaRift")
				{
					static auto RiftBCWID = FindObject("BGAConsumableWrapperItemDefinition /Game/Athena/Items/ForagedItems/Rift/ConsumableVersion/Athena_Foraged_Rift.Athena_Foraged_Rift");
					std::cout << "Rift Location: " << Location.Describe() << '\n';
					Helper::SpawnBGAConsumable(RiftBCWID, Location,
						"/Game/Athena/Items/ForagedItems/Rift/BGA_RiftPortal_Athena.BGA_RiftPortal_Athena_C");
				}

				if (SpawnLootTierGroup == "Loot_ForagedItem_Glitch")
				{
					static auto GlitchedBCWID = FindObject("BGAConsumableWrapperItemDefinition /Game/Athena/Items/ForagedItems/Glitch/Athena_Foraged_Glitch.Athena_Foraged_Glitch");

					// std::cout << "Rift Location: " << Location.Describe() << '\n';
					Helper::SpawnBGAConsumable(GlitchedBCWID, Location,
						"/Game/Athena/Items/ForagedItems/Glitch/CBGA_Glitch.CBGA_Glitch_C");
				}

				if (SpawnLootTierGroup == "Loot_ForagedItem_SpookyMist") // Cube Consumable
				{
					static auto SpookyMistBCWID = FindObject("BGAConsumableWrapperItemDefinition /Game/Athena/Items/ForagedItems/SpookyMist/Athena_Foraged_SpookyMist.Athena_Foraged_SpookyMist");
					Helper::SpawnBGAConsumable(SpookyMistBCWID, Helper::GetActorLocation(AllActor),
						"/Game/Athena/Items/ForagedItems/SpookyMist/CBGA_SpookyMist.CBGA_SpookyMist_C");
				}

				if (SpawnLootTierGroup == "Loot_ForagedItem_Grassland") // Apple
				{
					static auto GrasslandBCWID = FindObject("BGAConsumableWrapperItemDefinition /Game/Athena/Items/ForagedItems/HealthSmall/Athena_Foraged_HealthSmall.Athena_Foraged_HealthSmall");
					Helper::SpawnBGAConsumable(GrasslandBCWID, Helper::GetActorLocation(AllActor),
						"/Game/Athena/Items/ForagedItems/HealthSmall/CBGA_HealthSmall.CBGA_HealthSmall_C");
				}

				else if (SpawnLootTierGroup == "Loot_ForagedItem_Arid") // Apple again?
				{
					static auto AridBCWID = FindObject("BGAConsumableWrapperItemDefinition /Game/Athena/Items/ForagedItems/HealthSmall/Athena_Foraged_HealthSmall.Athena_Foraged_HealthSmall");
					Helper::SpawnBGAConsumable(AridBCWID, Helper::GetActorLocation(AllActor),
						"/Game/Athena/Items/ForagedItems/HealthSmall/CBGA_HealthSmall.CBGA_HealthSmall_C");
				}

				else if (SpawnLootTierGroup == "Loot_ForagedItem_Farmland")
				{

				}

				else if (SpawnLootTierGroup == "Loot_ForagedItem_TempEvent")
				{

				}

				else if (SpawnLootTierGroup == "Loot_ForagedItem_Jungle")
				{

				}

				else if (SpawnLootTierGroup == "Loot_ForagedItem_Mountain")
				{

				}

				else if (SpawnLootTierGroup == "Loot_ForagedItem_Forest")
				{

				}

				else if (SpawnLootTierGroup == "Loot_ForagedItem_Swamp")
				{

				}
			}
		}
	}
}
