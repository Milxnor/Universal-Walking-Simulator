#pragma once

#include <Gameplay/helper.h>

namespace Player
{
	void RespawnPlayer(UObject* PlayerController)
	{
		auto Pawn = *PlayerController->Member<UObject*>("Pawn");
		auto PawnLocation = Helper::GetActorLocation(Pawn);

		/* if (Pawn)
		{
			Helper::DestroyActor(Pawn);
		} */

		static auto setHealthFn = Pawn->Function(("SetHealth"));
		struct { float NewHealthVal; }healthParams{ 100 };

		if (setHealthFn)
			Pawn->ProcessEvent(setHealthFn, &healthParams);

		static auto PickaxeDefinition = FindObject(("FortWeaponMeleeItemDefinition /Game/Athena/Items/Weapons/WID_Harvest_Pickaxe_Athena_C_T01.WID_Harvest_Pickaxe_Athena_C_T01"));

		// TODO: StructProperty /Script/FortniteGame.FortPlaylistAthena.RespawnHeight
		struct { float HeightAboveGround; }TeleportToSkyDiveParams{ 10000 };

		auto NewPawn = Helper::InitPawn(PlayerController, false, PawnLocation);

		if (NewPawn)
		{
			static auto TeleportToSkyDiveFn = NewPawn->Function(("TeleportToSkyDive"));

			if (TeleportToSkyDiveFn)
				NewPawn->ProcessEvent(TeleportToSkyDiveFn, &TeleportToSkyDiveParams);
		}

		*NewPawn->Member<bool>("bIsDBNO") = false;

		static auto OnRep_DBNOFn = NewPawn->Function("OnRep_IsDBNO");

		if (OnRep_DBNOFn)
			NewPawn->ProcessEvent(OnRep_DBNOFn);

		// PlayerController->ProcessEvent(("RespawnPlayer"));
	}
}
