#pragma once

#include <iostream>
#include <fstream>
#include <UE/structs.h>
#include <Net/funcs.h>
#include <dpp/nlohmann/json.hpp>

static bool bPickupAnimsEnabled = true;

UObject* GetWorldW(bool bReset = false)
{
	static auto GameViewportOffset = GetOffset(GetEngine(), "GameViewport");
	auto GameViewport = *(UObject**)(__int64(GetEngine()) + GameViewportOffset);

	if (GameViewport)
	{
		static auto WorldOffset = GetOffset(GameViewport, "World");
		return *(UObject**)(__int64(GameViewport) + WorldOffset);
	}

	return nullptr;
}

static TArray<UObject*> GetAllActorsOfClass_(UObject* Class, UObject* World = nullptr)
{
	static auto GSCClass = FindObject(("GameplayStatics /Script/Engine.Default__GameplayStatics"));

	static UObject* GetAllActorsOfClass = GSCClass->Function(("GetAllActorsOfClass"));

	struct
	{
		UObject* World;
		UObject* Class;
		TArray<UObject*> ReturnValue;
	} Params;

	Params.World = World ? World : GetWorldW();
	Params.Class = Class;

	ProcessEventO(GSCClass, GetAllActorsOfClass, &Params);

	return Params.ReturnValue;
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

		FQuat Ehh; // wrong
		Ehh.W = 0;
		Ehh.X = Rot.Pitch;
		Ehh.Y = Rot.Roll;
		Ehh.Z = Rot.Yaw;

		FTransform transform;
		transform.Translation = Loc;
		transform.Rotation = Ehh;
		transform.Scale3D = { 1, 1, 1 };

		FActorSpawnParameters spawnParams = FActorSpawnParameters();
		spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		spawnParams.bAllowDuringConstructionScript = true;

		if (FnVerDouble < 19.00)
			return SpawnActorO(GetWorldW(), Class, &Loc, &Rot, spawnParams);
		else
		{
			auto GameInstance = *GetEngine()->Member<UObject*>(("GameInstance"));
			auto& LocalPlayers = *GameInstance->Member<TArray<UObject*>>(("LocalPlayers"));
			auto PlayerController = *LocalPlayers.At(0)->Member<UObject*>(("PlayerController"));

			auto CheatManager = *PlayerController->Member<UObject*>("CheatManager");

			static auto Summon = CheatManager->Function("Summon");

			// auto AllActorsOld = Helper::GetAllActorsOfClass(Class);

			std::wstring className = Class->GetFullNFame();
			className.erase(0, className.find_last_of(L".", className.length() - 1) + 1);

			FString ClassName;
			ClassName.Set(className.c_str());

			if (Summon)
				CheatManager->ProcessEvent(Summon, &ClassName);
			else
				std::cout << "Unable to find Summon!\n";

			auto AllActors = GetAllActorsOfClass_(Class);
			std::cout << "All Actors Num: " << AllActors.Num() << '\n';
			auto ActorWeWant = AllActors.At(AllActors.Num() - 1);
			AllActors.Free();
			// Helper::SetActorLocation(ActorWeWant, Location);
			return ActorWeWant;
			
			/* struct {
				UObject* WorldContextObject;
				UObject* ActorClass;
				FTransform SpawnTransform;
				ESpawnActorCollisionHandlingMethod CollisionHandlingOverride;
				UObject* Owner;
				UObject* NewActor;
			} beginParams{GetWorldW(), Class, transform, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn, nullptr};

			static auto GSC = FindObject(("GameplayStatics /Script/Engine.Default__GameplayStatics"));

			static auto BeginDeferredActorSpawnFromClass = GSC->Function("BeginDeferredActorSpawnFromClass");

			if (BeginDeferredActorSpawnFromClass)
				GSC->ProcessEvent(BeginDeferredActorSpawnFromClass, &beginParams);

			if (beginParams.NewActor)
			{
				struct {
					UObject* Actor;
					FTransform Transform;
					UObject* ret;
				} finishParams{ beginParams.NewActor, transform };

				static auto FinishSpawningActor = GSC->Function("FinishSpawningActor");

				if (GSC)
					GSC->ProcessEvent(FinishSpawningActor, &finishParams);

				return finishParams.ret;
			}
			else
				std::cout << "Failed beginning spawning actor!\n";

			return SpawnActorOTrans(GetWorldW(), Class, &transform, spawnParams); */
		}

		return nullptr;
	}
}

namespace Helper
{
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
			} params{ SoftObject, nullptr };

			if (fn)
				KSLClass->ProcessEvent(fn, &Object);

			return params.ReturnValue;
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


	void ShowBuilding(UObject* Foundation, bool bShow = true)
	{
		if (!Foundation)
			return;

		auto DynamicFoundationType = Foundation->Member<uint8_t>(("DynamicFoundationType"));
		
		if (DynamicFoundationType && *DynamicFoundationType)
			*DynamicFoundationType = bShow ? 0 : 3;

		struct BITMF
		{
			uint8_t                                        bConditionalFoundation : 1;                        // Mask : 0x1 0xC68(0x1)(Edit, NoDestructor, Protected, HasGetValueTypeHash, NativeAccessSpecifierProtected)
			uint8_t                                        bServerStreamedInLevel : 1;
		};

		Foundation->Member<BITMF>("bServerStreamedInLevel")->bServerStreamedInLevel = bShow; // fixes hlods
		Foundation->ProcessEvent("OnRep_ServerStreamedInLevel");
	}

	float GetTimeSeconds()
	{
		static auto GSCClass = FindObject(("GameplayStatics /Script/Engine.Default__GameplayStatics"));

		struct { UObject* world; float timeseconds; } parms{ GetWorldW() };

		static auto GetTimeSeconds = GSCClass->Function("GetTimeSeconds");

		if (GetTimeSeconds)
			GSCClass->ProcessEvent(GetTimeSeconds, &parms);

		return parms.timeseconds;
	}

	void TeleportToSkyDive(UObject* Pawn, float Height = 10000)
	{
		struct { float HeightAboveGround; }TeleportToSkyDiveParams{ Height };

		static auto TeleportToSkyDiveFn = Pawn->Function(("TeleportToSkyDive"));

		if (TeleportToSkyDiveFn)
			Pawn->ProcessEvent(TeleportToSkyDiveFn, &TeleportToSkyDiveParams);
	}

	UObject* LoadLevelInstance(const FString& LevelName)
	{
		struct ULevelStreamingDynamic_LoadLevelInstance_Params
		{
			UObject* WorldContextObject;                                       // (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			FString                                     LevelName;                                                // (Parm, ZeroConstructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			FVector                                     Location;                                                 // (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			FRotator                                    Rotation;                                                 // (Parm, ZeroConstructor, IsPlainOldData, NoDestructor, NativeAccessSpecifierPublic)
			bool                                               bOutSuccess;                                              // (Parm, OutParm, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			UObject* ReturnValue;                                              // (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		} ULevelStreamingDynamic_LoadLevelInstance_Params{GetWorldW(), LevelName, AircraftLocationToUse, FRotator()};

		static auto levelStreamingDynamicClass = FindObject("LevelStreamingDynamic /Script/Engine.Default__LevelStreamingDynamic");
		static auto LoadLevelInstance = levelStreamingDynamicClass->Function("LoadLevelInstance");

		if (LoadLevelInstance)
			levelStreamingDynamicClass->ProcessEvent(LoadLevelInstance, &ULevelStreamingDynamic_LoadLevelInstance_Params);

		return ULevelStreamingDynamic_LoadLevelInstance_Params.ReturnValue;
	}

	//Show Missing POIs. Credit to Ultimanite for most of this.
	void FixPOIs() {
		float Version = std::stof(FN_Version);
		int Season = (int)Version;
		//Volcano
		if (Season == 8) {
			static auto Volcano = FindObject(("LF_Athena_POI_50x50_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.LF_Athena_POI_50x53_Volcano"));
			ShowBuilding(Volcano);
		}
		//Pleasant
		if (Season == 7) {
			static auto idfk = FindObject(("LF_Athena_POI_25x25_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.LF_Athena_POI_25x36")); // polar peak?
			ShowBuilding(idfk);

			static auto tiltedtower = FindObject("BuildingFoundation5x5 /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.ShopsNew");
			ShowBuilding(tiltedtower);

		}

		if (Season >= 7 && Engine_Version < 424)
		{
			static auto TheBlock = FindObject("BuildingFoundationSlab_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.SLAB_2"); // SLAB_3 is blank
			ShowBuilding(TheBlock);

			/* FString dababy;
			dababy.Set(L"Dababy");
			*TheBlock->Member<FText>("CreatorName") = Helper::Conversion::StringToText(dababy); */

			// LevelSequencePlayer /Game/Athena/Prototype/Blueprints/Slab/Replicated_LevelSequence.Default__Replicated_LevelSequence_C.AnimationPlayer

			/*
			
			Block_25x25_ColossalCompound - 7.40

			Block_25x25_HarmonyHotel - 8.10

			Block_25x25_TrickyTracks - 8.50

			Block_25x25_AlienSanctuary - 8.50
			
			*/
		}

		//Marshamello
		if (Version == 7.30f) {
			static auto PleasantPark = FindObject(("LF_Athena_POI_50x50_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.PleasentParkFestivus"));
			ShowBuilding(PleasantPark);
		}
		//Loot Lake
		if (Season == 6) {
			static auto FloatingIsland = FindObject(("LF_Athena_POI_15x15_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.LF_FloatingIsland"));
			static auto Lake = FindObject(("LF_Athena_POI_75x75_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.LF_Lake1"));
			static auto Lake2 = FindObject("LF_Athena_POI_75x75_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.LF_Lake2");

			ShowBuilding(FloatingIsland);
			ShowBuilding(Lake);
			// ShowBuilding(Lake2); // this is for after the event

			// *scripting->Member<FVector>("IslandPosition") = Helper::GetActorLocation(FloatingIsland);

			/* static auto iforgot = FindObject("LF_Athena_POI_15x15_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.LF_Athena_StreamingTest13");

			if (iforgot)
			{
				*iforgot->Member<uint8_t>(("DynamicFoundationType")) = 0;
				iforgot->Member<BITMF>("bServerStreamedInLevel")->bServerStreamedInLevel = true;
				iforgot->ProcessEvent("OnRep_ServerStreamedInLevel");
			}
			else
				std::cout << "No I forgot!\n"; */

		}

		/*
		
		[51709] Function /Script/FortniteGame.BuildingFoundation.OnLevelShown
		[51710] Function /Script/FortniteGame.BuildingFoundation.OnLevelStreamedIn
		[51711] Function /Script/FortniteGame.BuildingFoundation.OnRep_DynamicFoundationRepData
		// OnRep_DynamicFoundationTransform

		*/
	}
	
	UObject* GetWorld()
	{
		return GetWorldW();
	}

	static void ChoosePart(UObject* Pawn, TEnumAsByte<EFortCustomPartType> Part, UObject* ChosenCharacterPart)
	{
		/* if (FnVerDouble >= 6) // TODO: make sure this is actually when they change it.
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

	FVector GetActorForwardVector(UObject* Actor)
	{
		if (!Actor)
			return FVector();

		/* static */ auto GetActorForwardVectorFN = Actor->Function(("GetActorForwardVector"));

		if (GetActorForwardVectorFN)
		{
			FVector loc;
			Actor->ProcessEvent(GetActorForwardVectorFN, &loc);
			return loc;
		}
		else
			GetActorForwardVectorFN = Actor->Function(("GetActorForwardVector"));

		return FVector();
	}

	FVector GetActorRightVector(UObject* Actor)
	{
		if (!Actor)
			return FVector();

		/* static */ auto GetActorRightVectorFN = Actor->Function(("GetActorRightVector"));

		if (GetActorRightVectorFN)
		{
			FVector loc;
			Actor->ProcessEvent(GetActorRightVectorFN, &loc);
			return loc;
		}
		else
			GetActorRightVectorFN = Actor->Function(("GetActorRightVector"));

		return FVector();
	}

	FVector GetCorrectLocation(UObject* Actor) {

		if (!Actor)
			return FVector{ 0, 0, 0 };

		auto Location = Helper::GetActorLocation(Actor);
		auto RightVector = Helper::GetActorRightVector(Actor);

		return Location + RightVector * 70.0f + FVector{ 0, 0, 50 };
	}

	void EnablePickupAnimation(UObject* Pawn, UObject* Pickup, float FlyTime = 0.75f, FVector StartDirection = FVector())
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
				PickupLocationData->FlyTime = FlyTime;
				PickupLocationData->StartDirection = StartDirection;
				static auto GuidOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortItemEntry"), ("ItemGuid"));
				auto Guid = (FGuid*)(__int64(&*Pickup->Member<__int64>(("PrimaryPickupItemEntry"))) + GuidOffset);
				PickupLocationData->PickupGuid = *Guid; // *FFortItemEntry::GetGuid(Pickup->Member<__int64>(("PrimaryPickupItemEntry")));
			}
		}

		auto OnRep_PickupLocationData = Pickup->Function(("OnRep_PickupLocationData"));

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

	UObject* GetOwner(UObject* Actor)
	{
		static auto fn = Actor->Function(("GetOwner"));

		UObject* Owner = nullptr;

		if (fn)
			Actor->ProcessEvent(fn, &Owner);

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

	UObject* GetPawnFromController(UObject* PC)
	{
		if (!PC)
			return nullptr;

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
		UObject* FinalActor = nullptr;
		if (FnVerDouble < 18.00) // wrong probs
		{
			// 	void InitializeKismetSpawnedBuildingActor(class ABuildingActor* BuildingOwner, class AFortPlayerController* SpawningController, bool bUsePlayerBuildAnimations = true);
			struct {
				UObject* BuildingOwner; // ABuildingActor
				UObject* SpawningController;
				bool bUsePlayerBuildAnimations; // I think this is not on some versions
			} IBAParams{ BuildingActor, Controller, bUsePlayerBuildAnimations };

			if (Controller && BuildingActor)
			{
				static auto fn = BuildingActor->Function(("InitializeKismetSpawnedBuildingActor"));

				if (fn)
					BuildingActor->ProcessEvent(fn, &IBAParams);

				FinalActor = BuildingActor;
			}
		}
		else
		{
			// 	void InitializeKismetSpawnedBuildingActor(class ABuildingActor* BuildingOwner, class AFortPlayerController* SpawningController, bool bUsePlayerBuildAnimations = true);
			struct {
				UObject* BuildingOwner; // ABuildingActor
				UObject* SpawningController;
				bool bUsePlayerBuildAnimations; // I think this is not on some versions
				UObject* ReplacedBuilding;
			} IBAParams{ BuildingActor, Controller, bUsePlayerBuildAnimations, nullptr};

			if (Controller && BuildingActor)
			{
				static auto fn = BuildingActor->Function(("InitializeKismetSpawnedBuildingActor"));

				if (fn)
					BuildingActor->ProcessEvent(fn, &IBAParams);
				FinalActor = BuildingActor;
			}
		}
		
		*FinalActor->Member<uint8_t>("Team") = *(*Controller->Member<UObject*>("PlayerState"))->Member<uint8_t>("TeamIndex");
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

	bool HasAircraftStarted()
	{
		return *GetGamePhase() == EAthenaGamePhase::Aircraft;
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

	void SpawnPlaysetFromDefinition(UObject* Definition, const FVector& Loc, const FRotator& Rot)
	{
		struct FFortPlaysetStreamingData
		{
			FName                                       PackageName;                                              // 0x0000(0x0008) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			FName                                       UniquePackageName;                                        // 0x0008(0x0008) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			FVector                                     Location;                                                 // 0x0010(0x000C) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			FRotator                                    Rotation;                                                 // 0x001C(0x000C) (ZeroConstructor, IsPlainOldData, NoDestructor, NativeAccessSpecifierPublic)
			unsigned char                                      bValid : 1;                                               // 0x0028(0x0001) (NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			unsigned char                                      UnknownData00[0x3];                                       // 0x0029(0x0003) MISSED OFFSET
		};

		struct {
			UObject* WorldContextObject;
			UObject* Playset;
			FVector Location;
			FRotator Rotation;
			FFortPlaysetStreamingData OutLevelData;
			UObject* LevelStreamingDynamic;
		} params{ Helper::GetWorld(), Definition, Loc, Rot };

		static auto def = FindObject("FortPlaysetItemDefinition /Script/FortniteGame.Default__FortPlaysetItemDefinition");

		std::cout << "Size of FFortPlaysetStreamingData: " << GetSizeOfStruct(FindObject("ScriptStruct /Script/FortniteGame.FortPlaysetStreamingData")) << '\n';
		std::cout << "Our size: " << sizeof(FFortPlaysetStreamingData) << '\n';

		static auto SpawnPlayset = def->Function("SpawnPlayset");

		if (SpawnPlayset)
			def->ProcessEvent(SpawnPlayset, &params);
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
	static FVector getRandomLocation()
	{
		static std::vector<FVector> Locations = {

			{ 24426, 37710, 25000 }, // retail row
			{ 50018, 73844, 25000 }, // lonely lodge
			{ 39781, 61621, 25000 }, // Moisty Mire
			{ -26479, 41847, 20000 }, // Prison
			{ 56771, 32818, 20000 }, // Containers/crates
			{ -75353, -8694, 20000 }, //Lucky Landing
			{ 34278, 867, 25000 }, // dusty depot / factories
			{ 79710, 15677, 25000 }, // tomato town
			{ 103901, -20203, 25000 }, // ANARCHY acres
			{ 86766, -83071, 25000 }, // pleasant park
			{ 2399, -96255, 25000 }, // greasy grove
			{ -35037, -463, 25000 }, // fatal fields
			{ 83375, 50856, 25000 }, // Wailing Woods
			{ 35000, -60121, 25000 }, // Tilted Towers
			{ 40000, -127121, 25000 }, // Snobby Shores
			{ 5000, -60121, 25000 }, // shifty shafts
			{ 110088, -115332, 25000 }, // Haunted Hills
			{ 119126, -86354, 25000 }, // Junk Houses
			{ 130036, -105092, 25000 }, // Junk Junction
			{ -68000, -63521, 25000 }, // Flush Factory
			{ 3502, -9183, 25000 }, // Salty Springs
			{ 7760, 76702, 25000 }, //race track
			{ 38374, -94726, 25000 }, //Soccer field
			{ 70000, -40121, 35000 }, // Loot Lake
			//New Locations: 7/4/22
			{ 117215, -53654, 25000 }, //motel
			{ 106521, -69597, 25000 }, //Pleasant Park Mountain
			{ 86980, -105015, 25000 }, //Pleasant Park Mountain 2
			{ 76292, -104977, 25000 }, //Haunted/Pleasant House
			{ 56131, -106880, 25000 }, //Snobby Mountain (Before Villain Lair)
			{ 29197, -109347, 25000 }, //Snobby Mountain 2
			{ -29734, -60767, 25000 }, //chair
			{ -19903, -26194, 25000 }, //Grandma's house
			{ -26851, 16299, 25000 }, //Tunnel near Fatal Fields
			{ -63592, 35933, 25000 }, //Random bush circle I've never seen before
			{ -75810, 33594, 25000 }, //Crab behind Moisty
			{ 28374, -94726, 25000 }, //Soccer mountain
			{ 73770, -19009, 25000 }, //Random Location 1
			{ 29050, -21225, 25000 }, //Dusty Mountain
			{ 18325, -17881, 25000 }, //Salty Mountain
			{ 6621, 18784, 25000 }, //Random Location 2
			{ -6702, 33251, 25000 }, //Random Location 3/bridge
			//Off map
			{ 137767, 40939, 25000 }, //off map near where risky would be
			{ 136084, -46013, 25000 }, //off map near motel
			{ -2450, -127394, 25000 }, //off map bottom left
			{ -26584, -90150, 25000 }, //off map bottom left 2
			{ 70000, -40121, 35000 } // Loot Lake
			//{ -123778, -112480, 20000 } //Spawn Island
		};

		auto Location = Locations[rand() % Locations.size()];
		return Location;
	}

	static UObject* GetRandomFoundation()
	{
		/*if (Helper::IsSmallZoneEnabled())
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
		}*/

		static auto FoundationClass = FindObject("Class /Script/FortniteGame.BuildingFoundation");
		auto AllFoundations = GetAllActorsOfClass_(FoundationClass);

		if (AllFoundations.Num() > 0)
		{
			while (true)
			{
				auto Foundation = AllFoundations.At(rand() % (AllFoundations.Num()));;

				if (Foundation)
					return Foundation;
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

	void DumpObjects()
	{
		std::ofstream objects("Objects.log");

		for (int32_t i = 0; i < (ObjObjects ? ObjObjects->Num() : OldObjects->Num()); i++)
		{
			auto Object = GetByIndex(i);

			if (!Object)
				continue;

			objects << std::format("[{}] {}\n", Object->InternalIndex, Object->GetFullName()); // TODO: add the offset
		}
		objects.close();
	}

	void SetHealth(UObject* Pawn, float Health)
	{
		static auto SetHealth = Pawn->Function("SetHealth");

		if (SetHealth)
			Pawn->ProcessEvent(SetHealth, &Health);
	}

	UObject* InitPawn(UObject* PC, bool bResetCharacterParts = false, FVector Location = Helper::GetPlayerStart(), bool bResetTeams = false)
	{
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

		Helper::SetOwner(Pawn, PC); //prob not needed

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
		static auto noBackpack = FindObject("CustomCharacterPart /Game/Characters/CharacterParts/Backpacks/NoBackpack.NoBackpack");

		if (!headPart)
			headPart = FindObject(("CustomCharacterPart /Game/Characters/CharacterParts/Female/Medium/Heads/CP_Head_F_RebirthDefaultA.CP_Head_F_RebirthDefaultA"));

		if (!bodyPart)
			bodyPart = FindObject(("CustomCharacterPart /Game/Athena/Heroes/Meshes/Bodies/CP_Body_Commando_F_RebirthDefaultA.CP_Body_Commando_F_RebirthDefaultA"));

		if (headPart && bodyPart && bResetCharacterParts)
		{
			Helper::ChoosePart(Pawn, EFortCustomPartType::Head, headPart);
			Helper::ChoosePart(Pawn, EFortCustomPartType::Body, bodyPart);
			Helper::ChoosePart(Pawn, EFortCustomPartType::Backpack, noBackpack);

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

		if (FnVerDouble >= 6.10) // WRONG
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

	auto GetStructuralSupportSystem()
	{
		auto GameState = Helper::GetGameState();

		return *GameState->Member<UObject*>("StructuralSupportSystem");
	}

	FBuildingSupportCellIndex GetCellIndexFromLocation(const FVector& Location)
	{
		auto StructuralSupportSystem = GetStructuralSupportSystem();

		struct
		{
			FVector                                     WorldLoc;                                                 // (ConstParm, Parm, OutParm, ReferenceParm, IsPlainOldData)
			FBuildingSupportCellIndex                   OutGridIndices;                                           // (Parm, OutParm)
			bool                                               ReturnValue;                                              // (Parm, OutParm, ZeroConstructor, ReturnParm, IsPlainOldData)
		} UBuildingStructuralSupportSystem_K2_GetGridIndicesFromWorldLoc_Params{Location};

		static auto K2_GetGridIndicesFromWorldLoc = StructuralSupportSystem->Function("K2_GetGridIndicesFromWorldLoc");

		if (K2_GetGridIndicesFromWorldLoc)
			StructuralSupportSystem->ProcessEvent(K2_GetGridIndicesFromWorldLoc, &UBuildingStructuralSupportSystem_K2_GetGridIndicesFromWorldLoc_Params);

		return UBuildingStructuralSupportSystem_K2_GetGridIndicesFromWorldLoc_Params.OutGridIndices;
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
