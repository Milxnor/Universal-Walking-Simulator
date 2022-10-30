#pragma once

// TODO: Update ImGUI

#include <Windows.h>
#include <dxgi.h>
#include <d3d11.h>
#include <d3d9.h>

#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_dx9.h>

#include <string>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_stdlib.h>
#include "fontawesome.h"
#include <vector>
#include <format>
#include <imgui/imgui_internal.h>
#include <Gameplay/helper.h>
#include <Gameplay/events.h>
#include <hooks.h>
#include "Gameplay/inventory.h"
#include <AI.h>
#include <Gameplay/harvesting.h>
#include <set>
#include <Gameplay/carmine.h>

#define GAME_TAB 1
#define PLAYERS_TAB 2
#define GAMEMODE_TAB 3
#define THANOS_TAB 4
#define EVENT_TAB 5
#define LATEGAME_TAB 6
#define DUMP_TAB 7
#define SETTINGS_TAB 8
#define CREDITS_TAB 9

// THE BASE CODE IS FROM IMGUI GITHUB

static LPDIRECT3D9              g_pD3D = NULL;
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

std::string wstring_to_utf8(const std::wstring& str)
{
	if (str.empty()) return {};
	const auto size_needed = WideCharToMultiByte(CP_UTF8, 0, &str[0], static_cast<int>(str.size()), nullptr, 0, nullptr, nullptr);
	std::string str_to(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &str[0], static_cast<int>(str.size()), &str_to[0], size_needed, nullptr, nullptr);
	return str_to;
}

void InitStyle()
{
	ImGui::GetIO().Fonts->AddFontFromFileTTF("Reboot Resources/fonts/ruda-bold.ttf", 17);
	ImGui::GetStyle().FrameRounding = 4.0f;
	ImGui::GetStyle().GrabRounding = 4.0f;

	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.36f, 0.42f, 0.47f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.12f, 0.20f, 0.28f, 1.00f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.12f, 0.14f, 0.65f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.39f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.18f, 0.22f, 0.25f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.09f, 0.21f, 0.31f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.37f, 0.61f, 1.00f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.20f, 0.25f, 0.29f, 0.55f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	colors[ImGuiCol_Tab] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

class Playera
{
public:
	std::string Name;
	int Kills = 0;

	Playera(const std::string& _Name, int _Kills) : Name(_Name), Kills(_Kills) {}

	Playera() {}
};

void TextCentered(std::string text, bool bNewLine = true) {
	if (bNewLine)
		ImGui::NewLine();

	float win_width = ImGui::GetWindowSize().x;
	float text_width = ImGui::CalcTextSize(text.c_str()).x;

	// calculate the indentation that centers the text on one line, relative
	// to window left, regardless of the `ImGuiStyleVar_WindowPadding` value
	float text_indentation = (win_width - text_width) * 0.5f;

	// if text is too long to be drawn on one line, `text_indentation` can
	// become too small or even negative, so we check a minimum indentation
	float min_indentation = 20.0f;
	if (text_indentation <= min_indentation) {
		text_indentation = min_indentation;
	}

	ImGui::SameLine(text_indentation);
	ImGui::PushTextWrapPos(win_width - text_indentation);
	ImGui::TextWrapped(text.c_str());
	ImGui::PopTextWrapPos();
}

bool ButtonCentered(std::string text, bool bNewLine = true) {
	if (bNewLine)
		ImGui::NewLine();

	float win_width = ImGui::GetWindowSize().x;
	float text_width = ImGui::CalcTextSize(text.c_str()).x;

	// calculate the indentation that centers the text on one line, relative
	// to window left, regardless of the `ImGuiStyleVar_WindowPadding` value
	float text_indentation = (win_width - text_width) * 0.5f;

	// if text is too long to be drawn on one line, `text_indentation` can
	// become too small or even negative, so we check a minimum indentation
	float min_indentation = 20.0f;
	if (text_indentation <= min_indentation) {
		text_indentation = min_indentation;
	}

	ImGui::SameLine(text_indentation);
	ImGui::PushTextWrapPos(win_width - text_indentation);
	auto res = ImGui::Button(text.c_str());
	ImGui::PopTextWrapPos();
	return res;
}

static int Width = 640;
static int Height = 480;

DWORD WINAPI GuiThread(LPVOID)
{
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"RebootClass", NULL };
	::RegisterClassEx(&wc);
	HWND hwnd = ::CreateWindowExW(0L, wc.lpszClassName, L"Project Reboot", (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX), 100, 100, Width, Height, NULL, NULL, wc.hInstance, NULL);

	HANDLE hIcon = LoadImageW(wc.hInstance, L"Reboot Resources/images/reboot.ico", IMAGE_ICON, 48, 48, LR_LOADFROMFILE);
	SendMessageW(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);

	// SetWindowLongPtrW(hwnd, GWL_STYLE, WS_POPUP); // Disables windows title bar at the cost of dragging and some quality

	// Initialize Direct3D
	if (!CreateDeviceD3D(hwnd))
	{
		CleanupDeviceD3D();
		::UnregisterClass(wc.lpszClassName, wc.hInstance);
		return 1;
	}

	// Show the window
	::ShowWindow(hwnd, SW_SHOWDEFAULT);
	::UpdateWindow(hwnd);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	io.IniFilename = NULL; // Disable imgui.ini generation.
	io.DisplaySize = ImGui::GetMainViewport()->Size;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	// io.Fonts->AddFontFromFileTTF("../vendor/fonts/Aller_Bd.ttf", 17);

	// Setup Dear ImGui style
	InitStyle();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX9_Init(g_pd3dDevice);

	// Our state
	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	ImFontConfig config;
	config.MergeMode = true;
	config.GlyphMinAdvanceX = 13.0f; // Use if you want to make the icon monospaced
	static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
	io.Fonts->AddFontFromFileTTF("Reboot Resources/fonts/fontawesome-webfont.ttf", 13.0f, &config, icon_ranges);

	// Main loop
	bool done = false;
	while (!done)
	{
		MSG msg;
		while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
			{
				done = true;
				break;
			}
		}

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		auto WindowSize = ImGui::GetMainViewport()->Size;
		// ImGui::SetNextWindowPos(ImVec2(WindowSize.x * 0.5f, WindowSize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f)); // Center
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);

		tagRECT rect;

		if (GetWindowRect(hwnd, &rect))
		{
			int width = rect.right - rect.left;
			int height = rect.bottom - rect.top;
			ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Always);
		}

		static int Tab = 1;
		static int PlayerTab = -1;
		static bool bInformationTab = false;

		if (!ImGui::IsWindowCollapsed())
		{
			ImGui::Begin(("Project Reboot"), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);

			std::vector<std::pair<UObject*, UObject*>> Players; // Pawn, PlayerState

			auto InitializePlayers = [&Players]() { // TODO: slow
				auto World = Helper::GetWorld();
				if (World)
				{
					auto NetDriver = *World->Member<UObject*>(("NetDriver"));
					if (NetDriver)
					{
						auto ClientConnections = NetDriver->Member<TArray<UObject*>>(("ClientConnections"));

						if (ClientConnections)
						{
							for (int i = 0; i < ClientConnections->Num(); i++)
							{
								auto Connection = ClientConnections->At(i);

								if (!Connection)
									continue;

								auto Controller = *Connection->Member<UObject*>(("PlayerController"));

								if (Controller)
								{
									auto PlayerState = Helper::GetPlayerStateFromController(Controller);
									auto Pawn = Helper::GetPawnFromController(Controller);

									if (Pawn)
									{
										static auto IsActorBeingDestroyed = Pawn->Function(("IsActorBeingDestroyed"));

										bool bIsActorBeingDestroyed = true;

										if (IsActorBeingDestroyed)
											Pawn->ProcessEvent(IsActorBeingDestroyed, &bIsActorBeingDestroyed);

										if (PlayerState && Pawn && !bIsActorBeingDestroyed)
										{
											Players.push_back({
												Pawn,
												PlayerState
												});
										}
									}
								}
							}
						}
					}
				}
			};

			if (ImGui::BeginTabBar(""))
			{
				if (ImGui::BeginTabItem(ICON_FA_GAMEPAD " Game"))
				{
					Tab = GAME_TAB;
					PlayerTab = -1;
					bInformationTab = false;
					ImGui::EndTabItem();
				}

				if (serverStatus == EServerStatus::Up)
				{
					if (ImGui::BeginTabItem(ICON_FA_PEOPLE_CARRY " Players"))
					{
						Tab = PLAYERS_TAB;
						ImGui::EndTabItem();
					}
				}

				if (ImGui::BeginTabItem(("Gamemode")))
				{
					Tab = GAMEMODE_TAB;
					PlayerTab = -1;
					bInformationTab = false;
					ImGui::EndTabItem();
				}

				if (std::floor(FnVerDouble) == 8 || Engine_Version >= 424 || FnVerDouble == 4.1)
				{
					if (ImGui::BeginTabItem(("Thanos")))
					{
						Tab = THANOS_TAB;
						PlayerTab = -1;
						bInformationTab = false;
						ImGui::EndTabItem();
					}
				}

				if (bStarted && Events::HasEvent())
				{
					if (ImGui::BeginTabItem(("Event")))
					{
						Tab = EVENT_TAB;
						PlayerTab = -1;
						bInformationTab = false;
						ImGui::EndTabItem();
					}
				}

				if (bIsLateGame && ImGui::BeginTabItem(("Lategame")))
				{
					Tab = LATEGAME_TAB;
					PlayerTab = -1;
					bInformationTab = false;
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Dump"))
				{
					Tab = DUMP_TAB;
					PlayerTab = -1;
					bInformationTab = false;
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem(("Settings")))
				{
					Tab = SETTINGS_TAB;
					PlayerTab = -1;
					bInformationTab = false;
					ImGui::EndTabItem();
				}

				// maybe a Replication Stats for >3.3?

				if (ImGui::BeginTabItem(("Credits")))
				{
					Tab = CREDITS_TAB;
					PlayerTab = -1;
					bInformationTab = false;
					ImGui::EndTabItem();
				}

				ImGui::EndTabBar();
			}

			if (PlayerTab == -1)
			{
				switch (Tab)
				{
				case GAME_TAB:
				{
					static std::string ConsoleCommand;

					ImGui::Checkbox(("Log RPCS"), &bLogRpcs);
					ImGui::Checkbox(("Log ProcessEvent"), &bLogProcessEvent);
					ImGui::Checkbox("Log SpawnActor", &bPrintSpawnActor);
					ImGui::Checkbox(std::format("Restart {} seconds after someone wins", RestartSeconds).c_str(), &bAutoRestart);
					ImGui::Checkbox("Siphon", &bSiphonEnabled);

					if (FnVerDouble == 19.10)
						ImGui::Checkbox("zoneRet", &zoneRet);

					if (FnVerDouble < 19.00)
						ImGui::Checkbox("Clear Inventory on Aircraft", &bClearInventoryOnAircraftJump);
	
					if (bEmotingEnabled)
						ImGui::Checkbox("bPrintFUnny", &bPrintFUnny);

					if (FnVerDouble < 3)
						ImGui::InputInt("Amount Of Players Per Team", &maxAmountOfPlayersPerTeam);

					if (false)
					{
						auto GameState = Helper::GetGameState();

						if (GameState)
						{
							auto GamePhase = Helper::GetGamePhase();

							if (GamePhase && *GamePhase == EAthenaGamePhase::Warmup)
							{
								static auto AircraftStartTimeOffset = GetOffset(GameState, "AircraftStartTime");

								if (AircraftStartTimeOffset != -1)
								{
									auto AircraftStartTime = (float*)(__int64(GameState) + AircraftStartTimeOffset);
									ImGui::InputFloat("Aircraft Start Time", AircraftStartTime);

									static auto WarmupCountdownEndTimeOffset = GetOffset(GameState, "WarmupCountdownEndTime");
									auto WarmupCountdownEndTime = (float*)(__int64(GameState) + WarmupCountdownEndTimeOffset);

									ImGui::InputFloat("Warmup End Time", AircraftStartTime);
								}
							}
						}
					}

					ImGui::InputText("Console command", &ConsoleCommand);

					if (ImGui::Button("Execute console command"))
					{
						auto wstr = std::wstring(ConsoleCommand.begin(), ConsoleCommand.end());

						FString cmd;
						cmd.Set(wstr.c_str());

						Helper::Console::ExecuteConsoleCommand(cmd);
					}

					// ImGui::Checkbox("boozasdgwq9i", &boozasdgwq9i);

					// if (bDoubleBuildFix2)
						// ImGui::InputInt("funnythingy", &funnythingy);
						
					// ImGui::Checkbox("Save The World", &bIsSTW);
					// ImGui::Checkbox("Log SpawnActor", &bPrintSpawnActor);

					// ImGui::Checkbox(("Use Beacons"), &bUseBeacons);

					if (serverStatus == EServerStatus::Down && !bTraveled)
					{
						// TODO: Map name

						if (ImGui::Button(("Load in Battle Royale")))
						{
							bIsCreative = false;
							LoadInMatch();
						}

						if (FnVerDouble == 7.40 && ImGui::Button(("Load in Creative")))
						{
							PlaylistToUse = "FortPlaylistAthena /Game/Athena/Playlists/Creative/Playlist_PlaygroundV2.Playlist_PlaygroundV2";
							bIsCreative = true;
							LoadInMatch();
						}
					}

					if (FnVerDouble == 12.41 && ImGui::Button("Open All Vaults"))
					{
						FortAI::OpenVaults();
					}
					
					// https://media.discordapp.net/attachments/998297579857137734/1006634482884939807/unknown.png
					if (bAISpawningEnabled && ImGui::Button("Spawn AI"))
					{
						static auto aiPCClass = FindObject("Class /Script/FortniteGame.AthenaAIController");
						auto AiPC = Easy::SpawnActor(aiPCClass);

						auto AIPawn = Helper::InitPawn(AiPC, false, Helper::GetPlayerStart(), false);
						
						TArray<UObject*>* SkillSets = AiPC->Member<TArray<UObject*>>("BotSkillSetClasses");

						if (SkillSets)
						{
							std::cout << "skillSet Num: " << SkillSets->Num() << '\n';

							static auto MovementSkillClass = FindObject("Class /Script/FortniteGame.FortAthenaAIBotMovementSkillSet");
							auto MovementSkill = Easy::SpawnObject(MovementSkillClass, AiPC);

							SkillSets->Add(MovementSkill);
						}
						else
							std::cout << "Invalid SkillSets!\n";

						std::cout << "Setup AI!\n";
					}

					if (/* Engine_Version == 420 && */ ImGui::Button("Summon Floorloot"))
					{
						LootingV2::SummonFloorLoot(nullptr);
					}

					if (false && ImGui::Button("Spawn Vehicles"))
					{
						LootingV2::SpawnVehicles(nullptr);
					}

					if (serverStatus == EServerStatus::Up)
					{
						if (ImGui::Button("Freecam"))
						{
							FString StartAircraftCmd;
							StartAircraftCmd.Set(L"toggledebugcamera");

							Helper::Console::ExecuteConsoleCommand(StartAircraftCmd);
						}

						{
							if (ImGui::Button(("Start Aircraft")))
							{
								Helper::SetGamePhase(EAthenaGamePhase::Warmup);

								auto AircraftStartTime = Helper::GetGameState()->Member<float>(("AircraftStartTime"));

								if (AircraftStartTime)
								{
									auto WillSkipAircraft = Helper::GetGameState()->Member<bool>(("bGameModeWillSkipAircraft"));

									if (WillSkipAircraft)
										*WillSkipAircraft = true;

									*AircraftStartTime = 0;
									*Helper::GetGameState()->Member<float>(("WarmupCountdownEndTime")) = 0;
								}
								else
								{
									FString StartAircraftCmd;
									StartAircraftCmd.Set(L"startaircraft");

									Helper::Console::ExecuteConsoleCommand(StartAircraftCmd);
								}

								if (Helper::IsSmallZoneEnabled())
								{
									if (AircraftStartTime)
										Sleep(1000); // stupid

									auto gameState = Helper::GetGameState();

									auto Aircrafts = gameState->Member<TArray<UObject*>>(("Aircrafts"));

									if (Aircrafts && Aircrafts->IsValid())
									{
										auto Aircraft = Aircrafts->At(0);

										if (Aircraft)
										{
											auto FlightInfo = Aircraft->Member<__int64>("FlightInfo");

											if (FlightInfo)
											{
												static auto FlightSpeedOffset = FindOffsetStruct("ScriptStruct /Script/FortniteGame.AircraftFlightInfo", "FlightSpeed");
												static auto FlightStartLocationOffset = FindOffsetStruct("ScriptStruct /Script/FortniteGame.AircraftFlightInfo", "FlightStartLocation");

												auto FlightSpeed = (float*)(__int64(FlightInfo) + FlightSpeedOffset);
												auto FlightStartLocation = (FVector*)(__int64(FlightInfo) + FlightStartLocationOffset);

												if (FlightSpeed)
													*FlightSpeed = 0;

												auto RandomFoundation = Helper::GetRandomFoundation();

												if (RandomFoundation)
												{
													AircraftLocationToUse = Helper::GetActorLocation(RandomFoundation) + FVector{0, 0, 10000};

													*FlightStartLocation = AircraftLocationToUse;
													Helper::SetActorLocation(Aircraft, AircraftLocationToUse);
													std::cout << std::format("Set aircraft location to {} {} {}\n", AircraftLocationToUse.X, AircraftLocationToUse.Y, AircraftLocationToUse.Z);
												}
												else
													std::cout << "No POI!\n";

												struct wtf {
													uint8_t ahh : 1;
													uint8_t ahh4 : 1;
													uint8_t ahh2 : 1;
													uint8_t ahh3 : 1;
												};

												gameState->Member<wtf>("bAircraftIsLocked")->ahh = false;

												FString ifrogor;
												ifrogor.Set(L"startsafezone");
												Helper::Console::ExecuteConsoleCommand(ifrogor);
												*gameState->Member<float>("SafeZonesStartTime") = 0.f;
											}
										}
										else
											std::cout << "No Aircraft!\n";
									}
								}

								std::cout << ("Started aircraft!\n");

								/* static auto BuildingSMActorClass = FindObject(("Class /Script/FortniteGame.BuildingSMActor"));

								// Helper::DestroyAll(BuildingSMActorClass);

								ExistingBuildings.clear(); */
							}
						}
						
						// else
						{
						}

						if (ImGui::Button(("Summon Llamas")))
						{
							// CreateThread(0, 0, Looting::Tables::SpawnLlamas, 0, 0, 0);
						}

						if (ImGui::Button("Fill Vending Machines"))
						{
							CreateThread(0, 0, LootingV2::FillVendingMachines, 0, 0, 0);
						}
					}

					/* if (ImGui::Button("idfk2"))
					{
						Helper::DestroyActor(FindObjectOld("B_BaseGlider_C /Game/Athena/Maps/Athena_Terrain.Athena_Terrain.PersistentLevel.B_BaseGlider_C_"));
					} */

					if (ImGui::Button("SKid"))
					{
						static auto BGAConsumableSpawnerClass = FindObject("Class /Script/FortniteGame.BGAConsumableSpawner");
						auto ConsumableClass = StaticLoadObject(Helper::GetBGAClass(), nullptr, "/Game/Athena/Items/ForagedItems/Rift/BGA_RiftPortal_Athena.BGA_RiftPortal_Athena_C");

						std::cout << "ConsumableClass: " << ConsumableClass << '\n';

						if (ConsumableClass)
							std::cout << "ConsumableClass Name: " << ConsumableClass->GetFullName() << '\n';
					}

					if (ImGui::Button("bbb"))
					{
						LootingV2::SpawnForagedItems();
					}

					if (false && ImGui::Button("ee"))
					{
						InitializeHarvestingHooks();
					}

					if (false && ImGui::Button("idfk"))
					{
						auto scripting = FindObjectOld("BP_IslandScripting_C /Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.BP_IslandScripting3", true);
						std::cout << "scripting: " << scripting << '\n';
						
						if (scripting)
						{
							// *scripting->Member<FVector>("IslandPosition") = FVector{ 1250, 1818, 3284 };
							// scripting->ProcessEvent("LoadDynamicLevels");
							// void BindCalendarEvents();
							/*
							
								void TrySetMapMarker();
								void SetupMapMarker();
								void TrySetIslandLocation();
							
							*/
						}
					}

					/* if (ImGui::Button(("Summon FloorLoot")))
					{
						CreateThread(0, 0, Looting::Tables::SpawnFloorLoot, 0, 0, 0);
					} */

					/*
					
					if (Engine_Version >= 423 && bStarted)
					{
						if (ImGui::Button("Spawn Vehicles"))
						{
							CreateThread(0, 0, Looting::Tables::SpawnVehicles, 0, 0, 0);
						}
					}

					*/

					if (/* bStarted && */ ImGui::Button("Restart"))
					{
						Restart();
					}

					if (bAISpawningEnabled && ImGui::Button(("SpawnHenchmans"))) {
						FortAI::SpawnHenchmans();
					}

					if (false && Engine_Version >= 422 && Engine_Version < 424) 
					{
						if (ImGui::Button("Spawn Volume (stay in 1 place to get creative inventory)"))
						{
							auto World = Helper::GetWorld();
							auto NetDriver = *World->Member<UObject*>(("NetDriver"));

							auto ClientConnections = NetDriver->Member<TArray<UObject*>>(("ClientConnections"));

							for (int i = 0; i < ClientConnections->Num(); i++)
							{
								auto Connection = ClientConnections->At(i);

								auto Controller = *Connection->Member<UObject*>(("PlayerController"));

								auto Pawn = *Controller->Member<UObject*>(("Pawn"));
								if (Pawn)
								{
									auto Location = Helper::GetActorLocation(Pawn);
									static auto VolumeClass = FindObject("Class /Script/FortniteGame.FortVolume");
									auto Volume = Easy::SpawnActor(VolumeClass, Location);
								}
							}
						}
					}

					break;
				}
				case PLAYERS_TAB:
					// ImGui::Text("Players Connected: ")
					InitializePlayers();

					for (int i = 0; i < Players.size(); i++)
					{
						auto& Player = Players[i];
						auto PlayerState = Player.second;

						if (!Player.first || !PlayerState)
							continue;

						// if (ImGui::Button(Helper::GetfPlayerName(PlayerState).ToString().c_str()))
						auto wstr = std::wstring(Helper::GetfPlayerName(Helper::GetControllerFromPawn(Player.first)).Data.GetData());

						if (ImGui::Button(std::string(wstr.begin(), wstr.end()).c_str()))
						{
							PlayerTab = i;
						}
					}

					break;
				case GAMEMODE_TAB:
				{
					if (!bStarted && !bIsCreative)
						ImGui::InputText(("Playlist"), &PlaylistToUse);
		
					if (ImGui::Checkbox(("Playground"), &bIsPlayground))
					{

					}

					// if (!bStarted) // couldnt we wqait till aircraft start

					if (!bIsPlayground && Engine_Version < 424)
						ImGui::Checkbox(("Lategame"), &bIsLateGame);

					break;
				}

				case THANOS_TAB:
					if (bStarted == true) {
						if (FnVerDouble >= 8.51 && FnVerDouble < 11.0 && ImGui::Button(("Init Ashton"))) {
							Ashton::InitAshton();
						}
						if (FnVerDouble >= 8.51 && FnVerDouble < 11.0 && ImGui::Button(("Spawn Stone"))) {
							Ashton::SpawnRandomStone();
						}

						if (FnVerDouble == 4.1 && ImGui::Button(("Init Carmine"))) {
							Carmine::InitCarmine();
						}
						if (FnVerDouble == 4.1 && ImGui::Button(("Spawn Gauntlet"))) {
							Carmine::SpawnGauntlet();
						}
					}
					else {
						if (FnVerDouble >= 8.51 && FnVerDouble < 11.0 && ImGui::Button(("Set Playlist"))) {
							PlaylistToUse = "FortPlaylistAthena /Game/Athena/Playlists/Ashton/Playlist_Ashton_Lg.Playlist_Ashton_Lg";
						}
						if (FnVerDouble == 4.1 && ImGui::Button(("Set Playlist"))) {
							PlaylistToUse = "FortPlaylistAthena /Game/Athena/Playlists/Carmine/Playlist_Carmine.Playlist_Carmine";
						}
					}
					break;

				case EVENT_TAB:
					if (ImGui::Button(("Start Event")))
						Events::StartEvent();

					if (ImGui::Button("Initialize Event Hooks"))
						InitializeEventHooks();

					if (FnVerDouble == 8.51) {
						ImGui::InputText("Item to Unvault (DrumGun, Bouncer, Sword, Grappler, Tac)", &EventHelper::UV_ItemName);

						if (ImGui::Button("Unvault Item")) {
							FString InStr;
							InStr.Set(std::wstring(EventHelper::UV_ItemName.begin(), EventHelper::UV_ItemName.end()).c_str());
							EventHelper::UnvaultItem(Helper::Conversion::StringToName(InStr));
						}
					}

					if (FnVerDouble == 12.41 && ImGui::Button("Fly players up"))
						EventHelper::BoostUpTravis();

					if (FnVerDouble == 6.21)
					{
						/* if (ImGui::Button("Show Before Event Lake"))
							EventHelper::LoadAndUnloadLake(false);
						if (ImGui::Button("Show After Event Lake"))
							EventHelper::LoadAndUnloadLake(true); */
						if (ImGui::Button("Teleport Players to Butterfly"))
							EventHelper::TeleportPlayersToButterfly();
					}
					break;
				case LATEGAME_TAB:
					if (auto SafeZoneIndicator = Helper::GetSafeZoneIndicator())
					{
						static auto bPausedOffset = FindOffsetStruct("Class /Script/FortniteGame.FortSafeZoneIndicator", "bPaused");
						auto bPaused = (bool*)(__int64(SafeZoneIndicator) + bPausedOffset);

						if (ImGui::Checkbox("Paused", bPaused))
						{
							std::cout << "bPaused: " << *bPaused << '\n';
							static auto bPausedForPreviewOffset = FindOffsetStruct("Class /Script/FortniteGame.FortSafeZoneIndicator", "bPausedForPreview");

							if (bPausedForPreviewOffset != -1)
								*(bool*)(__int64(SafeZoneIndicator) + bPausedForPreviewOffset) = *bPaused;
						}

						static auto NextCenterOffset = FindOffsetStruct("Class /Script/FortniteGame.FortSafeZoneIndicator", "NextCenter");
						auto NextCenter = (FVector*)(__int64(SafeZoneIndicator) + NextCenterOffset);
						ImGui::SliderFloat("Next Center X", &NextCenter->X, 1, 100000);
						ImGui::SliderFloat("Next Center Y", &NextCenter->Y, 1, 100000);

						// static auto RadiusOffset = FindOffsetStruct("Class /Script/FortniteGame.FortSafeZoneIndicator", "Radius");
						// ImGui::SliderFloat("Radius", (float*)(__int64(SafeZoneIndicator) + RadiusOffset), 1, 100000);

						static auto NextRadiusOffset = FindOffsetStruct("Class /Script/FortniteGame.FortSafeZoneIndicator", "NextRadius");
						ImGui::SliderFloat("NextRadius", (float*)(__int64(SafeZoneIndicator) + NextRadiusOffset), 1, 200000);

						static auto SafeZoneFinishShrinkTimeOffset = FindOffsetStruct("Class /Script/FortniteGame.FortSafeZoneIndicator", "SafeZoneFinishShrinkTime");
						ImGui::SliderFloat("SafeZoneFinishShrinkTime", (float*)(__int64(SafeZoneIndicator) + SafeZoneFinishShrinkTimeOffset), 1, 10000);

						static auto SafeZoneStartShrinkTimeOffset = FindOffsetStruct("Class /Script/FortniteGame.FortSafeZoneIndicator", "SafeZoneStartShrinkTime");
						ImGui::SliderFloat("SafeZoneStartShrinkTime", (float*)(__int64(SafeZoneIndicator) + SafeZoneStartShrinkTimeOffset), 1, 10000);
					}
					else
					{
						ImGui::Text("Safezone has not started yet!");

						if (serverStatus == EServerStatus::Up && 
							ImGui::Button("Start zone"))
						{
							FString StartAircraftCmd;
							StartAircraftCmd.Set(L"startsafezone"); // todo: not

							Helper::Console::ExecuteConsoleCommand(StartAircraftCmd);
						}
					}

					break;
				case DUMP_TAB:

					ImGui::Text("The dumped file will be in your Win64 folder (FortniteVersion/FortniteGame/Binaries/Win64).");

					if (ImGui::Button(("Dump Objects (Objects.log)")))
					{
						Helper::DumpObjects(nullptr);
					}

					if (ImGui::Button("Dump Skins (Skins.txt)"))
					{
						std::ofstream SkinsFile("Skins.txt");

						if (SkinsFile.is_open())
						{
							static auto CIDClass = FindObject("Class /Script/FortniteGame.AthenaCharacterItemDefinition");

							auto AllObjects = Helper::GetAllObjectsOfClass(CIDClass);

							for (int i = 0; i < AllObjects.size(); i++)
							{
								auto CurrentCID = AllObjects.at(i);

								static auto DisplayNameOffset = GetOffset(CurrentCID, "DisplayName");
								auto DisplayNameFText = (FText*)(__int64(CurrentCID) + DisplayNameOffset);

								SkinsFile << std::format("[{}] {}\n", Helper::Conversion::TextToString(*DisplayNameFText), CurrentCID->GetFullName());
							}
						}
					}

					if (ImGui::Button("Dump Playlists (Playlists.txt)"))
					{
						std::ofstream PlaylistsFile("Playlists.txt");

						if (PlaylistsFile.is_open())
						{
							PlaylistsFile << "Fortnite Version: " + FN_Version << "\n\n";
							static auto FortPlaylistClass = FindObject("Class /Script/FortniteGame.FortPlaylist");
							// static auto FortPlaylistClass = FindObject("Class /Script/FortniteGame.FortPlaylistAthena");

							for (int32_t i = 0; i < (ObjObjects ? ObjObjects->Num() : OldObjects->Num()); i++)
							{
								auto Object = GetByIndex(i);

								if (Object && Object->IsA(FortPlaylistClass))
								{
									// std::string PlaylistName = Object->Member<FName>("PlaylistName")->ToString(); // Short name basically
									std::string PlaylistName = Helper::Conversion::TextToString(*Object->Member<FText>("UIDisplayName"));

									PlaylistsFile << std::format("[{}] {}\n", PlaylistName, Object->GetFullName());
								}
							}
						}
						else
							std::cout << "Failed to open playlist file!\n";
					}

					if (ImGui::Button("Dump Weapons (Weapons.txt)"))
					{
						std::ofstream WeaponsFile("Weapons.txt");

						if (WeaponsFile.is_open())
						{
							WeaponsFile << "Fortnite Version: " + FN_Version << "\n\n";
							static auto FortWeaponItemDefinitionClass = FindObjectOld("Class /Script/FortniteGame.FortWeaponItemDefinition", true);

							for (int32_t i = 0; i < (ObjObjects ? ObjObjects->Num() : OldObjects->Num()); i++)
							{
								auto Object = GetByIndex(i);

								if (Object && Object->IsA(FortWeaponItemDefinitionClass))
								{
									// std::string PlaylistName = Object->Member<FName>("PlaylistName")->ToString(); // Short name basically
									std::string ItemDefinitionName = Helper::Conversion::TextToString(*Object->Member<FText>("DisplayName"));

									// check if it contains gallery or playset?

									WeaponsFile << std::format("[{}] {}\n", ItemDefinitionName, Object->GetFullName());
								}
							}
						}
						else
							std::cout << "Failed to open playlist file!\n";
					}

					break;
				case SETTINGS_TAB:

					{
						ImGui::InputText("CID", &CIDToUse);
						ImGui::NewLine();
					}

					if (ImGui::InputText("PickaxeDef", &PickaxeDef))
					{
						GlobalPickaxeDefObject = FindObject(PickaxeDef);
						std::cout << "New pickaxedef is " << GlobalPickaxeDefObject ? "valid\n" : "invalid\n";
					}

					if (std::floor(FnVerDouble) > 4)
						ImGui::Checkbox("Random Cosmetics", &bRandomCosmetics);

					ImGui::NewLine();

					ImGui::InputText("First Slot", &StartingSlot1.first);
					ImGui::InputInt("First Slot Amount", &StartingSlot1.second);
					ImGui::NewLine();
					ImGui::InputText("Second Slot", &StartingSlot2.first);
					ImGui::InputInt("Second Slot Amount", &StartingSlot2.second);
					ImGui::NewLine();
					ImGui::InputText("Third Slot", &StartingSlot3.first);
					ImGui::InputInt("Third Slot Amount", &StartingSlot3.second);
					ImGui::NewLine();
					ImGui::InputText("Fourth Slot", &StartingSlot4.first);
					ImGui::InputInt("Fourth Slot Amount", &StartingSlot4.second);
					ImGui::NewLine();
					ImGui::InputText("Fifth Slot", &StartingSlot5.first);
					ImGui::InputInt("Fifth Slot Amount", &StartingSlot5.second);
					ImGui::NewLine();

					break;
				case CREDITS_TAB:
					TextCentered(("Credits:"));
					TextCentered(("Milxnor: Made the base, main developer"));
					TextCentered(("GD: Added events, cleans up code and adds features."));

					break;
				}
			}
			else
			{
				InitializePlayers();
				if (PlayerTab < Players.size())
				{
					auto& CurrentPlayer = Players.at(PlayerTab);

					if (CurrentPlayer.first && CurrentPlayer.second)
					{
						auto Pawn = CurrentPlayer.first;

						if (!bInformationTab)
						{
							static std::string VehicleClass;
							static std::string WID;
							static int Count = 1;
							static int LoadedAmmo = 0;
							static std::string KickReason = "You have been kicked!";

							auto Controller = Helper::GetControllerFromPawn(Pawn);

							auto PlayerName = Helper::GetfPlayerName(Controller);
							ImGui::TextColored(ImVec4(18, 253, 112, 0.8), (("Player: ") + PlayerName.ToString()).c_str());
							ImGui::TextColored(ImVec4(18, 253, 112, 0.8), std::string("Team: " + std::to_string(((int)*Teams::GetTeamIndex(CurrentPlayer.second)) - 2)).c_str());

							// TODO: Add Inventory, let them drop, delete, add, modify, etc.
							if (ImGui::Button(("Game Statistics")))
							{
								bInformationTab = true;
							}

							ImGui::NewLine();

							/* if (ImGui::Button(ICON_FA_HAMMER " Ban"))
							{
								auto IP = Helper::GetfIP(CurrentPlayer.second);
								Helper::Banning::Ban(PlayerName.Data.GetData(), Controller, IP.Data.GetData());
							}  */

							ImGui::InputText("Kick Reason", &KickReason);
							if (ImGui::Button("Kick"))
							{
								std::wstring wstr = std::wstring(KickReason.begin(), KickReason.end());
								FString Reason;
								Reason.Set(wstr.c_str());

								static auto ClientReturnToMainMenu = Controller->Function("ClientReturnToMainMenu");

								if (ClientReturnToMainMenu)
									Controller->ProcessEvent(ClientReturnToMainMenu, &Reason);
							}

							static auto CurrentWeaponOffset = GetOffset(Pawn, "CurrentWeapon");
							auto CurrentWeapon = Pawn ? *(UObject**)(__int64(Pawn) + CurrentWeaponOffset) : nullptr;
							static auto AmmoCountOffset = FindOffsetStruct("Class /Script/FortniteGame.FortWeapon", "AmmoCount");

							static int stud = 0;

							ImGui::InputInt("Ammo Count of CurrentWeapon", CurrentWeapon ? (int*)(__int64(CurrentWeapon) + AmmoCountOffset) : &stud);

							static auto ForceKillFn = FindObject("Function /Script/FortniteGame.FortPawn.ForceKill");

							if (ForceKillFn && ImGui::Button(ICON_FA_CROSSHAIRS " Kill"))
							{
								struct {
									FGameplayTag DeathReason; 
									UObject* KillerController;
									UObject* KillerActor;
								} ForceKill_Params{FGameplayTag(), nullptr, nullptr};

								Pawn->ProcessEvent(ForceKillFn, &ForceKill_Params);
							}

							/* ImGui::InputText(("VehicleClass"), &VehicleClass);
							if (ImGui::Button("Spawn Vehicle"))
							{
								auto Pawn = Controller->Member<UObject*>("Pawn");
								auto vehicleClass = FindObject(VehicleClass);
								auto Location = Helper::GetActorLocation(*Pawn);
								Helper::SpawnVehicle(vehicleClass, Location);
							} */

							// TODO: Add teleport to location

							ImGui::InputText(("WID"), &WID);
							ImGui::InputInt(("Count"), &Count);
							ImGui::InputInt(("Loaded Ammo"), &LoadedAmmo);

							if (ImGui::Button("Spawn Card"))
							{
								FindObjectOld(".GA_Athena_SCMachine_Passive_C_")->ProcessEvent("Spawn");
								// Helper::SpawnChip(Controller, FVector{ 1250, 1818, 3284 });
							}

							if (ImGui::Button(ICON_FA_HAND_HOLDING_USD " Give Weapon"))
							{
								auto wID = FindObject(WID);

								if (wID)
								{
									auto instance = Inventory::GiveItem(Controller, wID, QuickBars::WhatQuickBars(wID), 1, Count);

									if (instance)
									{
										auto entry = GetItemEntryFromInstance(instance);
										*FFortItemEntry::GetLoadedAmmo(entry) = LoadedAmmo;
										Inventory::Update(Controller, -1, true, (FFastArraySerializerItem*)entry);
										// MarkItemDirty(Inventory::GetInventory(Controller), (FFastArraySerializerItem*)entry);
									}
								}
								else
									std::cout << ("Invalid WID! Please make sure it's a valid object.\n");
							}

							if (ImGui::Button("Summon Pickup"))
							{
								auto wID = FindObject(WID);

								if (wID)
								{
									auto pickup = Helper::SummonPickup(nullptr, wID, Helper::GetActorLocation(Pawn), EFortPickupSourceTypeFlag::Container, EFortPickupSpawnSource::Unset
									, 1, true, false);
									// *FFortItemEntry::GetLoadedAmmo(GetItemEntryFromInstance(instance)) = LoadedAmmo;
								}
								else
									std::cout << ("Invalid WID! Please make sure it's a valid object.\n");
							}

							if (ImGui::Button("Spawn Rift"))
							{
								static auto riftPortalClass = FindObject("BlueprintGeneratedClass /Game/Athena/Items/Consumables/RiftItem/BGA_RiftPortal_Item_Athena.BGA_RiftPortal_Item_Athena_C");

								auto newRift = Easy::SpawnActor(riftPortalClass, Helper::GetActorLocation(Pawn));

								if (newRift)
								{
									FVector riftTeleportLocation = Helper::GetActorLocation(newRift);
									riftTeleportLocation.Z += 10000;

									*newRift->Member<FVector>("TeleportLocation") = riftTeleportLocation;
									*newRift->Member<FScalableFloat>("TeleportHeight") = FScalableFloat(100000);
								}
							}

							if (FnVerDouble == 6.21)
							{
								if (ImGui::Button("Spawn Fiend (has no AI)"))
								{
									static auto riftPortalClass = FindObject("BlueprintGeneratedClass /Game/Athena/Deimos/Pawns/Deimos_Fiend.Deimos_Fiend_C");

									auto newRift = Easy::SpawnActor(riftPortalClass, Helper::GetActorLocation(Pawn));
								}

								if (ImGui::Button("Spawn Brute (has no AI)"))
								{
									static auto riftPortalClass = FindObject("BlueprintGeneratedClass /Game/Athena/Deimos/Pawns/Deimos_Brute.Deimos_Brute_C");

									auto newRift = Easy::SpawnActor(riftPortalClass, Helper::GetActorLocation(Pawn));
								}

								if (ImGui::Button("Spawn Deimos Rift"))
								{
									static auto riftPortalClass = FindObject("BlueprintGeneratedClass /Game/Athena/Deimos/Spawners/RiftSpawners/BP_DeimosRift.BP_DeimosRift_C");

									auto newRift = Easy::SpawnActor(riftPortalClass, Helper::GetActorLocation(Pawn));
								}

								if (ImGui::Button("Spawn Dynamic Deimos Rift"))
								{
									static auto riftPortalClass = FindObject("BlueprintGeneratedClass /Game/Athena/Deimos/Spawners/RiftSpawners/BP_DeimosRift_Dynamic.BP_DeimosRift_Dynamic_C");

									auto newRift = Easy::SpawnActor(riftPortalClass, Helper::GetActorLocation(Pawn));

									if (newRift)
									{
										// newRift->ProcessEvent("SpawnEffects");
									}
								}
							}

							if (ImGui::Button("Do Funny"))
							{
								static auto GrapplerPrjClass = FindObject("BlueprintGeneratedClass /Game/Weapons/FORT_Crossbows/Blueprints/B_Prj_Hook_Athena.B_Prj_Hook_Athena_C");

								auto NewPrj = Easy::SpawnActor(GrapplerPrjClass, Helper::GetActorLocation(Pawn));

								static auto RopeClass = FindObject("BlueprintGeneratedClass /Game/Weapons/FORT_Crossbows/Blueprints/B_HookGunRope.B_HookGunRope_C");

								auto RopeActor = Easy::SpawnActor(RopeClass, Helper::GetActorLocation(Pawn));

								// *NewPrj->Member<UObject*>("Rope") = RopeActor;
								// *RopeActor->Member<UObject*>("Projectile_Actor") = NewPrj;
								// *RopeActor->Member<UObject*>("PlayerPawn") = Pawn;
								// *NewPrj->Member<UObject*>("Weapon_Actor") = *Pawn->Member<UObject*>("CurrentWeapon"); // this makes it destroy
								// *NewPrj->Member<UObject*>("PlayerPawn") = Pawn;
							}

							ImGui::NewLine();

							if (ImGui::Button(ICON_FA_BACKWARD " Back"))
							{
								PlayerTab = -1;
								Tab = 2;
								WID = "";
							}
						}
						else
						{
							TextCentered(std::format(("Kills: {}"), *CurrentPlayer.second->Member<int>(("KillScore"))));
							auto PawnLocation = Helper::GetActorLocation(Pawn);
							TextCentered(std::format(("X: {} Y: {} Z: {}"), (int)PawnLocation.X, (int)PawnLocation.Y, (int)PawnLocation.Z)); // We cast to an int because it changes too fast. 

							auto CurrentWeapon = Helper::GetCurrentWeapon(Pawn);

							if (CurrentWeapon)
							{
								auto Guid = Inventory::GetWeaponGuid(CurrentWeapon);

								// if (Guid)
								{
									auto Controller = Helper::GetControllerFromPawn(Pawn);
									UObject* Definition = nullptr;
									int Count = -1;

									if (Controller)
									{
										auto ItemInstances = Inventory::GetItemInstances(Controller);
										UObject* ItemDefinition = nullptr;

										if (ItemInstances)
										{
											for (int j = 0; j < ItemInstances->Num(); j++)
											{
												auto ItemInstance = ItemInstances->At(j);

												if (!ItemInstance)
													continue;

												auto CurrentGuid = Inventory::GetItemGuid(ItemInstance);

												if (CurrentGuid == Guid)
												{
													Definition = Inventory::GetItemDefinition(ItemInstance);
													Count = *FFortItemEntry::GetCount(ItemInstance->Member<__int64>(("ItemEntry"))); // lets hope its a UFortWorldItem*
													// break;
												}
											}

											if (Definition)
											{
												auto Name = Definition->Member<FText>(("DisplayName"));

												if (Name)
												{
													auto DisplayName = Helper::Conversion::TextToString(*Name);
													// auto Rarity = *Definition->Member<EFortRarity>(("Rarity"));

													bool bMultiple = Count > 1;
													TextCentered(std::format("Holding {} {}", bMultiple ? std::format("{}x", Count) : "a", bMultiple ? DisplayName + "s" : DisplayName)); // TODO: Add Count and Rarity
												}
											}
										}
									}
								}
							}

							// TODO: Add health and shield

							ImGui::NewLine();

							if (ButtonCentered(("Exit")))
							{
								bInformationTab = false;
							}
						}
					}
				}
			}

			ImGui::End();
		}

		// Rendering
		ImGui::EndFrame();
		g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
		g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
		D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * clear_color.w * 255.0f), (int)(clear_color.y * clear_color.w * 255.0f), (int)(clear_color.z * clear_color.w * 255.0f), (int)(clear_color.w * 255.0f));
		g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);

		if (g_pd3dDevice->BeginScene() >= 0)
		{
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
			g_pd3dDevice->EndScene();
		}

		HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

		// Handle loss of D3D9 device
		if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
			ResetDevice();
	}

	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	::DestroyWindow(hwnd);
	::UnregisterClass(wc.lpszClassName, wc.hInstance);

	return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
	if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
		return false;

	// Create the D3DDevice
	ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
	g_d3dpp.Windowed = TRUE;
	g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN; // Need to use an explicit format with alpha if needing per-pixel alpha composition.
	g_d3dpp.EnableAutoDepthStencil = TRUE;
	g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
	//g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
	if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
		return false;

	return true;
}

void CleanupDeviceD3D()
{
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
	if (g_pD3D) { g_pD3D->Release(); g_pD3D = NULL; }
}

void ResetDevice()
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
	HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
	if (hr == D3DERR_INVALIDCALL)
		IM_ASSERT(0);
	ImGui_ImplDX9_CreateDeviceObjects();
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// my implementation of window dragging..
	/* {
		static int dababy = 0;
		if (dababy > 100) // wait until gui is initialized ig?
		{
			if (ImGui::IsMouseDragging(ImGuiMouseButton(0)))
			{
				// if (LOWORD(lParam) > 255 && HIWORD(lParam) > 255)
				{
					POINT p;
					GetCursorPos(&p);

					SetWindowPos(hWnd, nullptr, p.x, p.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
				}
			}
		}
		dababy++;
	} */

	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			g_d3dpp.BackBufferWidth = LOWORD(lParam);
			g_d3dpp.BackBufferHeight = HIWORD(lParam);
			ResetDevice();
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}