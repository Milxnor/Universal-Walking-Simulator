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
#include <set>

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
									auto PlayerState = *Controller->Member<UObject*>(("PlayerState"));
									auto Pawn = *Controller->Member<UObject*>(("Pawn"));

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
					Tab = 1;
					PlayerTab = -1;
					bInformationTab = false;
					ImGui::EndTabItem();
				}

				if (serverStatus == EServerStatus::Up)
				{
					if (ImGui::BeginTabItem(ICON_FA_PEOPLE_CARRY " Players"))
					{
						Tab = 2;
						ImGui::EndTabItem();
					}
				}

				if (ImGui::BeginTabItem(("Gamemode")))
				{
					Tab = 3;
					PlayerTab = -1;
					bInformationTab = false;
					ImGui::EndTabItem();
				}

				if (FnVerDouble >= 8.51 || FnVerDouble == 4.1)
				{
					if (ImGui::BeginTabItem(("Thanos")))
					{
						Tab = 4;
						PlayerTab = -1;
						bInformationTab = false;
						ImGui::EndTabItem();
					}
				}

				if (bStarted && Events::HasEvent())
				{
					if (ImGui::BeginTabItem(("Event")))
					{
						Tab = 5;
						PlayerTab = -1;
						bInformationTab = false;
						ImGui::EndTabItem();
					}
				}

				if (ImGui::BeginTabItem(("Settings")))
				{
					Tab = 6;
					PlayerTab = -1;
					bInformationTab = false;
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem(("Credits")))
				{
					Tab = 7;
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
				case 1:
				{
					ImGui::Checkbox(("Log RPCS"), &bLogRpcs);
					ImGui::Checkbox(("Log ProcessEvent"), &bLogProcessEvent);
					ImGui::Checkbox("Clear Inventory on Aircraft", &bClearInventoryOnAircraftJump);

					// ImGui::Checkbox(("Use Beacons"), &bUseBeacons);

					if (serverStatus == EServerStatus::Down && !bTraveled)
					{
						// TODO: Map name
						if (ImGui::Button(("Load in the Match")))
						{
							LoadInMatch();
						}
					}

					// https://media.discordapp.net/attachments/998297579857137734/1006634482884939807/unknown.png
					/* if (ImGui::Button("Spawn AI"))
					{
						static auto aiPCClass = FindObject("Class /Script/FortniteGame.FortAthenaAIBotController");
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
					} */

					if (Engine_Version == 422 && ImGui::Button("Summon Floorloot"))
					{
						CreateThread(0, 0, LootingV2::SummonFloorLoot, 0, 0, 0);
					}

					/* if (ImGui::Button("idfk3"))
					{
						*Helper::GetGameState()->Member<float>("SafeZonesStartTime") = 1;
					} */

					if (serverStatus == EServerStatus::Up)
					{
						if (ImGui::Button("Freecam"))
						{
							FString StartAircraftCmd;
							StartAircraftCmd.Set(L"toggledebugcamera");

							Helper::Console::ExecuteConsoleCommand(StartAircraftCmd);
						}

						//if (Engine_Version < 423 || FnVerDouble >= 16.00) // I do not know how to start the bus on S8+
						{
							if (ImGui::Button(("Start Aircraft")))
							{
								FString StartAircraftCmd;
								StartAircraftCmd.Set(L"startaircraft");

								Helper::Console::ExecuteConsoleCommand(StartAircraftCmd);

								auto gameState = Helper::GetGameState();

								if (Helper::IsSmallZoneEnabled())
								{
									auto Aircraft = gameState->Member<TArray<UObject*>>(("Aircrafts"))->At(0);

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

											if (true) // RandomPOI)
											{
												AircraftLocationToUse = Helper::GetActorLocation(RandomFoundation) + FVector{0, 0, 5000};

												*FlightStartLocation = AircraftLocationToUse;
												Helper::SetActorLocation(Aircraft, AircraftLocationToUse);
												std::cout << std::format("Set aircraft location to {} {} {}\n", AircraftLocationToUse.X, AircraftLocationToUse.Y, AircraftLocationToUse.Z);
											}
											else
												std::cout << "No POI!\n";

											*gameState->Member<bool>("bAircraftIsLocked") = false;

											FString ifrogor;
											ifrogor.Set(L"startsafezone");
											Helper::Console::ExecuteConsoleCommand(ifrogor);
											*gameState->Member<float>("SafeZonesStartTime") = 0.f;
										}
									}
									else
										std::cout << "No Aircraft!\n";
								}

								std::cout << ("Started aircraft!\n");

								static auto BuildingSMActorClass = FindObject(("Class /Script/FortniteGame.BuildingSMActor"));

								// Helper::DestroyAll(BuildingSMActorClass);

								ExistingBuildings.clear();
							}
						}
						// else
						{
							if (ImGui::Button(("Change Phase to Aircraft"))) // TODO: Improve phase stuff
							{
								auto world = Helper::GetWorld();
								auto gameState = *world->Member<UObject*>(("GameState"));

								*gameState->Member<EAthenaGamePhase>(("GamePhase")) = EAthenaGamePhase::Aircraft;

								struct {
									EAthenaGamePhase OldPhase;
								} params2{ EAthenaGamePhase::None };

								static const auto fnGamephase = gameState->Function(("OnRep_GamePhase"));

								// if (fnGamephase)
									// gameState->ProcessEvent(fnGamephase);

								std::cout << ("Changed Phase to Aircraft.");

								// TODO: Hook a func for this

								static auto BuildingSMActorClass = FindObject(("Class /Script/FortniteGame.BuildingSMActor"));

								// Helper::DestroyAll(BuildingSMActorClass);

								ExistingBuildings.clear();
							}
						}

						if (ImGui::Button(("Summon Llamas")))
						{
							CreateThread(0, 0, Looting::Tables::SpawnLlamas, 0, 0, 0);
						}

						if (ImGui::Button("Fill Vending Machines"))
						{
							CreateThread(0, 0, LootingV2::FillVendingMachines, 0, 0, 0);
						}
					}

					if (ImGui::Button("Dump Playlists")) // iirc ftext changed but idk
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

					if (ImGui::Button("Dump Weapons")) // iirc ftext changed but idk
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

					/* if (ImGui::Button("idfk2"))
					{
						Helper::DestroyActor(FindObjectOld("B_BaseGlider_C /Game/Athena/Maps/Athena_Terrain.Athena_Terrain.PersistentLevel.B_BaseGlider_C_"));
					} */

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

					/* if (ImGui::Button("Restart"))
					{
						bStarted = false;
						bListening = false;
						DisableNetHooks();
						
						if (BeaconHost)
							Helper::DestroyActor(BeaconHost);

						LoadInMatch();

						ExistingBuildings.empty();
					} */

					if (ImGui::Button(("Dump Objects (Win64/Objects.log)")))
					{
						std::cout << "Creating DumpObjects Thread \n";
						CreateThread(0, 0, Helper::DumpObjects, 0, 0, 0);
					}
					/*if (ImGui::Button(("SetupTurrets"))) {
						Henchmans::SpawnHenchmans();
					}
					if (ImGui::Button(("OpenVaults"))) {
						Henchmans::OpenVaults();
					}*/
					if (Engine_Version >= 422 && Engine_Version < 424) 
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
				case 2:
					// ImGui::Text("Players Connected: ")
					InitializePlayers();
					for (int i = 0; i < Players.size(); i++)
					{
						auto& Player = Players[i];
						auto PlayerState = Player.second;

						if (!Player.first || !PlayerState)
							continue;

						// if (ImGui::Button(Helper::GetfPlayerName(PlayerState).ToString().c_str()))
						if (ImGui::Button(wstring_to_utf8(std::wstring(Helper::GetfPlayerName(PlayerState).Data.GetData())).c_str()))
						{
							PlayerTab = i;
						}
					}
					break;
				case 3:
				{
					if (!bStarted)
						ImGui::InputText(("Playlist"), &PlaylistToUse);
		
					ImGui::Checkbox(("Playground"), &bIsPlayground);

					// if (!bStarted) // couldnt we wqait till aircraft start

					if (!bIsPlayground)
						ImGui::Checkbox(("Lategame"), &bIsLateGame);

					break;
				}

				case 4:
					if (bStarted == true) {
						if (FnVerDouble >= 8.51 && ImGui::Button(("Init Ashton"))) {
							Ashton::InitAshton();
						}
						if (FnVerDouble >= 8.51 && ImGui::Button(("Spawn Stone"))) {
							Ashton::SpawnRandomStone();
						}

						if (FnVerDouble == 4.1 && ImGui::Button(("Init Carmine"))) {
							Carmine::InitCarmine();
						}
						if (FnVerDouble == 4.1 && ImGui::Button(("Spawn Gauntlet"))) {
							Carmine::SpawnGauntlet();
						}
					}
					
					break;

				case 5:
					if (ImGui::Button(("Start Event")))
						Events::StartEvent();

					if (FnVerDouble == 12.41 && ImGui::Button("Fly players up"))
						EventHelper::BoostUpTravis();

					if (FnVerDouble == 6.21)
					{
						/* if (ImGui::Button("Show Before Event Lake"))
							EventHelper::LoadAndUnloadLake(false);
						if (ImGui::Button("Show After Event Lake"))
							EventHelper::LoadAndUnloadLake(true);
						if (ImGui::Button("Teleport Players to Butterfly"))
							EventHelper::TeleportPlayersToButterfly(); */
					}
					break;

				case 6: // settings

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
				case 7:
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
					auto& CurrentPlayer = Players.at(PlayerTab); // Iirc .at does more checking

					if (CurrentPlayer.first && CurrentPlayer.second)
					{
						auto Pawn = CurrentPlayer.first;

						if (!bInformationTab)
						{
							static std::string VehicleClass;
							static std::string WID;
							static int Count = 1;

							auto PlayerName = Helper::GetfPlayerName(CurrentPlayer.second);
							ImGui::TextColored(ImVec4(18, 253, 112, 0.8), (("Player: ") + PlayerName.ToString()).c_str());
							// TODO: Add Inventory, let them drop, delete, add, modify, etc.
							if (ImGui::Button(("Game Statistics")))
							{
								bInformationTab = true;
							}
							ImGui::NewLine();
							auto Controller = *CurrentPlayer.first->Member<UObject*>(("Controller"));
							/* if (ImGui::Button(ICON_FA_HAMMER " Ban"))
							{
								auto IP = Helper::GetfIP(CurrentPlayer.second);
								Helper::Banning::Ban(PlayerName.Data.GetData(), Controller, IP.Data.GetData());
							}  */
							if (ImGui::Button(("Kick (Do not use twice)")))
							{
								FString Reason;
								Reason.Set(L"You have been kicked!");
								Helper::KickController(Controller, Reason);
							}
							if (ImGui::Button(ICON_FA_CROSSHAIRS " Kill"))
							{

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

							if (ImGui::Button(ICON_FA_HAND_HOLDING_USD " Give Weapon"))
							{
								auto wID = FindObject(WID);

								if (wID)
									Inventory::GiveItem(Controller, wID, EFortQuickBars::Primary, 1, Count);
								else
									std::cout << ("Invalid WID! Please make sure it's a valid object.\n");
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

							auto CurrentWeapon = *Pawn->Member<UObject*>(("CurrentWeapon"));

							if (CurrentWeapon)
							{
								auto Guid = CurrentWeapon->Member<FGuid>(("ItemEntryGuid"));

								if (Guid)
								{
									auto Controller = *Pawn->Member<UObject*>(("Controller"));
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

												if (CurrentGuid == *Guid)
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
