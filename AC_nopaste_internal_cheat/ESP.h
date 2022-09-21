#pragma once
#include "glDraw.h"
#include "gltext.h"
#include "player.h"

//Scaling
const int VIRTUAL_SCREEN_WIDTH = 800;
const int GAME_UNIT_MAGIC = 400;

const float PLAYER_HEIGHT = 5.25f;
const float PLAYER_WIDTH = 2.0f;
const float EYE_HEIGHT = 4.5f;
const float PLAYER_ASPECT_RATIO = PLAYER_HEIGHT / PLAYER_WIDTH;

const int ESP_FONT_HEIGHT = 15;
const int ESP_FONT_WIDTH = 9;

class ESP
{
public:
	int* gameMode = (int*)(0x50F49C);
	float* matrix;
	bool IsEnemy(Player* e);
	bool IsValidEnt(Player* ent);
	uintptr_t entityList;

	Player* localPlayer;
	int playerCount;

	void DrawCircle(vector3d screen);

	bool IsTeamGame();
	int viewport[4];
	void DrawESPBox(Player* p, vector3d screen, GL::Font& font);
	void Draw(GL::Font& font);
	void DrawHealthBar(Player* e, vector3d screen);
}; 