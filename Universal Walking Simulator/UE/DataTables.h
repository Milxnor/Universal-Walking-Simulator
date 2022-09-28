#pragma once

#include <UE/structs.h>
#include <Gameplay/helper.h>

static TMap<FName, uint8_t*> GetRowMap(UObject* DataTable)
{
	static auto RowStructOffset = GetOffset(DataTable, "RowStruct");
	return *(TMap<FName, uint8_t*>*)(__int64(DataTable) + (RowStructOffset + sizeof(UObject*))); // because after rowstruct is rowmap
}

static uint8_t* FindRow(UObject* DataTable, const std::string& RowName)
{

}

// this dont really belong here
static UObject* GetLootPackages()
{
	if (FnVerDouble <= 2.5 || Engine_Version == 419) // TSOFTOBJECTPTR CHANGED or soemthing and theres no playlist
		return nullptr;

	UObject* Playlist = Helper::GetPlaylist();

	if (!Playlist)
		return nullptr;

	auto LootPackagesSoft = Playlist->Member<TSoftObjectPtr>(("LootPackages"));

	bool bForcePlaygroundLoot = Engine_Version >= 424;

	// FortniteGame/Content/Athena/Playlists/Fortnite/AthenaCompositeLP_Fortnite
	std::string Default = bIsPlayground || bForcePlaygroundLoot ? "/Game/Athena/Playlists/Playground/AthenaLootPackages_Client.AthenaLootPackages_Client" : "/Game/Items/Datatables/AthenaLootPackages_Client.AthenaLootPackages_Client";

	auto& LootPackagesName = Default;

	if (LootPackagesSoft && LootPackagesSoft->ObjectID.AssetPathName.ComparisonIndex && !bForcePlaygroundLoot) {
		LootPackagesName = LootPackagesSoft->ObjectID.AssetPathName.ToString();
		// std::cout << "SubPath: " << LootPackagesSoft->ObjectID.SubPathString.ToString();
	}
	else
		std::cout << std::format("Unable to find LootPackages! Falling back to default {}.\n", bIsPlayground ? "playground" : "solos");

	std::cout << "AssetPathName: " << LootPackagesName << '\n';

	static auto ClassToUse = (LootPackagesName.contains("Composite")) ?
		FindObject("Class /Script/Engine.CompositeDataTable") : FindObject("Class /Script/Engine.DataTable"); // cursed

	auto LootPackages = StaticLoadObject(ClassToUse, nullptr, LootPackagesName); // some versions its loaded idk why but some not
	std::cout << "LootPackages: " << LootPackages << '\n';

	if (LootPackages)
		std::cout << "LootPackages Name: " << LootPackages->GetFullName() << '\n';
	else
		return nullptr;

	return LootPackages;
}

struct FRichCurveKey
{
	ERichCurveInterpMode GetInterpMode() const
	{
		static auto InterpModeOffset = FindOffsetStruct("ScriptStruct /Script/Engine.RichCurveKey", "InterpMode");

		return *(ERichCurveInterpMode*)(__int64(this) + InterpModeOffset);
	}

	float GetTime() const
	{
		static auto TimeOffset = FindOffsetStruct("ScriptStruct /Script/Engine.RichCurveKey", "Time");

		return *(float*)(__int64(this) + TimeOffset);
	}

	float GetValue() const
	{
		static auto ValueOffset = FindOffsetStruct("ScriptStruct /Script/Engine.RichCurveKey", "Value");

		return *(float*)(__int64(this) + ValueOffset);
	}

	float GetLeaveTangent() const
	{
		static auto LeaveTangentOffset = FindOffsetStruct("ScriptStruct /Script/Engine.RichCurveKey", "LeaveTangent");

		return *(float*)(__int64(this) + LeaveTangentOffset);
	}

	float GetArriveTangent() const
	{
		static auto ArriveTangentOffset = FindOffsetStruct("ScriptStruct /Script/Engine.RichCurveKey", "ArriveTangent");

		return *(float*)(__int64(this) + ArriveTangentOffset);
	}
};

namespace FRichCurve
{
#define MAX_flt			(3.402823466e+38F)

	float Eval(UObject* Curve, float InTime, float InDefaultValue)
	{
		// i tried remaking but there was one eval function that was too much
	}
}