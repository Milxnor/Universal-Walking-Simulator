#pragma once

#include <UE/structs.h>
#include <unordered_map>
#include <functional>
#include <unordered_set>


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

static void(__fastcall* SendClientAdjustment)(UObject* controller);
static void(__fastcall* CallPreReplication)(UObject* actor, UObject* driver);
static char(__fastcall* ReplicateActor)(UObject* ActorChannel);

static char (*KickPlayer)(UObject* a1, UObject*, FText a3); // session, pc

static void(__fastcall* HandleReloadCost)(UObject* Weapon, int AmountToRemove);

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

static __int64(__fastcall* ClientTravel)(UObject* Controller, const FString& URL, ETravelType TravelType, bool bSeamless, FGuid MapPackageGuid);

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
	if (FnVerDouble >= 19.00)
		return L"Artemis_Terrain?game=/Game/Athena/Athena_GameMode.Athena_GameMode_C";
	else if (Engine_Version >= 424)
		return L"Apollo_Terrain?game=/Game/Athena/Athena_GameMode.Athena_GameMode_C";
	else if (Engine_Version < 424)
		return L"Athena_Terrain?game=/Game/Athena/Athena_GameMode.Athena_GameMode_C";
	return L"";
}


// START ABILITIES

struct FGameplayAbilitySpecHandle
{
	int Handle;
	static inline std::unordered_set<int> Handles;

	void GenerateNewHandle()
	{
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
	int16_t Current;
	int16_t Base;
	unsigned char UnknownData00[0x4];
	UObject* PredictiveConnection;
	bool bIsStale;
	bool bIsServerInitiated;
	unsigned char UnknownData01[0x6];
};

struct FPredictionKeyFTS // 4.26
{
	UObject* PredictiveConnection;
	int16_t Current;
	int16_t Base;
	bool bIsStale;
	bool bIsServerInitiated;
	unsigned char UnknownData00[0x2];
};

struct FGameplayEffectContextHandle
{
	unsigned char UnknownData00[0x18];
};

struct FGameplayTag
{
	FName TagName;
};

struct FOnGameplayEffectTagCountChanged
{
	// I did math to figure this out...
	char pad[0x18];
};

struct FGameplayTagContainer
{
	TArray<FGameplayTag> GameplayTags;
	TArray<FGameplayTag> ParentTags;

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
	unsigned char UnknownData00[0x20];
};

struct FServerAbilityRPCBatchOL
{
	FGameplayAbilitySpecHandle AbilitySpecHandle;
	unsigned char UnknownData00[0x4];
	FPredictionKey PredictionKey;
	FGameplayAbilityTargetDataHandleOL TargetData;
	bool InputPressed;
	bool Ended;
	bool Started;
	unsigned char UnknownData01[0x5];
};

struct FGameplayEventDataOL
{
	FGameplayTag EventTag;
	UObject* Instigator;
	UObject* Target;
	UObject* OptionalObject;
	UObject* OptionalObject2;
	FGameplayEffectContextHandle ContextHandle;
	FGameplayTagContainer InstigatorTags;
	FGameplayTagContainer TargetTags;
	float EventMagnitude;
	unsigned char UnknownData00[0x4];
	FGameplayAbilityTargetDataHandleOL TargetData;
};

struct FGameplayAbilityTargetDataHandleSE
{
	unsigned char UnknownData00[0x28];
};

struct FServerAbilityRPCBatchSE
{
	FGameplayAbilitySpecHandle AbilitySpecHandle;
	unsigned char UnknownData00[0x4];
	FPredictionKey PredictionKey;
	FGameplayAbilityTargetDataHandleSE TargetData;
	bool InputPressed;
	bool Ended;
	bool Started;
	unsigned char UnknownData01[0x5];
};

struct FServerAbilityRPCBatchNewer
{
	FGameplayAbilitySpecHandle AbilitySpecHandle;
	unsigned char UnknownData00[0x4];
	char PredictionKey[0x18];
	FGameplayAbilityTargetDataHandleSE TargetData;
	bool InputPressed;
	bool Ended;
	bool Started;
	unsigned char UnknownData01[0x5];
};

struct FGameplayEventDataSE
{
	FGameplayTag EventTag;
	UObject* Instigator;
	UObject* Target;
	UObject* OptionalObject;
	UObject* OptionalObject2;
	FGameplayEffectContextHandle ContextHandle;
	FGameplayTagContainer InstigatorTags;
	FGameplayTagContainer TargetTags;
	float  EventMagnitude;
	unsigned char UnknownData00[0x4];
	FGameplayAbilityTargetDataHandleSE TargetData;
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
	TEnumAsByte<EGameplayAbilityActivationMode> ActivationMode;
	unsigned char bCanBeEndedByOtherInstance : 1;
	unsigned char UnknownData00[0x6];
	FPredictionKey PredictionKeyWhenActivated;
};

struct FServerAbilityRPCBatchFTS
{
	FGameplayAbilitySpecHandle AbilitySpecHandle;
	unsigned char UnknownData00[0x4];
	FPredictionKeyFTS PredictionKey;
	FGameplayAbilityTargetDataHandleSE TargetData;
	bool InputPressed;
	bool Ended;
	bool Started;
	unsigned char UnknownData01[0x5];
};

struct FGameplayAbilityActivationInfoFTS
{
	TEnumAsByte<EGameplayAbilityActivationMode> ActivationMode;
	unsigned char bCanBeEndedByOtherInstance : 1;
	unsigned char UnknownData00[0x6];
	FPredictionKeyFTS PredictionKeyWhenActivated;
};

struct FActiveGameplayEffectHandle
{
	int  Handle;
	bool bPassedFiltersAndWasExecuted;
	unsigned char UnknownData00[0x3];
};

template <typename ActivationInfoType>
struct FGameplayAbilitySpec : public FFastArraySerializerItem
{
	FGameplayAbilitySpecHandle Handle;
	UObject* Ability;
	int Level;
	int InputID;
	UObject* SourceObject;
	unsigned char ActiveCount;
	unsigned char InputPressed : 1;
	unsigned char RemoveAfterActivation : 1;
	unsigned char PendingRemove : 1;
	unsigned char UnknownData00[0x6];
	ActivationInfoType ActivationInfo;
	TArray<UObject*> NonReplicatedInstances;
	TArray<UObject*> ReplicatedInstances;
	FActiveGameplayEffectHandle  GameplayEffectHandle;
	unsigned char UnknownData01[0x50];
};

struct FGameplayAbilitySpecNewer : FFastArraySerializerItem {
	FGameplayAbilitySpecHandle Handle;
	UObject* Ability;
	int32_t Level;
	int32_t InputID;
	UObject* SourceObject;
	char ActiveCount;
	char InputPressed : 1;
	char RemoveAfterActivation : 1;
	char PendingRemove : 1;
	char bActivateOnce : 1;
	char UnknownData_29_4 : 4;
	char UnknownData_2A[0x6];
	FGameplayAbilityActivationInfoFTS ActivationInfo;
	FGameplayTagContainer DynamicAbilityTags;
	TArray<UObject*> NonReplicatedInstances;
	TArray<UObject*> ReplicatedInstances;
	FActiveGameplayEffectHandle GameplayEffectHandle;
	char UnknownData_90[0x50];
	void* dababy;
};

struct FGameplayAbilitySpecContainerSE : public FFastArraySerializerSE
{
	TArray<FGameplayAbilitySpec<FGameplayAbilityActivationInfo>> Items;
	UObject* Owner;
};

struct FGameplayAbilitySpecContainerFTS : public FFastArraySerializerSE
{
	TArray<FGameplayAbilitySpec<FGameplayAbilityActivationInfoFTS>> Items;
	UObject* Owner;
};

struct FGameplayAbilitySpecContainerNewer : public FFastArraySerializerSE
{
	TArray<FGameplayAbilitySpecNewer> Items;
	UObject* Owner;
};

struct FGameplayAbilitySpecContainerOL : public FFastArraySerializerOL
{
	TArray<FGameplayAbilitySpec<FGameplayAbilityActivationInfo>> Items;
	UObject* Owner;
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

static FGameplayAbilitySpecHandle* (*GiveAbility)(UObject* comp, FGameplayAbilitySpecHandle* outHandle, FGameplayAbilitySpec<FGameplayAbilityActivationInfo> inSpec);
static FGameplayAbilitySpecHandle* (*GiveAbilityTest)(UObject* comp, FGameplayAbilitySpecHandle* outHandle, __int64 inSpec);
static FGameplayAbilitySpecHandle* (*GiveAbilityFTS)(UObject* comp, FGameplayAbilitySpecHandle* outHandle, FGameplayAbilitySpec<FGameplayAbilityActivationInfoFTS> inSpec);
static FGameplayAbilitySpecHandle* (*GiveAbilityNewer)(UObject* comp, FGameplayAbilitySpecHandle* outHandle, FGameplayAbilitySpecNewer inSpec);

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
static bool bClearInventoryOnAircraftJump = !bIsPlayground;
static bool bIsInEvent = false;

std::string PlaylistToUse = "FortPlaylistAthena /Game/Athena/Playlists/Playlist_DefaultSolo.Playlist_DefaultSolo";

static std::pair<std::string, int> StartingSlot1 = { "FortWeaponRangedItemDefinition /Game/Athena/Items/Weapons/WID_Assault_AutoHigh_Athena_SR_Ore_T03.WID_Assault_AutoHigh_Athena_SR_Ore_T03", 1 };
static std::pair<std::string, int> StartingSlot2 = { "FortWeaponRangedItemDefinition /Game/Athena/Items/Weapons/WID_Shotgun_Standard_Athena_UC_Ore_T03.WID_Shotgun_Standard_Athena_UC_Ore_T03", 1 };
static std::pair<std::string, int> StartingSlot3 = { "", 0 };
static std::pair<std::string, int> StartingSlot4 = { "FortWeaponRangedItemDefinition /Game/Athena/Items/Consumables/ShieldSmall/Athena_ShieldSmall.Athena_ShieldSmall", 3 };
static std::pair<std::string, int> StartingSlot5 = { "FortWeaponRangedItemDefinition /Game/Athena/Items/Consumables/PurpleStuff/Athena_PurpleStuff.Athena_PurpleStuff", 1 };