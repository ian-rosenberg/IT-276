#include "input.h"

PlayerInput HandleInput(const Uint8 *keys)
{
	PlayerInput flags;
	
	SDL_PumpEvents();

	keys = SDL_GetKeyboardState(NULL);

	if (keys[SDL_SCANCODE_ESCAPE])
	{
		flags |= Quit;
	}
	if (keys[SDL_SCANCODE_W])
	{
		flags |= MoveNorth;
	}
	if (keys[SDL_SCANCODE_A])
	{
		flags |= MoveWest;
	}
	if (keys[SDL_SCANCODE_S])
	{
		flags |= MoveSouth;
	}
	if (keys[SDL_SCANCODE_D])
	{
		flags |= MoveEast;
	}
	if (keys[SDL_SCANCODE_Q])
	{
		flags |= Cancel;
	}
	if (keys[SDL_SCANCODE_E])
	{
		flags |= Interact;
	}
	if (keys[SDL_SCANCODE_F])
	{
		flags |= Menu;
	}

	return flags;
}

