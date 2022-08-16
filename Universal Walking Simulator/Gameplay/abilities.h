#pragma once

#include <UE/structs.h>
#include <Gameplay/helper.h>
#include "Net/funcs.h"

FGameplayAbilitySpec<FGameplayAbilityActivationInfo>* FindAbilitySpecFromHandle(UObject* ASC, FGameplayAbilitySpecHandle Handle)
{
    TArray<FGameplayAbilitySpec<FGameplayAbilityActivationInfo>> Specs;

    if (Engine_Version <= 422)
        Specs = (*ASC->Member<FGameplayAbilitySpecContainerOL>(("ActivatableAbilities"))).Items;
    else
        Specs = (*ASC->Member<FGameplayAbilitySpecContainerSE>(("ActivatableAbilities"))).Items;

    for (int i = 0; i < Specs.Num(); i++)
    {
        auto& Spec = Specs[i];

        if (Spec.Handle.Handle == Handle.Handle)
        {
            return &Spec;
        }
    }

    return nullptr;
}

FGameplayAbilitySpec<FGameplayAbilityActivationInfoFTS>* FindAbilitySpecFromHandleFTS(UObject* ASC, FGameplayAbilitySpecHandle Handle)
{
    TArray<FGameplayAbilitySpec<FGameplayAbilityActivationInfoFTS>> Specs;

    Specs = (*ASC->Member<FGameplayAbilitySpecContainerFTS>(("ActivatableAbilities"))).Items;

    for (int i = 0; i < Specs.Num(); i++)
    {
        auto& Spec = Specs[i];

        if (Spec.Handle.Handle == Handle.Handle)
        {
            return &Spec;
        }
    }

    return nullptr;
}


/* static FAbilityReplicatedDataCache* FindReplicatedTargetData(UObject* AbilitySystemComponent, FGameplayAbilitySpecHandle Handle, FPredictionKey PredictionKey)
{
    FAbilityReplicatedDataCache OutData;
    auto ReplicatedDataContainer = (FGameplayAbilityReplicatedDataContainer*)(__int64(AbilitySystemComponent) + 1312); // Found at uhm serverseetreplicaqteddata

    if (ReplicatedDataContainer)
        return ReplicatedDataContainer->Find(FGameplayAbilitySpecHandleAndPredictionKey{ Handle, PredictionKey.Current }).Get();

    return nullptr;
}

void ConsumeAllReplicatedData(UObject* AbilitySystemComponent, FGameplayAbilitySpecHandle AbilityHandle, FPredictionKey AbilityOriginalPredictionKey)
{
    // TSharedPtr<FAbilityReplicatedDataCache> CachedData = AbilityTargetDataMap.Find(FGameplayAbilitySpecHandleAndPredictionKey(AbilityHandle, AbilityOriginalPredictionKey));
    auto CachedData = FindReplicatedTargetData(AbilitySystemComponent, AbilityHandle, AbilityOriginalPredictionKey);
    // if (CachedData.IsValid())
    if (CachedData)
    {
        CachedData->Reset();
    }
} */

// https://github.com/EpicGames/UnrealEngine/blob/46544fa5e0aa9e6740c19b44b0628b72e7bbd5ce/Engine/Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Private/AbilitySystemComponent_Abilities.cpp#L1754

void PrintExplicitTags(UObject* ASC)
{
    static auto AHAHA = 264;
    auto MinimalReplicationGameplayCuesOff = GetOffset(ASC, (("MinimalReplicationGameplayCues")));
    auto BlockedAbilityTags = (__int64(ASC) + (MinimalReplicationGameplayCuesOff + 0xD0));
    auto ExplicitTags = *(FGameplayTagContainer*)(BlockedAbilityTags + AHAHA);
    std::cout << "\n\nExplicit Tags: " << ExplicitTags.ToStringSimple(true) << "\n\n\n";
}

int16_t* GetCurrent(FPredictionKey* Key)
{
    if (!Key)
        return nullptr;

    if (Engine_Version < 426)
        return &Key->Current;
    else
        return &((FPredictionKeyFTS*)Key)->Current;
}

UObject** GetAbilityFromSpec(void* Spec)
{
    if (!Spec)
        return nullptr;

    if (Engine_Version < 426)
        return &((FGameplayAbilitySpec<FGameplayAbilityActivationInfo>*)Spec)->Ability;
    else
        return &((FGameplayAbilitySpec<FGameplayAbilityActivationInfoFTS>*)Spec)->Ability;
}

void InternalServerTryActivateAbility(UObject* ASC, FGameplayAbilitySpecHandle Handle, bool InputPressed, FPredictionKey* PredictionKey, __int64* TriggerEventData, bool bConsumeData = false)
{
    if (!PredictionKey)
    {
        std::cout << ("InternalServerTryActivateAbility. Rejecting ClientActivation of ability with invalid PredictionKey!\n"); // me but this will probably never happen
        return;
    }

    void* Spec = nullptr;

    if (Engine_Version < 426)
        Spec = FindAbilitySpecFromHandle(ASC, Handle);
    else
        Spec = FindAbilitySpecFromHandleFTS(ASC, Handle);

    if (!Spec)
    {
        // Can potentially happen in race conditions where client tries to activate ability that is removed server side before it is received.
        std::cout << ("InternalServerTryActivateAbility. Rejecting ClientActivation of ability with invalid SpecHandle!\n");
        Helper::Abilities::ClientActivateAbilityFailed(ASC, Handle, *GetCurrent(PredictionKey));
        return;
    }

    const UObject* AbilityToActivate = *GetAbilityFromSpec(Spec);

    if (!AbilityToActivate) //!ensure(AbilityToActivate))
    {
        std::cout << ("InternalServerTryActiveAbility. Rejecting ClientActivation of unconfigured spec ability!\n");
        Helper::Abilities::ClientActivateAbilityFailed(ASC, Handle, *GetCurrent(PredictionKey));
        return;
    }

    // Consume any pending target info, to clear out cancels from old executions
    // ConsumeAllReplicatedData(ASC, Handle, PredictionKey); // Try this?

    UObject* InstancedAbility = nullptr;

    if (Engine_Version < 426)
        ((FGameplayAbilitySpec<FGameplayAbilityActivationInfo>*)Spec)->InputPressed = true;
    else
        ((FGameplayAbilitySpec<FGameplayAbilityActivationInfoFTS>*)Spec)->InputPressed = true;

    // Attempt to activate the ability (server side) and tell the client if it succeeded or failed.

    bool res = false;

    if (Engine_Version < 426)
        res = InternalTryActivateAbility(ASC, Handle, *PredictionKey, &InstancedAbility, nullptr, TriggerEventData);
    else
        res = InternalTryActivateAbilityFTS(ASC, Handle, *(FPredictionKeyFTS*)PredictionKey, &InstancedAbility, nullptr, TriggerEventData);

    if (!res)
    {
        auto InternalTryActivateAbilityFailureTags = ASC->Member<FGameplayTagContainer>(("ClientDebugStrings"), sizeof(FGameplayTagContainer));
        std::cout << std::format("InternalServerTryActivateAbility. Rejecting ClientActivation of {}. InternalTryActivateAbility failed: {}\n", (*GetAbilityFromSpec(Spec))->GetName(), InternalTryActivateAbilityFailureTags->ToStringSimple(true));
        Helper::Abilities::ClientActivateAbilityFailed(ASC, Handle, *GetCurrent(PredictionKey));

        if (Engine_Version < 426)
            ((FGameplayAbilitySpec<FGameplayAbilityActivationInfo>*)Spec)->InputPressed = false;
        else
            ((FGameplayAbilitySpec<FGameplayAbilityActivationInfoFTS>*)Spec)->InputPressed = false;

        MarkItemDirty(ASC->Member<void>(("ActivatableAbilities")), (FFastArraySerializerItem*)Spec); // TODO: Start using the proper function again
    }
}

static inline UObject* GrantGameplayAbility(UObject* TargetPawn, UObject* GameplayAbilityClass) // CREDITS: kem0x, raider3.5
{
    if (!GameplayAbilityClass || !TargetPawn)
        return nullptr;

    auto AbilitySystemComponent = *TargetPawn->Member<UObject*>(("AbilitySystemComponent"));

    if (!AbilitySystemComponent)
        return nullptr;

    UObject* DefaultObject = nullptr;

    if (!GameplayAbilityClass->GetFullName().contains("Class "))
        DefaultObject = GameplayAbilityClass; //->CreateDefaultObject(); // Easy::SpawnObject(GameplayAbilityClass, GameplayAbilityClass->OuterPrivate);
    else
        DefaultObject = GameplayAbilityClass->CreateDefaultObject();

    if (!DefaultObject)
    {
        std::cout << "Failed to create defaultobject!\n";
        std::cout << "GameplayAbilityClass: " << GameplayAbilityClass->GetFullName() << '\n';
        return nullptr;
    }

    auto GenerateNewSpec = [&]() -> FGameplayAbilitySpec<FGameplayAbilityActivationInfo>
    {
        FGameplayAbilitySpecHandle Handle{};
        Handle.GenerateNewHandle();

        FGameplayAbilitySpec<FGameplayAbilityActivationInfo> Spec{ -1, -1, -1, Handle, DefaultObject, 1, -1, nullptr, 0, false, false, false };

        return Spec;
    };

    auto GenerateNewSpecFTS = [&]() -> FGameplayAbilitySpec<FGameplayAbilityActivationInfoFTS>
    {
        FGameplayAbilitySpecHandle Handle{};
        Handle.GenerateNewHandle();

        FGameplayAbilitySpec<FGameplayAbilityActivationInfoFTS> Spec{ -1, -1, -1, Handle, DefaultObject, 1, -1, nullptr, 0, false, false, false };

        return Spec;
    };

    void* NewSpec = nullptr;

    if (Engine_Version < 426)
    {
        auto spec = GenerateNewSpec();
        NewSpec = &spec;
    }
    else
    {
        auto spec = GenerateNewSpecFTS();
        NewSpec = &spec;
    }

    if (Engine_Version <= 422)
    {
        auto ActivatableAbilities = *AbilitySystemComponent->Member<FGameplayAbilitySpecContainerOL>(("ActivatableAbilities"));

        for (int i = 0; i < ActivatableAbilities.Items.Num(); i++)
        {
            auto& CurrentSpec = ActivatableAbilities.Items[i];

            if (CurrentSpec.Ability == *GetAbilityFromSpec(NewSpec))
                return nullptr;
        }
    }
    else if (Engine_Version < 426)
    {
        auto ActivatableAbilities = *AbilitySystemComponent->Member<FGameplayAbilitySpecContainerSE>(("ActivatableAbilities"));

        for (int i = 0; i < ActivatableAbilities.Items.Num(); i++)
        {
            auto& CurrentSpec = ActivatableAbilities.Items[i];

            if (CurrentSpec.Ability == *GetAbilityFromSpec(NewSpec))
                return nullptr;
        }
    }
    else
    {
        auto ActivatableAbilities = *AbilitySystemComponent->Member<FGameplayAbilitySpecContainerFTS>(("ActivatableAbilities"));

        for (int i = 0; i < ActivatableAbilities.Items.Num(); i++)
        {
            auto& CurrentSpec = ActivatableAbilities.Items[i];

            if (CurrentSpec.Ability == *GetAbilityFromSpec(NewSpec))
                return nullptr;
        }
    }

    // https://github.com/EpicGames/UnrealEngine/blob/4.22/Engine/Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Private/AbilitySystemComponent_Abilities.cpp#L232

    if (Engine_Version < 426)
        GiveAbility(AbilitySystemComponent, &((FGameplayAbilitySpec<FGameplayAbilityActivationInfo>*)NewSpec)->Handle, *(FGameplayAbilitySpec<FGameplayAbilityActivationInfo>*)NewSpec);
    else
        GiveAbilityFTS(AbilitySystemComponent, &((FGameplayAbilitySpec<FGameplayAbilityActivationInfoFTS>*)NewSpec)->Handle, *(FGameplayAbilitySpec<FGameplayAbilityActivationInfoFTS>*)NewSpec);

    return *GetAbilityFromSpec(NewSpec);
}

inline bool ServerTryActivateAbilityHook(UObject* AbilitySystemComponent, UFunction* Function, void* Parameters)
{
    struct UAbilitySystemComponent_ServerTryActivateAbility_Params {
        FGameplayAbilitySpecHandle                  AbilityToActivate;                                        // (Parm)
        bool                                               InputPressed;                                             // (Parm, ZeroConstructor, IsPlainOldData)
        FPredictionKey                              PredictionKey;                                            // (Parm)
    };

    auto Params = (UAbilitySystemComponent_ServerTryActivateAbility_Params*)Parameters;

    InternalServerTryActivateAbility(AbilitySystemComponent, Params->AbilityToActivate, Params->InputPressed, &Params->PredictionKey, nullptr);

    return false;
}

inline bool ServerAbilityRPCBatchHook(UObject* AbilitySystemComponent, UFunction* Function, void* Parameters)
{
    if (Engine_Version <= 422)
    {
        struct UAbilitySystemComponent_ServerAbilityRPCBatch_Params {
            FServerAbilityRPCBatchOL                      BatchInfo;                                                // (Parm)
        };

        auto Params = (UAbilitySystemComponent_ServerAbilityRPCBatch_Params*)Parameters;

        auto& BatchInfo = Params->BatchInfo;

        InternalServerTryActivateAbility(AbilitySystemComponent, BatchInfo.AbilitySpecHandle, BatchInfo.InputPressed, &BatchInfo.PredictionKey, nullptr);
        // PrintExplicitTags(AbilitySystemComponent);
    }
    else
    {
        struct UAbilitySystemComponent_ServerAbilityRPCBatch_Params {
            FServerAbilityRPCBatchSE                      BatchInfo;                                                // (Parm)
        };

        auto Params = (UAbilitySystemComponent_ServerAbilityRPCBatch_Params*)Parameters;

        auto& BatchInfo = Params->BatchInfo;

        InternalServerTryActivateAbility(AbilitySystemComponent, BatchInfo.AbilitySpecHandle, BatchInfo.InputPressed, &BatchInfo.PredictionKey, nullptr);
        // PrintExplicitTags(AbilitySystemComponent);
    }

    return false;
}

inline bool ServerTryActivateAbilityWithEventDataHook(UObject* AbilitySystemComponent, UFunction* Function, void* Parameters)
{
    // std::cout << ("ServerTryActivateAbilityWithEventData!\n");

    struct UAbilitySystemComponent_ServerTryActivateAbilityWithEventData_Params {
        FGameplayAbilitySpecHandle                  AbilityToActivate;                                        // (Parm)
        bool                                               InputPressed;                                             // (Parm, ZeroConstructor, IsPlainOldData)
        FPredictionKey                              PredictionKey;                                            // (Parm)
        __int64                          TriggerEventData;                                         // (Parm)
    };

    auto Params = (UAbilitySystemComponent_ServerTryActivateAbilityWithEventData_Params*)Parameters;

    InternalServerTryActivateAbility(AbilitySystemComponent, Params->AbilityToActivate, Params->InputPressed, &Params->PredictionKey, &Params->TriggerEventData);

    return false;
}

// BY ANDROID THANKS FIXES ABILITIES

bool (*o_CanActivateAbility)(UObject* GameplayAbility, const FGameplayAbilitySpecHandle Handle, const void* ActorInfo, const void* SourceTags, const void* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags);
bool hk_CanActivateAbility(UObject* GameplayAbility, const FGameplayAbilitySpecHandle Handle, const void* ActorInfo, const void* SourceTags, const void* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags)
{
    // bool ret = o_CanActivateAbility(GameplayAbility, Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);

    return true;
}

void InitializeAbilityHooks()
{
    AddHook(("Function /Script/GameplayAbilities.AbilitySystemComponent.ServerTryActivateAbilityWithEventData"), ServerTryActivateAbilityWithEventDataHook);
    AddHook(("Function /Script/GameplayAbilities.AbilitySystemComponent.ServerAbilityRPCBatch"), ServerAbilityRPCBatchHook);
    AddHook(("Function /Script/GameplayAbilities.AbilitySystemComponent.ServerTryActivateAbility"), ServerTryActivateAbilityHook);

    if (o_CanActivateAbility)
    {
        MH_CreateHook((PVOID)CanActivateAbilityAddr, hk_CanActivateAbility, (void**)&o_CanActivateAbility);
        MH_EnableHook((PVOID)CanActivateAbilityAddr);
    }
}

template <typename actualClass>
void AHH(const std::string& ClassName)
{
    static auto Class = FindObject(ClassName, true);
    if (Class)
    {
        auto SizeOfClass = GetSizeOfStruct(Class);
        auto OurSizeOfClass = sizeof(actualClass);

        std::cout << std::format("{}: {} {}\n", ClassName, OurSizeOfClass, SizeOfClass);
    }
}

void TestAbilitySizeDifference()
{
    /* auto PredictionKeyDiff = SizeOfPredictionKey >= sizeof(FPredictionKey) ? SizeOfPredictionKey - sizeof(FPredictionKey) : sizeof(FPredictionKey) - SizeOfPredictionKey; */

    if (Engine_Version < 426)
        AHH<FPredictionKey>(("ScriptStruct /Script/GameplayAbilities.PredictionKey"));
    else
        AHH<FPredictionKeyFTS>(("ScriptStruct /Script/GameplayAbilities.PredictionKey"));

    AHH<FGameplayAbilitySpecHandle>(("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpecHandle"));

    if (Engine_Version < 426)
        AHH<FGameplayAbilitySpec<FGameplayAbilityActivationInfo>>(("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpec"));
    else
        AHH<FGameplayAbilitySpec<FGameplayAbilityActivationInfoFTS>>(("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpec"));

    if (Engine_Version <= 422)
    {
        AHH<FGameplayAbilitySpecContainerOL>(("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpecContainer"));
        AHH<FGameplayAbilityTargetDataHandleOL>(("ScriptStruct /Script/GameplayAbilities.GameplayAbilityTargetDataHandle"));
        AHH<FServerAbilityRPCBatchOL>(("ScriptStruct /Script/GameplayAbilities.ServerAbilityRPCBatch"));
        AHH<FGameplayEventDataOL>(("ScriptStruct /Script/GameplayAbilities.GameplayEventData"));
        AHH<FFastArraySerializerOL>(("ScriptStruct /Script/Engine.FastArraySerializer"));
    }
    else
    {
        AHH<FGameplayAbilitySpecContainerSE>(("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpecContainer"));
        AHH<FGameplayAbilityTargetDataHandleSE>(("ScriptStruct /Script/GameplayAbilities.GameplayAbilityTargetDataHandle"));

        if (Engine_Version < 426)
            AHH<FServerAbilityRPCBatchSE>(("ScriptStruct /Script/GameplayAbilities.ServerAbilityRPCBatch"));
        else
            AHH<FServerAbilityRPCBatchFTS>(("ScriptStruct /Script/GameplayAbilities.ServerAbilityRPCBatch"));

        AHH<FGameplayEventDataSE>(("ScriptStruct /Script/GameplayAbilities.GameplayEventData"));
        AHH<FFastArraySerializerSE>(("ScriptStruct /Script/Engine.FastArraySerializer"));
    }
}