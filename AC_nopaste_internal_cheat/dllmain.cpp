#include <Windows.h>
#include <iostream>
#include <string>
#include "offsets.h"
#include <cmath>
#include "vector3.h"
#include <math.h>
#include "useful.h"
#include "hook.h"
#include "player.h"
#include "ESP.h"
#define PI 3.141592653589793238463
#include <stdio.h>
#include "menu.h"
#include "settings.h"
#include "scanner.h"
Hooker hooker;
ESP esp;
bool bRunning = true;
typedef BOOL(__stdcall* twglSwapBuffers)(HDC hdc);
typedef bool(__thiscall* _GunAttackFunc)(weapon* _this, vector3d& vec);
Scanner scanner;
twglSwapBuffers owglSwapBuffers;
_GunAttackFunc gunAttackFunc;

DWORD prevTicks = 0;
bool aimbot_enabled = true;
GL::Font glFont;
const int FONT_HEIGHT = 15;
const int FONT_WIDTH = 9;
const char* example = "ESP Box";
const char* example2 = "I'm inside fam";
Menu* myMenu = new Menu(500, 500);
Settings settings;
enum class AIM_TARGETING
{
    CLOSEST,
    CLOSEST_CROSSHAIR
};

struct NoRecoilData
{
    uintptr_t offset = 0xC520A;
    BYTE bytes[3]{ 0x83, 0xC4, 0x08 };
} _NoRecoil;

AIM_TARGETING aimbotTargeting = AIM_TARGETING::CLOSEST_CROSSHAIR;


float DifferenceOfAngles(vector3d to, vector3d from)
{
    vector3d vdifference;
    vdifference.y = from.y - to.y;
    vdifference.x = from.x - to.x;

    //normalize by making them positive values if they are negative
    if (vdifference.y < 0)
    {
        vdifference.y *= -1;
    }
    if (vdifference.x < 0)
    {
        vdifference.x *= -1;
    }

    //add them together and divide by 2, gives an average of the 2 angles
    float fDifference = (vdifference.y + vdifference.x) / 2;
    return fDifference;
}
Player* aimbotTarget;
void AimAt(Player* localPlayer, Player* enemy)
{
    if (!settings.aimbot || enemy == nullptr) return;

    aimbotTarget = enemy;

    vector3d closest_player_pos(enemy->pos_x, enemy->pos_y, enemy->head_z);
    vector3d local_player_pos(localPlayer->pos_x, localPlayer->pos_y, localPlayer->head_z);
    vector3d angles = CalcAngle(local_player_pos, closest_player_pos);

    if (!settings.aimbotSmoothing || settings.aimbotSmoothAmount == 0)
    {
        localPlayer->view_pitch = angles.y;
        localPlayer->view_yaw = angles.x;
        return;
    }
    
    float diffPitch = angles.y - localPlayer->view_pitch;
    float diffYaw = angles.x - localPlayer->view_yaw;
    localPlayer->view_pitch += diffPitch / settings.aimbotSmoothAmount;
    localPlayer->view_yaw += diffYaw / settings.aimbotSmoothAmount;
}

USHORT nextTimeCanShootPrev_ms = 0;
BYTE* originalReloadBytes;
Patch* noreloadPatch;
BOOL __stdcall hkwglSwapBuffers(HDC hdc)
{
    uintptr_t base = (uintptr_t)GetModuleHandle(0);
    uintptr_t local_player_addr = *(uintptr_t*)(base + off::local_player);
    Player* local_player = (Player*)local_player_addr;

    esp.localPlayer = local_player;
    uintptr_t entity_list = *(uintptr_t*)(base + off::entity_list);
    esp.entityList = entity_list;
    size_t player_count = *(uintptr_t*)(base + off::player_count);
    esp.playerCount = player_count;
    esp.matrix = (float*)(base + 0x17AFE0);

    if (GetAsyncKeyState(VK_DELETE) & 1)
    {
        hooker.Unhook();
        bRunning = false;
        return FALSE;
    }

    //if (local_player != nullptr && local_player->currWeapon != nullptr && local_player->currWeapon->weaponData != nullptr)
    //{
    //    if (local_player->currWeapon->weaponData->ammo == 1)
    //    {
    //        noreloadPatch->Enable();
    //        nextTimeCanShoot_ms = 0;
    //    }
    //}
    //
    
    DWORD ticks = GetTickCount();
    if (((GetKeyState(VK_LBUTTON) & 0x8000) != 0 && settings.currentAimbotLockingMethod == 0)
        || (GetKeyState(VK_MENU) & 0x8000) != 0 && settings.currentAimbotLockingMethod == 1) // left btn held
    {

        if ((GetKeyState(VK_LBUTTON) & 0x8000) != 0 && settings.forceFullAuto && local_player != nullptr && local_player->currWeapon != nullptr)
        {
            if (strcmp(local_player->currWeapon->name, "pistol") == 0 ||
                strcmp(local_player->currWeapon->name, "shotgun") == 0 || 
                strcmp(local_player->currWeapon->name, "carbine") == 0 || 
                strcmp(local_player->currWeapon->name, "sniper") == 0)
            {
                local_player->bShooting = true;
            }
            
        }
        if (aimbotTarget != nullptr && aimbotTarget->is_dead)
            aimbotTarget = nullptr;

        if (aimbotTarget == nullptr)
        {
            vector3d local_player_pos(local_player->pos_x, local_player->pos_y, local_player->head_z);
            switch (settings.currentTargetingMethod)
            {
            case 1:
            {
                // 1. loop through all players (in different team) and get the closest one
                float highest = 99999999;
                Player* closest_player = nullptr;
                for (size_t i = 0; i < player_count; i++)
                {
                    uintptr_t player_addr = *(uintptr_t*)(entity_list + i * 4);
                    Player* player = (Player*)player_addr;

                    if (player == nullptr) continue;

                    // dont consider teammates as enemies and dont shoot dead players
                    if ((player->team == local_player->team && !settings.aimbotEveryoneIsEnemy) || player->is_dead)
                        continue;

                    vector3d vec3(player->pos_x, player->pos_y, player->head_z);
                    float dist = vec3.distance(local_player_pos);

                    if (dist < highest)
                    {
                        highest = dist;
                        closest_player = player;
                    }
                }
                
                if (closest_player != nullptr && local_player->currWeapon != nullptr && settings.currentAimbotMode == 0)
                {
                    vector3d playerpos(closest_player->pos_x, closest_player->pos_y, closest_player->head_z);

                    // might cause exception
                    local_player->bShooting = true;
                    bool ret = gunAttackFunc(local_player->currWeapon, playerpos);
                    local_player->bShooting = false;
                    std::cout << "return value: " << ret << std::endl;
                }
                else
                    AimAt(local_player, closest_player);
                break;
            }
            case 0:
            {
                // 1. loop through all players (in different team) and get the closest one
                float highest = FLT_MAX;
                Player* closest_player = nullptr;
                for (size_t i = 0; i < player_count; i++)
                {
                    uintptr_t player_addr = *(uintptr_t*)(entity_list + i * 4);
                    Player* player = (Player*)player_addr;

                    if (player == nullptr) continue;

                    // dont consider teammates as enemies and dont shoot dead players
                    if ((player->team == local_player->team && !settings.aimbotEveryoneIsEnemy) || player->is_dead)
                        continue;

                    vector3d playerpos(player->pos_x, player->pos_y, player->head_z);

                    vector3d nigga = CalcAngle(local_player_pos, playerpos);
                    vector3d localPlayerAngles(local_player->view_yaw, local_player->view_pitch, 0.f);
                    float dist = DifferenceOfAngles(nigga, localPlayerAngles);

                    if (dist < highest)
                    {
                        highest = dist;
                        closest_player = player;
                    }
                }
                if (closest_player != nullptr && local_player->currWeapon != nullptr && settings.currentAimbotMode == 0)
                {
                    vector3d playerpos(closest_player->pos_x, closest_player->pos_y, closest_player->head_z);

                    // might cause exception
                    local_player->bShooting = true;
                    bool ret = gunAttackFunc(local_player->currWeapon, playerpos);
                    local_player->bShooting = false;

                }
                else
                    AimAt(local_player, closest_player);
                break;
            }
            case 2: // fov
            {
                // 1. loop through all players (in different team) and get the closest one
                float highest = FLT_MAX;
                Player* closest_player = nullptr;
                for (size_t i = 0; i < player_count; i++)
                {
                    uintptr_t player_addr = *(uintptr_t*)(entity_list + i * 4);
                    Player* player = (Player*)player_addr;

                    if (player == nullptr) continue;

                    // dont consider teammates as enemies and dont shoot dead players
                    if ((player->team == local_player->team && !settings.aimbotEveryoneIsEnemy) || player->is_dead)
                        continue;

                    vector3d playerpos(player->pos_x, player->pos_y, player->head_z);

                    vector3d nigga = CalcAngle(local_player_pos, playerpos);
                    vector3d localPlayerAngles(local_player->view_yaw, local_player->view_pitch, 0.f);
                    float dist = DifferenceOfAngles(nigga, localPlayerAngles);

                    if (dist < highest)
                    {
                        highest = dist;
                        closest_player = player;
                    }
                }
                if (highest > settings.fov) break;

                if (closest_player != nullptr && local_player->currWeapon != nullptr && settings.currentAimbotMode == 0)
                {
                    vector3d playerpos(closest_player->pos_x, closest_player->pos_y, closest_player->head_z);

                    // might cause exception
                    local_player->bShooting = true;
                    bool ret = gunAttackFunc(local_player->currWeapon, playerpos);
                    local_player->bShooting = false;
                    std::cout << "return value: " << ret << std::endl;
                }
                else
                    AimAt(local_player, closest_player);
                break;
            }

            }
        }
        else
        {
            AimAt(local_player, aimbotTarget);
        }
    }
    else
    {
        aimbotTarget = nullptr;
    }
    
    HDC currentHDC = wglGetCurrentDC();

    if (!glFont.bBuilt || currentHDC != glFont.hdc)
    {
        glFont.Build(FONT_HEIGHT);
    }

    GL::SetupOrtho();

    // <esp here>
    if (settings.esp)
        esp.Draw(glFont);
    // </esp here>

    GL::RestoreGL();    
    myMenu->Render();
    return owglSwapBuffers(hdc);
}

DWORD WINAPI main_thread(HMODULE hModule)
{
    FILE* _file;
    AllocConsole();
    freopen_s(&_file, "CONOUT$", "w", stdout);

    myMenu->Init();
    myMenu->Toggle();
    owglSwapBuffers = (twglSwapBuffers)GetProcAddress(GetModuleHandle(L"opengl32.dll"), "wglSwapBuffers");
    owglSwapBuffers = (twglSwapBuffers)hooker.TrampHook((BYTE*)owglSwapBuffers, (BYTE*)hkwglSwapBuffers, 5);
    
    uintptr_t moduleBase = (uintptr_t)GetModuleHandle(L"ac_client.exe");
    gunAttackFunc = (_GunAttackFunc)(moduleBase + off::attackFunc);
    //char* pattern = (char*)"\xA1\x0C\x7C\x58\x00\x83\xC4\x04\x8A\x48\x76\x88\x4C\x24\x3E\x39\x05\xB8\xB0\x57\x00";
    //char* mask = (char*)"\xA1????\x83??\x8A??\x88???\x39?????";
    //const size_t patternAndMaskLen = 21;
    //char* match = scanner.ScanModIn(pattern, mask, "ac_client.exe");

    //

    //if (match != nullptr)
    //{
    //    // grab local player ptr

    //    uintptr_t localPlayerAddr = *(uintptr_t*)(match + patternAndMaskLen - 4);

    //    uintptr_t offsetFromModuleBase = localPlayerAddr - moduleBase;

    //    off::local_player = offsetFromModuleBase;
    //    std::cout << "local player offset grabbed: " << std::hex << off::local_player << std::endl;
    //    int a = 5;
    //}

    
    Patch* norecoilPatch = hooker.NewPatch((BYTE*)moduleBase + _NoRecoil.offset, _NoRecoil.bytes, 3, &settings.noRecoil);

    BYTE noreloadBytes[2]{ 0x90, 0x90 };
    Patch* noreloadPatch = hooker.NewPatch((BYTE*)moduleBase + off::noreload, noreloadBytes, 2, &settings.noReload);

    while (bRunning)
    {
        if (GetAsyncKeyState(VK_INSERT) & 1) {
            bool active = myMenu->Toggle();
        }

        Patch::WatchToggles();
        Sleep(1);
    }
    Patch::DisableAllPatches();
    myMenu->Unload();
    fclose(_file);
    FreeConsole();
    FreeLibraryAndExitThread(hModule,0);

    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)main_thread, hModule, 0, NULL);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}