#pragma once

#include <Gameplay/helper.h>

namespace Player
{
	void RespawnPlayer(UObject* PlayerController)
	{
		auto Pawn = Helper::GetPawnFromController(PlayerController);

		if (!Pawn)
			return;

		auto PawnLocation = Helper::GetActorLocation(Pawn);

		// Helper::DestroyActor(Pawn);

		static auto PickaxeDefinition = FindObject(("FortWeaponMeleeItemDefinition /Game/Athena/Items/Weapons/WID_Harvest_Pickaxe_Athena_C_T01.WID_Harvest_Pickaxe_Athena_C_T01"));

		// TODO: StructProperty /Script/FortniteGame.FortPlaylistAthena.RespawnHeight
		struct { float HeightAboveGround; }TeleportToSkyDiveParams{ 11000 };

		auto NewPawn = Helper::InitPawn(PlayerController, false, PawnLocation);

		if (!NewPawn)
			return;

		static auto setHealthFn = NewPawn->Function(("SetHealth"));
		struct { float NewHealthVal; }healthParams{ 100 };

		if (setHealthFn)
			NewPawn->ProcessEvent(setHealthFn, &healthParams);

		Helper::SetShield(NewPawn, 100);

		if (Engine_Version >= 421)
		{
			static auto TeleportToSkyDiveFn = NewPawn->Function(("TeleportToSkyDive"));

			if (TeleportToSkyDiveFn)
				NewPawn->ProcessEvent(TeleportToSkyDiveFn, &TeleportToSkyDiveParams);
		}
		else
		{
			// PlayerController->ProcessEvent("RespawnPlayerAfterDeath");
		}

		setBitfield(NewPawn, "bIsDBNO", false);

		// PlayerController->ProcessEvent(("RespawnPlayer"));
	}
}