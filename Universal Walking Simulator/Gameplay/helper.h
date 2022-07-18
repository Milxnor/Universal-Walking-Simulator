#pragma once

#include <UE/structs.h>

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

	FVector GetPlayerStart()
	{
		static auto WarmupClass = FindObject(_("Class /Script/FortniteGame.FortPlayerStartWarmup"));
		TArray<UObject*> OutActors = GetAllActorsOfClass(WarmupClass);

		auto ActorsNum = OutActors.Num();

		auto SpawnTransform = FTransform();
		SpawnTransform.Scale3D = FVector(1, 1, 1);
		SpawnTransform.Rotation = FQuat();
		SpawnTransform.Translation = FVector{ 1250, 1818, 3284 }; // Next to salty

		// auto GamePhase = static_cast<AAthena_GameState_C*>(GetWorld()->GameState)->GamePhase;

		if (ActorsNum != 0) // && (GamePhase == EAthenaGamePhase::Setup || GamePhase == EAthenaGamePhase::Warmup))
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

	namespace Console
	{
		static UObject** ViewportConsole = nullptr;

		DWORD WINAPI Setup(LPVOID)
		{
			static auto Engine = FindObject(_("FortEngine_"));

			while (!Engine)
			{
				Engine = FindObject(_("FortEngine_"));
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
				class UObject* WorldContextObject;                                       // (Parm, ZeroConstructor, IsPlainOldData)
				struct FString                                     Command;                                                  // (Parm, ZeroConstructor)
				class APlayerController* SpecificPlayer;                                           // (Parm, ZeroConstructor, IsPlainOldData)
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

	UObject* InitPawn(UObject* PC, FVector Location = Helper::GetPlayerStart())
	{
		static const auto FnVerDouble = std::stod(FN_Version);

		UObject* PlayerState = *PC->Member<UObject*>(_("PlayerState"));

		static auto PawnClass = FindObject(_("BlueprintGeneratedClass /Game/Athena/PlayerPawn_Athena.PlayerPawn_Athena_C"));
		auto Pawn = Easy::SpawnActor(PawnClass, Location, {});

		static auto SetReplicateMovementFn = Pawn->Function(_("SetReplicateMovement"));
		struct {
			bool b;
		} bruh{true};
		Pawn->ProcessEvent(SetReplicateMovementFn, &bruh);
		auto Offset = GetOffset(Pawn, _("bReplicateMovement"));

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
			Pawn->ProcessEvent(setMaxShieldFn, &healthParams);
		else
			std::cout << _("Unable to find setMaxShieldFn!\n");
		static const auto HeroType = FindObject(_("FortHeroType /Game/Athena/Heroes/HID_058_Athena_Commando_M_SkiDude_GER.HID_058_Athena_Commando_M_SkiDude_GER"));

		*PlayerState->Member<UObject*>(_("HeroType")) = HeroType;
		static auto OnRepHeroType = PlayerState->Function(_("OnRep_HeroType"));
		PlayerState->ProcessEvent(OnRepHeroType);

		static auto headPart = FindObject(_("CustomCharacterPart /Game/Characters/CharacterParts/Female/Medium/Heads/F_Med_Head1.F_Med_Head1"));
		static auto bodyPart = FindObject(_("CustomCharacterPart /Game/Characters/CharacterParts/Female/Medium/Bodies/F_Med_Soldier_01.F_Med_Soldier_01"));

		if (headPart && bodyPart)
		{
			Helper::ChoosePart(Pawn, EFortCustomPartType::Head, headPart);
			Helper::ChoosePart(Pawn, EFortCustomPartType::Body, bodyPart);
			static auto OnRep_Parts = (FnVerDouble >= 10) ? PlayerState->Function(_("OnRep_CharacterData")) : PlayerState->Function(_("OnRep_CharacterParts")); //Make sure its s10 and up

			if (OnRep_Parts)
				PlayerState->ProcessEvent(OnRep_Parts, nullptr);
		}

		if (Engine_Version < 423)
		{
			*PlayerState->Member<uint8_t>(_("TeamIndex")) = 11;
			*PlayerState->Member<unsigned char>(_("SquadId")) = 1;
		}

		return Pawn;
	}

	namespace Abilities
	{
		void ClientActivateAbilityFailed(UObject* Component, FGameplayAbilitySpecHandle AbilityToActivate, int16_t PredictionKey)
		{
			struct
			{
				FGameplayAbilitySpecHandle                  AbilityToActivate;                                        // (Parm)
				int16_t                                            PredictionKey;                                            // (Parm, ZeroConstructor, IsPlainOldData)
			} UAbilitySystemComponent_ClientActivateAbilityFailed_Params{AbilityToActivate, PredictionKey};

			if (Component)
			{
				static auto fn = Component->Function(_("ClientActivateAbilityFailed"));

				if (fn)
					Component->ProcessEvent(fn, &UAbilitySystemComponent_ClientActivateAbilityFailed_Params);
				else
					std::cout << _("Could not find ClientActivateAbilityFailed!\n");
			}
			else
				std::cout << _("Invalid component!\n");
		}
		
		FGameplayAbilitySpecHandle GrantAbility(UObject* ASC, FGameplayAbilitySpecHandle* outHandle, FGameplayAbilitySpec& inSpec) // 
		{
			auto ActivatableAbilities = *ASC->Member<FGameplayAbilitySpecContainer>(_("ActivatableAbilities"));

			FGameplayAbilitySpec* OwnedSpec = &ActivatableAbilities.Items[ActivatableAbilities.Items.Add(inSpec)];

			// if (OwnedSpec.Ability && (*OwnedSpec.Ability->Member<EGameplayAbilityInstancingPolicy>(_("InstancingPolicy")) == EGameplayAbilityInstancingPolicy::InstancedPerActor))
			{
				// std::cout << _("Instancing Policy is InstancedPerActor!\n");
				// CreateNewInstanceOfAbility(OwnedSpec, Spec.Ability);
			}

			/* if (*(char*)(*(__int64*)(OwnedSpec + 16) + 207) == 1)
			{
				std::cout << _("Calling CreateNewInstanceOfAbility!\n");
				(*(void(__fastcall**)(UObject*, __int64, FGameplayAbilitySpec))(*(__int64*)ASC + 1944))(ASC, __int64(OwnedSpec), (&inSpec)[2]);
				std::cout << _("Called CreateNewInstanceOfAbility!\n");
			} */

			std::cout << _("Calling OnGiveAbility!\n");
			(*(void(__fastcall**)(UObject*, FGameplayAbilitySpec*))(*(__int64*)ASC + 1920))(ASC, OwnedSpec); // OnGiveAbility
			std::cout << _("Called OnGiveAbility!\n");
			MarkAbilitySpecDirtyNew(ASC, *OwnedSpec, true);
			std::cout << _("Called MarkAbilitySpecDirty!\n");
			return OwnedSpec->Handle;
		}
	}
}