#pragma once
#include <Windows.h>
#include "Imgui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_opengl2.h"
#pragma comment(lib, "OpenGL32.lib")
#include "ESP.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK hWindProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

class Menu
{
private:
	ESP* myEsp = nullptr;
	ImVec2 menuSize;
public:
	Menu(int x, int y);
	void Unload();
	void Init();
	void Render();
	bool Toggle();
};

