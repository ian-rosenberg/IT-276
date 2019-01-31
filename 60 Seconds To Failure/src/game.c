#include <SDL.h>
#include <stdio.h>

#include "simple_logger.h"
#include "sprites.h"
#include "graphics.h"

int main(int agrc, char *arg[])
{
	Bool done = false;
	SDL_Event e;
	const Uint8 *keys;

	init_logger("60TF.log");
	slog("---==== BEGIN ====---");

	Graphics_Init(1280, 720, 1);

	SpriteManagerInit(1024);

	SDL_ShowCursor(SDL_DISABLE);

	while (!done)
	{
		SDL_PumpEvents();
		keys = SDL_GetKeyboardState(NULL);



		if (keys[SDL_SCANCODE_ESCAPE])
		{
			done = true;
		}
	}

	Graphics_Close();

	slog("---==== END ====---");

	return 0;
}