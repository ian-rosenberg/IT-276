#include <SDL.h>
#include <stdio.h>

#include "simple_logger.h"
#include "gui.h"
#include "sprites.h"
#include "player.h"
#include "worlds.h"
#include "camera.h"
#include "graphics.h"
#include "building_entity.h"


int main(int agrc, char *arg[])
{
	Bool done = false;
	SDL_Event e;
	SDL_Rect dimensions = { 0,0,1280,720 };
	const Uint8 *keys;
	Vector4D  bgcolor = { 25, 128, 50, 255 };

	init_logger("60TF.log");
	slog("---==== BEGIN ====---");

	GraphicsInit(1280, 720, 0, 16, bgcolor);
	SpriteManagerInit(1024);
	gf2d_text_init("config/font.cfg");
	GUISetupHUD();
	EntityManagerInit(1024);
	TileMapManagerInit(2);
	WorldInit("config/overworld.cfg", "config/sideview.cfg");
	PlayerInit();

	AddEntityToTileMap(GetPlayerEntity());

	SetCameraDimensions(dimensions);
	SetCameraBounds(dimensions);
	SetCameraPosition(GetPlayerEntity()->position);

	SDL_ShowCursor(SDL_DISABLE);

	while (!done)
	{		
		SDL_PumpEvents();

		keys = SDL_GetKeyboardState(NULL);

		EntityThinkAll();

		EntityUpdateAll();

		MapUpdate(GetCurrentMap());

		ClearScreen();
		

		DrawMap(GetCurrentMap());

		EntityDrawAll();

		GUIDrawHUD();

		gf2d_space_draw(GetCurrentMap()->mapSpace, GetCameraOffset());
		
		NextFrame();	

		if (keys[SDL_SCANCODE_ESCAPE])
		{
			done = true;
		}	
	}

	//CleanUpWorld(gameWorld);

	slog("---==== END ====---");

	return 0;
}