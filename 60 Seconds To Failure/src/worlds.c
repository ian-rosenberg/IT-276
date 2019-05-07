#include "worlds.h"
#include "player.h"
#include "simple_logger.h"

static World gameWorld = { 0 };

void WorldInit(char *oFilename, char *sFilename)
{
	//gameWorld.overworld = LoadTileMapFromFile(oFilename, false);

	gameWorld.sideView = LoadTileMapFromFile(sFilename, true);

	//gameWorld.overworldPixelWidth = gameWorld.overworld->numColumns * gameWorld.overworld->cellWidth;
	//gameWorld.overworldPixelHeight = gameWorld.overworld->numRows * gameWorld.overworld->cellHeight;

	gameWorld.sidePixelWidth = gameWorld.sideView->numColumns * gameWorld.sideView->cellWidth;
	gameWorld.sidePixelHeight = gameWorld.sideView->numRows * gameWorld.sideView->cellHeight;
}

Vector2D GetWorldDimensions(World *world, Bool gravity)
{
	Vector2D dim = { 0 };
	if (!gravity)
	{
		dim.x = world->overworldPixelWidth;
		dim.y = world->overworldPixelHeight;
	}
	else
	{
		dim.x = world->sidePixelWidth;
		dim.y = world->sidePixelHeight;
	}

	return dim;
}

void CleanUpWorld(World *world)
{
	TileMapDelete(world->overworld);

	TileMapDelete(world->sideView);

	memset(world, 0, sizeof(World));
	
	free(world);
}

TileMap* GetCurrentMap()
{
	int i;
	Entity *playerEnt = GetPlayerEntity();
	
	/*for (i = 0; i < gameWorld.overworld->numEnts; ++i)
	{
		if (&gameWorld.overworld->ents[i] == playerEnt)
		{
			return gameWorld.overworld;
		}
	}*/

	for (i = 0; i < gameWorld.sideView->numEnts; ++i)
	{
		if (gameWorld.sideView->ents[i].id == playerEnt->id)
		{
			return gameWorld.sideView;
		}
	}

	return NULL;
}