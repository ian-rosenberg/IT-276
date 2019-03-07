#include "worlds.h"
#include "simple_logger.h"

typedef struct LevelManager_S
{
	World *worldList;

	Uint32 maxWorlds;
}WorldManager;

//local global
static WorldManager worldManager = { 0 };

void WorldManagerInit(Uint32 max)
{
	if (!max)
	{
		slog("Cannot allocate 0 overworlds");

		return;
	}

	worldManager.worldList = (World*)malloc(sizeof(World)* max);
	
	memset(worldManager.worldList, 0, sizeof(World) * max);

	
	if (!worldManager.worldList)
	{
		slog("failed to allocate overworlds in world manager");

		WorldManagerClose();

		return;
	}

	worldManager.maxWorlds = max;

	atexit(WorldManagerClose);

	slog("WorldManager system initialized");
}

World* WorldInit(char *oFilename, char *sFilename)
{
	World* world = NULL;

	world = NewWorld();
	
	world->world = LoadOverworldTileMapFromFile(oFilename);
	world->pixelWidth = world->world->numColumns * world->world->cellWidth;
	world->pixelHeight = world->world->numRows * world->world->cellHeight;

	return world;
}

World* NewWorld()
{
	int i;

	for (i = 0; i < worldManager.maxWorlds; i++)
	{
		if ((worldManager.worldList[i]._inUse == 0) && (worldManager.worldList[i].world == NULL))
		{
			return &worldManager.worldList[i];
		}
	}

	slog("Out of worlds to return");

	return NULL;
}

void WorldManagerClose()
{
	memset(worldManager.worldList, 0, sizeof(World) *worldManager.maxWorlds );
}

Vector2D GetWorldDimensions(World *world)
{
	Vector2D dim = { 0 };

	dim.x = world->pixelWidth;
	dim.y = world->pixelHeight;

	return dim;
}