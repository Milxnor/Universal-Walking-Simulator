#pragma once

#include <MinHook/MinHook.h>

#include <Net/funcs.h>
#include <Gameplay/helper.h>
#include <Gameplay/inventory.h>
#include <Gameplay/abilities.h>
#include <Gameplay/loot.h>

#include <discord.h>
#include <Net/replication.h>

static bool bTraveled = false;

inline bool ServerAcknowledgePossessionHook(UObject* Object, UFunction* Function, void* Parameters) // This fixes movement on S5+, Cat led me to the right direction, then I figured out it's something with ClientRestart and did some common sense and found this.
{
    struct SAP_Params
    {
        UObject* P;
    };
    auto Params = (SAP_Params*)Parameters;
    if (Params)
    {
        auto Pawn = Params->P; // (UObject*)Parameters;

        *Object->Member<UObject*>(("AcknowledgedPawn")) = Pawn;
        std::cout << "Set Pawn!\n";
    }

    return false;
}

void InitializeNetUHooks()
{
    AddHook(("Function /Script/Engine.PlayerController.ServerAcknowledgePossession"), ServerAcknowledgePossessionHook);
}

void TickFlushDetour(UObject* thisNetDriver, float DeltaSeconds)
{
    // std::cout << ("TicKFluSh!\n");
    static auto NetDriver = *Helper::GetWorld()->Member<UObject*>(("NetDriver"));
    static auto& ClientConnections = *NetDriver->Member<TArray<UObject*>>(("ClientConnections"));

    if (ClientConnections.Num() > 0)
    {
        ServerReplicateActors(NetDriver);
    }

    return TickFlush(thisNetDriver, DeltaSeconds);
}

uint64_t GetNetModeDetour(UObject* World)
{
    return ENetMode::NM_ListenServer;
}

bool LP_SpawnPlayActorDetour(UObject* Player, const FString& URL, FString& OutError, UObject* World)
{
    if (!bTraveled)
    {
        return LP_SpawnPlayActor(Player, URL, OutError, World);
    }
    return true;
}

char KickPlayerDetour(__int64 a1, __int64 a2, __int64 a3)
{
    std::cout << ("Player Kick! Returning 0..\n");
    return 0;
}

char __fastcall ValidationFailureDetour(__int64* a1, __int64 a2)
{
    std::cout << ("Validation!\n");
    return 0;
}

bool bMyPawn = false; // UObject*

UObject* SpawnPlayActorDetour(UObject* World, UObject* NewPlayer, ENetRole RemoteRole, FURL& URL, void* UniqueId, FString& Error, uint8_t NetPlayerIndex)
{
    static bool bSpawnedFloorLoot = false;

    if (!bSpawnedFloorLoot)
    {
        // CreateThread(0, 0, Looting::Tables::SpawnFloorLoot, 0, 0, 0);
        bSpawnedFloorLoot = true;
    }

    std::cout << ("SpawnPlayActor called!\n");
    // static UObject* CameraManagerClass = FindObject(("BlueprintGeneratedClass /Game/Blueprints/Camera/Original/MainPlayerCamera.MainPlayerCamera_C"));

    auto PlayerController = SpawnPlayActor(Helper::GetWorld(), NewPlayer, RemoteRole, URL, UniqueId, Error, NetPlayerIndex); // crashes 0x356 here sometimes when rejoining

    // *PlayerController->Member<UObject*>(("PlayerCameraManagerClass")) = CameraManagerClass;

    static auto FortPlayerControllerAthenaClass = FindObject(("Class /Script/FortniteGame.FortPlayerControllerAthena"));

    if (!PlayerController || !PlayerController->IsA(FortPlayerControllerAthenaClass))
        return nullptr;

    auto PlayerState = *PlayerController->Member<UObject*>(("PlayerState"));

    if (Helper::Banning::IsBanned(Helper::GetfIP(PlayerState).Data.GetData()))
    {
        FString Reason;
        Reason.Set(L"You are banned!");
        Helper::KickController(PlayerController, Reason);
    }

    auto OPlayerName = Helper::GetPlayerName(PlayerState);
    std::string PlayerName = OPlayerName;
    std::transform(OPlayerName.begin(), OPlayerName.end(), OPlayerName.begin(), ::tolower);

    if (OPlayerName.contains(("fuck")) || OPlayerName.contains(("shit")))
    {
        FString Reason;
        Reason.Set(L"Inappropriate name!");
        Helper::KickController(PlayerController, Reason);
    }

    if (OPlayerName.length() >= 40)
    {
        FString Reason;
        Reason.Set(L"Too long of a name!");
        Helper::KickController(PlayerController, Reason);
    }

    *NewPlayer->Member<UObject*>(("PlayerController")) = PlayerController;

    static const auto FnVerDouble = std::stod(FN_Version);

    if (FnVerDouble < 7.4)
    {
        static const auto QuickBarsClass = FindObject("Class /Script/FortniteGame.FortQuickBars", true);
        auto QuickBars = PlayerController->Member<UObject*>(("QuickBars"));

        if (QuickBars)
        {
            *QuickBars = Easy::SpawnActor(QuickBarsClass, FVector(), FRotator());
            Helper::SetOwner(*QuickBars, PlayerController);
        }
    }

    auto Pawn = Helper::InitPawn(PlayerController, true, Helper::GetPlayerStart(), true);

    if (!Pawn)
        return PlayerController;

    if (GiveAbility || GiveAbilityFTS)
    {
        auto AbilitySystemComponent = *Pawn->Member<UObject*>(("AbilitySystemComponent"));

        if (AbilitySystemComponent && FnVerDouble < 14 && Engine_Version >= 424)
        {
            std::cout << ("Granting abilities!\n");
            if (FnVerDouble < 8)
            {
                static auto AbilitySet = FindObject(("FortAbilitySet /Game/Abilities/Player/Generic/Traits/DefaultPlayer/GAS_DefaultPlayer.GAS_DefaultPlayer"));

                if (AbilitySet)
                {
                    auto Abilities = AbilitySet->Member<TArray<UObject*>>(("GameplayAbilities"));

                    if (Abilities)
                    {
                        for (int i = 0; i < Abilities->Num(); i++)
                        {
                            auto Ability = Abilities->At(i);

                            if (!Ability)
                                continue;

                            GrantGameplayAbility(Pawn, Ability);
                        }
                    }
                }
            }
            else
            {
                // static auto MatsAbility = FindObject(("/Game/Athena/Playlists/Fill/GA_Fill.GA_Fill"));
                //GrantGameplayAbility(Pawn, MatsAbility);

                static auto SprintAbility = FindObject("FortGameplayAbility_Sprint /Script/FortniteGame.Default__FortGameplayAbility_Sprint");
                static auto JumpAbility = FindObject("FortGameplayAbility_Jump /Script/FortniteGame.Default__FortGameplayAbility_Jump");
                static auto InteractUseAbility = FindObject("GA_DefaultPlayer_InteractUse_C /Game/Abilities/Player/Generic/Traits/DefaultPlayer/GA_DefaultPlayer_InteractUse.Default__GA_DefaultPlayer_InteractUse_C");
                static auto InteractSearchAbility = FindObject("GA_DefaultPlayer_InteractSearch_C /Game/Abilities/Player/Generic/Traits/DefaultPlayer/GA_DefaultPlayer_InteractSearch.Default__GA_DefaultPlayer_InteractSearch_C");
                
                // GAB_CarryPlayer_C

                if (Engine_Version >= 424)
                {
                    static auto JumpOutAbility = FindObject("GA_Athena_HidingProp_JumpOut_C /Game/Athena/Items/EnvironmentalItems/HidingProps/GA_Athena_HidingProp_JumpOut.Default__GA_Athena_HidingProp_JumpOut_C");
                    static auto HidingAbility = FindObject("GA_Athena_HidingProp_Hide_C /Game/Athena/Items/EnvironmentalItems/HidingProps/GA_Athena_HidingProp_Hide.Default__GA_Athena_HidingProp_Hide_C");
                    static auto LandedOnAbility = FindObject("GA_Athena_HidingProp_LandedOn_C /Game/Athena/Items/EnvironmentalItems/HidingProps/GA_Athena_HidingProp_LandedOn.Default__GA_Athena_HidingProp_LandedOn_C");

                    if (JumpOutAbility)
                        GrantGameplayAbility(Pawn, JumpOutAbility);
                    if (HidingAbility)
                        GrantGameplayAbility(Pawn, HidingAbility);
                    if (LandedOnAbility)
                        GrantGameplayAbility(Pawn, LandedOnAbility);
                }

                if (SprintAbility)
                    GrantGameplayAbility(Pawn, SprintAbility);
                if (JumpAbility)
                    GrantGameplayAbility(Pawn, JumpAbility);
                if (InteractUseAbility)
                   GrantGameplayAbility(Pawn, InteractUseAbility);
                if (InteractSearchAbility)
                   GrantGameplayAbility(Pawn, InteractSearchAbility);

            /*    static auto SprintAbility = FindObject(("Class /Script/FortniteGame.FortGameplayAbility_Sprint"));
                static auto ReloadAbility = FindObject(("Class /Script/FortniteGame.FortGameplayAbility_Reload"));
                static auto JumpAbility = FindObject(("Class /Script/FortniteGame.FortGameplayAbility_Jump"));
                static auto InteractUseAbility = FindObject(("BlueprintGeneratedClass /Game/Abilities/Player/Generic/Traits/DefaultPlayer/GA_DefaultPlayer_InteractUse.GA_DefaultPlayer_InteractUse_C"));
                static auto InteractSearchAbility = FindObject("BlueprintGeneratedClass /Game/Abilities/Player/Generic/Traits/DefaultPlayer/GA_DefaultPlayer_InteractSearch.GA_DefaultPlayer_InteractSearch_C");
                static auto EnterVehicleAbility = FindObject(("BlueprintGeneratedClass /Game/Athena/DrivableVehicles/GA_AthenaEnterVehicle.GA_AthenaEnterVehicle_C"));
                static auto ExitVehicleAbility = FindObject(("BlueprintGeneratedClass /Game/Athena/DrivableVehicles/GA_AthenaExitVehicle.GA_AthenaExitVehicle_C"));
                static auto InVehicleAbility = FindObject(("BlueprintGeneratedClass /Game/Athena/DrivableVehicles/GA_AthenaInVehicle.GA_AthenaInVehicle_C"));

                if (SprintAbility)
                    GrantGameplayAbility(Pawn, SprintAbility); //, true);
                if (ReloadAbility)
                    GrantGameplayAbility(Pawn, ReloadAbility);
                if (JumpAbility)
                    GrantGameplayAbility(Pawn, JumpAbility);
                if (InteractUseAbility)
                    GrantGameplayAbility(Pawn, InteractUseAbility);
                if (InteractSearchAbility)
                    GrantGameplayAbility(Pawn, InteractSearchAbility);
                if (EnterVehicleAbility)
                    GrantGameplayAbility(Pawn, EnterVehicleAbility);
                if (ExitVehicleAbility)
                    GrantGameplayAbility(Pawn, ExitVehicleAbility);
                if (InVehicleAbility)
                    GrantGameplayAbility(Pawn, InVehicleAbility);
            // if (FnVerDouble < 9)
            {
                static auto EmoteAbility = FindObject(("BlueprintGeneratedClass /Game/Abilities/Emotes/GAB_Emote_Generic.GAB_Emote_Generic_C"));

                if (EmoteAbility)
                    GrantGameplayAbility(Pawn, EmoteAbility);
            } */

            }

            std::cout << ("Granted Abilities!\n");
        }
        else
            std::cout << ("Unable to find AbilitySystemComponent!\n");
    }
    else
        std::cout << ("Unable to grant abilities due to no GiveAbility!\n");

    // if (FnVerDouble < 15) // if (Engine_Version < 424) // if (FnVerDouble < 9) // (std::floor(FnVerDouble) != 9)
    {
        static auto PickaxeDef = FindObject(("FortWeaponMeleeItemDefinition /Game/Athena/Items/Weapons/WID_Harvest_Pickaxe_Athena_C_T01.WID_Harvest_Pickaxe_Athena_C_T01"));
        static auto Minis = FindObject(("FortWeaponRangedItemDefinition /Game/Athena/Items/Consumables/ShieldSmall/Athena_ShieldSmall.Athena_ShieldSmall"));
        static auto SlurpJuice = FindObject(("FortWeaponRangedItemDefinition /Game/Athena/Items/Consumables/PurpleStuff/Athena_PurpleStuff.Athena_PurpleStuff"));
        static auto GoldAR = FindObject(("FortWeaponRangedItemDefinition /Game/Athena/Items/Weapons/WID_Assault_AutoHigh_Athena_SR_Ore_T03.WID_Assault_AutoHigh_Athena_SR_Ore_T03"));

        Inventory::CreateAndAddItem(PlayerController, PickaxeDef, EFortQuickBars::Primary, 0, 1);
        Inventory::CreateAndAddItem(PlayerController, GoldAR, EFortQuickBars::Primary, 1, 1);

        if (FnVerDouble < 9.10)
        {
            static auto GoldPump = FindObject(("FortWeaponRangedItemDefinition /Game/Athena/Items/Weapons/WID_Shotgun_Standard_Athena_SR_Ore_T03.WID_Shotgun_Standard_Athena_SR_Ore_T03"));
            static auto HeavySniper = FindObject(("FortWeaponRangedItemDefinition /Game/Athena/Items/Weapons/WID_Sniper_Heavy_Athena_SR_Ore_T03.WID_Sniper_Heavy_Athena_SR_Ore_T03"));

            Inventory::CreateAndAddItem(PlayerController, GoldPump, EFortQuickBars::Primary, 2, 1);
            Inventory::CreateAndAddItem(PlayerController, HeavySniper, EFortQuickBars::Primary, 3, 1);
        }
        else
        {
            static auto BurstSMG = FindObject(("FortWeaponRangedItemDefinition /Game/Athena/Items/Weapons/WID_Pistol_BurstFireSMG_Athena_R_Ore_T03.WID_Pistol_BurstFireSMG_Athena_R_Ore_T03"));
            static auto CombatShotgun = FindObject(("FortWeaponRangedItemDefinition /Game/Athena/Items/Weapons/WID_Shotgun_Combat_Athena_SR_Ore_T03.WID_Shotgun_Combat_Athena_SR_Ore_T03"));

            Inventory::CreateAndAddItem(PlayerController, CombatShotgun, EFortQuickBars::Primary, 2, 1);
            Inventory::CreateAndAddItem(PlayerController, BurstSMG, EFortQuickBars::Primary, 3, 1);
        }

        Inventory::CreateAndAddItem(PlayerController, Minis, EFortQuickBars::Primary, 4, 1);
        Inventory::CreateAndAddItem(PlayerController, SlurpJuice, EFortQuickBars::Primary, 5, 1);

        Inventory::GiveAllAmmo(PlayerController);
        Inventory::GiveStartingItems(PlayerController); // Gives the needed items like edit tool and builds
        Inventory::GiveMats(PlayerController);
    }

    if (std::stod(FN_Version) >= 7.40)
    {
        static auto world = Helper::GetWorld();
        static auto gameState = *world->Member<UObject*>(("GameState"));

        auto TeamIndex_ = 1;
        auto AllTeams = gameState->Member<TArray<UObject*>>(("Teams"));

        std::cout << "AllTeams: " << AllTeams << '\n';

        static auto teaminfclass = FindObject("Class /Script/FortniteGame.FortTeamInfoAthena");
        static auto teaminfo = Easy::SpawnActor(teaminfclass);

        std::cout << "teaminfo: " << teaminfo << '\n';

        static bool bDidAdd = false;

        if (!bDidAdd && AllTeams)
        {
            std::cout << "Adding\n";
            // AllTeams->Add(teaminfo); // idfk started crashing
            bDidAdd = true;
            std::cout << "Added to gamestate teams!\n";
        }

        static auto privteaminfclass = FindObject("Class /Script/FortniteGame.FortTeamPrivateInfo");
        static auto privTeamInfo = FindObject(".FortTeamPrivateInfo_"); // Easy::SpawnActor(privteaminfclass);

        std::cout << "privTeamInfo: " << privTeamInfo << '\n';

        // if (AllTeams)
           // std::cout << "AllTeams Num: " << AllTeams->Num() << '\n';

        auto TeamIndex = PlayerState->Member<uint8_t>(("TeamIndex")); // AllTeams.Num();
        auto oldTeamIndex = *TeamIndex;
        std::cout << "OldTeamIndex: " << oldTeamIndex << '\n';

        *TeamIndex = TeamIndex_;

        static int intdababy = 1;

        *PlayerState->Member<uint8_t>(("SquadId")) = intdababy;

        std::cout << "Squad Id: " << intdababy << '\n';

        struct FPrivateTeamDataItem : FFastArraySerializerItem {
            float Value; // 0x0c(0x04)
            char PlayerId[0x28];
            char pad_38[0x8]; // 0x38(0x08)
        };

        struct FPrivateTeamDataArray : public FFastArraySerializerSE
        {
        public:
            TArray<FPrivateTeamDataItem>          Items;
            uint8_t                               Pad_373D[0x68];
        };

        auto TeamDataArr = FPrivateTeamDataArray();
        auto TeamDataItem = FPrivateTeamDataItem();

        if (privTeamInfo)
        {
            *privTeamInfo->Member<UObject*>(_("TeamInfo")) = teaminfo;

            // idk whats wrong
            // if (false)
            {

                auto RepData = privTeamInfo->Member<FPrivateTeamDataArray>("RepData");
                *RepData = TeamDataArr;
                std::cout << "RepData: " << RepData << '\n';

                static auto ItemsOffset = FindOffsetStruct("ScriptStruct /Script/FortniteGame.PrivateTeamDataArray", "Items");

                FPrivateTeamDataItem currentItem = TeamDataItem;
                std::cout << "currentitem: " << &currentItem << '\n';

                currentItem.Value = intdababy; // ???
                static auto PrivateTeamDataItemClass = FindObject("ScriptStruct /Script/FortniteGame.PrivateTeamDataItem");
                std::cout << "Sizeof PrivateTeamIdataitem: " << GetSizeOfStruct(PrivateTeamDataItemClass) << '\n';
                static auto PlayerIdOffset = FindOffsetStruct("ScriptStruct /Script/FortniteGame.PrivateTeamDataItem", "PlayerID");

                std::cout << "PlayerIdoffset: " << PlayerIdOffset << '\n';

                // i dont think this is right
                *(__int64*)(__int64(&currentItem) + PlayerIdOffset) = *PlayerState->Member<__int64>(("UniqueId"));

                auto Items = (TArray<FPrivateTeamDataItem>*)(__int64(&*RepData) + ItemsOffset); // &RepData->Items;

                std::cout << "ITems: " << Items << '\n';

                if (Items)
                {
                    Items->Add(currentItem);
                    MarkArrayDirty(RepData);
                    MarkItemDirty(RepData, &currentItem);
                }

                static auto OnRep_RepData = privTeamInfo->Function(("OnRep_RepData"));

                if (OnRep_RepData)
                    privTeamInfo->ProcessEvent(OnRep_RepData);

                std::cout << "did funn\n";
            }
        }

        ++intdababy;

        *PlayerState->Member<ETeamMemberState>(("TeamMemberState")) = ETeamMemberState::None;
        *PlayerState->Member<ETeamMemberState>(("ReplicatedTeamMemberState")) = ETeamMemberState::None;

        auto PlayerTeam = PlayerState->Member<UObject*>(("PlayerTeam"));

        *PlayerTeam = teaminfo; // ->At(*TeamIndex); // TArray<class AFortTeamInfo*>
        std::cout << "PlayerTeam: " << PlayerTeam << '\n';
        std::cout << "PlayerTeam DeRef: " << *PlayerTeam << '\n';

        if (PlayerTeam && *PlayerTeam)
            std::cout << "PlayerTeam Name: " << (*PlayerTeam)->GetFullName() << '\n';

        // *(*PlayerTeam)->Member<uint8_t>(_("Team")) = *TeamIndex;

        *(*PlayerTeam)->Member<UObject*>(("PrivateInfo")) = privTeamInfo;
        auto PrivateInfo = (*PlayerTeam)->Member<UObject*>(("PrivateInfo"));

        if (PrivateInfo && *PrivateInfo)
            *PlayerState->Member<UObject*>(("PlayerTeamPrivate")) = *PrivateInfo;
        else
            std::cout << "Invalid PrivateInfo!\n";

        static auto OnRep_ChangeTeamInfo = PlayerState->Function(_("OnRep_ChangeTeamInfo"));

        if (OnRep_ChangeTeamInfo)
            PlayerState->ProcessEvent(OnRep_ChangeTeamInfo);
        else
            std::cout << ("Unable to find OnRep_ChangeTeamInfo!\n");

        static auto OnRepSquadIdFn = PlayerState->Function(("OnRep_SquadId"));

        if (OnRepSquadIdFn)
            PlayerState->ProcessEvent(OnRepSquadIdFn);
        else
            std::cout << ("Unable to find OnRep_SquadId!\n");

        if (PlayerTeam && *PlayerTeam)
        {
            (*PlayerTeam)->Member<TArray<UObject*>>(("TeamMembers"))->Add(PlayerController);
            static auto OnRepPlayerFn = PlayerState->Function(("OnRep_PlayerTeam"));

            if (OnRepPlayerFn)
                PlayerState->ProcessEvent(OnRepPlayerFn);
            else
                std::cout << ("Unable to find OnRepPlayerFn!\n");
        }

        // onrepplayerteamprivate

        static auto OnRep_TeamIndex = PlayerState->Function(("OnRep_TeamIndex"));

        unsigned char OldVal = oldTeamIndex;

        if (OnRep_TeamIndex)
            PlayerState->ProcessEvent(OnRep_TeamIndex, &OldVal);
        else
            std::cout << ("Unable to find OnRep_TeamIndex!\n");
    }

    std::cout << ("Spawned Player!\n");

    return PlayerController;
}

static bool __fastcall HasClientLoadedCurrentWorldDetour(UObject* pc)
{
    std::cout << ("yes\n");
    return true;
}

void* NetDebugDetour(UObject* idk)
{
    // std::cout << "NetDebug!\n";
    return nullptr;
}

char __fastcall malformedDetour(__int64 a1, __int64 a2)
{
    /* 9.41:
    
    v20 = *(_QWORD *)(a1 + 48);
    *(double *)(a1 + 0x430) = v19;
    if ( v20 )
      (*(void (__fastcall **)(__int64))(*(_QWORD *)v20 + 0xC90i64))(v20);// crashes

    */

    auto old = *(__int64*)(a1 + 48);
    std::cout << "Old: " << old << '\n';
    *(__int64*)(a1 + 48) = 0;
    std::cout << "Old2: " << old << '\n';
    auto ret = malformed(a1, a2);
    *(__int64*)(a1 + 48) = old;
    return ret;
}

void World_NotifyControlMessageDetour(UObject* World, UObject* Connection, uint8_t MessageType, __int64* Bunch)
{
    std::cout << ("Receieved control message: ") << std::to_string((int)MessageType) << '\n';

    static const auto FnVerDouble = std::stod(FN_Version);

    switch (MessageType)
    {
    case 0:
    {
        //S5-6 Fix
        if (Engine_Version == 421) // not the best way to fix it...
        {
            {
                /* uint8_t */ char IsLittleEndian = 0;
                unsigned int RemoteNetworkVersion = 0;
                auto v38 = (__int64*)Bunch[1];
                FString EncryptionToken; // This should be a short* but ye

                if ((unsigned __int64)(*v38 + 1) > v38[1])
                    (*(void(__fastcall**)(__int64*, char*, __int64))(*Bunch + 72))(Bunch, &IsLittleEndian, 1);
                else
                    IsLittleEndian = *(char*)(*v38)++;
                auto v39 = Bunch[1];
                if ((unsigned __int64)(*(__int64*)v39 + 4) > *(__int64*)(v39 + 8))
                {
                    (*(void(__fastcall**)(__int64*, unsigned int*, __int64))(*Bunch + 72))(Bunch, &RemoteNetworkVersion, 4);
                    if ((*((char*)Bunch + 41) & 0x10) != 0)
                        Idkf(__int64(Bunch), __int64(&RemoteNetworkVersion), 4);
                }
                else
                {
                    RemoteNetworkVersion = **(int32_t**)v39;
                    *(__int64*)v39 += 4;
                }

                ReceiveFString(Bunch, EncryptionToken);
                // if (*((char*)Bunch + 40) < 0)
                    // do stuff

                std::cout << ("EncryptionToken: ") << __int64(EncryptionToken.Data.GetData()) << '\n';

                if (EncryptionToken.Data.GetData())
                    std::cout << ("EncryptionToken Str: ") << EncryptionToken.ToString() << '\n';

                std::cout << ("Sending challenge!\n");
                SendChallenge(World, Connection);
                std::cout << ("Sent challenge!\n");
                // World_NotifyControlMessage(World, Connection, MessageType, (void*)Bunch);
                // std::cout << "IDK: " << *((char*)Bunch + 40) << '\n';
                return;
            }
        }
        break;
    }
    case 4: // NMT_Netspeed // Do we even have to rei,plment this?
        // if (Engine_Version >= 423)
          //  *Connection->Member<int>(("CurrentNetSpeed")) = 60000; // sometimes 60000
        // else
        *Connection->Member<int>(("CurrentNetSpeed")) = 30000; // sometimes 60000
        return;
    case 5: // NMT_Login
    {
        if (Engine_Version >= 420)
        {
            Bunch[7] += (16 * 1024 * 1024);

            FString OnlinePlatformName;

            static double CurrentFortniteVersion = std::stod(FN_Version);

            if (CurrentFortniteVersion >= 7 && Engine_Version < 424)
            {
                ReceiveFString(Bunch, *(FString*)(__int64(Connection) + 400)); // clientresponse
                ReceiveFString(Bunch, *(FString*)(__int64(Connection) + 424)); // requesturl
            }
            else if (CurrentFortniteVersion < 7)
            {
                ReceiveFString(Bunch, *(FString*)((__int64*)Connection + 51));
                ReceiveFString(Bunch, *(FString*)((__int64*)Connection + 54));
            }
            else if (Engine_Version >= 424)
            {
                ReceiveFString(Bunch, *(FString*)(__int64(Connection) + 416));
                ReceiveFString(Bunch, *(FString*)(__int64(Connection) + 440));
            }

            ReceiveUniqueIdRepl(Bunch, Connection->Member<__int64>(("PlayerID")));
            std::cout << ("Got PlayerID!\n");
            // std::cout << "PlayerID => " + *Connection->Member<__int64>(("PlayerID")) << '\n';
            // std::cout << "PlayerID => " + std::to_string(*UniqueId) << '\n';
            ReceiveFString(Bunch, OnlinePlatformName);

            std::cout << ("Got OnlinePlatformName!\n");

            if (OnlinePlatformName.Data.GetData())
                std::cout << ("OnlinePlatformName => ") << OnlinePlatformName.ToString() << '\n';

            Bunch[7] -= (16 * 1024 * 1024);

            WelcomePlayer(Helper::GetWorld(), Connection);

            return;
        }
        break;
    }
    case 6:
        return;
    case 9: // NMT_Join
    {
        if (Engine_Version == 421)
        {
            auto ConnectionPC = Connection->Member<UObject*>(("PlayerController"));
            if (!*ConnectionPC)
            {
                FURL InURL;
                /* std::cout << "Calling1!\n";
                std::wcout << L"RequestURL Data: " << Connection->RequestURL.Data << '\n';
                InURL = o_FURL_Construct(InURL, NULL, Connection->RequestURL.Data, ETravelType::TRAVEL_Absolute);
                std::cout << "Called2!\n";
                std::cout << "URL2 Map: " << InURL.Map.ToString() << '\n'; */

                // FURL InURL(NULL, Connection->RequestURL, ETravelType::TRAVEL_Absolute);

                InURL.Map.Set(L"/Game/Maps/Frontend");
                InURL.Valid = 1;
                InURL.Protocol.Set(L"unreal");
                InURL.Port = 7777;

                // ^ This was all taken from 3.5

                FString ErrorMsg;
                SpawnPlayActorDetour(World, Connection, ENetRole::ROLE_AutonomousProxy, InURL, Connection->Member<void>(("PlayerID")), ErrorMsg, 0);

                if (!*ConnectionPC)
                {
                    std::cout << ("Failed to spawn PlayerController! Error Msg: ") << ErrorMsg.ToString() << "\n";
                    // TODO: Send NMT_Failure and FlushNet
                    return;
                }
                else
                {
                    std::cout << ("Join succeeded!\n");
                    FString LevelName;
                    const bool bSeamless = true; // If this is true and crashes the client then u didn't remake NMT_Hello or NMT_Join correctly.

                    static auto ClientTravelFn = (*ConnectionPC)->Function(("ClientTravel"));
                    struct {
                        const FString& URL;
                        ETravelType TravelType;
                        bool bSeamless;
                        FGuid MapPackageGuid;
                    } paramsTravel{ LevelName, ETravelType::TRAVEL_Relative, bSeamless, FGuid() };

                    // ClientTravel(*ConnectionPC, LevelName, ETravelType::TRAVEL_Relative, bSeamless, FGuid());

                    (*ConnectionPC)->ProcessEvent(("ClientTravel"), &paramsTravel);

                    std::cout << ("Traveled client.\n");

                    *(int32_t*)(__int64(&*Connection->Member<int>(("LastReceiveTime"))) + (sizeof(double) * 4) + (sizeof(int32_t) * 2)) = 0; // QueuedBits
                }
                return;
            }
        }
        break;
    }
    }

    return World_NotifyControlMessage(Helper::GetWorld(), Connection, MessageType, Bunch);
}

void Beacon_NotifyControlMessageDetour(UObject* Beacon, UObject* Connection, uint8_t MessageType, __int64* Bunch)
{
    return World_NotifyControlMessageDetour(Helper::GetWorld(), Connection, MessageType, Bunch);
}

char __fastcall NoReserveDetour(__int64* a1, __int64 a2, char a3, __int64* a4)
{
    std::cout << ("No Reserve!\n");
    return 0;
}

UObject* __fastcall CreateNetDriver_LocalDetour(UObject* Engine, __int64 a2, FName NetDriverDefinition)
{
    UObject* ReturnVal = nullptr; // UNetDriver
    // FNetDriverDefinition* Definition = nullptr;

    // if (Definition != nullptr)
    ReturnVal = *Helper::GetWorld()->Member<UObject*>(("NetDriver"));
    if (false)
    {
        // UClass* NetDriverClass = StaticLoadClass(UNetDriver::StaticClass(), nullptr, *Definition->DriverClassName.ToString(), nullptr, LOAD_Quiet);

        static auto NetDriverClass = FindObject(("Class /Script/Engine.NetDriver"));

        // if it fails, then fall back to standard fallback
        /* if (NetDriverClass == nullptr || !NetDriverClass->GetDefaultObject<UNetDriver>()->IsAvailable())
        {
            NetDriverClass = StaticLoadClass(NetDriverClass, nullptr, *Definition->DriverClassNameFallback.ToString(),
                nullptr, LOAD_None);
        } */

        if (NetDriverClass)
        {
            ReturnVal = Easy::SpawnObject(NetDriverClass, Engine);// NewObject<UNetDriver>(GetTransientPackage(), NetDriverClass);

            if (ReturnVal)
            {
                ReturnVal->NamePrivate = FName(282); // GamenetDriver
                // ReturnVal->SetNetDriverName(ReturnVal->NamePrivate); // We luckily don't have to do this since CreateNamedNetDriver redoes it!

                // new(Context.ActiveNetDrivers) FNamedNetDriver(ReturnVal, Definition);
            }
        }
    }


    if (!ReturnVal)
    {
        std::cout << ("Failed to create netdriver!\n");
        // UE_LOG(LogNet, Log, TEXT("CreateNamedNetDriver failed to create driver from definition %s"), *NetDriverDefinition.ToString());
    }

    return ReturnVal;
}

__int64 CollectGarbageDetour(__int64) { return 0; }

bool TryCollectGarbageHook()
{
    return 0;
}

int __fastcall ReceivedPacketDetour(__int64* a1, LARGE_INTEGER a2, char a3)
{
}

void InitializeNetHooks()
{
    static const auto FnVerDouble = std::stod(FN_Version);

    MH_CreateHook((PVOID)SpawnPlayActorAddr, SpawnPlayActorDetour, (void**)&SpawnPlayActor);
    MH_EnableHook((PVOID)SpawnPlayActorAddr);

    MH_CreateHook((PVOID)Beacon_NotifyControlMessageAddr, Beacon_NotifyControlMessageDetour, (void**)&Beacon_NotifyControlMessage);
    MH_EnableHook((PVOID)Beacon_NotifyControlMessageAddr);

    MH_CreateHook((PVOID)World_NotifyControlMessageAddr, World_NotifyControlMessageDetour, (void**)&World_NotifyControlMessage);
    MH_EnableHook((PVOID)World_NotifyControlMessageAddr);

    if (Engine_Version < 424 && GetNetModeAddr) // i dont even think we have to hook this
    {
        MH_CreateHook((PVOID)GetNetModeAddr, GetNetModeDetour, (void**)&GetNetMode);
        MH_EnableHook((PVOID)GetNetModeAddr);
    }
    
    if (Engine_Version > 423)
    {
        MH_CreateHook((PVOID)ValidationFailureAddr, ValidationFailureDetour, (void**)&ValidationFailure);
        MH_EnableHook((PVOID)ValidationFailureAddr);
    }

    MH_CreateHook((PVOID)TickFlushAddr, TickFlushDetour, (void**)&TickFlush);
    MH_EnableHook((PVOID)TickFlushAddr);

    if (KickPlayer)
    {
        MH_CreateHook((PVOID)KickPlayerAddr, KickPlayerDetour, (void**)&KickPlayer);
        MH_EnableHook((PVOID)KickPlayerAddr);
    }

    if (LP_SpawnPlayActorAddr)
    {
        MH_CreateHook((PVOID)LP_SpawnPlayActorAddr, LP_SpawnPlayActorDetour, (void**)&LP_SpawnPlayActor);
        MH_EnableHook((PVOID)LP_SpawnPlayActorAddr);
    }

    if (NoReserveAddr)
    {
        MH_CreateHook((PVOID)NoReserveAddr, NoReserveDetour,  (void**)&NoReserve);
        MH_EnableHook((PVOID)NoReserveAddr);
    }

    // if (NetDebug)
    {
        if (NetDebugAddr)
        {
            MH_CreateHook((PVOID)NetDebugAddr, NetDebugDetour, (void**)&NetDebug);
            MH_EnableHook((PVOID)NetDebugAddr);
        }

        if (CollectGarbageAddr)
        {
            // if (Engine_Version < 424)
            {
                if (Engine_Version >= 422 && Engine_Version < 424)
                {
                    MH_CreateHook((PVOID)CollectGarbageAddr, TryCollectGarbageHook, nullptr);
                    MH_EnableHook((PVOID)CollectGarbageAddr);
                }
                else
                {
                    MH_CreateHook((PVOID)CollectGarbageAddr, CollectGarbageDetour, (void**)&CollectGarbage);
                    MH_EnableHook((PVOID)CollectGarbageAddr);
                }
            }
        }
        else
            std::cout << ("[WARNING] Unable to hook CollectGarbage!\n");
    }
}