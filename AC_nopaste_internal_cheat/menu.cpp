#include "Menu.h"
#include "settings.h"
extern Settings settings;

struct MenuConfig {
	bool menuActive = false;
	WNDPROC GameWindowProc = NULL;
}menuConf;

LRESULT __stdcall hWindProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	//pass message to imgui if menu active
	if (menuConf.menuActive) {
		if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) {
			return true;
		}
	}

	return CallWindowProc(menuConf.GameWindowProc, hWnd, uMsg, wParam, lParam);
}

Menu::Menu(int x, int y) {
	menuSize.x = x;
	menuSize.y = y;
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.WantCaptureKeyboard = true;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	myEsp = new ESP();
}

void Menu::Unload() {
	menuConf.menuActive = false;
	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	SetWindowLongPtr(FindWindowA(NULL, "AssaultCube"), GWLP_WNDPROC, (LONG_PTR)menuConf.GameWindowProc);
}

void Menu::Init() {
	HWND hWnd = NULL;
	while (hWnd == NULL) {
		hWnd = FindWindowA(NULL, "AssaultCube");
	}
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplOpenGL2_Init();
	ImGuiStyle* myStyle = &ImGui::GetStyle();
	ImVec4* colors = myStyle->Colors;
	myStyle->WindowMinSize = ImVec2(300, 300);
	myStyle->WindowPadding = ImVec2(15, 15);
	myStyle->WindowRounding = 10.0f;
	myStyle->WindowPadding = ImVec2(6, 6);

	ImGui::SetNextWindowSize(menuSize, 0);

	// set hWindProc as new address for window procedure
	menuConf.GameWindowProc = (WNDPROC)SetWindowLongPtr(hWnd, GWL_WNDPROC, (LONG_PTR)hWindProc);
}
void Menu::Render() {
		if (menuConf.menuActive) {
		ImGuiIO& io = ImGui::GetIO();
		
		ImGui_ImplOpenGL2_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		ImGui::Begin("AC - Osiris", &menuConf.menuActive);
		
		static bool test = false;
		
		ImGui::BeginTabBar("tabbar");
		if (ImGui::BeginTabItem("ESP")) {
			ImGui::Checkbox("ENABLE###ESP1", &settings.esp);
			ImGui::Checkbox("Draw health bar###ESP2", &settings.espDrawHealthBar);
			ImGui::Checkbox("Draw names###ESP3", &settings.espDrawNames);
			
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("AIMBOT")) {
			ImGui::Checkbox("ENABLE###AIMBOT123", &settings.aimbot);
			ImGui::Combo("Mode", &settings.currentAimbotMode, settings.currentAimbotModes, IM_ARRAYSIZE(settings.currentAimbotModes));
			ImGui::Checkbox("Smoothing###AIMBOT142", &settings.aimbotSmoothing);

			if (settings.aimbotSmoothing)
			{
				ImGui::SliderFloat("Smooth amount#smoothamount111", &settings.aimbotSmoothAmount, 1.f, 5.f);
			}
			ImGui::Combo("Locking method", &settings.currentAimbotLockingMethod, settings.aimbotLockingMethods, IM_ARRAYSIZE(settings.aimbotLockingMethods));

			ImGui::Combo("Targeting method", &settings.currentTargetingMethod, settings.aimbotTargetingMethods, IM_ARRAYSIZE(settings.aimbotTargetingMethods));
			
			if (settings.currentTargetingMethod == 2) // fov
			{
				ImGui::SliderFloat("Fov###float1", &settings.fov, 0.f, 100.f);
			}

			ImGui::Checkbox("Everyone is enemy###aim233123123", &settings.aimbotEveryoneIsEnemy);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("WEAPON")) {
			ImGui::Checkbox("No recoil###WEAPON1", &settings.noRecoil);
			ImGui::Checkbox("Force fullauto###weapon123", &settings.forceFullAuto);
			ImGui::Checkbox("No reload###weapon12341234", &settings.noReload);
			//ImGui::Checkbox("Rapid fire###weapon2", &settings.rapidFire);

			if (settings.rapidFire)
			{
				//ImGui::SliderFloat("Speed###weaponf3", &settings.fov, 1.f, 5.f);

			}

			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
	}
}
bool Menu::Toggle() {
	menuConf.menuActive = !menuConf.menuActive;
	return menuConf.menuActive;
}