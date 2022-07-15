#pragma once

#include <UE/structs.h>

namespace Helper
{
	UObject* GetWorld()
	{
		auto GameViewport = *GetEngine()->Member<UObject*>(_("GameViewport"));

		if (GameViewport)
			return *GameViewport->Member<UObject*>(_("World")); // we could also find the world by name but that depends on the map

		return nullptr;
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
}

namespace Easy
{
	UObject* SpawnActor(UObject* Class, const FVector& Location = FVector(), const FRotator& Rotation = FRotator())
	{
		auto Loc = Location;
		auto Rot = Rotation;
		return SpawnActorO(Helper::GetWorld(), Class, &Loc, &Rot, FActorSpawnParameters());
	}
}