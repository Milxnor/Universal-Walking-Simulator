#pragma once

#include <Gameplay/helper.h>
#include <Gameplay/inventory.h>

inline bool OnDamageServerHook(UObject* BuildingActor, UFunction* Function, void* Parameters)
{
	static auto BuildingSMActorClass = FindObject(_("Class /Script/FortniteGame.BuildingSMActor"));
	if (BuildingActor->IsA(BuildingSMActorClass))
	{
		auto InstigatedByOffset = FindOffsetStruct(_("Function /Script/FortniteGame.BuildingActor.OnDamageServer"), _("InstigatedBy"));
		auto InstigatedBy = *(UObject**)(__int64(Parameters) + InstigatedByOffset);

		auto DamageCauserOffset = FindOffsetStruct(_("Function /Script/FortniteGame.BuildingActor.OnDamageServer"), _("DamageCauser"));
		auto DamageCauser = *(UObject**)(__int64(Parameters) + DamageCauserOffset);

		auto DamageTagsOffset = FindOffsetStruct(_("Function /Script/FortniteGame.BuildingActor.OnDamageServer"), _("DamageTags"));
		auto DamageTags = (FGameplayTagContainer*)(__int64(Parameters) + DamageTagsOffset);

		struct Bitfield
		{
			unsigned char                                      UnknownData09 : 1;                                        // 0x0544(0x0001)
			unsigned char                                      bWorldReadyCalled : 1;                                    // 0x0544(0x0001) (Transient)
			unsigned char                                      bBeingRotatedOrScaled : 1;                                // 0x0544(0x0001) (Transient)
			unsigned char                                      bBeingTranslated : 1;                                     // 0x0544(0x0001) (Transient)
			unsigned char                                      bRotateInPlaceEditor : 1;                                 // 0x0544(0x0001)
			unsigned char                                      bEditorPlaced : 1;                                        // 0x0544(0x0001) (Net, Transient)
			unsigned char                                      bPlayerPlaced : 1;                                        // 0x0544(0x0001) (Edit, BlueprintVisible, BlueprintReadOnly, Net, DisableEditOnTemplate)
			unsigned char                                      bShouldTick : 1;
		};

		auto BitField = BuildingActor->Member<Bitfield>(_("bPlayerPlaced"));
		auto bPlayerPlaced = false; // BitField->bPlayerPlaced;

		static auto FortPlayerControllerAthenaClass = FindObject(_("Class /Script/FortniteGame.FortPlayerControllerAthena"));

		// if (DamageTags)
			// std::cout << _("DamageTags: ") << DamageTags->ToStringSimple(false) << '\n';

		if (!bPlayerPlaced && InstigatedBy && InstigatedBy->IsA(FortPlayerControllerAthenaClass) &&
			DamageCauser->GetFullName().contains("B_Melee_Impact_Pickaxe_Athena_C")) // cursed
		{
			// TODO: Not hardcode the PickaxeDef, do like slot 0  or something
			static auto PickaxeDef = FindObject(_("FortWeaponMeleeItemDefinition /Game/Athena/Items/Weapons/WID_Harvest_Pickaxe_Athena_C_T01.WID_Harvest_Pickaxe_Athena_C_T01"));
			auto CurrentWeapon = *(*InstigatedBy->Member<UObject*>(_("MyFortPawn")))->Member<UObject*>(_("CurrentWeapon"));
			if (CurrentWeapon && *CurrentWeapon->Member<UObject*>(_("WeaponData")) == PickaxeDef)
			{
				std::random_device rd; // obtain a random number from hardware
				std::mt19937 gen(rd()); // seed the generator
				std::uniform_int_distribution<> distr(3, 6); // define the range

				auto Random = distr(gen);

				struct
				{
					UObject* BuildingSMActor;                                          // (Parm, ZeroConstructor, IsPlainOldData)
					TEnumAsByte<EFortResourceType>                     PotentialResourceType;                                    // (Parm, ZeroConstructor, IsPlainOldData)
					int                                                PotentialResourceCount;                                   // (Parm, ZeroConstructor, IsPlainOldData)
					bool                                               bDestroyed;                                               // (Parm, ZeroConstructor, IsPlainOldData)
					bool                                               bJustHitWeakspot;                                         // (Parm, ZeroConstructor, IsPlainOldData)
				} AFortPlayerController_ClientReportDamagedResourceBuilding_Params{ BuildingActor, *BuildingActor->Member<TEnumAsByte<EFortResourceType>>(_("ResourceType")),
					 distr(gen), false, false };

				static auto ClientReportDamagedResourceBuilding = InstigatedBy->Function(_("ClientReportDamagedResourceBuilding"));
				InstigatedBy->ProcessEvent(ClientReportDamagedResourceBuilding, &AFortPlayerController_ClientReportDamagedResourceBuilding_Params);
			}
		}
		else
			std::cout << _("AAAAAAAAAAAA");
;	}

	return false;
}

inline bool ClientReportDamagedResourceBuildingHook(UObject* Controller, UFunction* Function, void* Parameters)
{
	struct AFortPlayerController_ClientReportDamagedResourceBuilding_Params
	{
		UObject* BuildingSMActor;                                          // (Parm, ZeroConstructor, IsPlainOldData)
		TEnumAsByte<EFortResourceType>                     PotentialResourceType;                                    // (Parm, ZeroConstructor, IsPlainOldData)
		int                                                PotentialResourceCount;                                   // (Parm, ZeroConstructor, IsPlainOldData)
		bool                                               bDestroyed;                                               // (Parm, ZeroConstructor, IsPlainOldData)
		bool                                               bJustHitWeakspot;                                         // (Parm, ZeroConstructor, IsPlainOldData)
	};

	auto Params = (AFortPlayerController_ClientReportDamagedResourceBuilding_Params*)Parameters;

	if (Controller)
	{
		auto Pawn = *Controller->Member<UObject*>(_("Pawn"));

		static auto WoodItemData = FindObject(_("FortResourceItemDefinition /Game/Items/ResourcePickups/WoodItemData.WoodItemData"));
		static auto StoneItemData = FindObject(_("FortResourceItemDefinition /Game/Items/ResourcePickups/StoneItemData.StoneItemData"));
		static auto MetalItemData = FindObject(_("FortResourceItemDefinition /Game/Items/ResourcePickups/MetalItemData.MetalItemData"));

		UObject* ItemDef = WoodItemData;

		if (Params->PotentialResourceType.Get() == EFortResourceType::Stone)
			ItemDef = StoneItemData;

		if (Params->PotentialResourceType.Get() == EFortResourceType::Metal)
			ItemDef = MetalItemData;

		auto ItemInstance = Inventory::FindItemInInventory(Controller, ItemDef);

		int AmountToGive = Params->PotentialResourceCount;

		if (Params->bJustHitWeakspot)
		{
			std::random_device rd; // obtain a random number from hardware
			std::mt19937 gen(rd()); // seed the generator
			std::uniform_int_distribution<> distr(2, 4); // define the range

			AmountToGive += distr(gen);
		}

		if (ItemInstance && Pawn)
		{
			auto Entry = ItemInstance->Member<__int64>(_("ItemEntry"));

			// BUG: You lose some mats if you have like 998 or idfk
			if (*FFortItemEntry::GetCount(Entry) >= 999)
			{
				Helper::SummonPickup(Pawn, ItemDef, Helper::GetActorLocation(Pawn), EFortPickupSourceTypeFlag::Other, EFortPickupSpawnSource::Unset, AmountToGive);
				return false;
			}
		}

		Inventory::GiveItem(Controller, ItemDef, EFortQuickBars::Secondary, AmountToGive);
		std::cout << "wtf\n";
	}

	return false;
}


void InitializeHarvestingHooks()
{
	AddHook(_("Function /Script/FortniteGame.BuildingActor.OnDamageServer"), OnDamageServerHook);
	AddHook(_("Function /Script/FortniteGame.FortPlayerController.ClientReportDamagedResourceBuilding"), ClientReportDamagedResourceBuildingHook);
}