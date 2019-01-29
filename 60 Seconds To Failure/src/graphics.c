#include <SDL.h>

#include "simple_logger.h"
#include "graphics.h"

//make configurable
const int SCREEN_HEIGHT = 768;
const int SCREEN_WIDTH = 1024;

//Wrapper for graphics rendering
typedef struct
{
	SDL_Window			*window;
	SDL_Surface			*screenSurface;

}Graphics;


//temp color wrapper 
typedef struct
{
	Uint32 r;
	Uint32 g;
	Uint32 b;
	Uint32 a;
}Color;


static Graphics graphics;


void Graphics_Init()
{
	graphics.window = NULL;
	graphics.screenSurface = NULL;

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		slog("SDL failed to init! SDL_Error: %s", SDL_GetError());
	}
	else
	{
		graphics.window = SDL_CreateWindow("60 Seconds To Failure", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

		if (graphics.window == NULL)
		{
			slog("SDL window failed to create!");
		}
		else
		{
			graphics.screenSurface = SDL_GetWindowSurface(graphics.window);
		}
	}
}