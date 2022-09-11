#pragma once

#include "item.h"

namespace Player
{
    inline void TeleportToSkyDive(SkyDiveParameters TeleportToSkyDiveParams, UObject* const NewPawn)
    {
        static auto TeleportToSkyDiveFn = NewPawn->Function("TeleportToSkyDive");
        if (!TeleportToSkyDiveFn)
        {
            return;
        }

        NewPawn->ProcessEvent(TeleportToSkyDiveFn, &TeleportToSkyDiveParams);
    }

    // TODO: Set StructProperty /Script/FortniteGame.FortPlaylistAthena.RespawnHeight
    inline void Respawn(UObject* PlayerController)
    {
        const auto Pawn = *PlayerController->Member<UObject*>("Pawn");
        const auto PawnLocation = Helper::GetActorLocation(Pawn);

        if (static auto setHealthFn = Pawn->Function("SetHealth"))
        {
            HealthParameters HealthParams{100};
            Pawn->ProcessEvent(setHealthFn, &HealthParams);
        }

        const auto NewPawn = Helper::InitPawn(PlayerController, false, PawnLocation);
        if (NewPawn)
        {
            constexpr SkyDiveParameters TeleportToSkyDiveParams{10000};
            TeleportToSkyDive(TeleportToSkyDiveParams, NewPawn);
        }

        *NewPawn->Member<bool>("bIsDBNO") = false;
        if (static auto OnRepFn = NewPawn->Function("OnRep_IsDBNO"))
        {
            NewPawn->ProcessEvent(OnRepFn);
        }

        PlayerController->ProcessEvent("RespawnPlayer");
    }

    inline auto GetWorldInventory(UObject* Controller)
    {
        return *Controller->Member<UObject*>("WorldInventory");
    }

    inline auto GetInventory(UObject* Controller) -> long long*
    {
        const auto WorldInventory = GetWorldInventory(Controller);
        if (!WorldInventory)
        {
            return nullptr;
        }

        return WorldInventory->Member<long long>("Inventory");
    }

    inline auto GetItems(UObject* Controller) -> TArray<UObject*>*
    {
        static const long long ItemInstancesOffset = FindOffsetStruct(
            ("ScriptStruct /Script/FortniteGame.FortItemList"), "ItemInstances");
        const auto Inventory = GetInventory(Controller);
        if (!Inventory)
        {
            return nullptr;
        }

        return reinterpret_cast<TArray<UObject*>*>(reinterpret_cast<long long>(Inventory) + ItemInstancesOffset);
    }

    inline auto GetItemByGuid(UObject* Controller, FGuid Guid) -> UObject*
    {
        if (!Controller)
        {
            return nullptr;
        }


        const auto Function = Controller->Function("K2_GetInventoryItemWithGuid");
        if (!Function)
        {
            return nullptr;
        }

        return static_cast<UObject*>(Controller->ProcessEvent(Function, &Guid));
    }

    inline auto GetReplicatedEntries(UObject* Controller)
    {
        static const auto ReplicatedEntriesOffset = FindOffsetStruct("ScriptStruct /Script/FortniteGame.FortItemList",
                                                                     "ReplicatedEntries");
        auto Inventory = GetInventory(Controller);
        return reinterpret_cast<TArray<FFastArraySerializerItem>*>(reinterpret_cast<long long>(Inventory) +
            ReplicatedEntriesOffset);
    }

    inline auto AddToReplicatedEntries(UObject* Controller, UObject* FortItem)
    {
        if (!Controller || !FortItem)
        {
            return -1;
        }

        const auto ItemEntry = Item::GetEntry(FortItem);
        return ItemEntry ? GetReplicatedEntries(Controller)->Add(*ItemEntry) : -1;
    }

    inline auto RemoveGuidFromReplicatedEntries(UObject* Controller, const FGuid& Guid)
    {
        const auto ReplicatedEntries = GetReplicatedEntries(Controller);

        bool AnyMatch = false;

        for (int x = 0; x < ReplicatedEntries->Num(); x++)
        {
            auto ItemEntry = ReplicatedEntries->At(x);
            if (Item::GetGuid(reinterpret_cast<UObject*>(&ItemEntry)) != Guid)
            {
                continue;
            }
            
            ReplicatedEntries->RemoveAt(x);
            AnyMatch = true;
        }


        return AnyMatch;
    }

    template <typename Type>
    bool ChangeReplicatedItems(UObject* Controller, UObject* Definition, const std::string& Name,
                               Type NewVal, int Count = -1)
    {
        if (!Controller || !Definition)
        {
            return false;
        }


        const auto ReplicatedEntries = GetReplicatedEntries(Controller);
        for (int x = 0; x < ReplicatedEntries->Num(); x++)
        {
            auto ItemEntry = ReplicatedEntries->At(x);
            if (const auto EntryDefinition = Item::GetDefinition(reinterpret_cast<UObject*>(ItemEntry));
                EntryDefinition != Definition || Count != -1 && Item::GetCount(ItemEntry) != Count)
            {
                continue;
            }

            const auto Offset = FindOffsetStruct("ScriptStruct /Script/FortniteGame.FortItemEntry", Name);
            *reinterpret_cast<Type*>(reinterpret_cast<long long>(&ItemEntry) + Offset) = NewVal;

            const auto Inventory = GetInventory(Controller);
            MarkItemDirty(Inventory, &ItemEntry);

            return true;
        }

        return false;
    }
}
