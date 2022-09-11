#pragma once

#include <Gameplay/helper.h>
#include <MinHook/MinHook.h>

#include "Net/functions.h"
#include "UE/hooks.h"

namespace Ability
{
    template <typename T>
    T* FindSpecByHandle(const FGameplayAbilitySpecHandle Handle, TArray<T> Specs)
    {
        auto it = std::ranges::find_if(Specs, [Handle](auto Spec) { return Spec.Handle.Handle == Handle.Handle; });
        return it == Specs.end() ? nullptr : *it;
    }

    inline auto FindAbilitySpecFromHandle(UObject* Object, const FGameplayAbilitySpecHandle Handle)
    {
        const auto Specs = EngineVersion <= 422
                               ? Object->Member<FGameplayAbilitySpecContainerOL>("ActivatableAbilities")->Items
                               : Object->Member<FGameplayAbilitySpecContainerSE>("ActivatableAbilities")->Items;
        return FindSpecByHandle(Handle, Specs);
    }

    inline auto FindAbilitySpecFromHandleFts(UObject* Object, const FGameplayAbilitySpecHandle Handle)
    {
        const auto Specs = Object->Member<FGameplayAbilitySpecContainerFTS>("ActivatableAbilities")->Items;
        return FindSpecByHandle(Handle, Specs);
    }

    inline auto FindAbilitySpecFromHandleNewer(UObject* Object, FGameplayAbilitySpecHandle Handle)
    {
        const auto Specs = Object->Member<FGameplayAbilitySpecContainerNewer>("ActivatableAbilities")->Items;
        return FindSpecByHandle(Handle, Specs);
    }

    inline auto GetCurrent(FPredictionKey* Key) -> int16_t*
    {
        if (!Key)
        {
            return nullptr;
        }

        static auto CurrentOffset = FindOffsetStruct("ScriptStruct /Script/GameplayAbilities.PredictionKey", "Current");
        return reinterpret_cast<int16_t*>(reinterpret_cast<long long>(Key) + CurrentOffset);
    }

    inline auto GetAbilityFromSpec(void* Spec) -> UObject**
    {
        if (!Spec)
        {
            return nullptr;
        }

        static auto AbilityOffset = FindOffsetStruct("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpec",
                                                     "Ability");
        return reinterpret_cast<UObject**>(reinterpret_cast<long long>(Spec) + AbilityOffset);
    }

    inline auto GetHandle(const FGameplayAbilitySpecHandle& Handle)
    {
        return Handle.Handle;
    }

    // IDEA: Template return and base the func and actual ret of fof that maybe?
    inline void LoopSpecs(UObject* Object, const std::function<void(long long*)>& Func)
    {
        auto ActivatableAbilities = Object->Member<long long>(("ActivatableAbilities"));

        static auto ItemsOffset = FindOffsetStruct(
            "ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpecContainer", "Items");
        const auto Items = reinterpret_cast<TArray<long long>*>(reinterpret_cast<long long>(ActivatableAbilities) +
            ItemsOffset);

        static auto SpecStruct = FindObjectOld("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpec", true);
        static auto SpecSize = GetSizeOfStruct(SpecStruct);

        if (!ActivatableAbilities || !Items)
        {
            return;
        }

        for (int i = 0; i < Items->Num(); i++)
        {
            const auto CurrentSpec = reinterpret_cast<long long*>(reinterpret_cast<long long>(Items->GetData()) +
                static_cast<long long>(SpecSize) * i);
            Func(CurrentSpec);
        }
    }

    inline long long* FindAbilitySpecFromHandle2(UObject* Object, FGameplayAbilitySpecHandle Handle)
    {
        long long* SpecToReturn = nullptr;

        auto compareHandles = [&Handle, &SpecToReturn](long long* Spec)
        {
            static auto HandleOffset = FindOffsetStruct("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpec",
                                                        "Handle");
            const auto CurrentHandle = reinterpret_cast<FGameplayAbilitySpecHandle*>(reinterpret_cast<long long>(Spec) +
                HandleOffset);
            if (GetHandle(*CurrentHandle) == Handle.Handle)
            {
                SpecToReturn = Spec;
            }
        };

        LoopSpecs(Object, compareHandles);

        return SpecToReturn;
    }

    inline void ActivateAbility(UObject* Object, FGameplayAbilitySpecHandle Handle, FPredictionKey* PredictionKey,
                                long long* TriggerEventData)
    {
        if (!PredictionKey)
        {
            return;
        }

        void* Spec = FindAbilitySpecFromHandle2(Object, Handle);
        if (!Spec)
        {
            Helper::Abilities::ClientActivateAbilityFailed(Object, Handle, *GetCurrent(PredictionKey));
            return;
        }

        if (const auto AbilityToActivate = *GetAbilityFromSpec(Spec); !AbilityToActivate)
        {
            Helper::Abilities::ClientActivateAbilityFailed(Object, Handle, *GetCurrent(PredictionKey));
            return;
        }

        UObject* InstancedAbility = nullptr;

        if (EngineVersion < 426)
        {
            static_cast<FGameplayAbilitySpec<FGameplayAbilityActivationInfo>*>(Spec)->InputPressed = true;
        }
        else if (EngineVersion == 426)
        {
            static_cast<FGameplayAbilitySpec<FGameplayAbilityActivationInfoFTS>*>(Spec)->InputPressed = true;
        }
        else
        {
            static_cast<FGameplayAbilitySpecNewer*>(Spec)->InputPressed = true;
        }

        if (EngineVersion < 426 && !InternalTryActivateAbility(Object, Handle, *PredictionKey, &InstancedAbility,
                                                                nullptr, TriggerEventData))
        {
            return;
        }

        if (EngineVersion >= 426 && !InternalTryActivateAbilityFts(Object, Handle,
                                                                    *reinterpret_cast<FPredictionKeyFTS*>(
                                                                        PredictionKey),
                                                                    &InstancedAbility, nullptr, TriggerEventData))
        {
            return;
        }


        Helper::Abilities::ClientActivateAbilityFailed(Object, Handle, *GetCurrent(PredictionKey));

        if (EngineVersion < 426)
        {
            static_cast<FGameplayAbilitySpec<FGameplayAbilityActivationInfo>*>(Spec)->InputPressed = false;
        }
        else if (EngineVersion == 426)
        {
            static_cast<FGameplayAbilitySpec<FGameplayAbilityActivationInfoFTS>*>(Spec)->InputPressed = false;
        }
        else
        {
            static_cast<FGameplayAbilitySpecNewer*>(Spec)->InputPressed = false;
        }

        // TODO: Start using the proper function again
        MarkItemDirty(Object->Member<void>("ActivatableAbilities"), static_cast<FFastArraySerializerItem*>(Spec));
    }

    inline UObject* DoesObjectHaveAbility(UObject* Object, UObject* Ability)
    {
        if (!Object || !Ability)
        {
            return nullptr;
        }

        UObject* AbilityToReturn = nullptr;
        auto compareAbilities = [&AbilityToReturn, &Ability](long long* Spec)
        {
            if (const auto CurrentAbility = GetAbilityFromSpec(Spec); *CurrentAbility == Ability)
            {
                AbilityToReturn = *CurrentAbility;
            }
        };

        LoopSpecs(Object, compareAbilities);
        return AbilityToReturn;
    }

    inline void ApplyGameplayEffect(UObject* AbilitySystemComponent, UObject* Ge)
    {
        if (!AbilitySystemComponent)
        {
            return;
        }

        static auto Func = AbilitySystemComponent->Function("BP_ApplyGameplayEffectToSelf");
        ApplyGameplayEffectParams Params{};
        Params.GameplayEffect = Ge;
        Params.Level = 1.0f;
        Params.EffectContext = FGameplayEffectContextHandle();
        AbilitySystemComponent->ProcessEvent(Func, &Params);
    }

    inline auto GetGameplayAbilityDefaultObject(UObject* GameplayAbilityClass)
    {
        if (!GameplayAbilityClass->GetFullName().contains("Class "))
        {
            // IDEA: ->CreateDefaultObject(); // Easy::SpawnObject(GameplayAbilityClass, GameplayAbilityClass->OuterPrivate);
            return GameplayAbilityClass;
        }

        const auto Name = GameplayAbilityClass->GetFullName();
        const auto Ending = GameplayAbilityClass->GetFullName().substr(Name.find_last_of('.') + 1);
        const auto Path = Name.substr(0, Name.find_last_of('.') + 1);
        return FindObject(std::format("{}Default__{}", Path, Ending));
    }

    inline auto GetGameplayAbilitySpec(UObject* const DefaultObject, const int& HandleOffset) -> void*
    {
        static auto GameplayAbilitySpecStruct = FindObjectOld(
            "ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpec", true);
        static auto GameplayAbilitySpecSize = GetSizeOfStruct(GameplayAbilitySpecStruct);


        auto ptr = malloc(GameplayAbilitySpecSize);
        if (!ptr)
        {
            return nullptr;
        }

        RtlSecureZeroMemory(ptr, GameplayAbilitySpecSize);

        FGameplayAbilitySpecHandle Handle{};
        Handle.GenerateNewHandle();

        static_cast<FFastArraySerializerItem*>(ptr)->MostRecentArrayReplicationKey = -1;
        static_cast<FFastArraySerializerItem*>(ptr)->ReplicationID = -1;
        static_cast<FFastArraySerializerItem*>(ptr)->ReplicationKey = -1;

        static auto AbilityOffset = FindOffsetStruct("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpec",
                                                     "Ability");
        static auto LevelOffset = FindOffsetStruct("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpec",
                                                   "Level");
        static auto InputIdOffset = FindOffsetStruct("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpec",
                                                     "InputID");

        *reinterpret_cast<FGameplayAbilitySpecHandle*>(reinterpret_cast<long long>(ptr) + HandleOffset) = Handle;
        *reinterpret_cast<UObject**>(reinterpret_cast<long long>(ptr) + AbilityOffset) = DefaultObject;
        *reinterpret_cast<int*>(reinterpret_cast<long long>(ptr) + LevelOffset) = 1;
        *reinterpret_cast<int*>(reinterpret_cast<long long>(ptr) + InputIdOffset) = -1;

        return ptr;
    }

    // CREDITS: kem0x, raider3.5
    // DOCUMENTATION: https://github.com/EpicGames/UnrealEngine/blob/4.22/Engine/Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Private/AbilitySystemComponent_Abilities.cpp#L232
    static auto EnableGameplayAbility(UObject* TargetPawn, UObject* GameplayAbilityClass) -> UObject*
    {
        if (!GameplayAbilityClass || !TargetPawn)
        {
            return nullptr;
        }

        const auto AbilitySystemComponent = *TargetPawn->Member<UObject*>(("AbilitySystemComponent"));
        if (!AbilitySystemComponent)
        {
            return nullptr;
        }


        const auto DefaultObject = GetGameplayAbilityDefaultObject(GameplayAbilityClass);
        if (!DefaultObject)
        {
            return nullptr;
        }

        static auto HandleOffset = FindOffsetStruct("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpec",
                                                    "Handle");
        auto NewSpec = GetGameplayAbilitySpec(DefaultObject, HandleOffset);
        if (!NewSpec)
        {
            return nullptr;
        }

        const auto Handle = reinterpret_cast<FGameplayAbilitySpecHandle*>(reinterpret_cast<long long>(NewSpec) + HandleOffset);

        if (!NewSpec || DoesObjectHaveAbility(AbilitySystemComponent, *GetAbilityFromSpec(NewSpec)))
        {
            return nullptr;
        }

        if (EngineVersion < 426)
        {
            GiveAbility(AbilitySystemComponent, Handle,
                        *static_cast<FGameplayAbilitySpec<FGameplayAbilityActivationInfo>*>(NewSpec));
            return *GetAbilityFromSpec(NewSpec);
        }

        if (EngineVersion == 426)
        {
            GiveAbilityFts(AbilitySystemComponent, Handle,
                           *static_cast<FGameplayAbilitySpec<FGameplayAbilityActivationInfoFTS>*>(NewSpec));
            return *GetAbilityFromSpec(NewSpec);
        }

        GiveAbilityNewer(AbilitySystemComponent, Handle, *static_cast<FGameplayAbilitySpecNewer*>(NewSpec));
        return *GetAbilityFromSpec(NewSpec);
    }

    inline bool ServerTryActivateAbilityHook(UObject* AbilitySystemComponent, [[maybe_unused]] UFunction* Function,
                                             void* Parameters)
    {
        const auto Params = static_cast<UAbilitySystemComponentServerTryActivateAbilityParams*>(Parameters);
        ActivateAbility(AbilitySystemComponent, Params->AbilityToActivate, &Params->PredictionKey, nullptr);
        return false;
    }

    inline bool ServerAbilityRpcBatchHook(UObject* AbilitySystemComponent, [[maybe_unused]] UFunction* Function,
                                          void* Parameters)
    {
        static auto AbilitySpecHandleOffset = FindOffsetStruct(
            "ScriptStruct /Script/GameplayAbilities.ServerAbilityRPCBatch", "AbilitySpecHandle");
        static auto InputPressedOffset = FindOffsetStruct(
            "ScriptStruct /Script/GameplayAbilities.ServerAbilityRPCBatch", "InputPressed");
        static auto PredictionKeyOffset = FindOffsetStruct(
            "ScriptStruct /Script/GameplayAbilities.ServerAbilityRPCBatch", "PredictionKey");

        const auto Params = static_cast<UAbilitySystemComponentServerAbilityRpcBatchParams*>(Parameters);

        const auto AbilitySpecHandle = reinterpret_cast<FGameplayAbilitySpecHandle*>(1441 + AbilitySpecHandleOffset);

        const auto InputPressed = reinterpret_cast<bool*>(reinterpret_cast<long long>(&Params->BatchInfo) +
            InputPressedOffset);
        const auto PredictionKey = reinterpret_cast<FPredictionKey*>(PredictionKeyOffset + reinterpret_cast<long long>(&
            Params->BatchInfo));
        if (!AbilitySpecHandle || !InputPressed || !PredictionKey)
        {
            return false;
        }

        if (const auto AbilitySpec = FindAbilitySpecFromHandle2(AbilitySystemComponent, *AbilitySpecHandle); !
            AbilitySpec)
        {
            return false;
        }

        ActivateAbility(AbilitySystemComponent, *AbilitySpecHandle, PredictionKey, nullptr);
        return false;
    }

    inline bool ServerTryActivateAbilityWithEventDataHook(UObject* AbilitySystemComponent,
                                                          [[maybe_unused]] UFunction* Function, void* Parameters)
    {
        const auto Params = static_cast<UAbilitySystemComponentServerTryActivateAbilityWithEventDataParams*>(
            Parameters);
        ActivateAbility(AbilitySystemComponent, Params->AbilityToActivate, &Params->PredictionKey,
                        &Params->TriggerEventData);
        return false;
    }

    // CREDITS: By Android, thanks for ability fix
    inline bool (*OCanActivateAbility)(UObject* GameplayAbility, FGameplayAbilitySpecHandle Handle,
                                       const void* ActorInfo,
                                       const void* SourceTags, const void* TargetTags,
                                       OUT FGameplayTagContainer* OptionalRelevantTags);

    inline bool HkCanActivateAbility([[maybe_unused]] UObject* GameplayAbility,
                                     [[maybe_unused]] const FGameplayAbilitySpecHandle Handle,
                                     [[maybe_unused]] const void* ActorInfo,
                                     [[maybe_unused]] const void* SourceTags, [[maybe_unused]] const void* TargetTags,
                                     [[maybe_unused]] OUT FGameplayTagContainer* OptionalRelevantTags)
    {
        return true;
    }

    inline void InitHooks()
    {
        Hooks::Add(("Function /Script/GameplayAbilities.AbilitySystemComponent.ServerTryActivateAbilityWithEventData"),
                ServerTryActivateAbilityWithEventDataHook);
        Hooks::Add(("Function /Script/GameplayAbilities.AbilitySystemComponent.ServerAbilityRPCBatch"),
                ServerAbilityRpcBatchHook);
        Hooks::Add(("Function /Script/GameplayAbilities.AbilitySystemComponent.ServerTryActivateAbility"),
                ServerTryActivateAbilityHook);
        if (!OCanActivateAbility)
        {
            return;
        }

        MH_CreateHook(reinterpret_cast<PVOID>(CanActivateAbilityAddress), reinterpret_cast<LPVOID>(HkCanActivateAbility),
                      reinterpret_cast<void**>(&OCanActivateAbility));
        MH_EnableHook(reinterpret_cast<PVOID>(CanActivateAbilityAddress));
    }
}
