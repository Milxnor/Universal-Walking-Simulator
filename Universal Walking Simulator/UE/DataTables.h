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
	if (Engine_Version < 420) // TSOFTOBJECTPTR CHANGED or soemthing
		return nullptr;

	UObject* Playlist = Helper::GetPlaylist();

	if (!Playlist)
		return nullptr;

	auto LootPackagesSoft = Playlist->Member<TSoftObjectPtr>(("LootPackages"));

	std::string Default = "/Game/Athena/Playlists/Playground/AthenaLootPackages_Client.AthenaLootPackages_Client";

	auto& LootPackagesName = Default;

	if (LootPackagesSoft && LootPackagesSoft->ObjectID.AssetPathName.ComparisonIndex) {
		LootPackagesName = LootPackagesSoft->ObjectID.AssetPathName.ToString();
		// std::cout << "SubPath: " << LootPackagesSoft->ObjectID.SubPathString.ToString();
	}
	else
		std::cout << "Unable to find LootPackages! Falling back to default Playground.\n";

	std::cout << "AssetPathName: " << LootPackagesName << '\n';

	static auto ClassToUse = LootPackagesName.ends_with("_Client") ? FindObject("Class /Script/Engine.DataTable") : FindObject("Class /Script/Engine.CompositeDataTable"); // cursed

	auto LootPackages = StaticLoadObject(ClassToUse, nullptr, LootPackagesName); // some versions its loaded idk why but some not
	std::cout << "LootPackages: " << LootPackages << '\n';

	if (LootPackages)
		std::cout << "LootPackages Name: " << LootPackages->GetFullName() << '\n';
	else
		return nullptr;

	return LootPackages;
}