#pragma once

#include <iostream>
#include <fstream>
#include <UE/structs.h>
#include <Net/funcs.h>
#include <dpp/nlohmann/json.hpp>

static bool bPickupAnimsEnabled = true;

int GetMaxBullets(UObject* Definition)
{
	if (!Definition)
		return 0;

	struct FDataTableRowHandle
	{
	public:
		UObject* DataTable;                                         // 0x0(0x8)(Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		FName                                  RowName;                                           // 0x8(0x8)(Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	};

	static auto WeaponStatHandleOffset = GetOffset(Definition, "WeaponStatHandle");
	auto statHandle = (FDataTableRowHandle*)(__int64(Definition) + WeaponStatHandleOffset);

	if (!statHandle || !statHandle->DataTable || !statHandle->RowName.ComparisonIndex)
		return 0;

	auto RangedWeaponsTable = statHandle->DataTable;

	// auto RangedWeaponRows = GetRowMap(RangedWeaponsTable);

	static auto RowStructOffset = GetOffset(RangedWeaponsTable, "RowStruct");
	auto RangedWeaponRows = *(TMap<FName, uint8_t*>*)(__int64(RangedWeaponsTable) + (RowStructOffset + sizeof(UObject*))); // because after rowstruct is rowmap

	static auto ClipSizeOffset = FindOffsetStruct("ScriptStruct /Script/FortniteGame.FortBaseWeaponStats", "ClipSize");

	// std::cout << "Number of RangedWeapons: " << RangedWeaponRows.Pairs.Elements.Data.Num() << '\n';

	{
		for (int i = 0; i < RangedWeaponRows.Pairs.Elements.Data.Num(); i++)
		{
			auto& Man = RangedWeaponRows.Pairs.Elements.Data.At(i);
			auto& Pair = Man.ElementData.Value;
			auto RowFName = Pair.First;

			if (!RowFName.ComparisonIndex)
				continue;

			// if (RowFName.ToString() == statHandle->RowName.ToString())
			if (RowFName.ComparisonIndex == statHandle->RowName.ComparisonIndex)
			{
				auto data = Pair.Second;
				auto ClipSize = *(int*)(__int64(data) + ClipSizeOffset);
				// std::cout << "ClipSize: " << ClipSize << '\n';
				return ClipSize;
			}
		}
	}

	return 0;
}

static UObject* GetDefaultObject(UObject* Class)
{
	if (!Class)
		return nullptr;

	UObject* DefaultObject = nullptr;

	if (!Class->GetFullName().contains("Class "))
		DefaultObject = Class; //->CreateDefaultObject(); // Easy::SpawnObject(GameplayAbilityClass, GameplayAbilityClass->OuterPrivate);
	else
	{
		// im dumb
		static std::unordered_map<std::string, UObject*> defaultAbilities; // normal class name, default ability.

		auto name = Class->GetFullName();

		auto defaultafqaf = defaultAbilities.find(name);

		if (defaultafqaf != defaultAbilities.end())
		{
			DefaultObject = defaultafqaf->second;
		}
		else
		{
			// skunked class to default
			auto ending = name.substr(name.find_last_of(".") + 1);
			auto path = name.substr(0, name.find_last_of(".") + 1);

			path = path.substr(path.find_first_of(" ") + 1);

			auto DefaultAbilityName = std::format("{1} {0}Default__{1}", path, ending);

			// std::cout << "DefaultAbilityName: " << DefaultAbilityName << '\n';

			DefaultObject = FindObject(DefaultAbilityName);
			defaultAbilities.emplace(name, DefaultObject);
		}
	}

	return DefaultObject;
}

UObject* GetGameViewport()
{
	static auto GameViewportOffset = GetOffset(GetEngine(), "GameViewport");
	auto GameViewport = *(UObject**)(__int64(GetEngine()) + GameViewportOffset);

	return GameViewport;
}

UObject* GetWorldW()
{
	auto GameViewport = GetGameViewport();

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

bool SetActorLocation_(UObject* Actor, const FVector& Location)
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

	UObject* SpawnActor(UObject* Class, const FVector& Location = FVector(), const FRotator& Rotation = FRotator(), UObject* Owner = nullptr, bool bUseOtherSpawning = false)
	{
		if (!Class)
			return nullptr;

		if (Location.ContainsNaN())
		{
			std::cout << "NAN LOCATION!\n";
			return nullptr;
		}

		auto Loc = Location;
		auto Rot = Rotation;

		FTransform transform;
		transform.Translation = Loc;
		transform.Rotation = FQuat(); // Rotation.Quaternion();
		transform.Scale3D = { 1, 1, 1 };

		FActorSpawnParameters spawnParams = FActorSpawnParameters();
		spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		spawnParams.bAllowDuringConstructionScript = true;
		spawnParams.bNoFail = true;
		spawnParams.Owner = Owner;

		if (FnVerDouble < 19.00)
		{
			if (bUseOtherSpawning)
			{
				static auto GSC = FindObject(("GameplayStatics /Script/Engine.Default__GameplayStatics"));

				UObject* NewActor = nullptr;

				/* if (Engine_Version <= 422)
				{
					struct {
						UObject* WorldContextObject;
						UObject* ActorClass;
						FTransform SpawnTransform;
						bool bNoCollisionFail;
						UObject* Owner;
						UObject* NewActor;
					} beginParams{ GetWorldW(), Class, transform, spawnParams.bNoFail, nullptr };

					static auto BeginSpawningActorFromClass = GSC->Function("BeginSpawningActorFromClass");

					if (BeginSpawningActorFromClass)
						GSC->ProcessEvent(BeginSpawningActorFromClass, &beginParams);

					NewActor = beginParams.NewActor;
				}
				else */
				{
					struct {
						UObject* WorldContextObject;
						UObject* ActorClass;
						FTransform SpawnTransform;
						ESpawnActorCollisionHandlingMethod CollisionHandlingOverride;
						UObject* Owner;
						UObject* NewActor;
					} beginParams{ GetWorldW(), Class, transform, ESpawnActorCollisionHandlingMethod::AlwaysSpawn/* spawnParams.SpawnCollisionHandlingOverride */, nullptr};

					static auto BeginDeferredActorSpawnFromClass = GSC->Function("BeginDeferredActorSpawnFromClass");

					if (BeginDeferredActorSpawnFromClass)
						GSC->ProcessEvent(BeginDeferredActorSpawnFromClass, &beginParams);

					NewActor = beginParams.NewActor;
				}

				if (NewActor)
				{
					struct {
						UObject* Actor;
						FTransform Transform;
						UObject* ret;
					} finishParams{ NewActor, transform };

					static auto FinishSpawningActor = GSC->Function("FinishSpawningActor");

					if (GSC)
						GSC->ProcessEvent(FinishSpawningActor, &finishParams);

					return finishParams.ret;
				}
				else
					std::cout << "Failed beginning spawning actor!\n";
			}
			else 
			{
				return SpawnActorO(GetWorldW(), Class, &Loc, &Rot, spawnParams);
			}
		}
		else
		{
			auto GameInstance = *GetEngine()->Member<UObject*>(("GameInstance"));
			auto& LocalPlayers = *GameInstance->Member<TArray<UObject*>>(("LocalPlayers"));
			auto PlayerController = *LocalPlayers.At(0)->Member<UObject*>(("PlayerController"));

			if (!PlayerController)
				return nullptr;

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

			if (AllActors.Num() == 0)
			{
				std::cout << "Actor must have failed to spawn!\n";
				return nullptr;
			}

			auto ActorWeWant = AllActors.At(AllActors.Num() - 1);
			AllActors.Free();
			SetActorLocation_(ActorWeWant, Location);
			return ActorWeWant;
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

		FName StringToName(FString Str)
		{
			static auto KTL = FindObject(("KismetStringLibrary /Script/Engine.Default__KismetStringLibrary"));

			FName Ret;

			if (KTL)
			{
				static auto fn = KTL->Function(("Conv_StringToName"));

				struct {
					FString InStr;
					FName ReturnValue;
				} params{ Str };

				if (fn)
					KTL->ProcessEvent(fn, &params);
				else
					std::cout << ("Unable to find Conv_TextToString!\n");

				Ret = params.ReturnValue;
			}
			else
				std::cout << ("Unable to find KTL!\n");

			return Ret;
		}
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

	FVector GetActorLocation(UObject* Actor)
	{
		if (!Actor)
			return FVector();

		static auto K2_GetActorLocationFN = Actor->Function(("K2_GetActorLocation"));

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

	__int64* GetEntryFromPickup(UObject* Pickup)
	{
		static auto PrimaryPickupItemEntryOffset = GetOffset(Pickup, "PrimaryPickupItemEntry");
		auto PrimaryPickupItemEntry = (__int64*)(__int64(Pickup) + PrimaryPickupItemEntryOffset);

		return PrimaryPickupItemEntry;
	}

	void ShowBuilding(UObject* Foundation, bool bShow = true)
	{
		if (!Foundation)
			return;

		static auto DynamicFoundationTypeOffset = GetOffset(Foundation, "DynamicFoundationType");
		auto DynamicFoundationType = (uint8_t*)(__int64(Foundation) + DynamicFoundationTypeOffset);
		
		if (DynamicFoundationType && *DynamicFoundationType)
			*DynamicFoundationType = bShow ? 0 : 3;

		struct BITMF
		{
			uint8_t                                        bConditionalFoundation : 1;                        // Mask : 0x1 0xC68(0x1)(Edit, NoDestructor, Protected, HasGetValueTypeHash, NativeAccessSpecifierProtected)
			uint8_t                                        bServerStreamedInLevel : 1;
		};

		static auto bServerStreamedInLevelOffset = GetOffset(Foundation, "bServerStreamedInLevel");

		((BITMF*)(__int64(Foundation) + bServerStreamedInLevelOffset))->bServerStreamedInLevel = bShow; // fixes hlods

		static auto OnRep_ServerStreamedInLevel = Foundation->Function("OnRep_ServerStreamedInLevel");

		if (OnRep_ServerStreamedInLevel)
			Foundation->ProcessEvent(OnRep_ServerStreamedInLevel);

		if (FnVerDouble >= 10.0) {
			auto DynamicFoundationRepData = Foundation->Member<__int64>("DynamicFoundationRepData");
			static int EnabledState_Offset = FindOffsetStruct("ScriptStruct /Script/FortniteGame.DynamicBuildingFoundationRepData", "EnabledState");
			*reinterpret_cast<uint8_t*>(__int64(DynamicFoundationRepData) + EnabledState_Offset) = 1;
			*Foundation->Member<uint8_t>("FoundationEnabledState") = 1;
			Foundation->ProcessEvent("OnRep_DynamicFoundationRepData");
		}
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
			auto Volcano = FindObject(("LF_Athena_POI_50x50_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.LF_Athena_POI_50x53_Volcano"));
			ShowBuilding(Volcano);
		}
		//Pleasant
		if (Season == 7) {
			auto idfk = FindObject(("LF_Athena_POI_25x25_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.LF_Athena_POI_25x36")); // polar peak?
			ShowBuilding(idfk);

			auto tiltedtower = FindObject("BuildingFoundation5x5 /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.ShopsNew");
			ShowBuilding(tiltedtower);
		}

		if (Season >= 7 && Engine_Version < 424)
		{
			auto TheBlock = FindObject("BuildingFoundationSlab_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.SLAB_2"); // SLAB_3 is blank
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
			auto PleasantPark = FindObject(("LF_Athena_POI_50x50_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.PleasentParkFestivus"));
			ShowBuilding(PleasantPark);
		}

		if (Season == 13) {
			UObject* WL = FindObject("Apollo_WaterSetup_C /Game/Athena/Apollo/Maps/Apollo_POI_Foundations.Apollo_POI_Foundations.PersistentLevel.Apollo_WaterSetup_2");
			UObject* Func = WL->Function("SetWaterLevel");
			UObject* Func2 = WL->Function("OnRep_CurrentWaterLevel");
			int NewWaterLevel = 0;
			WL->ProcessEvent(Func, &NewWaterLevel);
			WL->ProcessEvent(Func2);
		}

		//Loot Lake
		if (Season == 6) {
			if (FnVerDouble != 6.10)
			{
				auto FloatingIsland = FindObject(("LF_Athena_POI_15x15_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.LF_FloatingIsland"));
				auto Lake = FindObject(("LF_Athena_POI_75x75_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.LF_Lake1"));
				auto Lake2 = FindObject("LF_Athena_POI_75x75_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.LF_Lake2");

				ShowBuilding(FloatingIsland);

				if (FnVerDouble <= 6.21)
					ShowBuilding(Lake);
				else
					ShowBuilding(Lake2); // after event // do we have to do this lol
			}
			else
			{
				auto FloatingIsland = FindObject(("LF_Athena_POI_15x15_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.LF_Athena_StreamingTest13"));
				auto Lake = FindObject(("LF_Athena_POI_75x75_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.LF_Athena_StreamingTest12"));

				ShowBuilding(FloatingIsland);
				ShowBuilding(Lake);
			}

			// *scripting->Member<FVector>("IslandPosition") = Helper::GetActorLocation(FloatingIsland);

		}
	}

	void SetHealth()
	{

	}

	void ForceNetUpdate(UObject* Actor)
	{
		static auto ForceNetUpdate = Actor->Function("ForceNetUpdate");

		if (ForceNetUpdate)
			Actor->ProcessEvent(ForceNetUpdate);
	}

	void SetShield(UObject* Pawn, float Shield)
	{
		static auto setShieldFn = Pawn->Function(("SetShield"));
		struct { float NewValue; }shieldParams{ Shield };

		if (setShieldFn)
			Pawn->ProcessEvent(setShieldFn, &shieldParams);
		else
			std::cout << ("Unable to find setShieldFn!\n");
	}
	
	UObject* GetWorld()
	{
		return GetWorldW();
	}

	static void ChoosePart(UObject* Pawn, TEnumAsByte<EFortCustomPartType> Part, UObject* ChosenCharacterPart)
	{
		struct {
			TEnumAsByte<EFortCustomPartType> Part;
			UObject* ChosenCharacterPart;
		} params{ Part, ChosenCharacterPart };

		static auto chooseFn = Pawn->Function(("ServerChoosePart"));
		Pawn->ProcessEvent(chooseFn, &params);
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

	UObject* GetTransientPackage()
	{
		static auto TransientPackage = FindObject("Package /Engine/Transient");
		return TransientPackage;
	}

	static std::vector<UObject*> GetAllObjectsOfClass(UObject* Class)
	{
		std::vector<UObject*> Objects;

		for (int32_t i = 0; i < (ObjObjects ? ObjObjects->Num() : OldObjects->Num()); i++)
		{
			auto Object = ObjObjects ? ObjObjects->GetObjectById(i) : OldObjects->GetObjectById(i);

			if (!Object) continue;

			if (Object->IsA(Class))
			{
				Objects.push_back(Object);
			}
		}

		return Objects;
	}

	UObject* GetBGAClass()
	{
		static auto BGAClass = FindObject("Class /Script/Engine.BlueprintGeneratedClass");

		return BGAClass;
	}

	void EnablePickupAnimation(UObject* Pawn, UObject* Pickup, float FlyTime = 0.75f, FVector StartDirection = FVector())
	{
		if (bPickupAnimsEnabled)
		{
			static auto PickupLocationDataOffset = GetOffset(Pickup, "PickupLocationData");
			auto PickupLocationData = (FFortPickupLocationData*)(__int64(Pickup) + PickupLocationDataOffset);

			if (PickupLocationData)
			{
				PickupLocationData->PickupTarget = Pawn;
				PickupLocationData->ItemOwner = Pawn;
				// PickupLocationData->LootInitialPosition = Helper::GetActorLocation(Pickup);
				PickupLocationData->FlyTime = FlyTime;
				PickupLocationData->StartDirection = StartDirection;
				static auto GuidOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortItemEntry"), ("ItemGuid"));
				auto Guid = (FGuid*)(__int64(GetEntryFromPickup(Pickup)) + GuidOffset);

				if (Guid)
					PickupLocationData->PickupGuid = *Guid; // *FFortItemEntry::GetGuid(Pickup->Member<__int64>(("PrimaryPickupItemEntry")));
			}
		}

		static auto OnRep_PickupLocationData = Pickup->Function(("OnRep_PickupLocationData"));

		if (OnRep_PickupLocationData)
			Pickup->ProcessEvent(OnRep_PickupLocationData);
		else
			std::cout << "Failed to find OnRep_PickupLocationData!\n";
	}

	static UObject* GetSkeletalMesh(UObject* Pawn)
	{
		if (!Pawn)
			return nullptr;

		auto Mesh = Pawn->Member<UObject*>(("Mesh"));

		if (Mesh)
			return *Mesh;

		return nullptr;
	}

	static UObject* GetAnimInstance(UObject* Pawn)
	{
		if (!Pawn)
			return nullptr;

		auto Mesh = GetSkeletalMesh(Pawn);

		if (!Mesh)
			return nullptr;

		static auto fn = Mesh->Function(("GetAnimInstance"));

		struct {
			UObject* ReturnValue; // UAnimInstance*
		} params;

		Mesh->ProcessEvent(fn, &params);

		return params.ReturnValue;
	}

	UObject* SummonPickup(UObject* Pawn, UObject* Definition, FVector Location, EFortPickupSourceTypeFlag PickupSource, EFortPickupSpawnSource SpawnSource, int Count = 1, bool bTossPickup = true, bool bMaxAmmo = true, int ammo = 0)
	{
		static UObject* PickupClass = FindObject(("Class /Script/FortniteGame.FortPickupAthena")); // Class FortniteGame.FortGameModePickup

		auto Pickup = Easy::SpawnActor(PickupClass, Location, FRotator());  //, false && FnVerDouble < 19.00);

		if (Pickup && Definition)
		{
			auto ItemEntry = GetEntryFromPickup(Pickup);

			static auto CountOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortItemEntry"), ("Count"));
			static auto ItemDefOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortItemEntry"), ("ItemDefinition"));
			static auto LoadedAmmoOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.FortItemEntry"), ("LoadedAmmo"));

			if (bMaxAmmo)
				*(int*)(__int64(&*ItemEntry) + LoadedAmmoOffset) = GetMaxBullets(Definition);
			else
				*(int*)(__int64(&*ItemEntry) + LoadedAmmoOffset) = ammo;

			static auto OnRep_PrimaryPickupItemEntry = Pickup->Function(("OnRep_PrimaryPickupItemEntry"));

			if (ItemEntry && OnRep_PrimaryPickupItemEntry)
			{
				*(int*)(__int64(ItemEntry) + CountOffset) = Count;
				*(UObject**)(__int64(ItemEntry) + ItemDefOffset) = Definition;

				Pickup->ProcessEvent(OnRep_PrimaryPickupItemEntry);

				// UFortNotificationHandler
				// what is this FFortPickupEntryData

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
							EFortPickupSourceTypeFlag InPickupSourceTypeFlags;
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
							EFortPickupSourceTypeFlag InPickupSourceTypeFlags;
							EFortPickupSpawnSource InPickupSpawnSource;
						} TPParams{ Location, Pawn, 6, true, true, PickupSource, SpawnSource };

						if (TossPickupFn)
							Pickup->ProcessEvent(TossPickupFn, &TPParams);
					}
				}

				if (PickupSource == EFortPickupSourceTypeFlag::Container)
				{
					static auto bTossedFromContainerOffset = GetOffset(Pickup, "bTossedFromContainer");
					*(bool*)(__int64(Pickup) + bTossedFromContainerOffset) = true;

					static auto OnRep_TossedFromContainer = Pickup->Function(("OnRep_TossedFromContainer"));

					if (OnRep_TossedFromContainer)
						Pickup->ProcessEvent(OnRep_TossedFromContainer);
				}

				static auto SetReplicateMovementFn = Pickup->Function(("SetReplicateMovement"));
				struct { bool b; } bruh{ true };
				Pickup->ProcessEvent(SetReplicateMovementFn, &bruh);

				static auto Rep_ReplicateMovement = Pickup->Function(("OnRep_ReplicateMovement"));
				Pickup->ProcessEvent(Rep_ReplicateMovement);
				
				static auto ProjectileMovementComponentClass = FindObject("Class /Script/Engine.ProjectileMovementComponent");
				// UFortProjectileMovementComponent

				static auto MovementComponentOffset = GetOffset(Pickup, "MovementComponent");
				auto MovementComponent = (UObject**)(__int64(Pickup) + MovementComponentOffset);
				*MovementComponent = Easy::SpawnObject(ProjectileMovementComponentClass, Pickup);
			}
		}

		return Pickup;
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

	UObject* GetControllerFromPawn(UObject* Pawn)
	{
		if (!Pawn)
			return nullptr;

		static auto ControllerOffset = GetOffset(Pawn, "Controller");

		return *(UObject**)(__int64(Pawn) + ControllerOffset);
	}

	UObject* GetPlayerStateFromController(UObject* PC)
	{
		if (!PC)
			return nullptr;

		static auto PlayerStateOffset = GetOffset(PC, "PlayerState");

		return *(UObject**)(__int64(PC) + PlayerStateOffset);
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

		GSCClass->ProcessEvent(GetAllActorsOfClass, &Params);

		return Params.ReturnValue;
	}

	static bool IsRespawnEnabled()
	{
		bool bIsRespawningEnabled = bIsPlayground;
		return bIsRespawningEnabled;
	}

	static void InitializeBuildingActor(UObject* Controller, UObject* BuildingActor, bool bUsePlayerBuildAnimations = false, UObject* ReplacedBuilding = nullptr)
	{
		struct {
			UObject* BuildingOwner; // ABuildingActor
			UObject* SpawningController;
			bool bUsePlayerBuildAnimations; // I think this is not on some versions
			UObject* ReplacedBuilding; // this also not on like below 18.00
		} IBAParams{ BuildingActor, Controller, bUsePlayerBuildAnimations, ReplacedBuilding };

		if (Controller && BuildingActor)
		{
			static auto fn = BuildingActor->Function(("InitializeKismetSpawnedBuildingActor"));

			if (fn)
				BuildingActor->ProcessEvent(fn, &IBAParams);
		}
	}

	static UObject* SpawnChip(UObject* Controller, FVector ChipLocation)
	{
		static auto ChipClass = FindObject(("/Game/Athena/Items/EnvironmentalItems/SCMachine/BGA_Athena_SCMachine_Pickup.BGA_Athena_SCMachine_Pickup_C"));

		auto Pawn = Helper::GetPawnFromController(Controller);

		if (ChipClass && Pawn)
		{
			auto PlayerState = Pawn->Member<UObject*>(("PlayerState"));

			if (PlayerState && *PlayerState)
			{
				std::cout << ("Spawning Chip!\n");

				auto Chip = Easy::SpawnActor(ChipClass, ChipLocation, Helper::GetActorRotation(Pawn));

				std::cout << ("Initializing Chip!\n");

				Helper::InitializeBuildingActor(Controller, Chip);

				std::cout << ("Initialized Chip!\n");

				*Chip->Member<int>("OwnerTeam") = *(*PlayerState)->Member<unsigned char>(("TeamIndex"));
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
			FRotator& Rotation;
			bool Res;
		} params{ Location, Rot, false };

		auto fn = Actor->Function(("K2_TeleportTo"));

		if (fn)
			Actor->ProcessEvent(fn, &params);

		return params.Res;
	}

	UObject* GetGameMode()
	{
		auto world = GetWorldW();

		static auto AuthorityGameModeOffset = GetOffset(world, "AuthorityGameMode");

		return *(UObject**)(__int64(world) + AuthorityGameModeOffset);
	}

	UObject* GetGameState(UObject* World = nullptr)
	{
		auto world = World ? World : Helper::GetWorld();

		static auto gameStateOffset = GetOffset(world, "GameState");
		auto gameState = *(UObject**)(__int64(world) + gameStateOffset);

		return gameState;
	}

	auto GetStructuralSupportSystem()
	{
		auto GameState = Helper::GetGameState();

		return *GameState->Member<UObject*>("StructuralSupportSystem");
	}

	void IDoNotKnow(UObject* BuildingActor)
	{
		/* void(__fastcall * IDONTKNOW)(UObject * a1);

		auto Addy = FindPattern("40 53 48 81 EC ? ? ? ? F6 81 ? ? ? ? ? 48 8B D9 0F 85 ? ? ? ? 48 8B 01 48 89 BC 24 ? ? ? ? FF 90 ? ? ? ? 48 8B C8 E8 ? ? ? ? 48 8B F8 0F B6 83 ? ? ? ? 24 40 75 4A 48 8B 8B ? ? ? ? 48 85 C9 74 3E 48 8B 01");

		IDONTKNOW = decltype(IDONTKNOW)(Addy);

		std::cout << "IDONTKNOW ADDY: " << IDONTKNOW << '\n';

		return IDONTKNOW ? IDONTKNOW(BuildingActor) : void(); */

		/* char(__fastcall * sub_7FF706A6EF90)(UObject * a1, UObject * a2, int* a3);

		auto addy = FindPattern("48 89 5C 24 ? 48 89 74 24 ? 55 57 41 56 48 8B EC 48 81 EC ? ? ? ? 48 8B 02 48 8B F2 4D 8B F0 48 8D 55 D0 48 8B D9 33 FF 45");

		sub_7FF706A6EF90 = decltype(sub_7FF706A6EF90)(addy);

		std::cout << "ADXDY: " << addy << '\n';

		int idk = -1;

		if (sub_7FF706A6EF90)
			sub_7FF706A6EF90(Helper::GetStructuralSupportSystem(), BuildingActor, &idk);

		std::cout << "idk: " << idk << '\n'; */

		// int someoutidk = -1;

		// bool(__fastcall * hmmmmm)(UObject * StructuralSupportSystem, float* a2, int* someOut);

		/* float idk;

		__int64(__fastcall * bozoratio)(UObject* a1, float* a2, __int64 a3, __int64 a4);

		bozoratio = decltype(bozoratio)(FindPattern("48 89 74 24 ? 48 89 7C 24 ? 55 48 8B EC 48 81 EC ? ? ? ? 48 8B FA 48 8B F1 4D 85 C0 74 17 41"));

		bozoratio(BuildingActor, &idk, 0, 0);
		
		std::cout << "idk: " << idk << '\n'; */

		/* auto addy = FindPattern("48 83 EC 28 F3 0F 10 99 ? ? ? ? 4D 8B D0 F3 0F 10 0D ? ? ? ? F3 0F 10 15 ? ? ? ? 0F 29 74 24 ? 0F 29 3C");

		hmmmmm = decltype(hmmmmm)(addy);

		std::cout << "hmmmmm addy: " << hmmmmm << '\n';

		float idk = 0;
		int v35[2];

		auto reet = hmmmmm(Helper::GetStructuralSupportSystem(), &idk, v35);

		std::cout << "idkafg9qu: " << someoutidk << '\n';

		std::cout << "rett: " << reet << '\n'; */
	}

	char CanAddBuildingActorClass(UObject* BuildingActorClass)
	{
		auto Addy = FindPattern("40 55 56 57 41 55 41 56 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 83 B9 ? ? ? ? ? 4D 8B E8 48 8B F2 48 8B F9 74 1A 48 8B 89 ? ? ? ? 48 85 C9");
	
		char(__fastcall * CanAddBuildingActorClassToGrid)(UObject * World, UObject * a2, UObject * StructuralSupportSystem);

		CanAddBuildingActorClassToGrid = decltype(CanAddBuildingActorClassToGrid)(Addy);

		std::cout << "CanAddBuildingActorClassToGrid Addy: " << CanAddBuildingActorClassToGrid << '\n';

		return CanAddBuildingActorClassToGrid ? CanAddBuildingActorClassToGrid(GetWorldW(), BuildingActorClass, GetStructuralSupportSystem()) : false;
	}

	EAthenaGamePhase* GetGamePhase()
	{
		return GetGameState()->Member<EAthenaGamePhase>(("GamePhase"));
	}

	bool HasAircraftStarted()
	{
		return *GetGamePhase() >= EAthenaGamePhase::Aircraft;
	}

	UObject* ReplaceBuildingActor(UObject* Controller, UObject* BuildingActor, UObject* NewBuildingClass, int RotationIterations, bool bMirrored)
	{
		static auto BuildingSMActorReplaceBuildingActorAddr = FindPattern("4C 8B DC 55 57 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 48 8B 85 ? ? ? ? 33 FF 40 38 3D ? ? ? ?");

		if (!BuildingSMActorReplaceBuildingActorAddr)
		{
			if (Engine_Version < 421)
			{
				BuildingSMActorReplaceBuildingActorAddr = FindPattern("48 8B C4 4C 89 40 18 55 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 48 89 70 E8 33 FF 40 38 3D ? ? ? ? 48 8B F1 4C 89 60 E0 44 8B E2 4C 89 70 D0 44 8B F7 4C 89 78 C8 48 8D 05 ? ? ? ?");
			}
			else
			{
				if (Engine_Version >= 426 && !BuildingSMActorReplaceBuildingActorAddr)
				{
					BuildingSMActorReplaceBuildingActorAddr = FindPattern("48 8B C4 48 89 58 18 55 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 70 B8 0F 29 78 A8 44 0F 29 40 ? 44 0F 29 48 ? 44 0F 29 90 ? ? ? ? 44 0F 29 B8 ? ? ? ? 48 8B 05");

					if (!BuildingSMActorReplaceBuildingActorAddr) // c3
						BuildingSMActorReplaceBuildingActorAddr = FindPattern("48 8B C4 48 89 58 18 55 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 70 B8 0F 29 78 A8 44 0F 29 40 ? 44 0F 29 48 ? 44 0F 29 90 ? ? ? ? 48 8B 05 ? ? ? ?");
				}

				if (!BuildingSMActorReplaceBuildingActorAddr || Engine_Version <= 421)
					BuildingSMActorReplaceBuildingActorAddr = FindPattern("48 8B C4 44 89 48 20 55 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 48 89 70 E8 33 FF 40 38 3D ? ? ? ? 48 8B F1 4C 89 60 E0 44 8B E2");
			}
		}

		UObject* (__fastcall * BuildingSMActorReplaceBuildingActor)(UObject* BuildingSMActor, unsigned int ReplacementType, UObject* a3, unsigned int BuildingLevel, int a5, unsigned __int8 bMirrored, UObject * Controller);

		BuildingSMActorReplaceBuildingActor = decltype(BuildingSMActorReplaceBuildingActor)(BuildingSMActorReplaceBuildingActorAddr);

		if (BuildingSMActorReplaceBuildingActor)
		{
			auto newActor = BuildingSMActorReplaceBuildingActor(BuildingActor, 1, NewBuildingClass, 0, RotationIterations, bMirrored, Controller);
			// std::cout << "new: " << newActor << '\n';

			return newActor;
		}
		else
			std::cout << "No BuildingSMActorReplaceBuildingActor!\n";

		return nullptr;
	}

	void SetMirrored(UObject* BuildingActor, bool bMirrored)
	{
		if (!BuildingActor)
			return;

		static auto SetMirrored = BuildingActor->Function("SetMirrored");

		if (SetMirrored)
			BuildingActor->ProcessEvent(SetMirrored, &bMirrored);
	}

	FVector GetPlayerStart(UObject** PlayerStartActor = nullptr)
	{
		static auto WarmupClass = bIsCreative ? FindObject("Class /Script/FortniteGame.FortPlayerStartCreative") : FindObject(("Class /Script/FortniteGame.FortPlayerStartWarmup"));
		TArray<UObject*> OutActors = GetAllActorsOfClass(WarmupClass);

		auto ActorsNum = OutActors.Num();

		auto SpawnTransform = FTransform();
		SpawnTransform.Scale3D = FVector(1, 1, 1);
		SpawnTransform.Rotation = FQuat();
		SpawnTransform.Translation = FVector{ 1250, 1818, 3284 }; // Next to salty

		auto GamePhase = *GetGamePhase();

		if (WarmupClass && ActorsNum != 0 && (GamePhase <= EAthenaGamePhase::Warmup))
		{
			auto ActorToUseNum = RandomIntInRange(2, ActorsNum - 1);
			auto ActorToUse = (OutActors)[ActorToUseNum];

			while (!ActorToUse)
			{
				ActorToUseNum = RandomIntInRange(2, ActorsNum - 1);
				ActorToUse = (OutActors)[ActorToUseNum];
			}

			if (PlayerStartActor)
				*PlayerStartActor = ActorToUse;

			static auto ActorClass = FindObject("Class /Script/Engine.Actor");

			if (ActorToUse->IsA(ActorClass))
				SpawnTransform.Translation = GetActorLocation(ActorToUse);

			// PC->WarmupPlayerStart = static_cast<AFortPlayerStartWarmup*>(ActorToUse);
		}

		OutActors.Free();

		return SpawnTransform.Translation;
	}

	void LoopAlivePlayers(std::function<void(UObject* Controller)> fn)
	{
		auto GameMode = Helper::GetGameMode();
		static auto AlivePlayersOffset = GetOffset(GameMode, "AlivePlayers");

		auto AlivePlayers = (TArray<UObject*>*)(__int64(GameMode) + AlivePlayersOffset);

		if (AlivePlayers)
		{
			for (int i = 0; i < AlivePlayers->Num(); i++)
			{
				auto AlivePlayer = AlivePlayers->At(i);

				if (!AlivePlayer)
					return;

				fn(AlivePlayer);
			}
		}
	}

	bool IsControllerAlive(UObject* Controller) // NOT RECOMMENDED
	{
		// bMarkedAlive

		bool bIsControllerAlive = false;

		auto CompareController = [&bIsControllerAlive, &Controller](UObject* OtherController) {
			if (Controller == OtherController)
			{
				bIsControllerAlive = true;
			}
		};

		LoopAlivePlayers(CompareController);

		return bIsControllerAlive;
	}

	void LoopConnections(std::function<void(UObject* Controller)> fn)
	{
		auto World = Helper::GetWorld();

		if (World)
		{
			static auto NetDriverOffset = GetOffset(World, "NetDriver");
			auto NetDriver = *(UObject**)(__int64(World) + NetDriverOffset);
			if (NetDriver)
			{
				static auto ClientConnectionsOffset = GetOffset(NetDriver, "ClientConnections");
				auto ClientConnections = (TArray<UObject*>*)(__int64(NetDriver) + ClientConnectionsOffset);

				if (ClientConnections)
				{
					for (int i = 0; i < ClientConnections->Num(); i++)
					{
						auto Connection = ClientConnections->At(i);

						if (!Connection)
							continue;

						static auto Connection_PlayerControllerOffset = GetOffset(Connection, "PlayerController");
						auto aaController = *(UObject**)(__int64(Connection) + Connection_PlayerControllerOffset);

						if (aaController)
						{
							// auto aaPlayerState = Helper::GetPlayerStateFromController(aaController);
							auto aaPawn = Helper::GetPawnFromController(aaController);

							if (aaPawn)
							{
								static auto IsActorBeingDestroyed = aaPawn->Function(("IsActorBeingDestroyed"));

								bool bIsActorBeingDestroyed = true;

								if (IsActorBeingDestroyed)
									aaPawn->ProcessEvent(IsActorBeingDestroyed, &bIsActorBeingDestroyed);

								fn(aaController);
							}
						}
					}
				}
			}
		}
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

		static auto LaunchPlayerFn = Pawn->Function("LaunchCharacterJump");

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

	void SetGamePhase(EAthenaGamePhase newPhase)
	{
		auto GamePhase = Helper::GetGameState()->Member<EAthenaGamePhase>(("GamePhase"));

		// if (Engine_Version >= 420)
		if (GamePhase)
		{
			*GamePhase = newPhase;

			struct {
				EAthenaGamePhase OldPhase;
			} params2{ EAthenaGamePhase::None };

			static const auto fnGamephase = Helper::GetGameState()->Function(("OnRep_GamePhase"));

			if (fnGamephase)
				Helper::GetGameState()->ProcessEvent(fnGamephase, &params2);
		}
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
			auto GameViewport = Engine->Member<UObject*>(("GameViewport"));

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

		void ExecuteConsoleCommand(FString Command)
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

	static FString GetfPlayerName(UObject* Controller)
	{
		if (!Controller)
			return FString();

		auto MCPProfileGroupPTR = Controller->Member<UObject*>("McpProfileGroup");

		FString Name; // = *PlayerState->Member<FString>(("PlayerNamePrivate"));

		if (!MCPProfileGroupPTR || !*MCPProfileGroupPTR)
		{
			auto PlayerState = Helper::GetPlayerStateFromController(Controller);

			if (PlayerState)
			{
				static auto fn = PlayerState->Function(("GetPlayerName"));

				if (fn)
					PlayerState->ProcessEvent(fn, &Name);
			}
		}
		else
		{
			auto MCPProfileGroup = *MCPProfileGroupPTR;

			Name = *MCPProfileGroup->Member<FString>("PlayerName");
		}

		return Name;
	}

	static bool IsSmallZoneEnabled()
	{
		return bIsLateGame || FnVerDouble >= 13.00;
	}

	float GetDistanceTo(UObject* Actor, UObject* OtherActor)
	{
		static auto GetDistanceTo = Actor->Function("GetDistanceTo");

		struct { UObject* otherActor; float distance; } GetDistanceTo_Params{ OtherActor };

		if (GetDistanceTo)
			Actor->ProcessEvent(GetDistanceTo, &GetDistanceTo_Params);

		return GetDistanceTo_Params.distance;
	}

	static UObject* GetRandomFoundation()
	{
		return nullptr;

		srand(time(0));

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

	static std::string GetPlayerName(UObject* Controller)
	{
		auto name = GetfPlayerName(Controller);
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

	static UObject* GetPickaxeDef(UObject* Controller)
	{
		// GlobalPickaxeDefObject = FindObject(PickaxeDef);
		return GlobalPickaxeDefObject;
	}

	void SetHealth(UObject* Pawn, float Health)
	{
		static auto SetHealth = Pawn->Function("SetHealth");

		if (SetHealth)
			Pawn->ProcessEvent(SetHealth, &Health);
	}

	void SetMaxShield(UObject* Pawn, float MaxShield, UObject* PlayerState = nullptr)
	{
		if (!Pawn)
			return;

		if (Engine_Version <= 420)
		{
			PlayerState = PlayerState ? PlayerState : Helper::GetPlayerStateFromController(Helper::GetControllerFromPawn(Pawn));

			static auto MaxShieldOffset = GetOffset(PlayerState, "MaxShield");
			*(float*)(__int64(PlayerState) + MaxShieldOffset) = MaxShield;
		}
		else
		{
			static auto setMaxShieldFn = Pawn->Function(("SetMaxShield"));

			if (setMaxShieldFn)
				Pawn->ProcessEvent(setMaxShieldFn, &MaxShield);
			else
				std::cout << ("Unable to find setMaxShieldFn!\n");

		}
	}

	UObject* GetGameData()
	{
		return *(*GetEngine()->Member<UObject*>("AssetManager"))->Member<UObject*>("GameData");
	}

	void ApplyCID(UObject* Pawn, UObject* CID)
	{
		// CID->ItemVariants
		// CID->DefaultBackpack

		// CID->Hero->Specialization
		
		if (!CID)
			return;

		static auto CCPClass = FindObject("Class /Script/FortniteGame.CustomCharacterPart");

		static auto HeroDefinitionOffset = GetOffset(CID, "HeroDefinition");

		auto HeroDefinition = *(UObject**)(__int64(CID) + HeroDefinitionOffset);

		if (!HeroDefinition)
			return;

		auto HeroSpecializations = HeroDefinition->CachedMember<TArray<TSoftObjectPtr>>("Specializations");

		if (!HeroSpecializations)
		{
			std::cout << "No HeroSpecializations!\n";
			return;
		}

		for (int j = 0; j < HeroSpecializations->Num(); j++)
		{
			static auto SpecializationClass = FindObject("Class /Script/FortniteGame.FortHeroSpecialization");

			auto SpecializationName = HeroSpecializations->At(j).ObjectID.AssetPathName.ToString();

			auto Specialization = StaticLoadObject(SpecializationClass, nullptr, SpecializationName);

			auto CharacterParts = Specialization->Member<TArray<TSoftObjectPtr>>("CharacterParts");

			if (!CharacterParts)
			{
				std::cout << "No CharacterParts!\n";
				return;
			}

			for (int i = 0; i < CharacterParts->Num(); i++)
			{
				auto CharacterPart = StaticLoadObject(CCPClass, nullptr, CharacterParts->At(i).ObjectID.AssetPathName.ToString());

				if (CharacterPart)
				{
					auto PartType = *CharacterPart->CachedMember<TEnumAsByte<EFortCustomPartType>>("CharacterPartType");
					Helper::ChoosePart(Pawn, PartType, CharacterPart);
				}
			}
		}
	}

	UObject* InitPawn(UObject* PC, bool bResetCharacterParts = false, FVector Location = Helper::GetPlayerStart(), bool bResetTeams = false)
	{
		UObject* PlayerState = GetPlayerStateFromController(PC);

		static auto PawnClass = FindObject(("BlueprintGeneratedClass /Game/Athena/PlayerPawn_Athena.PlayerPawn_Athena_C"));
		auto Pawn = Easy::SpawnActor(PawnClass, Location, {});

		if (!Pawn)
			return nullptr;

		static auto SetReplicateMovementFn = Pawn->Function(("SetReplicateMovement"));
		struct { bool b; } bruh{true};
		Pawn->ProcessEvent(SetReplicateMovementFn, &bruh);

		// setBitfield(Pawn, "bReplicateMovement", true);

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

		SetMaxShield(Pawn, 100, PlayerState);

		static auto setMaxHealthFn = Pawn->Function(("SetMaxHealth"));
		struct { float NewHealthVal; }healthParams{ 100 };

		if (setMaxHealthFn)
			Pawn->ProcessEvent(setMaxHealthFn, &healthParams);
		else
			std::cout << ("Unable to find setMaxHealthFn!\n");

		auto CIDObject = CIDToUse == "None" ? nullptr : FindObject(CIDToUse); // we need the check cuz if we dont have staticfindobject then it finds it

		// if (FnVerDouble < 4)
		{
			static auto HeroDefinitionOffset = GetOffset(CIDObject, "HeroDefinition");

			const auto HeroType = CIDObject ? *(UObject**)(__int64(CIDObject) + HeroDefinitionOffset)
				: FindObject(("FortHeroType /Game/Athena/Heroes/HID_058_Athena_Commando_M_SkiDude_GER.HID_058_Athena_Commando_M_SkiDude_GER"));

			static auto HeroTypeOffset = GetOffset(PlayerState, "HeroType");
			*(UObject**)(__int64(PlayerState) + HeroTypeOffset) = HeroType;

			static auto OnRepHeroType = PlayerState->Function(("OnRep_HeroType"));

			if (OnRepHeroType)
				PlayerState->ProcessEvent(OnRepHeroType);
		}

		if (FnVerDouble < 19.00 && bResetCharacterParts) // they are just automatic when you touch the ground lol
		{
			static auto CCPClass = FindObject("Class /Script/FortniteGame.CustomCharacterPart");
			static auto backpackPart = FindObject("CustomCharacterPart /Game/Characters/CharacterParts/Backpacks/NoBackpack.NoBackpack");

			static auto headPart = FindObject(("CustomCharacterPart /Game/Characters/CharacterParts/Female/Medium/Heads/F_Med_Head1.F_Med_Head1"));
			static auto bodyPart = FindObject(("CustomCharacterPart /Game/Characters/CharacterParts/Female/Medium/Bodies/F_Med_Soldier_01.F_Med_Soldier_01"));
			static auto hatPart = nullptr;

			if (!headPart)
				headPart = FindObject(("CustomCharacterPart /Game/Characters/CharacterParts/Female/Medium/Heads/CP_Head_F_RebirthDefaultA.CP_Head_F_RebirthDefaultA"));

			if (!bodyPart)
				bodyPart = FindObject(("CustomCharacterPart /Game/Athena/Heroes/Meshes/Bodies/CP_Body_Commando_F_RebirthDefaultA.CP_Body_Commando_F_RebirthDefaultA"));

			if (headPart && bodyPart)
			{
				Helper::ChoosePart(Pawn, EFortCustomPartType::Head, headPart);
				Helper::ChoosePart(Pawn, EFortCustomPartType::Body, bodyPart);
				Helper::ChoosePart(Pawn, EFortCustomPartType::Hat, hatPart);
				Helper::ChoosePart(Pawn, EFortCustomPartType::Backpack, backpackPart);
			}
			else
				std::cout << ("Unable to find Head and Body!\n");

			if (CIDObject)
			{
				ApplyCID(Pawn, CIDObject);
			}
		}

		static auto OnRep_Parts = (FnVerDouble >= 10) ? PlayerState->Function(("OnRep_CharacterData")) : PlayerState->Function(("OnRep_CharacterParts"));

		if (OnRep_Parts)
			PlayerState->ProcessEvent(OnRep_Parts, nullptr);

		return Pawn;
	}
	
	UObject* GetCurrentWeapon(UObject* Pawn)
	{
		static auto CurrentWeaponOffset = GetOffset(Pawn, "CurrentWeapon");

		auto currentWeapon = (UObject**)(__int64(Pawn) + CurrentWeaponOffset);

		return currentWeapon ? *currentWeapon : nullptr;
	}

	UObject* GetWeaponData(UObject* CurrentWeapon)
	{
		if (!CurrentWeapon)
			return nullptr;

		static auto WeaponDataOffset = GetOffset(CurrentWeapon, "WeaponData");

		return *(UObject**)(__int64(CurrentWeapon) + WeaponDataOffset);
	}

	UObject* SoftObjectToObject(TSoftObjectPtr SoftObject)
	{
		return FindObject(SoftObject.ObjectID.AssetPathName.ToString());
	}

	UObject* GetPlaylist(UObject*** outAddr = nullptr)
	{
		if (bIsSTW)
			return nullptr;

		auto world = Helper::GetWorld();
		auto gameState = Helper::GetGameState();

		if (FnVerDouble >= 6.10) // WRONG
		{
			static auto BasePlaylistOffset = FindOffsetStruct(("ScriptStruct /Script/FortniteGame.PlaylistPropertyArray"), ("BasePlaylist"));
			if (BasePlaylistOffset)
			{
				static auto CurrentPlaylistInfoOffset = GetOffset(gameState, "CurrentPlaylistInfo");
				auto PlaylistInfo = (void*)(__int64(gameState) + CurrentPlaylistInfoOffset); // gameState->Member<void>(("CurrentPlaylistInfo"));

				auto BasePlaylist = (UObject**)(__int64(PlaylistInfo) + BasePlaylistOffset);// *gameState->Member<UObject>(("CurrentPlaylistInfo"))->Member<UObject*>(("BasePlaylist"), true);

				if (outAddr)
					*outAddr = BasePlaylist;
				
				return BasePlaylist ? *BasePlaylist : nullptr;
			}
		}
		else
		{
			static auto CurrentPlaylistDataOffset = GetOffset(gameState, "CurrentPlaylistData");
			auto PlaylistData = (UObject**)(__int64(gameState) + CurrentPlaylistDataOffset);

			if (outAddr)
				*outAddr = PlaylistData;

			return *PlaylistData;
		}

		return nullptr;
	}

	void SilentDie(UObject* BuildingActor)
	{
		static auto fn = BuildingActor->Function(("SilentDie"));
		
		bool bPropagateSilentDeath = true; // yes, this isn't on like below s16 or something, but passing in more args wont hurt.

		if (fn)
			BuildingActor->ProcessEvent(fn, &bPropagateSilentDeath);
	}

	void DumpDeathCauses()
	{
		auto MappingsSoft = Helper::GetGameData()->Member<TSoftObjectPtr>("FortDeathCauseFromTagMapping");

		auto Mappings = StaticLoadObject(FindObject("Class /Script/FortniteGame.FortDeathCauseFromTagMapping"), nullptr, MappingsSoft->ObjectID.AssetPathName.ToString());

		struct FFortTagToDeathCause
		{
			FGameplayTag                                DeathTag;                                                 // 0x0000(0x0008) (Edit, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			EDeathCause                                        DBNOCause;                                                // 0x0008(0x0001) (Edit, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			EDeathCause                                        DeathCause;                                               // 0x0009(0x0001) (Edit, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
			unsigned char                                      UnknownData00[0x2];                                       // 0x000A(0x0002) MISSED OFFSET
		};

		auto TagToDeathCauseArray = Mappings->Member<TArray<FFortTagToDeathCause>>("TagToDeathCauseArray");

		if (TagToDeathCauseArray)
		{
			std::cout << "Num: " << TagToDeathCauseArray->Num() << '\n';

			for (int i = 0; i < TagToDeathCauseArray->Num(); i++)
			{
				auto& DeathCauseTag = TagToDeathCauseArray->At(i);

				WriteToFile(std::format("[{}] {} {}\n", DeathCauseTag.DeathTag.TagName.ToString(), std::to_string((uint8_t)DeathCauseTag.DeathCause), std::to_string((uint8_t)DeathCauseTag.DBNOCause)));
			}
		}
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

	bool SetActorLocationAndRotation(UObject* Actor, const FVector& Location, const FRotator& Rotation)
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
	}

	void KickController(UObject* Controller, FString Reason)
	{
		if (false && KickPlayer && Controller && Reason.Data.GetData())
		{
			// FText text = Conversion::StringToText(Reason);
			auto World = Helper::GetWorld();
			auto GameMode = Helper::GetGameMode();
			auto GameSession = *World->Member<UObject*>(("GameSession"));

			if (GameSession)
			{
				std::cout << "EH!\n";

				static auto ServerReturnToMainMenu = Controller->Function("ServerReturnToMainMenu");

				Controller->ProcessEvent(ServerReturnToMainMenu);

				/*
				static auto ClientReturnToMainMenu = Controller->Function("ClientReturnToMainMenu");

				if (ClientReturnToMainMenu)
					Controller->ProcessEvent(ClientReturnToMainMenu, &Reason);
				*/
			}
			else
				std::cout << ("Unable to find GameSession!\n");
		}
		else
		{
			// std::cout << std::format(("Something is invalid cannot kick player\n\nKickPlayer: {}\nController: {}\nIf none of these are null then it's data!\n\n"), __int64(KickPlayer), Controller); // , Reason.Data.GetData());
		}
	}

	/* static bool IsLowGravity()
	{
		auto Playlist = GetPlaylist();
		bool LowGravity = Playlist ? Playlist->GetFullName().contains("FortPlaylistAthena /Game/Athena/Playlists/Low/Playlist_Low_Squads.Playlist_Low_") : false;

		return LowGravity;
	} */

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
			Reason.Set(L"You have beeb banned!");

			static auto ClientReturnToMainMenu = Controller->Function("ClientReturnToMainMenu");

			if (ClientReturnToMainMenu)
				Controller->ProcessEvent(ClientReturnToMainMenu, &Reason);

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

	float GetRespawnHeight()
	{
		auto Playlist = GetPlaylist();

		if (Playlist)
			return Playlist->Member<FScalableFloat>("RespawnHeight")->Value; // improper cuz yeah but yeah

		return 10000.f;
	}
}
