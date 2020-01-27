#ifndef __PLAYER__
#define __PLAYER__

#include "entity.h"

typedef enum
{
	None = 0,
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


typedef struct Player_S
{		
	Entity				*self;

	SDL_GameController	*controller;

	PlayerInput			curInputState;
	PlayerInput			prevInputState;

	float				sensitivity;

	Vector3D			dimensions;
	Vector2D			enteredFrom;
	Vector2D			plannedMovement;

	int32_t				axisLeftXLock;
	int32_t				axisLeftYLock;
	int32_t				axisRightXLock;
	int32_t				axisRightYLock;

	const Uint8			*keys;
}Player;

void PlayerInit();

void PlayerThink(Entity *self);

void PlayerDraw(Entity *self);

void PlayerUpdate(Entity *self);

void SetInputState(PlayerInput state);

/**
* @brief Move the player based on keyboard inputs
*/
void MovePlayerTopDown();

void MovePlayerGravity();

Entity* GetPlayerEntity();

#endif