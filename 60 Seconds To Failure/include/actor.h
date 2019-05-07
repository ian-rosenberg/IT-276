#ifndef __ACTOR__
#define __ACTOR__

#include <SDL.h>

#include "animations.h"

typedef struct AnimationData_S
{
	char			name[GF2DTEXTLEN];
	char			filepath[GF2DTEXTLEN];

	Uint32			length;
	Uint32			cellWidth, cellHeight, yOffset;

	Vector4D		colorSpecial;

	float			frameRate;

	AnimationType	animType;

	char			tempData[GF2DTEXTLEN];
}AnimationData;

typedef struct ActorData_S
{
	char			name[GF2DTEXTLEN];

	Vector4D		actorColor;

	Uint32			numAnim;

	char			tempData[GF2DTEXTLEN];
}ActorData;

typedef struct Actor_S
{
	Uint8			_inUse;

	char			*name;

	Vector2D		scale;

	Sprite			*currentSprite;

	Vector4D		color;

	Animation		*animations;

	Animation		*currentAnimation;

	Uint32			numAnimations;

	State			animState;
}Actor;

/**
* @brief Get a new actor list
* @param numAnim The number of animations to allocate
* @returns an unused/unreferenced actor list
*/
Actor* NewActor(Uint32 numAnim);

/**
* @brief Get a new actor list
* @param name Name of the actor
* @returns an unused/unreferenced actor list
*/
Actor* NewActorByName(char *name);

/**
* @brief Delete an actor from memory
* @param actor The actor to delete
*/
void DeleteActor(Actor *actor);

/**
* @brief Load an actor from file
* @param actor The actor to load into
* @param file The file to load
* @returns The actor loaded with animation data
*/
Actor* LoadActor(char *file);

/**
* @brief Free an individual actor
* @param actor The actor to free
*/
void FreeActor(Actor *actor);

/**
* @brief Retrieve the average width and height
* of the sprites across all animations
* @param actor The actor whose attributes we get
*/
Vector2D GetAverageActorDimensions(Actor *actor);

/**
* @brief Pars animation data from file
* @param filename File to extract data from
* @returns A list of animations
*/
Animation* ParseAnimation(char* filename);
#endif