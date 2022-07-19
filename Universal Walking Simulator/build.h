#pragma once

#include <Net/funcs.h>

// Includes building and editing..

inline bool ServerCreateBuildingActorHook(UObject* Controller, UFunction* Function, void* Parameters)
{
	if (Controller && Parameters)
	{

	}

	return false;
}

void InitializeBuildHooks()
{
	AddHook(_("Function /Script/FortniteGame.FortPlayerController.ServerCreateBuildingActor"), ServerCreateBuildingActorHook);
}