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

// THE BASE CODE IS FROM IMGUI GITHUB

static LPDIRECT3D9              g_pD3D = NULL;
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

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
		ImGui::SetNextWindowSize(ImVec2(640, 480), ImGuiCond_Always);

		static int Tab = 1;
		static int PlayerTab = -1;
		static bool bInformationTab = false;

		if (!ImGui::IsWindowCollapsed())
		{
			ImGui::Begin(_("Project Reboot"), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);

			std::vector<std::pair<UObject*, UObject*>> Players; // Pawn, PlayerState

			auto InitializePlayers = [&Players]() { // TODO: slow
				auto World = Helper::GetWorld();
				if (World)
				{
					auto NetDriver = *World->Member<UObject*>(_("NetDriver"));
					if (NetDriver)
					{
						auto ClientConnections = NetDriver->Member<TArray<UObject*>>(_("ClientConnections"));

						if (ClientConnections)
						{
							for (int i = 0; i < ClientConnections->Num(); i++)
							{
								auto Connection = ClientConnections->At(i);

								if (!Connection)
									return;

								auto Controller = *Connection->Member<UObject*>(_("PlayerController"));

								if (Controller)
								{
									auto PlayerState = *Controller->Member<UObject*>(_("PlayerState"));
									auto Pawn = *Controller->Member<UObject*>(_("Pawn"));

									if (Pawn)
									{
										static auto IsActorBeingDestroyed = Pawn->Function(_("IsActorBeingDestroyed"));

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

				if (ImGui::BeginTabItem(_("Gamemode")))
				{
					Tab = 3;
					PlayerTab = -1;
					bInformationTab = false;
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem(_("Thanos")))
				{
					Tab = 5;
					PlayerTab = -1;
					bInformationTab = false;
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem(_("Credits")))
				{
					Tab = 4;
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
					ImGui::Checkbox(_("Log RPCS"), &bLogRpcs);
					ImGui::Checkbox(_("Log ProcessEvent"), &bLogProcessEvent);
					ImGui::Checkbox(_("Use Beacons"), &bUseBeacons);

					if (serverStatus == EServerStatus::Down && !bTraveled)
					{
						// TODO: Map name
						if (ImGui::Button(_("Load in the Match")))
						{
							LoadInMatch();
						}
					}

					if (serverStatus == EServerStatus::Up)
					{
						if (Engine_Version < 423) // I do not know how to start the bus on S8+
						{
							if (ImGui::Button(_("Start Aircraft")))
							{
								FString StartAircraftCmd;
								StartAircraftCmd.Set(L"startaircraft");

								Helper::Console::ExecuteConsoleCommand(StartAircraftCmd);

								std::cout << _("Started aircraft!\n");

								// TODO: Hook a func for this

								static auto BuildingSMActorClass = FindObject(_("Class /Script/FortniteGame.BuildingSMActor"));

								// Helper::DestroyAll(BuildingSMActorClass);

								ExistingBuildings.clear();
							}
						}
						// else
						{
							if (ImGui::Button(_("Change Phase to Aircraft"))) // TODO: Improve phase stuff
							{
								auto world = Helper::GetWorld();
								auto gameState = *world->Member<UObject*>(_("GameState"));

								*gameState->Member<EAthenaGamePhase>(_("GamePhase")) = EAthenaGamePhase::Aircraft;

								struct {
									EAthenaGamePhase OldPhase;
								} params2{ EAthenaGamePhase::None };

								static const auto fnGamephase = gameState->Function(_("OnRep_GamePhase"));

								std::cout << _("Changed Phase to Aircraft.");

								// TODO: Hook a func for this

								static auto BuildingSMActorClass = FindObject(_("Class /Script/FortniteGame.BuildingSMActor"));

								// Helper::DestroyAll(BuildingSMActorClass);

								ExistingBuildings.clear();
							}
						}

						if (ImGui::Button(_("Summon Llamas")))
						{
							CreateThread(0, 0, Looting::Tables::SpawnLlamas, 0, 0, 0);
						}
					}

					if (Events::HasEvent()) {
						if (ImGui::Button(_("Start Event")))
							Events::StartEvent();
					}

					/* if (ImGui::Button(_("Summon FloorLoot")))
					{
						CreateThread(0, 0, Looting::Tables::SpawnFloorLoot, 0, 0, 0);
					} */

					if (ImGui::Button(_("Dump Objects (Win64/Objects.log)")))
					{
						CreateThread(0, 0, Helper::DumpObjects, 0, 0, 0);
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

						if (ImGui::Button(Helper::GetPlayerName(PlayerState).c_str()))
						{
							PlayerTab = i;
						}
					}
					break;
				case 3:
				{
					static std::string CurrentPlaylist;
					ImGui::InputText(_("Playlist"), &CurrentPlaylist);
					ImGui::Checkbox(_("Playground"), &bIsPlayground);

					// TODO: default character parts
					break;
				}
				case 4:
					TextCentered(_("Credits:"));
					TextCentered(_("Milxnor: Made the base, main developer"));
					TextCentered(_("GD: Added events, cleans up code and adds features."));

					break;

				case 5:
					if(ImGui::Button(_("Spawn Mind Stone"))) {
						FVector RandLocation;
						std::random_device rd; // obtain a random number from hardware
						std::mt19937 gen(rd()); // seed the generator

						// CHAPTER 1

						std::uniform_int_distribution<> Xdistr(-40000, 128000);
						std::uniform_int_distribution<> Ydistr(-90000, 70000);
						std::uniform_int_distribution<> Zdistr(-40000, 30000); // doesnt matter

						RandLocation.X = Xdistr(gen);
						RandLocation.Y = Ydistr(gen);
						RandLocation.Z = Zdistr(gen);

						Helper::SummonPickup(nullptr, FindObject("/Game/Athena/Items/LTM/AshtonRockItemDef_Y.AshtonRockItemDef_Y"), RandLocation, EFortPickupSourceTypeFlag::Other, EFortPickupSpawnSource::Unset);
					}
					if (ImGui::Button(_("Spawn Reality Stone"))) {
						FVector RandLocation;
						std::random_device rd; // obtain a random number from hardware
						std::mt19937 gen(rd()); // seed the generator

						// CHAPTER 1

						std::uniform_int_distribution<> Xdistr(-40000, 128000);
						std::uniform_int_distribution<> Ydistr(-90000, 70000);
						std::uniform_int_distribution<> Zdistr(-40000, 30000); // doesnt matter

						RandLocation.X = Xdistr(gen);
						RandLocation.Y = Ydistr(gen);
						RandLocation.Z = Zdistr(gen);

						RandLocation = { 1250, 1818, 3284 };

						Helper::SummonPickup(nullptr, FindObject("/Game/Athena/Items/LTM/AshtonRockItemDef_R.AshtonRockItemDef_R"), RandLocation, EFortPickupSourceTypeFlag::Other, EFortPickupSpawnSource::Unset);
						//Easy::SpawnActor(FindObject("/Game/Athena/Items/LTM/AshtonRockItemDef_R.AshtonRockItemDef_R"), RandLocation, {});
					}
					if (ImGui::Button(_("Spawn Power Stone"))) {
						FVector RandLocation;
						std::random_device rd; // obtain a random number from hardware
						std::mt19937 gen(rd()); // seed the generator

						// CHAPTER 1

						std::uniform_int_distribution<> Xdistr(-40000, 128000);
						std::uniform_int_distribution<> Ydistr(-90000, 70000);
						std::uniform_int_distribution<> Zdistr(-40000, 30000); // doesnt matter

						RandLocation.X = Xdistr(gen);
						RandLocation.Y = Ydistr(gen);
						RandLocation.Z = Zdistr(gen);

						Helper::SummonPickup(nullptr, FindObject("/Game/Athena/Items/LTM/AshtonRockItemDef_P.AshtonRockItemDef_P"), RandLocation, EFortPickupSourceTypeFlag::Other, EFortPickupSpawnSource::Unset);
						//Easy::SpawnActor(FindObject("/Game/Athena/Items/LTM/AshtonRockItemDef_P.AshtonRockItemDef_P"), RandLocation, {});
					}
					if (ImGui::Button(_("Spawn Soul Stone"))) {
						FVector RandLocation;
						std::random_device rd; // obtain a random number from hardware
						std::mt19937 gen(rd()); // seed the generator

						// CHAPTER 1

						std::uniform_int_distribution<> Xdistr(-40000, 128000);
						std::uniform_int_distribution<> Ydistr(-90000, 70000);
						std::uniform_int_distribution<> Zdistr(-40000, 30000); // doesnt matter

						RandLocation.X = Xdistr(gen);
						RandLocation.Y = Ydistr(gen);
						RandLocation.Z = Zdistr(gen);

						Helper::SummonPickup(nullptr, FindObject("/Game/Athena/Items/LTM/AshtonRockItemDef_O.AshtonRockItemDef_O"), RandLocation, EFortPickupSourceTypeFlag::Other, EFortPickupSpawnSource::Unset);
						//Easy::SpawnActor(FindObject("/Game/Athena/Items/LTM/AshtonRockItemDef_O.AshtonRockItemDef_O"), RandLocation, {});
					}
					if (ImGui::Button(_("Spawn Time Stone"))) {
						FVector RandLocation;
						std::random_device rd; // obtain a random number from hardware
						std::mt19937 gen(rd()); // seed the generator

						// CHAPTER 1

						std::uniform_int_distribution<> Xdistr(-40000, 128000);
						std::uniform_int_distribution<> Ydistr(-90000, 70000);
						std::uniform_int_distribution<> Zdistr(-40000, 30000); // doesnt matter

						RandLocation.X = Xdistr(gen);
						RandLocation.Y = Ydistr(gen);
						RandLocation.Z = Zdistr(gen);

						Helper::SummonPickup(nullptr, FindObject("/Game/Athena/Items/LTM/AshtonRockItemDef_G.AshtonRockItemDef_G"), RandLocation, EFortPickupSourceTypeFlag::Other, EFortPickupSpawnSource::Unset);
						//Easy::SpawnActor(FindObject("/Game/Athena/Items/LTM/AshtonRockItemDef_G.AshtonRockItemDef_G"), RandLocation, {});
					}
					if (ImGui::Button(_("Spawn Space Stone"))) {
						FVector RandLocation;
						std::random_device rd; // obtain a random number from hardware
						std::mt19937 gen(rd()); // seed the generator

						// CHAPTER 1

						std::uniform_int_distribution<> Xdistr(-40000, 128000);
						std::uniform_int_distribution<> Ydistr(-90000, 70000);
						std::uniform_int_distribution<> Zdistr(-40000, 30000); // doesnt matter

						RandLocation.X = Xdistr(gen);
						RandLocation.Y = Ydistr(gen);
						RandLocation.Z = Zdistr(gen);

						Helper::SummonPickup(nullptr, FindObject("/Game/Athena/Items/LTM/AshtonRockItemDef_B.AshtonRockItemDef_B"), RandLocation, EFortPickupSourceTypeFlag::Other, EFortPickupSpawnSource::Unset);
						//Easy::SpawnActor(FindObject("/Game/Athena/Items/LTM/AshtonRockItemDef_B.AshtonRockItemDef_B"), RandLocation, {});
					}
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
						if (!bInformationTab)
						{
							static std::string WID;
							static int Count = 1;

							auto PlayerName = Helper::GetfPlayerName(CurrentPlayer.second);
							ImGui::TextColored(ImVec4(18, 253, 112, 0.8), (_("Player: ") + PlayerName.ToString()).c_str());
							if (ImGui::Button(_("Game Statistics")))
							{
								bInformationTab = true;
							}
							ImGui::NewLine();
							auto Controller = *CurrentPlayer.first->Member<UObject*>(_("Controller"));
							/* if (ImGui::Button(ICON_FA_HAMMER " Ban"))
							{
								auto IP = Helper::GetfIP(CurrentPlayer.second);
								Helper::Banning::Ban(PlayerName.Data.GetData(), Controller, IP.Data.GetData());
							}  */
							if (ImGui::Button(_("Kick (Do not use twice)")))
							{
								FString Reason;
								Reason.Set(L"You have been kicked!");
								Helper::KickController(Controller, Reason);
							}
							if (ImGui::Button(ICON_FA_CROSSHAIRS " Kill"))
							{

							}

							// TODO: Add teleport to location

							ImGui::InputText(_("WID"), &WID);
							ImGui::InputInt(_("Count"), &Count);

							if (ImGui::Button(ICON_FA_HAND_HOLDING_USD " Give Weapon"))
							{
								auto wID = FindObject(WID);

								if (wID)
									Inventory::GiveItem(Controller, wID, EFortQuickBars::Primary, 1, Count);
								else
									std::cout << _("Invalid WID! Please make sure it's a valid object.\n");
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
							auto Pawn = CurrentPlayer.first;
							TextCentered(std::format(("Kills: {}"), *CurrentPlayer.second->Member<int>(_("KillScore"))));
							auto PawnLocation = Helper::GetActorLocation(Pawn);
							TextCentered(std::format(("X: {} Y: {} Z: {}"), (int)PawnLocation.X, (int)PawnLocation.Y, (int)PawnLocation.Z)); // We cast to an int because it changes too fast. 

							auto CurrentWeapon = *Pawn->Member<UObject*>(_("CurrentWeapon"));

							if (CurrentWeapon)
							{
								auto Guid = CurrentWeapon->Member<FGuid>(_("ItemEntryGuid"));

								if (Guid)
								{
									auto Controller = *Pawn->Member<UObject*>(_("Controller"));
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
													Count = *FFortItemEntry::GetCount(ItemInstance->Member<__int64>(_("ItemEntry"))); // lets hope its a UFortWorldItem*
													// break;
												}
											}

											if (Definition)
											{
												auto Name = Definition->Member<FText>(_("DisplayName"));

												if (Name)
												{
													auto DisplayName = Helper::Conversion::TextToString(*Name);
													// auto Rarity = *Definition->Member<EFortRarity>(_("Rarity"));

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

							if (ButtonCentered(_("Exit")))
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