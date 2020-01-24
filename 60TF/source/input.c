#include "input.h"

PlayerInput HandleInput(const Uint8 *keys)
{
	SDL_PumpEvents();

	keys = SDL_GetKeyboardState(NULL);

	if (keys[SDL_SCANCODE_ESCAPE])
	{
		return Quit;
	}
	if (keys[SDL_SCANCODE_W])
	{
		return MoveNorth;
	}
	if (keys[SDL_SCANCODE_A])
	{
		return MoveWest;
	}
	if (keys[SDL_SCANCODE_S])
	{
		return MoveSouth;
	}
	if (keys[SDL_SCANCODE_D])
	{
		return MoveEast;
	}
	if (keys[SDL_SCANCODE_Q])
	{
		return Cancel;
	}
	if (keys[SDL_SCANCODE_E])
	{
		return Interact;
	}
	if (keys[SDL_SCANCODE_F])
	{
		return Menu;
	}
	
	return NULL;
}

