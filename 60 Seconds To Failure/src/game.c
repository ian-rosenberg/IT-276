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
	int mouseX, mouseY;
	Sprite* bobby = NULL;
	Animation* numbers = NULL;
	Vector2D mousePos = {0};
	Vector2D mouseScale;
	float prevTime, curTime;

	prevTime = 0;
	curTime = 0;

	mouseScale.x = 1;
	mouseScale.y = 1;
	prevTime = 0;
	curTime = 0;

	init_logger("60TF.log");
	slog("---==== BEGIN ====---");

	GraphicsInit(1280, 720, 0, 16);

	SpriteManagerInit(1024);

	numbers =  LoadAnimation("images/numbers.png", 
		GetRenderer(),
		27, 
		35,
		1, 
		10,
		16.0f);
	
	SDL_ShowCursor(SDL_DISABLE);

	while (!done)
	{
		prevTime = curTime;
		curTime = SDL_GetTicks();
		SDL_PumpEvents();
		keys = SDL_GetKeyboardState(NULL);

		slog("deltaTime: %f", curTime - prevTime);

		SDL_GetMouseState(&mouseX, &mouseY);

		mousePos.x = mouseX;
		mousePos.y = mouseY;
	
		//draw animated numbers

		ClearScreen();

		/*DrawAnimatedSprite(numbers,
			mousePos,
			&mouseScale,
			NULL,
			NULL,
			NULL,
			GetFrameDelay(),
			curTime - prevTime);*/

		DrawFramerate(numbers);

		NextFrame();	

		if (keys[SDL_SCANCODE_ESCAPE])
		{
			done = true;
		}	
	}

	slog("---==== END ====---");

	return 0;
}