#pragma once
#include <stdint.h>

// old
//class Player
//{
//public:
//    char pad_0000[4]; //0x0000
//    float head_x; //0x0004
//    float head_y; //0x0008
//    float head_z; //0x000C
//    char pad_0010[24]; //0x0010
//    float pos_x; //0x0028
//    float pos_y; //0x002C
//    float pos_z; //0x0030
//    float view_yaw; //0x0034
//    float view_pitch; //0x0038
//    char pad_003C[58]; //0x003C
//    bool is_dead; //0x0076
//    char pad_0077[117]; //0x0077
//    int32_t health; //0x00EC
//    char pad_00F0[276]; //0x00F0
//    bool b_shooting; //0x0204
//    char name[16]; //0x0205
//    char pad_0205[247];
//    int32_t team; //0x030C
//    char pad_0310[304]; //0x0310
//}; //Size: 0x0440/Size: 0x0442

class Player
{
public:
	char pad_0000[4]; //0x0000
	float head_x; //0x0004
	float head_y; //0x0008
	float head_z; //0x000C
	char pad_0010[24]; //0x0010
	float pos_x; //0x0028
	float pos_y; //0x002C
	float pos_z; //0x0030
	float view_yaw; //0x0034
	float view_pitch; //0x0038
	char pad_003C[58]; //0x003C
	bool is_dead; //0x0076
	char pad_0077[117]; //0x0077
	int32_t health; //0x00EC
	char pad_00F0[256]; //0x00F0
	int32_t lastaction; //0x01F0
	char pad_01F4[16]; //0x01F4
	bool bShooting; //0x0204
	char name[16]; //0x0205
	char pad_0215[247]; //0x0215
	int32_t team; //0x030C
	char pad_0310[84]; //0x0310
	class weapon* currWeapon; //0x0364
	char pad_0368[60]; //0x0368
}; //Size: 0x03A4



class weapon
{
public:
	char pad_0000[12]; //0x0000
	char* name; //0x000C
	class weapon_data* N000002FD; //0x0010
	int32_t* N000002FE; //0x0014
	int32_t* gunWait; //0x0018
	char pad_001C[260]; //0x001C
}; //Size: 0x0120

class weapon_data
{
public:
	int32_t spare_ammo; //0x0000
	char pad_0004[32]; //0x0004
	int32_t ammo; //0x0024
};