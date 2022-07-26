#pragma once

#include <UE/structs.h>
#include <Gameplay/helper.h>

static int GetEntrySize()
{
	static auto FortItemEntryClass = FindObject(_("ScriptStruct /Script/FortniteGame.FortItemEntry"), true);

	return GetSizeOfStruct(FortItemEntryClass);
}

namespace FFortItemEntry
{
	int* GetCount(__int64* Entry)
	{
		static auto CountOffset = FindOffsetStruct(_("ScriptStruct /Script/FortniteGame.FortItemEntry"), _("Count"));
		auto Count = (int*)(__int64(&*Entry) + CountOffset);

		return Count;
	}

	FGuid* GetGuid(__int64* Entry)
	{
		static auto GuidOffset = FindOffsetStruct(_("ScriptStruct /Script/FortniteGame.FortItemEntry"), _("ItemGuid"));
		auto Guid = (FGuid*)(__int64(&*Entry) + GuidOffset);

		return Guid;
	}

	UObject* GetItemDefinition(__int64* Entry)
	{
		static auto ItemDefinitionOffset = FindOffsetStruct(_("ScriptStruct /Script/FortniteGame.FortItemEntry"), _("ItemDefinition"));
		auto Definition = (UObject**)(__int64(&*Entry) + ItemDefinitionOffset);

		return *Definition;
	}
}

namespace Inventory
{
	__int64* GetInventory(UObject* Controller)
	{
		auto WorldInventory = *Controller->Member<UObject*>(_("WorldInventory"));
		auto Inventorya = WorldInventory->Member<__int64>(_("Inventory"));

		return Inventorya;
	}

	TArray<UObject*>* GetItemInstances(UObject* Controller)
	{
		static __int64 ItemInstancesOffset = FindOffsetStruct(_("ScriptStruct /Script/FortniteGame.FortItemList"), _("ItemInstances"));

		auto Inventory = GetInventory(Controller);

		// std::cout << _("ItemInstances Offset: ") << ItemInstancesOffset << '\n';

		// ItemInstancesOffset = 0x110;

		return (TArray<UObject*>*)(__int64(Inventory) + ItemInstancesOffset);
	}

	inline void EquipWeapon(UObject* Pawn, UObject* FortWeapon, const FGuid& Guid, int Ammo = 0)
	{
		static auto PickaxeDef = FindObject(_("FortWeaponMeleeItemDefinition /Game/Athena/Items/Weapons/WID_Harvest_Pickaxe_Athena_C_T01.WID_Harvest_Pickaxe_Athena_C_T01"));
		auto CurrentWeapon = *Pawn->Member<UObject*>(_("CurrentWeapon"));
		if (FortWeapon && Pawn)
		{
			//if (*CurrentWeapon->Member<UObject*>(_("WeaponData")) != PickaxeDef)
			{
				static auto getBulletsFn = FortWeapon->Function(_("GetBulletsPerClip"));
				if (getBulletsFn)
				{
					int BulletsPerClip;
					FortWeapon->ProcessEvent(getBulletsFn, &BulletsPerClip);
					*FortWeapon->Member<int>(_("AmmoCount")) = BulletsPerClip;
				}
				else
					std::cout << _("No GetBulletsPerClip!\n");
			}

			// Instance->ItemEntry.LoadedAmmo = Weapon->AmmoCount;

			FortWeapon->ProcessEvent(_("OnRep_ReplicatedWeaponData"));

			// the functgion below starts taking a param
			// Weapon->ProcessEvent(_("OnRep_AmmoCount")); // crashes :( (I think it's because if we call it on a pickaxe??)

			struct { UObject* P; } givenParams{ Pawn };

			static auto givenFn = FortWeapon->Function(_("ClientGivenTo"));
			if (givenFn)
				givenFn->ProcessEvent(givenFn, &givenParams);
			else
				std::cout << _("No ClientGivenTo!\n");

			struct { UObject* Weapon; } internalEquipParams{ FortWeapon };

			if (std::stod(FN_Version) >= 7.40)
			{
				static auto internalEquipFn = Pawn->Function(_("ClientInternalEquipWeapon"));
				if (internalEquipFn)
					Pawn->ProcessEvent(internalEquipFn, &internalEquipParams);
				else
					std::cout << _("No ClientInternalEquipWeapon!\n");
			}

			std::cout << _("Noob!\n");

			// Pawn->OnRep_CurrentWeapon(); // i dont think this is needed but alr
		}
		else
			std::cout << _("No weapon!\n");
	}

	inline UObject* EquipWeaponDefinition(UObject* Pawn, UObject* Definition, const FGuid& Guid, int Ammo = 0)
	{
		if (Pawn)
		{
			static auto fn = Definition->Function(_("GetWeaponActorClass"));
			if (fn)
			{
				/* // UObject* WeaponClass = nullptr;
				struct { UObject* WeaponClass; }Params{};
				Definition->ProcessEvent(fn, &Params);
				auto WeaponClass = Params.WeaponClass;
				if (WeaponClass)
				{
					auto Weapon = Easy::SpawnActor(WeaponClass);
				}
				else
					std::cout << _("Invalid WeaponClass!\n");

				return WeaponClass; */

				static auto equipFn = Pawn->Function(_("EquipWeaponDefinition"));
				struct {
					UObject* Def;
					FGuid Guid;
					UObject* Wep;
				} params{ Definition, Guid };
				Pawn->ProcessEvent(equipFn, &params);
				auto Weapon = params.Wep;

				if (Weapon)
				{
					Helper::SetOwner(Weapon, Pawn);
					//*Weapon->Member<UObject*>(_("WeaponData")) = Definition;
					//EquipWeapon(Pawn, Weapon, Guid, Ammo);
					return Weapon;
				}
				else
					std::cout << _("Failed to spawn Weapon!\n");

				return Weapon;
			}
			else
				std::cout << _("Failed to find GetWeaponActorClass!\n");
		}

		return nullptr;
	}

	FGuid GetItemGuid(UObject* ItemInstance)
	{
		static UObject* GetItemGuidFn = FindObject(_("Function /Script/FortniteGame.FortItem.GetItemGuid"));
		FGuid Guid;

		ItemInstance->ProcessEvent(GetItemGuidFn, &Guid);

		return Guid;
	}

	UObject* GetItemDefinition(UObject* ItemInstance)
	{
		static UObject* GetDefFn = FindObject(_("Function /Script/FortniteGame.FortItem.GetItemDefinitionBP"));
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
		static auto fn = Controller->Function(_("GetNumQuickbarSlots"));

		if (Controller && fn)
			Controller->ProcessEvent(fn, &parms);

		return parms.Ret;
	}

	inline UObject* EquipInventoryItem(UObject* Controller, FGuid& Guid)
	{
		auto ItemInstances = GetItemInstances(Controller);
		auto Pawn = *Controller->Member<UObject*>(_("Pawn"));

		for (int i = 0; i < ItemInstances->Num(); i++)
		{
			auto CurrentItemInstance = ItemInstances->At(i);

			if (!CurrentItemInstance)
				continue;

			if (GetItemGuid(CurrentItemInstance) == Guid)
			{
				auto Def = GetItemDefinition(CurrentItemInstance);
				/*if (Def != nullptr) {
					if (std::stof(FN_Version) >= 7.40) {
						return EquipWeaponDefinition(Pawn, Def, Guid);
					}
					else {
						EquipWeapon(Pawn, Def, Guid);
					}
				}*/
				EquipWeaponDefinition(Pawn, Def, Guid);
			}
		}

		return nullptr;
	}

	template <typename EntryStruct>
	TArray<EntryStruct>* GetReplicatedEntries(UObject* Controller)
	{
		static __int64 ReplicatedEntriesOffset = FindOffsetStruct(_("ScriptStruct /Script/FortniteGame.FortItemList"), _("ReplicatedEntries"));

		auto Inventory = GetInventory(Controller);
		// ReplicatedEntriesOffset = 0xB0;

		// std::cout << _("ReplicatedEntries Offset: ") << ReplicatedEntriesOffset << '\n';

		return (TArray<EntryStruct>*)(__int64(Inventory) + ReplicatedEntriesOffset);

		// return (TArray<EntryStruct>*)(__int64(Inventory) + ReplicatedEntriesOffset);
	}

	void Update(UObject* Controller, int Idx = -1, bool bRemovedItem = false, FFastArraySerializerItem* ModifiedItem = nullptr)
	{
		static const auto FnVerDouble = std::stod(FN_Version);

		auto WorldInventory = *Controller->Member<UObject*>(_("WorldInventory"));
		auto Inventory = GetInventory(Controller);

		static auto WorldHandleInvUpdate = WorldInventory->Function(_("HandleInventoryLocalUpdate"));

		if (WorldHandleInvUpdate)
		{
			WorldInventory->ProcessEvent(WorldHandleInvUpdate);

			if (std::floor(FnVerDouble) < 10) // idk crashes
			{
				static auto PCHandleInvUpdate = Controller->Function(_("HandleWorldInventoryLocalUpdate"));

				if (PCHandleInvUpdate)
					Controller->ProcessEvent(PCHandleInvUpdate);
			}
		}

		if (FnVerDouble < 7.4)
		{
			const auto QuickBars = *Controller->Member<UObject*>(_("QuickBars"));

			if (QuickBars)
			{
				static auto OnRep_PrimaryQuickBar = QuickBars->Function(_("OnRep_PrimaryQuickBar"));
				QuickBars->ProcessEvent(OnRep_PrimaryQuickBar);

				static auto OnRep_SecondaryQuickBar = QuickBars->Function(_("OnRep_SecondaryQuickBar"));
				QuickBars->ProcessEvent(OnRep_SecondaryQuickBar);
			} 
		}
		else
		{
			static auto ClientForceUpdateQuickbar = Controller->Function(_("ClientForceUpdateQuickbar"));
			auto PrimaryQuickbar = EFortQuickBars::Primary;
			Controller->ProcessEvent(ClientForceUpdateQuickbar, &PrimaryQuickbar);
			auto SecondaryQuickbar = EFortQuickBars::Secondary;
			Controller->ProcessEvent(ClientForceUpdateQuickbar, &SecondaryQuickbar);

			if (FnVerDouble < 8)
			{
				static auto UpdateQuickBars = Controller->Function(_("UpdateQuickBars"));
				if (UpdateQuickBars)
					Controller->ProcessEvent(UpdateQuickBars);
			}
		}

		// static auto OnRep_QuickBar = Controller->Function(_("OnRep_QuickBar"));
		// Controller->ProcessEvent(OnRep_QuickBar, nullptr);

		if ((bRemovedItem || Idx != -1) && Inventory)
			((FFastArraySerializer*)Inventory)->MarkArrayDirty();

		// if (Idx != -1)
		if (ModifiedItem)
			((FFastArraySerializer*)Inventory)->MarkItemDirty(ModifiedItem);
	}

	UObject* CreateItemInstance(UObject* Controller, UObject* Definition, int Count = 1)
	{
		if (!Definition || !Controller)
			return nullptr;

		struct {
			int count;
			int level;
			UObject* instance;
		} Params{ Count, 1 };

		static auto CTIIFn = Definition->Function(_("CreateTemporaryItemInstanceBP"));

		Definition->ProcessEvent(CTIIFn, &Params);

		UObject* itemInstance = Params.instance;

		if (itemInstance)
		{
			static UObject* SOCFTIFn = itemInstance->Function(_("SetOwningControllerForTemporaryItem"));

			itemInstance->ProcessEvent(SOCFTIFn, &Controller);

			static auto CountOffset = FindOffsetStruct(_("ScriptStruct /Script/FortniteGame.FortItemEntry"), _("Count"));

			auto GunCount = (int*)(__int64(&*itemInstance->Member<__int64>(_("ItemEntry"))) + CountOffset);
			*GunCount = Count;

			auto OwnerInventory = itemInstance->Member<UObject*>(_("OwnerInventory")); // We should probably set this?
			*OwnerInventory = *Controller->Member<UObject*>(_("WorldInventory"));

			return itemInstance;
		}

		return nullptr;
	}

	template <typename EntryStruct>
	int AddToReplicatedEntries(UObject* Controller, UObject* FortItem)
	{
		if (!Controller || !FortItem)
			return -1;

		auto ItemEntry = FortItem->Member<EntryStruct>(_("ItemEntry"));

		// std::cout << _("ItemEntryStruct Size: ") << GetEntrySize() << '\n';

		if (ItemEntry)
			return GetReplicatedEntries<EntryStruct>(Controller)->Add(*ItemEntry); // GetEntrySize());

		return -1;
	}

	template <typename EntryStruct, typename Type>
	bool ChangeItemInReplicatedEntriesWithEntries(UObject* Controller, UObject* Definition, const std::string& Name, Type NewVal)
	{
		if (!Controller || !Definition)
			return false;

		auto ReplicatedEntries = GetReplicatedEntries<EntryStruct>(Controller);
		bool bSuccessful = false;

		for (int x = 0; x < ReplicatedEntries->Num(); x++)
		{
			auto& ItemEntry = ReplicatedEntries->At(x);

			if (FFortItemEntry::GetItemDefinition((__int64*)&ItemEntry) == Definition)
			{
				static auto Offset = FindOffsetStruct(_("ScriptStruct /Script/FortniteGame.FortItemEntry"), Name);
				*(Type*)(__int64(&ItemEntry) + Offset) = NewVal;
				bSuccessful = true;
				auto Inventory = GetInventory(Controller);
				((FFastArraySerializer*)Inventory)->MarkItemDirty((FFastArraySerializerItem*)&ItemEntry);
				// break;
			}
		}

		return bSuccessful;
	}

	template <typename Type>
	bool ChangeItemInReplicatedEntries(UObject* Controller, UObject* Definition, const std::string& Name, Type NewVal)
	{
		static const auto FlooredVer = std::floor(std::stod(FN_Version));

		if (FlooredVer == 3)
		{
			struct ItemEntrySize { unsigned char Unk00[0xC0]; };
			return ChangeItemInReplicatedEntriesWithEntries<ItemEntrySize, int>(Controller, Definition, Name, NewVal);
		}
		else if (FlooredVer > 3 && std::stod(FN_Version) < 7.40)
		{
			struct ItemEntrySize { unsigned char Unk00[0xD0]; };
			ChangeItemInReplicatedEntriesWithEntries<ItemEntrySize, int>(Controller, Definition, Name, NewVal);
		}
		else if (std::stod(FN_Version) >= 7.40 && Engine_Version < 424) // not right idc
		{
			struct ItemEntrySize { unsigned char Unk00[0x120]; };
			ChangeItemInReplicatedEntriesWithEntries<ItemEntrySize, int>(Controller, Definition, Name, NewVal);
		}
		else if (std::stod(FN_Version) >= 424)
		{
			struct ItemEntrySize { unsigned char Unk00[0x150]; };
			return ChangeItemInReplicatedEntriesWithEntries<ItemEntrySize, int>(Controller, Definition, Name, NewVal);
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
			static auto GuidOffset = FindOffsetStruct(_("ScriptStruct /Script/FortniteGame.FortItemEntry"), _("ItemGuid"));
			auto& ItemEntry = ReplicatedEntries->At(x);

			if (*(FGuid*)(__int64(&ItemEntry) + GuidOffset) == Guid)
			{
				ReplicatedEntries->RemoveAt(x);
				bSuccessful = true;
				// break;
			}
		}

		if (!bSuccessful)
			std::cout << _("Failed to find ItemGuid in Inventory!\n");

		return bSuccessful;
	}

	static void AddItem(UObject* Controller, UObject* FortItem, EFortQuickBars Bars, int Slot, int Count = 1)
	{
		if (!Controller || !FortItem)
			return;

		static const auto FlooredVer = std::floor(std::stod(FN_Version));

		// auto Inventory = GetInventory(Controller);

		*FFortItemEntry::GetCount(FortItem->Member<__int64>(_("ItemEntry"))) = Count;

		GetItemInstances(Controller)->Add(FortItem);

		static const auto FnVerDouble = std::stod(FN_Version);

		if (7.4 > FnVerDouble)
		{
			auto QuickBars = Controller->Member<UObject*>(_("QuickBars"));

			if (QuickBars && *QuickBars)
			{
				static UObject* GetItemGuidFn = FindObject(_("Function /Script/FortniteGame.FortItem.GetItemGuid"));
				FGuid Guid;

				FortItem->ProcessEvent(GetItemGuidFn, &Guid);

				struct
				{
					FGuid Item;
					EFortQuickBars Quickbar;
					int Slot;
				} SAIIParams{ Guid, Bars, Slot };

				static auto SAIIFn = (*QuickBars)->Function(_("ServerAddItemInternal"));

				if (SAIIFn)
					(*QuickBars)->ProcessEvent(SAIIFn, &SAIIParams);
			}
		}

		if (FlooredVer == 3)
		{
			struct ItemEntrySize { unsigned char Unk00[0xC0]; };
			AddToReplicatedEntries<ItemEntrySize>(Controller, FortItem);
		}
		else if (std::stof(FN_Version) < 7.40f)
		{
			struct ItemEntrySize { unsigned char Unk00[0xD0]; };
			AddToReplicatedEntries<ItemEntrySize>(Controller, FortItem);
		}
		else if (std::stod(FN_Version) >= 7.40 && Engine_Version < 424) // not right idc
		{
			struct ItemEntrySize { unsigned char Unk00[0x120]; };
			AddToReplicatedEntries<ItemEntrySize>(Controller, FortItem);
		}
		else if (std::stod(FN_Version) >= 424)
		{
			struct ItemEntrySize { unsigned char Unk00[0x150]; };
			AddToReplicatedEntries<ItemEntrySize>(Controller, FortItem);
		}
		else
			std::cout << _("Could not get ItemEntrySize Struct! Please make a new one for this version using: ") << GetEntrySize() << '\n';

		Inventory::Update(Controller, -1, true);
	}

	static void CreateAndAddItem(UObject* Controller, UObject* Definition, EFortQuickBars Bars, int Slot, int Count = 1)
	{
		if (Controller && Definition)
		{
			auto Instance = CreateItemInstance(Controller, Definition, Count);

			if (Instance)
				AddItem(Controller, Instance, Bars, Slot, Count);
			else
				std::cout << _("Failed to create ItemInstance!\n");
		}
	}

	// Returns a item instance
	UObject* FindItemInInventory(UObject* Controller, UObject* Definition) // TODO: Return a vector.
	{
		auto ItemInstances = GetItemInstances(Controller);
		auto Pawn = *Controller->Member<UObject*>(_("Pawn"));

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

	UObject* RemoveItem(UObject* Controller, const FGuid& Guid, int Count = 1)
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
				ItemInstances->RemoveAt(j);
				if (!ItemDefinition)
					ItemDefinition = GetItemDefinition(ItemInstance);
				// break;
			}
		}

		static const auto FlooredVer = std::floor(std::stod(FN_Version));

		if (FlooredVer == 3)
		{
			struct ItemEntrySize { unsigned char Unk00[0xC0]; };
			RemoveGuidFromReplicatedEntries<ItemEntrySize>(Controller, Guid);
		}
		else if (FlooredVer > 3 && std::stod(FN_Version) < 7.40)
		{
			struct ItemEntrySize { unsigned char Unk00[0xD0]; };
			RemoveGuidFromReplicatedEntries<ItemEntrySize>(Controller, Guid);
		}
		else if (std::stod(FN_Version) >= 7.40 && Engine_Version < 424) // not right idc
		{
			struct ItemEntrySize { unsigned char Unk00[0x120]; };
			RemoveGuidFromReplicatedEntries<ItemEntrySize>(Controller, Guid);
		}
		else if (std::stod(FN_Version) >= 424)
		{
			struct ItemEntrySize { unsigned char Unk00[0x150]; };
			RemoveGuidFromReplicatedEntries<ItemEntrySize>(Controller, Guid);
		}

		Update(Controller, -1, true);

		return ItemDefinition;
	}

	bool IncreaseItemCount(UObject* Controller, UObject* Definition, int Count) // stack
	{
		// For now, assume it's a consumable or ammo, and it can go higher than what we currently have.

		if (Controller && Definition)
		{
			auto ItemInstances = GetItemInstances(Controller);
			for (int j = 0; j < ItemInstances->Num(); j++)
			{
				auto ItemInstance = ItemInstances->At(j);

				if (!ItemInstance)
					continue;

				auto CurrentDefinition = GetItemDefinition(ItemInstance);

				if (CurrentDefinition == Definition)
				{
					auto ItemEntry = ItemInstance->Member<__int64>(_("ItemEntry")); // Keep as pointer!
					auto CurrentCount = FFortItemEntry::GetCount(ItemEntry);
					
					std::cout << std::format("Item going to stack on count: {} Picking up item count: {}", *CurrentCount, Count) << '\n';
					auto NewCount = *CurrentCount + Count;
					*CurrentCount = NewCount;

					ChangeItemInReplicatedEntries<int>(Controller, Definition, _("Count"), NewCount);

					Update(Controller, -1, false, (FFastArraySerializerItem*)ItemEntry);

					return true;
				}
			}
		}

		return false;
	}

	bool DecreaseItemCount(UObject* Controller, UObject* Definition, int Count, __int64** OutItemEntry = nullptr)
	{
		if (Controller && Definition)
		{
			auto ItemInstances = GetItemInstances(Controller);

			for (int j = 0; j < ItemInstances->Num(); j++)
			{
				auto ItemInstance = ItemInstances->At(j);

				if (!ItemInstance)
					continue;

				auto CurrentDefinition = GetItemDefinition(ItemInstance);
				if (CurrentDefinition == Definition)
				{
					auto ItemEntry = ItemInstance->Member<__int64>(_("ItemEntry")); // Keep as pointer!
					auto CurrentCount = FFortItemEntry::GetCount(ItemEntry);

					// std::cout << std::format("Item going to stack on count: {} Picking up item count: {}", *CurrentCount, Count) << '\n';
					auto NewCount = *CurrentCount - Count;
					*CurrentCount = NewCount;

					ChangeItemInReplicatedEntries<int>(Controller, Definition, _("Count"), NewCount);

					Update(Controller, -1, false, (FFastArraySerializerItem*)ItemEntry);

					if (OutItemEntry)
						*OutItemEntry = ItemEntry;

					return true;
				}
			}
		}

		return false;
	}

	void GiveAllAmmo(UObject* Controller)
	{
		// omfg

		static const auto FnVerDouble = std::stod(FN_Version);

		if (Engine_Version < 422)
		{
			static UObject* AthenaAmmoDataRockets = FindObject(_("FortAmmoItemDefinition /Game/Athena/Items/Ammo/AthenaAmmoDataRockets.AthenaAmmoDataRockets"));
			static UObject* AthenaAmmoDataShells = FindObject(_("FortAmmoItemDefinition /Game/Items/Ammo/AthenaAmmoDataShells.AthenaAmmoDataShells"));
			static UObject* AthenaAmmoDataBulletsMedium = FindObject(_("FortAmmoItemDefinition /Game/Items/Ammo/AthenaAmmoDataBulletsMedium.AthenaAmmoDataBulletsMedium"));
			static UObject* AthenaAmmoDataBulletsLight = FindObject(_("FortAmmoItemDefinition /Game/Items/Ammo/AthenaAmmoDataBulletsLight.AthenaAmmoDataBulletsLight"));
			static UObject* AthenaAmmoDataBulletsHeavy = FindObject(_("FortAmmoItemDefinition /Game/Items/Ammo/AthenaAmmoDataBulletsHeavy.AthenaAmmoDataBulletsHeavy"));

			CreateAndAddItem(Controller, AthenaAmmoDataRockets, EFortQuickBars::Secondary, 0, 999);
			CreateAndAddItem(Controller, AthenaAmmoDataShells, EFortQuickBars::Secondary, 0, 999);
			CreateAndAddItem(Controller, AthenaAmmoDataBulletsMedium, EFortQuickBars::Secondary, 0, 999);
			CreateAndAddItem(Controller, AthenaAmmoDataBulletsLight, EFortQuickBars::Secondary, 0, 999);
			CreateAndAddItem(Controller, AthenaAmmoDataBulletsHeavy, EFortQuickBars::Secondary, 0, 999);
		}
		else
		{
			static UObject* AthenaAmmoDataRockets = FindObject(_("FortAmmoItemDefinition /Game/Athena/Items/Ammo/AmmoDataRockets.AmmoDataRockets"));
			static UObject* AthenaAmmoDataShells = FindObject(_("FortAmmoItemDefinition /Game/Athena/Items/Ammo/AthenaAmmoDataShells.AthenaAmmoDataShells"));
			static UObject* AthenaAmmoDataBulletsMedium = FindObject(_("FortAmmoItemDefinition /Game/Athena/Items/Ammo/AthenaAmmoDataBulletsMedium.AthenaAmmoDataBulletsMedium"));
			static UObject* AthenaAmmoDataBulletsLight = FindObject(_("FortAmmoItemDefinition /Game/Athena/Items/Ammo/AthenaAmmoDataBulletsLight.AthenaAmmoDataBulletsLight"));
			static UObject* AthenaAmmoDataBulletsHeavy = FindObject(_("FortAmmoItemDefinition /Game/Athena/Items/Ammo/AthenaAmmoDataBulletsHeavy.AthenaAmmoDataBulletsHeavy"));

			CreateAndAddItem(Controller, AthenaAmmoDataRockets, EFortQuickBars::Secondary, 0, 999);
			CreateAndAddItem(Controller, AthenaAmmoDataShells, EFortQuickBars::Secondary, 0, 999);
			CreateAndAddItem(Controller, AthenaAmmoDataBulletsMedium, EFortQuickBars::Secondary, 0, 999);
			CreateAndAddItem(Controller, AthenaAmmoDataBulletsLight, EFortQuickBars::Secondary, 0, 999);
			CreateAndAddItem(Controller, AthenaAmmoDataBulletsHeavy, EFortQuickBars::Secondary, 0, 999);
		}
	}

	void GiveMats(UObject* Controller)
	{
		static auto WoodItemData = FindObject(_("FortResourceItemDefinition /Game/Items/ResourcePickups/WoodItemData.WoodItemData"));
		static auto StoneItemData = FindObject(_("FortResourceItemDefinition /Game/Items/ResourcePickups/StoneItemData.StoneItemData"));
		static auto MetalItemData = FindObject(_("FortResourceItemDefinition /Game/Items/ResourcePickups/MetalItemData.MetalItemData"));

		CreateAndAddItem(Controller, WoodItemData, EFortQuickBars::Secondary, 0, 999);
		CreateAndAddItem(Controller, StoneItemData, EFortQuickBars::Secondary, 0, 999);
		CreateAndAddItem(Controller, MetalItemData, EFortQuickBars::Secondary, 0, 999);
	}

	void GiveBuildings(UObject* Controller)
	{
		static auto BuildingItemData_Wall = FindObject(_("FortBuildingItemDefinition /Game/Items/Weapons/BuildingTools/BuildingItemData_Wall.BuildingItemData_Wall"));
		static auto BuildingItemData_Floor = FindObject(_("FortBuildingItemDefinition /Game/Items/Weapons/BuildingTools/BuildingItemData_Floor.BuildingItemData_Floor"));
		static auto BuildingItemData_Stair_W = FindObject(_("FortBuildingItemDefinition /Game/Items/Weapons/BuildingTools/BuildingItemData_Stair_W.BuildingItemData_Stair_W"));
		static auto BuildingItemData_RoofS = FindObject(_("FortBuildingItemDefinition /Game/Items/Weapons/BuildingTools/BuildingItemData_RoofS.BuildingItemData_RoofS"));

		CreateAndAddItem(Controller, BuildingItemData_Wall, EFortQuickBars::Secondary, 0, 1);
		CreateAndAddItem(Controller, BuildingItemData_Floor, EFortQuickBars::Secondary, 1, 1);
		CreateAndAddItem(Controller, BuildingItemData_Stair_W, EFortQuickBars::Secondary, 2, 1);
		CreateAndAddItem(Controller, BuildingItemData_RoofS, EFortQuickBars::Secondary, 3, 1);
	}

	void GiveStartingItems(UObject* Controller)
	{
		static auto EditTool = FindObject(_("FortEditToolItemDefinition /Game/Items/Weapons/BuildingTools/EditTool.EditTool"));

		CreateAndAddItem(Controller, EditTool, EFortQuickBars::Primary, 0, 1);
		GiveBuildings(Controller);
	}
}

inline bool ServerExecuteInventoryItemHook(UObject* Controller, UFunction* Function, void* Parameters)
{
	Inventory::EquipInventoryItem(Controller, *(FGuid*)Parameters);
	return false;
}

inline bool ServerExecuteInventoryWeaponHook(UObject* Controller, UFunction* Function, void* Parameters)
{
	auto Weapon = (UObject**)Parameters;

	if (Weapon && *Weapon)
	{
		auto Pawn = *Controller->Member<UObject*>(_("Pawn"));
		auto Guid = *(*Weapon)->Member<FGuid>(_("ItemEntryGuid"));
		auto Def = *(*Weapon)->Member<UObject*>(_("WeaponData"));

		int Ammo = 0; // TODO: implmeent

		// Inventory::EquipInventoryItem(Controller, Guid); // "hacky" // this doesjnt work maybe
		
		if (Def)
			Inventory::EquipWeaponDefinition(Pawn, Def, Guid, Ammo); // scuffed
		else
			std::cout << _("No Def!\n");

		// Inventory::EquipWeapon, *Weapon, Guid);
	}
	else
		std::cout << _("No weapon?\n");

	return false;
}

inline bool ServerAttemptInventoryDropHook(UObject* Controller, UFunction* Function, void* Parameters)
{
	if (Controller && Parameters && !Helper::IsInAircraft(Controller))
	{
		struct AFortPlayerController_ServerAttemptInventoryDrop_Params
		{
			FGuid                                       ItemGuid;                                                 // (Parm, ZeroConstructor, IsPlainOldData)
			int                                                Count;                                                    // (Parm, ZeroConstructor, IsPlainOldData)
		};

		auto Params = (AFortPlayerController_ServerAttemptInventoryDrop_Params*)Parameters;

		auto Definition = Inventory::RemoveItem(Controller, Params->ItemGuid, Params->Count);
		auto Pawn = Controller->Member<UObject*>(_("Pawn"));

		if (Pawn && *Pawn)
		{
			auto loc = Helper::GetActorLocation(*Pawn);

			auto Pickup = Helper::SummonPickup(*Pawn, Definition, loc, EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::Unset, Params->Count);
		}
	}

	return false;
}

inline bool ServerHandlePickupHook(UObject* Pawn, UFunction* Function, void* Parameters)
{
	if (Pawn && Parameters)
	{
		struct AFortPlayerPawn_ServerHandlePickup_Params
		{
			UObject* Pickup;                                                   // (Parm, ZeroConstructor, IsPlainOldData)
			float                                              InFlyTime;                                                // (Parm, ZeroConstructor, IsPlainOldData)
			FVector                                     InStartDirection;                                         // (ConstParm, Parm, ZeroConstructor, IsPlainOldData)
			bool                                               bPlayPickupSound;                                         // (Parm, ZeroConstructor, IsPlainOldData)
		};

		auto Params = (AFortPlayerPawn_ServerHandlePickup_Params*)Parameters;

		if (Params->Pickup)
		{
			bool* bPickedUp = Params->Pickup->Member<bool>(_("bPickedUp"));

			if (bPickedUp && !*bPickedUp)
			{

				auto PrimaryPickupItemEntry = Params->Pickup->Member<__int64>(_("PrimaryPickupItemEntry"));
				static auto ItemDefinitionOffset = FindOffsetStruct(_("ScriptStruct /Script/FortniteGame.FortItemEntry"), _("ItemDefinition"));
				static auto CountOffset = FindOffsetStruct(_("ScriptStruct /Script/FortniteGame.FortItemEntry"), _("Count"));
				auto Controller = Pawn->Member<UObject*>(_("Controller"));

				// auto SwappinDef = *(*Controller)->Member<UObject*>(_("SwappingItemDefinition")); // This is the same tihng as the pickup

				auto Definition = (UObject**)(__int64(&*PrimaryPickupItemEntry) + ItemDefinitionOffset);
				auto Count = (int*)(__int64(&*PrimaryPickupItemEntry) + CountOffset);

				if (Controller && *Controller)
				{
					if (Definition && *Definition && Count)
					{
						static UObject* EffectClass = FindObject("BlueprintGeneratedClass /Game/Effects/Fort_Effects/Gameplay/Pickups/B_Pickups_Default.B_Pickups_Default_C");

						if (EffectClass)
						{
							UObject* Effect = Easy::SpawnActor(EffectClass, Helper::GetActorLocation(Pawn));

							if (Effect)
							{
								*Effect->Member<UObject*>(_("ItemDefinition")) = *Definition;

								auto PEBP = Params->Pickup->Member<TWeakObjectPtr<UObject>>("PickupEffectBlueprint");
								PEBP->ObjectIndex = Effect->InternalIndex;
								PEBP->ObjectSerialNumber = GetSerialNumber(Effect);

								// PEBP = Effect;

								static auto OnPickup = Effect->Function(_("OnPickup"));
								// static auto OnPickup = Effect->Function(_("OnPickedUp"));

								/*

								void OnTossed();
								void OnPickedUp();

								*/

								if (OnPickup)
									Effect->ProcessEvent(OnPickup);
								else
									std::cout << _("Failed to find OnPickup!\n");
							}
							else
								std::cout << _("Failed to spawn effect!\n");
						}
						else
							std::cout << _("Could not find Effectclass!\n");

						auto bWasAbleToStack = Inventory::IncreaseItemCount(*Controller, *Definition, *Count);
						if (!bWasAbleToStack)
							Inventory::CreateAndAddItem(*Controller, *Definition, EFortQuickBars::Primary, *Count); // TODO: Figure out what quickbars are supposed to be used.
					}
					else
						std::cout << _("Player is trying to pickup an item with a null Definition or null Count!\n");
				}
				else
					std::cout << _("Invalid Controller: ") << Controller << '\n';

				Helper::DestroyActor(Params->Pickup);
				*bPickedUp = true;
				static auto bPickedUpFn = Params->Pickup->Function(_("OnRep_bPickedUp"));
				Params->Pickup->ProcessEvent(bPickedUpFn);
			}
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
		bool* bPickedUp = Params->Pickup->Member<bool>(_("bPickedUp"));

		if (bPickedUp && !*bPickedUp)
		{
			auto Controller = Pawn->Member<UObject*>(_("Controller"));

			auto PrimaryPickupItemEntry = Params->Pickup->Member<__int64>(_("PrimaryPickupItemEntry"));
			static auto ItemDefinitionOffset = FindOffsetStruct(_("ScriptStruct /Script/FortniteGame.FortItemEntry"), _("ItemDefinition"));
			static auto CountOffset = FindOffsetStruct(_("ScriptStruct /Script/FortniteGame.FortItemEntry"), _("Count"));

			auto Definition = (UObject**)(__int64(&*PrimaryPickupItemEntry) + ItemDefinitionOffset);
			auto Count = (int*)(__int64(&*PrimaryPickupItemEntry) + CountOffset);

			struct
			{
				FGuid                                       ItemGuid;                                                 // (Parm, ZeroConstructor, IsPlainOldData)
				int                                                Count;                                                    // (Parm, ZeroConstructor, IsPlainOldData)
			} dropParams{ Params->Swap, 1 }; // (*Pawn->Member<UObject*>(_("CurrentWeapon"))};

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

inline bool ServerSetShouldSwapPickupHook(UObject* Controller, UFunction* Function, void* Parameters)
{
	struct parms { bool ba; };

	auto Params = (parms*)Parameters;

	if (Controller && Params)
	{
		bool bShouldSwap = Params->ba;
		std::cout << _("Should Swap: ") << bShouldSwap << '\n';
	}

	return false;
}

template <typename EntryStruct>
void idekatthispoint(UObject* PlayerController, UObject* Def, int AmountToRemove)
{
	auto& ReplicatedEntries = Inventory::GetReplicatedEntries<EntryStruct>(PlayerController);
	for (int i = 0; i < ReplicatedEntries->Num(); i++)
	{
		auto& CurrentEntry = ReplicatedEntries->At(i);
		if (FFortItemEntry::GetItemDefinition((__int64*)(&CurrentEntry)) == Def)
		{
			*FFortItemEntry::GetCount(CurrentEntry) -= AmountToRemove;
			Inventory::Update(PlayerController, false, CurrentEntry);
		}
	}
}

void __fastcall HandleReloadCostDetour(UObject* Weapon, int AmountToRemove) // nova go brr
{
	if (!Weapon)
		return;

	static auto GetOwner = Weapon->Function(_("GetOwner"));
	UObject* Pawn;
	Weapon->ProcessEvent(GetOwner, &Pawn);

	if (!Pawn)
		return;

	auto PlayerController = *Pawn->Member<UObject*>(_("Controller"));

	if (PlayerController)
	{
		auto WeaponData = *Weapon->Member<UObject*>(_("WeaponData"));
		static auto GetAmmoWorldItemDefinition_BP = WeaponData->Function(_("GetAmmoWorldItemDefinition_BP"));
		UObject* AmmoDef;
		WeaponData->ProcessEvent(GetAmmoWorldItemDefinition_BP, &AmmoDef);

		if (!AmmoDef || WeaponData->GetName().contains("TID"))
			AmmoDef = WeaponData;

		__int64* ItemEntry = nullptr;

		Inventory::DecreaseItemCount(PlayerController, AmmoDef, AmountToRemove, &ItemEntry);

		if (ItemEntry)
		{
			if (*FFortItemEntry::GetCount(ItemEntry) <= 0) // Destroy the item if it has no count
			{
				Inventory::RemoveItem(PlayerController, *FFortItemEntry::GetGuid(ItemEntry));
			}
		}

		/* static const auto FlooredVer = std::floor(std::stod(FN_Version));

		if (FlooredVer == 3)
		{
			struct ItemEntrySize { unsigned char Unk00[0xC0]; };
			return idekatthispoint<ItemEntrySize>(PlayerController, AmountToRemove);
		}
		else if (FlooredVer > 4 && std::stod(FN_Version) < 7.40)
		{
			struct ItemEntrySize { unsigned char Unk00[0xD0]; };
			return idekatthispoint<ItemEntrySize>(PlayerController, AmountToRemove);
		}
		else if (std::stod(FN_Version) >= 7.40 && Engine_Version < 424) // not right idc
		{
			struct ItemEntrySize { unsigned char Unk00[0x120]; };
			return idekatthispoint<ItemEntrySize>(PlayerController, AmountToRemove);
		}
		else if (std::stod(FN_Version) >= 424)
		{
			struct ItemEntrySize { unsigned char Unk00[0x150]; };
			return idekatthispoint<ItemEntrySize>(PlayerController, AmountToRemove);
		}

		auto ItemInstances = Inventory::GetItemInstances(PlayerController);

		for (int j = 0; j < ItemInstances->Num(); j++)
		{
			auto ItemInstance = ItemInstances->At(j);

			if (ItemInstance)
			{
				static auto GetItemDefinitionBP = ItemInstance->Function(_("GetItemDefinitionBP"));
				UObject* CurrentAmmoDef = nullptr;
				ItemInstance->ProcessEvent(GetItemDefinitionBP, &CurrentAmmoDef);

				if (ItemInstance && CurrentAmmoDef == AmmoDef)
				{
					auto ItemEntry = ItemInstance->Member<__int64>(_("ItemEntry")); // Keep as pointer!
					auto CurrentCount = FFortItemEntry::GetCount(ItemEntry);

					// std::cout << std::format("Item going to stack on count: {} Picking up item count: {}", *CurrentCount, Count) << '\n';
					auto NewCount = *CurrentCount - Count;
					*CurrentCount = NewCount;

					ChangeItemInReplicatedEntries<int>(Controller, Definition, _("Count"), NewCount);

					Update(Controller, -1, false, (FFastArraySerializerItem*)ItemEntry);
				}
					ItemInstances->RemoveAt(j);
			}
		}
		*/

		return HandleReloadCost(Weapon, AmountToRemove);
	}
}

void InitializeInventoryHooks()
{
	AddHook(_("Function /Script/FortniteGame.FortPlayerController.ServerExecuteInventoryItem"), ServerExecuteInventoryItemHook);

	if (Engine_Version >= 423)
		AddHook(_("Function /Script/FortniteGame.FortPlayerController.ServerExecuteInventoryWeapon"), ServerExecuteInventoryWeaponHook);

	AddHook(_("Function /Script/FortniteGame.FortPlayerController.ServerAttemptInventoryDrop"), ServerAttemptInventoryDropHook);
	AddHook(_("Function /Script/FortniteGame.FortPlayerPawn.ServerHandlePickup"), ServerHandlePickupHook);

	if (std::stod(FN_Version) >= 7.40)
	{
		AddHook(_("Function /Script/FortniteGame.FortPlayerPawn.ServerHandlePickupWithSwap"), ServerHandlePickupWithSwapHook);
		AddHook(_("Function /Script/FortniteGame.FortPlayerControllerAthena.ServerSetShouldSwapPickup"), ServerSetShouldSwapPickupHook)
	}

	if (HandleReloadCost)
	{
		MH_CreateHook((PVOID)HandleReloadCostAddr, HandleReloadCostDetour, (void**)&HandleReloadCost);
		MH_EnableHook((PVOID)HandleReloadCostAddr);
	}
}