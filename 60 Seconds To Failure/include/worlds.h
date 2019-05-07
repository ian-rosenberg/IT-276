#ifndef __WORLDS__
#define __WORLDS__

#include "tilemap.h"

typedef struct World_S
{
	Uint32 overworldPixelWidth, overworldPixelHeight, 
			sidePixelWidth, sidePixelHeight,
			numSideViews;

	TileMap	*overworld;
	TileMap *sideView;
}World;

/**
* @brief Initialize the gameworld given
* @param oFilename The config file to build the overworld from
* @param sFilename The name of the file to build side views from
*/
void WorldInit(char *oFilename, char *sFilename);

/**
* @brief Get the dimensions of the world
* @gravity Determines if we need sideview or overworld vals
* @returns the world's width and height in a vector 2d
*/
Vector2D GetWorldDimensions(World *world, Bool gravity);

/**
* @brief Clean up the game world
* @param world The world whose tilemaps to free
*/
void CleanUpWorld(World *world);

/**
* @brief Return the current tilemap that's active
*/
TileMap* GetCurrentMap();
#endif