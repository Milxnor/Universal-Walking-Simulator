#pragma once

#include <Gameplay/helper.h>
#include <Gameplay/inventory.h>
#include <UE/DataTables.h>

void DoHarvesting(UObject* Controller, UObject* BuildingActor, float Damage = 0.f)
{
	static auto ResourceTypeOffset = GetOffset(BuildingActor, "ResourceType");
	auto ResourceType = *(TEnumAsByte<EFortResourceType>*)(__int64(BuildingActor) + ResourceTypeOffset);

	// static auto bDestroyedOffset = GetOffset(BuildingActor, "bDestroyed");
	// static auto bDestroyedBI = GetBitIndex(GetProperty(BuildingActor, "bDestroyed"));

	auto bDestroyed = false; // readd((uint8_t*)(__int64(BuildingActor) + bDestroyedOffset), bDestroyedBI);

	if (bDestroyed)
		return;

	static auto CarClass = FindObject("BlueprintGeneratedClass /Game/Building/ActorBlueprints/Prop/Car_DEFAULT.Car_DEFAULT_C");

	if (!CarClass)
		CarClass = FindObject("BlueprintGeneratedClass /Game/Building/ActorBlueprints/Prop/Car_Copper.Car_Copper_C");

	bool bIsCar = BuildingActor->IsA(CarClass);

	std::cout << "pock rtock!\n";

	// HasDestructionLoot

	// int amoutnagug = 5;

	if (!bIsCar)
	{
		static auto BuildingResourceAmountOverrideOffset = GetOffset(BuildingActor, "BuildingResourceAmountOverride");
		auto BuildingResourceAmountOverride = (FCurveTableRowHandle*)(__int64(BuildingActor) + BuildingResourceAmountOverrideOffset);

		if (!BuildingResourceAmountOverride->RowName.ComparisonIndex) // player placed replacement
			return;

		struct skunkedCurveTable : public UObject {
			void* idkk;
			TMap<FName, FRealCurve*>	RowMap;
		};

		// ClientRemotePlayerDamagedResourceBuilding

		/* static auto curveTableClass = FindObject("Class /Script/Engine.CurveTable");
		skunkedCurveTable* curveTable = (skunkedCurveTable*)BuildingResourceAmountOverride->CurveTable;

		if (!curveTable)
		{
			std::cout << "unable to find curvetable!\n";
			return;
		} */

		/* for (int i = 0; i < 15; i++)
		{
			auto rowMap = (TMap<FName, FRealCurve*>*)(__int64(curveTable) + offsetToRowMap + i);

			if (rowMap)
				std::cout << std::format("[{}] RowMap Size: {}\n", i,rowMap->Pairs.Elements.Data.Num());
			else
				std::cout << std::format("[{}] Invalid pointer!\n", i);
		}

		auto rowMap = &curveTable->RowMap; // (TMap<FName, FRealCurve*>*)(__int64(curveTable) + offsetToRowMap);

		std::cout << "Amount of rows: " << rowMap->Pairs.Elements.Data.Num() << '\n';

		auto rowToFind = BuildingResourceAmountOverride->RowName;

		for (int i = 0; i < rowMap->Pairs.Elements.Data.Num(); i++)
		{
			auto& currentRow = rowMap->Pairs.Elements.Data.At(i).ElementData.Value;

			// std::cout << std::format("[{}] {}\n", i, currentRow.First.ToString());

			if (currentRow.First.ComparisonIndex == rowToFind)
			{
				// help
				std::cout << "mf!\n";

				amoutnagug = currentRow.Second->GetDefaultValue();

				std::cout << "PROPER L: " << amoutnagug << '\n';
				break;
			}
		}

		// float Y;
		// BuildingResourceAmountOverride->Eval(1.0, &Y, FString()); */
	}

	std::random_device rd; // obtain a random number from hardware
	std::mt19937 gen(rd()); // seed the generator
	auto MaxResourcesToSpawn = 6; // *BuildingActor->Member<int>("MaxResourcesToSpawn");
	std::uniform_int_distribution<> distr(MaxResourcesToSpawn / 2, MaxResourcesToSpawn); // define the range

	auto Random = distr(gen);

	auto HitWeakspot = (Damage) == 100.f;

	auto funne = distr(gen);

	if (HitWeakspot)
	{
		std::random_device rd; // obtain a random number from hardware
		std::mt19937 gen(rd()); // seed the generator
		std::uniform_int_distribution<> distr(MaxResourcesToSpawn / 2, MaxResourcesToSpawn); // define the range

		funne += distr(gen);
	}

	// std::cout << "StaticGameplayTags: " << BuildingActor->Member<FGameplayTagContainer>("StaticGameplayTags")->ToStringSimple(true) << '\n';

	struct
	{
		UObject* BuildingSMActor;                                          // (Parm, ZeroConstructor, IsPlainOldData)
		TEnumAsByte<EFortResourceType>                     PotentialResourceType;                                    // (Parm, ZeroConstructor, IsPlainOldData)
		int                                                PotentialResourceCount;                                   // (Parm, ZeroConstructor, IsPlainOldData)
		bool                                               bDestroyed;                                               // (Parm, ZeroConstructor, IsPlainOldData)
		bool                                               bJustHitWeakspot;                                         // (Parm, ZeroConstructor, IsPlainOldData)
	} AFortPlayerController_ClientReportDamagedResourceBuilding_Params{ BuildingActor, 
		bIsCar ? EFortResourceType::Metal : ResourceType,
		 funne, bDestroyed, HitWeakspot }; // ender weakspotrs

	static auto ClientReportDamagedResourceBuilding = Controller->Function(("ClientReportDamagedResourceBuilding"));

	if (ClientReportDamagedResourceBuilding)
	{
		auto Params = &AFortPlayerController_ClientReportDamagedResourceBuilding_Params;

		Controller->ProcessEvent(ClientReportDamagedResourceBuilding, &AFortPlayerController_ClientReportDamagedResourceBuilding_Params);

		auto Pawn = Helper::GetPawnFromController(Controller);

		static auto WoodItemData = FindObject(("FortResourceItemDefinition /Game/Items/ResourcePickups/WoodItemData.WoodItemData"));
		static auto StoneItemData = FindObject(("FortResourceItemDefinition /Game/Items/ResourcePickups/StoneItemData.StoneItemData"));
		static auto MetalItemData = FindObject(("FortResourceItemDefinition /Game/Items/ResourcePickups/MetalItemData.MetalItemData"));

		UObject* ItemDef = WoodItemData;

		if (Params->PotentialResourceType.Get() == EFortResourceType::Stone)
			ItemDef = StoneItemData;

		if (Params->PotentialResourceType.Get() == EFortResourceType::Metal)
			ItemDef = MetalItemData;

		auto MaterialInstance = Inventory::FindItemInInventory(Controller, ItemDef);

		int AmountToGive = Params->PotentialResourceCount;

		// IMPROPER, we should account weakspot here.

		auto CurrentWeapon = Helper::GetCurrentWeapon(Helper::GetPawnFromController(Controller));

		if (CurrentWeapon)
		{
			if (MaterialInstance && Pawn)
			{
				auto Entry = GetItemEntryFromInstance(MaterialInstance);

				// BUG: You lose some mats if you have like 998 or idfk
				if (*FFortItemEntry::GetCount(Entry) >= 999)
				{
					Helper::SummonPickup(Pawn, ItemDef, Helper::GetActorLocation(Pawn), EFortPickupSourceTypeFlag::Other, EFortPickupSpawnSource::Unset, AmountToGive, true, false);
					return;
				}
			}

			Inventory::GiveItem(Controller, ItemDef, EFortQuickBars::Secondary, -1, AmountToGive);
		}
		else
			std::cout << "ky!\n";
	}
}

inline bool OnDamageServerHook(UObject* BuildingActor, UFunction* Function, void* Parameters)
{
	static auto BuildingSMActorClass = FindObject(("Class /Script/FortniteGame.BuildingSMActor"));
	
	if (BuildingActor->IsA(BuildingSMActorClass)) // || BuildingActor->GetFullName().contains(("Car_")))
	{
		auto ahh = Function->GetFullName();

		auto InstigatedByOffset = FindOffsetStruct(ahh, ("InstigatedBy"));
		auto InstigatedBy = *(UObject**)(__int64(Parameters) + InstigatedByOffset);

		auto DamageCauserOffset = FindOffsetStruct(ahh, ("DamageCauser"));
		auto DamageCauser = *(UObject**)(__int64(Parameters) + DamageCauserOffset);

		auto DamageTagsOffset = FindOffsetStruct(ahh, "DamageTags");
		auto DamageTags = (FGameplayTagContainer*)(__int64(Parameters) + DamageTagsOffset);

		auto DamageOffset = FindOffsetStruct(ahh, "Damage");
		auto Damage = (float*)(__int64(Parameters) + DamageOffset);

		static auto FortPlayerControllerAthenaClass = FindObject(("Class /Script/FortniteGame.FortPlayerControllerAthena"));

		// if (DamageTags)
			// std::cout << ("DamageTags: ") << DamageTags->ToStringSimple(false) << '\n';

		static auto MeleeClass = FnVerDouble < 10.00 ? FindObject("BlueprintGeneratedClass /Game/Weapons/FORT_Melee/Blueprints/B_Melee_Generic.B_Melee_Generic_C") :
			FindObject("BlueprintGeneratedClass /Game/Weapons/FORT_Melee/Blueprints/B_Athena_Pickaxe_Generic.B_Athena_Pickaxe_Generic_C");
		
		std::cout << "AA!\n";

		if (DamageCauser && InstigatedBy && InstigatedBy->IsA(FortPlayerControllerAthenaClass))
		{
			std::cout << "DamageCauser name: " << DamageCauser->GetFullName() << '\n';

			if (DamageCauser->IsA(MeleeClass))
			{
				std::cout << "mann!\n";

				// auto CurrentWeapon = Helper::GetCurrentWeapon(Helper::GetPawnFromController(InstigatedBy));

				// if (CurrentWeapon)
				{
					DoHarvesting(InstigatedBy, BuildingActor, *Damage);
				}
			}
			else
			{
				for (auto SuperStruct = GetSuperStructOfClass(DamageCauser->ClassPrivate); SuperStruct; SuperStruct = GetSuperStructOfClass(SuperStruct))
				{
					std::cout << "SuperStruct: " << SuperStruct->GetFullName() << '\n';
				}
			}
		}
;	}

	return false;
}

inline bool BlueprintCanAttemptGenerateResourcesHook(UObject* BuildingActor, UFunction* Function, void* Parameters)
{
	// this might be right?

	struct parms {
		FGameplayTagContainer InTags;
		UObject* InstigatorController; // AController*
		bool ret;
	};

	auto Params = (parms*)Parameters;

	ProcessEventO(BuildingActor, Function, Params);

	static auto PlayerControllerClass = FindObject(("BlueprintGeneratedClass /Game/Athena/Athena_PlayerController.Athena_PlayerController_C"));

	if (!Params || !Params->InstigatorController || !Params->InstigatorController->IsA(PlayerControllerClass)) // thanks s18
		return true;

	auto PickaxeDef = Helper::GetPickaxeDef(Params->InstigatorController);
	auto CurrentWeapon = Helper::GetCurrentWeapon(Helper::GetPawnFromController(Params->InstigatorController));

	if (CurrentWeapon && Helper::GetWeaponData(CurrentWeapon) == PickaxeDef)
	{
		if (Params && Params->ret)
		{
			DoHarvesting(Params->InstigatorController, BuildingActor);
		}
	}

	return true;
}

void InitializeHarvestingHooks()
{
	if (Engine_Version > 424)
		AddHook("Function /Script/FortniteGame.BuildingSMActor.BlueprintCanAttemptGenerateResources", BlueprintCanAttemptGenerateResourcesHook);
	else
	{
		AddHook(("Function /Script/FortniteGame.BuildingActor.OnDamageServer"), OnDamageServerHook);

		if (FnVerDouble >= 8.00)
			AddHook("Function /Game/Building/ActorBlueprints/Prop/Car_DEFAULT.Car_DEFAULT_C.OnDamageServer", OnDamageServerHook);
		else
			AddHook("Function /Game/Building/ActorBlueprints/Prop/Car_Copper.Car_Copper_C.OnDamageServer", OnDamageServerHook);
	}
}