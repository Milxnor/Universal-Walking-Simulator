#pragma once

#include <UE/structs.h>
#include <Gameplay/carmine.h>
#include <Gameplay/helper.h>
#include <Gameplay/abilities.h>

#include <UE/DataTables.h>

/*

GiveItem - Creates a new item and or stacks
TakeItem - Decreases and or removes if requested

*/

constexpr bool newSwapping = false; // there is this bug where its not actuallythe correct weapon on the client

static int GetEntrySize()
{
	static auto FortItemEntryClass = FindObjectOld(("ScriptStruct /Script/FortniteGame.FortItemEntry"), true);

	return GetSizeOfStruct(FortItemEntryClass);
}

static float GetMaxStackSize(UObject* Definition)
{
	static auto MaxStackSizeOffset = GetOffset(Definition, "MaxStackSize");

	bool bIsScalableFloat = Engine_Version >= 424; // probs wrong

	return bIsScalableFloat ? ((FScalableFloat*)(__int64(Definition) + MaxStackSizeOffset))->Value :
		*(int*)(__int64(Definition) + MaxStackSizeOffset);
}

template <typename EntryType = __int64>
inline EntryType* GetItemEntryFromInstance(UObject* Instance)
{
	if (!Instance)
		return nullptr;

	static auto ItemEntryOffset = GetOffset(Instance, "ItemEntry");
	return (EntryType*)(__int64(Instance) + ItemEntryOffset);
}

namespace Items {
	void HandleCarmine(UObject* Controller) {
		if (!Controller)
			return;

		UObject* Pawn = Helper::GetPawnFromController(Controller);
		Helper::ChoosePart(Pawn, EFortCustomPartType::Body, FindObject("CustomCharacterPart /Game/Athena/Heroes/Meshes/Bodies/Dev_TestAsset_Body_M_XL.Dev_TestAsset_Body_M_XL"));
		Helper::ChoosePart(Pawn, EFortCustomPartType::Head, FindObject("CustomCharacterPart /Game/Athena/Heroes/Meshes/Heads/Dev_TestAsset_Head_M_XL.Dev_TestAsset_Head_M_XL"));
		*Pawn->Member<UObject*>("AnimBPOverride") = FindObject("AnimBlueprintGeneratedClass /Game/Characters/Player/Male/Male_Avg_Base/Gauntlet_Player_AnimBlueprint.Gauntlet_Player_AnimBlueprint_C");
		
		UObject* AS = FindObject("FortAbilitySet /Game/Athena/Items/Gameplay/BackPacks/CarminePack/AS_CarminePack.AS_CarminePack");
		auto GrantedAbilities = AS->Member<TArray<UObject*>>("GameplayAbilities");

		for (int i = 0; i < GrantedAbilities->Num(); i++) {
			GrantGameplayAbility(Pawn, GrantedAbilities->At(i));
		}

		UObject* Montage = FindObject("AnimMontage /Game/Animation/Game/MainPlayer/Skydive/Freefall/Custom/Jim/Transitions/Spawn_Montage.Spawn_Montage");

		//if (Montage && PlayMontage && Engine_Version < 426)
		//{
		//	auto AbilitySystemComponent = *Pawn->Member<UObject*>(("AbilitySystemComponent"));
		//	static auto EmoteClass = FindObject(("BlueprintGeneratedClass /Game/Abilities/Emotes/GAB_Emote_Generic.GAB_Emote_Generic_C"));

		//	TArray<FGameplayAbilitySpec<FGameplayAbilityActivationInfo>> Specs;

		//	if (Engine_Version <= 422)
		//		Specs = (*AbilitySystemComponent->Member<FGameplayAbilitySpecContainerOL>(("ActivatableAbilities"))).Items;
		//	else
		//		Specs = (*AbilitySystemComponent->Member<FGameplayAbilitySpecContainerSE>(("ActivatableAbilities"))).Items;

		//	UObject* DefaultObject = EmoteClass->CreateDefaultObject();

		//	for (int i = 0; i < Specs.Num(); i++)
		//	{
		//		auto& CurrentSpec = Specs[i];

		//		if (CurrentSpec.Ability == DefaultObject)
		//		{
		//			auto ActivationInfo = CurrentSpec.Ability->Member<FGameplayAbilityActivationInfo>(("CurrentActivationInfo"));

		//			// Helper::SetLocalRole(Pawn, ENetRole::ROLE_SimulatedProxy);
		//			auto Dura = PlayMontage(AbilitySystemComponent, CurrentSpec.Ability, FGameplayAbilityActivationInfo(), Montage, 1.0f, FName(0));
		//			// Helper::SetLocalRole(Pawn, ENetRole::ROLE_AutonomousProxy);

		//			std::cout << ("Played for: ") << Dura << '\n';
		//		}
		//	}
		//}
	}
}

namespace FFortItemEntry
{
	int* GetLoadedAmmo(__int64* Entry)
	{
		if (!Entry)
			return nullptr;

		static auto LoadedAmmoOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortItemEntry"), ("LoadedAmmo"));
		auto LoadedAmmo = (int*)(__int64(&*Entry) + LoadedAmmoOffset);

		return LoadedAmmo;
	}

	int* GetCount(__int64* Entry)
	{
		if (!Entry)
			return nullptr;

		static auto CountOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortItemEntry"), ("Count"));
		auto Count = (int*)(__int64(&*Entry) + CountOffset);

		return Count;
	}

	bool* GetbIsDirty(__int64* Entry)
	{
		if (!Entry)
			return nullptr;

		static auto bIsDirtyOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortItemEntry"), ("bIsDirty"));
		auto bIsDirty = (bool*)(__int64(&*Entry) + bIsDirtyOffset);

		return bIsDirty;
	}

	FGuid* GetGuid(__int64* Entry)
	{
		static auto GuidOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortItemEntry"), ("ItemGuid"));
		auto Guid = (FGuid*)(__int64(&*Entry) + GuidOffset);

		return Guid;
	}

	UObject* GetItemDefinition(__int64* Entry, UObject*** ahh = nullptr)
	{
		if (!Entry)
			return nullptr;

		static auto ItemDefinitionOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortItemEntry"), ("ItemDefinition"));
		auto Definition = (UObject**)(__int64(&*Entry) + ItemDefinitionOffset);

		if (ahh)
			*ahh = Definition;

		return *Definition;
	}
}

namespace QuickBars
{
	UObject* GetInstanceInSlot(UObject* Controller, EFortQuickBars Bars, int Slot)
	{
		if (!Controller)
			return nullptr;

		{
			static auto GetItemInQuickbarSlotfn = Controller->Function(("GetItemInQuickbarSlot"));

			struct {
				EFortQuickBars QuickBarType;
				int SlotIndex;
				UObject* Ret;
			} GetItemInQuickbarSlot_params{Bars, Slot};

			if (GetItemInQuickbarSlotfn)
				Controller->ProcessEvent(GetItemInQuickbarSlotfn, &GetItemInQuickbarSlot_params);

			return GetItemInQuickbarSlot_params.Ret;
		}

		return nullptr;
	}

	bool IsHoldingPickaxe(UObject* Controller)
	{
		if (!Controller)
			return false;

		static auto PickaxeDefObject = Helper::GetPickaxeDef(Controller);

		auto Pawn = Helper::GetPawnFromController(Controller);

		if (Pawn)
		{
			auto CurrentWeapon = Helper::GetCurrentWeapon(Pawn);

			if (CurrentWeapon)
			{
				auto CurrentWeaponDef = Helper::GetWeaponData(CurrentWeapon);

				if (CurrentWeaponDef == PickaxeDefObject)
					return true;
			}
		}

		return false;
	}

	// returns -1 if there are no slots
	int FindNextAvailableSlot(UObject* Controller, EFortQuickBars Bars)
	{
		auto getSlot = [](TArray<__int64>* Slots, int i) -> __int64* {
			static auto SizeOfQuickBarSlot = GetSizeOfStruct(FindObject("ScriptStruct /Script/FortniteGame.QuickBarSlot"));

			return (__int64*)(__int64(Slots) + (static_cast<__int64>(SizeOfQuickBarSlot) * i));
		};

		auto QuickBars = *Controller->Member<UObject*>("QuickBars");
		auto QuickBar = Bars == EFortQuickBars::Primary ? QuickBars->Member<__int64>("PrimaryQuickBar") 
			: QuickBars->Member<__int64>("SecondaryQuickBar");

		static auto SlotsOffset = FindOffsetStruct("ScriptStruct /Script/FortniteGame.QuickBar", "Slots");

		auto QuickBarSlots = (TArray<__int64>*)(__int64(QuickBar) + SlotsOffset);

		for (int i = Bars == EFortQuickBars::Primary ? 1 : 0; i < QuickBarSlots->Num(); i++)
		{
			auto Slot = getSlot(QuickBarSlots, i);

			static auto ItemsOffset = FindOffsetStruct("ScriptStruct /Script/FortniteGame.QuickBarSlot", "Items");
			auto Items = (TArray<FGuid>*)(__int64(Slot) + ItemsOffset);

			if (Items && !Items->GetData()) // slot is empty
			{
				if (i >= 6 && Bars == EFortQuickBars::Primary)
					return -1;

				return i;
			}
		}

		return -1;
	}

	EFortQuickBars WhatQuickBars(UObject* Definition) // returns the quickbar the item should go in
	{
		static auto FortWeaponItemDefinitionClass = FindObject(("Class /Script/FortniteGame.FortWeaponItemDefinition"));
		static auto FortDecoItemDefinitionClass = FindObject("Class /Script/FortniteGame.FortDecoItemDefinition");

		if (Definition->IsA(FortWeaponItemDefinitionClass) && !Definition->IsA(FortDecoItemDefinitionClass))
			return EFortQuickBars::Primary;
		else
			return EFortQuickBars::Secondary;
	}
}

namespace Inventory
{
	FGuid GetWeaponGuid(UObject* Weapon)
	{
		static auto ItemEntryGuidOffset = GetOffset(Weapon, "ItemEntryGuid");

		return *(FGuid*)(__int64(Weapon) + ItemEntryGuidOffset);
	}

	UObject* GetQuickBars(UObject* Controller)
	{
		static auto QuickBarsOffset = GetOffset(Controller, "QuickBars");

		return *(UObject**)(__int64(Controller) + QuickBarsOffset);
	}

	UObject* GetWorldInventory(UObject* Controller)
	{
		static auto WorldInventoryOffset = GetOffset(Controller, "WorldInventory");
		auto WorldInventoryP = (UObject**)(__int64(Controller) + WorldInventoryOffset);

		// return *Controller->Member<UObject*>(("WorldInventory"));
		return !IsBadReadPtr(WorldInventoryP) ? *WorldInventoryP : nullptr;
	}

	__int64* GetInventory(UObject* Controller)
	{
		auto WorldInventory = GetWorldInventory(Controller);

		if (WorldInventory)
		{
			static auto InventoryOffset = GetOffset(WorldInventory, "Inventory");
			auto Inventorya = (__int64*)(__int64(WorldInventory) + InventoryOffset);
			// auto Inventorya = WorldInventory->Member<__int64>(("Inventory"));

			return Inventorya;
		}

		return nullptr;
	}


	TArray<UObject*>* GetItemInstances(UObject* Controller)
	{
		static __int64 ItemInstancesOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortItemList"), ("ItemInstances"));

		auto Inventory = GetInventory(Controller);

		// std::cout << ("ItemInstances Offset: ") << ItemInstancesOffset << '\n';

		// ItemInstancesOffset = 0x110;

		if (Inventory)
			return (TArray<UObject*>*)(__int64(Inventory) + ItemInstancesOffset);
		else
			return nullptr;
	}

	void Update(UObject* Controller, int Idx = -1, bool bRemovedItem = false, FFastArraySerializerItem* ModifiedItem = nullptr)
	{
		auto WorldInventory = GetWorldInventory(Controller);
		auto Inventory = GetInventory(Controller);

		// *WorldInventory->Member<bool>("bRequiresLocalUpdate") = true;

		static auto WorldHandleInvUpdate = WorldInventory->Function(("HandleInventoryLocalUpdate"));

		if (WorldHandleInvUpdate)
			WorldInventory->ProcessEvent(WorldHandleInvUpdate);

		if (std::floor(FnVerDouble) < 10) // idk crashes // TODO: Test this again
		{
			static auto PCHandleInvUpdate = Controller->Function(("HandleWorldInventoryLocalUpdate"));

			if (PCHandleInvUpdate)
				Controller->ProcessEvent(PCHandleInvUpdate);
		}

		if (FnVerDouble < 7.4)
		{
			const auto QuickBars = Inventory::GetQuickBars(Controller);

			if (QuickBars)
			{
				static auto OnRep_PrimaryQuickBar = QuickBars->Function(("OnRep_PrimaryQuickBar"));
				QuickBars->ProcessEvent(OnRep_PrimaryQuickBar);

				static auto OnRep_SecondaryQuickBar = QuickBars->Function(("OnRep_SecondaryQuickBar"));
				QuickBars->ProcessEvent(OnRep_SecondaryQuickBar);

				static auto OnRep_QuickBar = Controller->Function(("OnRep_QuickBar"));
				Controller->ProcessEvent(OnRep_QuickBar, nullptr);
			}
		}
		else
		{
			static auto ClientForceUpdateQuickbar = Controller->Function(("ClientForceUpdateQuickbar"));
			auto PrimaryQuickbar = EFortQuickBars::Primary;
			Controller->ProcessEvent(ClientForceUpdateQuickbar, &PrimaryQuickbar);
			auto SecondaryQuickbar = EFortQuickBars::Secondary;
			Controller->ProcessEvent(ClientForceUpdateQuickbar, &SecondaryQuickbar);

			if (FnVerDouble < 8)
			{
				static auto UpdateQuickBars = Controller->Function(("UpdateQuickBars"));

				if (UpdateQuickBars)
					Controller->ProcessEvent(UpdateQuickBars);
			}
		}

		if ((bRemovedItem || Idx != -1) && Inventory)
			MarkArrayDirty(Inventory);

		// if (Idx != -1)
		if (ModifiedItem)
		{
			*FFortItemEntry::GetbIsDirty((__int64*)ModifiedItem) = true;
			MarkItemDirty(Inventory, ModifiedItem);

		}
	}

	static UObject* EquipWeapon(UObject* Pawn, UObject* FortWeapon)
	{
		if (FortWeapon && Pawn)
		{
			static auto OnRep_ReplicatedWeaponData = FortWeapon->Function("OnRep_ReplicatedWeaponData");

			if (OnRep_ReplicatedWeaponData)
				FortWeapon->ProcessEvent(OnRep_ReplicatedWeaponData);

			// the functgion below starts taking a param
			// Weapon->ProcessEvent(("OnRep_AmmoCount"));

			struct { UObject* P; } givenParams{ Pawn };

			static auto givenFn = FortWeapon->Function(("ClientGivenTo"));
			if (givenFn)
				givenFn->ProcessEvent(givenFn, &givenParams);
			else
				std::cout << ("No ClientGivenTo!\n");

			struct { UObject* Weapon; } internalEquipParams{ FortWeapon };

			if (FnVerDouble >= 7.40)
			{
				static auto internalEquipFn = Pawn->Function(("ClientInternalEquipWeapon"));
				if (internalEquipFn)
					Pawn->ProcessEvent(internalEquipFn, &internalEquipParams);
				else
					std::cout << ("No ClientInternalEquipWeapon!\n");
			}

			return FortWeapon;
		}
		else
			std::cout << ("No weapon!\n");

		return nullptr;
	}

	inline UObject* EquipWeaponDefinition(UObject* Pawn, UObject* Definition, const FGuid& Guid, int Ammo = -1, const FGuid& TrackerGuid = FGuid(), bool ahhh = false)
	{
		if (Pawn && Definition)
		{
			static auto AthenaGadgetItemDefinitionClass = FindObject("Class /Script/FortniteGame.FortGadgetItemDefinition");

			bool IsAGID = Definition->IsA(AthenaGadgetItemDefinitionClass);

			static auto FortTrapItemDefinitionClass = FindObject("Class /Script/FortniteGame.FortTrapItemDefinition");
			// static auto FortContextTrapItemDefinitionClass = FindObject("Class /Script/FortniteGame.FortContextTrapItemDefinition");
			static auto FortDecoItemDefinitionClass = FindObject("Class /Script/FortniteGame.FortDecoItemDefinition");

			// std::cout << "Definition: " << Definition->GetFullName() << '\n';

			/*
			std::cout << "ADD: " << WeaponClass << '\n';
			std::cout << "WeapojnClass: " << (WeaponClass ? WeaponClass->GetFullName() : std::string("00000")) << '\n';
			*/

			if (Definition->IsA(FortTrapItemDefinitionClass)) // wrong? probs
			{
				UObject* WeaponClass = nullptr;
				static auto GetWeaponActorClass = Definition->Function("GetWeaponActorClass");
				Definition->ProcessEvent(GetWeaponActorClass, &WeaponClass);

				auto TrapToolClass = WeaponClass; // FindObject("BlueprintGeneratedClass /Game/Weapons/FORT_BuildingTools/TrapTool.TrapTool_C");
				auto newTrapTool = Easy::SpawnActor(TrapToolClass, Helper::GetActorLocation(Pawn));
				
				static auto PickUpActor = Pawn->Function("PickUpActor");
				struct { UObject* PickupActor; UObject* PlacementDecoItemDefinition; } parms{newTrapTool, Definition};

				Pawn->ProcessEvent(PickUpActor, &parms);
			}

			else if (!IsAGID)
			{
				UObject* Weapon = nullptr;

				static auto equipFn = Pawn->Function(("EquipWeaponDefinition"));

				if (FnVerDouble < 16.00)
				{
					struct {
						UObject* Def;
						FGuid Guid;
						UObject* Wep;
					} params{ Definition, Guid };
					if (equipFn)
						Pawn->ProcessEvent(equipFn, &params);

					Weapon = params.Wep;
				}
				else
				{
					struct {
						UObject* Def;
						FGuid Guid;
						FGuid TrackerGuid;
						UObject* Wep;
					} params{ Definition, Guid, TrackerGuid };

					if (equipFn)
						Pawn->ProcessEvent(equipFn, &params);

					Weapon = params.Wep;
				}

				if (Weapon)
				{
					Helper::SetOwner(Weapon, Pawn);
					//*Weapon->Member<UObject*>(("WeaponData")) = Definition;
					//EquipWeapon(Pawn, Weapon, Guid, Ammo);

					// std::cout << "Ammo: " << Ammo << '\n';

					// *Weapon->Member<int>(("AmmoCount")) = Ammo; // GetEntryhFromGuid(Gretequi r08wig09wr

					// bUpdateLocalAmmoCount

					if (Ammo != -1 && ahhh)
					{
						static auto AmmoCountOffset = GetOffset(Weapon, "AmmoCount");
						*(int*)(__int64(Weapon) + AmmoCountOffset) = Ammo;
					}

					static auto OnRep_ReplicatedWeaponData = ("OnRep_ReplicatedWeaponData");

					if (OnRep_ReplicatedWeaponData)
						Weapon->ProcessEvent(OnRep_ReplicatedWeaponData);
				}
				else
					std::cout << ("Failed to spawn Weapon!\n");

				return Weapon;
			}
			else if (IsAGID)
			{
				// std::cout << ("Equipping AGID!\n");
				static auto GetItemDefinition = Definition->Function(("GetWeaponItemDefinition"));

				UObject* Def = nullptr;

				if (GetItemDefinition) {
					Definition->ProcessEvent(GetItemDefinition, &Def);
				}
				else {
					GetItemDefinition = Definition->Function(("GetDecoItemDefinition"));
					Definition->ProcessEvent(GetItemDefinition, &Def);
				}

				if (Def)
				{
					// std::cout << "Def Name: " << Def->GetFullName() << '\n';
					Inventory::EquipWeaponDefinition(Pawn, Def, Guid);
				}
				else
					std::cout << ("Failed to get AGID's Definition!\n");
			}
			/* else if (Definition->IsA(FortDecoItemDefinitionClass))
			{
				static auto BlueprintClassOffset = GetOffset(Definition, "BlueprintClass");
				auto BlueprintPickup = Easy::SpawnActor(*(UObject**)(__int64(Definition) + BlueprintClassOffset), Helper::GetActorLocation(Pawn));

				static auto PickUpActor = Pawn->Function("PickUpActor");
				struct { UObject* PickupActor; UObject* PlacementDecoItemDefinition; } parms{ BlueprintPickup, Definition };

				Pawn->ProcessEvent(PickUpActor, &parms);
			} */
		}

		return nullptr;
	}

	FGuid GetItemGuid(UObject* ItemInstance)
	{
		if (!ItemInstance)
			return FGuid();

		static UObject* GetItemGuidFn = FindObject(("Function /Script/FortniteGame.FortItem.GetItemGuid"));
		FGuid Guid;

		if (GetItemGuidFn)
			ItemInstance->ProcessEvent(GetItemGuidFn, &Guid);

		return Guid;
	}

	UObject* GetItemDefinition(UObject* ItemInstance)
	{
		static UObject* GetDefFn = FindObject(("Function /Script/FortniteGame.FortItem.GetItemDefinitionBP"));
		UObject* Def;

		ItemInstance->ProcessEvent(GetDefFn, &Def);

		return Def;
	}

	int GetNumQuickbarSlots(UObject* Controller, EFortQuickBars bars)
	{
		struct {
			EFortQuickBars bars;
			int Ret;
		}parms{bars};
		static auto fn = Controller->Function(("GetNumQuickbarSlots"));

		if (Controller && fn)
			Controller->ProcessEvent(fn, &parms);

		return parms.Ret;
	}

	inline UObject* GetItemInstanceFromGuid(UObject* Controller, const FGuid& Guid)
	{
		if (!Controller)
			return nullptr;

		auto ItemInstances = GetItemInstances(Controller);

		for (int i = 0; i < ItemInstances->Num(); i++)
		{
			auto CurrentItemInstance = ItemInstances->At(i);

			if (!CurrentItemInstance)
				continue;

			if (GetItemGuid(CurrentItemInstance) == Guid)
				return CurrentItemInstance;
		}

		return nullptr;
	}

	inline UObject* EquipInventoryItem(UObject* Controller, const FGuid& Guid)
	{
		if (!Controller)
			return nullptr;

		auto Pawn = Helper::GetPawnFromController(Controller);

		if (!Pawn)
			return nullptr;

		int currentWeaponAmmo = 0;

		if (newSwapping)
		{
			auto CurrentWeaponList = Pawn->Member<TArray<UObject*>>("CurrentWeaponList");

			std::cout << "Weapon List size: " << CurrentWeaponList->Num() << '\n';

			for (int i = 0; i < CurrentWeaponList->Num(); i++)
			{
				auto CurrentWeaponInList = CurrentWeaponList->At(i);

				if (CurrentWeaponInList && (*CurrentWeaponInList->Member<FGuid>("ItemEntryGuid") == Guid))
				{
					static auto GetMagazineAmmoCount = CurrentWeaponInList->Function("GetMagazineAmmoCount");

					if (GetMagazineAmmoCount)
						CurrentWeaponInList->ProcessEvent(GetMagazineAmmoCount, &currentWeaponAmmo);

					break;
					/* static auto OnRep_ReplicatedWeaponData = CurrentWeaponInList->Function("OnRep_ReplicatedWeaponData");

					if (OnRep_ReplicatedWeaponData)
						CurrentWeaponInList->ProcessEvent(OnRep_ReplicatedWeaponData);

					// OnRep_ReplicatedAppliedAlterations?

					static auto ClientGivenTo = CurrentWeaponInList->Function("ClientGivenTo");

					if (ClientGivenTo)
						CurrentWeaponInList->ProcessEvent(ClientGivenTo, &Pawn);

					static auto ClientInternalEquipWeapon = Pawn->Function("ClientInternalEquipWeapon");

					struct { UObject* Weapon; } ClientInternalEquipWeapon_Params{ CurrentWeaponInList };

					if (ClientInternalEquipWeapon)
						Pawn->ProcessEvent(ClientInternalEquipWeapon, &ClientInternalEquipWeapon_Params);

					return nullptr; */
				}
			}
		}

		auto CurrentItemInstance = GetItemInstanceFromGuid(Controller, Guid);

		if (!CurrentItemInstance)
			return nullptr;

		auto Def = GetItemDefinition(CurrentItemInstance);

		FGuid TrackerGuid;

		if (FnVerDouble >= 16.00)
		{
			static auto GetTrackerGuid = CurrentItemInstance->Function("GetTrackerGuid");

			if (GetTrackerGuid)
				CurrentItemInstance->ProcessEvent(GetTrackerGuid, &TrackerGuid);
		}

		auto currentWeapon = Helper::GetCurrentWeapon(Pawn);
		static auto PickaxeDefObject = Helper::GetPickaxeDef(Controller);

		// if (!QuickBars::IsHoldingPickaxe()) 
		if (currentWeapon && Helper::GetWeaponData(currentWeapon) != PickaxeDefObject)
		{
			auto currentInstance = GetItemInstanceFromGuid(Controller, Inventory::GetWeaponGuid(currentWeapon));

			if (currentInstance)
			{
				auto currentItemEntry = GetItemEntryFromInstance(currentInstance);

				if (currentItemEntry)
				{
					int currentAmmoCount = 0; // = *currentWeapon->Member<int>("AmmoCount");
					static auto GetMagazineAmmoCount = currentWeapon->Function("GetMagazineAmmoCount");

					if (GetMagazineAmmoCount)
						currentWeapon->ProcessEvent(GetMagazineAmmoCount, &currentAmmoCount);

					// std::cout << "AmmoCount: " << currentAmmoCount << '\n';

					auto LoadedAmmo = FFortItemEntry::GetLoadedAmmo(currentItemEntry);
					// std::cout << "LoadedAmmo: " << *LoadedAmmo << '\n';

					if (LoadedAmmo)
					{
						*LoadedAmmo = currentAmmoCount;
						Inventory::Update(Controller, -1, true, (FFastArraySerializerItem*)currentItemEntry);
						// MarkItemDirty(Inventory::GetInventory(Controller), (FFastArraySerializerItem*)currentItemEntry);
					}
				}
				else
					std::cout << "No ItemEntry!\n";
			}
			else
				std::cout << "No ItemInstance!\n";
		}
		else
			std::cout << "Brudaa!\n";

		int equippingAmmo = *FFortItemEntry::GetLoadedAmmo(GetItemEntryFromInstance(CurrentItemInstance));

		std::cout << "equippingAmmo ammo: " << equippingAmmo << '\n';

		EquipWeaponDefinition(Pawn, Def, Guid, equippingAmmo/* *FFortItemEntry::GetLoadedAmmo(GetItemEntryFromInstance(CurrentItemInstance)) */, TrackerGuid);

		/* std::string FullName = Def->GetFullName();

		if (FullName.contains("CarminePack") || FullName.contains("AshtonPack.")) {
			Items::HandleCarmine(Controller);
		} */

		return nullptr;
	}

	template <typename EntryStruct>
	TArray<EntryStruct>* GetReplicatedEntries(UObject* Controller)
	{
		static __int64 ReplicatedEntriesOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortItemList"), ("ReplicatedEntries"));

		auto Inventory = GetInventory(Controller);
		// ReplicatedEntriesOffset = 0xB0;

		// std::cout << ("ReplicatedEntries Offset: ") << ReplicatedEntriesOffset << '\n';

		return (TArray<EntryStruct>*)(__int64(Inventory) + ReplicatedEntriesOffset);

		// return (TArray<EntryStruct>*)(__int64(Inventory) + ReplicatedEntriesOffset);
	}

	UObject* CreateItemInstance(UObject* Controller, UObject* Definition, int Count = 1)
	{
		if (!Definition || !Controller)
			return nullptr;

		static auto FortWorldItemClass = FindObjectOld("Class /Script/FortniteGame.FortWorldItem", true);

		static auto TransientPackage = FindObject("Package /Engine/Transient");
		auto itemInstance = Easy::SpawnObject(FortWorldItemClass, TransientPackage); // we love ch3!

		if (itemInstance)
		{
			// std::cout << "ItemInstance: " << itemInstance->GetFullName() << '\n';

			static UObject* SOCFTIFn = itemInstance->Function(("SetOwningControllerForTemporaryItem"));

			itemInstance->ProcessEvent(SOCFTIFn, &Controller);
			
			static auto bIsTemporaryItemOffset = GetOffset(itemInstance, "bIsTemporaryItem");
			auto bIsTemp = (bool*)(__int64(itemInstance) + bIsTemporaryItemOffset);

			if (bIsTemp)
				*bIsTemp = true;

			auto Entry = GetItemEntryFromInstance(itemInstance);

			UObject** entryDefinition = nullptr;
			FFortItemEntry::GetItemDefinition(Entry, &entryDefinition);

			*entryDefinition = Definition;
			*FFortItemEntry::GetCount(Entry) = Count;

			static auto OwnerInventoryOffset = GetOffset(itemInstance, "OwnerInventory");
			*(UObject**)(__int64(itemInstance) + OwnerInventoryOffset) = Inventory::GetWorldInventory(Controller);

			return itemInstance;
		}

		return nullptr;
	}

	template <typename EntryStruct>
	int AddToReplicatedEntries(UObject* Controller, UObject* FortItem)
	{
		if (!Controller || !FortItem)
			return -1;

		auto ItemEntry = GetItemEntryFromInstance<EntryStruct>(FortItem); // FortItem->Member<EntryStruct>(("ItemEntry"));
		
		// std::cout << ("ItemEntryStruct Size: ") << GetEntrySize() << '\n';

		if (ItemEntry)
			return GetReplicatedEntries<EntryStruct>(Controller)->Add(*ItemEntry); // GetEntrySize());

		return -1;
	}

	template <typename EntryStruct, typename Type>
	bool ChangeItemInReplicatedEntriesWithEntries(UObject* Controller, UObject* Instance, const std::string& Name, Type NewVal, int Count = -1)
	{
		if (!Controller || !Instance)
			return false;

		auto ReplicatedEntries = GetReplicatedEntries<EntryStruct>(Controller);
		bool bSuccessful = false;

		for (int x = 0; x < ReplicatedEntries->Num(); x++)
		{
			auto& ItemEntry = ReplicatedEntries->At(x);

			if (*FFortItemEntry::GetGuid((__int64*)&ItemEntry) == Inventory::GetItemGuid(Instance))
			{
				auto Offset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortItemEntry"), Name); // todo: idk like make this faster
				*(Type*)(__int64(&ItemEntry) + Offset) = NewVal;
				bSuccessful = true;
				auto Inventory = GetInventory(Controller);

				MarkItemDirty(Inventory, (FFastArraySerializerItem*)&ItemEntry);

				break;
			}
		}

		return bSuccessful;
	}

	template <typename Type>
	bool ChangeItemInReplicatedEntries(UObject* Controller, UObject* Instance, const std::string& Name, Type NewVal)
	{
		static const auto FlooredVer = std::floor(FnVerDouble);

		if (FlooredVer == 2)
		{
			struct ItemEntrySize { unsigned char Unk00[0xB8]; };
			return ChangeItemInReplicatedEntriesWithEntries<ItemEntrySize, int>(Controller, Instance, Name, NewVal);
		}
		else if (FlooredVer == 3)
		{
			// struct ItemEntrySize { unsigned char Unk00[0xC0]; };
			struct ItemEntrySize { unsigned char Unk00[0xC8]; };
			return ChangeItemInReplicatedEntriesWithEntries<ItemEntrySize, int>(Controller, Instance, Name, NewVal);
		}
		else if (FlooredVer > 3 && FnVerDouble < 7.30)
		{
			struct ItemEntrySize { unsigned char Unk00[0xD0]; };
			ChangeItemInReplicatedEntriesWithEntries<ItemEntrySize, int>(Controller, Instance, Name, NewVal);
		}
		else if (FnVerDouble >= 7.30 && Engine_Version <= 424)
		{
			struct ItemEntrySize { unsigned char Unk00[0x120]; };
			ChangeItemInReplicatedEntriesWithEntries<ItemEntrySize, int>(Controller, Instance, Name, NewVal);
		}
		else if (Engine_Version == 425)
		{
			struct ItemEntrySize { unsigned char Unk00[0x150]; };
			ChangeItemInReplicatedEntriesWithEntries<ItemEntrySize, int>(Controller, Instance, Name, NewVal);
		}
		else if (Engine_Version >= 426 && FlooredVer < 15) // idk if right
		{
			struct ItemEntrySize { unsigned char Unk00[0x160]; };
			ChangeItemInReplicatedEntriesWithEntries<ItemEntrySize, int>(Controller, Instance, Name, NewVal);
		}
		else if (FlooredVer >= 15 && FlooredVer < 18)
		{
			struct ItemEntrySize { unsigned char Unk00[0x190]; };
			ChangeItemInReplicatedEntriesWithEntries<ItemEntrySize, int>(Controller, Instance, Name, NewVal);
		}
		else if (FlooredVer >= 18)
		{
			struct ItemEntrySize { unsigned char Unk00[0x1A0]; };
			ChangeItemInReplicatedEntriesWithEntries<ItemEntrySize, int>(Controller, Instance, Name, NewVal);
		}

		return false;
	}

	template <typename EntryStruct>
	bool RemoveGuidFromReplicatedEntries(UObject* Controller, const FGuid& Guid)
	{
		auto ReplicatedEntries = GetReplicatedEntries<EntryStruct>(Controller);

		bool bSuccessful = false;

		for (int x = 0; x < ReplicatedEntries->Num(); x++)
		{
			static auto GuidOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortItemEntry"), ("ItemGuid"));
			auto& ItemEntry = ReplicatedEntries->At(x);

			if (*(FGuid*)(__int64(&ItemEntry) + GuidOffset) == Guid)
			{
				ReplicatedEntries->RemoveAt(x);
				bSuccessful = true;
				break;
			}
		}

		if (!bSuccessful)
			std::cout << ("Failed to find ItemGuid in Inventory!\n");

		return bSuccessful;
	}

	// returns instance
	static UObject* AddItem(UObject* Controller, UObject* FortItem, EFortQuickBars Bars, int Slot, int Count = 1)
	{
		if (!Controller || !FortItem)
			return nullptr;

		static const auto FlooredVer = std::floor(FnVerDouble);

		// auto Inventory = GetInventory(Controller);

		auto ItemEntry = GetItemEntryFromInstance(FortItem);
		*FFortItemEntry::GetCount(ItemEntry) = Count;

		auto ItemInstances = GetItemInstances(Controller);

		if (!ItemInstances)
			return nullptr; // believe it or not, this happens sometimes

		if (Engine_Version < 424) // chapter two momentum
			ItemInstances->Add(FortItem);

		if (7.4 > FnVerDouble)
		{
			auto QuickBars = Inventory::GetQuickBars(Controller);

			if (QuickBars)
			{
				static UObject* GetItemGuidFn = FindObject(("Function /Script/FortniteGame.FortItem.GetItemGuid"));
				FGuid Guid;

				FortItem->ProcessEvent(GetItemGuidFn, &Guid);

				struct
				{
					FGuid Item;
					EFortQuickBars Quickbar;
					int Slot;
				} SAIIParams{ Guid, Bars, Slot };

				static auto SAIIFn = (QuickBars)->Function(("ServerAddItemInternal"));

				if (SAIIFn)
					(QuickBars)->ProcessEvent(SAIIFn, &SAIIParams);
			}
		}

		int Idx = -1;

		auto fortnite = GetItemEntryFromInstance<__int64>(FortItem); // FortItem->Member<EntryStruct>(("ItemEntry"));

		// std::cout << ("ItemEntryStruct Size: ") << GetEntrySize() << '\n';

		static __int64 ReplicatedEntriesOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortItemList"), ("ReplicatedEntries"));

		auto Inventory = GetInventory(Controller);

		Idx = ((TArray<__int64>*)(__int64(Inventory) + ReplicatedEntriesOffset))->Add(*fortnite, GetEntrySize());

		if (Idx != -1)
			Inventory::Update(Controller, -1, false, (FFastArraySerializerItem*)ItemEntry);

		return FortItem;
	}

	static UObject* CreateAndAddItem(UObject* Controller, UObject* Definition, EFortQuickBars Bars, int Slot, int Count = 1, bool bFillAmmo = false)
	{
		if (Controller && Definition)
		{
			auto Instance = CreateItemInstance(Controller, Definition, Count);

			if (Instance)
			{
				auto instance = AddItem(Controller, Instance, Bars, Slot, Count);

				auto entry = GetItemEntryFromInstance(instance);

				if (bFillAmmo && entry)
				{
					auto LoadedAmmo = FFortItemEntry::GetLoadedAmmo(entry);

					if (LoadedAmmo)
					{
						*LoadedAmmo = GetMaxBullets(Definition);
						Inventory::Update(Controller, -1, true, (FFastArraySerializerItem*)entry);
						// MarkItemDirty(Inventory::GetInventory(Controller), (FFastArraySerializerItem*)entry);
					}
				}
			}
			else
				std::cout << ("Failed to create ItemInstance!\n");

			return Instance;
		}

		return nullptr;
	}

	UObject* FindItemInInventory(UObject* Controller, const FGuid& Guid) 
	{
		auto ItemInstances = GetItemInstances(Controller);
		auto Pawn = Helper::GetPawnFromController(Controller);

		for (int i = 0; i < ItemInstances->Num(); i++)
		{
			auto CurrentItemInstance = ItemInstances->At(i);

			if (!CurrentItemInstance)
				continue;

			if (GetItemGuid(CurrentItemInstance) == Guid)
				return CurrentItemInstance;
		}

		return nullptr;
	}

	// Returns a item instance
	UObject* FindItemInInventory(UObject* Controller, UObject* Definition) // TODO: Return a vector.
	{
		auto ItemInstances = GetItemInstances(Controller);
		auto Pawn = Helper::GetPawnFromController(Controller);

		for (int i = 0; i < ItemInstances->Num(); i++)
		{
			auto CurrentItemInstance = ItemInstances->At(i);

			if (!CurrentItemInstance)
				continue;

			if (GetItemDefinition(CurrentItemInstance) == Definition)
				return CurrentItemInstance;
		}

		return nullptr;
	}

	void RemoveItem(UObject* Controller, const FGuid& Guid)
	{
		static const auto FlooredVer = std::floor(FnVerDouble);

		bool bFortnite = false;

		if (FlooredVer == 2)
		{
			struct ItemEntrySize { unsigned char Unk00[0xB8]; };
			bFortnite = RemoveGuidFromReplicatedEntries<ItemEntrySize>(Controller, Guid);
		}
		else if (FlooredVer == 3)
		{
			struct ItemEntrySize { unsigned char Unk00[0xC8]; };
			bFortnite = RemoveGuidFromReplicatedEntries<ItemEntrySize>(Controller, Guid);
		}
		else if (FlooredVer > 3 && FnVerDouble < 7.30)
		{
			struct ItemEntrySize { unsigned char Unk00[0xD0]; };
			bFortnite = RemoveGuidFromReplicatedEntries<ItemEntrySize>(Controller, Guid);
		}
		else if (FnVerDouble >= 7.30 && Engine_Version <= 424)
		{
			struct ItemEntrySize { unsigned char Unk00[0x120]; };
			bFortnite = RemoveGuidFromReplicatedEntries<ItemEntrySize>(Controller, Guid);
		}
		else if (Engine_Version == 425)
		{
			struct ItemEntrySize { unsigned char Unk00[0x150]; };
			bFortnite = RemoveGuidFromReplicatedEntries<ItemEntrySize>(Controller, Guid);
		}
		else if (Engine_Version >= 426 && FlooredVer < 15) // idk if right
		{
			struct ItemEntrySize { unsigned char Unk00[0x160]; };
			bFortnite = RemoveGuidFromReplicatedEntries<ItemEntrySize>(Controller, Guid);
		}
		else if (FlooredVer >= 15 && FlooredVer < 18)
		{
			struct ItemEntrySize { unsigned char Unk00[0x190]; };
			bFortnite = RemoveGuidFromReplicatedEntries<ItemEntrySize>(Controller, Guid);
		}
		else if (FlooredVer >= 18)
		{
			struct ItemEntrySize { unsigned char Unk00[0x1A0]; };
			bFortnite = RemoveGuidFromReplicatedEntries<ItemEntrySize>(Controller, Guid);
		}

		/*
		
		for (int x = 0; x < ReplicatedEntries->Num(); x++)
		{
			static auto GuidOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortItemEntry"), ("ItemGuid"));
			auto ItemEntry = (__int64*)(__int64(ReplicatedEntries) + (x * EntrySize));

			if (*(FGuid*)(__int64(ItemEntry) + GuidOffset) == Guid)
			{
				ReplicatedEntries->RemoveAt(x, EntrySize);
				bFortnite = true;
			}
		}

		*/

		if (bFortnite)
		{
			UObject* Def = nullptr;
			auto ItemInstances = Inventory::GetItemInstances(Controller);

			if (ItemInstances)
			{
				for (int i = 0; i < ItemInstances->Num(); i++)
				{
					auto ItemInstance = ItemInstances->At(i);

					if (Inventory::GetItemGuid(ItemInstance) == Guid)
					{
						Def = Inventory::GetItemDefinition(ItemInstance);
						ItemInstances->RemoveAt(i);
					}
				}
			}

			if (false)
			{
				if (FnVerDouble < 7.40 && Def)
				{
					auto QuickBars = GetQuickBars(Controller);

					static auto PrimaryQuickBarOffset = GetOffset(QuickBars, "PrimaryQuickBar");
					auto PrimaryQuickBar = (__int64*)(__int64(QuickBars) + PrimaryQuickBarOffset);

					static auto SecondaryQuickBarOffset = GetOffset(QuickBars, "SecondaryQuickBar");
					auto SecondaryQuickBar = (__int64*)(__int64(QuickBars) + SecondaryQuickBarOffset);

					auto QuickBarsToGoIn = QuickBars::WhatQuickBars(Def);
					auto QuickBar = QuickBarsToGoIn == EFortQuickBars::Primary ? PrimaryQuickBar : SecondaryQuickBar;

					auto getSlot = [](TArray<__int64>* Slots, int i) -> __int64* {
						static auto SizeOfQuickBarSlot = GetSizeOfStruct(FindObject("ScriptStruct /Script/FortniteGame.QuickBarSlot"));

						return (__int64*)(__int64(Slots) + (static_cast<__int64>(SizeOfQuickBarSlot) * i));
					};

					static auto SlotsOffset = FindOffsetStruct("ScriptStruct /Script/FortniteGame.QuickBar", "Slots");

					auto QuickBarSlots = (TArray<__int64>*)(__int64(QuickBar) + SlotsOffset);

					bool bFoundInQuickBar = false;

					for (int i = 0; i < QuickBarSlots->Num(); i++)
					{
						auto Slot = getSlot(QuickBarSlots, i);

						auto Items = (TArray<FGuid>*)(Slot);

						if (Items)
						{
							for (int j = 0; j < Items->Num(); j++)
							{
								if (Items->At(j) == Guid)
								{
									bFoundInQuickBar = true;

									static auto EmptySlot = QuickBars->Function("EmptySlot");

									struct { EFortQuickBars Bars; int SlotIndex; } EmptySlot_Params{ QuickBarsToGoIn, i };

									QuickBars->ProcessEvent(EmptySlot, &EmptySlot_Params);
									break;
								}
							}
						}
					}
				}
			}

			Update(Controller, -1, true);
		}
	}

	bool IncreaseItemCount(UObject* Controller, UObject* Instance, int Count) // stack
	{
		// For now, assume it's a consumable or ammo, and it can go higher than what we currently have.

		if (Controller && Instance)
		{
			auto ItemEntry = GetItemEntryFromInstance(Instance);
			auto CurrentCount = FFortItemEntry::GetCount(ItemEntry);

			// std::cout << std::format("Item going to stack on count: {} Picking up item count: {}", *CurrentCount, Count) << '\n';
			auto NewCount = *CurrentCount + Count;
			auto OldCount = *CurrentCount;
			*CurrentCount = NewCount;

			ChangeItemInReplicatedEntries<int>(Controller, Instance, ("Count"), NewCount);

			Update(Controller, -1, false, (FFastArraySerializerItem*)ItemEntry);

			return true;
		}

		return false;
	}

	bool DecreaseItemCount(UObject* Controller, UObject* Instance, int Count, __int64** OutItemEntry = nullptr, UObject* Definition = nullptr)
	{
		if (Controller && Instance)
		{
			auto ItemInstances = GetItemInstances(Controller);

			for (int j = 0; j < ItemInstances->Num(); j++)
			{
				auto ItemInstance = ItemInstances->At(j);

				if (!ItemInstance)
					continue;

				if (Definition ? GetItemDefinition(ItemInstance) == Definition : ItemInstance == Instance)
				{
					auto ItemEntry = GetItemEntryFromInstance(ItemInstance);
					auto CurrentCount = FFortItemEntry::GetCount(ItemEntry);

					// std::cout << std::format("Item going to stack on count: {} Picking up item count: {}", *CurrentCount, Count) << '\n';
					auto NewCount = *CurrentCount - Count;

					if (NewCount < 0)
						NewCount = 0;

					*CurrentCount = NewCount;

					ChangeItemInReplicatedEntries<int>(Controller, Instance, "Count", NewCount);

					Update(Controller, -1, false, (FFastArraySerializerItem*)ItemEntry);

					if (OutItemEntry)
						*OutItemEntry = ItemEntry;

					return true;
				}
			}
		}

		return false;
	}

	UObject* GetDefinitionFromGuid(UObject* Controller, const FGuid& Guid)
	{
		auto ItemInstances = GetItemInstances(Controller);
		UObject* ItemDefinition = nullptr;

		for (int j = 0; j < ItemInstances->Num(); j++)
		{
			auto ItemInstance = ItemInstances->At(j);

			if (!ItemInstance)
				continue;

			auto CurrentGuid = GetItemGuid(ItemInstance);

			if (CurrentGuid == Guid)
			{
				return GetItemDefinition(ItemInstance);
			}
		}

		return nullptr;
	}

	// RETURNS DEFINITION
	UObject* TakeItem(UObject* Controller, const FGuid& Guid, int Count = 1, bool bDestroyIfEmpty = false) // Use this, this removes from a definition
	{
		if (!Controller)
			return nullptr;

		__int64* Entry = nullptr;

		if (DecreaseItemCount(Controller, FindItemInInventory(Controller, Guid), Count, &Entry) && Entry) // it successfully decreased
		{
			if (*FFortItemEntry::GetCount(Entry) == 0 && bDestroyIfEmpty)
				RemoveItem(Controller, Guid);
		}
		else
			RemoveItem(Controller, Guid);

		if (newSwapping)
		{
			auto Pawn = Helper::GetPawnFromController(Controller);

			if (Pawn)
			{
				auto CurrentWeaponList = Pawn->Member<TArray<UObject*>>("CurrentWeaponList");

				if (CurrentWeaponList)
				{
					for (int i = 0; i < CurrentWeaponList->Num(); i++)
					{
						auto CurrentWeaponInList = CurrentWeaponList->At(i);

						if (CurrentWeaponInList && (*CurrentWeaponInList->Member<FGuid>("ItemEntryGuid") == Guid))
						{
							CurrentWeaponList->RemoveAt(i);
							break;
						}
					}
				}
			}
		}

		return FFortItemEntry::GetItemDefinition(Entry);
	}

	UObject* canStack(UObject* Controller, UObject* Definition)
	{
		UObject* ItemInstance = nullptr;

		auto ItemInstances = GetItemInstances(Controller);

		if (!ItemInstances)
			return nullptr;

		auto Pawn = Helper::GetPawnFromController(Controller);

		if (!Pawn)
			return nullptr;

		std::vector<UObject*> InstancesOfItem;

		for (int i = 0; i < ItemInstances->Num(); i++)
		{
			auto CurrentItemInstance = ItemInstances->At(i);

			if (CurrentItemInstance && GetItemDefinition(CurrentItemInstance) == Definition)
			{
				InstancesOfItem.push_back(CurrentItemInstance);
			}
		}

		if (InstancesOfItem.size() > 0)
		{
			auto MaxStackCount = GetMaxStackSize(Definition);

			// We need this skunked thing because if they have 2 full stacks and half a stack then we want to find the lowest stack and stack to there.
			for (auto InstanceOfItem : InstancesOfItem)
			{
				if (InstanceOfItem)
				{
					auto ItemEntry = GetItemEntryFromInstance(InstanceOfItem);

					if (ItemEntry)
					{
						auto currentCount = FFortItemEntry::GetCount(ItemEntry);

						if (currentCount && *currentCount < MaxStackCount)
						{
							ItemInstance = InstanceOfItem;
							break;
						}
					}
				}
			}
		}

		return ItemInstance;
	}

	// dont use bDidStack unless you know what ur doing
	// TODO: Make bdontcreatenewstack as bAllowMultipleStacks
	static UObject* GiveItem(UObject* Controller, UObject* Definition, EFortQuickBars Bars, int Slot, int Count = 1, bool bDontCreateNewStack = false, bool* skunk = false, UObject* ItemToStack = nullptr) // Use this, it stacks if it finds an item, if not, then it adds
	{
		if (Controller && Definition)
		{
			if (Slot == -1)
				Slot = FnVerDouble >= 7.40 ? 0 : QuickBars::FindNextAvailableSlot(Controller, Bars);

			static auto FortResourceItemDefinition = FindObject(("Class /Script/FortniteGame.FortResourceItemDefinition"));

			if (Definition->IsA(FortResourceItemDefinition))
				bDontCreateNewStack = true;

			auto Pawn = Helper::GetPawnFromController(Controller);

			UObject* ItemInstance = ItemToStack ? ItemToStack : canStack(Controller, Definition);
			int OverStack = 0;

			{
				{
					
					if (ItemInstance)
					{
						auto ItemEntry = GetItemEntryFromInstance(ItemInstance);

						if (ItemEntry)
						{
							auto currentCount = FFortItemEntry::GetCount(ItemEntry);

							if (currentCount)
							{
								//			      3	      +   2   -      6       =   -1
								OverStack = *currentCount + Count - GetMaxStackSize(Definition);

								// checks if it is going to overstack, if it is then we subtract the incoming count by the overstack, but its not then we just use the incoming count.
								int AmountToStack = OverStack > 0 ? Count - OverStack : Count;

								// std::cout << ("MaxStackCount: ") << MaxStackCount << '\n';
								std::cout << ("OverStack: ") << OverStack << '\n';
								std::cout << ("AmountToStack: ") << AmountToStack << '\n';
								std::cout << ("currentCount: ") << *currentCount << '\n';

								IncreaseItemCount(Controller, ItemInstance, AmountToStack);

								if (skunk)
									*skunk = false;

								if (OverStack <= 0) // there is no overstack, we can now return peacefully.
									return ItemInstance;

								if (skunk)
									*skunk = true;
							}
						}
					}
				}
			}

			// std::cout << ("count: ") << Count << '\n';
			// std::cout << ("OverStack: ") << OverStack << '\n';

			// If someway, the count is bigger than the MaxStackSize, and there is nothing to stack on, then it will not create 2 items.

			if (!ItemInstance || (OverStack > 0 && !bDontCreateNewStack))
			{
				if (OverStack > 0)
					return CreateAndAddItem(Controller, Definition, Bars, Slot, OverStack);
				else
					return CreateAndAddItem(Controller, Definition, Bars, Slot, Count);
			}
		}

		return nullptr;
	}

	int* GetCount(UObject* Instance)
	{
		return FFortItemEntry::GetCount(GetItemEntryFromInstance(Instance));
	}

	void GiveAllAmmo(UObject* Controller, int RocketCount = 999, int ShellCount = 999, int MediumCount = 999, int LightCount = 999, int HeavyCount = 999)
	{
		{
			static UObject* AthenaAmmoDataRockets = FindObject(("FortAmmoItemDefinition /Game/Athena/Items/Ammo/AmmoDataRockets.AmmoDataRockets"));
			static UObject* AthenaAmmoDataShells = FindObject(("FortAmmoItemDefinition /Game/Athena/Items/Ammo/AthenaAmmoDataShells.AthenaAmmoDataShells"));
			static UObject* AthenaAmmoDataBulletsMedium = FindObject(("FortAmmoItemDefinition /Game/Athena/Items/Ammo/AthenaAmmoDataBulletsMedium.AthenaAmmoDataBulletsMedium"));
			static UObject* AthenaAmmoDataBulletsLight = FindObject(("FortAmmoItemDefinition /Game/Athena/Items/Ammo/AthenaAmmoDataBulletsLight.AthenaAmmoDataBulletsLight"));
			static UObject* AthenaAmmoDataBulletsHeavy = FindObject(("FortAmmoItemDefinition /Game/Athena/Items/Ammo/AthenaAmmoDataBulletsHeavy.AthenaAmmoDataBulletsHeavy"));

			if (!AthenaAmmoDataRockets || !AthenaAmmoDataShells || !AthenaAmmoDataBulletsMedium || !AthenaAmmoDataBulletsLight || !AthenaAmmoDataBulletsHeavy)
				std::cout << "Some ammo is invalid!\n";

			CreateAndAddItem(Controller, AthenaAmmoDataRockets, EFortQuickBars::Secondary, 0, RocketCount);
			CreateAndAddItem(Controller, AthenaAmmoDataShells, EFortQuickBars::Secondary, 0, ShellCount);
			CreateAndAddItem(Controller, AthenaAmmoDataBulletsMedium, EFortQuickBars::Secondary, 0, MediumCount);
			CreateAndAddItem(Controller, AthenaAmmoDataBulletsLight, EFortQuickBars::Secondary, 0, LightCount);
			CreateAndAddItem(Controller, AthenaAmmoDataBulletsHeavy, EFortQuickBars::Secondary, 0, HeavyCount);
		}
	}

	void GiveMats(UObject* Controller, int WoodCount = 999, int StoneCount = 999, int MetalCount = 999)
	{
		static auto WoodItemData = FindObject(("FortResourceItemDefinition /Game/Items/ResourcePickups/WoodItemData.WoodItemData"));
		static auto StoneItemData = FindObject(("FortResourceItemDefinition /Game/Items/ResourcePickups/StoneItemData.StoneItemData"));
		static auto MetalItemData = FindObject(("FortResourceItemDefinition /Game/Items/ResourcePickups/MetalItemData.MetalItemData"));

		CreateAndAddItem(Controller, WoodItemData, EFortQuickBars::Secondary, 0, WoodCount);
		CreateAndAddItem(Controller, StoneItemData, EFortQuickBars::Secondary, 0, StoneCount);
		CreateAndAddItem(Controller, MetalItemData, EFortQuickBars::Secondary, 0, MetalCount);
	}

	void GiveBuildings(UObject* Controller)
	{
		static auto BuildingItemData_Wall = FindObject(("FortBuildingItemDefinition /Game/Items/Weapons/BuildingTools/BuildingItemData_Wall.BuildingItemData_Wall"));
		static auto BuildingItemData_Floor = FindObject(("FortBuildingItemDefinition /Game/Items/Weapons/BuildingTools/BuildingItemData_Floor.BuildingItemData_Floor"));
		static auto BuildingItemData_Stair_W = FindObject(("FortBuildingItemDefinition /Game/Items/Weapons/BuildingTools/BuildingItemData_Stair_W.BuildingItemData_Stair_W"));
		static auto BuildingItemData_RoofS = FindObject(("FortBuildingItemDefinition /Game/Items/Weapons/BuildingTools/BuildingItemData_RoofS.BuildingItemData_RoofS"));

		CreateAndAddItem(Controller, BuildingItemData_Wall, EFortQuickBars::Secondary, 0, 1);
		CreateAndAddItem(Controller, BuildingItemData_Floor, EFortQuickBars::Secondary, 1, 1);
		CreateAndAddItem(Controller, BuildingItemData_Stair_W, EFortQuickBars::Secondary, 2, 1);
		CreateAndAddItem(Controller, BuildingItemData_RoofS, EFortQuickBars::Secondary, 3, 1);
	}

	void GiveStartingItems(UObject* Controller)
	{
		static auto EditTool = FindObject(("FortEditToolItemDefinition /Game/Items/Weapons/BuildingTools/EditTool.EditTool"));

		CreateAndAddItem(Controller, EditTool, EFortQuickBars::Primary, 0, 1);
		GiveBuildings(Controller);
	}

	__int64* GetEntryFromWeapon(UObject* Controller, UObject* Weapon)
	{
		auto instance = Inventory::GetItemInstanceFromGuid(Controller, Inventory::GetWeaponGuid(Weapon)); // ahhhhhhhhhhhhhh
		return GetItemEntryFromInstance(instance);
	}
}

inline bool ServerExecuteInventoryItemHook(UObject* Controller, UFunction* Function, void* Parameters)
{
	// if (FnVerDouble < 16.00)
	// if (curtretwnapon != fguaid) ret

	Inventory::EquipInventoryItem(Controller, *(FGuid*)Parameters);

	return false;
}

inline bool ServerExecuteInventoryWeaponHook(UObject* Controller, UFunction* Function, void* Parameters)
{
	auto Weapon = (UObject**)Parameters;

	if (Weapon && *Weapon)
	{
		auto Pawn = Helper::GetPawnFromController(Controller);
		auto Guid = Inventory::GetWeaponGuid(*Weapon);
		auto Def = Helper::GetWeaponData(*Weapon);

		int* AmmoPTR = FFortItemEntry::GetLoadedAmmo(GetItemEntryFromInstance(Inventory::FindItemInInventory(Controller, Guid))); // TODO: implmeent

		// Inventory::EquipInventoryItem(Controller, Guid); // "hacky" // this doesjnt work maybe

		if (Def && AmmoPTR)
		{
			auto Ammo = *AmmoPTR;

			std::cout << " execute ammo: " << Ammo << '\n';

			Inventory::EquipWeaponDefinition(Pawn, Def, Guid, Ammo, FGuid(), true); // scuffed

			/* static auto ClientGivenTo = (*Weapon)->Function("ClientGivenTo");

			if (ClientGivenTo)
				(*Weapon)->ProcessEvent(ClientGivenTo, &Pawn);

			static auto ClientInternalEquipWeapon = Pawn->Function("ClientInternalEquipWeapon");

			struct { UObject* Weapon; } ClientInternalEquipWeapon_Params{ (*Weapon) };

			if (ClientInternalEquipWeapon)
				Pawn->ProcessEvent(ClientInternalEquipWeapon, &ClientInternalEquipWeapon_Params); */
		}
	}
	else
		std::cout << ("No weapon?\n");

	return false;
}

inline bool ServerAttemptInventoryDropHook(UObject* Controller, UFunction* Function, void* Parameters)
{
	if (Controller && Parameters && !Helper::IsInAircraft(Controller))
	{
		struct AFortPlayerController_ServerAttemptInventoryDrop_Params
		{
			FGuid                                       ItemGuid;                                                 // (Parm, ZeroConstructor, IsPlainOldData)
			int                                         Count;                                                    // (Parm, ZeroConstructor, IsPlainOldData)
		};

		auto Params = (AFortPlayerController_ServerAttemptInventoryDrop_Params*)Parameters;

		auto instance = Inventory::GetItemInstanceFromGuid(Controller, Params->ItemGuid); // ahhhhhhhhhhhhhh

		if (!instance)
		{
			std::cout << "Player is attempting to drop an item that they do not have!\n";
			return false;
		}

		if (Params->Count > *Inventory::GetCount(instance))
		{
			std::cout << "Player is attempting to drop an amount of items that they do not have!\n";
			return false;
		}

		auto ammo = *FFortItemEntry::GetLoadedAmmo(GetItemEntryFromInstance(instance));
		// std::cout << "ammo: " << ammo << '\n';
		auto Definition = Inventory::TakeItem(Controller, Params->ItemGuid, Params->Count, true);
		auto Pawn = Helper::GetPawnFromController(Controller);

		if (Pawn)
		{
			auto loc = Helper::GetActorLocation(Pawn);
			std::cout << "ammo: " << ammo << '\n';
;			auto Pickup = Helper::SummonPickup(Pawn, Definition, loc, EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::Unset, Params->Count, true, false, ammo);
		}
	}

	return false;
}

inline bool ServerHandlePickupHook(UObject* Pawn, UFunction* Function, void* Parameters)
{
	if (Pawn && Parameters && !bIsInEvent)
	{
		struct AFortPlayerPawn_ServerHandlePickup_Params
		{
			UObject* Pickup;                                                   // (Parm, ZeroConstructor, IsPlainOldData)
			float                                              InFlyTime;                                                // (Parm, ZeroConstructor, IsPlainOldData)
			FVector                                     InStartDirection;                                         // (ConstParm, Parm, ZeroConstructor, IsPlainOldData)
			bool                                               bPlayPickupSound;                                         // (Parm, ZeroConstructor, IsPlainOldData)
		};

		auto Params = (AFortPlayerPawn_ServerHandlePickup_Params*)Parameters;

		if (Params->Pickup && Pawn && basicLocationCheck(Pawn, Params->Pickup, 500.f))
		{
			FVector InStartDirection;
			float InFlyTime = 1.f;

			if (FnVerDouble >= 16)
			{
				auto PickupInfo = (__int64*)(__int64(Params) + 8);

				static auto InStartDirectionOffset = FindOffsetStruct("ScriptStruct /Script/FortniteGame.FortPickupRequestInfo", "Direction");
				static auto FlyTimeOffset = FindOffsetStruct("ScriptStruct /Script/FortniteGame.FortPickupRequestInfo", "FlyTime");

				auto inStartDirection = (FVector*)(__int64(PickupInfo) + InStartDirectionOffset);
				auto flyTime = (float*)(__int64(PickupInfo) + FlyTimeOffset);

				if (inStartDirection)
					InStartDirection = *inStartDirection;

				if (flyTime)
					InFlyTime = *flyTime;
			}
			else
			{
				InStartDirection = Params->InStartDirection;
				InFlyTime = Params->InFlyTime;
			}

			static auto bPickedUpOffset = GetOffset(Params->Pickup, "bPickedUp");
			bool* bPickedUp = (bool*)(__int64(Params->Pickup) + bPickedUpOffset);
			auto Controller = Helper::GetControllerFromPawn(Pawn);

			if (bPickedUp && !*bPickedUp && Controller)
			{
				auto PrimaryPickupItemEntry = Helper::GetEntryFromPickup(Params->Pickup);
				static auto ItemDefinitionOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortItemEntry"), ("ItemDefinition"));
				static auto CountOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortItemEntry"), ("Count"));

				// auto SwappinDef = *(*Controller)->Member<UObject*>(("SwappingItemDefinition")); // This is the same tihng as the pickup

				auto Definition = (UObject**)(__int64(&*PrimaryPickupItemEntry) + ItemDefinitionOffset);
				auto Count = (int*)(__int64(&*PrimaryPickupItemEntry) + CountOffset);

				auto ItemInstances = Inventory::GetItemInstances(Controller);
				
				auto quickBarsToGoIn = QuickBars::WhatQuickBars(*Definition);
				bool shouldGoInSecondaryBar = quickBarsToGoIn == EFortQuickBars::Secondary;

				bool bShouldSwap = false;
				int slotToGoInto = -1;

				if (!shouldGoInSecondaryBar)
				{
					if (true) // FnVerDouble >= 7.40)
					{
						int PrimaryQuickBarSlotsFilled = 0;

						for (int i = 0; i < ItemInstances->Num(); i++)
						{
							auto ItemInstance = ItemInstances->At(i);

							if (!ItemInstance)
								break;

							auto Definition = Inventory::GetItemDefinition(ItemInstance);

							if (QuickBars::WhatQuickBars(Definition) == EFortQuickBars::Primary)
								PrimaryQuickBarSlotsFilled++;
						}

						bShouldSwap = (PrimaryQuickBarSlotsFilled -= 6) >= 5; // so for some reason the primary quickbar are 11 with a full primary quickbar
					}
					else
					{
						slotToGoInto = QuickBars::FindNextAvailableSlot(Controller, quickBarsToGoIn);
						bShouldSwap = slotToGoInto == -1;
					}
				}

				auto ItemToStackInto = Inventory::canStack(Controller, *Definition);
				bShouldSwap = bShouldSwap && !ItemToStackInto;

				// auto incomingPickups = Pawn->Member<TArray<UObject*>>("IncomingPickups");

				if (bShouldSwap && (QuickBars::IsHoldingPickaxe(Controller)
					/* || incomingPickups->Num() >= 1) */)) // we should also check if the pickup is stackable, then it doesnt coutn but thats too complicated and slow so
					return false;

				// incomingPickups->Add(Params->Pickup);

				// *Controller->Member<UObject*>("SwappingItemDefinition") = bShouldSwap ? currentitem : nullptr;

				bool bSucceededSwap = true;

				Helper::EnablePickupAnimation(Pawn, Params->Pickup, 0.40f /* InFlyTime */, InStartDirection);

				if (bSucceededSwap)
				{
					// auto AthenaPickupManager = *AthenaHUD->Member<UObject*>("AthenaPickupManager");

					*bPickedUp = true;

					static auto bPickedUpFn = Params->Pickup->Function(("OnRep_bPickedUp")); 

					if (bPickedUpFn)
						Params->Pickup->ProcessEvent(bPickedUpFn);
				}

				bool bDidStack = false;
				UObject* newInstance = nullptr;

				if (Definition && *Definition && Count)
				{
					auto quickBarsItemShouldGoIn = QuickBars::WhatQuickBars(*Definition);

					newInstance = Inventory::GiveItem(Controller, *Definition, quickBarsItemShouldGoIn, slotToGoInto, *Count, &bDidStack, nullptr, ItemToStackInto);

					static auto ThanosID = FnVerDouble >= 8 ? FindObject("AthenaGadgetItemDefinition /Game/Athena/Items/Gameplay/BackPacks/Ashton/AGID_AshtonPack.AGID_AshtonPack") :
						FindObject("AthenaGadgetItemDefinition /Game/Athena/Items/Gameplay/BackPacks/CarminePack/AGID_CarminePack.AGID_CarminePack");

					bool bIsCarmine = *Definition == ThanosID;

					if (bIsCarmine) {
						Carmine::HandleCarmine(Controller);
					}

					if (/* bShouldSwap || */ bIsCarmine)
						Inventory::EquipInventoryItem(Controller, Inventory::GetItemGuid(newInstance));

					// Inventory::GetWorldInventory(Controller)->Member<TArray<UObject*>>("PendingInstances")->Add(newInstance);

					if (newInstance)
					{
						// the below crashes sometimes

						auto newEntry = GetItemEntryFromInstance(newInstance);
						auto newItemLoadedAmmo = FFortItemEntry::GetLoadedAmmo(newEntry);

						if (newItemLoadedAmmo)
						{
							auto pickupLoadedAmmo = FFortItemEntry::GetLoadedAmmo(PrimaryPickupItemEntry);

							if (pickupLoadedAmmo)
							{
								*newItemLoadedAmmo = *pickupLoadedAmmo;
								Inventory::Update(Controller, -1, true, (FFastArraySerializerItem*)newEntry);
								// MarkItemDirty(Inventory::GetInventory(Controller), (FFastArraySerializerItem*)newEntry);
							}
						}
					}

				}
				else
				{
					std::cout << "Detected invalid ptr!\n";
					std::cout << "Def: " << Definition << '\n';
					if (Definition)
						std::cout << "Def Deref: " << *Definition << '\n';
					std::cout << "Count: " << Count << '\n';
				}

				if (bShouldSwap && !bDidStack)
				{
					auto CurrentWeapon = Helper::GetCurrentWeapon(Pawn);
					auto HeldWeaponDef = Helper::GetWeaponData(CurrentWeapon);

					auto Entry = Inventory::GetEntryFromWeapon(Controller, CurrentWeapon);

					auto HeldWeaponAmmo = FFortItemEntry::GetLoadedAmmo(Entry); // *CurrentWeapon->Member<int>("AmmoCount");
					auto OldCount = FFortItemEntry::GetCount(Entry);

					if (OldCount && HeldWeaponAmmo)
					{
						Inventory::RemoveItem(Controller, Inventory::GetWeaponGuid(CurrentWeapon));

						auto DroppedPickup = Helper::SummonPickup(Pawn, HeldWeaponDef, Helper::GetActorLocation(Pawn), EFortPickupSourceTypeFlag::Player,
							EFortPickupSpawnSource::Unset, *OldCount, true, false, *HeldWeaponAmmo);

						if (bShouldSwap && Definition)
						{
							// Inventory::EquipWeaponDefinition(Pawn, *Definition, Inventory::GetItemGuid(newInstance));
						}
					}
				}
			}
		}
	}

	return false;
}

inline bool ServerCombineInventoryItemsHook(UObject* Controller, UFunction* Function, void* Parameters)
{
	struct ServerCombineInventoryItems_Params { FGuid TargetItemGuid; FGuid SourceItemGuid; };

	auto Params = (ServerCombineInventoryItems_Params*)Parameters;

	if (Params && Controller)
	{
		auto TargetInstance = Inventory::GetItemInstanceFromGuid(Controller, Params->TargetItemGuid); // the item we are combining into
		auto SourceInstance = Inventory::GetItemInstanceFromGuid(Controller, Params->SourceItemGuid); // the item we are going to be taking from

		if (!TargetInstance || !SourceInstance)
			return false;

		auto TargetDefinition = Inventory::GetItemDefinition(TargetInstance);
		auto SourceDefinition = Inventory::GetItemDefinition(SourceInstance);

		if (TargetDefinition != SourceDefinition)
			return false;

		auto TargetCount = Inventory::GetCount(TargetInstance);
		auto SourceCount = Inventory::GetCount(SourceInstance);

		if (!TargetCount || !SourceCount)
			return false;
		
		auto MaxStackCount = GetMaxStackSize(TargetDefinition);

		// dont ask how i made these bools up idfk

		auto AmountToTake = MaxStackCount - *TargetCount;

		auto bool1 = *TargetCount + AmountToTake <= MaxStackCount;
		auto bool2 = *SourceCount <= *TargetCount;
		auto bool3 = *SourceCount > AmountToTake;

		bool bDestroySource = bool1 && bool2;

		if (bool3 || !bDestroySource)
		{
			Inventory::DecreaseItemCount(Controller, SourceInstance, AmountToTake);
			Inventory::IncreaseItemCount(Controller, TargetInstance, AmountToTake);
		}
		else
		{
			Inventory::RemoveItem(Controller, Inventory::GetItemGuid(SourceInstance));
			Inventory::IncreaseItemCount(Controller, TargetInstance, *SourceCount);
		}
	}

	return false;
}

inline bool ServerHandlePickupWithSwapHook(UObject* Pawn, UFunction* Function, void* Parameters)
{
	struct AFortPlayerPawn_ServerHandlePickupWithSwap_Params
	{
		UObject* Pickup;                                                   // (Parm, ZeroConstructor, IsPlainOldData)
		FGuid                                       Swap;                                                     // (Parm, ZeroConstructor, IsPlainOldData)
		float                                              InFlyTime;                                                // (Parm, ZeroConstructor, IsPlainOldData)
		FVector                                     InStartDirection;                                         // (ConstParm, Parm, ZeroConstructor, IsPlainOldData)
		bool                                               bPlayPickupSound;                                         // (Parm, ZeroConstructor, IsPlainOldData)
	};
	auto Params = (AFortPlayerPawn_ServerHandlePickupWithSwap_Params*)Parameters;

	if (Params && Pawn && Params->Pickup)
	{
		bool* bPickedUp = Params->Pickup->Member<bool>(("bPickedUp"));

		if (bPickedUp && !*bPickedUp)
		{
			auto Controller = Pawn->CachedMember<UObject*>(("Controller"));

			auto PrimaryPickupItemEntry = Params->Pickup->Member<__int64>(("PrimaryPickupItemEntry"));
			static auto ItemDefinitionOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortItemEntry"), ("ItemDefinition"));
			static auto CountOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortItemEntry"), ("Count"));

			auto Definition = (UObject**)(__int64(&*PrimaryPickupItemEntry) + ItemDefinitionOffset);
			auto Count = (int*)(__int64(&*PrimaryPickupItemEntry) + CountOffset);

			struct
			{
				FGuid                                       ItemGuid;                                                 // (Parm, ZeroConstructor, IsPlainOldData)
				int                                                Count;                                                    // (Parm, ZeroConstructor, IsPlainOldData)
			} dropParams{ Params->Swap, 1 }; // (*Pawn->Member<UObject*>(("CurrentWeapon"))};

			ServerAttemptInventoryDropHook(*Controller, nullptr, &dropParams);

			struct
			{
				UObject* Pickup;                                                   // (Parm, ZeroConstructor, IsPlainOldData)
				float                                              InFlyTime;                                                // (Parm, ZeroConstructor, IsPlainOldData)
				FVector                                     InStartDirection;                                         // (ConstParm, Parm, ZeroConstructor, IsPlainOldData)
				bool                                               bPlayPickupSound;                                         // (Parm, ZeroConstructor, IsPlainOldData)
			} pickupParams{Params->Pickup, Params->InFlyTime, Params->InStartDirection, Params->bPlayPickupSound};

			ServerHandlePickupHook(Pawn, nullptr, &pickupParams);
		}
	}

	return false;
}

void __fastcall HandleReloadCostDetour(UObject* Weapon, int AmountToRemove)
{
	std::cout << "wtf!\n";

	if (!Weapon)
		return;

	/* static auto addr = FindPattern("48 89 5C 24 ? 57 48 83 EC 20 48 8B F9 E8 ? ? ? ? 48 8B D8 48 85 C0 74 32 E8 ? ? ? ? 48 8B 4B 10 48 83 C0 30 48 63 50 08 3B 51 38 7F 1C 48 8B 49 30 48 39 04 D1");

	__int64 (*fortnite)(UObject* wep);

	fortnite = decltype(fortnite)(addr);

	auto idk = (__int64*)fortnite(Weapon);

	auto Controller = (UObject*)(__int64(idk) - 0x688);

	auto pawnbnseghwea = idk;

	// std::cout << "OG: " << std::to_string(*(char*)(pawnbnseghwea - 0x570)) << '\n';
	// *(char*)(pawnbnseghwea - 0x570) = 2;

	std::cout << "+ 88: " << __int64((*(uintptr_t*)(idk + 88)) - (uintptr_t)GetModuleHandleW(0)) << '\n';
	std::cout << "+ 160: " << __int64((*(uintptr_t*)(idk + 160)) - (uintptr_t)GetModuleHandleW(0)) << '\n'; */

	static auto GetOwner = Weapon->Function(("GetOwner"));
	UObject* Pawn;
	Weapon->ProcessEvent(GetOwner, &Pawn);

	if (!Pawn)
	{
		std::cout << "Unable to get weapon for pawn, this should not happen.\n";
		return;
	}

	std::cout << "wtfa!\n";

	auto PlayerController = Helper::GetControllerFromPawn(Pawn);

	if (PlayerController)
	{
		auto WeaponGuidPtr = Inventory::GetWeaponGuid(Weapon);

		// if (!WeaponGuidPtr) // i hate u vehicles
			// return;

		auto WeaponGuid = WeaponGuidPtr;

		auto WeaponItemEntry = GetItemEntryFromInstance(Inventory::FindItemInInventory(PlayerController, WeaponGuid));

		if (!WeaponItemEntry)
			return;

		auto WeaponData = Helper::GetWeaponData(Weapon);

		static auto GetAmmoWorldItemDefinition_BP = WeaponData->Function(("GetAmmoWorldItemDefinition_BP"));
		UObject* AmmoDef = nullptr;

		if (GetAmmoWorldItemDefinition_BP)
			WeaponData->ProcessEvent(GetAmmoWorldItemDefinition_BP, &AmmoDef);

		static auto FortTrapItemDefinitionClass = FindObject("Class /Script/FortniteGame.FortTrapItemDefinition");

		std::cout << "WeaponData Name: " << WeaponData->GetFullName() << '\n';
		
		if (!AmmoDef || WeaponData->IsA(FortTrapItemDefinitionClass))
			AmmoDef = WeaponData;

		__int64* AmmoEntry = nullptr;

		auto instance = Inventory::FindItemInInventory(PlayerController, AmmoDef);
		AmmoEntry = GetItemEntryFromInstance(instance);

		if (AmmoEntry)
		{
			// *FFortItemEntry::GetCount(ItemEntry) -= AmountToRemove;
			// *FFortItemEntry::GetLoadedAmmo(ItemEntry) += AmountToRemove;
			// *Weapon->Member<int>("AmmoCount") += AmountToRemove;

			HandleReloadCost(Weapon, AmountToRemove);

			auto WeaponLoadedAmmo = FFortItemEntry::GetLoadedAmmo(WeaponItemEntry);

			static auto AmmoCountOffset = GetOffset(Weapon, "AmmoCount");
			auto WeaponAmmoCount = (int*)(__int64(Weapon) + AmmoCountOffset);

			if (WeaponLoadedAmmo && WeaponAmmoCount)
			{
				std::cout << "wep ammo: " << *WeaponAmmoCount << '\n';
				std::cout << "WEaponLoadedAMmo: " << *WeaponLoadedAmmo << '\n';

				*WeaponLoadedAmmo = *WeaponAmmoCount;
				// setBitfield(Weapon, "bUpdateLocalAmmoCount", true);

				Inventory::Update(PlayerController, -1, true, (FFastArraySerializerItem*)WeaponItemEntry);

				/* auto ReplicatedEntries = Inventory::GetReplicatedEntries<__int64>(PlayerController);

				for (int i = 0; i < ReplicatedEntries->Num(); i++)
				{
					auto ReplicatedEntry = (__int64*)(__int64(ReplicatedEntries->GetData()) + (GetEntrySize() * i));

					if (*FFortItemEntry::GetGuid(ReplicatedEntry) == WeaponGuid)
					{
						std::cout << "Found at: " << i << '\n';

						Inventory::Update(PlayerController, -1, true, (FFastArraySerializerItem*)ReplicatedEntry);
					}
				} */

				// Inventory::GetWorldInventory(PlayerController)->ProcessEvent("ForceNetUpdate");

				if (bIsPlayground)
					return;

				Inventory::DecreaseItemCount(PlayerController, instance, AmountToRemove, &AmmoEntry);

				if (AmmoEntry)
				{
					auto ammocounnt = FFortItemEntry::GetCount(AmmoEntry);

					if (ammocounnt && *ammocounnt <= 0) // Destroy the item if it has no count
					{
						Inventory::RemoveItem(PlayerController, *FFortItemEntry::GetGuid(AmmoEntry));
					}
				}
			}
		}
	}
	else
	{
		std::cout << "Unable to get controller from pawn, this should not happen.\n";
	}
}

// auto picks up ammo when dropped idk why
inline bool OnCapsuleBeginOverlapHook(UObject* Pawn, UFunction* Function, void* Parameters) // this is for like consumables
{
	if (Pawn)
	{
		struct PartOfTheArgs { UObject* OverlappedComp; UObject* OtherActor; };

		auto Params = (PartOfTheArgs*)Parameters;

		auto Pickup = Params->OtherActor;

		static UObject* PickupClass = FindObject(("Class /Script/FortniteGame.FortPickupAthena"));

		if (Pickup && Pickup->IsA(PickupClass)) // can it be anything other?
		{
			struct
			{
				UObject* Pickup;                                                   // (Parm, ZeroConstructor, IsPlainOldData)
				float                                              InFlyTime;                                                // (Parm, ZeroConstructor, IsPlainOldData)
				FVector                                     InStartDirection;                                         // (ConstParm, Parm, ZeroConstructor, IsPlainOldData)
				bool                                               bPlayPickupSound;                                         // (Parm, ZeroConstructor, IsPlainOldData)
			} AFortPlayerPawn_ServerHandlePickup_Params{Pickup, 0.75f, FVector(), true};

			ServerHandlePickupHook(Pawn, nullptr,&AFortPlayerPawn_ServerHandlePickup_Params);
		}
	}

	return false;
}

inline bool OnAboutToEnterBackpackHook(UObject* PickupEffect, UFunction* func, void* Parameters)
{
	static auto ReceiveEndPlayFunc = FindObject("Function /Script/Engine.Actor.ReceiveEndPlay");
	static auto OnAboutToEnterBackpackFunc = FindObject("Function /Game/Effects/Fort_Effects/Gameplay/Pickups/B_Pickups_Parent.B_Pickups_Parent_C.OnAboutToEnterBackpack");
	static auto PickupClass = FindObject(("Class /Script/FortniteGame.FortPickupAthena"));

	UObject* Pawn = nullptr;
	UObject* Pickup = nullptr;

	if (func == ReceiveEndPlayFunc && PickupEffect->IsA(PickupClass) && !bRestarting)
	{
		Pickup = PickupEffect; 

		auto PickupLocationData = Pickup->Member<FFortPickupLocationData>("PickupLocationData");

		Pawn = PickupLocationData->ItemOwner;
	}

	else if (func == OnAboutToEnterBackpackFunc)
	{
		Pawn = *(UObject**)(Parameters);
		Pickup = Helper::GetOwner(PickupEffect);
	}

	if (Pawn && Pickup)
	{
		auto Controller = Helper::GetControllerFromPawn(Pawn);

		auto PrimaryPickupItemEntry = Helper::GetEntryFromPickup(Pickup);
		static auto ItemDefinitionOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortItemEntry"), ("ItemDefinition"));
		static auto CountOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortItemEntry"), ("Count"));

		auto Definition = (UObject**)(__int64(&*PrimaryPickupItemEntry) + ItemDefinitionOffset);
		auto Count = (int*)(__int64(&*PrimaryPickupItemEntry) + CountOffset);

		auto ItemInstances = Inventory::GetItemInstances(Controller);

		if (!ItemInstances)
			return false;

		auto quickBarsToGoIn = QuickBars::WhatQuickBars(*Definition);
		bool shouldGoInSecondaryBar = quickBarsToGoIn == EFortQuickBars::Secondary;

		bool bShouldSwap = false;
		int slotToGoInto = -1;

		std::cout << "ehaa!\n";

		// TODO: For >7.40 check all quickbar slots until we find a empty one.

		int PrimaryQuickBarSlotsFilled = 0;

		if (!shouldGoInSecondaryBar)
		{
			if (true) // if (FnVerDouble >= 7.40)
			{
				for (int i = 0; i < ItemInstances->Num(); i++)
				{
					auto ItemInstance = ItemInstances->At(i);

					if (!ItemInstance)
						break;

					auto Definition = Inventory::GetItemDefinition(ItemInstance);

					if (QuickBars::WhatQuickBars(Definition) == EFortQuickBars::Primary)
						PrimaryQuickBarSlotsFilled++;
				}

				bShouldSwap = (PrimaryQuickBarSlotsFilled -= 6) >= 5; // so for some reason the primary quickbar are 11 with a full primary quickbar
			}
			else
			{
				slotToGoInto = QuickBars::FindNextAvailableSlot(Controller, quickBarsToGoIn);
				bShouldSwap = slotToGoInto == -1;
			}
		}

		bool bSucceededSwap = true;

		auto CurrentWeapon = Helper::GetCurrentWeapon(Pawn);
		auto HeldWeaponDef = Helper::GetWeaponData(CurrentWeapon);

		bool bDidStack = false;

		UObject* newInstance = nullptr;

		auto ItemToStackInto = Inventory::canStack(Controller, *Definition);
		bShouldSwap = bShouldSwap && !ItemToStackInto;

		auto incomingPickups = Pawn->Member<TArray<UObject*>>("IncomingPickups");

		std::cout << "PrimaryQuickBarSlotsFilled: " << PrimaryQuickBarSlotsFilled << '\n';
		std::cout << "bShouldSwap: " << bShouldSwap << '\n';
		std::cout << "ItemToStackInto: " << ItemToStackInto << '\n';
		std::cout << "ehbb!\n";

		if (bShouldSwap && QuickBars::IsHoldingPickaxe(Controller))
			return false;

		std::cout << "eccc!\n";

		if (Definition && *Definition && Count)
		{
			auto quickBarsItemShouldGoIn = QuickBars::WhatQuickBars(*Definition);

			newInstance = Inventory::GiveItem(Controller, *Definition, quickBarsItemShouldGoIn, slotToGoInto, *Count, &bDidStack, nullptr, ItemToStackInto);

			// Inventory::GetWorldInventory(Controller)->Member<TArray<UObject*>>("PendingInstances")->Add(newInstance);

			if (newInstance)
			{
				// the below crashes sometimes

				auto newItemLoadedAmmo = FFortItemEntry::GetLoadedAmmo(GetItemEntryFromInstance(newInstance));

				if (newItemLoadedAmmo)
				{
					auto pickupLoadedAmmo = FFortItemEntry::GetLoadedAmmo(PrimaryPickupItemEntry);

					if (pickupLoadedAmmo)
					{
						*newItemLoadedAmmo = *pickupLoadedAmmo;
						Inventory::Update(Controller, -1, true, (FFastArraySerializerItem*)PrimaryPickupItemEntry);
						// MarkItemDirty(Inventory::GetInventory(Controller), (FFastArraySerializerItem*)PrimaryPickupItemEntry);
					}
				}
			}

		}
		else
		{
			std::cout << "Detected invalid ptr!\n";
			std::cout << "Def: " << Definition << '\n';
			if (Definition)
				std::cout << "Def Deref: " << *Definition << '\n';
			std::cout << "Count: " << Count << '\n';
		}

		if (bShouldSwap && !bDidStack)
		{
			auto Entry = Inventory::GetEntryFromWeapon(Controller, CurrentWeapon);

			auto HeldWeaponAmmo = FFortItemEntry::GetLoadedAmmo(Entry); // *CurrentWeapon->Member<int>("AmmoCount");
			auto OldCount = FFortItemEntry::GetCount(Entry);

			if (OldCount && HeldWeaponAmmo)
			{
				Inventory::RemoveItem(Controller, Inventory::GetWeaponGuid(CurrentWeapon));

				auto DroppedPickup = Helper::SummonPickup(Pawn, HeldWeaponDef, Helper::GetActorLocation(Pawn), EFortPickupSourceTypeFlag::Player,
					EFortPickupSpawnSource::Unset, *OldCount, true, false, *HeldWeaponAmmo);

				if (bShouldSwap && Definition)
				{
					// Inventory::EquipInventoryItem(Controller, Inventory::GetItemGuid(newInstance));
					// Inventory::EquipWeaponDefinition(Pawn, *Definition, Inventory::GetItemGuid(newInstance));
				}
			}
		}
	}

	return false;
}

void InitializeInventoryHooks()
{
	// AddHook("Function /Script/FortniteGame.FortPlayerPawn.OnCapsuleBeginOverlap", OnCapsuleBeginOverlapHook);

	if (FnVerDouble < 16.00)
		AddHook(("Function /Script/FortniteGame.FortPlayerController.ServerExecuteInventoryItem"), ServerExecuteInventoryItemHook);

	AddHook("Function /Script/FortniteGame.FortPlayerController.ServerCombineInventoryItems", ServerCombineInventoryItemsHook);

	if (Engine_Version >= 423 && FnVerDouble < 16.00)
		AddHook(("Function /Script/FortniteGame.FortPlayerController.ServerExecuteInventoryWeapon"), ServerExecuteInventoryWeaponHook);

	// if (FnVerDouble >= 7)
		// AddHook("Function /Game/Effects/Fort_Effects/Gameplay/Pickups/B_Pickups_Parent.B_Pickups_Parent_C.OnAboutToEnterBackpack", OnAboutToEnterBackpackHook);
	// else
		// AddHook("Function /Script/Engine.Actor.ReceiveEndPlay", OnAboutToEnterBackpackHook); //  Function /Game/Effects/Fort_Effects/Gameplay/Pickups/B_Pickups.B_Pickups_C.ReceiveDestroyed

	if (Engine_Version >= 420)
		AddHook(("Function /Script/FortniteGame.FortPlayerController.ServerAttemptInventoryDrop"), ServerAttemptInventoryDropHook);
	else
		AddHook(("Function /Script/FortniteGame.FortPlayerController.ServerSpawnInventoryDrop"), ServerAttemptInventoryDropHook);

	if (FnVerDouble >= 7.40)
	{
		AddHook(("Function /Script/FortniteGame.FortPlayerPawn.ServerHandlePickupWithSwap"), ServerHandlePickupWithSwapHook);
	}

	if (FnVerDouble >= 13) // idk
	{
		AddHook("Function /Script/FortniteGame.FortPlayerPawn.ServerHandlePickupInfo", ServerHandlePickupHook);
	}
	else
	{
		AddHook(("Function /Script/FortniteGame.FortPlayerPawn.ServerHandlePickup"), ServerHandlePickupHook);
	}

	if (HandleReloadCost)
	{
		MH_CreateHook((PVOID)HandleReloadCostAddr, HandleReloadCostDetour, (void**)&HandleReloadCost);
		MH_EnableHook((PVOID)HandleReloadCostAddr);
	}
}

bool IsDroppable(UObject* Controller, UObject* CurrentItemDefinition, bool bTakePickaxe = true)
{
	static auto BuildingItemData_Wall = FindObject(("FortBuildingItemDefinition /Game/Items/Weapons/BuildingTools/BuildingItemData_Wall.BuildingItemData_Wall"));
	static auto BuildingItemData_Floor = FindObject(("FortBuildingItemDefinition /Game/Items/Weapons/BuildingTools/BuildingItemData_Floor.BuildingItemData_Floor"));
	static auto BuildingItemData_Stair_W = FindObject(("FortBuildingItemDefinition /Game/Items/Weapons/BuildingTools/BuildingItemData_Stair_W.BuildingItemData_Stair_W"));
	static auto BuildingItemData_RoofS = FindObject(("FortBuildingItemDefinition /Game/Items/Weapons/BuildingTools/BuildingItemData_RoofS.BuildingItemData_RoofS"));
	static auto PickaxeDef = Helper::GetPickaxeDef(Controller);

	if (CurrentItemDefinition == BuildingItemData_Wall || CurrentItemDefinition == BuildingItemData_Floor
		|| CurrentItemDefinition == (BuildingItemData_Stair_W) || CurrentItemDefinition == (BuildingItemData_RoofS) || CurrentItemDefinition == PickaxeDef)
		return false;

	return true;
}

void ClearInventory(UObject* Controller, bool bTakePickaxe = false)
{
	if (FnVerDouble >= 10)
		return;

	static auto PickaxeDef = Helper::GetPickaxeDef(Controller);

	auto ItemInstances = Inventory::GetItemInstances(Controller);

	if (ItemInstances)
	{
		auto NumItemInstances = ItemInstances->Num();
		std::cout << "ItemInstances->Num(): " << NumItemInstances << '\n';

		int Start = bTakePickaxe ? 5 : 6; // 0
		int loopedThrough = Start;

		// while (loopedThrough <= NumItemInstances)
		{
			for (int i = loopedThrough; i < NumItemInstances; i++)
			{
				auto CurrentItemInstance = ItemInstances->At(i);

				if (!CurrentItemInstance || IsBadReadPtr(CurrentItemInstance))
					continue;

				auto CurrentItemDefinition = Inventory::GetItemDefinition(CurrentItemInstance);

				if (!CurrentItemDefinition || IsBadReadPtr(CurrentItemDefinition))
					continue;

				// if (!CurrentItemInstance || !CurrentItemDefinition || readBitfield(CurrentItemDefinition, "bCanBeDropped")) // dont ask
					// continue;

				// std::cout << "CurrentItemDefinition: " << CurrentItemDefinition->GetFullName() << '\n';

				Inventory::RemoveItem(Controller, Inventory::GetItemGuid(CurrentItemInstance));
				// loopedThrough++;
			}
		}

		std::cout << "loopedThrough: " << loopedThrough << '\n';
	}

	std::cout << "bro!\n";

	static auto WoodItemData = FindObject(("FortResourceItemDefinition /Game/Items/ResourcePickups/WoodItemData.WoodItemData"));
	static auto StoneItemData = FindObject(("FortResourceItemDefinition /Game/Items/ResourcePickups/StoneItemData.StoneItemData"));
	static auto MetalItemData = FindObject(("FortResourceItemDefinition /Game/Items/ResourcePickups/MetalItemData.MetalItemData"));

	auto WoodInstance = Inventory::FindItemInInventory(Controller, WoodItemData);

	if (WoodInstance)
		Inventory::TakeItem(Controller, Inventory::GetItemGuid(WoodInstance), *Inventory::GetCount(WoodInstance), true);

	auto StoneInstance = Inventory::FindItemInInventory(Controller, StoneItemData);

	if (StoneInstance)
		Inventory::TakeItem(Controller, Inventory::GetItemGuid(StoneInstance), *Inventory::GetCount(StoneInstance), true);

	auto MetalInstance = Inventory::FindItemInInventory(Controller, MetalItemData);

	if (MetalInstance)
		Inventory::TakeItem(Controller, Inventory::GetItemGuid(MetalInstance), *Inventory::GetCount(MetalInstance), true);
}