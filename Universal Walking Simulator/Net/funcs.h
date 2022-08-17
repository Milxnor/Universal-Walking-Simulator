#pragma once

#include <UE/structs.h>
#include <unordered_map>
#include <functional>


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

    if (!funcObject)
        std::cout << "Unable to find Function: " << str << '\n';
    else
        FunctionsToHook.insert({ funcObject, func });
}

static auto AircraftLocationToUse = FVector{ 3500, -9180, 10500 };

const wchar_t* GetMapName()
{
	if (Engine_Version >= 424)
		return L"Apollo_Terrain?game=/Game/Athena/Athena_GameMode.Athena_GameMode_C";
	else if (Engine_Version < 424)
		return L"Athena_Terrain?game=/Game/Athena/Athena_GameMode.Athena_GameMode_C";
    return L"";
}


// START ABILITIES

struct FGameplayAbilitySpecHandle 
{ 
    int Handle; 

    void GenerateNewHandle()
    {
        // Must be in C++ to avoid duplicate statics accross execution units
        static int32_t GHandle = 1;
        Handle = GHandle++;
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

struct FGameplayEffectContextHandle
{
    unsigned char                                      UnknownData00[0x18];                                      // 0x0000(0x0018) MISSED OFFSET
};

struct FGameplayTag
{
    FName                                       TagName;                                                  // 0x0000(0x0008) (Edit, ZeroConstructor, EditConst, IsPlainOldData)
};

struct FOnGameplayEffectTagCountChanged
{
    // I did math to figure this out...
    char pad[0x18];
};

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
};

struct FGameplayAbilityTargetDataHandleOL
{
    unsigned char                                      UnknownData00[0x20];                                      // 0x0000(0x0020) MISSED OFFSET
};

struct FServerAbilityRPCBatchOL
{
    FGameplayAbilitySpecHandle                  AbilitySpecHandle;                                        // 0x0000(0x0004)
    unsigned char                                      UnknownData00[0x4];                                       // 0x0004(0x0004) MISSED OFFSET
    FPredictionKey                              PredictionKey;                                            // 0x0008(0x0018)
    FGameplayAbilityTargetDataHandleOL            TargetData;                                               // 0x0020(0x0020)
    bool                                               InputPressed;                                             // 0x0040(0x0001) (ZeroConstructor, IsPlainOldData)
    bool                                               Ended;                                                    // 0x0041(0x0001) (ZeroConstructor, IsPlainOldData)
    bool                                               Started;                                                  // 0x0042(0x0001) (ZeroConstructor, IsPlainOldData, RepSkip, RepNotify, Interp, NonTransactional, EditorOnly, NoDestructor, AutoWeak, ContainsInstancedReference, AssetRegistrySearchable, SimpleDisplay, AdvancedDisplay, Protected, BlueprintCallable, BlueprintAuthorityOnly, TextExportTransient, NonPIEDuplicateTransient, ExposeOnSpawn, PersistentInstance, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic, NativeAccessSpecifierProtected, NativeAccessSpecifierPrivate)
    unsigned char                                      UnknownData01[0x5];                                       // 0x0043(0x0005) MISSED OFFSET
};

struct FGameplayEventDataOL
{
    FGameplayTag                                EventTag;                                                 // 0x0000(0x0008) (Edit, BlueprintVisible)
    UObject* Instigator;                                               // 0x0008(0x0008) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
    UObject* Target;                                                   // 0x0010(0x0008) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
    UObject* OptionalObject;                                           // 0x0018(0x0008) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
    UObject* OptionalObject2;                                          // 0x0020(0x0008) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
    FGameplayEffectContextHandle                ContextHandle;                                            // 0x0028(0x0018) (Edit, BlueprintVisible)
    FGameplayTagContainer                       InstigatorTags;                                           // 0x0040(0x0020) (Edit, BlueprintVisible)
    FGameplayTagContainer                       TargetTags;                                               // 0x0060(0x0020) (Edit, BlueprintVisible)
    float                                              EventMagnitude;                                           // 0x0080(0x0004) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
    unsigned char                                      UnknownData00[0x4];                                       // 0x0084(0x0004) MISSED OFFSET
    FGameplayAbilityTargetDataHandleOL TargetData;                                               // 0x0088(0x0020) (Edit, BlueprintVisible)
};

struct FGameplayAbilityTargetDataHandleSE
{
    unsigned char                                      UnknownData00[0x28];                                      // 0x0000(0x0028) MISSED OFFSET
    // unsigned char                                      UnknownData00[0x20];                                      // 0x0000(0x0020) MISSED OFFSET // 8.20..
};

struct FServerAbilityRPCBatchSE
{
    FGameplayAbilitySpecHandle                  AbilitySpecHandle;                                        // 0x0000(0x0004)
    unsigned char                                      UnknownData00[0x4];                                       // 0x0004(0x0004) MISSED OFFSET
    FPredictionKey                              PredictionKey;                                            // 0x0008(0x0018)
    FGameplayAbilityTargetDataHandleSE           TargetData;                                               // 0x0020(0x0020)
    bool                                               InputPressed;                                             // 0x0040(0x0001) (ZeroConstructor, IsPlainOldData)
    bool                                               Ended;                                                    // 0x0041(0x0001) (ZeroConstructor, IsPlainOldData)
    bool                                               Started;                                                  // 0x0042(0x0001) (ZeroConstructor, IsPlainOldData, RepSkip, RepNotify, Interp, NonTransactional, EditorOnly, NoDestructor, AutoWeak, ContainsInstancedReference, AssetRegistrySearchable, SimpleDisplay, AdvancedDisplay, Protected, BlueprintCallable, BlueprintAuthorityOnly, TextExportTransient, NonPIEDuplicateTransient, ExposeOnSpawn, PersistentInstance, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic, NativeAccessSpecifierProtected, NativeAccessSpecifierPrivate)
    unsigned char                                      UnknownData01[0x5];                                       // 0x0043(0x0005) MISSED OFFSET
};

struct FGameplayEventDataSE
{
    FGameplayTag                                EventTag;                                                 // 0x0000(0x0008) (Edit, BlueprintVisible)
    UObject* Instigator;                                               // 0x0008(0x0008) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
    UObject* Target;                                                   // 0x0010(0x0008) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
    UObject* OptionalObject;                                           // 0x0018(0x0008) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
    UObject* OptionalObject2;                                          // 0x0020(0x0008) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
    FGameplayEffectContextHandle                ContextHandle;                                            // 0x0028(0x0018) (Edit, BlueprintVisible)
    FGameplayTagContainer                       InstigatorTags;                                           // 0x0040(0x0020) (Edit, BlueprintVisible)
    FGameplayTagContainer                       TargetTags;                                               // 0x0060(0x0020) (Edit, BlueprintVisible)
    float                                              EventMagnitude;                                           // 0x0080(0x0004) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
    unsigned char                                      UnknownData00[0x4];                                       // 0x0084(0x0004) MISSED OFFSET
    FGameplayAbilityTargetDataHandleSE TargetData;                                               // 0x0088(0x0020) (Edit, BlueprintVisible)
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

struct FServerAbilityRPCBatchFTS
{
    FGameplayAbilitySpecHandle                  AbilitySpecHandle;                                        // 0x0000(0x0004)
    unsigned char                                      UnknownData00[0x4];                                       // 0x0004(0x0004) MISSED OFFSET
    FPredictionKeyFTS                              PredictionKey;                                            // 0x0008(0x0018)
    FGameplayAbilityTargetDataHandleSE           TargetData;                                               // 0x0020(0x0020)
    bool                                               InputPressed;                                             // 0x0040(0x0001) (ZeroConstructor, IsPlainOldData)
    bool                                               Ended;                                                    // 0x0041(0x0001) (ZeroConstructor, IsPlainOldData)
    bool                                               Started;                                                  // 0x0042(0x0001) (ZeroConstructor, IsPlainOldData, RepSkip, RepNotify, Interp, NonTransactional, EditorOnly, NoDestructor, AutoWeak, ContainsInstancedReference, AssetRegistrySearchable, SimpleDisplay, AdvancedDisplay, Protected, BlueprintCallable, BlueprintAuthorityOnly, TextExportTransient, NonPIEDuplicateTransient, ExposeOnSpawn, PersistentInstance, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic, NativeAccessSpecifierProtected, NativeAccessSpecifierPrivate)
    unsigned char                                      UnknownData01[0x5];                                       // 0x0043(0x0005) MISSED OFFSET
};

struct FGameplayAbilityActivationInfoFTS
{
    TEnumAsByte<EGameplayAbilityActivationMode>        ActivationMode;                                           // 0x0000(0x0001) (BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData)
    unsigned char                                      bCanBeEndedByOtherInstance : 1;                           // 0x0001(0x0001)
    unsigned char                                      UnknownData00[0x6];                                       // 0x0002(0x0006) MISSED OFFSET
    FPredictionKeyFTS                              PredictionKeyWhenActivated;                               // 0x0008(0x0018)
};

struct FActiveGameplayEffectHandle
{
    int                                                Handle;                                                   // 0x0000(0x0004) (ZeroConstructor, IsPlainOldData)
    bool                                               bPassedFiltersAndWasExecuted;                             // 0x0004(0x0001) (ZeroConstructor, IsPlainOldData)
    unsigned char                                      UnknownData00[0x3];                                       // 0x0005(0x0003) MISSED OFFSET
};

template <typename ActivationInfoType>
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
    /*
    
                                THIS DOESNT EXIST ON OLDER VERSIONS      ^^^
    
    */                             
    FActiveGameplayEffectHandle                 GameplayEffectHandle; // UGameplayAbility*
    unsigned char                                      UnknownData01[0x50];                                      // 0x0078(0x0050) MISSED OFFSET
};

struct FGameplayAbilitySpecContainerSE : public FFastArraySerializerSE
{
    TArray<FGameplayAbilitySpec<FGameplayAbilityActivationInfo>>                Items;                                                    // 0x00B0(0x0010) (ZeroConstructor)
    UObject* Owner; // ASC* // 0x0118
};

struct FGameplayAbilitySpecContainerFTS : public FFastArraySerializerSE
{
    TArray<FGameplayAbilitySpec<FGameplayAbilityActivationInfoFTS>>                Items;                                                    // 0x00B0(0x0010) (ZeroConstructor)
    UObject* Owner; // ASC* // 0x0118
};

struct FGameplayAbilitySpecContainerOL : public FFastArraySerializerOL
{
    TArray<FGameplayAbilitySpec<FGameplayAbilityActivationInfo>>                Items;                                                    // 0x00B0(0x0010) (ZeroConstructor)
    UObject* Owner; // ASC* // 0x0118
};

enum class EGameplayAbilityInstancingPolicy : uint8_t
{
    NonInstanced = 0,
    InstancedPerActor = 1,
    InstancedPerExecution = 2,
    EGameplayAbilityInstancingPolicy_MAX = 3
};

struct FGameplayAbilitySpecHandleAndPredictionKey
{
    FGameplayAbilitySpecHandle AbilityHandle;

    int32_t PredictionKeyAtCreation;
};

/* struct FAbilityReplicatedDataCache
{
    FGameplayAbilityTargetDataHandle TargetData;
    FGameplayTag ApplicationTag;
    bool bTargetConfirmed;
    bool bTargetCancelled;
    char TargetSetDelegate[0x18];
    FSimpleMulticastDelegate TargetCancelledDelegate;
    FAbilityReplicatedData	GenericEvents[EAbilityGenericReplicatedEvent::MAX];
    FPredictionKey PredictionKey;
};

struct FGameplayAbilityReplicatedDataContainer
{
public:

    // typedef TPair<FGameplayAbilitySpecHandleAndPredictionKey, TSharedRef<FAbilityReplicatedDataCache>> FKeyDataPair;

    char InUseData[0x10];
    TArray<TSharedRef<FAbilityReplicatedDataCache>> FreeData;
};  */

static FGameplayAbilitySpecHandle* (*GiveAbility)(UObject* comp, FGameplayAbilitySpecHandle* outHandle, FGameplayAbilitySpec<FGameplayAbilityActivationInfo> inSpec);
static FGameplayAbilitySpecHandle* (*GiveAbilityTest)(UObject* comp, FGameplayAbilitySpecHandle* outHandle,__int64 inSpec);
static FGameplayAbilitySpecHandle* (*GiveAbilityFTS)(UObject* comp, FGameplayAbilitySpecHandle* outHandle, FGameplayAbilitySpec<FGameplayAbilityActivationInfoFTS> inSpec);

static bool (*InternalTryActivateAbility)(UObject* comp, FGameplayAbilitySpecHandle Handle, FPredictionKey InPredictionKey, UObject** /* UGameplayAbility** */ OutInstancedAbility, void* OnGameplayAbilityEndedDelegate, __int64* TriggerEventData); // // https://github.com/EpicGames/UnrealEngine/blob/46544fa5e0aa9e6740c19b44b0628b72e7bbd5ce/Engine/Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Private/AbilitySystemComponent_Abilities.cpp#L1327
static bool (*InternalTryActivateAbilityFTS)(UObject* comp, FGameplayAbilitySpecHandle Handle, FPredictionKeyFTS InPredictionKey, UObject** /* UGameplayAbility** */ OutInstancedAbility, void* OnGameplayAbilityEndedDelegate, __int64* TriggerEventData); // // https://github.com/EpicGames/UnrealEngine/blob/46544fa5e0aa9e6740c19b44b0628b72e7bbd5ce/Engine/Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Private/AbilitySystemComponent_Abilities.cpp#L1327

static void (*MarkAbilitySpecDirtyOld)(UObject* comp, FGameplayAbilitySpec<FGameplayAbilityActivationInfo>& Spec);
static void (*MarkAbilitySpecDirtyFTS)(UObject* comp, FGameplayAbilitySpec<FGameplayAbilityActivationInfoFTS>& Spec);

void MarkAbilitySpecDirty(UObject* ASC, void* Spec)
{
    if (Engine_Version < 426)
        MarkAbilitySpecDirtyOld(ASC, *(FGameplayAbilitySpec<FGameplayAbilityActivationInfo>*)Spec);
    else
        MarkAbilitySpecDirtyFTS(ASC, *(FGameplayAbilitySpec<FGameplayAbilityActivationInfoFTS>*)Spec);
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

static bool bDoubleBuildFix = false;

std::string PlaylistToUse = "FortPlaylistAthena /Game/Athena/Playlists/Playground/Playlist_Playground.Playlist_Playground";