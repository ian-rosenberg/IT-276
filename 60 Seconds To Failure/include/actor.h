#ifndef __ACTOR__
#define __ACTOR__

#include <SDL.h>

#include "animations.h"

typedef struct{
	Uint8			_inUse;

	TextLine		name;

	Sprite			*currentSprite;

	Vector4D		color;

	Animation		*animations;

	Animation		*currentAnimation;

	Uint32			numAnimations;

	State			animState;
}Actor;


/**
* @brief Intialize the actor list on a per actor basis
* @param max The max number of actors to be simultaneously loaded
*/
void ActorManagerInit(Uint32 max);

/**
* @brief Get a new actor list
* @returns an unused/unreferenced actor list
*/
Actor* NewActor(Uint32 numAnim);

/**
* @brief Delete an actor from memory
* @param actor The actor to delete
*/
void DeleteActor(Actor *actor);

/**
* @brief Get an animatiob by its filename
* @param filename The file to check against
* @returns The actor in question
*/
Actor* GetActorByName(char* name);

/**
* @brief Load an actor from file
* @param actor The actor to load into
* @param file The file to load
* @returns The actor loaded with animation data
*/
Actor* LoadActor(char *file);

/**
* @brief Set a specific actor given a name within context
* @param actor The actor to modify
* @param name The animation's name to use
*/
void SetActorAnimation(Actor *actor, char* name);

/**
* @brief Based on the actor's current action set the next frame and return type
* @param actor The actor to process
*/
void ActorNextFrame(Actor *actor);

/**
* @brief Free an individual actor
* @param actor The actor to free
*/
void FreeActor(Actor* actor);

/**
* @brief Delete all actors
*/
void ClearAllActors(void);

/**
* @brief Close the actor manager
*/
void ActorManagerClose(void);

/**
* @brief Retrieve the average width and height
* of the sprites across all animations
* @param actor The actor whose attributes we get
*/
Vector2D GetAverageActorDimensions(Actor *actor);
#endif