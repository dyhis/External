#pragma once
#include <utility>
#include <cstddef>
#include <intrin.h>
#include <string>
#include <utility>
#include <d3dx11tex.h>
#include <d3d11.h>
#include <dwmapi.h>
#include <iostream>

#include "../../backend/ImGui/imgui_impl_dx11.h"
#include "../../backend/ImGui/imgui_impl_win32.h"
#include "../../backend/ImGui/imgui_internal.h"

#pragma comment(lib, "dwmapi")
#pragma comment (lib, "d3d11.lib")

class _discord 
{
public:
	static bool init();
};
inline _discord* _discord_ = new _discord();

class _render
{
private:

public:

	static inline ImDrawList* Backround_drawlist;

	static inline HWND windowhwnd;

	static bool hijack();

	static bool imgui();

	static bool render();


};

