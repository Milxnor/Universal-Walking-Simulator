#include <UE/structs.h>
#include <Gameplay/helper.h>

static int GetEntrySize()
{
	static auto FortItemEntryClass = FindObject(_("ScriptStruct /Script/FortniteGame.FortItemEntry"), true);

	if (Engine_Version <= 420)
		return ((UClass_FT*)FortItemEntryClass)->PropertiesSize;

	else if (Engine_Version == 421) // && Engine_Version <= 424)
		return ((UClass_FTO*)FortItemEntryClass)->PropertiesSize;

	else if (Engine_Version >= 422 && Engine_Version <= 424)
		return ((UClass_FTT*)FortItemEntryClass)->PropertiesSize;

	else if (Engine_Version >= 425)
		return ((UClass_CT*)FortItemEntryClass)->PropertiesSize;
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
		if (Pawn)
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

				std::cout << _("Setting owner!\n");
				Helper::SetOwner(Weapon, Pawn);
				std::cout << _("Setted owner!\n");
				
				Weapon->ProcessEvent(_("OnRep_ReplicatedWeaponData"));
				// Weapon->ProcessEvent(_("OnRep_AmmoCount"));

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

	TArray<__int64>* GetReplicatedEntries(UObject* Controller)
	{
		static __int64 ReplicatedEntriesOffset = *(uint32_t*)(__int64(FindObject(_("ArrayProperty /Script/FortniteGame.FortItemList.ReplicatedEntries"))) + 0x44);// FindOffsetStruct(_("ScriptStruct /Script/FortniteGame.FortItemList"), _("ReplicatedEntries"));

		auto Inventory = GetInventory(Controller);
		// ReplicatedEntriesOffset = 0xB0;

		std::cout << _("ReplicatedEntries Offset: ") << ReplicatedEntriesOffset << '\n';

		return (TArray<__int64>*)(__int64(Inventory) + ReplicatedEntriesOffset);

		// return (TArray<EntryStruct>*)(__int64(Inventory) + ReplicatedEntriesOffset);
	}

	void Update(UObject* Controller, int Idx = -1, bool bRemovedItem = false)
	{
		static const auto FnVerDouble = std::stod(FN_Version);

		auto WorldInventory = *Controller->Member<UObject*>(_("WorldInventory"));
		auto Inventory = GetInventory(Controller);

		static auto WorldHandleInvUpdate = WorldInventory->Function(_("HandleInventoryLocalUpdate"));

		if (WorldHandleInvUpdate)
		{
			WorldInventory->ProcessEvent(WorldHandleInvUpdate);

			static auto PCHandleInvUpdate = Controller->Function(_("HandleWorldInventoryLocalUpdate"));

			if (PCHandleInvUpdate)
				Controller->ProcessEvent(PCHandleInvUpdate);
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
			static auto ClientForceUpdateQuickbar = FindObject(_("Function /Script/FortniteGame.FortPlayerController.ClientForceUpdateQuickbar"));
			auto PrimaryQuickbar = EFortQuickBars::Primary;
			Controller->ProcessEvent(ClientForceUpdateQuickbar, &PrimaryQuickbar);
			auto SecondaryQuickbar = EFortQuickBars::Secondary;
			Controller->ProcessEvent(ClientForceUpdateQuickbar, &SecondaryQuickbar);
		}

		// static auto OnRep_QuickBar = Controller->Function(_("OnRep_QuickBar"));
		// Controller->ProcessEvent(OnRep_QuickBar, nullptr);

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

	int AddToReplicatedEntries(UObject* Controller, UObject* FortItem)
	{
		if (!Controller || !FortItem)
			return -1;

		auto ItemEntry = FortItem->Member<__int64>(_("ItemEntry"));

		std::cout << _("ItemEntryStruct Size: ") << sizeof(GetEntrySize()) << '\n';

		if (ItemEntry)
			return GetReplicatedEntries(Controller)->Add(*ItemEntry, GetEntrySize());

		return -1;
	}

	static void AddItem(UObject* Controller, UObject* FortItem, EFortQuickBars Bars, int Slot)
	{
		static const auto FlooredVer = std::floor(std::stod(FN_Version));

		// auto Inventory = GetInventory(Controller);

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

		AddToReplicatedEntries(Controller, FortItem);
		Inventory::Update(Controller, -1, true);
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

inline bool ServerAttemptInventoryDropHook(UObject* Controller, UFunction* Function, void* Parameters)
{
	if (Controller && Parameters)
	{
	}

	return false;
}

void InitializeInventoryHooks()
{
	AddHook(_("Function /Script/FortniteGame.FortPlayerController.ServerExecuteInventoryItem"), ServerExecuteInventoryItemHook);
	AddHook(_("Function /Script/FortniteGame.FortPlayerController.ServerAttemptInventoryDrop"), ServerAttemptInventoryDropHook);
}