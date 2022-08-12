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

// template return and base the func and actual ret of fof that maybe?
void LoopSpecs(UObject* ASC, std::function<void(__int64*)> func)
{
    auto ActivatableAbilities = ASC->Member<__int64>(("ActivatableAbilities"));

    auto ItemsOffset = FindOffsetStruct("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpecContainer", "Items");
    auto Items = (TArray<__int64>*)(__int64(ActivatableAbilities) + ItemsOffset);

    static auto SpecStruct = FindObjectOld("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpec", true);
    static auto SpecSize = GetSizeOfStruct(SpecStruct);

    if (ActivatableAbilities)
    {
        for (int i = 0; i < Items->Num(); i++)
        {
            auto CurrentSpec = (__int64*)(__int64(Items) + (static_cast<long long>(SpecSize) * i));
            func(CurrentSpec);
        }
    }
}

FGameplayAbilitySpecHandle* GetHandleFromSpec(__int64* Spec)
{
    static auto HandleOffset = FindOffsetStruct("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpec", "Handle");
    return (FGameplayAbilitySpecHandle*)(__int64(Spec) + HandleOffset);
}

int GetHandleFromHandle(FGameplayAbilitySpecHandle& handle)
{
    return handle.Handle;
}

__int64* FindAbilitySpecFromHandle2(UObject* ASC, FGameplayAbilitySpecHandle Handle)
{
    __int64* SpecToReturn = nullptr;

    auto compareHandles = [&Handle, &SpecToReturn](__int64* Spec) {
        auto CurrentHandle = GetHandleFromSpec(Spec);
        
        if (GetHandleFromHandle(*CurrentHandle) == Handle.Handle)
        {
            SpecToReturn = Spec;
            return;
        }
    };

    LoopSpecs(ASC, compareHandles);

    return SpecToReturn;
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

void InternalServerTryActivateAbility(UObject* ASC, FGameplayAbilitySpecHandle Handle, bool InputPressed, FPredictionKey* PredictionKey, __int64* TriggerEventData, bool bConsumeData = false)
{
    if (!PredictionKey)
    {
        std::cout << ("InternalServerTryActivateAbility. Rejecting ClientActivation of ability with invalid PredictionKey!\n"); // me but this will probably never happen
        return;
    }
    
    void* Spec = FindAbilitySpecFromHandle(ASC, Handle);

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

    struct hahah
    {
        unsigned char                                      InputPressed : 1;                                         // 0x0029(0x0001) (RepSkip, RepNotify, Interp, NonTransactional, EditorOnly, NoDestructor, AutoWeak, ContainsInstancedReference, AssetRegistrySearchable, SimpleDisplay, AdvancedDisplay, Protected, BlueprintCallable, BlueprintAuthorityOnly, TextExportTransient, NonPIEDuplicateTransient, ExposeOnSpawn, PersistentInstance, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic, NativeAccessSpecifierProtected, NativeAccessSpecifierPrivate)
        unsigned char                                      RemoveAfterActivation : 1;                                // 0x0029(0x0001) (RepSkip, RepNotify, Interp, NonTransactional, EditorOnly, NoDestructor, AutoWeak, ContainsInstancedReference, AssetRegistrySearchable, SimpleDisplay, AdvancedDisplay, Protected, BlueprintCallable, BlueprintAuthorityOnly, TextExportTransient, NonPIEDuplicateTransient, ExposeOnSpawn, PersistentInstance, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic, NativeAccessSpecifierProtected, NativeAccessSpecifierPrivate)
        unsigned char                                      PendingRemove : 1;
    };

    static auto InputPressedOffset = FindOffsetStruct("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpec", "InputPressed");
   
    ((hahah*)(__int64(Spec) + InputPressedOffset))->InputPressed = true;

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
        std::cout << std::format("InternalServerTryActivateAbility. Rejecting ClientActivation of {}.\n", (*GetAbilityFromSpec(Spec))->GetName());
        Helper::Abilities::ClientActivateAbilityFailed(ASC, Handle, *GetCurrent(PredictionKey));

        ((hahah*)(__int64(Spec) + InputPressedOffset))->InputPressed = false;

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
        std::cout << "Failed to create defaultobject for GameplayAbilityClass: " << GameplayAbilityClass->GetFullName() << '\n';
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

    auto GenerateNewSpecNew = [&]() -> void* {
        static auto GameplayAbilitySpecStruct = FindObjectOld("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpec", true);
        static auto GameplayAbilitySpecSize = GetSizeOfStruct(GameplayAbilitySpecStruct);

        std::cout << "Size of GameplayAbilitySpec: " << GameplayAbilitySpecSize << '\n';

        auto ptr = malloc(GameplayAbilitySpecSize);

        if (!ptr)
            return ptr;

        RtlSecureZeroMemory(ptr, GameplayAbilitySpecSize);

        FGameplayAbilitySpecHandle Handle{};
        Handle.GenerateNewHandle();

        ((FFastArraySerializerItem*)ptr)->MostRecentArrayReplicationKey = -1;
        ((FFastArraySerializerItem*)ptr)->ReplicationID = -1;
        ((FFastArraySerializerItem*)ptr)->ReplicationKey = -1;

        static auto HandleOffset = FindOffsetStruct("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpec", "Handle");
        static auto AbilityOffset = FindOffsetStruct("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpec", "Ability");
        static auto LevelOffset = FindOffsetStruct("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpec", "Level");
        static auto InputIDOffset = FindOffsetStruct("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpec", "InputID");
        static auto SourceObjectOffset = FindOffsetStruct("ScriptStruct /Script/GameplayAbilities.GameplayAbilitySpec", "SourceObject");

        *(FGameplayAbilitySpecHandle*)(__int64(ptr) + HandleOffset) = Handle;
        *(UObject**)(__int64(ptr) + AbilityOffset) = DefaultObject;
        *(int*)(__int64(ptr) + LevelOffset) = 1;
        *(int*)(__int64(ptr) + InputIDOffset) = -1;
        // *(UObject**)(__int64(ptr) + SourceObjectOffset) = nullptr;

        return ptr;
    };

    void* NewSpec = GenerateNewSpecNew(); // nullptr;

    /* if (Engine_Version < 426)
    {
        auto spec = GenerateNewSpec();
        NewSpec = &spec;
    }
    else
    {
        auto spec = GenerateNewSpecFTS();
        NewSpec = &spec;
    } */

    if (!NewSpec || DoesASCHaveAbility(AbilitySystemComponent, *GetAbilityFromSpec(NewSpec)))
        return nullptr;

    // https://github.com/EpicGames/UnrealEngine/blob/4.22/Engine/Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Private/AbilitySystemComponent_Abilities.cpp#L232

    /* GiveAbilityTest = decltype(GiveAbilityTest)(__int64(GiveAbility));

    GiveAbilityTest(AbilitySystemComponent, &((FGameplayAbilitySpec<FGameplayAbilityActivationInfo>*)NewSpec)->Handle, *(__int64*)NewSpec); */

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

    // auto AbilityToActivate = Function->GetParam<FGameplayAbilitySpecHandle>("AbilityToActivate", Parameters);// FindOffsetStruct("")

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