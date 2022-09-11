#pragma once

#include <Gameplay/helper.h>
#include <Gameplay/inventory.h>

namespace Building
{
    inline bool CanBuild([[maybe_unused]] UObject* BuildingActor, [[maybe_unused]] bool Mirrored)
    {
        return true;
    }

    inline void PlaceBuildInternal(UObject* Controller, bool Mirrored, UObject* const BuildingActor)
    {
        if (!CanBuild(BuildingActor, Mirrored))
        {
            Helper::SetActorScale3D(BuildingActor, {});
            Helper::SilentDie(BuildingActor);
            return;
        }

        if (GameMode::UseDoubleBuildFix)
        {
            GameMode::Buildings.push_back(BuildingActor);
        }

        Helper::InitializeBuildingActor(Controller, BuildingActor, true);

        auto ResourceType = *BuildingActor->Member<TEnumAsByte<EFortResourceType>>("ResourceType");
        const auto MatDefinition = Item::GetMaterial(ResourceType.Get());
        if (!MatDefinition)
        {
            return;
        }

        const auto Definition = Item::GetDefinition(Inventory::FindItem(Controller, MatDefinition));
        if (*Item::GetCount(reinterpret_cast<FFastArraySerializerItem*>(Definition)) < 10)
        {
            return;
        }

        if (GameMode::IsPlayground())
        {
            return;
        }


        Inventory::Decrement(Controller, MatDefinition, 10);
    }

    inline bool ServerCreateBuildingActorHook(UObject* Controller, [[maybe_unused]] UFunction* Function,
                                              void* Parameters)
    {
        if (!Controller || !Parameters)
        {
            return false;
        }

        if (FortniteVersion <= 8)
        {
            const auto Params = static_cast<NewBuildingParametersOld*>(Parameters);
            const auto BuildingClass = Params->BuildingClassData.BuildingClass;
            if (!BuildingClass)
            {
                return false;
            }

            const auto BuildingActor = Easy::SpawnActor(BuildingClass, Params->BuildLoc, Params->BuildRot);
            if (!BuildingActor)
            {
                return false;
            }

            PlaceBuildInternal(Controller, Params->Mirrored, BuildingActor);
            return false;
        }

        const auto Params = static_cast<NewBuildingParameters*>(Parameters);
        static auto BroadcastRemoteClientInfoOffset = GetOffset(Controller, "BroadcastRemoteClientInfo");
        const auto RemoteClientInfo = reinterpret_cast<UObject**>(reinterpret_cast<long long>(Controller) +
            BroadcastRemoteClientInfoOffset);

        if (!RemoteClientInfo || !*RemoteClientInfo)
        {
            return false;
        }

        static auto RemoteBuildableClassOffset = GetOffset(*RemoteClientInfo, "RemoteBuildableClass");
        const auto BuildingClass = reinterpret_cast<UObject**>(reinterpret_cast<long long>(*RemoteClientInfo) +
            RemoteBuildableClassOffset);
        const auto BuildLoc = Params->CreateBuildingData.BuildLoc;
        const auto BuildRot = Params->CreateBuildingData.BuildRot;
        if (!BuildingClass || !*BuildingClass)
        {
            return false;
        }

        const auto BuildingActor = Easy::SpawnActor(*BuildingClass, BuildLoc, BuildRot);
        PlaceBuildInternal(Controller, Params->CreateBuildingData.Mirrored, BuildingActor);
        return false;
    }

    inline bool ServerBeginEditingBuildingActorHook(UObject* Controller, [[maybe_unused]] UFunction* Function,
                                                    void* Parameters)
    {
        const auto EditToolInstance = Inventory::FindItem(Controller, Item::EditTool);

        const auto Pawn = *Controller->Member<UObject*>("Pawn");

        const auto BuildingToEdit = static_cast<BeginEditingParameters*>(Parameters)->BuildingActor;
        if (!Controller || !BuildingToEdit)
        {
            return false;
        }

        if (!EditToolInstance)
        {
            return false;
        }

        const auto EditTool = Inventory::EquipWeaponDefinition(Pawn, Item::EditTool, Item::GetGuid(EditToolInstance));
        if (!EditTool)
        {
            return false;
        }

        *BuildingToEdit->Member<UObject*>("EditingPlayer") = *Controller->Member<UObject*>("PlayerState");
        if (static auto OnRepEditingPlayer = BuildingToEdit->Function("OnRep_EditingPlayer"))
        {
            BuildingToEdit->ProcessEvent(OnRepEditingPlayer);
        }

        *EditTool->Member<UObject*>(("EditActor")) = BuildingToEdit;
        if (static auto OnRepEditActor = EditTool->Function("OnRep_EditActor"))
        {
            EditTool->ProcessEvent(OnRepEditActor);
        }

        return false;
    }

    inline auto GetBuildingReplacementAddress()
    {
        auto BuildingSmActorReplaceBuildingActorAddress = FindPattern(
            "4C 8B DC 55 57 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 8B 85 ? ? ? ? 33 FF 40 38 3D ? ? ? ?");
        if (BuildingSmActorReplaceBuildingActorAddress)
        {
            return BuildingSmActorReplaceBuildingActorAddress;
        }

        if (EngineVersion >= 426)
        {
            BuildingSmActorReplaceBuildingActorAddress = FindPattern(
                "48 8B C4 48 89 58 18 55 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 70 B8 0F 29 78 A8 44 0F 29 40 ? 44 0F 29 48 ? 44 0F 29 90 ? ? ? ? 44 0F 29 B8 ? ? ? ? 48 8B 05");
            if (BuildingSmActorReplaceBuildingActorAddress)
            {
                return BuildingSmActorReplaceBuildingActorAddress;
            }
        }

        if (EngineVersion <= 421)
        {
            BuildingSmActorReplaceBuildingActorAddress = FindPattern(
                "48 8B C4 44 89 48 20 55 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 48 89 70 E8 33 FF 40 38 3D ? ? ? ? 48 8B F1 4C 89 60 E0 44 8B E2");
        }

        return BuildingSmActorReplaceBuildingActorAddress;
    }

    inline bool ServerEditBuildingActorHook(UObject* Controller, UFunction* Function, void* Parameters)
    {
        const auto Params = static_cast<EditingParameters*>(Parameters);
        if (!Params || !Controller)
        {
            return false;
        }

        const auto BuildingActor = Params->BuildingActorToEdit;
        const auto NewBuildingClass = Params->NewBuildingClass;
        static auto bMirroredOffset = FindOffsetStruct(
            "Function /Script/FortniteGame.FortPlayerController.ServerEditBuildingActor", "bMirrored");
        const auto Mirrored = *reinterpret_cast<bool*>(reinterpret_cast<long long>(Parameters) + bMirroredOffset);
        static auto RotationIterationsOffset = FindOffsetStruct(
            "Function /Script/FortniteGame.FortPlayerController.ServerEditBuildingActor", "RotationIterations");
        const auto RotationIterations = *reinterpret_cast<int*>(reinterpret_cast<long long>(Parameters) +
            RotationIterationsOffset);

        if (!BuildingActor || !NewBuildingClass)
        {
            return false;
        }

        const auto BitField = Params->BuildingActorToEdit->Member<IsDestroyedBitField>("bDestroyed");
        if (!BitField || BitField->Destroyed || RotationIterations > 3)
        {
            return false;
        }

        const auto BuildingActorAddress = GetBuildingReplacementAddress();

        auto BuildingSmActorReplaceBuildingActor = decltype(BuildingSmActorReplaceBuildingActor)(BuildingActorAddress);

        if (BuildingSmActorReplaceBuildingActor)
        {
            return false;
        }

        BuildingSmActorReplaceBuildingActor(BuildingActor, 1, NewBuildingClass, 0, RotationIterations, Mirrored,
                                            Controller);
        return false;
    }

    inline void OnRepEditActor(UObject** const CurrentWep)
    {
        if (const auto CurrentWepItemDef = *(*CurrentWep)->Member<UObject*>("WeaponData"); CurrentWepItemDef != Item::EditTool)
        {
            return;   
        }
        
        const auto EditTool = *CurrentWep;
        *EditTool->Member<bool>("bEditConfirmed") = true;
        *EditTool->Member<UObject*>("EditActor") = nullptr;

        static auto OnRepEditActorFn = EditTool->Function("OnRep_EditActor");
        if (!OnRepEditActorFn)
        {
            return;
        }

        EditTool->ProcessEvent(OnRepEditActorFn);
    }

    inline void OnCancelEditing(const EndEditingParameters* Params)
    {
        *Params->BuildingActorToStopEditing->Member<UObject*>("EditingPlayer") = nullptr;

        static auto OnRepEditingPlayer = Params->BuildingActorToStopEditing->Function("OnRep_EditingPlayer");
        if(!OnRepEditingPlayer)
        {
            return;
        }
        
        Params->BuildingActorToStopEditing->ProcessEvent(OnRepEditingPlayer);
    }

    // TODO: Check if the controller is in aircraft, if they edit on spawn island, it will make them end on the battle bus, which will not go well.
    inline bool ServerEndEditingBuildingActorHook(UObject* Controller, [[maybe_unused]] UFunction* Function, void* Parameters)
    {
        if (!Controller || !Parameters || Helper::IsInAircraft(Controller))
        {
            return false;
        }
        
        const auto Params = static_cast<EndEditingParameters*>(Parameters);
        const auto Pawn = Controller->Member<UObject*>("Pawn");
        if (!Pawn || !*Pawn)
        {
            return false;
        }

        if (const auto CurrentWep = (*Pawn)->Member<UObject*>("CurrentWeapon"); CurrentWep && *CurrentWep)
        {
            OnRepEditActor(CurrentWep);
        }

        if (Params->BuildingActorToStopEditing)
        {
            OnCancelEditing(Params);
        }
        
        return false;
    }

    inline void InitHooks()
    {
        Hooks::Add("Function /Script/FortniteGame.FortPlayerController.ServerCreateBuildingActor",
                ServerCreateBuildingActorHook);
        Hooks::Add("Function /Script/FortniteGame.FortPlayerController.ServerBeginEditingBuildingActor",
                ServerBeginEditingBuildingActorHook);
        Hooks::Add("Function /Script/FortniteGame.FortPlayerController.ServerEditBuildingActor",
                ServerEditBuildingActorHook);
        Hooks::Add("Function /Script/FortniteGame.FortPlayerController.ServerEndEditingBuildingActor",
                ServerEndEditingBuildingActorHook);
    }
}
