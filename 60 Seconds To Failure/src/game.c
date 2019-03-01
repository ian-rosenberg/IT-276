#include <SDL.h>
#include <stdio.h>

#include "simple_logger.h"
#include "graphics.h"
#include "gui.h"
#include "player.h"
#include "tilemap.h"
#include "camera.h"


int main(int agrc, char *arg[])
{
	Bool done = false;
	SDL_Event e;
	TileMap *map;
	const Uint8 *keys;
	int mouseX, mouseY;
	Vector2D mousePos = {0};
	Vector2D mouseScale;
	Vector4D  bgcolor = { 25, 128, 50, 255 };
	float prevTime, curTime;
	prevTime = 0;
	curTime = 0;

	mouseScale.x = 1;
	mouseScale.y = 1;
	prevTime = 0;
	curTime = 0;

	init_logger("60TF.log");
	slog("---==== BEGIN ====---");

	GraphicsInit(1280, 720, 0, 16, bgcolor);
	SpriteManagerInit(1024);
	AnimationManagerInit(1024);
	gf2d_text_init("config/font.cfg");
	GUISetupHUD();
	ActorManagerInit(1024);
	EntityManagerInit(1024);
	TileInit(65536);
	TileMapInit(256);
	camera_set_dimensions(0, 0, 1280, 720);
	camera_set_bounds(0, 0, 64 * 32, 64 * 32);
	map = LoadOverworldTileMapFromFile("config/overworld.cfg");
	
	//Load level

	PlayerInit();

	SDL_ShowCursor(SDL_DISABLE);
	

	while (!done)
	{
		prevTime = curTime;
		curTime = SDL_GetTicks();
		SDL_PumpEvents();
		keys = SDL_GetKeyboardState(NULL);

		SDL_GetMouseState(&mouseX, &mouseY);

		mousePos.x = mouseX;
		mousePos.y = mouseY;
		
		EntityThinkAll();

		EntityUpdateAll();

		ClearScreen();

		DrawOverworldTileMap(map);

		EntityDrawAll();

		GUIDrawHUD();

		NextFrame();	

		if (keys[SDL_SCANCODE_ESCAPE])
		{
			done = true;
		}	
	}

	slog("---==== END ====---");

	return 0;
}