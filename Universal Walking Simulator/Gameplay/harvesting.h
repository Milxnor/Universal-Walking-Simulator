#pragma once

#include <Gameplay/helper.h>
#include <Gameplay/inventory.h>

void DoHarvesting(UObject* Controller, UObject* BuildingActor, float Damage = 0.f)
{
	// HasDestructionLoot

	struct FCurveTableRowHandle
	{
	public:
		UObject* CurveTable;
		FName RowName;
	};

	auto BuildingResourceAmountOverride = BuildingActor->Member<FCurveTableRowHandle>("BuildingResourceAmountOverride");

	if (!BuildingResourceAmountOverride->RowName.ComparisonIndex) // player placed replacement
		return;

	std::random_device rd;
	std::mt19937 gen(rd());
	auto MaxResourcesToSpawn = 6; // *BuildingActor->Member<int>("MaxResourcesToSpawn");
	std::uniform_int_distribution<> distr(MaxResourcesToSpawn / 2, MaxResourcesToSpawn);

	auto Random = distr(gen);

	auto HitWeakspot = (Damage) == 100.f;

	auto funne = distr(gen);

	if (HitWeakspot)
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> distr(MaxResourcesToSpawn / 2, MaxResourcesToSpawn);

		funne += distr(gen);
	}

	std::cout << "StaticGameplayTags: " << BuildingActor->Member<FGameplayTagContainer>("StaticGameplayTags")->ToStringSimple(true) << '\n';

	struct
	{
		UObject* BuildingSMActor;
		TEnumAsByte<EFortResourceType> PotentialResourceType;
		int PotentialResourceCount;
		bool bDestroyed;
		bool bJustHitWeakspot;
	} AFortPlayerController_ClientReportDamagedResourceBuilding_Params{ BuildingActor, *BuildingActor->Member<TEnumAsByte<EFortResourceType>>(("ResourceType")), funne, false, HitWeakspot }; // ender weakspotrs

	static auto ClientReportDamagedResourceBuilding = Controller->Function(("ClientReportDamagedResourceBuilding"));

	if (ClientReportDamagedResourceBuilding)
	{
		auto Params = &AFortPlayerController_ClientReportDamagedResourceBuilding_Params;

		Controller->ProcessEvent(ClientReportDamagedResourceBuilding, &AFortPlayerController_ClientReportDamagedResourceBuilding_Params);

		auto Pawn = *Controller->Member<UObject*>(("Pawn"));

		static auto WoodItemData = FindObject(("FortResourceItemDefinition /Game/Items/ResourcePickups/WoodItemData.WoodItemData"));
		static auto StoneItemData = FindObject(("FortResourceItemDefinition /Game/Items/ResourcePickups/StoneItemData.StoneItemData"));
		static auto MetalItemData = FindObject(("FortResourceItemDefinition /Game/Items/ResourcePickups/MetalItemData.MetalItemData"));

		UObject* ItemDef = WoodItemData;

		if (Params->PotentialResourceType.Get() == EFortResourceType::Stone)
			ItemDef = StoneItemData;

		if (Params->PotentialResourceType.Get() == EFortResourceType::Metal)
			ItemDef = MetalItemData;

		auto ItemInstance = Inventory::FindItemInInventory(Controller, ItemDef);

		int AmountToGive = Params->PotentialResourceCount;

		// IMPROPER, we should add weakspot here.

		if (ItemInstance && Pawn)
		{
			auto Entry = ItemInstance->Member<__int64>(("ItemEntry"));

			// BUG: You lose some mats if you have like 998 or idfk
			if (*FFortItemEntry::GetCount(Entry) >= 999)
			{
				Helper::SummonPickup(Pawn, ItemDef, Helper::GetActorLocation(Pawn), EFortPickupSourceTypeFlag::Other, EFortPickupSpawnSource::Unset, AmountToGive);
				return;
			}
		}

		Inventory::GiveItem(Controller, ItemDef, EFortQuickBars::Secondary, 1, AmountToGive);
	}
}

inline bool OnDamageServerHook(UObject* BuildingActor, UFunction* Function, void* Parameters)
{
	static auto BuildingSMActorClass = FindObject(("Class /Script/FortniteGame.BuildingSMActor"));

	if (BuildingActor->IsA(BuildingSMActorClass)) // || BuildingActor->GetFullName().contains(("Car_")))
	{
		auto InstigatedByOffset = FindOffsetStruct(("Function /Script/FortniteGame.BuildingActor.OnDamageServer"), ("InstigatedBy"));
		auto InstigatedBy = *(UObject**)(__int64(Parameters) + InstigatedByOffset);

		auto DamageCauserOffset = FindOffsetStruct(("Function /Script/FortniteGame.BuildingActor.OnDamageServer"), ("DamageCauser"));
		auto DamageCauser = *(UObject**)(__int64(Parameters) + DamageCauserOffset);

		auto DamageTagsOffset = FindOffsetStruct(("Function /Script/FortniteGame.BuildingActor.OnDamageServer"), ("DamageTags"));
		auto DamageTags = (FGameplayTagContainer*)(__int64(Parameters) + DamageTagsOffset);

		auto DamageOffset = FindOffsetStruct(("Function /Script/FortniteGame.BuildingActor.OnDamageServer"), ("Damage"));
		auto Damage = (float*)(__int64(Parameters) + DamageOffset);

		static auto FortPlayerControllerAthenaClass = FindObject(("Class /Script/FortniteGame.FortPlayerControllerAthena"));

		if (InstigatedBy && InstigatedBy->IsA(FortPlayerControllerAthenaClass) &&
			DamageCauser->GetFullName().contains("B_Melee_Impact_Pickaxe_Athena_C")) // cursed
		{
			// TODO: Not hardcode the PickaxeDef, do like slot 0  or something
			static auto PickaxeDef = FindObject(("FortWeaponMeleeItemDefinition /Game/Athena/Items/Weapons/WID_Harvest_Pickaxe_Athena_C_T01.WID_Harvest_Pickaxe_Athena_C_T01"));
			auto CurrentWeapon = *(*InstigatedBy->Member<UObject*>(("MyFortPawn")))->Member<UObject*>(("CurrentWeapon"));

			if (CurrentWeapon && *CurrentWeapon->Member<UObject*>(("WeaponData")) == PickaxeDef)
			{
				DoHarvesting(InstigatedBy, BuildingActor, *Damage);
			}
		}
		;
	}

	return false;
}

inline bool BlueprintCanAttemptGenerateResourcesHook(UObject* BuildingActor, UFunction* Function, void* Parameters)
{
	struct parms {
		FGameplayTagContainer InTags;
		UObject* InstigatorController; // AController*
		bool ret;
	};

	auto Params = (parms*)Parameters;

	ProcessEventO(BuildingActor, Function, Params);

	static auto PickaxeDef = FindObject(("FortWeaponMeleeItemDefinition /Game/Athena/Items/Weapons/WID_Harvest_Pickaxe_Athena_C_T01.WID_Harvest_Pickaxe_Athena_C_T01"));
	auto CurrentWeapon = *(*Params->InstigatorController->Member<UObject*>(("MyFortPawn")))->Member<UObject*>(("CurrentWeapon"));

	if (CurrentWeapon && *CurrentWeapon->Member<UObject*>(("WeaponData")) == PickaxeDef)
	{
		if (Params && Params->ret)
		{
			DoHarvesting(Params->InstigatorController, BuildingActor);
		}
	}

	return false;
}

void InitializeHarvestingHooks()
{
	AddHook(("Function /Script/FortniteGame.BuildingActor.OnDamageServer"), OnDamageServerHook);

	if (Engine_Version > 424)
		AddHook("Function /Script/FortniteGame.BuildingSMActor.BlueprintCanAttemptGenerateResources", BlueprintCanAttemptGenerateResourcesHook);
}