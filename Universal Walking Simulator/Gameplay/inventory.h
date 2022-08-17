#pragma once

#include <UE/structs.h>
#include <Gameplay/helper.h>
#include <Gameplay/abilities.h>

/*

GiveItem - Creates a new item and or stacks
TakeItem - Decreases and or removes if requested

*/

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

		static const auto FnVerDouble = std::stod(FN_Version);

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
		// static auto WorldInventoryOffset = GetOffset(Controller, "WorldInventory");
		// auto WorldInventory = *(UObject**)(__int64(Controller) + WorldInventoryOffset);

		return *Controller->Member<UObject*>(("WorldInventory")); // WorldInventory;
	}

	__int64* GetInventory(UObject* Controller)
	{
		auto WorldInventory = GetWorldInventory(Controller);

		if (WorldInventory)
		{
			// static auto InventoryOffset = GetOffset(WorldInventory, "Inventory");
			auto Inventorya = WorldInventory->Member<__int64>(("Inventory")); // (__int64*)(__int64(WorldInventory) + InventoryOffset);

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

	inline void EquipWeapon(UObject* Pawn, UObject* FortWeapon, const FGuid& Guid, int Ammo = 0)
	{
		static auto PickaxeDef = FindObject(("FortWeaponMeleeItemDefinition /Game/Athena/Items/Weapons/WID_Harvest_Pickaxe_Athena_C_T01.WID_Harvest_Pickaxe_Athena_C_T01"));
		auto CurrentWeapon = *Pawn->Member<UObject*>(("CurrentWeapon"));
		if (FortWeapon && Pawn)
		{
			//if (*CurrentWeapon->Member<UObject*>(("WeaponData")) != PickaxeDef)
			{
				static auto getBulletsFn = FortWeapon->Function(("GetBulletsPerClip"));
				if (getBulletsFn)
				{
					int BulletsPerClip;
					FortWeapon->ProcessEvent(getBulletsFn, &BulletsPerClip);
					*FortWeapon->Member<int>(("AmmoCount")) = BulletsPerClip;
				}
				else
					std::cout << ("No GetBulletsPerClip!\n");
			}
			 
			// Instance->ItemEntry.LoadedAmmo = Weapon->AmmoCount;

			static auto OnRep_ReplicatedWeaponData = ("OnRep_ReplicatedWeaponData");
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

			if (std::stod(FN_Version) >= 7.40)
			{
				static auto internalEquipFn = Pawn->Function(("ClientInternalEquipWeapon"));
				if (internalEquipFn)
					Pawn->ProcessEvent(internalEquipFn, &internalEquipParams);
				else
					std::cout << ("No ClientInternalEquipWeapon!\n");
			}

			std::cout << ("Noob!\n");

			// Pawn->OnRep_CurrentWeapon(); // i dont think this is needed but alr
		}
		else
			std::cout << ("No weapon!\n");
	}

	inline UObject* EquipWeaponDefinition(UObject* Pawn, UObject* Definition, const FGuid& Guid, int Ammo = 0)
	{
		if (Pawn && Definition)
		{
			auto FullName = Definition->GetFullName();
			bool IsAGID = (FullName.contains(("AthenaGadgetItemDefinition ")) || FullName.contains(("FortGadgetItemDefinition "))) ? true : false; // TODO: Use IsA
			
			if (!IsAGID)
			{
				// i dont think we need this on s15?
				static auto equipFn = Pawn->Function(("EquipWeaponDefinition"));
				struct {
					UObject* Def;
					FGuid Guid;
					UObject* Wep;
				} params{ Definition, Guid };
				if (equipFn)
					Pawn->ProcessEvent(equipFn, &params);
				auto Weapon = params.Wep;

				if (Weapon)
				{
					Helper::SetOwner(Weapon, Pawn);
					//*Weapon->Member<UObject*>(("WeaponData")) = Definition;
					//EquipWeapon(Pawn, Weapon, Guid, Ammo);
					return Weapon;
				}
				else
					std::cout << ("Failed to spawn Weapon!\n");

				return Weapon;
			}
			else
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
					EquipWeaponDefinition(Pawn, Def, Guid, Ammo);
				}
				else
					std::cout << ("Failed to get AGID's Definition!\n");

				if (FullName.contains(("AthenaGadgetItemDefinition "))) // Ability class seems to always be invalid
				{
					std::cout << ("Trying to get gameplay ability of class.\n");
					static auto GetGameplayAbility = Definition->Function(("GetGameplayAbility"));
					std::cout << ("Found GetGameplayAbility function.\n");
					//UObject* GameplayAbility = nullptr;
					//FindObject<TSoftClassPtr>("SoftClassProperty /Script/FortniteGame.FortGadgetItemDefinition.GameplayAbility");

					//UClass* GameplayAbility = Helper::Conversion::SoftClassToClass(Definition->Member<TSoftClassPtr>("GameplayAbility"));
					std::cout << ("Trying to convert to class.\n");
					static auto KSLClass = FindObject(("KismetSystemLibrary /Script/Engine.Default__KismetSystemLibrary"));

					static auto fn = KSLClass->Function(("Conv_SoftClassReferenceToClass"));

					/*auto Class = FindObject<TSoftClassPtr>("SoftClassProperty /Script/FortniteGame.FortGadgetItemDefinition.GameplayAbility");
					auto gameplayclass = Helper::Conversion::SoftClassToClass(Class);*/
					/*std::cout << ("Found Class");
					struct {
						//TSoftClassPtr* SoftClass;
						UClass* ReturnValue;
					} params{nullptr};

					if (fn)
					{
						std::cout << ("Trying to process Conversion");
						KSLClass->ProcessEvent(fn, &Class);
						std::cout << ("Processed Conversion");
					}
						
					//UClass* GameplayAbility = params.ReturnValue;
					std::cout << ("Got return value.\n");*/
					/*if (GetGameplayAbility)
					{
						Definition->ProcessEvent(GetGameplayAbility, &GameplayAbility);
						std::cout << ("Processing GetGameplayAbility");
					}
					else
						std::cout << ("Get Gameplay Ability Failed!\n");*/
					/*if (gameplayclass)
					{
						//std::cout << ("Granting ability: ") << GameplayAbility->GetFullName() << '\n';
						std::cout << ("Granting ability: ");
						GrantGameplayAbility(Pawn, gameplayclass);
					}
					else
						std::cout << ("Failed to get GameplayAbilityClass!\n");*/
				} 
			}
		}

		return nullptr;
	}

	FGuid GetItemGuid(UObject* ItemInstance)
	{
		static UObject* GetItemGuidFn = FindObject(("Function /Script/FortniteGame.FortItem.GetItemGuid"));
		FGuid Guid;

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

	inline UObject* EquipInventoryItem(UObject* Controller, const FGuid& Guid)
	{
		auto ItemInstances = GetItemInstances(Controller);
		auto Pawn = Helper::GetPawnFromController(Controller);

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
				std::string FullName = Def->GetFullName();
				if (FullName.contains("CarminePack") || FullName.contains("AshtonPack.")) {
					Items::HandleCarmine(*Pawn->Member<UObject*>("Controller"));
				}
			}
		}

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

	void Update(UObject* Controller, int Idx = -1, bool bRemovedItem = false, FFastArraySerializerItem* ModifiedItem = nullptr)
	{
		static const auto FnVerDouble = std::stod(FN_Version);

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

			auto OwnerInventory = itemInstance->Member<UObject*>(("OwnerInventory")); // We should probably set this?
			*OwnerInventory = *Controller->Member<UObject*>(("WorldInventory"));

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
		static const auto FlooredVer = std::floor(std::stod(FN_Version));

		if (FlooredVer == 3)
		{
			struct ItemEntrySize { unsigned char Unk00[0xC0]; };
			return ChangeItemInReplicatedEntriesWithEntries<ItemEntrySize, int>(Controller, Definition, Name, NewVal, Count);
		}
		else if (FlooredVer > 3 && std::stod(FN_Version) < 7.40)
		{
			struct ItemEntrySize { unsigned char Unk00[0xD0]; };
			ChangeItemInReplicatedEntriesWithEntries<ItemEntrySize, int>(Controller, Definition, Name, NewVal, Count);
		}
		else if (std::stod(FN_Version) >= 7.40 && Engine_Version <= 424)
		{
			struct ItemEntrySize { unsigned char Unk00[0x120]; };
			ChangeItemInReplicatedEntriesWithEntries<ItemEntrySize, int>(Controller, Definition, Name, NewVal, Count);
		}
		else if (Engine_Version == 425)
		{
			struct ItemEntrySize { unsigned char Unk00[0x150]; };
			ChangeItemInReplicatedEntriesWithEntries<ItemEntrySize, int>(Controller, Definition, Name, NewVal, Count);
		}
		else if (Engine_Version >= 426 && FlooredVer < 15) // idk if right
		{
			struct ItemEntrySize { unsigned char Unk00[0x160]; };
			ChangeItemInReplicatedEntriesWithEntries<ItemEntrySize, int>(Controller, Definition, Name, NewVal, Count);
		}
		else if (FlooredVer >= 15)
		{
			struct ItemEntrySize { unsigned char Unk00[0x190]; };
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
				// break;
			}
		}

		if (!bSuccessful)
			std::cout << ("Failed to find ItemGuid in Inventory!\n");

		return bSuccessful;
	}

	static void AddItem(UObject* Controller, UObject* FortItem, EFortQuickBars Bars, int Slot, int Count = 1)
	{
		if (!Controller || !FortItem)
			return;

		static const auto FlooredVer = std::floor(std::stod(FN_Version));

		// auto Inventory = GetInventory(Controller);

		auto ItemEntry = GetItemEntryFromInstance(FortItem);
		*FFortItemEntry::GetCount(ItemEntry) = Count;

		if (Engine_Version < 424) // chapter two momentum
			GetItemInstances(Controller)->Add(FortItem);

		static const auto FnVerDouble = std::stod(FN_Version);

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
			struct ItemEntrySize { unsigned char Unk00[0xC0]; };
			Idx = AddToReplicatedEntries<ItemEntrySize>(Controller, FortItem);
		}
		else if (std::stof(FN_Version) < 7.40f)
		{
			struct ItemEntrySize { unsigned char Unk00[0xD0]; };
			Idx = AddToReplicatedEntries<ItemEntrySize>(Controller, FortItem);
		}
		else if (std::stod(FN_Version) >= 7.40 && Engine_Version <= 424)
		{
			struct ItemEntrySize { unsigned char Unk00[0x120]; };
			Idx = AddToReplicatedEntries<ItemEntrySize>(Controller, FortItem);
		}
		else if (Engine_Version == 425)
		{
			struct ItemEntrySize { unsigned char Unk00[0x150]; };
			Idx = AddToReplicatedEntries<ItemEntrySize>(Controller, FortItem);
		}
		else if (Engine_Version >= 426 && FlooredVer < 15)
		{
			struct ItemEntrySize { unsigned char Unk00[0x160]; };
			Idx = AddToReplicatedEntries<ItemEntrySize>(Controller, FortItem);
		}
		else if (FlooredVer >= 15)
		{
			struct ItemEntrySize { unsigned char Unk00[0x190]; };
			Idx = AddToReplicatedEntries<ItemEntrySize>(Controller, FortItem);
		}
		else
			std::cout << ("Could not get ItemEntrySize Struct! Please make a new one for this version using: ") << GetEntrySize() << '\n';

		if (Idx != -1)
			Inventory::Update(Controller, -1, false, (FFastArraySerializerItem*)ItemEntry);
	}

	static UObject* CreateAndAddItem(UObject* Controller, UObject* Definition, EFortQuickBars Bars, int Slot, int Count = 1)
	{
		if (Controller && Definition)
		{
			auto Instance = CreateItemInstance(Controller, Definition, Count);

			if (Instance)
				AddItem(Controller, Instance, Bars, Slot, Count);
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

		static const auto FlooredVer = std::floor(std::stod(FN_Version));

		bool bFortnite = false;

		if (FlooredVer == 3)
		{
			struct ItemEntrySize { unsigned char Unk00[0xC0]; };
			bFortnite = RemoveGuidFromReplicatedEntries<ItemEntrySize>(Controller, Guid);
		}
		else if (FlooredVer > 3 && std::stod(FN_Version) < 7.40)
		{
			struct ItemEntrySize { unsigned char Unk00[0xD0]; };
			bFortnite = RemoveGuidFromReplicatedEntries<ItemEntrySize>(Controller, Guid);
		}
		else if (std::stod(FN_Version) >= 7.40 && Engine_Version <= 424)
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
		else if (FlooredVer >= 15)
		{
			struct ItemEntrySize { unsigned char Unk00[0x190]; };
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
		__int64* Entry = nullptr;

		if (DecreaseItemCount(Controller, GetDefinitionFromGuid(Controller, Guid), Count, &Entry) && Entry) // it successfully decreased
		{
			if (*FFortItemEntry::GetCount(Entry) == 0 && bDestroyIfEmpty)
				RemoveItem(Controller, Guid);
		}
		else
			RemoveItem(Controller, Guid);

		return FFortItemEntry::GetItemDefinition(Entry);
	}

	// dont use bDidStack unless you know what ur doing
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
								//			      3	      +   2   -      6       =   -1
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

	void GiveAllAmmo(UObject* Controller)
	{
		// omfg

		static const auto FnVerDouble = std::stod(FN_Version);

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

		// Inventory::EquipInventoryItem(Controller, Guid); // "hacky" // this doesjnt work maybe
		
		if (Def)
			Inventory::EquipWeaponDefinition(Pawn, Def, Guid, Ammo); // scuffed
		else
			std::cout << ("No Def!\n");

		// Inventory::EquipWeapon, *Weapon, Guid);
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

		if (Params->Pickup && Pawn)
		{
			bool* bPickedUp = Params->Pickup->Member<bool>(("bPickedUp"));
			auto Controller = *Pawn->Member<UObject*>(("Controller"));

			if (bPickedUp && !*bPickedUp && Controller)
			{
				auto PrimaryPickupItemEntry = Params->Pickup->Member<__int64>(("PrimaryPickupItemEntry"));
				static auto ItemDefinitionOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortItemEntry"), ("ItemDefinition"));
				static auto CountOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortItemEntry"), ("Count"));

				// auto SwappinDef = *(*Controller)->Member<UObject*>(("SwappingItemDefinition")); // This is the same tihng as the pickup

				auto Definition = (UObject**)(__int64(&*PrimaryPickupItemEntry) + ItemDefinitionOffset);
				auto Count = (int*)(__int64(&*PrimaryPickupItemEntry) + CountOffset);

				auto ItemInstances = Inventory::GetItemInstances(Controller);

				// kms (TODO: Check GetNumQuickBarSlots)
				
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

				if (Definition && *Definition && Count)
				{
					auto NewInstance = Inventory::GiveItem(Controller, *Definition, EFortQuickBars::Primary, 1, *Count, &bDidStack); // TODO: Figure out what quickbars are supposed to be used.
					Helper::EnablePickupAnimation(Pawn, Params->Pickup);

					if (bShouldSwap)
					{
						// Inventory::EquipInventoryItem(Controller, Inventory::GetItemGuid(NewInstance));
					}
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

					// Helper::DestroyActor(Params->Pickup);

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
			auto Controller = Pawn->Member<UObject*>(("Controller"));

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

		// Inventory::TakeItem(PlayerController, *FFortItemEntry::GetGuid(ItemEntry), AmountToRemove, true);

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

	if (Engine_Version >= 423)
		AddHook(("Function /Script/FortniteGame.FortPlayerController.ServerExecuteInventoryWeapon"), ServerExecuteInventoryWeaponHook);

	AddHook(("Function /Script/FortniteGame.FortPlayerController.ServerAttemptInventoryDrop"), ServerAttemptInventoryDropHook);
	AddHook(("Function /Script/FortniteGame.FortPlayerPawn.ServerHandlePickup"), ServerHandlePickupHook);

	if (std::stod(FN_Version) >= 7.40)
	{
		AddHook(("Function /Script/FortniteGame.FortPlayerPawn.ServerHandlePickupWithSwap"), ServerHandlePickupWithSwapHook);
	}

	if (HandleReloadCost)
	{
		MH_CreateHook((PVOID)HandleReloadCostAddr, HandleReloadCostDetour, (void**)&HandleReloadCost);
		MH_EnableHook((PVOID)HandleReloadCostAddr);
	}
}