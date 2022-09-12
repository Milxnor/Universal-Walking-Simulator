#pragma once

#include <ranges>
#include <UE/structs.h>
#include <Gameplay/helper.h>
#include <Gameplay/ability.h>

#include "gamemode.h"
#include "item.h"
#include "ltm.h"
#include "player.h"

namespace Inventory
{
    inline UObject* EquipWeapon(UObject* Pawn, UObject* Definition, const FGuid& Guid, const FGuid& TrackerGuid)
    {
        static const auto EquipFunction = Pawn->Function("EquipWeaponDefinition");
        if (!EquipFunction)
        {
            return nullptr;
        }

        if (FortniteVersion < 16.00)
        {
            EquipWeaponParametersOld Params{Definition, Guid, nullptr};
            Pawn->ProcessEvent(EquipFunction, &Params);
            return Definition;
        }


        EquipWeaponParameters params{Definition, Guid, TrackerGuid, nullptr};
        Pawn->ProcessEvent(EquipFunction, &params);
        return Definition;
    }

    inline UObject* EquipWeaponDefinition(UObject* Pawn, UObject* Definition, const FGuid& Guid, int Ammo = 0,
                                          const FGuid& TrackerGuid = FGuid())
    {
        if (!Pawn || !Definition)
        {
            return nullptr;
        }

        if (!Item::IsGadget(Definition))
        {
            const auto Equipped = EquipWeapon(Pawn, Definition, Guid, TrackerGuid);
            Helper::SetOwner(Equipped, Pawn);
            Equipped->ProcessEvent("OnRep_ReplicatedWeaponData");
            return Equipped;
        }

        static auto GetItemDefinition = Definition->Function("GetWeaponItemDefinition")
                                            ? Definition->Function("GetWeaponItemDefinition")
                                            : Definition->Function("GetDecoItemDefinition");
        if (!GetItemDefinition)
        {
            return nullptr;
        }

        UObject* Result = nullptr;
        Definition->ProcessEvent(GetItemDefinition, &Result);
        if (!Result)
        {
            return nullptr;
        }

        return EquipWeaponDefinition(Pawn, Result, Guid);
    }

    inline auto GetTrackerGuid(UObject* const CurrentItemInstance)
    {
        FGuid TrackerGuid;
        if (FortniteVersion >= 16.00)
        {
            if (static auto GetTrackerGuid = CurrentItemInstance->Function("GetTrackerGuid"))
            {
                CurrentItemInstance->ProcessEvent(GetTrackerGuid, &TrackerGuid);
            }
        }
        return TrackerGuid;
    }

    inline void EquipInventoryItem(UObject* Controller, const FGuid& Guid)
    {
        if (!Controller)
        {
            return;
        }

        const auto Pawn = Helper::GetPawnFromController(Controller);
        if (!Pawn)
        {
            return;
        }

        const auto CurrentItemInstance = Player::GetItemByGuid(Controller, Guid);
        if (!CurrentItemInstance)
        {
            return;
        }

        const auto CurrentItemDefinition = Item::GetDefinition(CurrentItemInstance);
        const auto TrackerGuid = GetTrackerGuid(CurrentItemInstance);
        const auto Ammo = Item::GetLoadedAmmo(CurrentItemInstance);
        EquipWeaponDefinition(Pawn, CurrentItemDefinition, Guid, Ammo ? *Ammo : 0, TrackerGuid);
    }

    inline void Update(UObject* Controller, const int Index = -1, const bool RemovedItem = false,
                       FFastArraySerializerItem* ModifiedItem = nullptr)
    {
        const auto WorldInventory = Player::GetWorldInventory(Controller);
        const auto Inventory = Player::GetInventory(Controller);

        if (static auto WorldHandleInvUpdate = WorldInventory->Function("HandleInventoryLocalUpdate"))
        {
            WorldInventory->ProcessEvent(WorldHandleInvUpdate);
        }

        // TODO: Check this function
        if (std::floor(FortniteVersion) < 10)
        {
            if (static auto HandleWorldInventoryLocalUpdate = Controller->Function("HandleWorldInventoryLocalUpdate"))
            {
                Controller->ProcessEvent(HandleWorldInventoryLocalUpdate);
            }
        }

        if (FortniteVersion < 7.4)
        {
            if (const auto QuickBars = *Controller->Member<UObject*>("QuickBars"))
            {
                static const auto OnRepPrimaryQuickBar = QuickBars->Function("OnRep_PrimaryQuickBar");
                QuickBars->ProcessEvent(OnRepPrimaryQuickBar);
                static const auto OnRepSecondaryQuickBar = QuickBars->Function("OnRep_SecondaryQuickBar");
                QuickBars->ProcessEvent(OnRepSecondaryQuickBar);
            }
        }
        else
        {
            static const auto ClientForceUpdateQuickBar = Controller->Function("ClientForceUpdateQuickbar");

            auto PrimaryParameters = EFortQuickBars::Primary;
            Controller->ProcessEvent(ClientForceUpdateQuickBar, &PrimaryParameters);

            auto SecondaryParameters = EFortQuickBars::Secondary;
            Controller->ProcessEvent(ClientForceUpdateQuickBar, &SecondaryParameters);

            if (FortniteVersion < 8)
            {
                if (static const auto UpdateQuickBars = Controller->Function("UpdateQuickBars"))
                {
                    Controller->ProcessEvent(UpdateQuickBars);
                }
            }
        }

        if ((RemovedItem || Index != -1) && Inventory)
        {
            MarkArrayDirty(Inventory);
        }


        if (ModifiedItem)
        {
            MarkItemDirty(Inventory, ModifiedItem);
        }
    }

    inline auto CreateAndAddItem(UObject* Controller, UObject* Definition, const EFortQuickBars Bars, const int Slot,
                                 const int Count = 1) -> UObject*
    {
        if (!Controller || !Definition)
        {
            return nullptr;
        }

        CreateTempItemParameters Params{Count, 1, nullptr};
        if (static auto Function = Definition->Function("CreateTemporaryItemInstanceBP"))
        {
            Definition->ProcessEvent(Function, &Params);
        }

        const auto ItemInstance = Params.Instance;
        if (!ItemInstance)
        {
            return nullptr;
        }

        static const auto SetFunction = ItemInstance->Function("SetOwningControllerForTemporaryItem");
        ItemInstance->ProcessEvent(SetFunction, &Controller);

        static auto CountOffset = FindOffsetStruct("ScriptStruct /Script/FortniteGame.FortItemEntry", "Count");
        const auto GunCount = reinterpret_cast<int*>(Item::GetEntry(ItemInstance) + CountOffset);
        *GunCount = Count;

        *ItemInstance->Member<UObject*>("OwnerInventory") = *Controller->Member<UObject*>("WorldInventory");


        if (EngineVersion < 424)
        {
            Player::GetItems(Controller)->Add(ItemInstance);
        }

        if (FortniteVersion < 7.4)
        {
            if (const auto QuickBars = Controller->Member<UObject*>("QuickBars"); QuickBars)
            {
                if (static auto Function = (*QuickBars)->Function(("ServerAddItemInternal")))
                {
                    AddItemInternalParameters Parameters{Item::GetGuid(ItemInstance), Bars, Slot};
                    (*QuickBars)->ProcessEvent(Function, &Parameters);
                }
            }
        }

        if (const auto Index = Player::AddToReplicatedEntries(Controller, ItemInstance); Index != -1)
        {
            Update(Controller, -1, false, reinterpret_cast<FFastArraySerializerItem*>(ItemInstance));
        }

        return ItemInstance;
    }

    inline auto FindItem(UObject* Controller, const UObject* Definition) -> UObject*
    {
        const auto ItemInstances = *Player::GetItems(Controller);
        for (int i = 0; i < ItemInstances.Num(); i++)
        {
            const auto CurrentItemInstance = ItemInstances.At(i);
            if (!CurrentItemInstance)
            {
                continue;
            }

            if (Item::GetDefinition(CurrentItemInstance) != Definition)
            {
                continue;
            }

            return CurrentItemInstance;
        }

        return nullptr;
    }

    inline auto RemoveItem(UObject* Controller, const FGuid& Guid)
    {
        const auto ItemInstances = Player::GetItems(Controller);
        for (int j = 0; j < ItemInstances->Num(); j++)
        {
            const auto ItemInstance = ItemInstances->At(j);
            if (!ItemInstance)
            {
                continue;
            }

            if (Item::GetGuid(ItemInstance) != Guid)
            {
                continue;
            }

            ItemInstances->RemoveAt(j);
        }

        const auto Result = Player::RemoveGuidFromReplicatedEntries(Controller, Guid);
        if (Result)
        {
            Update(Controller, -1, true);
        }

        return Result;
    }

    // FIXME: For now, assume it's a consumable or ammo, and it can go higher than what we currently have.
    inline bool Increment(UObject* Controller, UObject* Instance, int Count)
    {
        if (!Controller || !Instance)
        {
            return false;
        }

        const auto ItemEntry = Item::GetEntry(Instance);
        const auto CurrentCount = Item::GetCount(ItemEntry);

        const auto NewCount = *CurrentCount + Count;
        const auto OldCount = *CurrentCount;
        *CurrentCount = NewCount;

        Player::ChangeReplicatedItems(Controller, Item::GetDefinition(Instance), "Count", NewCount, OldCount);
        Update(Controller, -1, false, reinterpret_cast<FFastArraySerializerItem*>(ItemEntry));
        return true;
    }

    inline auto Decrement(UObject* Controller, UObject* Definition, const int Count) -> UObject*
    {
        if (!Controller || !Definition)
        {
            return nullptr;
        }

        const auto ItemInstances = Player::GetItems(Controller);
        for (int j = 0; j < ItemInstances->Num(); j++)
        {
            const auto ItemInstance = ItemInstances->At(j);
            if (!ItemInstance)
            {
                continue;
            }

            if (const auto CurrentDefinition = Item::GetDefinition(ItemInstance); CurrentDefinition != Definition)
            {
                continue;
            }

            const auto ItemEntry = Item::GetEntry(ItemInstance);
            const auto CurrentCount = Item::GetCount(ItemEntry);

            auto NewCount = *CurrentCount - Count;
            if (NewCount < 0)
            {
                NewCount = 0;
            }

            *CurrentCount = NewCount;

            Player::ChangeReplicatedItems(Controller, Definition, "Count", NewCount);
            Update(Controller, -1, false, reinterpret_cast<FFastArraySerializerItem*>(ItemEntry));
            return ItemEntry;
        }

        return nullptr;
    }

    inline auto TakeItem(UObject* Controller, const FGuid& Guid, const int Count = 1,
                         const bool DestroyIfEmpty = false) -> UObject*
    {
        if (!Controller)
        {
            return nullptr;
        }

        const auto Removed = Decrement(Controller, Player::GetItemByGuid(Controller, Guid), Count);
        if (!Removed)
        {
            RemoveItem(Controller, Guid);
            return nullptr;
        }

        if (*Item::GetCount(Removed) != 0 || !DestroyIfEmpty)
        {
            return Item::GetDefinition(Removed);
        }

        RemoveItem(Controller, Guid);
        return Item::GetDefinition(Removed);
    }

    inline auto GetOverStack(UObject* Controller, UObject* Definition,
                             const int Count) -> std::tuple<void*, int>
    {
        const auto ItemInstances = Player::GetItems(Controller);
        if (!ItemInstances)
        {
            return {nullptr, 0};
        }

        std::vector<UObject*> InstancesOfItem;
        std::ranges::copy_if(*ItemInstances,
                             std::back_inserter(InstancesOfItem),
                             [Definition](auto Entry) { return Entry && Item::GetDefinition(Entry) == Definition; });
        if (InstancesOfItem.empty())
        {
            return {nullptr, 0};
        }

        const auto MaxStackCount = *Definition->Member<int>("MaxStackSize");
        const auto ItemInstance = std::ranges::find_if(InstancesOfItem, [MaxStackCount](auto Entry)
        {
            const auto EntryCount = Item::GetCount(Item::GetEntry(Entry));
            return EntryCount && *EntryCount < MaxStackCount;
        });

        if (ItemInstance == InstancesOfItem.end())
        {
            return {nullptr, 0};
        }

        const auto ItemEntry = Item::GetEntry(*ItemInstance);
        if (!ItemEntry)
        {
            return {nullptr, 0};
        }

        const auto CurrentCount = Item::GetCount(reinterpret_cast<long long*>(ItemEntry));
        if (!CurrentCount)
        {
            return {ItemEntry, 0};
        }

        const auto OverStack = *CurrentCount + Count - MaxStackCount;
        const auto AmountToStack = OverStack > 0 ? Count - OverStack : Count;
        Increment(Controller, *ItemInstance, AmountToStack);
        return {ItemEntry, OverStack};
    }

    static UObject* GiveItem(UObject* Controller, UObject* Definition, const EFortQuickBars Bars, const int Slot,
                             const int Count = 1)
    {
        if (!Controller || !Definition)
        {
            return nullptr;
        }

        const auto [ItemDefinition, OverStack] = GetOverStack(Controller, Definition, Count);
        if (OverStack <= 0)
        {
            return nullptr;
        }

        if (const auto FullName = Definition->GetFullName(); FullName.contains("CarminePack") || FullName.contains(
            "AshtonRockItemDef"))
        {
            Ltm::Thanos::HandleGauntlet(Controller);
            UObject* item = CreateAndAddItem(
                Controller, FindObject(
                    "/Game/Athena/Items/Gameplay/BackPacks/CarminePack/AGID_CarminePack.AGID_CarminePack"), Bars, Slot,
                OverStack);
            EquipInventoryItem(Controller, Item::GetGuid(item));
        }

        return ItemDefinition && (OverStack <= 0 || Item::IsResource(Definition))
                   ? nullptr
                   : CreateAndAddItem(Controller, Definition, Bars, Slot, OverStack <= 0 ? OverStack : 1);
    }

    inline bool ServerExecuteInventoryItemHook(UObject* Controller, [[maybe_unused]] UFunction* Function,
                                               void* Parameters)
    {
        EquipInventoryItem(Controller, *static_cast<FGuid*>(Parameters));
        return false;
    }

    inline bool ServerExecuteInventoryWeaponHook(UObject* Controller, [[maybe_unused]] UFunction* Function,
                                                 void* Parameters)
    {
        const auto Weapon = static_cast<UObject**>(Parameters);
        if (!Weapon)
        {
            return false;
        }

        const auto Pawn = Helper::GetPawnFromController(Controller);
        const auto Guid = *(*Weapon)->Member<FGuid>("ItemEntryGuid");
        const auto Def = *(*Weapon)->Member<UObject*>("WeaponData");
        if (!Def)
        {
            return false;
        }

        EquipWeaponDefinition(Pawn, Def, Guid, 0);
        return false;
    }

    inline bool ServerAttemptInventoryDropHook(UObject* Controller, [[maybe_unused]] UFunction* Function,
                                               void* Parameters)
    {
        if (!Controller || !Parameters || Helper::IsInAircraft(Controller))
        {
            return false;
        }

        const auto Params = static_cast<ServerAttemptInventoryDropParameters*>(Parameters);
        const auto Definition = TakeItem(Controller, Params->ItemGuid, Params->Count, true);
        if (const auto Pawn = Helper::GetPawnFromController(Controller))
        {
            const auto Location = Helper::GetActorLocation(Pawn);
            Helper::SummonPickup(Pawn, Definition, Location, EFortPickupSourceTypeFlag::Player,
                                 EFortPickupSpawnSource::Unset, Params->Count);
        }

        return false;
    }

    inline auto ShouldSwap(UObject** const Definition, TArray<UObject*>* const ItemInstances)
    {
        if (Item::GetQuickBar(*Definition) == EFortQuickBars::Secondary)
        {
            return false;
        }
        
        auto PrimaryQuickBarSlotsFilled = 0;
        for (int i = 0; i < ItemInstances->Num(); i++)
        {
            auto ItemInstance = ItemInstances->At(i);

            if (!ItemInstance)
            {
                break;
            }
            
            if (Item::GetQuickBar(Item::GetDefinition(ItemInstance)) != EFortQuickBars::Primary)
            {
                continue;
            }

            PrimaryQuickBarSlotsFilled++;
        }

        return PrimaryQuickBarSlotsFilled - 6 >= 5;
    }

    // TODO: For >7.40 check all quickbar slots until we find a empty one.
    // TODO: Figure out what quickbars are supposed to be used.
    inline bool ServerHandlePickupHook(UObject* Pawn, [[maybe_unused]] UFunction* Function, void* Parameters)
    {
        if (!Pawn || !Parameters || GameMode::IsRunningEvent)
        {
            return false;
        }

        const auto Params = static_cast<ServerHandlePickupParameters*>(Parameters);
        if (!Params->Pickup || !Pawn)
        {
            return false;
        }

        const auto PickedUp = Params->Pickup->Member<bool>("bPickedUp");
        const auto Controller = *Pawn->Member<UObject*>(("Controller"));

        if (!PickedUp || *PickedUp || !Controller)
        {
            return false;
        }
       
        const auto PrimaryPickupItemEntry = Params->Pickup->Member<long long>("PrimaryPickupItemEntry");
        static const auto ItemDefinitionOffset = FindOffsetStruct("ScriptStruct /Script/FortniteGame.FortItemEntry", "ItemDefinition");
        static const auto CountOffset = FindOffsetStruct("ScriptStruct /Script/FortniteGame.FortItemEntry", "Count");

        const auto Definition = reinterpret_cast<UObject**>(reinterpret_cast<long long>(&*PrimaryPickupItemEntry) + ItemDefinitionOffset);
        const auto Count = reinterpret_cast<int*>(reinterpret_cast<long long>(&*PrimaryPickupItemEntry) + CountOffset);

        const auto ItemInstances = Player::GetItems(Controller);

        const auto Swap = ShouldSwap(Definition, ItemInstances);
        const auto CurrentWeapon = *Pawn->Member<UObject*>("CurrentWeapon");
        const auto HeldWeaponDef = *CurrentWeapon->Member<UObject*>("WeaponData");

        auto DidStack = false;
        if (Definition && *Definition && Count)
        {
            DidStack = GiveItem(Controller, *Definition, EFortQuickBars::Primary, 1, *Count);
            Helper::EnablePickupAnimation(Pawn, Params->Pickup, 0.75f, Params->InStartDirection);
        }

        auto SucceededSwap = true;
        if (Swap && !DidStack)
        {
            SucceededSwap = HeldWeaponDef != Item::Pickaxe;
            if (SucceededSwap) 
            {
                RemoveItem(Controller, *CurrentWeapon->Member<FGuid>("ItemEntryGuid"));
                const auto Loc = Helper::GetActorLocation(Pawn);
                Helper::SummonPickup(Pawn, HeldWeaponDef, Loc,
                                                          EFortPickupSourceTypeFlag::Player,EFortPickupSpawnSource::Unset, 1);
            }	
        }

        if (SucceededSwap)
        {
            *PickedUp = true;
            if (static auto bPickedUpFn = Params->Pickup->Function("OnRep_bPickedUp")){
                Params->Pickup->ProcessEvent(bPickedUpFn);
            }
        }
        
        return false;
    }

    inline bool ServerHandlePickupWithSwapHook(UObject* Pawn, [[maybe_unused]] UFunction* Function, void* Parameters)
    {
        const auto Params = static_cast<ServerHandlePickupWithSwapParameters*>(Parameters);
        if(!Params || !Params->Pickup)
        {
            return false;
        }

        if (const auto PickedUp = Params->Pickup->Member<bool>("bPickedUp"); !PickedUp || *PickedUp)
        {
            return false; 
        }

        const auto Controller = Pawn->Member<UObject*>("Controller");

        ClientDropParameters DropParameters{Params->Swap, 1};
        ServerAttemptInventoryDropHook(*Controller, nullptr, &DropParameters);

        ClientPickupParameters PickupParameters{Params->Pickup, Params->InFlyTime, Params->InStartDirection, Params->PlayPickupSound};
        ServerHandlePickupHook(Pawn, nullptr, &PickupParameters);
        
        return false;
    }

    inline void __fastcall HandleReloadCostDetour(UObject* Weapon, int AmountToRemove)
    {
        if (!Weapon || GameMode::IsPlayground())
        {
            return;
        }

        static const auto GetOwner = Weapon->Function("GetOwner");
        UObject* Pawn;
        Weapon->ProcessEvent(GetOwner, &Pawn);
        if (!Pawn)
        {
            return;
        }

        const auto PlayerController = *Pawn->Member<UObject*>("Controller");
        if (!PlayerController)
        {
            return;
        }

        const auto WeaponData = *Weapon->Member<UObject*>("WeaponData");
        UObject* AmmoDef = nullptr;
        if (static const auto GetAmmoWorldItemDefinitionBp = WeaponData->Function("GetAmmoWorldItemDefinition_BP"))
        {
            WeaponData->ProcessEvent(GetAmmoWorldItemDefinitionBp, &AmmoDef);
        }

        if (!AmmoDef || WeaponData->GetName().contains("TID"))
        {
            AmmoDef = WeaponData;
        }

        if (const auto ItemEntry = Decrement(PlayerController, AmmoDef, AmountToRemove))
        {
            if (*Item::GetCount(ItemEntry) <= 0) 
            {
                RemoveItem(PlayerController, Item::GetGuid(reinterpret_cast<UObject*>(ItemEntry)));
            }
        }

        return HandleReloadCost(Weapon, AmountToRemove);
    }

    inline void InitHooks()
    {
        Hooks::Add("Function /Script/FortniteGame.FortPlayerController.ServerExecuteInventoryItem", ServerExecuteInventoryItemHook);

        if (EngineVersion >= 423 && FortniteVersion < 16.00)
        {
            Hooks::Add("Function /Script/FortniteGame.FortPlayerController.ServerExecuteInventoryWeapon", ServerExecuteInventoryWeaponHook);
        }

        Hooks::Add("Function /Script/FortniteGame.FortPlayerController.ServerAttemptInventoryDrop", ServerAttemptInventoryDropHook);
        Hooks::Add("Function /Script/FortniteGame.FortPlayerPawn.ServerHandlePickup", ServerHandlePickupHook);

        if (FortniteVersion >= 7.40)
        {
            Hooks::Add("Function /Script/FortniteGame.FortPlayerPawn.ServerHandlePickupWithSwap", ServerHandlePickupWithSwapHook);
        }

        if (HandleReloadCost)
        {
            MH_CreateHook(reinterpret_cast<PVOID>(HandleReloadCostAddress), reinterpret_cast<LPVOID>(HandleReloadCostDetour), reinterpret_cast<void**>(&HandleReloadCost));
            MH_EnableHook(reinterpret_cast<PVOID>(HandleReloadCostAddress));
        }
    }

    inline void Clear(UObject* Controller, const bool TakePickaxe = false)
    {
        if (const auto ItemInstances = Player::GetItems(Controller))
        {
            for (int i = 0; i < ItemInstances->Num(); i++)
            {
                const auto CurrentItemInstance = ItemInstances->At(i);
                if (!CurrentItemInstance
                    || CurrentItemInstance->IsA(Item::WallBuild)
                    || CurrentItemInstance->IsA(Item::FloorBuild)
                    || CurrentItemInstance->IsA(Item::StairBuild)
                    || CurrentItemInstance->IsA(Item::RoofBuild)
                    || !TakePickaxe
                    && CurrentItemInstance == Item::Pickaxe)
                {
                    continue;
                }


                const auto Guid = Item::GetGuid(CurrentItemInstance);
                const auto Count = *Item::GetCount(Item::GetEntry(CurrentItemInstance));
                TakeItem(Controller, Guid, Count, true);
            }
        }

        if (const auto WoodInstance = FindItem(Controller, Item::WoodMaterial))
        {
            TakeItem(Controller, Item::GetGuid(WoodInstance), *Item::GetCount(WoodInstance), true);
        }

        if (const auto StoneInstance = FindItem(Controller, Item::StoneMaterial))
        {
            TakeItem(Controller, Item::GetGuid(StoneInstance), *Item::GetCount(StoneInstance), true);
        }

        if (const auto MetalInstance = FindItem(Controller, Item::MetalMaterial))
        {
            TakeItem(Controller, Item::GetGuid(MetalInstance), *Item::GetCount(MetalInstance),true);   
        }
    }

    inline void GiveBattleRoyalItems(UObject* Controller)
    {
        CreateAndAddItem(Controller, Item::Pickaxe, EFortQuickBars::Primary, 0, 1);
        CreateAndAddItem(Controller, Item::WallBuild, EFortQuickBars::Secondary, 0, 1);
        CreateAndAddItem(Controller, Item::FloorBuild, EFortQuickBars::Secondary, 1, 1);
        CreateAndAddItem(Controller, Item::StairBuild, EFortQuickBars::Secondary, 2, 1);
        CreateAndAddItem(Controller, Item::RoofBuild, EFortQuickBars::Secondary, 3, 1);
        CreateAndAddItem(Controller, Item::EditTool, EFortQuickBars::Primary, 0, 1);
    }

    inline void GiveAllAmmo(UObject* Controller)
	{

        CreateAndAddItem(Controller, Item::RocketBullet, EFortQuickBars::Secondary, 0, 999);
        CreateAndAddItem(Controller, Item::ShellBullet, EFortQuickBars::Secondary, 0, 999);
        CreateAndAddItem(Controller, Item::MediumBullet, EFortQuickBars::Secondary, 0, 999);
        CreateAndAddItem(Controller, Item::LightBullet, EFortQuickBars::Secondary, 0, 999);
        CreateAndAddItem(Controller, Item::HeavyBullet, EFortQuickBars::Secondary, 0, 999);
        CreateAndAddItem(Controller, Item::GrapplerBullet, EFortQuickBars::Secondary, 0, 999);
	}

    inline void GiveMaxMats(UObject* Controller)
	{
		CreateAndAddItem(Controller, Item::WoodMaterial, EFortQuickBars::Secondary, 0, Item::MaxMaterials);
		CreateAndAddItem(Controller, Item::StoneMaterial, EFortQuickBars::Secondary, 0, Item::MaxMaterials);
		CreateAndAddItem(Controller, Item::MetalMaterial, EFortQuickBars::Secondary, 0, Item::MaxMaterials);
	}

}
