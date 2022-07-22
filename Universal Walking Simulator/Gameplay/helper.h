#pragma once

#include <UE/structs.h>
#include <Net/funcs.h>


UObject* GetWorldW(bool bReset = false)
{
	auto GameViewport = *GetEngine()->Member<UObject*>(_("GameViewport"));

	if (GameViewport)
		return *GameViewport->Member<UObject*>(_("World")); // we could also find the world by name but that depends on the map

	return nullptr;
}

namespace Easy
{
	UObject* SpawnObject(UObject* ObjectClass, UObject* Outer)
	{
		if (!ObjectClass || !Outer)
			return nullptr;

		struct {
			UObject* ObjectClass;
			UObject* Outer;
			UObject* ReturnValue;
		} params{};

		params.ObjectClass = ObjectClass;
		params.Outer = Outer;

		static auto GSC = FindObject(_("GameplayStatics /Script/Engine.Default__GameplayStatics"));
		static auto fn = GSC->Function(_("SpawnObject"));

		GSC->ProcessEvent(fn, &params);

		return params.ReturnValue;
	}

	UObject* SpawnActor(UObject* Class, const FVector& Location = FVector(), const FRotator& Rotation = FRotator())
	{
		auto Loc = Location;
		auto Rot = Rotation;
		return SpawnActorO(GetWorldW(), Class, &Loc, &Rot, FActorSpawnParameters());
	}
}

namespace Helper
{
	UObject* GetWorld()
	{
		return GetWorldW();
	}

	static void ChoosePart(UObject* Pawn, TEnumAsByte<EFortCustomPartType> Part, UObject* ChosenCharacterPart)
	{
		/* if (std::stod(FN_Version) >= 6) // TODO: make sure this is actually when they change it.
		{
			auto PlayerState = *Pawn->Member<UObject*>(_("PlayerState"));
			auto CustomCharacterParts = PlayerState->Member<__int64>(_("CharacterParts")); // FCustomCharacterParts
			struct ahh
			{
				char WasReplicatedFlags; // 0x00(0x01)
				char UnknownData_1[0x7]; // 0x01(0x07)
				UObject* Parts[0x06];
			};
			UObject** Parts = ((ahh*)CustomCharacterParts)->Parts;
			Parts[Part.Get()] = ChosenCharacterPart;
		}
		else */
		{
			struct {
				TEnumAsByte<EFortCustomPartType> Part;
				UObject* ChosenCharacterPart;
			} params{ Part, ChosenCharacterPart };

			static auto chooseFn = Pawn->Function(_("ServerChoosePart"));
			Pawn->ProcessEvent(chooseFn, &params);
		}
	}

	FVector GetActorLocation(UObject* Actor)
	{
		static auto K2_GetActorLocationFN = Actor->Function(_("K2_GetActorLocation"));

		if (K2_GetActorLocationFN)
		{
			FVector loc;
			Actor->ProcessEvent(K2_GetActorLocationFN, &loc);
			return loc;
		}
		else
			K2_GetActorLocationFN = Actor->Function(_("K2_GetActorLocation"));

		return FVector();
	}

	UObject* SummonPickup(UObject* Pawn, UObject* Definition, FVector Location, EFortPickupSourceTypeFlag PickupSource, EFortPickupSpawnSource SpawnSource, int Count = 1, bool bTossPickup = true)
	{
		static UObject* PickupClass = FindObject(_("Class /Script/FortniteGame.FortPickupAthena"));

		auto Pickup = Easy::SpawnActor(PickupClass, Location);

		if (Pickup && Definition)
		{
			auto ItemEntry = Pickup->Member<void>(_("PrimaryPickupItemEntry"));
			static auto CountOffset = FindOffsetStruct(_("ScriptStruct /Script/FortniteGame.FortItemEntry"), _("Count"));
			static auto ItemDefOffset = FindOffsetStruct(_("ScriptStruct /Script/FortniteGame.FortItemEntry"), _("ItemDefinition"));

			static auto OnRep_PrimaryPickupItemEntry = Pickup->Function(_("OnRep_PrimaryPickupItemEntry"));

			if (ItemEntry && OnRep_PrimaryPickupItemEntry)
			{
				*(int*)(__int64(ItemEntry) + CountOffset) = Count;
				*(UObject**)(__int64(ItemEntry) + ItemDefOffset) = Definition;

				Pickup->ProcessEvent(OnRep_PrimaryPickupItemEntry);

				if (bTossPickup)
				{
					static auto TossPickupFn = Pickup->Function(_("TossPickup"));

					struct {
						FVector FinalLocation;
						UObject* ItemOwner;
						int OverrideMaxStackCount;
						bool bToss;
						EFortPickupSourceTypeFlag InPickupSourceTypeFlags; // Do these even exist on older versions?
						EFortPickupSpawnSource InPickupSpawnSource;
					} TPParams{ Location, Pawn, 6, true, PickupSource, SpawnSource };

					if (TossPickupFn)
						Pickup->ProcessEvent(TossPickupFn, &TPParams);
				}
			}
		}

		return nullptr;
	}

	void DestroyActor(UObject* Actor)
	{
		if (!Actor) 
			return;

		static auto fn = Actor->Function(_("K2_DestroyActor"));

		if (fn)
			Actor->ProcessEvent(fn);
		else
			std::cout << _("Failed to find K2_DestroyActor function!\n");
	}

	FRotator GetActorRotation(UObject* Actor)
	{
		static auto K2_GetActorRotationFN = Actor->Function(_("K2_GetActorRotation"));

		if (K2_GetActorRotationFN)
		{
			FRotator loc;
			Actor->ProcessEvent(K2_GetActorRotationFN, &loc);
			return loc;
		}
		else
			K2_GetActorRotationFN = Actor->Function(_("K2_GetActorRotation"));

		return FRotator();
	}

	static TArray<UObject*> GetAllActorsOfClass(UObject* Class)
	{
		static auto GSCClass = FindObject(_("GameplayStatics /Script/Engine.Default__GameplayStatics"));

		static UObject* GetAllActorsOfClass = GSCClass->Function(_("GetAllActorsOfClass"));

		struct
		{
			UObject* World;
			UObject* Class;
			TArray<UObject*> ReturnValue;
		} Params;

		Params.World = GetWorld();
		Params.Class = Class;

		ProcessEventO(GSCClass, GetAllActorsOfClass, &Params);

		return Params.ReturnValue;
	}

	static bool IsRespawnEnabled()
	{
		static bool Respawning = true;
		return Respawning;
	}

	static void InitializeBuildingActor(UObject* Controller, UObject* BuildingActor, bool bUsePlayerBuildAnimations = false)
	{
		// 	void InitializeKismetSpawnedBuildingActor(class ABuildingActor* BuildingOwner, class AFortPlayerController* SpawningController, bool bUsePlayerBuildAnimations = true);
		struct {
			UObject* BuildingOwner; // ABuildingActor
			UObject* SpawningController;
			bool bUsePlayerBuildAnimations; // I think this is not on some versions
		} IBAParams{BuildingActor, Controller, bUsePlayerBuildAnimations};

		if (Controller && BuildingActor)
		{
			static auto fn = BuildingActor->Function(_("InitializeKismetSpawnedBuildingActor"));

			if (fn)
				BuildingActor->ProcessEvent(fn, &IBAParams);
		}
	}

	static UObject* SpawnChip(UObject* Controller)
	{
		static auto ChipClass = FindObject(_("Class /Script/FortniteGame.BuildingGameplayActorSpawnChip"));

		auto Pawn = Controller->Member<UObject*>(_("Pawn"));

		if (ChipClass && Pawn && *Pawn)
		{
			auto PlayerState = (*Pawn)->Member<UObject*>(_("PlayerState"));

			if (PlayerState && *PlayerState)
			{
				std::cout << _("Spawning Chip!\n");

				auto Chip = Easy::SpawnActor(ChipClass, Helper::GetActorLocation(*Pawn), Helper::GetActorRotation(*Pawn));

				std::cout << _("Initializing Chip!\n");

				Helper::InitializeBuildingActor(Controller, Chip);

				std::cout << _("Initialized Chip!\n");

				*Chip->Member<UObject*>(_("OwnerPlayerController")) = Controller;
				*Chip->Member<UObject*>(_("OwnerPlayerState")) = *PlayerState;
				*Chip->Member<unsigned char>(_("SquadId")) = *(*PlayerState)->Member<unsigned char>(_("SquadId"));
				*Chip->Member<__int64>(_("OwnerPlayerId")) = *(*PlayerState)->Member<__int64>(_("UniqueId"));

				struct FRebootCardReplicatedState {
					float                                              ChipExpirationServerStartTime;                            // 0x0000(0x0004) (ZeroConstructor, IsPlainOldData)
					unsigned char                                      UnknownData00[0x4];                                       // 0x0004(0x0004) MISSED OFFSET
					UObject* PlayerState;                                              // 0x0008(0x0008) (ZeroConstructor, IsPlainOldData)
				};

				auto CardReplicatedState = FRebootCardReplicatedState{};

				CardReplicatedState.PlayerState = *PlayerState;

				*Chip->Member<FRebootCardReplicatedState>(_("RebootCardReplicatedState")) = CardReplicatedState;
				Chip->ProcessEvent(_("OnRep_RebootCardReplicatedState"));

				return Chip;
			}
		}
		else
			std::cout << _("Unable to find ChipClass!\n");

		return nullptr;
	}

	bool TeleportTo(UObject* Actor, FVector Location, FRotator Rot = FRotator())
	{
		struct {
			FVector& Location;
			FRotator& Rotation; // DestinationActor->GetActorRotation());
			bool Res;
		} params{ Location, Rot, false };

		auto fn = Actor->Function(_("K2_TeleportTo"));

		if (fn)
			Actor->ProcessEvent(fn, &params);

		return params.Res;
	}

	EAthenaGamePhase* GetGamePhase()
	{
		auto world = Helper::GetWorld();
		auto gameState = *world->Member<UObject*>(_("GameState"));
		return gameState->Member<EAthenaGamePhase>(_("GamePhase"));
	}

	FVector GetPlayerStart()
	{
		static auto WarmupClass = FindObject(_("Class /Script/FortniteGame.FortPlayerStartWarmup"));
		TArray<UObject*> OutActors = GetAllActorsOfClass(WarmupClass);

		auto ActorsNum = OutActors.Num();

		auto SpawnTransform = FTransform();
		SpawnTransform.Scale3D = FVector(1, 1, 1);
		SpawnTransform.Rotation = FQuat();
		SpawnTransform.Translation = FVector{ 1250, 1818, 3284 }; // Next to salty

		auto GamePhase = *GetGamePhase();

		if (WarmupClass && ActorsNum != 0 && Engine_Version < 423 && (GamePhase == EAthenaGamePhase::Setup || GamePhase == EAthenaGamePhase::Warmup))
		{
			auto ActorToUseNum = RandomIntInRange(0, ActorsNum);
			auto ActorToUse = (OutActors)[ActorToUseNum];

			while (!ActorToUse)
			{
				ActorToUseNum = RandomIntInRange(0, ActorsNum);
				ActorToUse = (OutActors)[ActorToUseNum];
			}

			SpawnTransform.Translation = GetActorLocation(ActorToUse);

			// PC->WarmupPlayerStart = static_cast<AFortPlayerStartWarmup*>(ActorToUse);
		}

		OutActors.Free();

		return SpawnTransform.Translation;
	}
	void LaunchPlayer(UObject* Pawn, FVector LaunchVelocity, bool bXYOverride, bool bZOverride, bool bIgnoreFallDamage, bool bPlayFeedbackEvent)
	{
		struct {
			FVector LaunchVelocity;
			bool bXYOverride;
			bool bZOverride;
			bool bIgnoreFallDamage;
			bool bPlayFeedbackEvent;
		} LCJParans {LaunchVelocity, bXYOverride, bZOverride, bIgnoreFallDamage, bPlayFeedbackEvent };
		auto LaunchPlayerFn = Pawn->Function("LaunchCharacterJump");
		Pawn->ProcessEvent(LaunchPlayerFn, &LCJParans);
	}

	namespace Console
	{
		static UObject** ViewportConsole = nullptr;

		DWORD WINAPI Setup(LPVOID)
		{
			static auto Engine = FindObjectOld(_("FortEngine_"));

			while (!Engine)
			{
				Engine = FindObjectOld(_("FortEngine_"));
				Sleep(1000 / 30);
			}

			static auto ConsoleClass = FindObject(_("Class /Script/Engine.Console"));
			static auto GameViewport = Engine->Member<UObject*>(_("GameViewport"));

			while (!*GameViewport)
			{
				GameViewport = Engine->Member<UObject*>(_("GameViewport"));
				Sleep(1000 / 30);
			}

			ViewportConsole = (*GameViewport)->Member<UObject*>(_("ViewportConsole"));

			auto ConsoleObject = Easy::SpawnObject(ConsoleClass, *GameViewport);

			if (ConsoleObject)
				*ViewportConsole = ConsoleObject;
			else
				std::cout << _("[WARNING] SpawnObject failed to create console!\n");

			return 0;
		}

		void ExecuteConsoleCommand(FString& Command)
		{
			struct {
				UObject* WorldContextObject;                                       // (Parm, ZeroConstructor, IsPlainOldData)
				FString                                     Command;                                                  // (Parm, ZeroConstructor)
				UObject* SpecificPlayer;                                           // (Parm, ZeroConstructor, IsPlainOldData)
			} params{Helper::GetWorld(), Command, nullptr};

			static auto KSLClass = FindObject(_("KismetSystemLibrary /Script/Engine.Default__KismetSystemLibrary"));

			if (KSLClass)
			{
				// static auto ExecuteConsoleCommandFn = KSLClass->Function(_("ExecuteConsoleCommand"));
				static auto ExecuteConsoleCommandFn = FindObject(_("Function /Script/Engine.KismetSystemLibrary.ExecuteConsoleCommand"));

				if (ExecuteConsoleCommandFn)
					KSLClass->ProcessEvent(ExecuteConsoleCommandFn, &params);
				else
					std::cout << _("No ExecuteConsoleCommand!\n");
			}
			else
				std::cout << _("No KismetSyustemLibrary!\n");
		}
	}

	void SetOwner(UObject* Actor, UObject* Owner)
	{
		*Actor->Member<UObject*>(_("Owner")) = Owner; // TODO: Call SetOwner
		static auto OnRepOwner = Actor->Function(_("OnRep_Owner"));
		if (OnRepOwner)
			Actor->ProcessEvent(OnRepOwner);
	}

	static UObject* GetVehicle(UObject* Pawn)
	{
		static auto fn = Pawn->Function(_("GetVehicle"));

		UObject* Vehicle;
		Pawn->ProcessEvent(fn, &Vehicle);

		return Vehicle;
	}

	void SetLocalRole(UObject* Actor, ENetRole LocalRole)
	{
		*Actor->Member<ENetRole>(_("Role")) = LocalRole;
	}

	void SetRemoteRole(UObject* Actor, ENetRole RemoteRole)
	{
		*Actor->Member<ENetRole>(_("RemoteRole")) = RemoteRole;
	}

	static void SetActorScale3D(UObject* Actor)
	{

	}

	UObject* InitPawn(UObject* PC, bool bResetCharacterParts = false, FVector Location = Helper::GetPlayerStart(), bool bResetTeams = false)
	{
		static const auto FnVerDouble = std::stod(FN_Version);

		UObject* PlayerState = *PC->Member<UObject*>(_("PlayerState"));

		static auto PawnClass = FindObject(_("BlueprintGeneratedClass /Game/Athena/PlayerPawn_Athena.PlayerPawn_Athena_C"));
		auto Pawn = Easy::SpawnActor(PawnClass, Location, {});

		if (!Pawn)
			return nullptr;

		static auto SetReplicateMovementFn = Pawn->Function(_("SetReplicateMovement"));
		struct {
			bool b;
		} bruh{true};
		Pawn->ProcessEvent(SetReplicateMovementFn, &bruh);

		// stupidBitField[0] = 1;
		static auto Rep_ReplicateMovement = Pawn->Function(_("OnRep_ReplicateMovement"));
		Pawn->ProcessEvent(Rep_ReplicateMovement);

		static auto Rep_ReplicatedMovement = Pawn->Function(_("OnRep_ReplicatedMovement"));
		Pawn->ProcessEvent(Rep_ReplicatedMovement);

		static auto Rep_ReplicatedBasedMovement = Pawn->Function(_("OnRep_ReplicatedBasedMovement"));
		Pawn->ProcessEvent(Rep_ReplicateMovement);

		if (Pawn)
		{
			auto PossessFn = PC->Function(_("Possess"));
			if (PossessFn)
			{
				struct {
					UObject* InPawn;
				} params{ Pawn }; // idk man
				PC->ProcessEvent(PossessFn, &params);
			}
			else
				std::cout << _("Could not find Possess!\n");
		}
		else
			std::cout << _("No Pawn!\n");

		/* *Pawn->Member<char>(_("bCanBeDamaged")) = false;
		*Pawn->Member<char>(_("bAlwaysRelevant")) = true;
		*Pawn->Member<char>(_("bReplicates")) = true;
		*Pawn->Member<char>(_("bOnlyRelevantToOwner")) = false; */
		Helper::SetOwner(Pawn, PC);

		*PC->Member<char>(_("bReadyToStartMatch")) = true;
		*PC->Member<char>(_("bClientPawnIsLoaded")) = true;
		*PC->Member<char>(_("bHasInitiallySpawned")) = true;

		*PC->Member<bool>(_("bHasServerFinishedLoading")) = true;
		*PC->Member<bool>(_("bHasClientFinishedLoading")) = true;

		*PlayerState->Member<char>(_("bHasStartedPlaying")) = true;
		*PlayerState->Member<char>(_("bHasFinishedLoading")) = true;
		*PlayerState->Member<char>(_("bIsReadyToContinue")) = true;

		// *Pawn->Member<float>(_("NetUpdateFrequency")) = 200;

		static auto setMaxHealthFn = Pawn->Function(_("SetMaxHealth"));
		struct { float NewHealthVal; }healthParams{ 100 };

		if (setMaxHealthFn)
			Pawn->ProcessEvent(setMaxHealthFn, &healthParams);
		else
			std::cout << _("Unable to find setMaxHealthFn!\n");

		static auto setMaxShieldFn = Pawn->Function(_("SetMaxShield"));
		struct { float NewValue; }shieldParams{ 100 };

		if (setMaxShieldFn)
			Pawn->ProcessEvent(setMaxShieldFn, &shieldParams);
		else
			std::cout << _("Unable to find setMaxShieldFn!\n");

		static const auto HeroType = FindObject(_("FortHeroType /Game/Athena/Heroes/HID_058_Athena_Commando_M_SkiDude_GER.HID_058_Athena_Commando_M_SkiDude_GER"));

		*PlayerState->Member<UObject*>(_("HeroType")) = HeroType;
		static auto OnRepHeroType = PlayerState->Function(_("OnRep_HeroType"));
		PlayerState->ProcessEvent(OnRepHeroType);

		static auto headPart = FindObject(_("CustomCharacterPart /Game/Characters/CharacterParts/Female/Medium/Heads/F_Med_Head1.F_Med_Head1"));
		static auto bodyPart = FindObject(_("CustomCharacterPart /Game/Characters/CharacterParts/Female/Medium/Bodies/F_Med_Soldier_01.F_Med_Soldier_01"));

		if (headPart && bodyPart && bResetCharacterParts)
		{
			Helper::ChoosePart(Pawn, EFortCustomPartType::Head, headPart);
			Helper::ChoosePart(Pawn, EFortCustomPartType::Body, bodyPart);
			static auto OnRep_Parts = (FnVerDouble >= 10) ? PlayerState->Function(_("OnRep_CharacterData")) : PlayerState->Function(_("OnRep_CharacterParts")); //Make sure its s10 and up

			if (OnRep_Parts)
				PlayerState->ProcessEvent(OnRep_Parts, nullptr);
		}

		if (Engine_Version < 423)
		{
			static auto OnRepSquadIdFn = PlayerState->Function(_("OnRep_SquadId"));
			static auto OnRepPlayerFn = PlayerState->Function(_("OnRep_PlayerTeam"));

			static uint8_t TeamIndex = 3;
			static const int SquadId = 1;

			*PlayerState->Member<unsigned char>(_("SquadId")) = SquadId;
			*PlayerState->Member<uint8_t>(_("TeamIndex")) = TeamIndex;

			/*static uint8_t TeamIndex = 3;
			static int SquadId = 1;
			static std::vector<UObject*> Members;

			*PlayerState->Member<uint8_t>(_("TeamIndex")) = TeamIndex;
			auto PlayerTeam = *PlayerState->Member<UObject*>(_("PlayerTeam")); // AFortTeamInfo
			*PlayerTeam->Member<uint8_t>(_("Team")) = TeamIndex;
			*PlayerState->Member<unsigned char>(_("SquadId")) = SquadId;
			SquadId++;

			for (auto& _Member : Members)
			{
				PlayerTeam->Member<TArray<UObject*>>(_("TeamMembers"))->Add(_Member); // AController
			} */

			if (OnRepSquadIdFn)
				PlayerState->ProcessEvent(OnRepSquadIdFn);
			else
				std::cout << _("Unable to find OnRepSquadIdFn!\n");

			if (OnRepPlayerFn)
				PlayerState->ProcessEvent(OnRepPlayerFn);
			else
				std::cout << _("Unable to find OnRepPlayerFn!\n");

			TeamIndex++;
		}

		return Pawn;
	}
	
	
	namespace Conversion
	{
		UObject* SoftObjectToObject(TSoftObjectPtr SoftObject)
		{
			// Function /Script/Engine.KismetSystemLibrary.Conv_SoftObjectReferenceToObject

			static auto KSLClass = FindObject(_("KismetSystemLibrary /Script/Engine.Default__KismetSystemLibrary"));

			static auto fn = KSLClass->Function(_("Conv_SoftObjectReferenceToObject"));

			UObject* Object;

			struct {
				TSoftObjectPtr SoftObject;
				UObject* ReturnValue;
			} params{SoftObject, nullptr};

			if (fn)
				KSLClass->ProcessEvent(fn, &Object);

			return params.ReturnValue;
		}
	}

	void SilentDie(UObject* BuildingActor)
	{
		static auto fn = BuildingActor->Function(_("SilentDie"));

		if (fn)
			BuildingActor->ProcessEvent(fn);
	}

	namespace Abilities
	{
		void ClientActivateAbilityFailed(UObject* ASC, FGameplayAbilitySpecHandle AbilityToActivate, int16_t PredictionKey)
		{
			struct
			{
				FGameplayAbilitySpecHandle                  AbilityToActivate;                                        // (Parm)
				int16_t                                            PredictionKey;                                            // (Parm, ZeroConstructor, IsPlainOldData)
			} UAbilitySystemComponent_ClientActivateAbilityFailed_Params{ AbilityToActivate, PredictionKey };

			if (ASC)
			{
				static auto fn = ASC->Function(_("ClientActivateAbilityFailed"));

				if (fn)
					ASC->ProcessEvent(fn, &UAbilitySystemComponent_ClientActivateAbilityFailed_Params);
				else
					std::cout << _("Could not find ClientActivateAbilityFailed!\n");
			}
			else
				std::cout << _("Invalid component!\n");
		}

		void ServerEndAbility(UObject* ASC, const FGameplayAbilitySpecHandle& AbilityToEnd, const FGameplayAbilityActivationInfo& ActivationInfo, const FPredictionKey& PredictionKey)
		{
			struct
			{
				const FGameplayAbilitySpecHandle& AbilityToEnd;
				const FGameplayAbilityActivationInfo& ActivationInfo;
				const FPredictionKey& PredictionKey;
			} UAbilitySystemComponent_ServerEndAbility_Params{ AbilityToEnd, ActivationInfo, PredictionKey };

			if (ASC)
			{
				static auto fn = ASC->Function(_("ServerEndAbility"));

				if (fn)
					ASC->ProcessEvent(fn, &UAbilitySystemComponent_ServerEndAbility_Params);
				else
					std::cout << _("Could not find ServerEndAbility!\n");
			}
			else
				std::cout << _("Invalid component!\n");
		}

		void ServerSetReplicatedTargetData(UObject* ASC, const FGameplayAbilitySpecHandle& AbilityHandle, const FPredictionKey& AbilityOriginalPredictionKey, const FGameplayAbilityTargetDataHandle& ReplicatedTargetDataHandle, const FGameplayTag& ApplicationTag, const FPredictionKey& CurrentPredictionKey)
		{
			struct
			{
				const FGameplayAbilitySpecHandle& AbilityHandle;
				const FPredictionKey& AbilityOriginalPredictionKey;
				const FGameplayAbilityTargetDataHandle& ReplicatedTargetDataHandle;
				const FGameplayTag& ApplicationTag;
				const FPredictionKey& CurrentPredictionKey;
			} UAbilitySystemComponent_ServerSetReplicatedTargetData_Params{ AbilityHandle, AbilityOriginalPredictionKey, ReplicatedTargetDataHandle, ApplicationTag, CurrentPredictionKey };

			if (ASC)
			{
				static auto fn = ASC->Function(_("ServerSetReplicatedTargetData"));

				if (fn)
					ASC->ProcessEvent(fn, &UAbilitySystemComponent_ServerSetReplicatedTargetData_Params);
				else
					std::cout << _("Could not find ServerEndAbility!\n");
			}
			else
				std::cout << _("Invalid component!\n");
		}
	}
}