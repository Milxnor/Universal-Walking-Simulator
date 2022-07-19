#include <UE/structs.h>

struct FFortItemEntry // My implementation // ALWAYS BE A POINTER 
{
	FGuid GetGuid()
	{
		static auto ItemGuidOffset = FindOffsetStruct(_("ScriptStruct /Script/FortniteGame.FortItemEntry"), _("ItemGuid"));
		// ItemGuidOffset = 0x0050;
		return *(FGuid*)(__int64(this) + ItemGuidOffset);
	}

	int* GetCount()
	{
		static auto CountOffset = FindOffsetStruct(_("ScriptStruct /Script/FortniteGame.FortItemEntry"), _("Count"));
		return (int*)(__int64(this) + CountOffset);
	}

	static int GetStructSize()
	{
		return 0x120;
	}
};

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
		static __int64 ItemInstancesOffset = *(uint32_t*)(__int64(FindObject("ArrayProperty /Script/FortniteGame.FortItemList.ItemInstances")) + 0x44); // FindOffsetStruct(_("ScriptStruct /Script/FortniteGame.FortItemList"), _("ItemInstances"));

		auto Inventory = GetInventory(Controller);

		std::cout << _("ItemInstances Offset: ") << ItemInstancesOffset << '\n';

		// ItemInstancesOffset = 0x110;

		return (TArray<UObject*>*)(__int64(Inventory) + ItemInstancesOffset);
	}

	inline UObject* EquipWeaponDefinition(UObject* Pawn, UObject* Definition, FGuid& Guid, int Ammo = 0)
	{
		// auto weaponClass = Definition->GetWeaponActorClass();
		// if (weaponClass)
		{
			// auto Weapon = (AFortWeapon*)SpawnActorTrans(weaponClass, {}, Pawn);
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
				*Weapon->Member<UObject*>(_("WeaponData")) = Definition;
				*Weapon->Member<FGuid>(_("ItemEntryGuid")) = Guid;

				// if (bEquipWithMaxAmmo)
				{
					static auto getBulletsFn = Weapon->Function(_("GetBulletsPerClip"));
					if (getBulletsFn)
					{
						int BulletsPerClip;
						Weapon->ProcessEvent(getBulletsFn, &BulletsPerClip);
						*Weapon->Member<int>(_("AmmoCount")) = BulletsPerClip;
					}
					else
						std::cout << _("No GetBulletsPerClip!\n");
				}

				// Instance->ItemEntry.LoadedAmmo = Weapon->AmmoCount;

				Helper::SetOwner(Weapon, Pawn);
				Weapon->ProcessEvent(_("OnRep_ReplicatedWeaponData"));
				Weapon->ProcessEvent(_("OnRep_AmmoCount"));

				struct { UObject* P; } givenParams{ Pawn };

				static auto givenFn = Weapon->Function(_("ClientGivenTo"));
				if (givenFn)
					givenFn->ProcessEvent(givenFn, &givenParams);
				else
					std::cout << _("No ClientGivenTo!\n");

				struct { UObject* Weapon; } internalEquipParams{ Weapon };

				static auto internalEquipFn = Pawn->Function(_("ClientInternalEquipWeapon"));
				if (internalEquipFn)
					Pawn->ProcessEvent(internalEquipFn, &internalEquipParams);
				else
					std::cout << _("No ClientInternalEquipWeapon!\n");

				// Pawn->OnRep_CurrentWeapon(); // i dont think this is needed but alr
			}
			else
				std::cout << _("No weapon!\n");

			return Weapon;
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
				if (auto Def = GetItemDefinition(CurrentItemInstance))
					return EquipWeaponDefinition(Pawn, Def, Guid);
			}
		}
	}

	template <typename EntryStruct>
	TArray<EntryStruct>* GetReplicatedEntries(UObject* Controller)
	{
		static __int64 ReplicatedEntriesOffset = *(uint32_t*)(__int64(FindObject(_("ArrayProperty /Script/FortniteGame.FortItemList.ReplicatedEntries"))) + 0x44);// FindOffsetStruct(_("ScriptStruct /Script/FortniteGame.FortItemList"), _("ReplicatedEntries"));

		auto Inventory = GetInventory(Controller);
		// ReplicatedEntriesOffset = 0xB0;

		std::cout << _("ReplicatedEntries Offset: ") << ReplicatedEntriesOffset << '\n';

		return (TArray<EntryStruct>*)(__int64(Inventory) + ReplicatedEntriesOffset);

		// return (TArray<EntryStruct>*)(__int64(Inventory) + ReplicatedEntriesOffset);
	}

	template<typename ItemEntryStruct> // = FFortItemEntry>
	void Update(UObject* Controller, int Idx = -1, bool bRemovedItem = false)
	{
		// auto QuickBars = *Controller->Member<UObject*>(_("QuickBars"));

		auto WorldInventory = *Controller->Member<UObject*>(_("WorldInventory"));
		auto Inventory = GetInventory(Controller);

		static auto WorldHandleInvUpdate = WorldInventory->Function(_("HandleInventoryLocalUpdate"));

		// *Inventorya->Member<bool>(_("bRequiresLocalUpdate")) = true;

		if (WorldHandleInvUpdate)
		{
			WorldInventory->ProcessEvent(WorldHandleInvUpdate);

			static auto PCHandleInvUpdate = Controller->Function(_("HandleWorldInventoryLocalUpdate"));

			if (PCHandleInvUpdate)
				Controller->ProcessEvent(PCHandleInvUpdate);
		}

		static auto ClientForceUpdateQuickbar = FindObject(_("Function /Script/FortniteGame.FortPlayerController.ClientForceUpdateQuickbar"));
		auto PrimaryQuickbar = EFortQuickBars::Primary;
		Controller->ProcessEvent(ClientForceUpdateQuickbar, &PrimaryQuickbar);

		// static auto OnRep_QuickBar = Controller->Function(_("OnRep_QuickBar"));
		// Controller->ProcessEvent(OnRep_QuickBar, nullptr);

		
		/* if (QuickBars)
		{
			static auto OnRep_PrimaryQuickBar = QuickBars->Function(_("OnRep_PrimaryQuickBar"));
			QuickBars->ProcessEvent(OnRep_PrimaryQuickBar);

			static auto OnRep_SecondaryQuickBar = QuickBars->Function(_("OnRep_SecondaryQuickBar"));
			QuickBars->ProcessEvent(OnRep_SecondaryQuickBar);
		} */

		if (bRemovedItem || Idx != -1)
			((FFastArraySerializer*)Inventory)->MarkArrayDirty();

		// if (Idx != -1)
			// ((FFastArraySerializer*)Inventorya)->MarkItemDirty((FFastArraySerializerItem)GetReplicatedEntries<ItemEntryStruct>(Controller)->At(Idx));
	}

	UObject* CreateItemInstance(UObject* Controller, UObject* Definition, int Count = 1)
	{
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

			// *itemInstance->Member<FFortItemEntry>(_("ItemEntry"))->GetCount() = Count;

			auto OwnerInventory = itemInstance->Member<UObject*>(_("OwnerInventory"));

			if (OwnerInventory && *OwnerInventory)
				std::cout << _("OwnerInventory Name: ") << (*OwnerInventory)->GetFullName() << '\n';
			else
				std::cout << _("OwnerInventory Invalid: ") << OwnerInventory << '\n';

			return itemInstance;
		}

		return nullptr;
	}

	template <typename ItemEntryStruct> // TODO: Do concepts and make sure its a FFortItemEntry
	int AddToReplicatedEntries(UObject* Controller, UObject* FortItem)
	{
		if (!Controller || !FortItem)
			return -1;

		auto ItemEntry = FortItem->Member<ItemEntryStruct>(_("ItemEntry"));

		std::cout << _("ItemEntryStruct Size: ") << sizeof(ItemEntryStruct) << '\n';

		if (ItemEntry)
			return GetReplicatedEntries<ItemEntryStruct>(Controller)->Add(*ItemEntry);

		return -1;
	}

	static int AddItem(UObject* Controller, UObject* FortItem, EFortQuickBars Bars, int Slot)
	{
		static const auto FlooredVer = std::floor(std::stod(FN_Version));

		// auto Inventory = GetInventory(Controller);

		int Index = -1;

		struct ItemEntrySize // : FFortItemEntry
		{
			unsigned char Unk00[0x120];
		};

		/* if (FlooredVer == 3)
		{
			struct ItemEntrySize : FFortItemEntry
			{
				unsigned char Unk00[0xC0];
			};
			Index = AddToReplicatedEntries<ItemEntrySize>(Controller, FortItem);
			Update<ItemEntrySize>(Controller, Index);
		}
		else if (FlooredVer == 4 || FlooredVer == 5 || FlooredVer == 6)
		{
			struct ItemEntrySize : FFortItemEntry
			{
				unsigned char Unk00[0xD0];
			};
			Index = AddToReplicatedEntries<ItemEntrySize>(Controller, FortItem);
			Update<ItemEntrySize>(Controller, Index);
		}
		else if (FlooredVer == 7  || FlooredVer == 9)
		{
			struct ItemEntrySize : FFortItemEntry
			{
				unsigned char Unk00[0x120];
			};
			Index = AddToReplicatedEntries<ItemEntrySize>(Controller, FortItem);
			Update<ItemEntrySize>(Controller, Index);
		} */

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

		Index = AddToReplicatedEntries<ItemEntrySize>(Controller, FortItem);
		Inventory::Update<ItemEntrySize>(Controller, Index, true);

		return Index;
	}

	static void CreateAndAddItem(UObject* Controller, UObject* Definition, EFortQuickBars Bars, int Slot, int Count = 1)
	{
		if (Controller)
		{
			auto Instance = CreateItemInstance(Controller, Definition, Count);

			if (Instance)
				AddItem(Controller, Instance, Bars, Slot);
			else
				std::cout << _("Failed to create ItemInstance!\n");
		}
	}

}

inline bool ServerExecuteInventoryItemHook(UObject* Controller, UFunction* Function, void* Parameters)
{
	Inventory::EquipInventoryItem(Controller, *(FGuid*)Parameters);
	return false;
}

void InitializeInventoryHooks()
{
	// AddHook(_("Function /Script/FortniteGame.FortPlayerController.ServerExecuteInventoryItem"), ServerExecuteInventoryItemHook);
}