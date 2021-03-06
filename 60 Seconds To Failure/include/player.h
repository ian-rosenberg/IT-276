#ifndef __PLAYER__
#define __PLAYER__

#define PLAYER_IDLE_ANIMATION "idle"
#define PLAYER_WALKING_ANIMATION "walking"

#include "entity.h"
#include "entity_common.h"

typedef struct Player_S
{
	TextLine			name;
	Entity				*self;
	Entity				*owner;

	SDL_GameController	*controller;
	float				sensitivity;
	float				maxSpeed;

	Vector3D			dimensions;

	int32_t				axisLeftXLock;
	int32_t				axisLeftYLock;
	int32_t				axisRightXLock;
	int32_t				axisRightYLock;
}Player;


/**
* @brief Init the player entity
*/
void PlayerInit();

/**
* @brief Get the attached entity
* @returns The player's entity
*/
Entity* GetPlayerEntity();

/**
* @brief Handle input
*/
void PlayerThink(Entity *self);

/**
* @brief Draw the player to screen
*/
void PlayerDraw(Entity *self);

/**
* @brief Update the player's state
*/
void PlayerUpdate(Entity *self);

/**
* @brief Free the Player struct data
*/
void PlayerFree();

/**
* @brief Set the owner of an entity
* @param owner The tilemap to assign as owner
*/
void PlayerSetOwner(TileMap* owner);

//Test for player collision
int PlayerTouch(struct Body_S *self, List *collision);
#endif