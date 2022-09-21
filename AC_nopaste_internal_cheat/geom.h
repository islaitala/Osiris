#pragma once
#include "vector3.h"
struct Vec4
{
	float x, y, z, w;
};
bool WorldToScreen(vector3d pos, vector3d& screen, float matrix[16], int windowWidth, int windowHeight);