#pragma once

#include <UE/structs.h>
#include <Gameplay/helper.h>
#include "Net/funcs.h"

// TODO: Rewrite, for now this is just kem0x's abilities on Raider.

FGameplayAbilitySpec* UAbilitySystemComponent_FindAbilitySpecFromHandle(UObject* AbilitySystemComponent, FGameplayAbilitySpecHandle Handle)
{
    auto Specs = (*AbilitySystemComponent->Member<FGameplayAbilitySpecContainer>(_("ActivatableAbilities"))).Items;

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

// https://github.com/EpicGames/UnrealEngine/blob/46544fa5e0aa9e6740c19b44b0628b72e7bbd5ce/Engine/Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Private/AbilitySystemComponent_Abilities.cpp#L1327
static inline void TryActivateAbility(UObject* AbilitySystemComponent, FGameplayAbilitySpecHandle AbilityToActivate, bool InputPressed, FPredictionKey* PredictionKey, FGameplayEventData* TriggerEventData)
{
    auto Spec = UAbilitySystemComponent_FindAbilitySpecFromHandle(AbilitySystemComponent, AbilityToActivate);

    if (!Spec)
    {
        std::cout << _("TryActivateAbility failed because it was not able to find the Spec!\n");
        Helper::Abilities::ClientActivateAbilityFailed(AbilitySystemComponent, AbilityToActivate, PredictionKey->Current);
        return;
    }

    UObject* InstancedAbility = nullptr; // UGameplayAbility*
    Spec->InputPressed = true;

    if (!InternalTryActivateAbility(AbilitySystemComponent, AbilityToActivate, *PredictionKey, &InstancedAbility, nullptr, TriggerEventData))
    {
        std::cout << _("TryActivateAbility failed because it was not able to activate the ability !\n");
        Helper::Abilities::ClientActivateAbilityFailed(AbilitySystemComponent, AbilityToActivate, PredictionKey->Current);
        Spec->InputPressed = false;
        return;
    }

   MarkAbilitySpecDirtyNew(AbilitySystemComponent, *Spec, false);
}

int GenerateNewHandle()
{
    // Must be in C++ to avoid duplicate statics accross execution units
    static int32_t GHandle = 1;
    return GHandle++;
}

static inline void GrantGameplayAbility(UObject* TargetPawn, UObject* GameplayAbilityClass)
{
    auto AbilitySystemComponent = *TargetPawn->Member<UObject*>(_("AbilitySystemComponent"));

    if (!AbilitySystemComponent)
        return;

    auto ActivatableAbilities = *AbilitySystemComponent->Member<FGameplayAbilitySpecContainer>(_("ActivatableAbilities"));

    UObject* DefaultObject = GameplayAbilityClass->CreateDefaultObject();// Easy::SpawnObject(GameplayAbilityClass, GameplayAbilityClass->OuterPrivate);

    auto GenerateNewSpec = [&]() -> FGameplayAbilitySpec
    {
        FGameplayAbilitySpecHandle Handle{ rand() };

        FGameplayAbilitySpec Spec{ -1, -1, -1, Handle, DefaultObject, 1, -1, nullptr, 0, false, false, false };

        return Spec;
    };

    auto Spec = GenerateNewSpec();

    for (int i = 0; i < ActivatableAbilities.Items.Num(); i++)
    {
        auto& CurrentSpec = ActivatableAbilities.Items[i];

        if (CurrentSpec.Ability == Spec.Ability)
            return;
    }

    // https://github.com/EpicGames/UnrealEngine/blob/4.22/Engine/Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Private/AbilitySystemComponent_Abilities.cpp#L232
    GiveAbility(AbilitySystemComponent, &Spec.Handle, Spec);
    // Helper::Abilities::GrantAbility(AbilitySystemComponent, &Spec.Handle, Spec);
    //AbilitySystemComponent->GiveAbility(
    // FGameplayAbilitySpec(StartupAbility, 
    //                      GetAbilityLevel(StartupAbility.GetDefaultObject()->AbilityID), static_cast<int32>(StartupAbility.GetDefaultObject()->AbilityInputID)
    // , this)); */
}

inline bool ServerTryActivateAbilityHook(UObject* AbilitySystemComponent, UFunction* Function, void* Parameters)
{
    struct UAbilitySystemComponent_ServerTryActivateAbility_Params {
        FGameplayAbilitySpecHandle                  AbilityToActivate;                                        // (Parm)
        bool                                               InputPressed;                                             // (Parm, ZeroConstructor, IsPlainOldData)
        FPredictionKey                              PredictionKey;                                            // (Parm)
    };

    // std::cout << _("ServerTryActivateAbility!\n");

    auto Params = (UAbilitySystemComponent_ServerTryActivateAbility_Params*)Parameters;

    TryActivateAbility(AbilitySystemComponent, Params->AbilityToActivate, Params->InputPressed, &Params->PredictionKey, nullptr);

    return false;
}

inline bool ServerAbilityRPCBatchHook(UObject* AbilitySystemComponent, UFunction* Function, void* Parameters)
{
    struct UAbilitySystemComponent_ServerAbilityRPCBatch_Params {
        FServerAbilityRPCBatch                      BatchInfo;                                                // (Parm)
    };

    // std::cout << _("ServerAbilityRPCBatch!\n");

    auto Params = (UAbilitySystemComponent_ServerAbilityRPCBatch_Params*)Parameters;

    TryActivateAbility(AbilitySystemComponent, Params->BatchInfo.AbilitySpecHandle, Params->BatchInfo.InputPressed, &Params->BatchInfo.PredictionKey, nullptr);

    return false;
}

inline bool ServerTryActivateAbilityWithEventDataHook(UObject* AbilitySystemComponent, UFunction* Function, void* Parameters)
{
    struct UAbilitySystemComponent_ServerTryActivateAbilityWithEventData_Params {
        FGameplayAbilitySpecHandle                  AbilityToActivate;                                        // (Parm)
        bool                                               InputPressed;                                             // (Parm, ZeroConstructor, IsPlainOldData)
        FPredictionKey                              PredictionKey;                                            // (Parm)
        FGameplayEventData                          TriggerEventData;                                         // (Parm)
    };

    // std::cout << _("ServerTryActivateAbilityWithEventData!\n");

    auto Params = (UAbilitySystemComponent_ServerTryActivateAbilityWithEventData_Params*)Parameters;

    TryActivateAbility(AbilitySystemComponent, Params->AbilityToActivate, Params->InputPressed, &Params->PredictionKey, &Params->TriggerEventData);

    return false;
}

void InitializeAbilityHooks()
{
    AddHook(_("Function /Script/GameplayAbilities.AbilitySystemComponent.ServerTryActivateAbilityWithEventData"), ServerTryActivateAbilityWithEventDataHook);
    AddHook(_("Function /Script/GameplayAbilities.AbilitySystemComponent.ServerAbilityRPCBatch"), ServerAbilityRPCBatchHook);
    AddHook(_("Function /Script/GameplayAbilities.AbilitySystemComponent.ServerTryActivateAbility"), ServerTryActivateAbilityHook);
}