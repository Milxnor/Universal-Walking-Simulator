#include <Gameplay/helper.h>

namespace Teams
{
	using AFortTeamInfo = UObject;
	using AFortTeamPrivateInfo = UObject;
	using AController = UObject;

	struct FGameMemberInfo : public FFastArraySerializerItem
	{
		unsigned char                                      SquadId;                                                  // 0x000C(0x0001) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		unsigned char                                      TeamIndex;                                                // 0x000D(0x0001) (ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		short                                      funny; // seems like this is this max teams (100) + squad id + teamindex
		FUniqueNetIdRepl                            MemberUniqueId;                                           // 0x0010(0x0028) (HasGetValueTypeHash, NativeAccessSpecifierPublic)
	};


	struct FPrivateTeamDataItem : public FFastArraySerializerItem
	{
	public:
		float                                        Value;                                             // 0xC(0x4)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
		FUniqueNetIdRepl                      PlayerID;                                          // 0x10(0x28)(HasGetValueTypeHash, NativeAccessSpecifierPublic)
		uint8_t                                        Pad_2787[0x8];                                     // Fixing Size Of Struct [ Dumper-7 ]
	};

	static constexpr int StartingTeamIndex = 4;
	static int NextTeamIndex = StartingTeamIndex; // TODO: Use GameMode->NumTeams

	uint8_t* GetTeamIndex(UObject* PlayerState)
	{
		static auto TeamIndexOffset = GetOffset(PlayerState, "TeamIndex");

		return (uint8_t*)(__int64(PlayerState) + TeamIndexOffset);
	}

	int GetMaxPlayersPerTeam()
	{
		if (Engine_Version <= 419)
			return 1;

		auto Playlist = Helper::GetPlaylist();

		static auto MaxTeamSizeOffset = GetOffset(Playlist, "MaxTeamSize");

		return *(int*)(__int64(Playlist) + MaxTeamSizeOffset);
	}

	void AssignTeam(UObject* Controller)
	{
		auto World = Helper::GetWorld();
		auto GameState = Helper::GetGameState(World);
		auto GameMode = *World->Member<UObject*>(("AuthorityGameMode"));
		auto Playlist = Helper::GetPlaylist();

		// we really shouldn't cache any of these as they aren't used anywhere else.

		auto PlayerState = Helper::GetPlayerStateFromController(Controller);

		static auto MaxPlayersPerTeamOffset = GetOffset(Playlist, "MaxTeamSize");

		int MaxPlayersPerTeam = *(int*)(__int64(Playlist) + MaxPlayersPerTeamOffset);
		int MaxTeams = GetMaxPlayersPerTeam();
		// bool bAllowJoinInProgress = *Playlist->CachedMember<bool>("bAllowJoinInProgress");

		auto AllTeams = GameState->Member<TArray<AFortTeamInfo*>>("Teams");

		if (!AllTeams)
			return;

		if (AllTeams->Num() <= 1)
		{
			std::cout << "AllTeams is empty!\n";
			return;
		}

		std::cout << "AllTeams Num: " << AllTeams->Num() << '\n';

		auto PlayerTeam = PlayerState->Member<UObject*>("PlayerTeam");
		AFortTeamInfo* CurrentTeam = AllTeams->At(NextTeamIndex); // *PlayerTeam;

		std::cout << "CurrentTeam: " << CurrentTeam << '\n';
		std::cout << "NextTeamIndex: " << NextTeamIndex << '\n';

		if (!CurrentTeam)
			return;

		/* std::cout << "Current Team: " << CurrentTeam->GetFullName() << '\n';
		std::cout << "Current Team PrivateInfo: " << (*CurrentTeam->Member<UObject*>("PrivateInfo"))->GetFullName() << '\n';
		std::cout << "Team: " << (int)*CurrentTeam->Member<uint8_t>("Team") << '\n';
		std::cout << "CUrrentTeams TEAM: " << (int)*(*PlayerState->Member<UObject*>("PlayerTeam"))->Member<uint8_t>("Team") << '\n'; */

		auto CurrentTeamMembers = CurrentTeam->CachedMember<TArray<AController*>>("TeamMembers");

		std::cout << "CurrentTeamMembers->Num(): " << CurrentTeamMembers->Num() << '\n';
		std::cout << "MaxPlayersPerTeam: " << MaxPlayersPerTeam << '\n';

		if (CurrentTeamMembers->Num() >= MaxPlayersPerTeam)
		{
			CurrentTeam = AllTeams->At(++NextTeamIndex);
			CurrentTeamMembers = CurrentTeam->CachedMember<TArray<AController*>>("TeamMembers");
		}

		// now we have the correct teaminfo and members

		*PlayerTeam = CurrentTeam;

		auto TeamIndex = NextTeamIndex;
		auto SquadId = NextTeamIndex - 0; // (Engine_Version >= 424 ? 2 : 1); // nice one fortnite // -0 -1 -2 ??

		std::cout << std::format("New player going on {} as {} team member.\n", TeamIndex, SquadId);

		auto PlayerStateTeamIDX = Teams::GetTeamIndex(PlayerState);
		auto OldTeamIdx = *PlayerStateTeamIDX;
		auto PlayerStateSquadId = PlayerState->Member<uint8_t>("SquadId");

		*PlayerStateTeamIDX = TeamIndex;
		*PlayerStateSquadId = SquadId;

		if (false)
		{
			std::cout << "PReviosut eam: " << *PlayerState->Member<UObject*>("PlayerTeam") << '\n';
			CurrentTeamMembers->Add(Controller);

			*PlayerTeam = CurrentTeam;
			*PlayerState->Member<AFortTeamPrivateInfo*>("PlayerTeamPrivate") = *CurrentTeam->Member<AFortTeamPrivateInfo*>("PrivateInfo");

			PlayerState->ProcessEvent("OnRep_PlayerTeam");
			PlayerState->ProcessEvent("OnRep_PlayerTeamPrivate");
		}

		PlayerState->ProcessEvent("OnRep_SquadId");
		PlayerState->ProcessEvent("OnRep_TeamIndex", &OldTeamIdx);

		(*PlayerTeam)->CachedMember<TArray<AController*>>("TeamMembers")->Add(Controller);

		/* std::cout << "Current Team PrivateInfo: " << (*CurrentTeam->Member<UObject*>("PrivateInfo"))->GetFullName() << '\n';

		std::cout << "PlayerTeam: " << *PlayerState->Member<UObject*>("PlayerTeam") << '\n';
		std::cout << "Team Members size: " << CurrentTeamMembers->Num() << '\n'; */

		if (Engine_Version >= 422)
		{
			FGameMemberInfo MemberInfo = FGameMemberInfo{ -1, -1, -1 };
			MemberInfo.TeamIndex = *PlayerStateTeamIDX; // TeamIndex;
			MemberInfo.SquadId = *PlayerStateSquadId; // SquadId;
			MemberInfo.MemberUniqueId = *PlayerState->CachedMember<FUniqueNetIdRepl>("UniqueId");
			// MemberInfo.funny = 100 + *PlayerStateTeamIDX + *PlayerStateSquadId;

			auto GameMemberInfoArray = GameState->Member<void>("GameMemberInfoArray");

			if (GameMemberInfoArray)
			{
				static auto MembersOffset = FindOffsetStruct("ScriptStruct /Script/FortniteGame.GameMemberInfoArray", "Members");
				auto Members = (TArray<FGameMemberInfo>*)(__int64(GameMemberInfoArray) + MembersOffset);

				if (GameMemberInfoArray && Members)
				{
					Members->Add(MemberInfo);
					MarkArrayDirty(GameMemberInfoArray);
				}
			}
		}
	}
}
