#pragma once

// TODO: Update ImGUI

#include <Windows.h>
#include <dxgi.h>
#include <d3d11.h>

#include <Windows.h>
#include <dxgi.h>
#include <d3d11.h>

#define VPS

#include <ImGui/imgui.h>
#include <Kiero/kiero.h>
#include <filesystem>

#include "Gameplay/helper.h"
#include "Gameplay/events.h"
#include <iostream>
#include <d3d9.h>
#include <ImGui/imgui_impl_dx9.h>

#ifndef VPS

HRESULT(WINAPI* PresentOriginal)(IDXGISwapChain* SwapChain, uint32_t Interval, uint32_t Flags);

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace fs = std::filesystem;

HWND wnd = NULL;
WNDPROC oWndProc;
ID3D11Device* pDevice = nullptr;
ID3D11DeviceContext* pContext = nullptr;
ID3D11RenderTargetView* mainRenderTargetView;

static bool bHasInit = false;
static bool bShow = false;

LRESULT __stdcall WndProc(const HWND hWnd, uint32_t message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYUP:
		if (wParam == VK_F8 || (bShow && wParam == VK_ESCAPE))
		{
			bShow = !bShow;
			ImGui::GetIO().MouseDrawCursor = bShow;
		}
		break;
	case WM_SIZE:
		if (pDevice && wParam != SIZE_MINIMIZED)
		{

		}
		break;
	case WM_QUIT:
		if (bShow)
			ExitProcess(0);

		break;
	case WM_DESTROY:
		PostQuitMessage(0);
	}

	if (bShow)
	{
		ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam);
		return TRUE;
	}

	return CallWindowProc(oWndProc, hWnd, message, wParam, lParam);
}

void SetupStyle()
{
	// Sorry if there's any errors here, I translated this back by hand.
	auto& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF(R"(../vendor/Aller_Bd.ttf)", 15.0f);

	auto& style = ImGui::GetStyle();
	style.FrameRounding = 4.0f;
	style.WindowBorderSize = 0.0f;
	style.PopupBorderSize = 0.0f;
	style.GrabRounding = 4.0f;

	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.73f, 0.75f, 0.74f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.09f, 0.09f, 0.09f, 0.94f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.20f, 0.20f, 0.20f, 0.50f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.71f, 0.39f, 0.39f, 0.54f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.84f, 0.66f, 0.66f, 0.40f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.84f, 0.66f, 0.66f, 0.67f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.47f, 0.22f, 0.22f, 0.67f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.47f, 0.22f, 0.22f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.47f, 0.22f, 0.22f, 0.67f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.34f, 0.16f, 0.16f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.71f, 0.39f, 0.39f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.84f, 0.66f, 0.66f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.47f, 0.22f, 0.22f, 0.65f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.71f, 0.39f, 0.39f, 0.65f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.20f, 0.20f, 0.50f);
	colors[ImGuiCol_Header] = ImVec4(0.71f, 0.39f, 0.39f, 0.54f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.84f, 0.66f, 0.66f, 0.65f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.84f, 0.66f, 0.66f, 0.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.71f, 0.39f, 0.39f, 0.54f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.71f, 0.39f, 0.39f, 0.54f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.71f, 0.39f, 0.39f, 0.54f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.84f, 0.66f, 0.66f, 0.66f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.84f, 0.66f, 0.66f, 0.66f);
	colors[ImGuiCol_Tab] = ImVec4(0.71f, 0.39f, 0.39f, 0.54f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.84f, 0.66f, 0.66f, 0.66f);
	colors[ImGuiCol_TabActive] = ImVec4(0.84f, 0.66f, 0.66f, 0.66f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

HRESULT WINAPI HookPresent(IDXGISwapChain* SwapChain, uint32_t Interval, uint32_t Flags)
{
	if (!bHasInit)
	{
		auto stat = SUCCEEDED(SwapChain->GetDevice(__uuidof(ID3D11Device), (PVOID*)&pDevice));
		if (stat)
		{
			pDevice->GetImmediateContext(&pContext);
			DXGI_SWAP_CHAIN_DESC sd;
			SwapChain->GetDesc(&sd);
			wnd = sd.OutputWindow;
			ID3D11Texture2D* pBackBuffer;
			SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
			pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
			pBackBuffer->Release();
			oWndProc = (WNDPROC)SetWindowLongPtr(wnd, GWLP_WNDPROC, (LONG_PTR)WndProc);
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO();
			io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;

			ImGui_ImplWin32_Init(wnd);
			ImGui_ImplDX11_Init(pDevice, pContext);

			SetupStyle();

			bHasInit = true;
		}

		else return PresentOriginal(SwapChain, Interval, Flags);
	}

	if (bShow) // TODO: Acutaly learn ImGUI and rewrite
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();

		ImGui::NewFrame();
		ImGui::SetNextWindowBgAlpha(0.8f);
		ImGui::SetNextWindowSize(ImVec2(560, 345));

		ImGui::Begin(_("Project Reboot"), 0, ImGuiWindowFlags_NoCollapse);

		// initialize variables used by the user using the gui

		static int Tab = 1;
		static float currentFOV = 80;
		static float FOV = 80;
		static char WID[60] = {};
		static bool bConsoleIsOpen = false;
		static char headPath[MAX_PATH] = "";
		static char bodyPath[MAX_PATH] = "";

		if (ImGui::BeginTabBar("")) {
			if (ImGui::BeginTabItem(_("Game")))
			{
				Tab = 1;
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem(_("Mode")))
			{
				Tab = 2;
				ImGui::EndTabItem();
			}

			// Add a tab called like POIs and add all POIs

			ImGui::EndTabBar();
		}

		switch (Tab) // now that we know what tab we can now display what that tab has
		{
		case 1:
			if (Engine_Version < 423) // I do not know how to start the bus on S8+
			{
				if (ImGui::Button(_("Start Aircraft")))
				{
					FString StartAircraftCmd;
					StartAircraftCmd.Set(L"startaircraft");

					Helper::Console::ExecuteConsoleCommand(StartAircraftCmd);

					std::cout << _("Started aircraft!\n");
				}
			}
			if (Events::HasEvent()) {
				if (ImGui::Button(_("Start Event"))) {
					Events::StartEvent();
				}
			}

			if (ImGui::Button(_("Enable Glider Redeploy")))
			{
				FString GliderRedeployCmd;
				GliderRedeployCmd.Set(L"Athena.EnableParachuteEverywhere 1");
				Helper::Console::ExecuteConsoleCommand(GliderRedeployCmd);

				std::cout << _("Enabled Glider Redeploy!\n");
			}

			if (ImGui::Button(_("Disable Glider Redeploy")))
			{
				FString GliderRedeployCmd;
				GliderRedeployCmd.Set(L"Athena.EnableParachuteEverywhere 0");
				Helper::Console::ExecuteConsoleCommand(GliderRedeployCmd);

				std::cout << _("Disabled Glider Redeploy!\n");
			}

			break;

		case 2:
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
			}

			/* if (ImGui::Button(_("Change Phase to SafeZones"))) // Crashes
			{
				auto world = Helper::GetWorld();
				auto gameState = *world->Member<UObject*>(_("GameState"));

				*gameState->Member<EAthenaGamePhase>(_("GamePhase")) = EAthenaGamePhase::SafeZones;

				struct {
					EAthenaGamePhase OldPhase;
				} params2{ EAthenaGamePhase::None };

				static const auto fnGamephase = gameState->Function(_("OnRep_GamePhase"));

				std::cout << _("Changed Phase to SafeZones.");
			} */
			/*if (ImGui::Button(_("Spawn Plane at spawn"))) // Crashes
			{

				auto plane = FindObject(_("Class /Script/FortniteGame.FortAthenaFerretVehicle"));

				Easy::SpawnActor(plane, FVector{ 0, 0, 3000 });



				std::cout << _("Spawn Plane at spawn");
			}*/
			break;
		}

		ImGui::End();
		ImGui::Render();

		pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}
	return PresentOriginal(SwapChain, Interval, Flags);
}

DWORD WINAPI GuiHook(LPVOID)
{
	bool bHooked = false;
	while (!bHooked)
	{
		auto status = kiero::init(kiero::RenderType::D3D11); // Don't do auto because it tries DX9 since that's the first direct thingy it finds
		if (status == kiero::Status::Success)
		{
			kiero::bind(8, (PVOID*)&PresentOriginal, HookPresent);
			bHooked = true;
		}

		Sleep(100);
	}

	std::cout << _("Initialized GUI!\n");

	return 0;
}

#else


static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS    g_d3dpp;

extern LRESULT ImGui_ImplDX9_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplDX9_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			ImGui_ImplDX9_InvalidateDeviceObjects();
			g_d3dpp.BackBufferWidth = LOWORD(lParam);
			g_d3dpp.BackBufferHeight = HIWORD(lParam);
			HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
			if (hr == D3DERR_INVALIDCALL)
				IM_ASSERT(0);
			ImGui_ImplDX9_CreateDeviceObjects();
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

DWORD WINAPI GuiThread(LPVOID)
{
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, LoadCursor(NULL, IDC_ARROW), NULL, NULL, L"Project_Reboot", NULL };
	RegisterClassEx(&wc);
	HWND hwnd = CreateWindowA("Project_Reboot", "Project Reboot UI", WS_POPUP | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX, 10, 10, 600, 400, NULL, NULL, wc.hInstance, NULL);

	// Initialize Direct3D
	LPDIRECT3D9 pD3D;
	if ((pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
	{
		UnregisterClassA("Project_Reboot", wc.hInstance);
		return 1;
	}

	ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
	g_d3dpp.Windowed = TRUE;
	g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	g_d3dpp.EnableAutoDepthStencil = TRUE;
	g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	// Create the D3DDevice
	if (pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
	{
		pD3D->Release();
		UnregisterClassA("Project_Reboot", wc.hInstance);
		return 1;
	}

	// Setup ImGui binding
	ImGui_ImplDX9_Init(hwnd, g_pd3dDevice);

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontDefault();

	// Main loop
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	ShowWindow(hwnd, SW_SHOWDEFAULT);
	UpdateWindow(hwnd);

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			continue;
		}
		ImGui_ImplDX9_NewFrame();

		static int Tab = 1;

		ImGui::Begin("Reboot", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
		{

			if (Engine_Version < 423) // I do not know how to start the bus on S8+
			{
				if (ImGui::Button(_("Start Aircraft")))
				{
					auto world = Helper::GetWorld();
					auto gameState = *world->Member<UObject*>(_("GameState"));

					if (gameState)
					{
						*gameState->Member<char>(_("bGameModeWillSkipAircraft")) = false;
						*gameState->Member<float>(_("AircraftStartTime")) = 10.0f;
						*gameState->Member<float>(_("WarmupCountdownEndTime")) = 5.0f;
					}
					std::cout << _("Aircraft will start!\n");
					/* FString StartAircraftCmd;
					StartAircraftCmd.Set(L"startaircraft");

					Helper::Console::ExecuteConsoleCommand(StartAircraftCmd);

					std::cout << _("Started aircraft!\n"); */
				}
			}

			/* if (ImGui::Button(_("Clear all Buildings")))
			{
				static auto BuildingSMActorClass = FindObject(_("Class /Script/FortniteGame.BuildingSMActor"));

				auto Buildings = Helper::GetAllActorsOfClass(BuildingSMActorClass);

				for (int i = 0; i < Buildings.Num(); i++)
				{
					auto Building = Buildings.At(i);

					if (Building)
						Helper::DestroyActor(Building);
				}

				std::cout << _("Destroyed all Buildings!\n");
			} */

			if (ImGui::Button(_("Fill vending machiees")))
			{
				CreateThread(0, 0, Looting::Tables::FillVendingMachines, 0, 0, 0);
			}

			if (ImGui::Button(_("Spawn FloorLoot")))
			{
				CreateThread(0, 0, Looting::Tables::SpawnFloorLoot, 0, 0, 0);
			}

			if (Events::HasEvent()) {
				if (ImGui::Button(_("Start Event"))) {
					Events::StartEvent();
				}
			}

			if (ImGui::Button(_("Enable Glider Redeploy")))
			{
				FString GliderRedeployCmd;
				GliderRedeployCmd.Set(L"Athena.EnableParachuteEverywhere 1");
				Helper::Console::ExecuteConsoleCommand(GliderRedeployCmd);

				std::cout << _("Enabled Glider Redeploy!\n");
			}

			if (ImGui::Button(_("Disable Glider Redeploy")))
			{
				FString GliderRedeployCmd;
				GliderRedeployCmd.Set(L"Athena.EnableParachuteEverywhere 0");
				Helper::Console::ExecuteConsoleCommand(GliderRedeployCmd);

				std::cout << _("Disabled Glider Redeploy!\n");
			}

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
			}

			/* if (ImGui::Button(_("Change Phase to SafeZones"))) // Crashes
			{
				auto world = Helper::GetWorld();
				auto gameState = *world->Member<UObject*>(_("GameState"));

				*gameState->Member<EAthenaGamePhase>(_("GamePhase")) = EAthenaGamePhase::SafeZones;

				struct {
					EAthenaGamePhase OldPhase;
				} params2{ EAthenaGamePhase::None };

				static const auto fnGamephase = gameState->Function(_("OnRep_GamePhase"));

				std::cout << _("Changed Phase to SafeZones.");
			} */
			/*if (ImGui::Button(_("Spawn Plane at spawn"))) // Crashes
			{

				auto plane = FindObject(_("Class /Script/FortniteGame.FortAthenaFerretVehicle"));

				Easy::SpawnActor(plane, FVector{ 0, 0, 3000 });



				std::cout << _("Spawn Plane at spawn");
			}*/

			if (ImGui::Button("Dump Objects (Objects.txt)")) {
				Helper::DumpObjects();
			}
		}

		// Rendering
		g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, false);
		g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, false);

		if (g_pd3dDevice->BeginScene() >= 0)
		{
			ImGui::Render();
			g_pd3dDevice->EndScene();
		}
		g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
	}

	ImGui_ImplDX9_Shutdown();
	if (g_pd3dDevice) g_pd3dDevice->Release();
	if (pD3D) pD3D->Release();
	UnregisterClassA("Project_Reboot", wc.hInstance);

	return 0;
}

#endif