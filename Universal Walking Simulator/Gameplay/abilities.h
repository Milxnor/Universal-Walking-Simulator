#pragma once

#include <UE/structs.h>
#include <Gameplay/helper.h>
#include "Net/funcs.h"

FGameplayAbilitySpec* FindAbilitySpecFromHandle(UObject* ASC, FGameplayAbilitySpecHandle Handle)
{
    auto Specs = (*ASC->Member<FGameplayAbilitySpecContainer>(_("ActivatableAbilities"))).Items;

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

void ConsumeAllReplicatedData(FGameplayAbilitySpecHandle AbilityHandle, FPredictionKey AbilityOriginalPredictionKey)
{
    /* TSharedPtr<FAbilityReplicatedDataCache> CachedData = AbilityTargetDataMap.Find(FGameplayAbilitySpecHandleAndPredictionKey(AbilityHandle, AbilityOriginalPredictionKey));
    if (CachedData.IsValid())
    {
        CachedData->Reset();
    } */
}

// https://github.com/EpicGames/UnrealEngine/blob/46544fa5e0aa9e6740c19b44b0628b72e7bbd5ce/Engine/Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Private/AbilitySystemComponent_Abilities.cpp#L1754

void PrintExplicitTags(UObject* ASC)
{
    static auto AHAHA = 264;
    auto MinimalReplicationGameplayCuesOff = GetOffset(ASC, (_("MinimalReplicationGameplayCues")));
    auto BlockedAbilityTags = (__int64(ASC) + (MinimalReplicationGameplayCuesOff + 0xD0));
    auto ExplicitTags = *(FGameplayTagContainer*)(BlockedAbilityTags + AHAHA);
    std::cout << "\n\nExplicit Tags: " << ExplicitTags.ToStringSimple(true) << "\n\n\n";
}

void InternalServerTryActivateAbility(UObject* ASC, FGameplayAbilitySpecHandle Handle, bool InputPressed, const FPredictionKey& PredictionKey, __int64* TriggerEventData)
{
    FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(ASC, Handle);
    if (!Spec)
    {
        // Can potentially happen in race conditions where client tries to activate ability that is removed server side before it is received.
        std::cout << _("InternalServerTryActivateAbility. Rejecting ClientActivation of ability with invalid SpecHandle!\n");
        Helper::Abilities::ClientActivateAbilityFailed(ASC, Handle, PredictionKey.Current);
        return;
    }

    const UObject* AbilityToActivate = Spec->Ability;

    if (!AbilityToActivate) //!ensure(AbilityToActivate))
    {
        std::cout << _("InternalServerTryActiveAbility. Rejecting ClientActivation of unconfigured spec ability!\n");
        Helper::Abilities::ClientActivateAbilityFailed(ASC, Handle, PredictionKey.Current);
        return;
    }

    // Consume any pending target info, to clear out cancels from old executions
    ConsumeAllReplicatedData(Handle, PredictionKey);

    /* FScopedPredictionWindow ScopedPredictionWindow(this, PredictionKey);

    ensure(AbilityActorInfo.IsValid());

    SCOPE_CYCLE_COUNTER(STAT_AbilitySystemComp_ServerTryActivate);
    SCOPE_CYCLE_UOBJECT(Ability, AbilityToActivate); */

    UObject* InstancedAbility = nullptr;
    Spec->InputPressed = true;

    // Attempt to activate the ability (server side) and tell the client if it succeeded or failed.

    if (InternalTryActivateAbility(ASC, Handle, PredictionKey, &InstancedAbility, nullptr, TriggerEventData))
    {
        // TryActivateAbility handles notifying the client of success
    }
    else
    {
        auto InternalTryActivateAbilityFailureTags = ASC->Member<FGameplayTagContainer>(_("ClientDebugStrings"), sizeof(FGameplayTagContainer));
        std::cout << std::format("InternalServerTryActivateAbility. Rejecting ClientActivation of {}. InternalTryActivateAbility failed: {}\n", Spec->Ability->GetName(), InternalTryActivateAbilityFailureTags->ToStringSimple(true));
        Helper::Abilities::ClientActivateAbilityFailed(ASC, Handle, PredictionKey.Current);
        Spec->InputPressed = false;
    }

    ASC->Member<FGameplayAbilitySpecContainer>(_("ActivatableAbilities"))->MarkArrayDirty();
    // ASC->Member< FGameplayAbilitySpecContainer>(_("ActivatableAbilities"))->MarkItemDirty(*Spec);
    
    // MarkAbilitySpecDirtyNew(ASC, *Spec, true);
}

static inline UObject* GrantGameplayAbility(UObject* TargetPawn, UObject* GameplayAbilityClass) // CREDITS: kem0x, raider3.5
{
    auto AbilitySystemComponent = *TargetPawn->Member<UObject*>(_("AbilitySystemComponent"));

    if (!AbilitySystemComponent)
        return nullptr;

    auto ActivatableAbilities = *AbilitySystemComponent->Member<FGameplayAbilitySpecContainer>(_("ActivatableAbilities"));

    UObject* DefaultObject = GameplayAbilityClass->CreateDefaultObject(); // Easy::SpawnObject(GameplayAbilityClass, GameplayAbilityClass->OuterPrivate);

    auto GenerateNewSpec = [&]() -> FGameplayAbilitySpec
    {
        FGameplayAbilitySpecHandle Handle{};
        Handle.GenerateNewHandle();

        FGameplayAbilitySpec Spec{ -1, -1, -1, Handle, DefaultObject, 1, -1, nullptr, 0, false, false, false };

        return Spec;
    };

    auto Spec = GenerateNewSpec();

    for (int i = 0; i < ActivatableAbilities.Items.Num(); i++)
    {
        auto& CurrentSpec = ActivatableAbilities.Items[i];

        if (CurrentSpec.Ability == Spec.Ability)
            return nullptr;
    }

    // https://github.com/EpicGames/UnrealEngine/blob/4.22/Engine/Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Private/AbilitySystemComponent_Abilities.cpp#L232
    
    GiveAbility(AbilitySystemComponent, &Spec.Handle, Spec);

    return Spec.Ability;
}

inline bool ServerTryActivateAbilityHook(UObject* AbilitySystemComponent, UFunction* Function, void* Parameters)
{
    struct UAbilitySystemComponent_ServerTryActivateAbility_Params {
        FGameplayAbilitySpecHandle                  AbilityToActivate;                                        // (Parm)
        bool                                               InputPressed;                                             // (Parm, ZeroConstructor, IsPlainOldData)
        FPredictionKey                              PredictionKey;                                            // (Parm)
    };

    auto Params = (UAbilitySystemComponent_ServerTryActivateAbility_Params*)Parameters;

    InternalServerTryActivateAbility(AbilitySystemComponent, Params->AbilityToActivate, Params->InputPressed, Params->PredictionKey, nullptr);

    return false;
}

inline bool ServerAbilityRPCBatchHook(UObject* AbilitySystemComponent, UFunction* Function, void* Parameters)
{
    struct UAbilitySystemComponent_ServerAbilityRPCBatch_Params {
        FServerAbilityRPCBatch                      BatchInfo;                                                // (Parm)
    };

    auto Params = (UAbilitySystemComponent_ServerAbilityRPCBatch_Params*)Parameters;

    auto BatchInfo = Params->BatchInfo;

    InternalServerTryActivateAbility(AbilitySystemComponent, Params->BatchInfo.AbilitySpecHandle, Params->BatchInfo.InputPressed, Params->BatchInfo.PredictionKey, nullptr);
    // PrintExplicitTags(AbilitySystemComponent);

    // The code below, is already done by fortnite.

    /* Helper::Abilities::ServerSetReplicatedTargetData(AbilitySystemComponent, BatchInfo.AbilitySpecHandle, BatchInfo.PredictionKey, BatchInfo.TargetData, FGameplayTag(), BatchInfo.PredictionKey);
    // ^ Crashes

    if (BatchInfo.Ended)
    {
        // This FakeInfo is probably bogus for the general case but should work for the limited use of batched RPCs
        FGameplayAbilityActivationInfo FakeInfo;
        // FakeInfo.ServerSetActivationPredictionKey(BatchInfo.PredictionKey);
        FakeInfo.PredictionKeyWhenActivated = BatchInfo.PredictionKey;
        Helper::Abilities::ServerEndAbility(AbilitySystemComponent, BatchInfo.AbilitySpecHandle, FakeInfo, BatchInfo.PredictionKey);
    } */

    return false;
}

inline bool ServerTryActivateAbilityWithEventDataHook(UObject* AbilitySystemComponent, UFunction* Function, void* Parameters)
{
    // std::cout << _("ServerTryActivateAbilityWithEventData!\n");

    struct UAbilitySystemComponent_ServerTryActivateAbilityWithEventData_Params {
        FGameplayAbilitySpecHandle                  AbilityToActivate;                                        // (Parm)
        bool                                               InputPressed;                                             // (Parm, ZeroConstructor, IsPlainOldData)
        FPredictionKey                              PredictionKey;                                            // (Parm)
        __int64                          TriggerEventData;                                         // (Parm)
    };

    auto Params = (UAbilitySystemComponent_ServerTryActivateAbilityWithEventData_Params*)Parameters;

    InternalServerTryActivateAbility(AbilitySystemComponent, Params->AbilityToActivate, Params->InputPressed, Params->PredictionKey, &Params->TriggerEventData);

    return false;
}

void InitializeAbilityHooks()
{
    AddHook(_("Function /Script/GameplayAbilities.AbilitySystemComponent.ServerTryActivateAbilityWithEventData"), ServerTryActivateAbilityWithEventDataHook);
    AddHook(_("Function /Script/GameplayAbilities.AbilitySystemComponent.ServerAbilityRPCBatch"), ServerAbilityRPCBatchHook);
    AddHook(_("Function /Script/GameplayAbilities.AbilitySystemComponent.ServerTryActivateAbility"), ServerTryActivateAbilityHook);
}

template <typename actualClass>
void AHH(const std::string& ClassName)
{
    static auto Class = FindObject(ClassName, true);
    auto SizeOfClass = GetSizeOfStruct(Class);
    auto OurSizeOfClass = sizeof(actualClass);

    std::cout << std::format("{}: {} {}\n", ClassName, OurSizeOfClass, SizeOfClass);
}

void TestAbilitySizeDifference()
{
    /* auto PredictionKeyDiff = SizeOfPredictionKey >= sizeof(FPredictionKey) ? SizeOfPredictionKey - sizeof(FPredictionKey) : sizeof(FPredictionKey) - SizeOfPredictionKey; */

    AHH<FPredictionKey>(_("ScriptStruct /Script/GameplayAbilities.PredictionKey"));
    AHH<FGameplayAbilitySpecHandle>(_("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpecHandle"));
    AHH<FGameplayAbilitySpec>(_("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpec"));
    AHH<FGameplayAbilitySpecContainer>(_("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpecContainer"));
    AHH<FGameplayAbilityTargetDataHandle>(_("ScriptStruct /Script/GameplayAbilities.GameplayAbilityTargetDataHandle"));
    AHH<FServerAbilityRPCBatch>(_("ScriptStruct /Script/GameplayAbilities.ServerAbilityRPCBatch"));
    AHH<FGameplayEventData>(_("ScriptStruct /Script/GameplayAbilities.GameplayEventData"));
    AHH<FFastArraySerializer>(_("ScriptStruct /Script/Engine.FastArraySerializer"));
}