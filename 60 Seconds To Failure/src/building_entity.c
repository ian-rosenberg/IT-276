#include "building_entity.h"
#include "simple_logger.h"

typedef struct
{
	BuildingEntity *buildings;

	Uint32 numBuildings;
	Uint32 maxBuildings;
}BuildingManager;

//local global
static BuildingManager buildingManager = { 0 };

void BuildingManagerInit(Uint32 max)
{
	if (!max)
	{
		slog("Cannot allocate 0 buildings");

		return;
	}

	buildingManager.buildings = malloc(sizeof(Building) * max);
	memset(buildingManager.buildings, 0, sizeof(Building) * max);

	buildingManager.maxBuildings = max;
	buildingManager.numBuildings = 0;

	atexit(BuildingManagerClose);

	slog("Building Manager initialized");
}

void BuildingManagerClose()
{
	int i;

	for (i = 0; i < buildingManager.maxBuildings; ++i)
	{
		BuildingDelete(&buildingManager.buildings[i]);
	}

	slog("Building manager closed");
}

void BuildingDelete(BuildingEntity *theBuilding)
{
	if (!theBuilding)
	{
		slog("no building to delete");
		return;
	}

	SpriteDelete(theBuilding->sprite);

	EntityFree(theBuilding->self);
}

BuildingEntity* NewBuildingEntity()
{
	int i;

	if (!buildingManager.maxBuildings)
	{
		return NULL;
	}

	for (i = 0; i < buildingManager.maxBuildings; ++i)
	{
		if (buildingManager.buildings[i]._refCount == 0 && &buildingManager.buildings[i] == NULL)
		{
			buildingManager.buildings[i].sprite = NULL;
			buildingManager.buildings[i].owner = NULL;
			buildingManager.buildings[i].self = NULL;
			buildingManager.buildings[i]._refCount = 1;

			return &buildingManager.buildings[i];
		}
	}

	slog("Out of building address space");

	return NULL;
}