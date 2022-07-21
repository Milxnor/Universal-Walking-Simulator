#include <UE/structs.h>
#include <Gameplay/helper.h>

static int GetEntrySize()
{
	static auto FortItemEntryClass = FindObject(_("ScriptStruct /Script/FortniteGame.FortItemEntry"), true);

	return GetSizeOfStruct(FortItemEntryClass);
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

		std::cout << _("ItemInstances Offset: ") << ItemInstancesOffset << '\n';

		// ItemInstancesOffset = 0x110;

		return (TArray<UObject*>*)(__int64(Inventory) + ItemInstancesOffset);
	}

	inline void EquipWeapon(UObject* Pawn, UObject* FortWeapon, const FGuid& Guid, int Ammo = 0)
	{
		if (FortWeapon && Pawn)
		{
			// if (bEquipWithMaxAmmo)
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
					*Weapon->Member<UObject*>(_("WeaponData")) = Definition;
					EquipWeapon(Pawn, Weapon, Guid, Ammo);
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

		return nullptr;
	}

	template <typename EntryStruct>
	TArray<EntryStruct>* GetReplicatedEntries(UObject* Controller)
	{
		static __int64 ReplicatedEntriesOffset = FindOffsetStruct(_("ScriptStruct /Script/FortniteGame.FortItemList"), _("ReplicatedEntries"));

		auto Inventory = GetInventory(Controller);
		// ReplicatedEntriesOffset = 0xB0;

		std::cout << _("ReplicatedEntries Offset: ") << ReplicatedEntriesOffset << '\n';

		return (TArray<EntryStruct>*)(__int64(Inventory) + ReplicatedEntriesOffset);

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

			// *itemInstance->Member<FFortItemEntry>(_("ItemEntry"))->GetCount() = Count;

			auto OwnerInventory = itemInstance->Member<UObject*>(_("OwnerInventory")); // We should probably set this?

			if (OwnerInventory && *OwnerInventory)
				std::cout << _("OwnerInventory Name: ") << (*OwnerInventory)->GetFullName() << '\n';
			else
				std::cout << _("OwnerInventory Invalid: ") << OwnerInventory << '\n';

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

		if (FlooredVer == 3)
		{
			struct ItemEntrySize { unsigned char Unk00[0xC0]; };
			AddToReplicatedEntries<ItemEntrySize>(Controller, FortItem);
		}
		else if (FlooredVer > 4 && std::stod(FN_Version) < 7.40)
		{
			struct ItemEntrySize { unsigned char Unk00[0xD0]; };
			AddToReplicatedEntries<ItemEntrySize>(Controller, FortItem);
		}
		else if (std::stod(FN_Version) >= 7.40 && Engine_Version < 424) // not right idc
		{
			struct ItemEntrySize { unsigned char Unk00[0x120]; };
			AddToReplicatedEntries<ItemEntrySize>(Controller, FortItem);
		}
		else
			std::cout << _("Could not get ItemEntrySize Struct! Please make a new one for this version using: ") << GetEntrySize() << '\n';

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

	void OnPickup()
	{

	}

	UObject* DecreaseItemCount()
	{

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
	if (Controller && Parameters)
	{

	}

	return false;
}

void InitializeInventoryHooks()
{
	AddHook(_("Function /Script/FortniteGame.FortPlayerController.ServerExecuteInventoryItem"), ServerExecuteInventoryItemHook);
	AddHook(_("Function /Script/FortniteGame.FortPlayerController.ServerExecuteInventoryWeapon"), ServerExecuteInventoryWeaponHook);
	AddHook(_("Function /Script/FortniteGame.FortPlayerController.ServerAttemptInventoryDrop"), ServerAttemptInventoryDropHook);
}