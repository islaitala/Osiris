#include "esp.h"
#include "vector3.h"
#include "settings.h"

extern Settings settings;

bool ESP::IsTeamGame()
{
	return true;
}

bool ESP::IsEnemy(Player* e)
{
	if (localPlayer == nullptr)
	{

		return true;
	}
	return localPlayer->team != e->team;
}

bool ESP::IsValidEnt(Player* ent)
{
	return true;
}

void ESP::DrawCircle(vector3d screen)
{
	return;
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i <= 300; i++) {
		double angle = 2 * 3.14159265358979323846f * i / 300;
		double x = cos(angle) * settings.fov * 15.f + settings.SCREEN_WIDTH/2.f ;
		double y = sin(angle) * settings.fov * 15.f + settings.SCREEN_HEIGHT/2.f;

		glVertex2d(x, y);
	}
	glEnd();
}

void ESP::DrawESPBox(Player* e, vector3d screen, GL::Font& font)
{
	const GLubyte* color = nullptr;

	if (IsEnemy(e))
		color = rgb::red;
	else
		color = rgb::green;

	// dist between enemy and player
	vector3d localPlayerPos(localPlayer->pos_x, localPlayer->pos_y, localPlayer->pos_z);
	vector3d enemyPlayerPos(e->pos_x, e->pos_y, e->pos_z);
	float dist = localPlayerPos.distance(enemyPlayerPos);

	float scale = (GAME_UNIT_MAGIC / dist) * (viewport[2] / VIRTUAL_SCREEN_WIDTH);
	float x = screen.x - scale;
	float y = screen.y - scale * PLAYER_ASPECT_RATIO;
	float width = scale * 2;
	float height = scale * PLAYER_ASPECT_RATIO * 2;

	GL::DrawOutline(x, y, width, height, 2.0f, color);

	if (!settings.espDrawNames) return;

	float textX = font.centerText(x, width, strlen(e->name) * ESP_FONT_WIDTH);
	float textY = y - ESP_FONT_HEIGHT / 2;
	font.Print(textX, textY, color, "%s", e->name);
}

void ESP::DrawHealthBar(Player* e, vector3d screen)
{
	// dist between enemy and player
	vector3d localPlayerPos(localPlayer->pos_x, localPlayer->pos_y, localPlayer->pos_z);
	vector3d enemyPlayerPos(e->pos_x, e->pos_y, e->pos_z);
	float dist = localPlayerPos.distance(enemyPlayerPos);
	float scale = (GAME_UNIT_MAGIC / dist) * (viewport[2] / VIRTUAL_SCREEN_WIDTH);
	float x = screen.x - scale;
	float y = screen.y - scale * PLAYER_ASPECT_RATIO;
	float width = scale * 2;
	float height = scale * PLAYER_ASPECT_RATIO * 2;

	const GLubyte* color = rgb::lightgray;
	GL::DrawFilledRect(x - scale, y, width * 0.15f, height, color);
	GL::DrawFilledRect(x - scale, y + (height - height * (e->health / 100.f)), width * 0.15f, height* (e->health / 100.f), rgb::green);
}

void ESP::Draw(GL::Font& font)
{
	glGetIntegerv(GL_VIEWPORT, viewport);

	for (size_t i = 0; i < playerCount; i++)
	{
		uintptr_t player_addr = *(uintptr_t*)(entityList + i * 4);

		Player* e = (Player*)player_addr;

		if (e == nullptr || e->is_dead) continue;

		vector3d center(e->head_x, e->head_y, e->head_z);
		center.z = center.z - EYE_HEIGHT + PLAYER_HEIGHT / 2;

		vector3d screenCoords;

		if (WorldToScreen(center, screenCoords, matrix, viewport[2], viewport[3]))
		{
			DrawESPBox(e, screenCoords, font);
			
			if (settings.espDrawHealthBar)
				DrawHealthBar(e, screenCoords);

			DrawCircle(screenCoords);
		}
	}
}