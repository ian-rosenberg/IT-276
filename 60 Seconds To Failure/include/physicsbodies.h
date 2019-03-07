#ifndef __PHYSICSBODIES__
#define __PHYSICSBODIES__

#include "gf2d_vector.h"
//#include "chipmunk/chipmunk.h"
#include <SDL.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define PLAYER_VELOCITY 10.0

#define PLAYER_GROUND_ACCEL_TIME 0.1
#define PLAYER_GROUND_ACCEL (PLAYER_VELOCITY/PLAYER_GROUND_ACCEL_TIME)

#define PLAYER_AIR_ACCEL_TIME 0.25
#define PLAYER_AIR_ACCEL (PLAYER_VELOCITY/PLAYER_AIR_ACCEL_TIME)

#define JUMP_HEIGHT 50.0
#define JUMP_BOOST_HEIGHT 55.0
#define FALL_VELOCITY 900.0
#define GRAVITY 2000.0

typedef struct Body_S
{
	Uint8			_inUse;

	//cpBody			*body;
	//cpShape			*shape;
	//cpDataPointer	*owner;
	//cpFloat			remainingJumpBoost;
	//cpBool			grounded;
	//cpBool			lastJumpState;
}BodyInfo;

/**
* @brief Initialize the physics body system
* @param max The maximum number of bodies to manage
*/
void BodyManagerInit(Uint16 max);

/**
* @brief Close the physics body system
*/
void BodyManagerClose();

/**
* @brief Delete a body and its shapes by reference
* @param body The body to destroy
*/
void DeleteBodyInfo(BodyInfo *body);

/**
* @brief Deletes all physics bodies in play
*/
void ClearAllBodies();

/**
* @brief Create a new physics body and associated shapes
* @param dimAndRadius Dimensions of box shape and radius 
* for beveled corners
* @param world The current level's worldspace
* @param jumpPhysics Set whether the player can jump or not initially
* @returns The body information for a particular entity
*/
BodyInfo* NewBodyInfo(Vector3D dimAndRadius,/* cpSpace *world,*/ Bool jumpPhysics);

#endif