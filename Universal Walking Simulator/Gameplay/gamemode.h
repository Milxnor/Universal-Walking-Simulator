#pragma once
#include <vector>

#include "UE/structs.h"

namespace GameMode
{
    inline constexpr auto PlaygroundPlaylist = "FortPlaylistAthena /Game/Athena/Playlists/Playground/Playlist_Playground.Playlist_Playground";
    inline constexpr auto LateGamePlaylist = "FortPlaylistAthena /Game/Athena/Playlists/Playground/Playlist_LateGame.Playlist_Playground"; // Made up
    
    inline FVector AircraftLocation = FVector{ 3500, -9180, 10500 };
    inline std::string PlaylistName = PlaygroundPlaylist;
    inline std::vector<UObject*> Buildings;
    inline bool UseBeacons = true;
    inline bool UseDoubleBuildFix = false;
    inline bool IsRunningEvent = false;

    inline auto GetMapName()
    {
        if (FortniteVersion >= 19.00)
        {
            return L"Artemis_Terrain?game=/Game/Athena/Athena_GameMode.Athena_GameMode_C";
        }

        if (FortniteVersion >= 424)
        {
            return L"Apollo_Terrain?game=/Game/Athena/Athena_GameMode.Athena_GameMode_C";
        }

        return L"Athena_Terrain?game=/Game/Athena/Athena_GameMode.Athena_GameMode_C";
    }

    inline bool IsPlayground()
    {
        return PlaylistName == PlaygroundPlaylist;
    }

    inline bool IsLateGame()
    {
        return PlaylistName == LateGamePlaylist;
    }
}
