#pragma once
#include "assault_cube.h"
#include "glDraw.h"
#include "gltext.h"
#include "geom.h"

const int VIRTUAL_SCREEN_WIDTH = 800;
const int GAME_UNIT_MAGIC = 400;

const float PLAYER_HEIGHT = 5.25f;
const float PLAYER_WIDTH = 2.0f;
const float EYE_HEIGHT = 4.5f;

const float PLAYER_ASPECT_RATIO = PLAYER_HEIGHT / PLAYER_WIDTH;
const int ESP_FONT_HEIGHT = 15;
const int ESP_FONT_WIDTH = 9;

const int ESP_HEALTH_HEIGHT = 15;
const int ESP_HEALTH_WIDTH = 2;
const float PLAYER_HEALTH_RATIO = ESP_HEALTH_HEIGHT / ESP_HEALTH_WIDTH;
class ESP
{
public:
	int* gameMode = (int*)(0x50F49C);
	int* numOfPlayers = (int*)(0x50f500);
	float* matrix = (float*)(0x501AE8);
	ent* localPlayer = *(ent**)0x50F4F4;
	entList* entlist = *(entList**)0x50F4F8;

	int viewport[4];

	bool IsTeamGame();
	bool IsEnemy(ent* e);
	bool isValidEnt(ent* ent);

	void DrawESPBox(ent* p, vec3, GL::Font& font);
	//void ESP::DrawESPHealth(ent* e, vec3 screen, GL::Font& font);
	void Draw(GL::Font& font);
};

