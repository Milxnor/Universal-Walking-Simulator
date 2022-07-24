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

		static auto setHealthFn = Pawn->Function(_("SetHealth"));
		struct { float NewHealthVal; }healthParams{ 100 };

		if (setHealthFn)
			Pawn->ProcessEvent(setHealthFn, &healthParams);
		static auto PickaxeDefinition = FindObject(_("FortWeaponMeleeItemDefinition /Game/Athena/Items/Weapons/WID_Harvest_Pickaxe_Athena_C_T01.WID_Harvest_Pickaxe_Athena_C_T01"));

		// TODO: StructProperty /Script/FortniteGame.FortPlaylistAthena.RespawnHeight
		struct { float HeightAboveGround; }TeleportToSkyDiveParams{ 10000 };

		auto NewPawn = Helper::InitPawn(PlayerController, false, PawnLocation);
		// static auto TeleportToSkyDiveFn = NewPawn->Function(_("TeleportToSkyDive"));
		// NewPawn->ProcessEvent(TeleportToSkyDiveFn, &TeleportToSkyDiveParams);
		// PlayerController->ProcessEvent(_("RespawnPlayer"));
	}
}