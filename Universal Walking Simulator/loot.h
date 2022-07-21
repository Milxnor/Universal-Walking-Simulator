#include <UE/structs.h>
#include "Gameplay/helper.h"

void GetLootTable()
{
	/*
	
	[65899] SoftObjectProperty /Script/FortniteGame.FortPlaylist.ResourceRates
	[65900] SoftObjectProperty /Script/FortniteGame.FortPlaylist.GameData
	[65901] SoftObjectProperty /Script/FortniteGame.FortPlaylist.RangedWeapons
	[65902] SoftObjectProperty /Script/FortniteGame.FortPlaylist.LootPackages
	[65903] SoftObjectProperty /Script/FortniteGame.FortPlaylist.LootTierData

	*/

	// static auto Playlist = FindObject(_("FortPlaylistAthena /Game/Athena/Playlists/Playlist_DefaultDuo.Playlist_DefaultDuo"));
	// static auto Playlist = FindObject(_("FortPlaylistAthena /Game/Athena/Playlists/Playlist_DefaultSquad.Playlist_DefaultSquad"));

	static UObject* Playlist = nullptr;

	if (!Playlist)
	{
		auto world = Helper::GetWorld();
		auto gameState = *world->Member<UObject*>(_("GameState"));

		if (std::stod(FN_Version) > 6.00)
		{
			static auto BasePlaylistOffset = FindOffsetStruct(_("ScriptStruct /Script/FortniteGame.PlaylistPropertyArray"), _("BasePlaylist"));
			static auto PlaylistInfo = gameState->Member<FFastArraySerializer>(_("CurrentPlaylistInfo"));

			if (BasePlaylistOffset)
			{
				auto BasePlaylist = (UObject**)(__int64(PlaylistInfo) + BasePlaylistOffset);// *gameState->Member<UObject>(_("CurrentPlaylistInfo"))->Member<UObject*>(_("BasePlaylist"), true);

				if (BasePlaylist && *BasePlaylist)
				{
					Playlist = *BasePlaylist;
				}
				else
					std::cout << _("Base Playlist is null!\n");
			}
			else
			{
				std::cout << _("Missing something related to the Playlist!\n");
				std::cout << _("BasePlaylist Offset: ") << BasePlaylistOffset << '\n';
				std::cout << _("Playlist: ") << Playlist << '\n';
			}
		}
		else
		{
			Playlist = *gameState->Member<UObject*>(_("CurrentPlaylistData"));
		}
	}

	if (Playlist)
	{

	}
}

void FindRow()
{

}