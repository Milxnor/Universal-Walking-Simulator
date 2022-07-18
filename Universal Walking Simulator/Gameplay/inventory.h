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
		return 0xD0;
	}
};

namespace Inventory
{
	TArray<UObject*>* GetItemInstances(UObject* Controller)
	{
		static auto ItemInstancesOffset = FindOffsetStruct(_("ScriptStruct /Script/FortniteGame.FortItemList"), _("ItemInstances"));

		auto WorldInventory = *Controller->Member<UObject*>(_("WorldInventory"));
		auto Inventory = WorldInventory->Member<void>(_("Inventory"));

		// ItemInstancesOffset = 0x110;

		return (TArray<UObject*>*)(__int64(Inventory) + ItemInstancesOffset);
	}

	template <typename EntryStruct>
	TArray<EntryStruct>* GetReplicatedEntries(UObject* Controller)
	{
		static auto ReplicatedEntriesOffset = FindOffsetStruct(_("ScriptStruct /Script/FortniteGame.FortItemList"), _("ReplicatedEntries"));

		auto WorldInventory = *Controller->Member<UObject*>(_("WorldInventory"));
		auto Inventory = WorldInventory->Member<void>(_("Inventory"));
		// ReplicatedEntriesOffset = 0xB0;

		return (TArray<EntryStruct>*)(__int64(Inventory) + ReplicatedEntriesOffset);
	}

	template<typename ItemEntryStruct = FFortItemEntry>
	void Update(UObject* Controller, int Idx = -1, bool bRemovedItem = false)
	{
		auto QuickBars = *Controller->Member<UObject*>(_("QuickBars"));

		auto WorldInventory = *Controller->Member<UObject*>(_("WorldInventory"));
		auto Inventory = WorldInventory->Member<void>(_("Inventory"));

		static auto WorldHandleInvUpdate = WorldInventory->Function(_("HandleInventoryLocalUpdate"));

		if (WorldHandleInvUpdate)
		{
			// WorldInventory->ProcessEvent(WorldHandleInvUpdate, nullptr);

			static auto PCHandleInvUpdate = Controller->Function(_("HandleWorldInventoryLocalUpdate"));

			// if (PCHandleInvUpdate)
				// Controller->ProcessEvent(PCHandleInvUpdate);
		}

		// static auto OnRep_QuickBar = Controller->Function(_("OnRep_QuickBar"));
		// Controller->ProcessEvent(OnRep_QuickBar, nullptr);

		if (QuickBars)
		{
			static auto OnRep_PrimaryQuickBar = QuickBars->Function(_("OnRep_PrimaryQuickBar"));
			QuickBars->ProcessEvent(OnRep_PrimaryQuickBar);

			static auto OnRep_SecondaryQuickBar = QuickBars->Function(_("OnRep_SecondaryQuickBar"));
			QuickBars->ProcessEvent(OnRep_SecondaryQuickBar);
		}

		if (bRemovedItem || Idx != -1)
			((FFastArraySerializer*)Inventory)->MarkArrayDirty();

		// if (Idx != -1)
			// ((FFastArraySerializer*)Inventory)->MarkItemDirty((FFastArraySerializerItem)GetReplicatedEntries<ItemEntryStruct>(Controller)->At(Idx));
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

			*itemInstance->Member<FFortItemEntry>(_("ItemEntry"))->GetCount() = Count;

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

		if (ItemEntry)
			return GetReplicatedEntries<ItemEntryStruct>(Controller)->Add(*ItemEntry);

		return -1;
	}

	static int AddItem(UObject* Controller, UObject* FortItem, EFortQuickBars Bars, int Slot)
	{
		static const auto FlooredVer = std::floor(std::stod(FN_Version));

		auto Inventory = *Controller->Member<UObject*>(_("WorldInventory"));

		int Index = -1;

		if (FlooredVer == 3)
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
		}

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