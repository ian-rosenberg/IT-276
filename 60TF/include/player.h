#ifndef __PLAYER__
#define __PLAYER__

#include "entity.h"
#include "tilemap.h"

typedef struct Player_S
{		
	Entity				*self;

	TileMap				*owner;

	SDL_GameController	*controller;


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

void PlayerUpdate(Entity *self);

void PlayerDraw(Entity *self);

/**
* @brief Move the player based on keyboard inputs
*/
void MovePlayer();

#endif