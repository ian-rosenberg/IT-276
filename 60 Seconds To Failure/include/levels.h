#ifndef __LEVELS__
#define __LEVELS__

#include "physicsbodies.h"
#include "tilemap.h"

typedef struct Level_S
{
	Uint8 _inUse;

	//cpSpace *levelSpace;

	BodyInfo *levelBody;

	TileMap	*map;

}Level;



#endif