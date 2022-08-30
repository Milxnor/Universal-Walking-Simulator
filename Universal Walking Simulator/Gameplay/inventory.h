#pragma once
#include <Gameplay/carmine.h>

#include <UE/structs.h>
#include <Gameplay/helper.h>
#include <Gameplay/abilities.h>

/*

GiveItem - Creates a new item and or stacks
TakeItem - Decreases and or removes if requested

*/

constexpr bool newSwapping = false; // there is this bug where its not actuallythe correct weapon on the client

static int GetEntrySize()
{
	static auto FortItemEntryClass = FindObject(("ScriptStruct /Script/FortniteGame.FortItemEntry"), true);

	return GetSizeOfStruct(FortItemEntryClass);
}

template <typename EntryType = __int64>
inline EntryType* GetItemEntryFromInstance(UObject* Instance)
{
	static auto ItemEntryOffset = GetOffset(Instance, "ItemEntry");
	return (EntryType*)(__int64(Instance) + ItemEntryOffset);
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

	FGuid* GetGuid(__int64* Entry)
	{
		static auto GuidOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortItemEntry"), ("ItemGuid"));
		auto Guid = (FGuid*)(__int64(&*Entry) + GuidOffset);

		return Guid;
	}

	UObject* GetItemDefinition(__int64* Entry)
	{
		if (!Entry)
			return nullptr;

		static auto ItemDefinitionOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortItemEntry"), ("ItemDefinition"));
		auto Definition = (UObject**)(__int64(&*Entry) + ItemDefinitionOffset);

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
			} GetItemInQuickbarSlot_params{ Bars, Slot };

			if (GetItemInQuickbarSlotfn)
				Controller->ProcessEvent(GetItemInQuickbarSlotfn, &GetItemInQuickbarSlot_params);

			return GetItemInQuickbarSlot_params.Ret;
		}

		return nullptr;
	}

	bool IsHoldingPickaxe()
	{

	}

	EFortQuickBars WhatQuickBars(UObject* Definition) // returns the quickbar the item should go in
	{
		static auto FortWeaponItemDefinitionClass = FindObject(("Class /Script/FortniteGame.FortWeaponItemDefinition"));

		if (Definition->IsA(FortWeaponItemDefinitionClass))
			return EFortQuickBars::Primary;
		else
			return EFortQuickBars::Secondary;
	}
}

static UObject* GetPickaxeDef(UObject* Controller)
{
	static auto PickaxeDef = FindObject(("FortWeaponMeleeItemDefinition /Game/Athena/Items/Weapons/WID_Harvest_Pickaxe_Athena_C_T01.WID_Harvest_Pickaxe_Athena_C_T01"));
	return PickaxeDef;
}

namespace Inventory
{
	UObject* GetWorldInventory(UObject* Controller)
	{
		return *Controller->Member<UObject*>(("WorldInventory")); // WorldInventory;
	}

	__int64* GetInventory(UObject* Controller)
	{
		auto WorldInventory = GetWorldInventory(Controller);

		if (WorldInventory)
		{
			auto Inventorya = WorldInventory->Member<__int64>(("Inventory"));

			return Inventorya;
		}

		return nullptr;
	}

	TArray<UObject*>* GetItemInstances(UObject* Controller)
	{
		static __int64 ItemInstancesOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortItemList"), ("ItemInstances"));

		auto Inventory = GetInventory(Controller);

		if (Inventory)
			return (TArray<UObject*>*)(__int64(Inventory) + ItemInstancesOffset);
		else
			return nullptr;
	}

	inline UObject* EquipWeapon(UObject* Pawn, UObject* FortWeapon, const FGuid& Guid, int Ammo = 0)
	{
		static auto PickaxeDef = FindObject(("FortWeaponMeleeItemDefinition /Game/Athena/Items/Weapons/WID_Harvest_Pickaxe_Athena_C_T01.WID_Harvest_Pickaxe_Athena_C_T01"));
		auto CurrentWeapon = *Pawn->Member<UObject*>(("CurrentWeapon"));
		if (FortWeapon && Pawn)
		{
			static auto OnRep_ReplicatedWeaponData = FortWeapon->Function("OnRep_ReplicatedWeaponData");

			if (OnRep_ReplicatedWeaponData)
				FortWeapon->ProcessEvent(OnRep_ReplicatedWeaponData);

			// the function below starts taking a param
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
	}

	inline UObject* EquipWeaponDefinition(UObject* Pawn, UObject* Definition, const FGuid& Guid, int Ammo = 0, const FGuid& TrackerGuid = FGuid())
	{
		if (Pawn && Definition)
		{
			auto FullName = Definition->GetFullName();
			bool IsAGID = (FullName.contains(("AthenaGadgetItemDefinition ")) || FullName.contains(("FortGadgetItemDefinition "))) ? true : false; // TODO: Use IsA

			if (!IsAGID)
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

					std::cout << "Ammo: " << Ammo << '\n';

					static auto OnRep_ReplicatedWeaponData = ("OnRep_ReplicatedWeaponData");

					if (OnRep_ReplicatedWeaponData)
						Weapon->ProcessEvent(OnRep_ReplicatedWeaponData);
				}
				else
					std::cout << ("Failed to spawn Weapon!\n");

				return Weapon;
			}
			else
			{
				static auto GetItemDefinition = Definition->Function(("GetWeaponItemDefinition"));

				UObject* Def = nullptr;

				if (!GetItemDefinition) {
					GetItemDefinition = Definition->Function(("GetDecoItemDefinition"));
				}
				Definition->ProcessEvent(GetItemDefinition, &Def);

				if (Def)
				{
					Inventory::EquipWeaponDefinition(Pawn, Def, Guid);
				}
				else
					std::cout << ("Failed to get AGID's Definition!\n");
			}
		}

		return nullptr;
	}

	FGuid GetItemGuid(UObject* ItemInstance)
	{
		static UObject* GetItemGuidFn = FindObject(("Function /Script/FortniteGame.FortItem.GetItemGuid"));
		FGuid Guid;

		if (ItemInstance && GetItemGuidFn)
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
		}parms{ bars };
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

		auto currentWeapon = *Pawn->Member<UObject*>("CurrentWeapon");
		static auto PickaxeDef = FindObject(("FortWeaponMeleeItemDefinition /Game/Athena/Items/Weapons/WID_Harvest_Pickaxe_Athena_C_T01.WID_Harvest_Pickaxe_Athena_C_T01"));

		if (currentWeapon && *currentWeapon->Member<UObject*>("WeaponData") != PickaxeDef)
		{
			auto currentInstance = GetItemInstanceFromGuid(Controller, *currentWeapon->Member<FGuid>("ItemEntryGuid"));

			if (currentInstance)
			{
				auto currentItemEntry = GetItemEntryFromInstance(currentInstance);

				if (currentItemEntry)
				{
					int currentAmmoCount;; // = *currentWeapon->Member<int>("AmmoCount");
					static auto GetMagazineAmmoCount = currentWeapon->Function("GetMagazineAmmoCount");

					if (GetMagazineAmmoCount)
						currentWeapon->ProcessEvent(GetMagazineAmmoCount, &currentAmmoCount);

					std::cout << "AmmoCount: " << currentAmmoCount << '\n';

					auto LoadedAmmo = FFortItemEntry::GetLoadedAmmo(currentItemEntry);
					std::cout << "LoadedAmmo: " << *LoadedAmmo << '\n';
				}
				else
					std::cout << "No ItemEntry!\n";
			}
			else
				std::cout << "No ItemInstance!\n";
		}
		else
			std::cout << "Brudaa!\n";

		std::cout << "currentWeapon ammo: " << currentWeaponAmmo << '\n';

		EquipWeaponDefinition(Pawn, Def, Guid, currentWeaponAmmo/* *FFortItemEntry::GetLoadedAmmo(GetItemEntryFromInstance(CurrentItemInstance)) */, TrackerGuid);

		return nullptr;
	}

	template <typename EntryStruct>
	TArray<EntryStruct>* GetReplicatedEntries(UObject* Controller)
	{
		static __int64 ReplicatedEntriesOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortItemList"), ("ReplicatedEntries"));

		auto Inventory = GetInventory(Controller);

		return (TArray<EntryStruct>*)(__int64(Inventory) + ReplicatedEntriesOffset);
	}

	void Update(UObject* Controller, int Idx = -1, bool bRemovedItem = false, FFastArraySerializerItem* ModifiedItem = nullptr)
	{
		auto WorldInventory = GetWorldInventory(Controller);
		auto Inventory = GetInventory(Controller);

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
			const auto QuickBars = *Controller->Member<UObject*>(("QuickBars"));

			if (QuickBars)
			{
				static auto OnRep_PrimaryQuickBar = QuickBars->Function(("OnRep_PrimaryQuickBar"));
				QuickBars->ProcessEvent(OnRep_PrimaryQuickBar);

				static auto OnRep_SecondaryQuickBar = QuickBars->Function(("OnRep_SecondaryQuickBar"));
				QuickBars->ProcessEvent(OnRep_SecondaryQuickBar);
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

		// static auto OnRep_QuickBar = Controller->Function(("OnRep_QuickBar"));
		// Controller->ProcessEvent(OnRep_QuickBar, nullptr);

		if ((bRemovedItem || Idx != -1) && Inventory)
			MarkArrayDirty(Inventory);

		// if (Idx != -1)
		if (ModifiedItem)
			MarkItemDirty(Inventory, ModifiedItem);
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

		static auto CTIIFn = Definition->Function(("CreateTemporaryItemInstanceBP"));

		if (CTIIFn)
			Definition->ProcessEvent(CTIIFn, &Params);

		UObject* itemInstance = Params.instance;

		if (itemInstance)
		{
			static UObject* SOCFTIFn = itemInstance->Function(("SetOwningControllerForTemporaryItem"));

			itemInstance->ProcessEvent(SOCFTIFn, &Controller);

			static auto CountOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortItemEntry"), ("Count"));

			auto GunCount = (int*)(__int64(&*GetItemEntryFromInstance(itemInstance)) + CountOffset);
			*GunCount = Count;

			*itemInstance->Member<UObject*>(("OwnerInventory")) = *Controller->Member<UObject*>(("WorldInventory"));

			return itemInstance;
		}

		return nullptr;
	}

	template <typename EntryStruct>
	int AddToReplicatedEntries(UObject* Controller, UObject* FortItem)
	{
		if (!Controller || !FortItem)
			return -1;

		auto ItemEntry = GetItemEntryFromInstance<EntryStruct>(FortItem);
		if (ItemEntry)
			return GetReplicatedEntries<EntryStruct>(Controller)->Add(*ItemEntry);

		return -1;
	}

	template <typename EntryStruct, typename Type>
	bool ChangeItemInReplicatedEntriesWithEntries(UObject* Controller, UObject* Definition, const std::string& Name, Type NewVal, int Count = -1)
	{
		if (!Controller || !Definition)
			return false;

		auto ReplicatedEntries = GetReplicatedEntries<EntryStruct>(Controller);
		bool bSuccessful = false;

		for (int x = 0; x < ReplicatedEntries->Num(); x++)
		{
			auto& ItemEntry = ReplicatedEntries->At(x);

			if (FFortItemEntry::GetItemDefinition((__int64*)&ItemEntry) == Definition && (Count == -1 ? true : *FFortItemEntry::GetCount((__int64*)&ItemEntry) == Count))
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
	bool ChangeItemInReplicatedEntries(UObject* Controller, UObject* Definition, const std::string& Name, Type NewVal, int Count = -1)
	{
		static const auto FlooredVer = std::floor(FnVerDouble);

		if (FlooredVer == 3)
		{
			struct ItemEntrySize { unsigned char Unk00[0xC8]; };
			return ChangeItemInReplicatedEntriesWithEntries<ItemEntrySize, int>(Controller, Definition, Name, NewVal, Count);
		}
		else if (FlooredVer > 3 && FnVerDouble < 7.40)
		{
			struct ItemEntrySize { unsigned char Unk00[0xD0]; };
			ChangeItemInReplicatedEntriesWithEntries<ItemEntrySize, int>(Controller, Definition, Name, NewVal, Count);
		}
		else if (FnVerDouble >= 7.40 && Engine_Version <= 424)
		{
			struct ItemEntrySize { unsigned char Unk00[0x120]; };
			ChangeItemInReplicatedEntriesWithEntries<ItemEntrySize, int>(Controller, Definition, Name, NewVal, Count);
		}
		else if (Engine_Version == 425)
		{
			struct ItemEntrySize { unsigned char Unk00[0x150]; };
			ChangeItemInReplicatedEntriesWithEntries<ItemEntrySize, int>(Controller, Definition, Name, NewVal, Count);
		}
		else if (Engine_Version >= 426 && FlooredVer < 14.60)
		{
			struct ItemEntrySize { unsigned char Unk00[0x160]; };
			ChangeItemInReplicatedEntriesWithEntries<ItemEntrySize, int>(Controller, Definition, Name, NewVal, Count);
		}
		else if (FnVerDouble == 14.60) {
			struct ItemEntrySize { unsigned char Unk00[0x180]; };
			ChangeItemInReplicatedEntriesWithEntries<ItemEntrySize, int>(Controller, Definition, Name, NewVal, Count);
		}
		else if (FlooredVer >= 15 && FlooredVer < 18)
		{
			struct ItemEntrySize { unsigned char Unk00[0x190]; };
			ChangeItemInReplicatedEntriesWithEntries<ItemEntrySize, int>(Controller, Definition, Name, NewVal, Count);
		}
		else if (FlooredVer >= 18)
		{
			struct ItemEntrySize { unsigned char Unk00[0x1A0]; };
			ChangeItemInReplicatedEntriesWithEntries<ItemEntrySize, int>(Controller, Definition, Name, NewVal, Count);
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

		auto ItemEntry = GetItemEntryFromInstance(FortItem);
		*FFortItemEntry::GetCount(ItemEntry) = Count;

		if (Engine_Version < 424)
			GetItemInstances(Controller)->Add(FortItem);

		if (7.4 > FnVerDouble)
		{
			auto QuickBars = Controller->Member<UObject*>(("QuickBars"));

			if (QuickBars && *QuickBars)
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

				static auto SAIIFn = (*QuickBars)->Function(("ServerAddItemInternal"));

				if (SAIIFn)
					(*QuickBars)->ProcessEvent(SAIIFn, &SAIIParams);
			}
		}

		int Idx = -1;

		if (FlooredVer == 3)
		{
			struct ItemEntrySize { unsigned char Unk00[0xC8]; };
			Idx = AddToReplicatedEntries<ItemEntrySize>(Controller, FortItem);
		}
		else if (std::stof(FN_Version) < 7.40f)
		{
			struct ItemEntrySize { unsigned char Unk00[0xD0]; };
			Idx = AddToReplicatedEntries<ItemEntrySize>(Controller, FortItem);
		}
		else if (FnVerDouble >= 7.40 && Engine_Version <= 424)
		{
			struct ItemEntrySize { unsigned char Unk00[0x120]; };
			Idx = AddToReplicatedEntries<ItemEntrySize>(Controller, FortItem);
		}
		else if (Engine_Version == 425)
		{
			struct ItemEntrySize { unsigned char Unk00[0x150]; };
			Idx = AddToReplicatedEntries<ItemEntrySize>(Controller, FortItem);
		}
		else if (Engine_Version >= 426 && FlooredVer < 14.60)
		{
			struct ItemEntrySize { unsigned char Unk00[0x160]; };
			Idx = AddToReplicatedEntries<ItemEntrySize>(Controller, FortItem);
		}
		else if (FnVerDouble == 14.60) {
			struct ItemEntrySize { unsigned char Unk00[0x180]; };
			Idx = AddToReplicatedEntries<ItemEntrySize>(Controller, FortItem);
		}
		else if (FlooredVer >= 15 && FlooredVer < 18)
		{
			struct ItemEntrySize { unsigned char Unk00[0x190]; };
			Idx = AddToReplicatedEntries<ItemEntrySize>(Controller, FortItem);
		}
		else if (FlooredVer >= 18)
		{
			struct ItemEntrySize { unsigned char Unk00[0x1A0]; };
			Idx = AddToReplicatedEntries<ItemEntrySize>(Controller, FortItem);
		}
		else
			std::cout << ("Could not get ItemEntrySize Struct! Please make a new one for this version using: ") << GetEntrySize() << '\n';

		if (Idx != -1)
			Inventory::Update(Controller, -1, false, (FFastArraySerializerItem*)ItemEntry);

		return FortItem;
	}

	static UObject* CreateAndAddItem(UObject* Controller, UObject* Definition, EFortQuickBars Bars, int Slot, int Count = 1)
	{
		if (Controller && Definition)
		{
			auto Instance = CreateItemInstance(Controller, Definition, Count);

			if (Instance)
			{
				auto instance = AddItem(Controller, Instance, Bars, Slot, Count);

				auto entry = GetItemEntryFromInstance(instance);

				static auto LoadedAmmoOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortItemEntry"), ("LoadedAmmo"));
			}
			else
				std::cout << ("Failed to create ItemInstance!\n");

			return Instance;
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

	UObject* RemoveItem(UObject* Controller, const FGuid& Guid)
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

		static const auto FlooredVer = std::floor(FnVerDouble);

		bool bFortnite = false;

		if (FlooredVer == 3)
		{
			struct ItemEntrySize { unsigned char Unk00[0xC8]; };
			bFortnite = RemoveGuidFromReplicatedEntries<ItemEntrySize>(Controller, Guid);
		}
		else if (FlooredVer > 3 && FnVerDouble < 7.40)
		{
			struct ItemEntrySize { unsigned char Unk00[0xD0]; };
			bFortnite = RemoveGuidFromReplicatedEntries<ItemEntrySize>(Controller, Guid);
		}
		else if (FnVerDouble >= 7.40 && Engine_Version <= 424)
		{
			struct ItemEntrySize { unsigned char Unk00[0x120]; };
			bFortnite = RemoveGuidFromReplicatedEntries<ItemEntrySize>(Controller, Guid);
		}
		else if (Engine_Version == 425)
		{
			struct ItemEntrySize { unsigned char Unk00[0x150]; };
			bFortnite = RemoveGuidFromReplicatedEntries<ItemEntrySize>(Controller, Guid);
		}
		else if (Engine_Version >= 426 && FlooredVer < 14.60)
		{
			struct ItemEntrySize { unsigned char Unk00[0x180]; };
			bFortnite = RemoveGuidFromReplicatedEntries<ItemEntrySize>(Controller, Guid);
		}
		else if (FnVerDouble == 14.60)
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

		if (bFortnite)
			Update(Controller, -1, true);

		return ItemDefinition;
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

	bool IncreaseItemCountInstance(UObject* Controller, UObject* Instance, int Count) // stack
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

			ChangeItemInReplicatedEntries<int>(Controller, GetItemDefinition(Instance), ("Count"), NewCount, OldCount);

			Update(Controller, -1, false, (FFastArraySerializerItem*)ItemEntry);

			return true;
		}

		return false;
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
					IncreaseItemCountInstance(Controller, ItemInstance, Count);
				}
			}
		}

		return false;
	}

	// TODO: Change defintion to instance
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
					auto ItemEntry = GetItemEntryFromInstance(ItemInstance);
					auto CurrentCount = FFortItemEntry::GetCount(ItemEntry);

					// std::cout << std::format("Item going to stack on count: {} Picking up item count: {}", *CurrentCount, Count) << '\n';
					auto NewCount = *CurrentCount - Count;

					if (NewCount < 0)
						NewCount = 0;

					*CurrentCount = NewCount;

					ChangeItemInReplicatedEntries<int>(Controller, Definition, ("Count"), NewCount);

					Update(Controller, -1, false, (FFastArraySerializerItem*)ItemEntry);

					if (OutItemEntry)
						*OutItemEntry = ItemEntry;

					return true;
				}
			}
		}

		return false;
	}

	// RETURNS DEFINITION
	UObject* TakeItem(UObject* Controller, const FGuid& Guid, int Count = 1, bool bDestroyIfEmpty = false) // Use this, this removes from a definition
	{
		if (!Controller)
			return nullptr;

		__int64* Entry = nullptr;

		if (DecreaseItemCount(Controller, GetDefinitionFromGuid(Controller, Guid), Count, &Entry) && Entry) // it successfully decreased
		{
			if (*FFortItemEntry::GetCount(Entry) == 0 && bDestroyIfEmpty)
				RemoveItem(Controller, Guid);
		}
		else
			RemoveItem(Controller, Guid);

		auto Pawn = Helper::GetPawnFromController(Controller);

		if (Pawn && newSwapping)
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
					}
				}
			}
		}

		return FFortItemEntry::GetItemDefinition(Entry);
	}

	static UObject* GiveItem(UObject* Controller, UObject* Definition, EFortQuickBars Bars, int Slot, int Count = 1, bool bDontCreateNewStack = false, bool* skunk = false) // Use this, it stacks if it finds an item, if not, then it adds
	{
		if (Controller && Definition)
		{
			static auto FortResourceItemDefinition = FindObject(("Class /Script/FortniteGame.FortResourceItemDefinition"));

			if (Definition->IsA(FortResourceItemDefinition))
				bDontCreateNewStack = true;

			auto ItemInstances = GetItemInstances(Controller);
			auto Pawn = Helper::GetPawnFromController(Controller);

			UObject* ItemInstance = nullptr;
			int OverStack = 0;

			if (ItemInstances)
			{
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
					auto MaxStackCount = *Definition->Member<int>(("MaxStackSize"));

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

					if (ItemInstance)
					{
						auto ItemEntry = GetItemEntryFromInstance(ItemInstance);

						if (ItemEntry)
						{
							auto currentCount = FFortItemEntry::GetCount(ItemEntry);

							if (currentCount)
							{
								OverStack = *currentCount + Count - MaxStackCount;

								// checks if it is going to overstack, if it is then we subtract the incoming count by the overstack, but its not then we just use the incoming count.
								int AmountToStack = OverStack > 0 ? Count - OverStack : Count;

								std::cout << ("MaxStackCount: ") << MaxStackCount << '\n';
								std::cout << ("OverStack: ") << OverStack << '\n';
								std::cout << ("AmountToStack: ") << AmountToStack << '\n';
								std::cout << ("currentCount: ") << *currentCount << '\n';

								IncreaseItemCountInstance(Controller, ItemInstance, AmountToStack);

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

			// If someway, the count is bigger than the MaxStackSize, and there is nothing to stack on, then it will not create 2 items.
			std::string FullName = Definition->GetFullName();
			if (FullName.contains("CarminePack") || FullName.contains("AshtonRockItemDef")) {
				Carmine::HandleCarmine(Controller);
				UObject* item = CreateAndAddItem(Controller, FindObject("/Game/Athena/Items/Gameplay/BackPacks/CarminePack/AGID_CarminePack.AGID_CarminePack"), Bars, Slot, OverStack);

				Inventory::EquipInventoryItem(Controller, Inventory::GetItemGuid(item));
			}
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

	void GiveAllAmmo(UObject* Controller)
	{
		if (Engine_Version < 420) // wrong I think
		{
			static UObject* AthenaAmmoDataRockets = FindObject(("FortAmmoItemDefinition /Game/Athena/Items/Ammo/AthenaAmmoDataRockets.AthenaAmmoDataRockets"));
			static UObject* AthenaAmmoDataShells = FindObject(("FortAmmoItemDefinition /Game/Items/Ammo/AthenaAmmoDataShells.AthenaAmmoDataShells"));
			static UObject* AthenaAmmoDataBulletsMedium = FindObject(("FortAmmoItemDefinition /Game/Items/Ammo/AthenaAmmoDataBulletsMedium.AthenaAmmoDataBulletsMedium"));
			static UObject* AthenaAmmoDataBulletsLight = FindObject(("FortAmmoItemDefinition /Game/Items/Ammo/AthenaAmmoDataBulletsLight.AthenaAmmoDataBulletsLight"));
			static UObject* AthenaAmmoDataBulletsHeavy = FindObject(("FortAmmoItemDefinition /Game/Items/Ammo/AthenaAmmoDataBulletsHeavy.AthenaAmmoDataBulletsHeavy"));

			CreateAndAddItem(Controller, AthenaAmmoDataRockets, EFortQuickBars::Secondary, 0, 999);
			CreateAndAddItem(Controller, AthenaAmmoDataShells, EFortQuickBars::Secondary, 0, 999);
			CreateAndAddItem(Controller, AthenaAmmoDataBulletsMedium, EFortQuickBars::Secondary, 0, 999);
			CreateAndAddItem(Controller, AthenaAmmoDataBulletsLight, EFortQuickBars::Secondary, 0, 999);
			CreateAndAddItem(Controller, AthenaAmmoDataBulletsHeavy, EFortQuickBars::Secondary, 0, 999);
		}
		else
		{
			static UObject* AthenaAmmoDataRockets = FindObject(("FortAmmoItemDefinition /Game/Athena/Items/Ammo/AmmoDataRockets.AmmoDataRockets"));
			static UObject* AthenaAmmoDataShells = FindObject(("FortAmmoItemDefinition /Game/Athena/Items/Ammo/AthenaAmmoDataShells.AthenaAmmoDataShells"));
			static UObject* AthenaAmmoDataBulletsMedium = FindObject(("FortAmmoItemDefinition /Game/Athena/Items/Ammo/AthenaAmmoDataBulletsMedium.AthenaAmmoDataBulletsMedium"));
			static UObject* AthenaAmmoDataBulletsLight = FindObject(("FortAmmoItemDefinition /Game/Athena/Items/Ammo/AthenaAmmoDataBulletsLight.AthenaAmmoDataBulletsLight"));
			static UObject* AthenaAmmoDataBulletsHeavy = FindObject(("FortAmmoItemDefinition /Game/Athena/Items/Ammo/AthenaAmmoDataBulletsHeavy.AthenaAmmoDataBulletsHeavy"));
			static UObject* GrapplerAmmo = FindObject("FortAmmoItemDefinition /Game/Athena/Items/Ammo/AthenaAmmoDataHooks.AthenaAmmoDataHooks");

			if (!AthenaAmmoDataRockets || !AthenaAmmoDataShells || !AthenaAmmoDataBulletsMedium || !AthenaAmmoDataBulletsLight || !AthenaAmmoDataBulletsHeavy || !GrapplerAmmo)
				std::cout << "Some ammo is invalid!\n";

			CreateAndAddItem(Controller, AthenaAmmoDataRockets, EFortQuickBars::Secondary, 0, 999);
			CreateAndAddItem(Controller, AthenaAmmoDataShells, EFortQuickBars::Secondary, 0, 999);
			CreateAndAddItem(Controller, AthenaAmmoDataBulletsMedium, EFortQuickBars::Secondary, 0, 999);
			CreateAndAddItem(Controller, AthenaAmmoDataBulletsLight, EFortQuickBars::Secondary, 0, 999);
			CreateAndAddItem(Controller, AthenaAmmoDataBulletsHeavy, EFortQuickBars::Secondary, 0, 999);
			CreateAndAddItem(Controller, GrapplerAmmo, EFortQuickBars::Secondary, 0, 999);
		}
	}

	void GiveMats(UObject* Controller)
	{
		static auto WoodItemData = FindObject(("FortResourceItemDefinition /Game/Items/ResourcePickups/WoodItemData.WoodItemData"));
		static auto StoneItemData = FindObject(("FortResourceItemDefinition /Game/Items/ResourcePickups/StoneItemData.StoneItemData"));
		static auto MetalItemData = FindObject(("FortResourceItemDefinition /Game/Items/ResourcePickups/MetalItemData.MetalItemData"));

		CreateAndAddItem(Controller, WoodItemData, EFortQuickBars::Secondary, 0, 999);
		CreateAndAddItem(Controller, StoneItemData, EFortQuickBars::Secondary, 0, 999);
		CreateAndAddItem(Controller, MetalItemData, EFortQuickBars::Secondary, 0, 999);
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
		auto Pawn = Helper::GetPawnFromController(Controller);
		auto Guid = *(*Weapon)->Member<FGuid>(("ItemEntryGuid"));
		auto Def = *(*Weapon)->Member<UObject*>(("WeaponData"));

		int Ammo = 0; // TODO: implmeent
		if (Def)
			Inventory::EquipWeaponDefinition(Pawn, Def, Guid, Ammo); // scuffed
		else
			std::cout << ("No Def!\n");
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
			int                                                Count;                                                    // (Parm, ZeroConstructor, IsPlainOldData)
		};

		auto Params = (AFortPlayerController_ServerAttemptInventoryDrop_Params*)Parameters;

		auto Definition = Inventory::TakeItem(Controller, Params->ItemGuid, Params->Count, true);
		auto Pawn = Helper::GetPawnFromController(Controller);

		if (Pawn)
		{
			auto loc = Helper::GetActorLocation(Pawn);

			auto Pickup = Helper::SummonPickup(Pawn, Definition, loc, EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::Unset, Params->Count);
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
			UObject* Pickup;
			float InFlyTime;
			FVector InStartDirection;
			bool bPlayPickupSound;
		};

		auto Params = (AFortPlayerPawn_ServerHandlePickup_Params*)Parameters;

		if (Params->Pickup && Pawn)
		{
			bool* bPickedUp = Params->Pickup->Member<bool>(("bPickedUp"));
			auto Controller = *Pawn->Member<UObject*>(("Controller"));

			if (bPickedUp && !*bPickedUp && Controller)
			{
				auto PrimaryPickupItemEntry = Params->Pickup->Member<__int64>(("PrimaryPickupItemEntry"));
				static auto ItemDefinitionOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortItemEntry"), ("ItemDefinition"));
				static auto CountOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortItemEntry"), ("Count"));

				auto Definition = (UObject**)(__int64(&*PrimaryPickupItemEntry) + ItemDefinitionOffset);
				auto Count = (int*)(__int64(&*PrimaryPickupItemEntry) + CountOffset);

				auto ItemInstances = Inventory::GetItemInstances(Controller);

				bool shouldGoInSecondaryBar = QuickBars::WhatQuickBars(*Definition) == EFortQuickBars::Secondary;

				bool bShouldSwap = false;

				// TODO: For >7.40 check all quickbar slots until we find a empty one.

				if (!shouldGoInSecondaryBar)
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

				// std::cout << "PrimaryQuickBarSlotsFilled: " << PrimaryQuickBarSlotsFilled << '\n';

				bool bSucceededSwap = true;

				auto CurrentWeapon = (*Pawn->Member<UObject*>(("CurrentWeapon")));
				auto HeldWeaponDef = *CurrentWeapon->Member<UObject*>(("WeaponData"));

				bool bDidStack = false;

				UObject* newInstance = nullptr;

				if (Definition && *Definition && Count)
				{
					newInstance = Inventory::GiveItem(Controller, *Definition, EFortQuickBars::Primary, 1, *Count, &bDidStack); // TODO: Figure out what quickbars are supposed to be used.
					Helper::EnablePickupAnimation(Pawn, Params->Pickup, /* Params->InFlyTime */ 0.75f, Params->InStartDirection);
				}

				if (bShouldSwap && !bDidStack)
				{
					static auto PickaxeDef = FindObject(("FortWeaponMeleeItemDefinition /Game/Athena/Items/Weapons/WID_Harvest_Pickaxe_Athena_C_T01.WID_Harvest_Pickaxe_Athena_C_T01"));

					if (HeldWeaponDef != PickaxeDef) // ahh
					{
						Inventory::RemoveItem(Controller, *CurrentWeapon->Member<FGuid>(("ItemEntryGuid")));
						auto loc = Helper::GetActorLocation(Pawn);

						auto DroppedPickup = Helper::SummonPickup(Pawn, HeldWeaponDef, loc, EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::Unset, 1);
					}
					else
						bSucceededSwap = false;
				}

				if (bSucceededSwap)
				{
					*bPickedUp = true;

					static auto bPickedUpFn = Params->Pickup->Function(("OnRep_bPickedUp"));

					if (bPickedUpFn)
						Params->Pickup->ProcessEvent(bPickedUpFn);
				}
			}
		}
	}

	return false;
}

inline bool ServerHandlePickupWithSwapHook(UObject* Pawn, UFunction* Function, void* Parameters)
{
	struct AFortPlayerPawn_ServerHandlePickupWithSwap_Params
	{
		UObject* Pickup;
		FGuid Swap;
		float InFlyTime;
		FVector InStartDirection;
		bool bPlayPickupSound;
	};
	auto Params = (AFortPlayerPawn_ServerHandlePickupWithSwap_Params*)Parameters;

	if (Params && Pawn && Params->Pickup)
	{
		bool* bPickedUp = Params->Pickup->Member<bool>(("bPickedUp"));

		if (bPickedUp && !*bPickedUp)
		{
			auto Controller = Pawn->Member<UObject*>(("Controller"));

			auto PrimaryPickupItemEntry = Params->Pickup->Member<__int64>(("PrimaryPickupItemEntry"));
			static auto ItemDefinitionOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortItemEntry"), ("ItemDefinition"));
			static auto CountOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortItemEntry"), ("Count"));

			auto Definition = (UObject**)(__int64(&*PrimaryPickupItemEntry) + ItemDefinitionOffset);
			auto Count = (int*)(__int64(&*PrimaryPickupItemEntry) + CountOffset);

			struct
			{
				FGuid ItemGuid;
				int Count;
			} dropParams{ Params->Swap, 1 };

			ServerAttemptInventoryDropHook(*Controller, nullptr, &dropParams);

			struct
			{
				UObject* Pickup;
				float InFlyTime;
				FVector InStartDirection;
				bool bPlayPickupSound;
			} pickupParams{ Params->Pickup, Params->InFlyTime, Params->InStartDirection, Params->bPlayPickupSound };

			ServerHandlePickupHook(Pawn, nullptr, &pickupParams);
		}
	}

	return false;
}

void __fastcall HandleReloadCostDetour(UObject* Weapon, int AmountToRemove) // nova go brr
{
	if (!Weapon || bIsPlayground)
		return;

	static auto GetOwner = Weapon->Function(("GetOwner"));
	UObject* Pawn;
	Weapon->ProcessEvent(GetOwner, &Pawn);

	if (!Pawn)
		return;

	auto PlayerController = *Pawn->Member<UObject*>(("Controller"));

	if (PlayerController)
	{
		auto WeaponData = *Weapon->Member<UObject*>(("WeaponData"));
		static auto GetAmmoWorldItemDefinition_BP = WeaponData->Function(("GetAmmoWorldItemDefinition_BP"));
		UObject* AmmoDef = nullptr;

		if (GetAmmoWorldItemDefinition_BP)
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

		return HandleReloadCost(Weapon, AmountToRemove);
	}

	return;
}

void InitializeInventoryHooks()
{
	AddHook(("Function /Script/FortniteGame.FortPlayerController.ServerExecuteInventoryItem"), ServerExecuteInventoryItemHook);

	if (Engine_Version >= 423 && FnVerDouble < 16.00)
		AddHook(("Function /Script/FortniteGame.FortPlayerController.ServerExecuteInventoryWeapon"), ServerExecuteInventoryWeaponHook);

	AddHook(("Function /Script/FortniteGame.FortPlayerController.ServerAttemptInventoryDrop"), ServerAttemptInventoryDropHook);
	AddHook(("Function /Script/FortniteGame.FortPlayerPawn.ServerHandlePickup"), ServerHandlePickupHook);

	if (FnVerDouble >= 7.40)
	{
		AddHook(("Function /Script/FortniteGame.FortPlayerPawn.ServerHandlePickupWithSwap"), ServerHandlePickupWithSwapHook);
	}

	if (HandleReloadCost)
	{
		MH_CreateHook((PVOID)HandleReloadCostAddr, HandleReloadCostDetour, (void**)&HandleReloadCost);
		MH_EnableHook((PVOID)HandleReloadCostAddr);
	}
}

void ClearInventory(UObject* Controller, bool bTakePickaxe = false)
{
	static auto BuildingItemData_Wall = FindObject(("FortBuildingItemDefinition /Game/Items/Weapons/BuildingTools/BuildingItemData_Wall.BuildingItemData_Wall"));
	static auto BuildingItemData_Floor = FindObject(("FortBuildingItemDefinition /Game/Items/Weapons/BuildingTools/BuildingItemData_Floor.BuildingItemData_Floor"));
	static auto BuildingItemData_Stair_W = FindObject(("FortBuildingItemDefinition /Game/Items/Weapons/BuildingTools/BuildingItemData_Stair_W.BuildingItemData_Stair_W"));
	static auto BuildingItemData_RoofS = FindObject(("FortBuildingItemDefinition /Game/Items/Weapons/BuildingTools/BuildingItemData_RoofS.BuildingItemData_RoofS"));
	static auto PickaxeDef = FindObject(("FortWeaponMeleeItemDefinition /Game/Athena/Items/Weapons/WID_Harvest_Pickaxe_Athena_C_T01.WID_Harvest_Pickaxe_Athena_C_T01"));

	auto ItemInstances = Inventory::GetItemInstances(Controller);

	if (ItemInstances)
	{
		for (int i = 0; i < ItemInstances->Num(); i++)
		{
			auto CurrentItemInstance = ItemInstances->At(i);

			if (!CurrentItemInstance || CurrentItemInstance->IsA(BuildingItemData_Wall) || CurrentItemInstance->IsA(BuildingItemData_Floor) || CurrentItemInstance->IsA(BuildingItemData_Stair_W) ||
				CurrentItemInstance->IsA(BuildingItemData_RoofS) || (bTakePickaxe ? false : Inventory::GetItemDefinition(CurrentItemInstance) == PickaxeDef))
				continue;

			Inventory::TakeItem(Controller, Inventory::GetItemGuid(CurrentItemInstance), *FFortItemEntry::GetCount(GetItemEntryFromInstance(CurrentItemInstance)), true);
		}
	}

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
