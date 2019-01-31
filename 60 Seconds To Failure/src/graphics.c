#include <SDL.h>

#include "simple_logger.h"
#include "graphics.h"
#include "sprites.h"

//Wrapper for graphics rendering
typedef struct
{
	SDL_Window			*window;
	SDL_Renderer		*renderer;

	Uint32				frameDelay;

}Graphics;


//temp color wrapper 
typedef struct
{
	Uint32 r;
	Uint32 g;
	Uint32 b;
	Uint32 a;
}Color;

//local global
static Graphics graphics;


//forward declarations
void Graphics_Close();

void Graphics_Init(Uint32 winWidth, Uint32 winHeight, Bool fullscreen)
{
	Uint32 flags = 0;
	
	graphics.window = NULL;
	graphics.renderer = NULL;

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		slog("SDL failed to init! SDL_Error: %s", SDL_GetError());

		return;
	}

	slog("SDL initialization success!");
	
	atexit(SDL_Quit);

	if (fullscreen)
	{
		if (winWidth == 0)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}
		else
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}
	}
		
	graphics.window = SDL_CreateWindow("60 Seconds To Failure", 
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
		winWidth, 
		winHeight,
		flags);


	if (!graphics.window)
	{
		slog("SDL window failed to create! %S", SDL_GetError());

		Graphics_Close();
	}

	graphics.renderer = SDL_CreateRenderer(graphics.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

	if (!graphics.renderer)
	{
		slog("Renderer was not created: %s", SDL_GetError());

		Graphics_Close();
	}

	SDL_SetRenderDrawColor(graphics.renderer, 16, 64, 192, 255);
	SDL_RenderClear(graphics.renderer);
	SDL_RenderPresent(graphics.renderer);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(graphics.renderer, winWidth, winHeight);

	srand(SDL_GetTicks());

	atexit(Graphics_Close);
	slog("Graphics initialized successfully");
}

void SetFrameDelay(Uint32 fDelay)
{
	graphics.frameDelay = fDelay;
}

SDL_Texture* LoadImageMedia(const char *filepath)
{
	SDL_Texture *tex = NULL;
	SDL_Surface *img = IMG_Load(filepath);

	if (!img)
	{
		slog("Failed to load image: %s", IMG_GetError());

		return NULL;
	}

	tex = SDL_CreateTextureFromSurface(graphics.renderer, tex);

	return img;
}

void Graphics_Close()
{
	if (graphics.renderer)
	{
		SDL_DestroyRenderer(graphics.renderer);
	}
	if (graphics.window)
	{
		SDL_DestroyWindow(graphics.window);
	}

	graphics.window = NULL;
	graphics.renderer = NULL;

	slog("SDL graphics shut down successfully");

	SDL_Quit();
}

