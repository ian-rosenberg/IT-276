#ifndef __BUILDING_ENTITY__
#define __BUILDING_ENTITY__

#include "entity.h"
#include "tilemap.h"

typedef enum
{
	SHELTER,
	BOSS_LAIR
}BuildingType;

typedef struct BuildingEntity_S
{
	int			_refCount;
	
	Entity		*self;

	TileMap		*owner;

	Sprite		*sprite;

	BuildingType type;
}BuildingEntity;

/**
* @brief Intialize the manager for points of interest/buildings
*/
void BuildingManagerInit(Uint32 max);

/**
* @brief Close the building manager and delete all buildings
*/
void BuildingManagerClose();

/**
* @brief Delete a building
* @param theBuilding To delete
*/
void BuildingDelete(BuildingEntity *theBuilding);

/**
* @brief Create a new building ent and return it from the manager
* @returns An empty building entity ptr
*/
BuildingEntity* NewBuildingEntity();
#endif