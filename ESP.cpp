#include "ESP.h"
#include "assault_cube.h"

gameModes gamemodes;

 bool  ESP::IsTeamGame() {
	if ((gamemodes == 0 || gamemodes == 4 || gamemodes == 5 || gamemodes == 7 || gamemodes == 11 || gamemodes == 13 ||
		gamemodes == 14 || gamemodes == 16 || gamemodes == 17 || gamemodes == 20 || gamemodes == 21)) {
		return true;
	}
	else return false;
}
bool ESP::IsEnemy(ent* e) {
	if (localPlayer->team == e->team)
		return false;
	else
		return true;
}
bool ESP::isValidEnt(ent* ent) {
	if (ent) {
		if (ent->vTable == 0x4E4A98 || ent->vTable == 0x4E4AC0)
		{
			return true;
		}
	}
	 return false;
}

void ESP::DrawESPBox(ent* e, vec3 screen, GL::Font& font) {
	const GLubyte* color = nullptr;

	if (IsTeamGame() && !IsEnemy(e)) {
		color = rgb::green;
	}
	else color = rgb::red;

	float dist = localPlayer->pos.Distance(e->pos);

	float scale = (GAME_UNIT_MAGIC / dist) * (viewport[2] / VIRTUAL_SCREEN_WIDTH);
	float x = screen.x - scale;
	float y = screen.y - scale * PLAYER_ASPECT_RATIO;
	float width = scale * 2;
	float height = scale * PLAYER_ASPECT_RATIO * 2;

	GL::DrawOutline(x, y, width, height, 2.0f, color);

	float textX = font.centerText(x, width, strlen(e->name) * ESP_FONT_WIDTH);

	float textY = y - ESP_FONT_HEIGHT / 2;

	font.Print(textX, textY, color, "%s", e->name);
	
}
void ESP::Draw(GL::Font& font) {
	glGetIntegerv(GL_VIEWPORT, viewport);
	
	for (int i = 0; i < *numOfPlayers; i++) {
		if (isValidEnt(entlist->ents[i])) {

			ent* e = entlist->ents[i];
			vec3 center = e->head;
			center.z = center.z - EYE_HEIGHT + PLAYER_HEIGHT / 2;

			vec3 screenCoords;

			if (WorldToScreen(center, screenCoords, matrix, viewport[2], viewport[3]))
			{
				DrawESPBox(e, screenCoords, font);
			}
		}
	}
}

/*
void ESP::DrawESPHealth(ent* e, vec3 screen, GL::Font& font) {
	const GLubyte* color_health = rgb::green;
	const GLubyte* color_damage = rgb::red;

	float dist = localPlayer->pos.Distance(e->pos);

	

}*/