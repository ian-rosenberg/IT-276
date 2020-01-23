#ifndef __INPUT__
#define __INPUT__

#include <SDL.h>

#include "gf2d_types.h"

typedef enum 
{
	Interact = 1,
	Cancel = 2,
	Menu = 4,
	MoveNorth = 8,
	MoveSouth = 16,
	MoveEast = 32,
	MoveWest = 64, 
	Quit = 128
}PlayerInput;

/*
* @brief Handle player inputs
*/
PlayerInput HandleInput();

#endif