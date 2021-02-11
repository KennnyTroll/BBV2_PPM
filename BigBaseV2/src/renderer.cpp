#include "common.hpp"
#include "fonts.hpp"
#include "logger.hpp"
#include "gui.hpp"
#include "pointers.hpp"
#include "renderer.hpp"
#include <imgui.h>
#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_win32.h>
#include <imgui_internal.h>

#include <XInput.h>
#pragma comment(lib, "Xinput.lib")
#pragma comment(lib, "Xinput9_1_0.lib")

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

typedef DWORD(WINAPI* PFN_XInputGetCapabilities)(DWORD, DWORD, XINPUT_CAPABILITIES*);
typedef DWORD(WINAPI* PFN_XInputGetState)(DWORD, XINPUT_STATE*);

static HMODULE                      g_XInputDLL = NULL;
static PFN_XInputGetCapabilities    g_XInputGetCapabilities = NULL;
static PFN_XInputGetState           g_XInputGetState = NULL;
static bool                 g_WantUpdateHasGamepad = true;
static bool                 g_HasGamepad = false;

Timeeer MainTimer;

namespace big
{
	renderer::renderer() :
		m_dxgi_swapchain(*g_pointers->m_swapchain)
	{
		void *d3d_device{};
		if (SUCCEEDED(m_dxgi_swapchain->GetDevice(__uuidof(ID3D11Device), &d3d_device)))
		{
			m_d3d_device.Attach(static_cast<ID3D11Device*>(d3d_device));
		}
		else
		{
			throw std::runtime_error("Failed to get D3D device.");
		}
		
		//auto file_path = std::filesystem::path(std::getenv("appdata"));
		auto file_path = std::filesystem::path(Menu::le_DLL_Path);
		
		file_path /= "PP_Log_Setings";
		if (!std::filesystem::exists(file_path))
		{
			std::filesystem::create_directory(file_path);
		}
		else if (!std::filesystem::is_directory(file_path))
		{
			std::filesystem::remove(file_path);
			std::filesystem::create_directory(file_path);
		}

		ImGuiContext* ctx = ImGui::CreateContext();
		std::filesystem::path m_backup_path_log = file_path;//PP_Log_Setings

		file_path /= "imgui.ini";
		static std::string path = file_path.make_preferred().string();
		ctx->IO.IniFilename = path.c_str();

		m_backup_path_log /= "imgui_log.txt";
		static std::string pathh_log = m_backup_path_log.make_preferred().string();
		ctx->IO.LogFilename = pathh_log.c_str();

		//Setup Dear ImGui
		//IMGUI_CHECKVERSION();

		ImGuiIO& io = ImGui::GetIO();
		(void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls	
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;

		////NavEnableGamepad needs to set ImGuiBackendFlags_HasGamepad
		io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
		////NavEnableGamepad needs to set ImGuiBackendFlags_HasGamepad
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

		m_d3d_device->GetImmediateContext(m_d3d_device_context.GetAddressOf());

		ImGui_ImplDX11_Init(m_d3d_device.Get(), m_d3d_device_context.Get());
		ImGui_ImplWin32_Init(g_pointers->m_hwnd);

		ImFontConfig font_cfg{};
		font_cfg.FontDataOwnedByAtlas = false;
		std::strcpy(font_cfg.Name, "Rubik");
		m_font = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(const_cast<std::uint8_t*>(font_rubik), sizeof(font_rubik), 20.f, &font_cfg);
		m_monospace_font = ImGui::GetIO().Fonts->AddFontDefault();

		g_gui.dx_init();

		g_renderer = this;
	}

	renderer::~renderer()
	{
		ImGui_ImplWin32_Shutdown();
		ImGui_ImplDX11_Shutdown();
		ImGui::DestroyContext();

		g_renderer = nullptr;
	}

	void renderer::on_present()
	{
		if (g_gui.m_opened)
		{
			ImGui::GetIO().MouseDrawCursor = true;
			ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
		}
		else
		{
			ImGui::GetIO().MouseDrawCursor = false;
			ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		g_gui.dx_on_tick();

		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	void renderer::pre_reset()
	{
		ImGui_ImplDX11_InvalidateDeviceObjects();
	}

	void renderer::post_reset()
	{
		ImGui_ImplDX11_CreateDeviceObjects();
	}

	void renderer::wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		MainTimer.curClock = clock();
		XINPUT_STATE xinput_state;
		static POINT cursor_coords{};
		if (g_WantUpdateHasGamepad)
		{
			const char* xinput_dll_names[] =
			{
				"xinput1_4.dll",   // Windows 8+
				"xinput1_3.dll",   // DirectX SDK
				"xinput9_1_0.dll", // Windows Vista, Windows 7
				"xinput1_2.dll",   // DirectX SDK
				"xinput1_1.dll"    // DirectX SDK
			};
			for (int n = 0; n < IM_ARRAYSIZE(xinput_dll_names); n++)
			{
				if (HMODULE dll = ::LoadLibraryA(xinput_dll_names[n]))
				{
					g_XInputDLL = dll;
					g_XInputGetCapabilities = (PFN_XInputGetCapabilities)::GetProcAddress(dll, "XInputGetCapabilities");
					g_XInputGetState = (PFN_XInputGetState)::GetProcAddress(dll, "XInputGetState");
					break;
				}
			}

			XINPUT_CAPABILITIES caps;
			g_HasGamepad = g_XInputGetCapabilities ? (g_XInputGetCapabilities(0, XINPUT_FLAG_GAMEPAD, &caps) == ERROR_SUCCESS) : false;
			g_WantUpdateHasGamepad = false;
			LOG(INFO) << "Founded Gamepad !";
		}

		if (g_HasGamepad && g_XInputGetState && g_XInputGetState(0, &xinput_state) == ERROR_SUCCESS)
		{
			//LOG(INFO) << "g_WantUpdateHasGamepad = false";
			//if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT))
			if ((xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)
				&& (xinput_state.Gamepad.wButtons & XINPUT_GAMEPAD_A)
				&& MainTimer.IsTimerGood()
				)
			{
				if (g_gui.m_opened)
				{
					LOG(INFO) << "->Gamepad Open Menu<-";
					//LOG(INFO) << "Gamepad Close Menu";
					GetCursorPos(&cursor_coords);
				}
				else if (cursor_coords.x + cursor_coords.y != 0)
				{
					//LOG(INFO) << "Gamepad Open Menu";
					LOG(INFO) << "->Gamepad Close Menu<-";
					SetCursorPos(cursor_coords.x, cursor_coords.y);
				}

				g_gui.m_opened ^= true;

				//LOG(INFO) << "MainTimer.Delay(350)";
				MainTimer.Delay(350);
			}
		}

		if (msg == WM_DEVICECHANGE)
		{
			if ((UINT)wparam == 0x0007/*DBT_DEVNODES_CHANGED*/)
				g_WantUpdateHasGamepad = true;
		}


		if (msg == WM_KEYUP && wparam == VK_END)
		{
			LOG(INFO) << "<Unload from Keyboard>";
			g_running = false;
		}			


		if (msg == WM_KEYUP && wparam == VK_INSERT && g_running)
		{
			//Persist and restore the cursor position between menu instances.
			
			if (g_gui.m_opened)
			{
				LOG(INFO) << "-Key Open Menu-";
				GetCursorPos(&cursor_coords);
			}
			else if (cursor_coords.x + cursor_coords.y != 0)
			{
				LOG(INFO) << "-Key Close Menu-";
				SetCursorPos(cursor_coords.x, cursor_coords.y);
			}

			g_gui.m_opened ^= true;
		}			

		if (g_gui.m_opened && g_running)
		{
			ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam);
		}
	}
}
