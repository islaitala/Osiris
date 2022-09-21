#pragma once
class Settings
{
public:
	size_t SCREEN_WIDTH;
	size_t SCREEN_HEIGHT;

	bool esp = true;
	bool espDrawNames = true;
	bool espDrawHealthBar = true;
	

	bool aimbot = true;
	const char* aimbotTargetingMethods[3] = { "closestCrosshair", "closest", "fov" };
	int currentTargetingMethod = 0;
	float fov;
	bool aimbotSmoothing = false;
	float aimbotSmoothAmount = 1.f;
	const char* aimbotLockingMethods[2] = { "LMB", "ALT"};
	int currentAimbotLockingMethod = 0;
	bool aimbotEveryoneIsEnemy = false;
	const char* currentAimbotModes[2] = { "rage", "snap" };
	int currentAimbotMode;

	bool noRecoil = true;
	bool forceFullAuto = true;
	bool rapidFire = false;
	float rapidFireAmount = 2.f;
	bool noReload = false;
};