#pragma once

#include <UE/structs.h>
#include <Gameplay/helper.h>
#include "Net/funcs.h"

// TODO: Rewrite, for now this is just kem0x's abilities on Raider.

FGameplayAbilitySpec* UAbilitySystemComponent_FindAbilitySpecFromHandle(UObject* AbilitySystemComponent, FGameplayAbilitySpecHandle Handle)
{
    auto Specs = (*AbilitySystemComponent->Member<FGameplayAbilitySpecContainer>(_("ActivatableAbilities"))).Items;

    std::cout << _("Spec size: ") << Specs.Num() << '\n';

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
static inline void TryActivateAbility(UObject* AbilitySystemComponent, FGameplayAbilitySpecHandle AbilityToActivate, bool InputPressed, FPredictionKey* PredictionKey, __int64* TriggerEventData)
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

static inline void GrantGameplayAbility(UObject* TargetPawn, UObject* GameplayAbilityClass, bool bAddSpecManually = false)
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
    
    if (bAddSpecManually)
        ActivatableAbilities.Items.Add(Spec);

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
    if (Engine_Version >= 423)
    {
        struct UAbilitySystemComponent_ServerAbilityRPCBatch_Params {
            FServerAbilityRPCBatchHigher                      BatchInfo;                                                // (Parm)
        };

        auto Params = (UAbilitySystemComponent_ServerAbilityRPCBatch_Params*)Parameters;

        TryActivateAbility(AbilitySystemComponent, Params->BatchInfo.AbilitySpecHandle, Params->BatchInfo.InputPressed, &Params->BatchInfo.PredictionKey, nullptr);
    }
    else
    {
        struct UAbilitySystemComponent_ServerAbilityRPCBatch_Params {
            FServerAbilityRPCBatch                      BatchInfo;                                                // (Parm)
        };

        auto Params = (UAbilitySystemComponent_ServerAbilityRPCBatch_Params*)Parameters;

        TryActivateAbility(AbilitySystemComponent, Params->BatchInfo.AbilitySpecHandle, Params->BatchInfo.InputPressed, &Params->BatchInfo.PredictionKey, nullptr);
    }

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

    TryActivateAbility(AbilitySystemComponent, Params->AbilityToActivate, Params->InputPressed, &Params->PredictionKey, &Params->TriggerEventData);

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
    AHH<FGameplayEventData>(_("ScriptStruct /Script/GameplayAbilities.GameplayEventData"));
    AHH<FGameplayEventDataHigher>(_("ScriptStruct /Script/GameplayAbilities.GameplayEventData"));
    AHH<FGameplayAbilitySpecHandle>(_("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpecHandle"));
    AHH<FServerAbilityRPCBatch>(_("ScriptStruct /Script/GameplayAbilities.ServerAbilityRPCBatch"));
    AHH<FServerAbilityRPCBatchHigher>(_("ScriptStruct /Script/GameplayAbilities.ServerAbilityRPCBatch"));
    AHH<FGameplayAbilitySpec>(_("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpec"));
    AHH<FGameplayAbilitySpecContainer>(_("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpecContainer"));
    AHH<FGameplayAbilityTargetDataHandle>(_("ScriptStruct /Script/GameplayAbilities.GameplayAbilityTargetDataHandle"));
    AHH<FGameplayAbilityTargetDataHandleHigher>(_("ScriptStruct /Script/GameplayAbilities.GameplayAbilityTargetDataHandle"));
    AHH<FFastArraySerializer>(_("ScriptStruct /Script/Engine.FastArraySerializer"));
}