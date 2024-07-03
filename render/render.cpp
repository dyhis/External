#define IMGUI_DEFINE_MATH_OPERATORS

#include "render.h"

#include "../cheat/loops/loop.h"

#include "../cheat/SDK.h"

#include "../cheat/driver/driver.h"

#include <wrl/client.h>

#include "../../backend/encryption/Importer.h"

#include "../../backend/encryption/SkCrypt.h"

#include "../../frontend/cheat/ue5/ue5.h"

#include "../../backend/encryption/Importer.h"

#include "font/font.h"

ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 0.0f);

static ImVec2 esp_preview_pos;

ImFont* FONT;

ImFontConfig config;

struct hwid_
{
	HWND window_handle = nullptr;
	ID3D11Device* d3d_device = nullptr;
	ID3D11DeviceContext* d3d_device_ctx = nullptr;
	IDXGISwapChain* d3d_swap_chain = nullptr;
	ID3D11RenderTargetView* d3d_render_target = nullptr;
};
inline hwid_* hwid = new hwid_();
class discord_
{
public:
	typedef struct _Header
	{
		UINT Magic;
		UINT FrameCount;
		UINT NoClue;
		UINT Width;
		UINT Height;
		BYTE Buffer[1];
	} Header;

	struct _ConnectedProcessInfo
	{
		UINT ProcessId;
		HANDLE File;
		Header* MappedAddress;
	}; _ConnectedProcessInfo processInfo;

	bool connect()
	{
		std::string mappedFilename = "DiscordOverlay_Framebuffer_Memory_" + std::to_string(processInfo.ProcessId);
		processInfo.File = OpenFileMappingA(FILE_MAP_ALL_ACCESS, false, mappedFilename.c_str());
		if (!processInfo.File || processInfo.File == INVALID_HANDLE_VALUE)
			return false;

		processInfo.MappedAddress = static_cast<Header*>(MapViewOfFile(processInfo.File, FILE_MAP_ALL_ACCESS, 0, 0, 0));
		return processInfo.MappedAddress;
	}
};
inline discord_* discord = new discord_();

bool _discord::init()
{
	hwid->window_handle = LI_FN(FindWindowA).forwarded_safe_cached()(nullptr, E("Fortnite  "));
	if (!hwid->window_handle)
	{
		std::cout << "\n [-] Fortnite Not Found" << std::endl;
	}

	else
	{
		std::cout << "\n [+]  Fortnite Found" << std::endl;
	}

	UINT targetProcessId;
	GetWindowThreadProcessId(hwid->window_handle, reinterpret_cast<LPDWORD>(&targetProcessId));

	discord->processInfo.ProcessId = targetProcessId;

	bool status = discord->connect();
	if (!status)
	{
		std::cout << "[!] Failed To Hook Discord" << std::endl;
	}

	return true;
}

void send_frame_buffer(IDXGISwapChain* pSwapChain)
{
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuffer = nullptr;
	HRESULT hr = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &pBackBuffer);

	D3D11_TEXTURE2D_DESC txtDesc;
	pBackBuffer->GetDesc(&txtDesc);

	txtDesc.Usage = D3D11_USAGE_STAGING;
	txtDesc.BindFlags = 0;
	txtDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBufferStaging = nullptr;
	hr = hwid->d3d_device->CreateTexture2D(&txtDesc, nullptr, &pBackBufferStaging);

	hwid->d3d_device_ctx->CopyResource(pBackBufferStaging.Get(), pBackBuffer.Get());

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	hr = hwid->d3d_device_ctx->Map(pBackBufferStaging.Get(), 0, D3D11_MAP_READ, 0, &mappedResource);
	if (SUCCEEDED(hr))
	{
		unsigned int bufferSize = txtDesc.Height * mappedResource.RowPitch;

		memcpy(discord->processInfo.MappedAddress->Buffer, mappedResource.pData, bufferSize);
		discord->processInfo.MappedAddress->FrameCount++;

		hwid->d3d_device_ctx->Unmap(pBackBufferStaging.Get(), 0);
	}
}

void embraceTheDarkness()
{
	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
	colors[ImGuiCol_Border] = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
	colors[ImGuiCol_Button] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
	colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
	colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TableHeaderBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_TableBorderStrong] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_TableBorderLight] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
	colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
	colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);

	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowPadding = ImVec2(8.00f, 8.00f);
	style.FramePadding = ImVec2(5.00f, 2.00f);
	style.CellPadding = ImVec2(6.00f, 6.00f);
	style.ItemSpacing = ImVec2(6.00f, 6.00f);
	style.ItemInnerSpacing = ImVec2(6.00f, 6.00f);
	style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
	style.IndentSpacing = 25;
	style.ScrollbarSize = 15;
	style.GrabMinSize = 10;
	style.WindowBorderSize = 1;
	style.ChildBorderSize = 1;
	style.PopupBorderSize = 1;
	style.FrameBorderSize = 1;
	style.TabBorderSize = 1;
	style.WindowRounding = 7;
	style.ChildRounding = 4;
	style.FrameRounding = 3;
	style.PopupRounding = 4;
	style.ScrollbarRounding = 9;
	style.GrabRounding = 3;
	style.LogSliderDeadzone = 4;
	style.TabRounding = 4;
}

bool _render::imgui()
{
	DXGI_SWAP_CHAIN_DESC scd = {};
	scd.BufferCount = 2;
	scd.BufferDesc.Width = 0;
	scd.BufferDesc.Height = 0;
	scd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	scd.BufferDesc.RefreshRate.Numerator = 60;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = hwid->window_handle;
	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;
	scd.Windowed = 1;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	D3D_FEATURE_LEVEL d3d_feature_lvl;
	const D3D_FEATURE_LEVEL d3d_feature_array[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };

	D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, d3d_feature_array, 2, D3D11_SDK_VERSION, &scd, &hwid->d3d_swap_chain, &hwid->d3d_device, &d3d_feature_lvl, &hwid->d3d_device_ctx);

	ID3D11Texture2D* pBackBuffer;
	hwid->d3d_swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
	hwid->d3d_device->CreateRenderTargetView(pBackBuffer, nullptr, &hwid->d3d_render_target);
	pBackBuffer->Release();

	ImGui::CreateContext();
	embraceTheDarkness();
	ImGuiIO& io = ImGui::GetIO();
	_render::Backround_drawlist = ImGui::GetBackgroundDrawList();

	ImFontConfig font_cfg;
	font_cfg.FontDataOwnedByAtlas = false; 
	io.Fonts->AddFontFromMemoryTTF((void*)fnfont, sizeof(fnfont), 18.0f, &font_cfg);

	ImGui_ImplWin32_Init(hwid->window_handle);
	ImGui_ImplDX11_Init(hwid->d3d_device, hwid->d3d_device_ctx);

	config.FontDataOwnedByAtlas = false;

	hwid->d3d_device->Release();

	return true;
}



ImFont* GlobalFont;
ImVec2 position;

int tab = 0;


int selectedHitbox = 0;

bool IsKeyPressed(int key) {
	return GetAsyncKeyState(key) & 0x8000;
}

void menu()
{
	embraceTheDarkness();

	if (GetAsyncKeyState(VK_INSERT) & 1)
		globals.render_menu = !globals.render_menu;

	if (globals.render_menu)
	{
		ImGui::SetNextWindowSize(ImVec2(500, 500));
		ImGui::Begin(" ", &globals.render_menu, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);

		ImGui::SetCursorPos(ImVec2(5, 5));
		if (ImGui::Button("Aimbot", ImVec2(160, 50))) tab = 0;
		ImGui::SameLine();
		if (ImGui::Button("Visuals", ImVec2(160, 50))) tab = 1;
		ImGui::SameLine();
		if (ImGui::Button("Exploits", ImVec2(160, 50))) tab = 2;

		if (tab == 0)
		{
			ImGui::Spacing();
			ImGui::Checkbox("Aimbot", &a1m::aimbot);
			ImGui::Checkbox("Team Check", &a1m::team_check);
			ImGui::Checkbox("Visible Only", &a1m::visible_check);
			ImGui::Checkbox("Prediction", &a1m::prediction);
			ImGui::SliderFloat("Aim FOV", &a1m::fov, 0.1f, 500.0f, "%.3g", NULL);
			ImGui::SliderFloat("Aim Smoothing", &a1m::smoothing, 1.0f, 35.0f, "%.2g", NULL);

			const char* hitboxNames[] = { "Head", "Neck", "Chest", "Pelvis", "Closest" };
			ImGui::Combo("Hitbox", &selectedHitbox, hitboxNames, IM_ARRAYSIZE(hitboxNames));

			a1m::hitbox = selectedHitbox;
			ImGui::Spacing();
			ImGui::Text("Aim Bind:");
			aimkey::HotkeyButton();
		

		}
		else if (tab == 1)
		{
			ImGui::Spacing();
			ImGui::Checkbox("Team Check", &esp::team_check);
			ImGui::Checkbox("Box", &esp::box);
			ImGui::Checkbox("Skeleton", &esp::skeleton);
			ImGui::Checkbox("Platform", &esp::platform);
			ImGui::Checkbox("Username", &esp::name);
			ImGui::Checkbox("Distance", &esp::distance);
			ImGui::Checkbox("Render FOV", &esp::render_fov);
		}
		else if (tab == 2)
		{
			ImGui::Spacing();
			ImGui::Checkbox("shoot through walls", &exploit::Magic);
		}
	}
}
inline MARGINS window_margin = { -1 };

bool _render::hijack()
{
	hwid->window_handle = LI_FN(FindWindowA).forwarded_safe_cached()("MedalOverlayClass", "MedalOverlay");
	if (!hwid->window_handle) return false;

	DwmExtendFrameIntoClientArea(hwid->window_handle, &window_margin);

	SetWindowPos(hwid->window_handle, 0, 0, 0, ue5::width, ue5::height, 0);

	LI_FN(UpdateWindow).forwarded_safe_cached()(hwid->window_handle);

	return true;
}
const char* brand = "luminox";

void render_loop()
{
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 0.f);

	ImGui::StyleColorsDark();

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuiIO& io = ImGui::GetIO();

	ImGui::PushFont(io.Fonts->Fonts[1]);

	ImGui::GetBackgroundDrawList()->AddText(ImVec2(10, 10), ImColor(255, 255, 255), brand);

	menu();

	loop::actor_loop();

	ImGui::PopFont();

	ImGui::Render();

	const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
	hwid->d3d_device_ctx->OMSetRenderTargets(1, &hwid->d3d_render_target, nullptr);
	hwid->d3d_device_ctx->ClearRenderTargetView(hwid->d3d_render_target, clear_color_with_alpha);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	hwid->d3d_swap_chain->Present(1, 0);
}

bool _render::render()
{
	static RECT rect_og;
	MSG msg = { NULL };
	ZeroMemory(&msg, sizeof(MSG));

	while (true)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.DeltaTime = 1.0f / 60.0f;

		POINT p_cursor;
		GetCursorPos(&p_cursor);
		io.MousePos.x = p_cursor.x;
		io.MousePos.y = p_cursor.y;

		if (GetAsyncKeyState(VK_LBUTTON))
		{
			io.MouseDown[0] = true;
			io.MouseClicked[0] = true;
			io.MouseClickedPos[0].x = io.MousePos.x;
			io.MouseClickedPos[0].x = io.MousePos.y;
		}
		else
			io.MouseDown[0] = false;

		send_frame_buffer(hwid->d3d_swap_chain);
		render_loop();
	}

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	DestroyWindow(hwid->window_handle);

	return true;
}