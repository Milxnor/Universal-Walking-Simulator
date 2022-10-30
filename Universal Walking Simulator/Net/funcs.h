#pragma once

#include <UE/structs.h>
#include <unordered_map>
#include <functional>
#include <unordered_set>

uint64_t NoMcpAddr = 0;
uint64_t ActorChannelCloseAddr = 0;
uint64_t IsNetRelevantForAddr = 0;
uint64_t GetNetModeAddr = 0;
uint64_t LP_SpawnPlayActorAddr = 0;
uint64_t TickFlushAddr = 0;
uint64_t KickPlayerAddr = 0;
uint64_t World_NotifyControlMessageAddr = 0;
uint64_t SpawnPlayActorAddr = 0;
uint64_t Beacon_NotifyControlMessageAddr = 0;
uint64_t ReceiveFStringAddr = 0;
uint64_t ReceiveUniqueIdReplAddr = 0;
uint64_t WelcomePlayerAddr = 0;
uint64_t InitListenAddr = 0;
uint64_t InitHostAddr = 0;
uint64_t funnyaddry = 0;
uint64_t PauseBeaconRequestsAddr = 0;
uint64_t NetDebugAddr = 0;
uint64_t IdkfAddr = 0;
uint64_t SendChallengeAddr = 0;
uint64_t SetWorldAddr = 0;
uint64_t NoReserveAddr = 0;
uint64_t ClientTravelAddr = 0;
uint64_t CreateNetDriverAddr = 0;
uint64_t HasClientLoadedCurrentWorldAddr = 0;
uint64_t FixCrashAddr = 0;
uint64_t malformedAddr = 0;
uint64_t SetReplicationDriverAddr = 0;
uint64_t ValidationFailureAddr = 0;
uint64_t CollectGarbageAddr = 0;
uint64_t GetPlayerViewpointAddr = 0;
uint64_t CreateNetDriver_LocalAddr = 0;
uint64_t HandleReloadCostAddr = 0;
uint64_t CanActivateAbilityAddr = 0;
uint64_t CallPreReplicationAddr = 0;
uint64_t ReplicateActorAddr = 0;
uint64_t SendClientAdjustmentAddr = 0;
uint64_t SetChannelActorAddr = 0;
uint64_t CreateChannelAddr = 0;
uint64_t PlayMontageAddr = 0;
uint64_t ValidENameAddr = 0;
uint64_t ReplicationGraph_EnableAddr = 0;
uint64_t RequestExitWSAddr = 0;
uint64_t StaticLoadObjectAddr = 0;

static bool (*IsNetRelevantFor)(UObject*, UObject*, UObject*, FVector*);

static void (*UActorChannel_Close)(UObject*);

static void (*RequestExitWithStatus)(bool Force, uint8_t ReturnCode);

static char(__fastcall* ValidationFailure)(__int64* a1, __int64 a2);

static void (*SendChallenge)(UObject* a1, UObject* a2); // World, NetConnection
static __int64(__fastcall* Idkf)(__int64 a1, __int64 a2, int a3); // tbh we can just paste pseudo code

static UObject* (__fastcall* ReplicationGraph_Enable)(UObject* NetDriver, UObject* World);

static UObject* (*CreateChannelByName)(UObject* Connection, FName* ChName, EChannelCreateFlags CreateFlags, int32_t ChannelIndex); // = -1);
static void (*SetChannelActor)(UObject* ActorChannel, UObject* InActor);
static UObject* (*CreateChannel)(UObject* Connection, EChannelType Type, bool bOpenedLocally, int32_t ChannelIndex);

static void (__fastcall* SendClientAdjustment)(UObject* controller);
static void (__fastcall* CallPreReplication)(UObject* actor, UObject* driver);
static char (__fastcall* ReplicateActor)(UObject* ActorChannel);
static __int64 (*CanBuild)(UObject*, UObject*, FVector, FRotator, char, void*, char*);

static char (*KickPlayer)(UObject* a1, UObject*, FText a3); // session, pc

static void (__fastcall* HandleReloadCost)(UObject* Weapon, int AmountToRemove);

static __int64 (*GetNetMode)(__int64* a1);
static bool (*LP_SpawnPlayActor)(UObject* Player, const FString& URL, FString& OutError, UObject* World); // LocalPlayer
static __int64 (*CollectGarbage)(__int64 a1);

static void(__fastcall* GetPlayerViewPoint)(UObject* pc, FVector* a2, FRotator* a3);

static void* (*SetWorld)(UObject* NetDriver, UObject* World);
static bool (*InitListen)(UObject* Driver, void* InNotify, FURL& LocalURL, bool bReuseAddressAndPort, FString& Error);
static void (*TickFlush)(UObject* NetDriver, float DeltaSeconds);

static void (*RepGraph_ServerReplicateActors)(UObject* ReplicationDriver);

static void (*ReceiveFString)(void* Bunch, FString& Str);
static void (*ReceiveUniqueIdRepl)(void* Bunch, void* Str);
//static char (*ValidationFailure)(__int64 a1, __int64 a2);

static void (*WelcomePlayer)(UObject* World, UObject* Connection);
static void (*World_NotifyControlMessage)(UObject* World, UObject* Connection, uint8_t MessageType, void* Bunch);
static UObject* (*SpawnPlayActor)(UObject* World, UObject* NewPlayer, ENetRole RemoteRole, FURL& URL, void* UniqueId, FString& Error, uint8_t NetPlayerIndex);

inline bool (*InitHost)(UObject* Beacon);
inline void (*Beacon_NotifyControlMessage)(UObject* World, UObject* Connection, uint8_t MessageType, void* Bunch);
inline void (*PauseBeaconRequests)(UObject* Beacon, bool bPause);

static void* (*NetDebug)(UObject* a);

static __int64 (__fastcall* ClientTravel)(UObject* Controller, const FString& URL, ETravelType TravelType, bool bSeamless, FGuid MapPackageGuid);

static UObject* (*CreateNetDriver)(UObject* Engine, UObject* InWorld, FName NetDriverDefinition);

static bool(__fastcall* HasClientLoadedCurrentWorld)(UObject* pc);

static __int64(__fastcall* FixCrash)(int32_t* PossiblyNull, __int64 a2, int* a3);

static void(__fastcall* SetReplicationDriver)(UObject* a1, UObject* NewRepDriver);

static char(__fastcall* malformed)(__int64 a1, __int64 a2);

char(__fastcall* NoReserve)(__int64* a1, __int64 a2, char a3, __int64* a4);

static std::unordered_map<UFunction*, std::function<bool(UObject*, UFunction*, void*)>> FunctionsToHook;

void AddHook(const std::string& str, std::function<bool(UObject*, UFunction*, void*)> func)
{
    // auto funcObject = FindObjectOld<UFunction>(str, true);
    auto funcObject = FindObject<UFunction>(str);
    // static auto FuncClass = FindObject("Class /Script/CoreUObject.Function");
    // auto funcObject = LoadObject<UFunction>(FuncClass, nullptr, str);

    if (!funcObject)
        std::cout << "Unable to find Function: " << str << '\n';
    else
        FunctionsToHook.insert({ funcObject, func });
}

static auto AircraftLocationToUse = FVector{ 3500, -10000, 10500 };

// START ABILITIES

struct FGameplayAbilitySpecHandle 
{ 
    int Handle; 
    static inline std::unordered_set<int> Handles;

    void GenerateNewHandle() // severe brain damaage
    {
        // Must be in C++ to avoid duplicate statics across execution units
        /* static int32_t GHandle = 1;
        Handle = GHandle++; */

        auto newHandle = rand();

        while (Handles.find(newHandle) != Handles.end())
        {
            newHandle = rand();
        }

        Handle = newHandle;
        Handles.emplace(Handle);
    }
};

struct FPredictionKey
{
    int16_t                                            Current;                                                  // 0x0000(0x0002) (ZeroConstructor, IsPlainOldData)
    int16_t                                            Base;                                                     // 0x0002(0x0002) (ZeroConstructor, IsPlainOldData)
    unsigned char                                      UnknownData00[0x4];                                       // 0x0004(0x0004) MISSED OFFSET
    UObject* PredictiveConnection;                                     // 0x0008(0x0008) (ZeroConstructor, IsPlainOldData, RepSkip, RepNotify, Interp, NonTransactional, EditorOnly, NoDestructor, AutoWeak, ContainsInstancedReference, AssetRegistrySearchable, SimpleDisplay, AdvancedDisplay, Protected, BlueprintCallable, BlueprintAuthorityOnly, TextExportTransient, NonPIEDuplicateTransient, ExposeOnSpawn, PersistentInstance, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic, NativeAccessSpecifierProtected, NativeAccessSpecifierPrivate)
    bool                                               bIsStale;                                                 // 0x0010(0x0001) (ZeroConstructor, IsPlainOldData, RepSkip, RepNotify, Interp, NonTransactional, EditorOnly, NoDestructor, AutoWeak, ContainsInstancedReference, AssetRegistrySearchable, SimpleDisplay, AdvancedDisplay, Protected, BlueprintCallable, BlueprintAuthorityOnly, TextExportTransient, NonPIEDuplicateTransient, ExposeOnSpawn, PersistentInstance, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic, NativeAccessSpecifierProtected, NativeAccessSpecifierPrivate)
    bool                                               bIsServerInitiated;                                       // 0x0011(0x0001) (ZeroConstructor, IsPlainOldData)
    unsigned char                                      UnknownData01[0x6];                                       // 0x0012(0x0006) MISSED OFFSET
};

struct FPredictionKeyFTS // 4.26
{
    UObject* PredictiveConnection;                                     // 0x0000(0x0008) (ZeroConstructor, IsPlainOldData, RepSkip, RepNotify, Interp, NonTransactional, EditorOnly, NoDestructor, AutoWeak, ContainsInstancedReference, AssetRegistrySearchable, SimpleDisplay, AdvancedDisplay, Protected, BlueprintCallable, BlueprintAuthorityOnly, TextExportTransient, NonPIEDuplicateTransient, ExposeOnSpawn, PersistentInstance, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic, NativeAccessSpecifierProtected, NativeAccessSpecifierPrivate)
    int16_t                                            Current;                                                  // 0x0008(0x0002) (ZeroConstructor, IsPlainOldData)
    int16_t                                            Base;                                                     // 0x000A(0x0002) (ZeroConstructor, IsPlainOldData)
    bool                                               bIsStale;                                                 // 0x000C(0x0001) (ZeroConstructor, IsPlainOldData, RepSkip, RepNotify, Interp, NonTransactional, EditorOnly, NoDestructor, AutoWeak, ContainsInstancedReference, AssetRegistrySearchable, SimpleDisplay, AdvancedDisplay, Protected, BlueprintCallable, BlueprintAuthorityOnly, TextExportTransient, NonPIEDuplicateTransient, ExposeOnSpawn, PersistentInstance, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic, NativeAccessSpecifierProtected, NativeAccessSpecifierPrivate)
    bool                                               bIsServerInitiated;                                       // 0x000D(0x0001) (ZeroConstructor, IsPlainOldData)
    unsigned char                                      UnknownData00[0x2];                                       // 0x0012(0x0006) MISSED OFFSET
};

struct FPredictionKeyNewer {
    UObject* PredictiveConnection; // 0x00(0x08)
    int16_t Current; // 0x08(0x02)
    int16_t Base; // 0x0a(0x02)
    bool bIsStale; // 0x0c(0x01)
    bool bIsServerInitiated; // 0x0d(0x01)
    char pad_E[0xA]; // 0x0e(0x0a)
};

struct FGameplayTag
{
    FName                                       TagName;                                                  // 0x0000(0x0008) (Edit, ZeroConstructor, EditConst, IsPlainOldData)
};

bool IsBadReadPtr(void* p)
{
    MEMORY_BASIC_INFORMATION mbi = { 0 };
    if (::VirtualQuery(p, &mbi, sizeof(mbi)))
    {
        DWORD mask = (PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY);
        bool b = !(mbi.Protect & mask);
        // check the page is not a guard page
        if (mbi.Protect & (PAGE_GUARD | PAGE_NOACCESS)) b = true;

        return b;
    }
    return true;
}

struct FGameplayTagContainer
{
    TArray<FGameplayTag>                        GameplayTags;                                             // 0x0000(0x0010) (BlueprintVisible, ZeroConstructor)
    TArray<FGameplayTag>                        ParentTags;                                               // 0x0010(0x0010) (ZeroConstructor, Transient)

    std::string ToStringSimple(bool bQuoted) const
    {
        std::string RetString;
        for (int i = 0; i < GameplayTags.Num(); ++i)
        {
            if (bQuoted)
            {
                RetString += ("\"");
            }
            RetString += GameplayTags[i].TagName.ToString();
            if (bQuoted)
            {
                RetString += ("\"");
            }

            if (i < GameplayTags.Num() - 1)
            {
                RetString += (", ");
            }
        }
        return RetString;
    }

    void Reset()
    {
        GameplayTags.Free();
        ParentTags.Free();
    }
};

struct PaddingDec24
{
    char pad[24];
};

struct PaddingDec224
{
    char pad[224];
};

struct PaddingDec232
{
    char pad[224];
};

struct PaddingDec32
{
    char pad[224];
};

enum class EGameplayAbilityActivationMode : uint8_t
{
    Authority = 0,
    NonAuthority = 1,
    Predicting = 2,
    Confirmed = 3,
    Rejected = 4,
    EGameplayAbilityActivationMode_MAX = 5
};

struct FGameplayAbilityActivationInfo
{
    TEnumAsByte<EGameplayAbilityActivationMode>        ActivationMode;                                           // 0x0000(0x0001) (BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData)
    unsigned char                                      bCanBeEndedByOtherInstance : 1;                           // 0x0001(0x0001)
    unsigned char                                      UnknownData00[0x6];                                       // 0x0002(0x0006) MISSED OFFSET
    FPredictionKey                              PredictionKeyWhenActivated;                               // 0x0008(0x0018)
};

struct FGameplayAbilityActivationInfoFTS
{
    TEnumAsByte<EGameplayAbilityActivationMode>        ActivationMode;                                           // 0x0000(0x0001) (BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData)
    unsigned char                                      bCanBeEndedByOtherInstance : 1;                           // 0x0001(0x0001)
    unsigned char                                      UnknownData00[0x6];                                       // 0x0002(0x0006) MISSED OFFSET
    FPredictionKeyFTS                              PredictionKeyWhenActivated;                               // 0x0008(0x0018)
};

struct FGameplayAbilityActivationInfoNewer
{
    TEnumAsByte<EGameplayAbilityActivationMode>        ActivationMode;                                           // 0x0000(0x0001) (BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData)
    unsigned char                                      bCanBeEndedByOtherInstance : 1;                           // 0x0001(0x0001)
    unsigned char                                      UnknownData00[0x6];                                       // 0x0002(0x0006) MISSED OFFSET
    FPredictionKeyNewer                              PredictionKeyWhenActivated;                               // 0x0008(0x0018)
};

struct FActiveGameplayEffectHandle
{
    int                                                Handle;                                                   // 0x0000(0x0004) (ZeroConstructor, IsPlainOldData)
    bool                                               bPassedFiltersAndWasExecuted;                             // 0x0004(0x0001) (ZeroConstructor, IsPlainOldData)
    unsigned char                                      UnknownData00[0x3];                                       // 0x0005(0x0003) MISSED OFFSET
};

template <typename ActivationInfoType, int UnknownDataSize>
struct FGameplayAbilitySpec : public FFastArraySerializerItem
{
    FGameplayAbilitySpecHandle                  Handle;                                                   // 0x000C(0x0004)
    UObject* Ability; // UGameplayAbility*
    int                                                Level;                                                    // 0x0018(0x0004) (ZeroConstructor, IsPlainOldData)
    int                                                InputID;                                                  // 0x001C(0x0004) (ZeroConstructor, IsPlainOldData)
    UObject* SourceObject;                                             // 0x0020(0x0008) (ZeroConstructor, IsPlainOldData)
    unsigned char                                      ActiveCount;                                              // 0x0028(0x0001) (ZeroConstructor, IsPlainOldData, RepSkip, RepNotify, Interp, NonTransactional, EditorOnly, NoDestructor, AutoWeak, ContainsInstancedReference, AssetRegistrySearchable, SimpleDisplay, AdvancedDisplay, Protected, BlueprintCallable, BlueprintAuthorityOnly, TextExportTransient, NonPIEDuplicateTransient, ExposeOnSpawn, PersistentInstance, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic, NativeAccessSpecifierProtected, NativeAccessSpecifierPrivate)
    unsigned char                                      InputPressed : 1;                                         // 0x0029(0x0001) (RepSkip, RepNotify, Interp, NonTransactional, EditorOnly, NoDestructor, AutoWeak, ContainsInstancedReference, AssetRegistrySearchable, SimpleDisplay, AdvancedDisplay, Protected, BlueprintCallable, BlueprintAuthorityOnly, TextExportTransient, NonPIEDuplicateTransient, ExposeOnSpawn, PersistentInstance, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic, NativeAccessSpecifierProtected, NativeAccessSpecifierPrivate)
    unsigned char                                      RemoveAfterActivation : 1;                                // 0x0029(0x0001) (RepSkip, RepNotify, Interp, NonTransactional, EditorOnly, NoDestructor, AutoWeak, ContainsInstancedReference, AssetRegistrySearchable, SimpleDisplay, AdvancedDisplay, Protected, BlueprintCallable, BlueprintAuthorityOnly, TextExportTransient, NonPIEDuplicateTransient, ExposeOnSpawn, PersistentInstance, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic, NativeAccessSpecifierProtected, NativeAccessSpecifierPrivate)
    unsigned char                                      PendingRemove : 1;                                        // 0x0029(0x0001) (RepSkip, RepNotify, Interp, NonTransactional, EditorOnly, NoDestructor, AutoWeak, ContainsInstancedReference, AssetRegistrySearchable, SimpleDisplay, AdvancedDisplay, Protected, BlueprintCallable, BlueprintAuthorityOnly, TextExportTransient, NonPIEDuplicateTransient, ExposeOnSpawn, PersistentInstance, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic, NativeAccessSpecifierProtected, NativeAccessSpecifierPrivate)
    unsigned char                                      UnknownData00[0x6];                                       // 0x002A(0x0006) MISSED OFFSET
    ActivationInfoType              ActivationInfo;                                           // 0x0030(0x0020) (RepSkip, RepNotify, Interp, NonTransactional, EditorOnly, NoDestructor, AutoWeak, ContainsInstancedReference, AssetRegistrySearchable, SimpleDisplay, AdvancedDisplay, Protected, BlueprintCallable, BlueprintAuthorityOnly, TextExportTransient, NonPIEDuplicateTransient, ExposeOnSpawn, PersistentInstance, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic, NativeAccessSpecifierProtected, NativeAccessSpecifierPrivate)
    TArray<UObject*>                    NonReplicatedInstances; // UGameplayAbility*
    TArray<UObject*>                    ReplicatedInstances;                      
    FActiveGameplayEffectHandle                 GameplayEffectHandle; // UGameplayAbility*
    unsigned char                                      UnknownData01[UnknownDataSize];                                      // 0x0078(0x0050) MISSED OFFSET
};

/* struct FGameplayAbilitySpecNewer : FFastArraySerializerItem {
    FGameplayAbilitySpecHandle Handle; // 0x0c(0x04)
    UObject* Ability; // 0x10(0x08)
    int32_t Level; // 0x18(0x04)
    int32_t InputID; // 0x1c(0x04)
    UObject* SourceObject; // 0x20(0x08)
    char ActiveCount; // 0x28(0x01)
    char InputPressed : 1; // 0x29(0x01)
    char RemoveAfterActivation : 1; // 0x29(0x01)
    char PendingRemove : 1; // 0x29(0x01)
    char bActivateOnce : 1; // 0x29(0x01)
    char pad_29_4 : 4; // 0x29(0x01)
    char pad_2A[0x6]; // 0x2a(0x06)
    FGameplayAbilityActivationInfoFTS ActivationInfo; // 0x30(0x20)
    FGameplayTagContainer DynamicAbilityTags; // 0x50(0x20)
    TArray<UObject*> NonReplicatedInstances; // 0x70(0x10)
    TArray<UObject*> ReplicatedInstances; // 0x80(0x10)
    FActiveGameplayEffectHandle GameplayEffectHandle; // 0x90(0x08)
    char pad_98[0x50]; // 0x98(0x50)
}; */

struct FGameplayAbilitySpecNewer : FFastArraySerializerItem {
    FGameplayAbilitySpecHandle Handle; // 0x0c(0x04)
    UObject* Ability; // 0x10(0x08)
    int32_t Level; // 0x18(0x04)
    int32_t InputID; // 0x1c(0x04)
    UObject* SourceObject; // 0x20(0x08)
    char ActiveCount; // 0x28(0x01)
    char InputPressed : 1; // 0x29(0x01)
    char RemoveAfterActivation : 1; // 0x29(0x01)
    char PendingRemove : 1; // 0x29(0x01)
    char bActivateOnce : 1; // 0x29(0x01)
    char UnknownData_29_4 : 4; // 0x29(0x01)
    char UnknownData_2A[0x6]; // 0x2a(0x06)
    FGameplayAbilityActivationInfoNewer ActivationInfo; // 0x30(0x18)
    FGameplayTagContainer DynamicAbilityTags; // 0x48(0x20)
    TArray<UObject*> NonReplicatedInstances; // 0x68(0x10)
    TArray<UObject*> ReplicatedInstances; // 0x78(0x10)
    FActiveGameplayEffectHandle GameplayEffectHandle; // 0x88(0x08)
    char UnknownData_90[0x50]; // 0x90(0x50)
};

struct FGameplayAbilitySpecContainerSE : public FFastArraySerializerSE
{
    TArray<FGameplayAbilitySpec<FGameplayAbilityActivationInfo, 0x50>>                Items;                                                    // 0x00B0(0x0010) (ZeroConstructor)
    UObject* Owner;
};

struct FGameplayAbilitySpecContainerFTS : public FFastArraySerializerSE
{
    TArray<FGameplayAbilitySpec<FGameplayAbilityActivationInfoFTS, 0x50>>                Items;                                                    // 0x00B0(0x0010) (ZeroConstructor)
    UObject* Owner; // ASC* // 0x0118
};

struct FGameplayAbilitySpecContainerNewer : public FFastArraySerializerSE
{
    TArray<FGameplayAbilitySpecNewer>                Items;                                                    // 0x00B0(0x0010) (ZeroConstructor)
    UObject* Owner; // ASC* // 0x0118
};

struct FGameplayAbilitySpecContainerOL : public FFastArraySerializerOL
{
    TArray<FGameplayAbilitySpec<FGameplayAbilityActivationInfo, 0x50>>                Items;                                                    // 0x00B0(0x0010) (ZeroConstructor)
    UObject* Owner; // ASC* // 0x0118
};

enum class EGameplayAbilityInstancingPolicy : uint8_t
{
    NonInstanced = 0,
    InstancedPerActor = 1,
    InstancedPerExecution = 2,
    EGameplayAbilityInstancingPolicy_MAX = 3
};

static FGameplayAbilitySpecHandle* (*GiveAbility)(UObject* comp, FGameplayAbilitySpecHandle* outHandle, FGameplayAbilitySpec<FGameplayAbilityActivationInfo, 0x50> inSpec);
static FGameplayAbilitySpecHandle* (*GiveAbilityOLDDD)(UObject* comp, FGameplayAbilitySpecHandle* outHandle, FGameplayAbilitySpec<FGameplayAbilityActivationInfo, 0> inSpec);
static FGameplayAbilitySpecHandle* (*GiveAbilityFTS)(UObject* comp, FGameplayAbilitySpecHandle* outHandle, FGameplayAbilitySpec<FGameplayAbilityActivationInfoFTS, 0x50> inSpec);
static FGameplayAbilitySpecHandle* (*GiveAbilityNewer)(UObject* comp, FGameplayAbilitySpecHandle* outHandle, FGameplayAbilitySpecNewer inSpec);
static FGameplayAbilitySpecHandle* (*GiveAbilityS14ANDS15)(UObject* comp, FGameplayAbilitySpecHandle* outHandle, PaddingDec224 inSpec);
static FGameplayAbilitySpecHandle* (*GiveAbilityS16)(UObject* comp, FGameplayAbilitySpecHandle* outHandle, PaddingDec232 inSpec);

static bool (*InternalTryActivateAbility)(UObject* comp, FGameplayAbilitySpecHandle Handle, FPredictionKey InPredictionKey, UObject** /* UGameplayAbility** */ OutInstancedAbility, void* OnGameplayAbilityEndedDelegate, __int64* TriggerEventData); // // https://github.com/EpicGames/UnrealEngine/blob/46544fa5e0aa9e6740c19b44b0628b72e7bbd5ce/Engine/Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Private/AbilitySystemComponent_Abilities.cpp#L1327
static bool (*InternalTryActivateAbilityFTS)(UObject* comp, FGameplayAbilitySpecHandle Handle, FPredictionKeyFTS InPredictionKey, UObject** /* UGameplayAbility** */ OutInstancedAbility, void* OnGameplayAbilityEndedDelegate, __int64* TriggerEventData); // // https://github.com/EpicGames/UnrealEngine/blob/46544fa5e0aa9e6740c19b44b0628b72e7bbd5ce/Engine/Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Private/AbilitySystemComponent_Abilities.cpp#L1327
static bool (*InternalTryActivateAbilityNewer)(UObject* comp, FGameplayAbilitySpecHandle Handle, FPredictionKeyNewer InPredictionKey, UObject** /* UGameplayAbility** */ OutInstancedAbility, void* OnGameplayAbilityEndedDelegate, __int64* TriggerEventData); // // https://github.com/EpicGames/UnrealEngine/blob/46544fa5e0aa9e6740c19b44b0628b72e7bbd5ce/Engine/Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Private/AbilitySystemComponent_Abilities.cpp#L1327

static void (*MarkAbilitySpecDirtyOld)(UObject* comp, FGameplayAbilitySpec<FGameplayAbilityActivationInfo, 0x50>& Spec);
static void (*MarkAbilitySpecDirtyFTS)(UObject* comp, FGameplayAbilitySpec<FGameplayAbilityActivationInfoFTS, 0x50>& Spec);

void MarkAbilitySpecDirty(UObject* ASC, void* Spec)
{
    if (Engine_Version < 426)
        MarkAbilitySpecDirtyOld(ASC, *(FGameplayAbilitySpec<FGameplayAbilityActivationInfo, 0x50>*)Spec);
    else
        MarkAbilitySpecDirtyFTS(ASC, *(FGameplayAbilitySpec<FGameplayAbilityActivationInfoFTS, 0x50>*)Spec);
}

uint64_t GiveAbilityAddr = 0;
uint64_t InternalTryActivateAbilityAddr = 0;
uint64_t MarkAbilitySpecDirtyAddr = 0;

uint64_t CreateNamedNetDriverAddr = 0; // CreateNetDriver_Local

UObject* (*CreateNetDriver_Local)(UObject* Engine, UObject* InWorld, FName NetDriverDefinition);

template<typename T, typename U> constexpr size_t offsetOf(U T::* member)
{
    return (char*)&((T*)nullptr->*member) - (char*)nullptr;
}

float (*PlayMontage)(UObject* AbilitySystemComponent, UObject* InAnimatingAbility, FGameplayAbilityActivationInfo ActivationInfo, UObject* NewAnimMontage, float InPlayRate, FName StartSectionName);

EServerStatus serverStatus = EServerStatus::Down;

// modes:

static bool bIsPlayground = false;
static bool bIsLateGame = false;

static std::vector<UObject*> ExistingBuildings;

static bool bUseBeacons = true;

// change these ig

static constexpr bool bDoubleBuildFix2 = true;
static constexpr bool bDoubleBuildFix = false;
static bool bClearInventoryOnAircraftJump = !bIsPlayground;
static bool bIsInEvent = false;
static bool bPrintSpawnActor = false;
static bool bRestarting = false;
static bool bIsReadyToRestart = true; // i hate my life
static bool bIsTrickshotting = false; // this is stupid
static constexpr bool bAreVehicleWeaponsEnabled = false;
static std::string PickaxeDef = "FortWeaponMeleeItemDefinition /Game/Athena/Items/Weapons/WID_Harvest_Pickaxe_Athena_C_T01.WID_Harvest_Pickaxe_Athena_C_T01";
static bool bAutoRestart = false;
static int RestartSeconds = 10; // give them time to get slomo n stuff
static bool bSiphonEnabled = false;
static int maxAmountOfPlayersPerTeam = 1; // >S2
static bool bRandomCosmetics = true;

static std::string CIDToUse = "None";

// std::string PlaylistToUse = "FortPlaylistAthena /Game/Athena/Playlists/Playlist_DefaultSolo.Playlist_DefaultSolo";
std::string PlaylistToUse = "FortPlaylistAthena /Game/Athena/Playlists/Playlist_DefaultSquad.Playlist_DefaultSquad";
// "FortPlaylistAthena /Game/Athena/Playlists/Playground/Playlist_Playground.Playlist_Playground";
// "FortPlaylistAthena /Game/Athena/Playlists/Low/Playlist_Low_Solo.Playlist_Low_Solo";

static std::pair<std::string, int> StartingSlot1 = { "", 0 };// { "FortWeaponRangedItemDefinition /Game/Athena/Items/Weapons/WID_Assault_AutoHigh_Athena_SR_Ore_T03.WID_Assault_AutoHigh_Athena_SR_Ore_T03", 1 };
static std::pair<std::string, int> StartingSlot2 = { "", 0 }; // { "FortWeaponRangedItemDefinition /Game/Athena/Items/Weapons/WID_Shotgun_Standard_Athena_UC_Ore_T03.WID_Shotgun_Standard_Athena_UC_Ore_T03", 1 };
static std::pair<std::string, int> StartingSlot3 = { "", 0 }; // { "", 0 };
static std::pair<std::string, int> StartingSlot4 = { "", 0 }; // { "FortWeaponRangedItemDefinition /Game/Athena/Items/Consumables/ShieldSmall/Athena_ShieldSmall.Athena_ShieldSmall", 3 };
static std::pair<std::string, int> StartingSlot5 = { "", 0 }; // { "FortWeaponRangedItemDefinition /Game/Athena/Items/Consumables/PurpleStuff/Athena_PurpleStuff.Athena_PurpleStuff", 1 };

// dont change these thanks

static int AmountOfRestarts = 0;
static bool bInjectedOnStartup = false;
static bool boozasdgwq9i = false;
static UObject* GlobalPickaxeDefObject = nullptr;
constexpr bool bEmotingEnabled = false;
static bool bPrintFUnny = false;
static constexpr double AboveVersionDisableAbilities = 19.00; // Any version Above this abilities will be disabled
static bool zoneRet = false;

// highly experimental

static bool bIsSTW = false;
static bool bIsCreative = false;
static constexpr bool bAutomaticPawnSpawning = false;
static constexpr bool bExperimentalRespawning = false;
static constexpr bool bAISpawningEnabled = false;
static constexpr bool bSpawnAPC = true;
static constexpr bool bTeamsEnabled = false;

const wchar_t* GetMapName()
{
    // TBH we should get it from the playlist

    if (bIsCreative)
    {
        return L"/Game/Playgrounds/Maps/Islands/CreativeHub/PG_CreativeHub_03?game=/Game/Athena/Athena_GameMode.Athena_GameMode_C";
    }

    if (FnVerDouble >= 19.00)
        return L"Artemis_Terrain?game=/Game/Athena/Athena_GameMode.Athena_GameMode_C";
    else if (Engine_Version >= 424)
        return L"Apollo_Terrain?game=/Game/Athena/Athena_GameMode.Athena_GameMode_C";
    else if (Engine_Version < 424)
        return bIsSTW ? L"Zone_Outpost_Stonewood" : L"Athena_Terrain?game=/Game/Athena/Athena_GameMode.Athena_GameMode_C"; // L"DS_BuilderGridPlane?game=/Game/Athena/Athena_GameMode.Athena_GameMode_C";
    return L"";
}