#ifndef __GAMEWORLD__
#define __GAMEWORLD__

#include "tilemap.h"
#include "entity.h"

typedef struct GameWorld_S
{	
	TileMap *overworld;
	
	TileMap *shelter;
	
	TileMap *bossMap;
	
	TileMap *encounterAreas;


}GameWorld;


void InitGameWorld();

#endif