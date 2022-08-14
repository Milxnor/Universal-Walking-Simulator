#pragma once

#include <iostream>
#include <fstream>
#include <UE/structs.h>
#include <Net/funcs.h>
#include <dpp/nlohmann/json.hpp>

static bool bPickupAnimsEnabled = true;

UObject* GetWorldW(bool bReset = false)
{
	auto GameViewport = *GetEngine()->Member<UObject*>(("GameViewport"));

	if (GameViewport)
		return *GameViewport->Member<UObject*>(("World")); // we could also find the world by name but that depends on the map

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

		static auto GSC = FindObject(("GameplayStatics /Script/Engine.Default__GameplayStatics"));
		static auto fn = GSC->Function(("SpawnObject"));

		GSC->ProcessEvent(fn, &params);

		return params.ReturnValue;
	}

	UObject* SpawnActor(UObject* Class, const FVector& Location = FVector(), const FRotator& Rotation = FRotator())
	{
		/* FTransform transform;
		transform.Translation = Location;
		transform.Rotation = Rotation.Quaternion();
		transform.Scale3D = FVector(1, 1, 1);

		struct
		{
			UObject* WorldContextObject;                                       // (ConstParm, Parm, ZeroConstructor, IsPlainOldData)
			UObject* ActorClass;                                               // (Parm, ZeroConstructor, IsPlainOldData)
			FTransform                                  SpawnTransform;                                           // (ConstParm, Parm, OutParm, ReferenceParm, IsPlainOldData)
			bool                                               bNoCollisionFail;                                         // (Parm, ZeroConstructor, IsPlainOldData)
			class AActor* Owner;                                                    // (Parm, ZeroConstructor, IsPlainOldData)
			class AActor* ReturnValue;                                              // (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData)
		} params{Helper::GetWorld(), Class, transform, false, nullptr};

		static auto GSC = FindObject(("Class /Script/Engine.GameplayStatics"));
		static auto BeginSpawningActorFromClass = GSC->Function(("BeginSpawningActorFromClass"));

		if (BeginSpawningActorFromClass) */

		auto Loc = Location;
		auto Rot = Rotation;
		FActorSpawnParameters spawnParams = FActorSpawnParameters();
		spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		return SpawnActorO(GetWorldW(), Class, &Loc, &Rot, spawnParams);
	}
}

namespace Helper
{
	//Show Missing POIs. Credit to Ultimanite for most of this.
	void FixPOIs() {
		float Version = std::stof(FN_Version);
		int Season = (int)Version;
		//Volcano
		if (Season == 8) {
			static auto Volcano = FindObject(("LF_Athena_POI_50x50_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.LF_Athena_POI_50x53_Volcano"));
			
			*Volcano->Member<uint8_t>(("DynamicFoundationType")) = 0;

			static auto OnRep_ServerStreamedInLevel = Volcano->Function(("OnRep_ServerStreamedInLevel"));
		
			if (OnRep_ServerStreamedInLevel)
				Volcano->ProcessEvent(OnRep_ServerStreamedInLevel);
			else
				std::cout << ("Unable to find OnRep_ServerStreamedInLevel!\n");
		}
		//Pleasant
		if (Season == 7) {
			*FindObject(("LF_Athena_POI_25x25_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.LF_Athena_POI_25x36"))->Member<uint8_t>(("DynamicFoundationType")) = 0;
		}
		//Marshamello
		if (Version == 7.30f) {
			*FindObject(("LF_Athena_POI_50x50_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.PleasentParkFestivus"))->Member<uint8_t>(("DynamicFoundationType")) = 0;
		}
		//Loot Lake
		if (Season == 6) {
			return;
			static auto FloatingIsland = FindObject(("LF_Athena_POI_15x15_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.LF_FloatingIsland"));
			if (FloatingIsland)
				*FloatingIsland->Member<uint8_t>(("DynamicFoundationType")) = 0;

			static auto Lake = FindObject(("LF_Athena_POI_75x75_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.LF_Lake1"));

			if (Lake)
				*Lake->Member<uint8_t>(("DynamicFoundationType")) = 0;
		}

		/*
		
		[51709] Function /Script/FortniteGame.BuildingFoundation.OnLevelShown
		[51710] Function /Script/FortniteGame.BuildingFoundation.OnLevelStreamedIn
		[51711] Function /Script/FortniteGame.BuildingFoundation.OnRep_DynamicFoundationRepData
		
		*/
	}
	
	UObject* GetWorld()
	{
		return GetWorldW();
	}

	static void ChoosePart(UObject* Pawn, TEnumAsByte<EFortCustomPartType> Part, UObject* ChosenCharacterPart)
	{
		/* if (std::stod(FN_Version) >= 6) // TODO: make sure this is actually when they change it.
		{
			auto PlayerState = *Pawn->Member<UObject*>(("PlayerState"));
			auto CustomCharacterParts = PlayerState->Member<__int64>(("CharacterParts")); // FCustomCharacterParts
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

			static auto chooseFn = Pawn->Function(("ServerChoosePart"));
			Pawn->ProcessEvent(chooseFn, &params);
		}
	}

	bool IsInAircraft(UObject* Controller)
	{
		if (!Controller)
			return false;

		static auto fn = Controller->Function(("IsInAircraft"));
		
		bool bIsInAircraft = false;

		if (fn)
			Controller->ProcessEvent(fn, &bIsInAircraft);

		return bIsInAircraft;
	}

	FVector GetActorLocation(UObject* Actor)
	{
		if (!Actor)
			return FVector();

		/* static */ auto K2_GetActorLocationFN = Actor->Function(("K2_GetActorLocation"));

		if (K2_GetActorLocationFN)
		{
			FVector loc;
			Actor->ProcessEvent(K2_GetActorLocationFN, &loc);
			return loc;
		}
		else
			K2_GetActorLocationFN = Actor->Function(("K2_GetActorLocation"));

		return FVector();
	}

	void EnablePickupAnimation(UObject* Pawn, UObject* Pickup)
	{
		if (bPickupAnimsEnabled)
		{
			struct FFortPickupLocationData
			{
			public:
				UObject* PickupTarget; // class AFortPawn* PickupTarget;                                      // 0x0(0x8)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
				UObject* CombineTarget; // class AFortPickup* CombineTarget;                                     // 0x8(0x8)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
				UObject* ItemOwner; // class AFortPawn* ItemOwner;                                         // 0x10(0x8)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
				FVector                 LootInitialPosition;                               // 0x18(0xC)(NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
				FVector                LootFinalPosition;                                 // 0x24(0xC)(NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
				float                                        FlyTime;                                           // 0x30(0x4)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
				FVector            StartDirection;                                    // 0x34(0xC)(NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
				FVector                 FinalTossRestLocation;                             // 0x40(0xC)(NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
				uint8_t TossState; // enum class EFortPickupTossState              TossState;                                         // 0x4C(0x1)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
				bool                                         bPlayPickupSound;                                  // 0x4D(0x1)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
				uint8_t                                        Pad_3687[0x2];                                     // Fixing Size After Last Property  [ Dumper-7 ]
				FGuid                                 PickupGuid;                                        // 0x50(0x10)(ZeroConstructor, IsPlainOldData, RepSkip, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPrivate)
			};

			auto PickupLocationData = Pickup->Member<FFortPickupLocationData>("PickupLocationData");

			if (PickupLocationData)
			{
				PickupLocationData->PickupTarget = Pawn;
				PickupLocationData->ItemOwner = Pawn; // wrong I think
				// PickupLocationData->LootInitialPosition = Helper::GetActorLocation(Pickup);
				PickupLocationData->FlyTime = 0.75f;
				static auto GuidOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortItemEntry"), ("ItemGuid"));
				auto Guid = (FGuid*)(__int64(&*Pickup->Member<__int64>(_("PrimaryPickupItemEntry"))) + GuidOffset);
				PickupLocationData->PickupGuid = *Guid; // *FFortItemEntry::GetGuid(Pickup->Member<__int64>(_("PrimaryPickupItemEntry")));
			}
		}

		auto OnRep_PickupLocationData = Pickup->Function(_("OnRep_PickupLocationData"));

		if (OnRep_PickupLocationData)
			Pickup->ProcessEvent(OnRep_PickupLocationData);
		else
			std::cout << "Failed to find OnRep_PickupLocationData!\n";
	}

	UObject* SummonPickup(UObject* Pawn, UObject* Definition, FVector Location, EFortPickupSourceTypeFlag PickupSource, EFortPickupSpawnSource SpawnSource, int Count = 1, bool bTossPickup = true)
	{
		static UObject* EffectClass = FindObject(("BlueprintGeneratedClass /Game/Effects/Fort_Effects/Gameplay/Pickups/B_Pickups_Default.B_Pickups_Default_C"));
		static UObject* PickupClass = FindObject(("Class /Script/FortniteGame.FortPickupAthena"));

		auto Pickup = Easy::SpawnActor(PickupClass, Location);
		auto Effect = Easy::SpawnActor(EffectClass, Location);

		if (Pickup && Definition)
		{
			auto ItemEntry = Pickup->Member<void>(("PrimaryPickupItemEntry"));
			static auto CountOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortItemEntry"), ("Count"));
			static auto ItemDefOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortItemEntry"), ("ItemDefinition"));

			static auto OnRep_PrimaryPickupItemEntry = Pickup->Function(("OnRep_PrimaryPickupItemEntry"));

			if (ItemEntry && OnRep_PrimaryPickupItemEntry)
			{
				*(int*)(__int64(ItemEntry) + CountOffset) = Count;
				*(UObject**)(__int64(ItemEntry) + ItemDefOffset) = Definition;

				Pickup->ProcessEvent(OnRep_PrimaryPickupItemEntry);

				if (bTossPickup)
				{
					static auto TossPickupFn = Pickup->Function(("TossPickup"));

					if (Engine_Version < 426)
					{
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
					else
					{
						struct {
							FVector FinalLocation;
							UObject* ItemOwner;
							int OverrideMaxStackCount;
							bool bToss;
							bool bShouldCombinePickupsWhenTossCompletes;
							EFortPickupSourceTypeFlag InPickupSourceTypeFlags; // Do these even exist on older versions?
							EFortPickupSpawnSource InPickupSpawnSource;
						} TPParams{ Location, Pawn, 6, true, true, PickupSource, SpawnSource };

						if (TossPickupFn)
							Pickup->ProcessEvent(TossPickupFn, &TPParams);
					}
				}

				if (PickupSource == EFortPickupSourceTypeFlag::Container)
				{
					*Pickup->Member<bool>(("bTossedFromContainer")) = true;

					static auto OnRep_TossedFromContainer = Pickup->Function(("OnRep_TossedFromContainer"));

					if (OnRep_TossedFromContainer)
						Pickup->ProcessEvent(OnRep_TossedFromContainer);
				}

				// physics go brr

				static auto SetReplicateMovementFn = Pickup->Function(("SetReplicateMovement"));
				struct { bool b; } bruh{ true };
				Pickup->ProcessEvent(SetReplicateMovementFn, &bruh);

				static auto Rep_ReplicateMovement = Pickup->Function(("OnRep_ReplicateMovement"));
				Pickup->ProcessEvent(Rep_ReplicateMovement);
				
				static auto ProjectileMovementComponentClass = FindObject("Class /Script/Engine.ProjectileMovementComponent");

				auto MovementComponent = Pickup->Member<UObject*>("MovementComponent");
				*MovementComponent = Easy::SpawnObject(ProjectileMovementComponentClass, Pickup);
			}
		}

		return Pickup;
	}

	bool RandomBoolWithWeight(float Weight)
	{
		static auto KSM = FindObject("KismetMathLibrary /Script/Engine.Default__KismetMathLibrary");

		struct {
			float weight;
			bool Ret;
		} parms{ Weight };

		if (KSM)
		{
			static auto RandomBoolWithWeight = KSM->Function("RandomBoolWithWeight");

			if (RandomBoolWithWeight)
				KSM->ProcessEvent(RandomBoolWithWeight, &parms);
		}

		return parms.Ret;
	}

	UObject* GetOwnerOfComponent(UObject* Component)
	{
		static auto fn = Component->Function(("GetOwner"));

		UObject* Owner = nullptr;

		if (fn)
			Component->ProcessEvent(fn, &Owner);

		return Owner;
	}

	void DestroyActor(UObject* Actor)
	{
		if (!Actor) 
			return;

		static auto fn = Actor->Function(("K2_DestroyActor"));

		if (fn)
			Actor->ProcessEvent(fn);
		else
			std::cout << ("Failed to find K2_DestroyActor function!\n");
	}

	FRotator GetActorRotation(UObject* Actor)
	{
		static auto K2_GetActorRotationFN = Actor->Function(("K2_GetActorRotation"));

		if (K2_GetActorRotationFN)
		{
			FRotator loc;
			Actor->ProcessEvent(K2_GetActorRotationFN, &loc);
			return loc;
		}
		else
			K2_GetActorRotationFN = Actor->Function(("K2_GetActorRotation"));

		return FRotator();
	}

	UObject* GetPawnFromController(UObject* PC)
	{
		static auto PawnOffset = GetOffset(PC, "Pawn");

		return *(UObject**)(__int64(PC) + PawnOffset);
	}

	static TArray<UObject*> GetAllActorsOfClass(UObject* Class, UObject* World = nullptr)
	{
		static auto GSCClass = FindObject(("GameplayStatics /Script/Engine.Default__GameplayStatics"));

		static UObject* GetAllActorsOfClass = GSCClass->Function(("GetAllActorsOfClass"));

		struct
		{
			UObject* World;
			UObject* Class;
			TArray<UObject*> ReturnValue;
		} Params;

		Params.World = World ? World : GetWorld();
		Params.Class = Class;

		ProcessEventO(GSCClass, GetAllActorsOfClass, &Params);

		return Params.ReturnValue;
	}

	static bool IsRespawnEnabled()
	{
		bool Respawning = bIsPlayground;
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
			static auto fn = BuildingActor->Function(("InitializeKismetSpawnedBuildingActor"));

			if (fn)
				BuildingActor->ProcessEvent(fn, &IBAParams);
		}
	}

	static UObject* SpawnChip(UObject* Controller, FVector ChipLocation)
	{
		static auto ChipClass = FindObject(("Class /Script/FortniteGame.BuildingGameplayActorSpawnChip"));

		auto Pawn = Controller->Member<UObject*>(("Pawn"));

		if (ChipClass && Pawn && *Pawn)
		{
			auto PlayerState = (*Pawn)->Member<UObject*>(("PlayerState"));

			if (PlayerState && *PlayerState)
			{
				std::cout << ("Spawning Chip!\n");

				auto Chip = Easy::SpawnActor(ChipClass, ChipLocation, Helper::GetActorRotation(*Pawn));

				std::cout << ("Initializing Chip!\n");

				Helper::InitializeBuildingActor(Controller, Chip);

				std::cout << ("Initialized Chip!\n");

				*Chip->Member<UObject*>(("OwnerPlayerController")) = Controller;
				*Chip->Member<UObject*>(("OwnerPlayerState")) = *PlayerState;
				*Chip->Member<unsigned char>(("SquadId")) = *(*PlayerState)->Member<unsigned char>(("SquadId"));
				*Chip->Member<__int64>(("OwnerPlayerId")) = *(*PlayerState)->Member<__int64>(("UniqueId"));

				struct FRebootCardReplicatedState {
					float                                              ChipExpirationServerStartTime;                            // 0x0000(0x0004) (ZeroConstructor, IsPlainOldData)
					unsigned char                                      UnknownData00[0x4];                                       // 0x0004(0x0004) MISSED OFFSET
					UObject* PlayerState;                                              // 0x0008(0x0008) (ZeroConstructor, IsPlainOldData)
				};

				auto CardReplicatedState = FRebootCardReplicatedState{};

				CardReplicatedState.PlayerState = *PlayerState;

				*Chip->Member<FRebootCardReplicatedState>(("RebootCardReplicatedState")) = CardReplicatedState;
				Chip->ProcessEvent(("OnRep_RebootCardReplicatedState"));

				return Chip;
			}
		}
		else
			std::cout << ("Unable to find ChipClass!\n");

		return nullptr;
	}
	static void SpawnMilo(UObject* Pawn)
	{
		auto def = FindObject(("FortWeaponRangedItemDefinition /Game/Athena/Items/Weapons/WID_AshtonPack_Milo.WID_AshtonPack_Milo"));
		auto Location = GetActorLocation(Pawn);
		SummonPickup(Pawn, def, Location, EFortPickupSourceTypeFlag::Tossed, EFortPickupSpawnSource::Chest, 1, true);
	}
	static void SpawnAshton(UObject* Pawn)
	{
		auto def = FindObject(("AthenaGadgetItemDefinition /Game/Athena/Items/Gameplay/BackPacks/Ashton/AGID_AshtonPack.AGID_AshtonPack"));
		auto Location = GetActorLocation(Pawn);
		SummonPickup(Pawn, def, Location, EFortPickupSourceTypeFlag::Tossed, EFortPickupSpawnSource::Chest, 1, true);
	}

	bool TeleportTo(UObject* Actor, FVector Location, FRotator Rot = FRotator())
	{
		struct {
			FVector& Location;
			FRotator& Rotation; // DestinationActor->GetActorRotation());
			bool Res;
		} params{ Location, Rot, false };

		auto fn = Actor->Function(("K2_TeleportTo"));

		if (fn)
			Actor->ProcessEvent(fn, &params);

		return params.Res;
	}

	UObject* GetGameState()
	{
		auto world = Helper::GetWorld();
		auto gameState = *world->Member<UObject*>(("GameState"));

		return gameState;
	}

	EAthenaGamePhase* GetGamePhase()
	{
		return GetGameState()->Member<EAthenaGamePhase>(("GamePhase"));
	}

	FVector GetPlayerStart()
	{
		static auto WarmupClass = FindObject(("Class /Script/FortniteGame.FortPlayerStartWarmup"));
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

	int DestroyAll(UObject* ClassToDestroy)
	{
		if (!ClassToDestroy)
			return -1;

		auto AllActors = Helper::GetAllActorsOfClass(ClassToDestroy);

		for (int i = 0; i < AllActors.Num(); i++)
		{
			auto Actor = AllActors.At(i);

			if (Actor)
				Helper::DestroyActor(Actor);
		}

		auto ActorsNum = AllActors.Num();

		AllActors.Free();

		return ActorsNum;
	}

	namespace Console
	{
		static UObject** ViewportConsole = nullptr;

		DWORD WINAPI Setup(LPVOID)
		{
			static auto Engine = FindObjectOld(("FortEngine_"));

			while (!Engine)
			{
				Engine = FindObjectOld(("FortEngine_"));
				Sleep(1000 / 30);
			}

			static auto ConsoleClass = FindObject(("Class /Script/Engine.Console"));
			static auto GameViewport = Engine->Member<UObject*>(("GameViewport"));

			while (!*GameViewport)
			{
				GameViewport = Engine->Member<UObject*>(("GameViewport"));
				Sleep(1000 / 30);
			}

			ViewportConsole = (*GameViewport)->Member<UObject*>(("ViewportConsole"));

			auto ConsoleObject = Easy::SpawnObject(ConsoleClass, *GameViewport);

			if (ConsoleObject)
				*ViewportConsole = ConsoleObject;
			else
				std::cout << ("[WARNING] SpawnObject failed to create console!\n");

			return 0;
		}

		void ExecuteConsoleCommand(FString& Command)
		{
			struct {
				UObject* WorldContextObject;                                       // (Parm, ZeroConstructor, IsPlainOldData)
				FString                                     Command;                                                  // (Parm, ZeroConstructor)
				UObject* SpecificPlayer;                                           // (Parm, ZeroConstructor, IsPlainOldData)
			} params{Helper::GetWorld(), Command, nullptr};

			static auto KSLClass = FindObject(("KismetSystemLibrary /Script/Engine.Default__KismetSystemLibrary"));

			if (KSLClass)
			{
				// static auto ExecuteConsoleCommandFn = KSLClass->Function(("ExecuteConsoleCommand"));
				static auto ExecuteConsoleCommandFn = FindObject(("Function /Script/Engine.KismetSystemLibrary.ExecuteConsoleCommand"));

				if (ExecuteConsoleCommandFn)
					KSLClass->ProcessEvent(ExecuteConsoleCommandFn, &params);
				else
					std::cout << ("No ExecuteConsoleCommand!\n");
			}
			else
				std::cout << ("No KismetSyustemLibrary!\n");
		}
	}

	void SetOwner(UObject* Actor, UObject* Owner)
	{
		*Actor->Member<UObject*>(("Owner")) = Owner; // TODO: Call SetOwner
		static auto OnRepOwner = Actor->Function(("OnRep_Owner"));
		if (OnRepOwner)
			Actor->ProcessEvent(OnRepOwner);
	}

	static UObject* GetVehicle(UObject* Pawn)
	{
		static auto fn = Pawn->Function(("GetVehicle"));

		UObject* Vehicle;
		Pawn->ProcessEvent(fn, &Vehicle);

		return Vehicle;
	}

	void SetLocalRole(UObject* Actor, ENetRole LocalRole)
	{
		*Actor->Member<ENetRole>(("Role")) = LocalRole;
	}

	void SetRemoteRole(UObject* Actor, ENetRole RemoteRole)
	{
		*Actor->Member<ENetRole>(("RemoteRole")) = RemoteRole;
	}

	static void SetActorScale3D(UObject* Actor, const FVector& Scale)
	{
		static auto SetActorScaleFn = Actor->Function(("SetActorScale3D"));
		struct { FVector Scale; }params{ Scale };

		if (SetActorScaleFn)
			Actor->ProcessEvent(SetActorScaleFn, &params);
	}

	static FString GetfPlayerName(UObject* PlayerState)
	{
		FString Name; // = *PlayerState->Member<FString>(("PlayerNamePrivate"));

		if (PlayerState)
		{
			static auto fn = PlayerState->Function(("GetPlayerName"));

			if (fn)
				PlayerState->ProcessEvent(fn, &Name);
		}

		return Name;
	}

	static bool IsSmallZoneEnabled()
	{
		return bIsLateGame;
	}

	static UObject* GetRandomPOIActor()
	{
		if (Helper::IsSmallZoneEnabled())
		{
			auto POIManager = *Helper::GetGameState()->Member<UObject*>("PoiManager");

			if (POIManager)
			{
				auto AllPois = POIManager->Member<TArray<UObject*>>("AllPoiVolumes");

				if (AllPois)
				{
					auto POI = AllPois->At(rand() % (AllPois->Num()));

					if (POI)
						return POI;
				}
			}
		}

		return nullptr;
	}

	static std::string GetPlayerName(UObject* PlayerState)
	{
		auto name = GetfPlayerName(PlayerState);
		return name.Data.GetData() ? name.ToString() : "";
	}

	static FString GetfIP(UObject* PlayerState) // UNSAFE
	{
		FString IP;

		if (PlayerState)
		{
			IP = *PlayerState->Member<FString>(("SavedNetworkAddress"));
		}

		return IP.Data.GetData() ? IP : FString();
	}

	static std::string GetIP(UObject* PlayerState)
	{
		auto ip = GetfIP(PlayerState);
		return ip.Data.GetData() ? ip.ToString() : "";
	}

	static bool IsStructurallySupported(UObject* BuildingActor)
	{
		if (!BuildingActor)
			return false;

		struct { bool Ret; }params;
		static auto fn = BuildingActor->Function(("IsStructurallySupported"));
		if (fn)
			BuildingActor->ProcessEvent(fn, &params);
		return params.Ret;
	}

	void PlayMontageReplicated(UObject* Controller, UObject* Montage)
	{

	}

	DWORD WINAPI DumpObjects(LPVOID)
	{
		std::ofstream objects("Objects.log");

		for (int32_t i = 0; i < (ObjObjects ? ObjObjects->Num() : OldObjects->Num()); i++)
		{
			auto Object = GetByIndex(i);

			if (!Object)
				continue;

			objects << std::format("[{}] {}\n", Object->InternalIndex, Object->GetFullName()); // TODO: add the offset
		}

		return 0;
	}

	void SetHealth(UObject* Pawn, float Health)
	{
		static auto SetHealth = Pawn->Function("SetHealth");

		if (SetHealth)
			Pawn->ProcessEvent(SetHealth, &Health);
	}

	UObject* InitPawn(UObject* PC, bool bResetCharacterParts = false, FVector Location = Helper::GetPlayerStart(), bool bResetTeams = false)
	{
		static const auto FnVerDouble = std::stod(FN_Version);

		UObject* PlayerState = *PC->Member<UObject*>(("PlayerState"));

		static auto PawnClass = FindObject(("BlueprintGeneratedClass /Game/Athena/PlayerPawn_Athena.PlayerPawn_Athena_C"));
		auto Pawn = Easy::SpawnActor(PawnClass, Location, {});

		if (!Pawn)
			return nullptr;

		static auto SetReplicateMovementFn = Pawn->Function(("SetReplicateMovement"));
		struct { bool b; } bruh{true};
		Pawn->ProcessEvent(SetReplicateMovementFn, &bruh);
		// Pawn->Member<bool>("bReplicateMovement", 1);

		// prob not needed here from

		static auto Rep_ReplicateMovement = Pawn->Function(("OnRep_ReplicateMovement"));
		Pawn->ProcessEvent(Rep_ReplicateMovement);

		static auto Rep_ReplicatedMovement = Pawn->Function(("OnRep_ReplicatedMovement"));
		Pawn->ProcessEvent(Rep_ReplicatedMovement);

		static auto Rep_ReplicatedBasedMovement = Pawn->Function(("OnRep_ReplicatedBasedMovement"));
		Pawn->ProcessEvent(Rep_ReplicateMovement);

		// here

		// PC->Exec("Possess", Pawn);

		static auto PossessFn = PC->Function(("Possess"));

		if (PossessFn)
		{
			struct {
				UObject* InPawn;
			} params{ Pawn }; // idk man
			PC->ProcessEvent(PossessFn, &params);
		}
		else
			std::cout << ("Could not find Possess!\n");

		Helper::SetOwner(Pawn, PC); // prob not needed

		// *Pawn->Member<float>(("NetUpdateFrequency")) = 200;

		static auto setMaxHealthFn = Pawn->Function(("SetMaxHealth"));
		struct { float NewHealthVal; }healthParams{ 100 };

		if (setMaxHealthFn)
			Pawn->ProcessEvent(setMaxHealthFn, &healthParams);
		else
			std::cout << ("Unable to find setMaxHealthFn!\n");

		static auto setMaxShieldFn = Pawn->Function(("SetMaxShield"));
		struct { float NewValue; }shieldParams{ 100 };

		if (setMaxShieldFn)
			Pawn->ProcessEvent(setMaxShieldFn, &shieldParams);
		else
			std::cout << ("Unable to find setMaxShieldFn!\n");

		/*

		static const auto HeroType = FindObject(("FortHeroType /Game/Athena/Heroes/HID_058_Athena_Commando_M_SkiDude_GER.HID_058_Athena_Commando_M_SkiDude_GER"));

		*PlayerState->Member<UObject*>(("HeroType")) = HeroType;
		static auto OnRepHeroType = PlayerState->Function(("OnRep_HeroType"));
		PlayerState->ProcessEvent(OnRepHeroType);

		*/

		/*static auto FortCustomizationAssetLoader = FindObject(("FortCustomizationAssetLoader /Script/FortniteGame.Default__FortCustomizationAssetLoader"));
		auto PawnCustomizationAssetLoader = Pawn->Member<UObject*>("CustomizationAssetLoader");
		auto LocalPawnCustomizationAssetLoader = PC->Member<UObject*>("LocalPawnCustomizationAssetLoader");
		*PawnCustomizationAssetLoader = FortCustomizationAssetLoader;
		*LocalPawnCustomizationAssetLoader = FortCustomizationAssetLoader;*/

		/* auto CustomizationAssetLoader = Pawn->Member<UObject*>(("CustomizationAssetLoader"));

		if (CustomizationAssetLoader && *CustomizationAssetLoader)
		{
			void* CurrentAssetsToLoad = (*CustomizationAssetLoader)->Member<void>(("CurrentAssetsToLoad"));

			static auto CharacterPartsOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortCustomizationAssetsToLoad"), ("CharacterParts"));

			auto CharacterPartsToLoad = (TArray<UObject*>*)(__int64(CurrentAssetsToLoad) + CharacterPartsOffset);
		}
		
		static auto headPart = Engine_Version >= 423 ? FindObject(("CustomCharacterPart /Game/Characters/CharacterParts/Female/Medium/Heads/CP_Head_F_TreasureHunterFashion.CP_Head_F_TreasureHunterFashion")) :
			FindObject(("CustomCharacterPart /Game/Characters/CharacterParts/Female/Medium/Heads/F_Med_Head1.F_Med_Head1"));
		static auto bodyPart = Engine_Version >= 423 ? FindObject(("CustomCharacterPart /Game/Athena/Heroes/Meshes/Bodies/CP_Body_Commando_F_TreasureHunterFashion.CP_Body_Commando_F_TreasureHunterFashion")) :
			FindObject(("CustomCharacterPart /Game/Characters/CharacterParts/Female/Medium/Bodies/F_Med_Soldier_01.F_Med_Soldier_01")); */

		static auto headPart = FindObject(("CustomCharacterPart /Game/Characters/CharacterParts/Female/Medium/Heads/F_Med_Head1.F_Med_Head1"));
		static auto bodyPart = FindObject(("CustomCharacterPart /Game/Characters/CharacterParts/Female/Medium/Bodies/F_Med_Soldier_01.F_Med_Soldier_01"));

		if (!headPart)
			headPart = FindObject(("CustomCharacterPart /Game/Characters/CharacterParts/Female/Medium/Heads/CP_Head_F_RebirthDefaultA.CP_Head_F_RebirthDefaultA"));

		if (!bodyPart)
			bodyPart = FindObject(("CustomCharacterPart /Game/Athena/Heroes/Meshes/Bodies/CP_Body_Commando_F_RebirthDefaultA.CP_Body_Commando_F_RebirthDefaultA"));

		if (headPart && bodyPart && bResetCharacterParts)
		{
			Helper::ChoosePart(Pawn, EFortCustomPartType::Head, headPart);
			Helper::ChoosePart(Pawn, EFortCustomPartType::Body, bodyPart);

			static auto OnRep_Parts = (FnVerDouble >= 10) ? PlayerState->Function(("OnRep_CharacterData")) : PlayerState->Function(("OnRep_CharacterParts")); //Make sure its s10 and up

			if (OnRep_Parts)
				PlayerState->ProcessEvent(OnRep_Parts, nullptr);
		}
		else
			std::cout << ("Unable to find Head and Body!\n");

		return Pawn;
	}
	
	UObject* SoftObjectToObject(TSoftObjectPtr SoftObject)
	{
		return FindObject(SoftObject.ObjectID.AssetPathName.ToString());
	}

	UObject* GetPlaylist()
	{
		auto world = Helper::GetWorld();
		auto gameState = *world->Member<UObject*>(("GameState"));

		if (std::stod(FN_Version) >= 6.10) // WRONG
		{
			static auto BasePlaylistOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.PlaylistPropertyArray"), ("BasePlaylist"));
			if (BasePlaylistOffset)
			{
				static auto PlaylistInfo = gameState->Member<void>(("CurrentPlaylistInfo"));

				auto BasePlaylist = (UObject**)(__int64(PlaylistInfo) + BasePlaylistOffset);// *gameState->Member<UObject>(("CurrentPlaylistInfo"))->Member<UObject*>(("BasePlaylist"), true);

				return BasePlaylist ? *BasePlaylist : nullptr;
			}
		}
		else
			return *gameState->Member<UObject*>(("CurrentPlaylistData"));

		return nullptr;
	}
	
	namespace Conversion
	{
		UObject* SoftObjectToObject(TSoftObjectPtr SoftObject)
		{
			// Function /Script/Engine.KismetSystemLibrary.Conv_SoftObjectReferenceToObject

			static auto KSLClass = FindObject(("KismetSystemLibrary /Script/Engine.Default__KismetSystemLibrary"));

			static auto fn = KSLClass->Function(("Conv_SoftObjectReferenceToObject"));

			UObject* Object;

			struct {
				TSoftObjectPtr SoftObject;
				UObject* ReturnValue;
			} params{SoftObject, nullptr};

			if (fn)
				KSLClass->ProcessEvent(fn, &Object);

			return params.ReturnValue;
		}

		std::string RarityToString(EFortRarity Rarity)
		{
			switch (Rarity)
			{
			case EFortRarity::Badass:
				return ("Badass");
			case EFortRarity::Elegant:
				return ("Elegant");
			case EFortRarity::Epic:
				return ("Epic");
			case EFortRarity::Masterwork:
				return ("Masterwork");
			case EFortRarity::Legendary:
				return ("Legendary");
			case EFortRarity::Sturdy:
				return ("Sturdy");
			case EFortRarity::Quality:
				return ("Quality");
			case EFortRarity::Ordinary:
				return ("Ordinary");
			default:
				return ("InvalidRarity");
			}
		}

		FText StringToText(FString String)
		{
			static auto KTL = FindObject(("KismetTextLibrary /Script/Engine.Default__KismetTextLibrary"));

			FText text;

			if (KTL)
			{
				static auto fn = KTL->Function(("Conv_StringToText"));

				struct {
					FString InText;
					FText ReturnValue;
				} params{ String };

				if (fn)
					KTL->ProcessEvent(fn, &params);
				else
					std::cout << ("Unable to find Conv_StringToText!\n");

				text = params.ReturnValue;
			}
			else
				std::cout << ("Unable to find KTL!\n");

			return text;
		}

		std::string TextToString(FText Text)
		{
			static auto KTL = FindObject(("KismetTextLibrary /Script/Engine.Default__KismetTextLibrary"));

			FString String;

			if (KTL)
			{
				static auto fn = KTL->Function(("Conv_TextToString"));

				struct {
					FText InText;
					FString ReturnValue;
				} params{ Text };

				if (fn)
					KTL->ProcessEvent(fn, &params);
				else
					std::cout << ("Unable to find Conv_TextToString!\n");

				String = params.ReturnValue;
			}
			else
				std::cout << ("Unable to find KTL!\n");

			return String.Data.GetData() ? String.ToString() : "INVALID_STRING";
		}
	}

	void SilentDie(UObject* BuildingActor)
	{
		static auto fn = BuildingActor->Function(("SilentDie"));

		if (fn)
			BuildingActor->ProcessEvent(fn);
	}

	bool SetActorLocation(UObject* Actor, const FVector& Location)
	{
		static auto fn = Actor->Function(("K2_SetActorLocation"));

		struct
		{
			FVector                                     NewLocation;                                              // (Parm, IsPlainOldData)
			bool                                               bSweep;                                                   // (Parm, ZeroConstructor, IsPlainOldData)
			char hitresult[0x88];
			bool                                               bTeleport;                                                // (Parm, ZeroConstructor, IsPlainOldData)
			bool                                               ReturnValue;                                              // (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData)
		} parms{ Location, false, 0, true };

		if (fn)
			Actor->ProcessEvent(fn, &parms);

		return parms.ReturnValue;
	}

	static FName StringToName(FString Str)
	{
		static auto fn = FindObject(("Function /Script/Engine.KismetStringLibrary.Conv_StringToName"));
		static auto KSL = FindObject(("KismetStringLibrary /Script/Engine.Default__KismetStringLibrary"));

		struct {
			FString InString;
			FName ReturnValue;
		} params{ Str };

		KSL->ProcessEvent(fn, &params);

		return params.ReturnValue;
	}

	bool SetActorLocationAndRotation(UObject* Actor, FVector& Location, const FRotator& Rotation)
	{
		static auto fn = Actor->Function(("K2_SetActorLocationAndRotation"));

		struct 
		{
			struct FVector                                     NewLocation;                                              // (Parm, IsPlainOldData)
			struct FRotator                                    NewRotation;                                              // (Parm, IsPlainOldData)
			bool                                               bSweep;                                                   // (Parm, ZeroConstructor, IsPlainOldData)
			char hitresult[0x88];
			bool                                               bTeleport;                                                // (Parm, ZeroConstructor, IsPlainOldData)
			bool                                               ReturnValue;                                              // (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData)
		} parms{ Location, Rotation, false, 0, true };

		if (fn)
			Actor->ProcessEvent(fn, &parms);

		return parms.ReturnValue;
	}
	void SpawnVehicle(UObject* VehicleClass, const FVector& Location, const FRotator& Rotation = FRotator())
	{
		Easy::SpawnActor(VehicleClass, Location, Rotation);
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
				static auto fn = ASC->Function(("ClientActivateAbilityFailed"));

				if (fn)
					ASC->ProcessEvent(fn, &UAbilitySystemComponent_ClientActivateAbilityFailed_Params);
				else
					std::cout << ("Could not find ClientActivateAbilityFailed!\n");
			}
			else
				std::cout << ("Invalid component!\n");
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
				static auto fn = ASC->Function(("ServerEndAbility"));

				if (fn)
					ASC->ProcessEvent(fn, &UAbilitySystemComponent_ServerEndAbility_Params);
				else
					std::cout << ("Could not find ServerEndAbility!\n");
			}
			else
				std::cout << ("Invalid component!\n");
		}

		void ServerSetReplicatedTargetData(UObject* ASC, const FGameplayAbilitySpecHandle& AbilityHandle, const FPredictionKey& AbilityOriginalPredictionKey, const FGameplayAbilityTargetDataHandleOL& ReplicatedTargetDataHandle, const FGameplayTag& ApplicationTag, const FPredictionKey& CurrentPredictionKey)
		{
			struct
			{
				FGameplayAbilitySpecHandle AbilityHandle;
				FPredictionKey AbilityOriginalPredictionKey;
				FGameplayAbilityTargetDataHandleOL ReplicatedTargetDataHandle;
				FGameplayTag ApplicationTag;
				FPredictionKey CurrentPredictionKey;
			} UAbilitySystemComponent_ServerSetReplicatedTargetData_Params{ AbilityHandle, AbilityOriginalPredictionKey, ReplicatedTargetDataHandle, ApplicationTag, CurrentPredictionKey };

			if (ASC)
			{
				static auto fn = ASC->Function(("ServerSetReplicatedTargetData"));

				if (fn)
					ASC->ProcessEvent(fn, &UAbilitySystemComponent_ServerSetReplicatedTargetData_Params);
				else
					std::cout << ("Could not find ServerEndAbility!\n");
			}
			else
				std::cout << ("Invalid component!\n");
		}

		void ServerSetReplicatedTargetData(UObject* ASC, const FGameplayAbilitySpecHandle& AbilityHandle, const FPredictionKey& AbilityOriginalPredictionKey, const FGameplayAbilityTargetDataHandleSE& ReplicatedTargetDataHandle, const FGameplayTag& ApplicationTag, const FPredictionKey& CurrentPredictionKey)
		{
			struct
			{
				FGameplayAbilitySpecHandle AbilityHandle;
				FPredictionKey AbilityOriginalPredictionKey;
				FGameplayAbilityTargetDataHandleSE ReplicatedTargetDataHandle;
				FGameplayTag ApplicationTag;
				FPredictionKey CurrentPredictionKey;
			} UAbilitySystemComponent_ServerSetReplicatedTargetData_Params{ AbilityHandle, AbilityOriginalPredictionKey, ReplicatedTargetDataHandle, ApplicationTag, CurrentPredictionKey };

			if (ASC)
			{
				static auto fn = ASC->Function(("ServerSetReplicatedTargetData"));

				if (fn)
					ASC->ProcessEvent(fn, &UAbilitySystemComponent_ServerSetReplicatedTargetData_Params);
				else
					std::cout << ("Could not find ServerEndAbility!\n");
			}
			else
				std::cout << ("Invalid component!\n");
		}
	}

	void KickController(UObject* Controller, FString Reason)
	{
		if (KickPlayer && Controller && Reason.Data.GetData())
		{
			FText text = Conversion::StringToText(Reason);
			static auto World = Helper::GetWorld();
			auto GameMode = *World->Member<UObject*>(("AuthorityGameMode"));
			auto GameSession = *World->Member<UObject*>(("GameSession"));

			if (GameSession)
				KickPlayer(GameSession, Controller, text); // TODO: Use a differentfunction, this crashes after a second try.
			else
				std::cout << ("Unable to find GameSession!\n");
		}
		else
		{
			// std::cout << std::format(("Something is invalid cannot kick player\n\nKickPlayer: {}\nController: {}\nIf none of these are null then it's data!\n\n"), __int64(KickPlayer), Controller); // , Reason.Data.GetData());
		}
	}

	namespace Banning
	{
		bool Ban(const std::wstring& IP, UObject* Controller, const std::wstring& Username)
		{
			std::ofstream stream(("banned-ips.json"), std::ios::app);

			if (!stream.is_open())
				return false;

			nlohmann::json j;
			j["IP"] = IP;
			j["Username"] = Username;

			stream << j << '\n'; // j.dump(4)

			stream.close();

			FString Reason;
			Reason.Set(L"You are banned!");
			Helper::KickController(Controller, Reason);

			return true;
		}

		bool Unban(const std::wstring& IP) // I think I have SEVERE brain damage, but it works. // IP Or Name
		{
			std::ifstream input_file(("banned-ips.json"));

			if (!input_file.is_open())
				return false;

			std::vector<std::string> lines;
			std::string line;
			int ipToRemove = -1; // the line

			while (std::getline(input_file, line))
			{
				lines.push_back(line);
				if (line.find(std::string(IP.begin(), IP.end())) != std::wstring::npos)
				{
					ipToRemove = lines.size();
				}
			}

			input_file.close();

			if (ipToRemove != -1)
			{
				std::ofstream stream("banned-ips.json", std::ios::ate);
				for (int i = 0; i < lines.size(); i++)
				{
					if (i != ipToRemove - 1)
						stream << lines[i] << '\n';
				}
			}

			return ipToRemove != 1;
		}

		bool IsBanned(const std::wstring& IP)
		{
			std::ifstream input_file(("banned-ips.json"));
			std::string line;

			if (!input_file.is_open())
				return false;

			while (std::getline(input_file, line))
			{
				if (std::wstring(line.begin(), line.end()).find(IP) != std::wstring::npos)
				{
					return true;
				}
			}

			return false;
		}
	}
}