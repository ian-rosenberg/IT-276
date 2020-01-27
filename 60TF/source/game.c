#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>

#include "graphics.h"
#include "sprites.h"
#include "simple_logger.h"
#include "entity.h"
#include "tilemap.h"
#include "player.h"
#include "camera.h"


int main(int agrc, char *arg[])
{
	Bool done = false;
	SDL_Rect dimensions = { 0, 0, 1280, 720 };
	const Uint8 *keys = NULL;
	Vector4D  bgcolor = { 25, 128, 50, 255 };
	TileMap *map = NULL;
	PlayerInput inputState;

	init_logger("60TF.log");
	slog("---==== BEGIN ====---");

	GraphicsInit(1280, 720, 0, 16, bgcolor);
	SpriteManagerInit(1024);
	gf2d_text_init("config/font.cfg");
	GUISetupHUD();

	EntityManagerInit(1024);
		
	TileMapManagerInit(1);

	map = LoadTileMapFromFile("config/overworld.cfg");

	PlayerInit();

	CameraInit(GetPlayerEntity()->position, GetRenderDimensions(), vector2d(GetCurrentTileMap()->boundingBox.w, GetCurrentTileMap()->boundingBox.h));


	//WorldInit("config/overworld.cfg", "config/sideview.cfg");

	//AddEntityToTileMap(GetPlayerEntity());

	//SetCameraDimensions(dimensions);


	//SetCameraBounds(GetCurrentTileMap()->boundingBox);


	//SetCameraPosition(GetPlayerEntity()->position);


	SDL_ShowCursor(SDL_DISABLE);

	while (!done)
	{
		inputState = HandleInput(keys);

		SetInputState(inputState);
		
		if (inputState == Quit)
		{
			done = true;
		}
		else
		{
			done = false;
		}

		EntityThinkAll();

		EntityUpdateAll();

		CameraMove(GetPlayerEntity());

		DrawMap(GetCurrentTileMap(), CameraMove(GetPlayerEntity()));

		EntityDrawAll();
		
		GUIDrawHUD();

		NextFrame();
	}

	//CleanUpWorld(GetGameWorld());

	slog("---==== END ====---");

	return 0;
}