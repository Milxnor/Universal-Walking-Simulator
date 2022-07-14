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

static __int64 (*GetNetMode)(__int64* a1);
bool (*LP_SpawnPlayActor)(UObject* Player, const FString& URL, FString& OutError, UObject* World); // LocalPlayer
__int64 (*CollectGarbage)(__int64 a1);

/* static FGameplayAbilitySpecHandle* (*GiveAbility)(UAbilitySystemComponent* _this, FGameplayAbilitySpecHandle* outHandle, FGameplayAbilitySpec inSpec);
static bool (*InternalTryActivateAbility)(UAbilitySystemComponent* _this, FGameplayAbilitySpecHandle Handle, FPredictionKey InPredictionKey, UGameplayAbility** OutInstancedAbility, void* OnGameplayAbilityEndedDelegate, FGameplayEventData* TriggerEventData);
static void (*MarkAbilitySpecDirty)(UAbilitySystemComponent* _this, FGameplayAbilitySpec& Spec); */

static void* (*SetWorld)(UObject* NetDriver, UObject* World);
static bool (*InitListen)(UObject* Driver, void* InNotify, FURL& LocalURL, bool bReuseAddressAndPort, FString& Error);
static void (*TickFlush)(UObject* NetDriver, float DeltaSeconds);

static void (*ServerReplicateActors)(UObject* ReplicationDriver);

static void (*ReceiveFString)(void* Bunch, FString& Str);
static void (*ReceiveUniqueIdRepl)(void* Bunch, void* Str);

static char (*KickPlayer)(UObject* a1, UObject*, void* /* FText */a3);

static void (*WelcomePlayer)(UObject* World, UObject* Connection);
static void (*World_NotifyControlMessage)(UObject* World, UObject* Connection, uint8_t MessageType, void* Bunch);
static UObject* (*SpawnPlayActor)(UObject* World, UObject* NewPlayer, ENetRole RemoteRole, FURL& URL, void* UniqueId, FString& Error, uint8_t NetPlayerIndex);

inline bool (*InitHost)(UObject* Beacon);
inline void (*Beacon_NotifyControlMessage)(UObject* World, UObject* Connection, uint8_t MessageType, void* Bunch);
inline void (*PauseBeaconRequests)(UObject* Beacon, bool bPause);

static void* (*NetDebug)(UObject* a);

static int ServerReplicateActorsOffset = 0x53; // UE4.20

static std::unordered_map<UFunction*, std::function<void(UObject*, UFunction*, void*)>> FunctionsToHook;

#define AddHook(str, func) FunctionsToHook.insert({FindObject<UFunction>(str), func});