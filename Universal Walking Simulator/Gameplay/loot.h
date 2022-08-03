#pragma once
#include <vector>
#include <random>

#include <Gameplay/helper.h>

namespace Looting
{
	namespace Tables
	{
		std::vector<std::vector<UObject*>> WeaponTable;
		std::vector<UObject*> ConsumableTable;
		std::vector<UObject*> AmmoTable;
		static bool bInitialized = false;

		static int GetWeaponTableIdx()
		{
			// int Random = UKismetMathLibrary::RandomIntegerInRange(0, 100);
			std::random_device rd; // obtain a random number from hardware
			std::mt19937 gen(rd()); // seed the generator
			std::uniform_int_distribution<> distr(0, 100); // define the range

			auto Random = distr(gen);

			if (Random <= 50)
				return 0;

			if (Random <= 75)
				return 1;

			if (Random <= 90)
				return 2;

			if (Random <= 97)
				return 3;

			if (Random <= 100)
				return 4;

			return 0;
		}

		static UObject* GetWeaponDef()
		{
			auto& Table = WeaponTable[GetWeaponTableIdx()];

			while (true)
			{
				auto Weapon = Table[rand() % (Table.size())];

				if (Weapon)
					return Weapon;
			}
		}

		static UObject* GetAmmoDef()
		{
			while (true)
			{
				auto Ammo = AmmoTable[rand() % (AmmoTable.size())];

				if (Ammo)
					return Ammo;
			}
		}

		static UObject* GetConsumableDef()
		{
			while (true)
			{
				auto Consumable = ConsumableTable[rand() % (ConsumableTable.size())];

				if (Consumable)
					return Consumable;
			}
		}

		static DWORD WINAPI Init(LPVOID)
		{
			if (bInitialized)
				return 0;

			bInitialized = true;

			for (int i = 0; i < 5; i++) // 0 = gray, 1 = green, 2 = blue, 3 = purple, 4 = gold
			{
				WeaponTable.push_back(std::vector<UObject*>());
			}

			// WeaponTable.reserve(5);

			WeaponTable[0].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Assault_Auto_Athena_C_Ore_T02.WID_Assault_Auto_Athena_C_Ore_T02")));
			WeaponTable[0].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Assault_SemiAuto_Athena_C_Ore_T02.WID_Assault_SemiAuto_Athena_C_Ore_T02")));
			WeaponTable[0].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Shotgun_SemiAuto_Athena_UC_Ore_T03.WID_Shotgun_SemiAuto_Athena_UC_Ore_T03")));
			WeaponTable[0].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Pistol_AutoHeavyPDW_Athena_C_Ore_T03.WID_Pistol_AutoHeavyPDW_Athena_C_Ore_T03")));
			WeaponTable[0].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Pistol_AutoHeavySuppressed_Athena_C_Ore_T02.WID_Pistol_AutoHeavySuppressed_Athena_C_Ore_T02")));
			WeaponTable[0].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Pistol_SemiAuto_Athena_C_Ore_T02.WID_Pistol_SemiAuto_Athena_C_Ore_T02")));
			WeaponTable[0].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Pistol_SixShooter_Athena_C_Ore_T02.WID_Pistol_SixShooter_Athena_C_Ore_T02")));
			WeaponTable[0].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Pistol_AutoHeavy_Athena_C_Ore_T02.WID_Pistol_AutoHeavy_Athena_C_Ore_T02")));
			WeaponTable[0].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Pistol_Flintlock_Athena_C.WID_Pistol_Flintlock_Athena_C")));

			WeaponTable[1].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Assault_Auto_Athena_UC_Ore_T03.WID_Assault_Auto_Athena_UC_Ore_T03")));
			WeaponTable[1].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Assault_SemiAuto_Athena_UC_Ore_T03.WID_Assault_SemiAuto_Athena_UC_Ore_T03")));
			WeaponTable[1].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Shotgun_Standard_Athena_C_Ore_T03.WID_Shotgun_Standard_Athena_C_Ore_T03")));
			WeaponTable[1].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Shotgun_SemiAuto_Athena_R_Ore_T03.WID_Shotgun_SemiAuto_Athena_R_Ore_T03")));
			WeaponTable[1].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Sniper_Auto_Suppressed_Scope_Athena_UC.WID_Sniper_Auto_Suppressed_Scope_Athena_UC")));
			WeaponTable[1].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Sniper_Standard_Scope_Athena_VR_Ore_T03.WID_Sniper_Standard_Scope_Athena_VR_Ore_T03")));
			WeaponTable[1].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Pistol_AutoHeavyPDW_Athena_UC_Ore_T03.WID_Pistol_AutoHeavyPDW_Athena_UC_Ore_T03")));
			WeaponTable[1].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Pistol_AutoHeavySuppressed_Athena_UC_Ore_T03.WID_Pistol_AutoHeavySuppressed_Athena_UC_Ore_T03")));
			WeaponTable[1].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Pistol_Scavenger_Athena_UC_Ore_T03.WID_Pistol_Scavenger_Athena_UC_Ore_T03")));
			WeaponTable[1].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Assault_AutoDrum_Athena_UC_Ore_T03.WID_Assault_AutoDrum_Athena_UC_Ore_T03")));
			WeaponTable[1].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Pistol_SemiAuto_Athena_UC_Ore_T03.WID_Pistol_SemiAuto_Athena_UC_Ore_T03")));
			WeaponTable[1].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_DualPistol_Suppresseed_Athena_UC_T01.WID_DualPistol_Suppresseed_Athena_UC_T01")));
			WeaponTable[1].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Pistol_SixShooter_Athena_UC_Ore_T03.WID_Pistol_SixShooter_Athena_UC_Ore_T03")));
			WeaponTable[1].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Pistol_Flintlock_Athena_UC.WID_Pistol_Flintlock_Athena_UC")));
			WeaponTable[1].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Pistol_Revolver_SingleAction_Athena_UC.WID_Pistol_Revolver_SingleAction_Athena_UC")));

			WeaponTable[2].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Assault_Auto_Athena_R_Ore_T03.WID_Assault_Auto_Athena_R_Ore_T03")));
			WeaponTable[2].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Assault_SemiAuto_Athena_R_Ore_T03.WID_Assault_SemiAuto_Athena_R_Ore_T03")));
			WeaponTable[2].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Assault_Heavy_Athena_R_Ore_T03.WID_Assault_Heavy_Athena_R_Ore_T03")));
			WeaponTable[2].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Assault_PistolCaliber_AR_Athena_R_Ore_T03.WID_Assault_PistolCaliber_AR_Athena_R_Ore_T03")));
			WeaponTable[2].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Shotgun_Standard_Athena_UC_Ore_T03.WID_Shotgun_Standard_Athena_UC_Ore_T03")));
			WeaponTable[2].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Shotgun_SemiAuto_Athena_VR_Ore_T03.WID_Shotgun_SemiAuto_Athena_VR_Ore_T03")));
			WeaponTable[2].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Sniper_BoltAction_Scope_Athena_R_Ore_T03.WID_Sniper_BoltAction_Scope_Athena_R_Ore_T03")));
			WeaponTable[2].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Sniper_Auto_Suppressed_Scope_Athena_R.WID_Sniper_Auto_Suppressed_Scope_Athena_R")));
			WeaponTable[2].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Sniper_Standard_Scope_Athena_SR_Ore_T03.WID_Sniper_Standard_Scope_Athena_SR_Ore_T03")));
			WeaponTable[2].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Pistol_AutoHeavyPDW_Athena_R_Ore_T03.WID_Pistol_AutoHeavyPDW_Athena_R_Ore_T03")));
			WeaponTable[2].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Pistol_AutoHeavySuppressed_Athena_R_Ore_T03.WID_Pistol_AutoHeavySuppressed_Athena_R_Ore_T03")));
			WeaponTable[2].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Pistol_Scavenger_Athena_R_Ore_T03.WID_Pistol_Scavenger_Athena_R_Ore_T03")));
			WeaponTable[2].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Pistol_BurstFireSMG_Athena_R_Ore_T03.WID_Pistol_BurstFireSMG_Athena_R_Ore_T03")));
			WeaponTable[2].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Assault_AutoDrum_Athena_R_Ore_T03.WID_Assault_AutoDrum_Athena_R_Ore_T03")));
			WeaponTable[2].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Pistol_SemiAuto_Athena_R_Ore_T03.WID_Pistol_SemiAuto_Athena_R_Ore_T03")));
			WeaponTable[2].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_DualPistol_SemiAuto_Athena_VR_Ore_T03.WID_DualPistol_SemiAuto_Athena_VR_Ore_T03")));
			WeaponTable[2].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Pistol_SixShooter_Athena_R_Ore_T03.WID_Pistol_SixShooter_Athena_R_Ore_T03")));
			WeaponTable[2].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Pistol_Revolver_SingleAction_Athena_R.WID_Pistol_Revolver_SingleAction_Athena_R")));
			WeaponTable[2].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Launcher_Grenade_Athena_R_Ore_T03.WID_Launcher_Grenade_Athena_R_Ore_T03")));

			WeaponTable[3].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Assault_AutoHigh_Athena_VR_Ore_T03.WID_Assault_AutoHigh_Athena_VR_Ore_T03")));
			WeaponTable[3].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Assault_SemiAuto_Athena_VR_Ore_T03.WID_Assault_SemiAuto_Athena_VR_Ore_T03")));
			WeaponTable[3].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Assault_Suppressed_Athena_VR_Ore_T03.WID_Assault_Suppressed_Athena_VR_Ore_T03")));
			WeaponTable[3].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Assault_Surgical_Thermal_Athena_VR_Ore_T03.WID_Assault_Surgical_Thermal_Athena_VR_Ore_T03")));
			WeaponTable[3].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Assault_Heavy_Athena_VR_Ore_T03.WID_Assault_Heavy_Athena_VR_Ore_T03")));
			WeaponTable[3].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Assault_PistolCaliber_AR_Athena_VR_Ore_T03.WID_Assault_PistolCaliber_AR_Athena_VR_Ore_T03")));
			WeaponTable[3].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Shotgun_Standard_Athena_VR_Ore_T03.WID_Shotgun_Standard_Athena_VR_Ore_T03")));
			WeaponTable[3].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Shotgun_HighSemiAuto_Athena_VR_Ore_T03.WID_Shotgun_HighSemiAuto_Athena_VR_Ore_T03")));
			WeaponTable[3].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Shotgun_SlugFire_Athena_VR.WID_Shotgun_SlugFire_Athena_VR")));
			WeaponTable[3].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Shotgun_Combat_Athena_VR_Ore_T03.WID_Shotgun_Combat_Athena_VR_Ore_T03")));
			WeaponTable[3].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Shotgun_BreakBarrel_Athena_VR_Ore_T03.WID_Shotgun_BreakBarrel_Athena_VR_Ore_T03")));
			WeaponTable[3].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Sniper_BoltAction_Scope_Athena_VR_Ore_T03.WID_Sniper_BoltAction_Scope_Athena_VR_Ore_T03")));
			WeaponTable[3].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Sniper_Suppressed_Scope_Athena_VR_Ore_T03.WID_Sniper_Suppressed_Scope_Athena_VR_Ore_T03")));
			WeaponTable[3].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Sniper_Auto_Suppressed_Scope_Athena_VR.WID_Sniper_Auto_Suppressed_Scope_Athena_VR")));
			WeaponTable[3].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Pistol_AutoHeavyPDW_Athena_VR_Ore_T03.WID_Pistol_AutoHeavyPDW_Athena_VR_Ore_T03")));
			WeaponTable[3].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Pistol_Scavenger_Athena_VR_Ore_T03.WID_Pistol_Scavenger_Athena_VR_Ore_T03")));
			WeaponTable[3].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Pistol_AutoHeavyPDW_Athena_VR_Ore_T03.WID_Pistol_AutoHeavyPDW_Athena_VR_Ore_T03")));
			WeaponTable[3].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Pistol_Standard_Athena_VR.WID_Pistol_Standard_Athena_VR")));
			WeaponTable[3].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Pistol_SemiAuto_Athena_SR_Ore_T03.WID_Pistol_SemiAuto_Athena_SR_Ore_T03")));
			WeaponTable[3].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_DualPistol_SemiAuto_Athena_SR_Ore_T03.WID_DualPistol_SemiAuto_Athena_SR_Ore_T03")));
			WeaponTable[3].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_DualPistol_Suppresseed_Athena_VR_T01.WID_DualPistol_Suppresseed_Athena_VR_T01")));
			WeaponTable[3].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Pistol_Scoped_Athena_VR_Ore_T03.WID_Pistol_Scoped_Athena_VR_Ore_T03")));
			WeaponTable[3].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Pistol_HandCannon_Athena_VR_Ore_T03.WID_Pistol_HandCannon_Athena_VR_Ore_T03")));
			WeaponTable[3].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Pistol_Revolver_SingleAction_Athena_VR.WID_Pistol_Revolver_SingleAction_Athena_VR")));
			WeaponTable[3].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Launcher_Rocket_Athena_VR_Ore_T03.WID_Launcher_Rocket_Athena_VR_Ore_T03")));
			WeaponTable[3].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Launcher_Grenade_Athena_VR_Ore_T03.WID_Launcher_Grenade_Athena_VR_Ore_T03")));
			WeaponTable[3].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Launcher_Military_Athena_VR_Ore_T03.WID_Launcher_Military_Athena_VR_Ore_T03")));

			WeaponTable[4].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Assault_AutoHigh_Athena_SR_Ore_T03.WID_Assault_AutoHigh_Athena_SR_Ore_T03")));
			WeaponTable[4].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Assault_SemiAuto_Athena_SR_Ore_T03.WID_Assault_SemiAuto_Athena_SR_Ore_T03")));
			WeaponTable[4].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Assault_Suppressed_Athena_SR_Ore_T03.WID_Assault_Suppressed_Athena_SR_Ore_T03")));
			WeaponTable[4].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Assault_Surgical_Thermal_Athena_SR_Ore_T03.WID_Assault_Surgical_Thermal_Athena_SR_Ore_T03")));
			WeaponTable[4].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Assault_Heavy_Athena_SR_Ore_T03.WID_Assault_Heavy_Athena_SR_Ore_T03")));
			WeaponTable[4].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Assault_PistolCaliber_AR_Athena_SR_Ore_T03.WID_Assault_PistolCaliber_AR_Athena_SR_Ore_T03")));
			WeaponTable[4].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Shotgun_Standard_Athena_SR_Ore_T03.WID_Shotgun_Standard_Athena_SR_Ore_T03")));
			WeaponTable[4].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Shotgun_HighSemiAuto_Athena_SR_Ore_T03.WID_Shotgun_HighSemiAuto_Athena_SR_Ore_T03")));
			WeaponTable[4].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Shotgun_SlugFire_Athena_SR.WID_Shotgun_SlugFire_Athena_SR")));
			WeaponTable[4].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Shotgun_BreakBarrel_Athena_SR_Ore_T03.WID_Shotgun_BreakBarrel_Athena_SR_Ore_T03")));
			WeaponTable[4].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Sniper_BoltAction_Scope_Athena_SR_Ore_T03.WID_Sniper_BoltAction_Scope_Athena_SR_Ore_T03")));
			WeaponTable[4].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Sniper_Heavy_Athena_SR_Ore_T03.WID_Sniper_Heavy_Athena_SR_Ore_T03")));
			WeaponTable[4].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Sniper_Suppressed_Scope_Athena_SR_Ore_T03.WID_Sniper_Suppressed_Scope_Athena_SR_Ore_T03")));
			WeaponTable[4].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Sniper_Auto_Suppressed_Scope_Athena_SR.WID_Sniper_Auto_Suppressed_Scope_Athena_SR")));
			WeaponTable[4].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Pistol_AutoHeavyPDW_Athena_SR_Ore_T03.WID_Pistol_AutoHeavyPDW_Athena_SR_Ore_T03")));
			WeaponTable[4].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Assault_LMG_Athena_SR_Ore_T03.WID_Assault_LMG_Athena_SR_Ore_T03")));
			WeaponTable[4].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Pistol_Standard_Athena_SR.WID_Pistol_Standard_Athena_SR")));
			WeaponTable[4].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Pistol_HandCannon_Athena_SR_Ore_T03.WID_Pistol_HandCannon_Athena_SR_Ore_T03")));
			WeaponTable[4].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Launcher_Rocket_Athena_SR_Ore_T03.WID_Launcher_Rocket_Athena_SR_Ore_T03")));
			WeaponTable[4].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Launcher_Grenade_Athena_SR_Ore_T03.WID_Launcher_Grenade_Athena_SR_Ore_T03")));
			WeaponTable[4].push_back(FindObject(_("/Game/Athena/Items/Weapons/WID_Launcher_Military_Athena_SR_Ore_T03.WID_Launcher_Military_Athena_SR_Ore_T03")));

			ConsumableTable.push_back(FindObject(_("/Game/Athena/Items/Consumables/Bandage/Athena_Bandage.Athena_Bandage")));
			ConsumableTable.push_back(FindObject(_("/Game/Athena/Items/Consumables/Medkit/Athena_Medkit.Athena_Medkit")));
			ConsumableTable.push_back(FindObject(_("/Game/Athena/Items/Consumables/Shields/Athena_Shields.Athena_Shields")));
			ConsumableTable.push_back(FindObject(_("/Game/Athena/Items/Consumables/ShieldSmall/Athena_ShieldSmall.Athena_ShieldSmall")));
			ConsumableTable.push_back(FindObject(_("/Game/Athena/Items/Consumables/PurpleStuff/Athena_PurpleStuff.Athena_PurpleStuff")));
			ConsumableTable.push_back(FindObject(_("/Game/Athena/Items/Consumables/SuperMedKit/Athena_SuperMedkit.Athena_SuperMedkit")));
			ConsumableTable.push_back(FindObject(_("/Game/Athena/Items/Consumables/Grenade/Athena_Grenade.Athena_Grenade")));
			ConsumableTable.push_back(FindObject(_("/Game/Athena/Items/Consumables/KnockGrenade/Athena_KnockGrenade.Athena_KnockGrenade")));
			ConsumableTable.push_back(FindObject(_("/Game/Athena/Items/Consumables/ShockwaveGrenade/Athena_ShockGrenade.Athena_ShockGrenade")));
			ConsumableTable.push_back(FindObject(_("/Game/Athena/Items/Consumables/StickyGrenade/Athena_StickyGrenade.Athena_StickyGrenade")));
			ConsumableTable.push_back(FindObject(_("/Game/Athena/Items/Consumables/ThrownConsumables/Athena_IceGrenade.Athena_IceGrenade")));
			ConsumableTable.push_back(FindObject(_("/Game/Athena/Items/Consumables/Bush/Athena_Bush.Athena_Bush")));

			if (Engine_Version < 422)
			{
				AmmoTable.push_back(FindObject(_("/Game/Athena/Items/Ammo/AthenaAmmoDataRockets.AthenaAmmoDataRockets")));
				AmmoTable.push_back(FindObject(_("/Game/Athena/Items/Ammo/AthenaAmmoDataShells.AthenaAmmoDataShells")));
				AmmoTable.push_back(FindObject(_("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsMedium.AthenaAmmoDataBulletsMedium")));
				AmmoTable.push_back(FindObject(_("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsLight.AthenaAmmoDataBulletsLight")));
				AmmoTable.push_back(FindObject(_("/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsHeavy.AthenaAmmoDataBulletsHeavy")));
			}
			else
			{
				static UObject* AthenaAmmoDataRockets = FindObject(_("FortAmmoItemDefinition /Game/Athena/Items/Ammo/AmmoDataRockets.AmmoDataRockets"));
				static UObject* AthenaAmmoDataShells = FindObject(_("FortAmmoItemDefinition /Game/Athena/Items/Ammo/AthenaAmmoDataShells.AthenaAmmoDataShells"));
				static UObject* AthenaAmmoDataBulletsMedium = FindObject(_("FortAmmoItemDefinition /Game/Athena/Items/Ammo/AthenaAmmoDataBulletsMedium.AthenaAmmoDataBulletsMedium"));
				static UObject* AthenaAmmoDataBulletsLight = FindObject(_("FortAmmoItemDefinition /Game/Athena/Items/Ammo/AthenaAmmoDataBulletsLight.AthenaAmmoDataBulletsLight"));
				static UObject* AthenaAmmoDataBulletsHeavy = FindObject(_("FortAmmoItemDefinition /Game/Athena/Items/Ammo/AthenaAmmoDataBulletsHeavy.AthenaAmmoDataBulletsHeavy"));
			
				AmmoTable.push_back(AthenaAmmoDataRockets);
				AmmoTable.push_back(AthenaAmmoDataShells);
				AmmoTable.push_back(AthenaAmmoDataBulletsMedium);
				AmmoTable.push_back(AthenaAmmoDataBulletsLight);
				AmmoTable.push_back(AthenaAmmoDataBulletsHeavy);
			}

			return 0;
		}

		inline UObject* GetRandomDefinition(bool* bIsConsum = nullptr) // NOT ammo
		{
			auto Random = rand() % 10;
			auto bIsConsumable = Random == 1 || Random == 2 || Random == 3 || Random == 0;

			if (bIsConsum)
				*bIsConsum = bIsConsumable;

			return bIsConsumable ? Tables::GetConsumableDef() : Tables::GetWeaponDef();
		}

		static DWORD WINAPI FillVendingMachines(LPVOID)
		{
			// Some vending machines are empty. We have to get all of the vending machines and then set the slot manually?

			static auto BuildingItemCollectorClass = FindObject(_("Class /Script/FortniteGame.BuildingItemCollectorActor"));

			if (BuildingItemCollectorClass)
			{
				auto Actors = Helper::GetAllActorsOfClass(BuildingItemCollectorClass);

				std::cout << _("Filling ") << Actors.Num() << _(" vending machines!\n");

				for (int i = 0; i < Actors.Num(); i++)
				{
					auto Actor = Actors[i];

					if (Actor)
					{
						struct FCollectorUnitInfo
						{
							class UFortWorldItemDefinition* InputItem;                                                // 0x0000(0x0008) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
							char pad[0x0028];
							UObject* OverrideInputItemTexture;                                 // 0x0028(0x0008) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
							unsigned char                                      bUseDefinedOutputItem : 1;                                // 0x0030(0x0001) (Edit, BlueprintVisible, BlueprintReadOnly, DisableEditOnInstance)
							unsigned char                                      UnknownData00[0x7];                                       // 0x0031(0x0007) MISSED OFFSET
							UObject* OutputItem;                                               // 0x0038(0x0008) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
							TArray<__int64>                      OutputItemEntry;                                          // 0x0040(0x0010) (BlueprintVisible, BlueprintReadOnly, ZeroConstructor, Transient)
							struct FName                                       OverrideOutputItemLootTierGroupName;                      // 0x0050(0x0008) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
							char pad1[0xA8];
							UObject* OverrideOutputItemTexture;                                // 0x0100(0x0008) (Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, DisableEditOnInstance, IsPlainOldData)
						};
						TArray<FCollectorUnitInfo>* ItemCollections = Actor->Member<TArray<FCollectorUnitInfo>>(_("ItemCollections")); // CollectorUnitInfo

						if (ItemCollections)
						{
							static UObject* CollectorUnitInfoClass = FindObject(_("ScriptStruct /Script/FortniteGame.CollectorUnitInfo"));
							static std::string CollectorUnitInfoClassName = _("ScriptStruct /Script/FortniteGame.CollectorUnitInfo");

							if (!CollectorUnitInfoClass)
							{
								CollectorUnitInfoClassName = _("ScriptStruct /Script/FortniteGame.ColletorUnitInfo"); // die fortnite
								CollectorUnitInfoClass = FindObject(CollectorUnitInfoClassName); // Wedc what this value is
							}

							static auto OutputItemOffset = FindOffsetStruct(CollectorUnitInfoClassName, _("OutputItem"));

							std::cout << _("Offset: ") << OutputItemOffset << '\n';
							// ItemCollections->At(i).OutputItem = LootingTables::GetWeaponDef();
							// So this is equal to Array[1] + OutputItemOffset, but since the array is __int64, it doesn't calcuate it properly so we have to implement it ourselves
							std::cout << __int64(&ItemCollections->At(0).OutputItem) << '\n';
							std::cout << __int64(&*(UObject**)(*(__int64*)(__int64(ItemCollections) + (GetSizeOfStruct(CollectorUnitInfoClass) * 0)) + OutputItemOffset)) << '\n';
							*(UObject**)(*(__int64*)((__int64(ItemCollections) + (GetSizeOfStruct(CollectorUnitInfoClass) * 0))) + OutputItemOffset) = Looting::Tables::GetWeaponDef();
							*(UObject**)(*(__int64*)((__int64(ItemCollections) + (GetSizeOfStruct(CollectorUnitInfoClass) * 1))) + OutputItemOffset) = Looting::Tables::GetWeaponDef();
							*(UObject**)(*(__int64*)((__int64(ItemCollections) + (GetSizeOfStruct(CollectorUnitInfoClass) * 2))) + OutputItemOffset) = Looting::Tables::GetWeaponDef();
							/* ItemCollections->At(0).OutputItem = Looting::Tables::GetWeaponDef();
							ItemCollections->At(1).OutputItem = Looting::Tables::GetWeaponDef();
							ItemCollections->At(2).OutputItem = Looting::Tables::GetWeaponDef(); */
						}
						else
							std::cout << _("ItemCollections Invalid: ") << ItemCollections << '\n';
					}
					else
						std::cout << _("Invalid Vending Actor! Index: ") << i << '\n';
				}

				Actors.Free();
			}

			std::cout << _("Finished filling vending machines!\n");

			return 0;
		}

		static DWORD WINAPI SpawnLlamas(LPVOID) // this fuhnction doesn't guarantee 3 llamas
		{
			static auto LlamaClass = FindObject(_("BlueprintGeneratedClass /Game/Athena/SupplyDrops/Llama/AthenaSupplyDrop_Llama.AthenaSupplyDrop_Llama_C"));

			if (LlamaClass)
			{
				for (int i = 0; i < 3; i++)
				{
					FVector RandLocation;

					// CHAPTER 1

					// SKUNKED VERY

					std::random_device rd; // obtain a random number from hardware
					std::mt19937 gen(rd()); // seed the generator

					// CHAPTER 1

					std::uniform_int_distribution<> Xdistr(-40000, 128000);
					std::uniform_int_distribution<> Ydistr(-90000, 70000);
					std::uniform_int_distribution<> Zdistr(-40000, 30000); // doesnt matter because llamas has physics that just teleports them down until it hits collision

					RandLocation.X = Xdistr(gen);
					RandLocation.Y = Ydistr(gen);
					RandLocation.Z = Zdistr(gen);

					std::cout << std::format("Spawning Llama at X: {} Y: {} Z: {}\n", RandLocation.X, RandLocation.Y, RandLocation.Z);

					FRotator RandRotation;
					// RandRotation.Yaw = RandomBetween(0, 360);

					Easy::SpawnActor(LlamaClass, RandLocation, RandRotation);
				}
			}

			return 0;
		}

		static DWORD WINAPI SpawnFloorLoot(LPVOID)
		{
			Init(nullptr);

			static auto FloorLootClass = FindObject(_("BlueprintGeneratedClass /Game/Athena/Environments/Blueprints/Tiered_Athena_FloorLoot_01.Tiered_Athena_FloorLoot_01_C"));

			if (FloorLootClass)
			{
				auto FloorLootActors = Helper::GetAllActorsOfClass(FloorLootClass); // BuildingContainerClass);

				std::cout << _("Spawning ") << FloorLootActors.Num() << _(" floor loot!\n");

				for (int i = 0; i < FloorLootActors.Num(); i++)
				{
					auto Actor = FloorLootActors[i];

					if (Actor)
					{
						bool bIsConsumable = false;
						auto Def = GetRandomDefinition(&bIsConsumable);

						if (Def)
							Helper::SummonPickup(nullptr, Def, Helper::GetActorLocation(Actor), EFortPickupSourceTypeFlag::FloorLoot, EFortPickupSpawnSource::Unset, *Def->Member<int>(_("DropCount")));
					}
				}

				std::cout << _("Spawned ") << FloorLootActors.Num() << _(" floor loot!\n");

				FloorLootActors.Free();
			}
			else
				std::cout << _("[WARNING] Unable to spawn floor loot actors!\n");

			static auto WarmupFloorLootClass = FindObject(_("BlueprintGeneratedClass /Game/Athena/Environments/Blueprints/Tiered_Athena_FloorLoot_Warmup.Tiered_Athena_FloorLoot_Warmup_C"));
			
			if (WarmupFloorLootClass)
			{
				auto WarmupFloorLootActors = Helper::GetAllActorsOfClass(WarmupFloorLootClass); // BuildingContainerClass);

				std::cout << _("Spawning ") << WarmupFloorLootActors.Num() << _(" warmup floor loot!\n");

				for (int i = 0; i < WarmupFloorLootActors.Num(); i++)
				{
					auto WarmupFloorLootActor = WarmupFloorLootActors[i];

					if (WarmupFloorLootActor)
					{
						bool bIsConsumable = false;
						auto Def = GetRandomDefinition(&bIsConsumable);

						if (Def)
						{
							Helper::SummonPickup(nullptr, Def, Helper::GetActorLocation(WarmupFloorLootActor), EFortPickupSourceTypeFlag::FloorLoot, EFortPickupSpawnSource::Unset, *Def->Member<int>(_("DropCount")));

							if (!bIsConsumable)
							{
								// spawn ammo
							}
						}
					}
				}

				std::cout << _("Spawned ") << WarmupFloorLootActors.Num() << _(" floor loot!\n");

				WarmupFloorLootActors.Free();
			}
			else
				std::cout << _("[WARNING] Unable to spawn warump floor loot actors!\n");

			return 0;
		}

		static void HandleSearch(UObject* BuildingContainer)
		{
			if (BuildingContainer)
			{
				auto BuildingContainerName = BuildingContainer->GetName();

				if (BuildingContainerName.contains(_("Tiered_Chest"))) //  LCD_ToolBox
				{
					auto WeaponDef = Tables::GetWeaponDef();
					if (WeaponDef)
					{
						static auto GetAmmoWorldItemDefinition_BP = WeaponDef->Function(_("GetAmmoWorldItemDefinition_BP"));
						if (GetAmmoWorldItemDefinition_BP)
						{
							struct { UObject* AmmoDefinition; }GetAmmoWorldItemDefinition_BP_Params{};
							WeaponDef->ProcessEvent(GetAmmoWorldItemDefinition_BP, &GetAmmoWorldItemDefinition_BP_Params);
							auto AmmoDef = GetAmmoWorldItemDefinition_BP_Params.AmmoDefinition;

							// if (AmmoDef)
							{
								auto Location = Helper::GetActorLocation(BuildingContainer);
								auto Rotation = Helper::GetActorRotation(BuildingContainer);

								// TODO: Rotation...

								std::cout << _("Yaw: ") << Rotation.Yaw << '\n';

								Location.Z += 50;

								/*
								forward (N) (0) = Y + 170
								backwards (S) (180) = Y - 170

								left (270) = X - 170
								right (90) = X + 170

								if (Yaw == 30)
									Y += Rotation.Yaw * 4
									X -= 10

								*/

								if (Rotation.Yaw >= 0 && Rotation.Yaw <= 89)
								{
									Location.X -= 170;
									std::cout << _("Removed 170 from the X!\n");
								}
								else if (Rotation.Yaw >= 90 && Rotation.Yaw <= 179 || Rotation.Yaw < -180 && Rotation.Yaw >= -269)
								{
									Location.Y -= 170;
									std::cout << _("Removed 170 from the Y!\n");
								}
								else if (Rotation.Yaw >= 180 && Rotation.Yaw <= 269 || Rotation.Yaw < -179 && Rotation.Yaw >= -90)
								{
									Location.Y += 170;
									std::cout << _("Added 170 to the Y!\n");
								}
								else if (Rotation.Yaw >= 270 && Rotation.Yaw <= 360 || Rotation.Yaw < 0 && Rotation.Yaw >= -89)
								{
									Location.X += 170;
									std::cout << _("Added 170 to the X!\n");
								}
								else
								{
									std::cout << _("Unhandled rotation!\n");
								}

								// Location.X += ((Rotation.Yaw / 5) / 2);

								if (WeaponDef)
									Helper::SummonPickup(nullptr, WeaponDef, Location, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 1);

								if (AmmoDef)
								{
									auto DropCount = *AmmoDef->Member<int>(_("DropCount"));
									Helper::SummonPickup(nullptr, AmmoDef, Location, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, DropCount);
								}

								if (auto Consumable = Tables::GetConsumableDef())
								{
									Helper::SummonPickup(nullptr, Consumable, Location, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Chest, 1); // *Consumable->Member<int>(_("DropCount")));
								}
							}
						}
					}
				}

				else if (BuildingContainerName.contains(_("Ammo")))
				{
					auto AmmoDef = GetAmmoDef();

					if (AmmoDef)
					{
						auto Location = Helper::GetActorLocation(BuildingContainer);
						auto Rotation = Helper::GetActorRotation(BuildingContainer);

						// TODO: Rotation...

						std::cout << _("Yaw: ") << Rotation.Yaw << '\n';

						Location.Z += 50;

						if (Rotation.Yaw >= 0 && Rotation.Yaw <= 89)
						{
							Location.X -= 170;
							std::cout << _("Removed 170 from the X!\n");
						}
						else if (Rotation.Yaw >= 90 && Rotation.Yaw <= 179 || Rotation.Yaw < -180 && Rotation.Yaw >= -269)
						{
							Location.Y -= 170;
							std::cout << _("Removed 170 from the Y!\n");
						}
						else if (Rotation.Yaw >= 180 && Rotation.Yaw <= 269 || Rotation.Yaw < -179 && Rotation.Yaw >= -90)
						{
							Location.Y += 170;
							std::cout << _("Added 170 to the Y!\n");
						}
						else if (Rotation.Yaw >= 270 && Rotation.Yaw <= 360 || Rotation.Yaw < 0 && Rotation.Yaw >= -89)
						{
							Location.X += 170;
							std::cout << _("Added 170 to the X!\n");
						}
						else
						{
							std::cout << _("Unhandled rotation!\n");
						}

						// DROPCOUNT IS VERY WRONG
						auto DropCount = *AmmoDef->Member<int>(_("DropCount"));
						Helper::SummonPickup(nullptr, AmmoDef, Location, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::AmmoBox, DropCount);
					}
				}

				else if (BuildingContainerName.contains(_("AthenaSupplyDrop_Llama_C")))
				{
					static auto WoodItemData = FindObject(_("FortResourceItemDefinition /Game/Items/ResourcePickups/WoodItemData.WoodItemData"));
					static auto StoneItemData = FindObject(_("FortResourceItemDefinition /Game/Items/ResourcePickups/StoneItemData.StoneItemData"));
					static auto MetalItemData = FindObject(_("FortResourceItemDefinition /Game/Items/ResourcePickups/MetalItemData.MetalItemData"));

					auto Location = Helper::GetActorLocation(BuildingContainer);
					auto Rotation = Helper::GetActorRotation(BuildingContainer);

					// TODO: Rotation...

					std::cout << _("Yaw: ") << Rotation.Yaw << '\n';

					Location.Z += 50;

					if (Rotation.Yaw >= 0 && Rotation.Yaw <= 89)
					{
						Location.X -= 170;
						std::cout << _("Removed 170 from the X!\n");
					}
					else if (Rotation.Yaw >= 90 && Rotation.Yaw <= 179 || Rotation.Yaw < -180 && Rotation.Yaw >= -269)
					{
						Location.Y -= 170;
						std::cout << _("Removed 170 from the Y!\n");
					}
					else if (Rotation.Yaw >= 180 && Rotation.Yaw <= 269 || Rotation.Yaw < -179 && Rotation.Yaw >= -90)
					{
						Location.Y += 170;
						std::cout << _("Added 170 to the Y!\n");
					}
					else if (Rotation.Yaw >= 270 && Rotation.Yaw <= 360 || Rotation.Yaw < 0 && Rotation.Yaw >= -89)
					{
						Location.X += 170;
						std::cout << _("Added 170 to the X!\n");
					}
					else
					{
						std::cout << _("Unhandled rotation!\n");
					}

					{
						static auto Minis = FindObject(_("FortWeaponRangedItemDefinition /Game/Athena/Items/Consumables/ShieldSmall/Athena_ShieldSmall.Athena_ShieldSmall"));

						Helper::SummonPickup(nullptr, WoodItemData, Location, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, 200);
						Helper::SummonPickup(nullptr, StoneItemData, Location, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, 200);
						Helper::SummonPickup(nullptr, MetalItemData, Location, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, 200);
						Helper::SummonPickup(nullptr, Minis, Location, EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::SupplyDrop, 6);
					}
				}

				else if (!BuildingContainerName.contains(_("Door")) && !BuildingContainerName.contains(_("Wall")))
				{
					std::cout << _("Unhandled container: ") << BuildingContainerName << "!\n";
				}
			}
		}
	}
}