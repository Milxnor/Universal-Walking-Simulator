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

	static constexpr int StartingTeamIndex = 4; // 3?
	static int NextTeamIndex = StartingTeamIndex; // TODO: Use GameMode->NumTeams

	uint8_t* GetTeamIndex(UObject* PlayerState)
	{
		static auto TeamIndexOffset = GetOffset(PlayerState, "TeamIndex");

		return (uint8_t*)(__int64(PlayerState) + TeamIndexOffset);
	}

	void AssignTeam(UObject* Controller)
	{
		auto World = Helper::GetWorld();
		auto GameState = Helper::GetGameState(World);
		auto GameMode = Helper::GetGameMode();
		auto Playlist = Helper::GetPlaylist();

		int MaxPlayersPerTeam = 1;

		if (Playlist)
		{
			static auto MaxPlayersPerTeamOffset = GetOffset(Playlist, "MaxTeamSize");
			MaxPlayersPerTeam = *(int*)(__int64(Playlist) + MaxPlayersPerTeamOffset);
		}
		else
			MaxPlayersPerTeam = maxAmountOfPlayersPerTeam;

		// we really shouldn't cache any of these as they aren't used anywhere else.

		auto PlayerState = Helper::GetPlayerStateFromController(Controller);

		// bool bAllowJoinInProgress = *Playlist->CachedMember<bool>("bAllowJoinInProgress");

		static auto TeamsOffset = GetOffset(GameState, "Teams");
		auto AllTeams = (TArray<AFortTeamInfo*>*)(__int64(GameState) + TeamsOffset);

		if (!AllTeams)
		{
			std::cout << "No AllTeams!\n";
			return;
		}

		if (AllTeams->Num() <= 1)
		{
			std::cout << "AllTeams is empty!\n";
			return;
		}

		std::cout << "AllTeams Num: " << AllTeams->Num() << '\n';

		static auto PlayerTeamOffset = GetOffset(PlayerState, "PlayerTeam");
		auto PlayerTeam = (UObject**)(__int64(PlayerState) + PlayerTeamOffset);

		AFortTeamInfo* CurrentTeam = AllTeams->At(NextTeamIndex); // *PlayerTeam;

		std::cout << "NextTeamIndex: " << NextTeamIndex << '\n';

		if (!CurrentTeam)
			return;

		/* std::cout << "Current Team: " << CurrentTeam->GetFullName() << '\n';
		std::cout << "Current Team PrivateInfo: " << (*CurrentTeam->Member<UObject*>("PrivateInfo"))->GetFullName() << '\n';
		std::cout << "Team: " << (int)*CurrentTeam->Member<uint8_t>("Team") << '\n';
		std::cout << "CUrrentTeams TEAM: " << (int)*(*PlayerState->Member<UObject*>("PlayerTeam"))->Member<uint8_t>("Team") << '\n'; */

		static auto TeamMembersOffset = GetOffset(CurrentTeam, "TeamMembers");
		auto CurrentTeamMembers = (TArray<AController*>*)(__int64(CurrentTeam) + TeamMembersOffset);

		std::cout << "CurrentTeamMembers->Num(): " << CurrentTeamMembers->Num() << '\n';
		std::cout << "MaxPlayersPerTeam: " << MaxPlayersPerTeam << '\n';

		if (CurrentTeamMembers->Num() >= MaxPlayersPerTeam)
		{
			++NextTeamIndex;
			CurrentTeam = AllTeams->At(NextTeamIndex);
			CurrentTeamMembers = (TArray<AController*>*)(__int64(CurrentTeam) + TeamMembersOffset);
		}

		// now we have the correct teaminfo and members

		std::cout << "CurrentTeam: " << CurrentTeam << '\n';
		std::cout << "CurrentTeamMembers->Num() New: " << CurrentTeamMembers->Num() << '\n';

		*PlayerTeam = CurrentTeam;

		auto TeamIndex = NextTeamIndex;
		auto SquadId = NextTeamIndex - (Engine_Version < 423 ? 0 : 0); // (Engine_Version >= 424 ? 2 : 1); // nice one fortnite // -0 -1 -2 ??

		std::cout << std::format("New player going on {} as {} team member.\n", TeamIndex, SquadId);

		auto PlayerStateTeamIDX = Teams::GetTeamIndex(PlayerState);
		auto OldTeamIdx = *PlayerStateTeamIDX;

		*PlayerStateTeamIDX = TeamIndex;

		static auto SquadIdOffset = GetOffset(PlayerState, "SquadId");

		auto PlayerStateSquadId = (uint8_t*)(__int64(PlayerState) + SquadIdOffset);

		if (SquadIdOffset != -1 && PlayerStateSquadId)
		{
			*PlayerStateSquadId = SquadId;
		}

		// if (false)
		{
			// std::cout << "PReviosut eam: " << *PlayerState->Member<UObject*>("PlayerTeam") << '\n';
			CurrentTeamMembers->Add(Controller);

			*PlayerTeam = CurrentTeam;

			static auto PlayerTeamPrivateOffset = GetOffset(PlayerState, "PlayerTeamPrivate");
			static auto PrivateInfoOffset = GetOffset(CurrentTeam, "PrivateInfo");

			if (PlayerTeamPrivateOffset != -1 && PrivateInfoOffset != -1)
			{
				*(AFortTeamPrivateInfo**)(__int64(PlayerState) + PlayerTeamPrivateOffset) = *(AFortTeamPrivateInfo**)(__int64(CurrentTeam) + PrivateInfoOffset);
			}

			static auto OnRep_PlayerTeam = PlayerState->Function("OnRep_PlayerTeam");
			PlayerState->ProcessEvent(OnRep_PlayerTeam);

			static auto OnRep_PlayerTeamPrivate = PlayerState->Function("OnRep_PlayerTeamPrivate");

			if (OnRep_PlayerTeamPrivate)
				PlayerState->ProcessEvent("OnRep_PlayerTeamPrivate");
		}

		static auto OnRep_TeamIndex = PlayerState->Function("OnRep_TeamIndex");

		if (OnRep_TeamIndex)
			PlayerState->ProcessEvent(OnRep_TeamIndex, &OldTeamIdx);

		static auto OnRep_SquadId = PlayerState->Function("OnRep_SquadId");

		if (OnRep_SquadId)
			PlayerState->ProcessEvent(OnRep_SquadId);
		
		/* auto FriendsInSquad = PlayerState->Member<TArray<UObject*>>("FriendsInSquad");
		auto SquadPlayerStates = PlayerState->Member<TArray<UObject*>>("SquadPlayerStates");

		for (int i = 0; i < CurrentTeamMembers->Num(); i++)
		{
			FriendsInSquad->Add(CurrentTeamMembers->At(i));
			SquadPlayerStates->Add(CurrentTeamMembers->At(i));
			(*PlayerState->Member<int>("SquadListUpdateValue"))++;
			PlayerState->ProcessEvent("OnRep_SquadListUpdateValue");
		} */

		// (*PlayerTeam)->CachedMember<TArray<AController*>>("TeamMembers")->Add(Controller);

		/* std::cout << "Current Team PrivateInfo: " << (*CurrentTeam->Member<UObject*>("PrivateInfo"))->GetFullName() << '\n';

		std::cout << "PlayerTeam: " << *PlayerState->Member<UObject*>("PlayerTeam") << '\n';
		std::cout << "Team Members size: " << CurrentTeamMembers->Num() << '\n'; */

		std::cout << "Team Members size: " << CurrentTeamMembers->Num() << '\n';

		if (Engine_Version >= 422)
		{
			static auto UniqueIdOffset = GetOffset(PlayerState, "UniqueId");

			FGameMemberInfo MemberInfo = FGameMemberInfo{ -1, -1, -1 };
			MemberInfo.TeamIndex = *PlayerStateTeamIDX; // TeamIndex;
			MemberInfo.SquadId = *PlayerStateSquadId; // SquadId;
			MemberInfo.MemberUniqueId = *(FUniqueNetIdRepl*)(__int64(PlayerState) + UniqueIdOffset);
			// MemberInfo.funny = 100 + *PlayerStateTeamIDX + *PlayerStateSquadId;

			static auto GameMemberInfoArrayOffset = GetOffset(GameState, "GameMemberInfoArray");
			auto GameMemberInfoArray = (void*)(__int64(GameState) + GameMemberInfoArrayOffset);

			if (GameMemberInfoArray)
			{
				static auto MembersOffset = FindOffsetStruct("ScriptStruct /Script/FortniteGame.GameMemberInfoArray", "Members");
				auto Members = (TArray<FGameMemberInfo>*)(__int64(GameMemberInfoArray) + MembersOffset);

				if (Members)
				{
					Members->Add(MemberInfo);
					MarkArrayDirty(GameMemberInfoArray);
				}
			}
		}

		// ++NextTeamIndex;
	}
}