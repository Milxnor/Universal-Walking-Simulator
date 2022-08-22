#pragma once

#include <UE/structs.h>
#include <Gameplay/helper.h>
#include "Net/funcs.h"
#include "../anticheat.h"

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

FGameplayAbilitySpecNewer* FindAbilitySpecFromHandleNewer(UObject* ASC, FGameplayAbilitySpecHandle Handle)
{
    TArray<FGameplayAbilitySpecNewer> Specs;

    Specs = (*ASC->Member<FGameplayAbilitySpecContainerNewer>(("ActivatableAbilities"))).Items;

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

int16_t* GetCurrent(FPredictionKey* Key)
{
    if (!Key)
        return nullptr;

    static auto CurrentOffset = FindOffsetStruct("ScriptStruct /Script/GameplayAbilities.PredictionKey", "Current");
    return (int16_t*)(__int64(Key) + CurrentOffset);
}

UObject** GetAbilityFromSpec(void* Spec)
{
    if (!Spec)
        return nullptr;

    static auto AbilityOffset = FindOffsetStruct("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpec", "Ability");

    return (UObject**)(__int64(Spec) + AbilityOffset);

    if (Engine_Version < 426)
        return &((FGameplayAbilitySpec<FGameplayAbilityActivationInfo>*)Spec)->Ability;
    else if (Engine_Version == 426)
        return &((FGameplayAbilitySpec<FGameplayAbilityActivationInfoFTS>*)Spec)->Ability;
    else
        return &((FGameplayAbilitySpecNewer*)Spec)->Ability;
}

int GetHandleFromHandle(FGameplayAbilitySpecHandle& handle)
{
    return handle.Handle;
}

// template return and base the func and actual ret of fof that maybe?
void LoopSpecs(UObject* ASC, std::function<void(__int64*)> func)
{
    auto ActivatableAbilities = ASC->Member<__int64>(("ActivatableAbilities"));

    static auto ItemsOffset = FindOffsetStruct("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpecContainer", "Items");
    auto Items = (TArray<__int64>*)(__int64(ActivatableAbilities) + ItemsOffset);

    static auto SpecStruct = FindObjectOld("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpec", true);
    static auto SpecSize = GetSizeOfStruct(SpecStruct);

    if (ActivatableAbilities && Items)
    {
        for (int i = 0; i < Items->Num(); i++)
        {
            auto CurrentSpec = (__int64*)(__int64(Items->GetData()) + (static_cast<long long>(SpecSize) * i));
            func(CurrentSpec);
        }
    }
}

__int64* FindAbilitySpecFromHandle2(UObject* ASC, FGameplayAbilitySpecHandle Handle)
{
    __int64* SpecToReturn = nullptr;

    auto compareHandles = [&Handle, &SpecToReturn](__int64* Spec) {
        static auto HandleOffset = FindOffsetStruct("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpec", "Handle");

        auto CurrentHandle = (FGameplayAbilitySpecHandle*)(__int64(Spec) + HandleOffset);

        if (GetHandleFromHandle(*CurrentHandle) == Handle.Handle)
        {
            SpecToReturn = Spec;
            return;
        }
    };

    LoopSpecs(ASC, compareHandles);

    return SpecToReturn;
}

void InternalServerTryActivateAbility(UObject* ASC, FGameplayAbilitySpecHandle Handle, bool InputPressed, FPredictionKey* PredictionKey, __int64* TriggerEventData, bool bConsumeData = false)
{
    if (!PredictionKey)
    {
        std::cout << ("InternalServerTryActivateAbility. Rejecting ClientActivation of ability with invalid PredictionKey!\n"); // me but this will probably never happen
        return;
    }

    void* Spec = FindAbilitySpecFromHandle2(ASC, Handle);

    /* void* Spec;

    if (Engine_Version < 426)
        Spec = FindAbilitySpecFromHandle(ASC, Handle);
    else if (Engine_Version == 426)
        Spec = FindAbilitySpecFromHandleFTS(ASC, Handle);
    else
        Spec = FindAbilitySpecFromHandleNewer(ASC, Handle); */

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
    else if (Engine_Version == 426)
        ((FGameplayAbilitySpec<FGameplayAbilityActivationInfoFTS>*)Spec)->InputPressed = true;
    else
        ((FGameplayAbilitySpecNewer*)Spec)->InputPressed = true;

    // Attempt to activate the ability (server side) and tell the client if it succeeded or failed.

    bool res = false;

    if (Engine_Version < 426)
        res = InternalTryActivateAbility(ASC, Handle, *PredictionKey, &InstancedAbility, nullptr, TriggerEventData);
    else
        res = InternalTryActivateAbilityFTS(ASC, Handle, *(FPredictionKeyFTS*)PredictionKey, &InstancedAbility, nullptr, TriggerEventData);

    if (!res)
    {
        // auto InternalTryActivateAbilityFailureTags = ASC->Member<FGameplayTagContainer>(("ClientDebugStrings"), sizeof(FGameplayTagContainer));
        // std::cout << std::format("InternalServerTryActivateAbility. Rejecting ClientActivation of {}. InternalTryActivateAbility failed: {}\n", (*GetAbilityFromSpec(Spec))->GetName(), InternalTryActivateAbilityFailureTags->ToStringSimple(true));
        std::cout << std::format("InternalServerTryActivateAbility. Rejecting ClientActivation of {}. InternalTryActivateAbility failed\n", (*GetAbilityFromSpec(Spec))->GetName());
        Helper::Abilities::ClientActivateAbilityFailed(ASC, Handle, *GetCurrent(PredictionKey));

        if (Engine_Version < 426)
            ((FGameplayAbilitySpec<FGameplayAbilityActivationInfo>*)Spec)->InputPressed = false;
        else if (Engine_Version == 426)
            ((FGameplayAbilitySpec<FGameplayAbilityActivationInfoFTS>*)Spec)->InputPressed = false;
        else
            ((FGameplayAbilitySpecNewer*)Spec)->InputPressed = false;

        MarkItemDirty(ASC->Member<void>(("ActivatableAbilities")), (FFastArraySerializerItem*)Spec); // TODO: Start using the proper function again
    }
}

UObject* DoesASCHaveAbility(UObject* ASC, UObject* Ability)
{
    if (!ASC || !Ability)
        return nullptr;

    UObject* AbilityToReturn = nullptr;

    auto compareAbilities = [&AbilityToReturn, &Ability](__int64* Spec) {
        auto CurrentAbility = GetAbilityFromSpec(Spec);

        if (*CurrentAbility == Ability)
        {
            AbilityToReturn = *CurrentAbility;
            return;
        }
    };

    LoopSpecs(ASC, compareAbilities);

    return AbilityToReturn;
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
    {
        auto name = GameplayAbilityClass->GetFullName();
        // skunked class to default
        auto ending = GameplayAbilityClass->GetFullName().substr(name.find_last_of(".") + 1);
        auto path = name.substr(0, name.find_last_of(".") + 1);
        DefaultObject = FindObject(std::format("{}Default__{}", path, ending));
    }

    if (!DefaultObject)
    {
        std::cout << "Failed to create defaultobject for GameplayAbilityClass: " << GameplayAbilityClass->GetFullName() << '\n';
        return nullptr;
    }

    static auto HandleOffset = FindOffsetStruct("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpec", "Handle");

    auto GenerateNewSpec = [&]() -> void*
    {
        static auto GameplayAbilitySpecStruct = FindObjectOld("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpec", true);
        static auto GameplayAbilitySpecSize = GetSizeOfStruct(GameplayAbilitySpecStruct);

        if (Engine_Version < 426)
            std::cout << "Size of GameplayAbilitySpec: " << GameplayAbilitySpecSize << '\n';

        auto ptr = malloc(GameplayAbilitySpecSize);

        if (!ptr)
            return nullptr;

        RtlSecureZeroMemory(ptr, GameplayAbilitySpecSize);

        FGameplayAbilitySpecHandle Handle{};
        Handle.GenerateNewHandle();

        ((FFastArraySerializerItem*)ptr)->MostRecentArrayReplicationKey = -1;
        ((FFastArraySerializerItem*)ptr)->ReplicationID = -1;
        ((FFastArraySerializerItem*)ptr)->ReplicationKey = -1;

        static auto AbilityOffset = FindOffsetStruct("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpec", "Ability");
        static auto LevelOffset = FindOffsetStruct("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpec", "Level");
        static auto InputIDOffset = FindOffsetStruct("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpec", "InputID");

        *(FGameplayAbilitySpecHandle*)(__int64(ptr) + HandleOffset) = Handle;
        *(UObject**)(__int64(ptr) + AbilityOffset) = DefaultObject;
        *(int*)(__int64(ptr) + LevelOffset) = 1;
        *(int*)(__int64(ptr) + InputIDOffset) = -1;

        return ptr;
    };

    void* NewSpec = GenerateNewSpec();

    if (!NewSpec)
        return nullptr;

    auto Handle = (FGameplayAbilitySpecHandle*)(__int64(NewSpec) + HandleOffset);

    if (!NewSpec || DoesASCHaveAbility(AbilitySystemComponent, *GetAbilityFromSpec(NewSpec)))
        return nullptr;

    // https://github.com/EpicGames/UnrealEngine/blob/4.22/Engine/Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Private/AbilitySystemComponent_Abilities.cpp#L232

    if (Engine_Version < 426)
        GiveAbility(AbilitySystemComponent, Handle, *(FGameplayAbilitySpec<FGameplayAbilityActivationInfo>*)NewSpec);
    else if (Engine_Version == 426)
        GiveAbilityFTS(AbilitySystemComponent, Handle, *(FGameplayAbilitySpec<FGameplayAbilityActivationInfoFTS>*)NewSpec);
    else
        GiveAbilityNewer(AbilitySystemComponent, Handle, *(FGameplayAbilitySpecNewer*)NewSpec);

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
    struct UAbilitySystemComponent_ServerAbilityRPCBatch_Params {
        __int64                      BatchInfo;                                                // (Parm)
    };

    static auto AbilitySpecHandleOffset = FindOffsetStruct("ScriptStruct /Script/GameplayAbilities.ServerAbilityRPCBatch", "AbilitySpecHandle"); // Function->GetParam<FGameplayAbilitySpecHandle>("AbilitySpecHandle", Parameters);
    static auto InputPressedOffset = FindOffsetStruct("ScriptStruct /Script/GameplayAbilities.ServerAbilityRPCBatch", "InputPressed");
    static auto PredictionKeyOffset = FindOffsetStruct("ScriptStruct /Script/GameplayAbilities.ServerAbilityRPCBatch", "PredictionKey");

    auto Params = (UAbilitySystemComponent_ServerAbilityRPCBatch_Params*)Parameters;

    auto AbilitySpecHandle = (FGameplayAbilitySpecHandle*)(__int64(&Params->BatchInfo) + AbilitySpecHandleOffset);
    auto InputPressed = (bool*)(__int64(&Params->BatchInfo) + InputPressedOffset);
    auto PredictionKey = (FPredictionKey*)(__int64(&Params->BatchInfo) + PredictionKeyOffset);

    if (AbilitySpecHandle && InputPressed && PredictionKey)
    {
        auto AbilitySpec = FindAbilitySpecFromHandle2(AbilitySystemComponent, *AbilitySpecHandle);

        if (!AbilitySpec)
            return false;

        InternalServerTryActivateAbility(AbilitySystemComponent, *AbilitySpecHandle, *InputPressed, PredictionKey, nullptr);
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
    else if (Engine_Version == 426)
        AHH<FGameplayAbilitySpec<FGameplayAbilityActivationInfoFTS>>(("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpec"));
    else
        AHH<FGameplayAbilitySpecNewer>(("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpec"));

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