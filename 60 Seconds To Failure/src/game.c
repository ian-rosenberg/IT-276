#include <SDL.h>
#include <stdio.h>

#include "simple_logger.h"
#include "gui.h"
#include "sprites.h"
#include "player.h"
#include "tilemap.h"
#include "worlds.h"
#include "camera.h"
#include "graphics.h"


int main(int agrc, char *arg[])
{
	Bool done = false;
	SDL_Event e;
	SDL_Rect dimensions = { 0,0,1280,720 };
	World *gameWorld;
	const Uint8 *keys;
	Vector4D  bgcolor = { 25, 128, 50, 255 };

	init_logger("60TF.log");
	slog("---==== BEGIN ====---");

	GraphicsInit(1280, 720, 0, 16, bgcolor);
	SetCameraDimensions(dimensions);
	SpriteManagerInit(1024);
	AnimationManagerInit(1024);
	gf2d_text_init("config/font.cfg");
	GUISetupHUD();
	ActorManagerInit(1024);
	EntityManagerInit(1024);
	TileInit(65536);
	TileMapInit(512);
	WorldManagerInit(256);
	gameWorld = WorldInit("config/overworld.cfg", "");
	PlayerInit();

	dimensions.w = gameWorld->overworld->cellWidth * gameWorld->overworld->numColumns;
	dimensions.h = gameWorld->overworld->cellHeight * gameWorld->overworld->numRows;

	SetCameraBounds(dimensions);
	SetCameraPosition(GetPlayerEntity()->position);

	PlayerSetOwner(gameWorld->overworld);

	AddEntityToTileMap(&GetPlayerEntity()->body, gameWorld->overworld);

	SDL_ShowCursor(SDL_DISABLE);

	while (!done)
	{		
		SDL_PumpEvents();
		keys = SDL_GetKeyboardState(NULL);

		EntityThinkAll();

		EntityUpdateAll();

		MapUpdate(gameWorld->overworld);

		ClearScreen();

		DrawMap(gameWorld->overworld);

		EntityDrawAll();

		GUIDrawHUD();

		gf2d_space_draw(gameWorld->overworld->mapSpace, GetCameraOffset());

		NextFrame();	

		if (keys[SDL_SCANCODE_ESCAPE])
		{
			done = true;
		}	
	}

	slog("---==== END ====---");

	return 0;
}