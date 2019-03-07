#ifndef __WORLDS__
#define __WORLDS__

#include "physicsbodies.h"
#include "tilemap.h"

typedef struct World_S
{
	Uint8 _inUse;

	//cpSpace *levelSpace;

	//BodyInfo *levelBody;

	Uint32 pixelWidth, pixelHeight;

	TileMap	*world;
}World;

/**
* @brief Intialize the world manager
* @param max The maximum number of worlds
*/
void WorldManagerInit(Uint32 max);

/**
* @brief Close the world manager system
*/
void WorldManagerClose();

/**
* @brief Initialize a world given
* @param oFilename The config file to build the overworld from
* @param sFilename The name of the file to build side views from
* @returns A pointer to a new world
*/
World* WorldInit(char *oFilename, char *sFilename);

/**
* @brief Get a new world struct
* @returns A pointer to an unused world
*/
World* NewWorld();

/**
* @brief Delete a single world
* world The world to delete
*/
void DeleteWorld(World *world);

/**
* @brief Get the dimensions of the world
* @returns the world's width and height in a vector 2d
*/
Vector2D GetWorldDimensions(World *world);
#endif