#include <stdio.h>
#include <stdlib.h>
#include "tilemap.h"
#include "graphics.h"
#include "simple_logger.h"
#include "sprites.h"
#include "camera.h"

#define TILE_TYPES 3

#define NOTHING {0,0,0}

char* tileNames[TILE_TYPES] = { "GROUND", "INSIDE", "BUILDING" };

typedef enum{
	GROUND = 0,
	INSIDE = 1,
	BUILDING = 2
}TileNames;

typedef struct{
	TileMap		*tilemaps;

	Uint32		maxMaps;
}TileMapManager;

typedef struct
{
	Tile *tiles;

	Uint32 maxTiles;
}TileManager;

//local global
static TileMapManager tilemapManager = { 0 };
static TileManager tileManager = { 0 };


void TileMapInit(Uint32 max)
{
	if (!max)
	{
		slog("Cannot initialize tilemap manager with 0 tile types");
		return;
	}

	tilemapManager.tilemaps = (TileMap*)malloc(sizeof(TileMap)* max);
	memset(tilemapManager.tilemaps, 0, sizeof(TileMap)* max);

	tilemapManager.maxMaps = max;
}

void TileInit(Uint32 max)
{
	if (!max)
	{
		slog("Cannot initialize tilemap manager with 0 tile types");
		return;
	}

	tileManager.tiles = (Tile*)malloc(sizeof(Tile)* max);
	memset(tileManager.tiles, 0, sizeof(Tile)* max);

	tileManager.maxTiles = max;
}

TileMap* NewTileMap(Uint32 width, Uint32 height)
{
	int i, j, x, y;

	if (!tilemapManager.maxMaps)
	{
		return NULL;
	}

	if (!width || !height)
	{
		slog("cannot create a tilemap with 0 height or width");

		return NULL;
	}

	for (i = 0; i < tilemapManager.maxMaps; ++i)
	{
		if (tilemapManager.tilemaps[i]._inUse != 1)
		{
			tilemapManager.tilemaps[i]._inUse = 1;
			tilemapManager.tilemaps[i].currentTileFilled = vector2d(-1, -1);
			tilemapManager.tilemaps[i].map = malloc(sizeof(Tile*)* height);

			for (j = 0; j < height; ++j)
			{
				tilemapManager.tilemaps[i].map[j] = malloc(width * sizeof (Tile));
			}


			for (y = 0; y < height; ++y)
			{
				for (x = 0; x < width; ++x)

				{
					tilemapManager.tilemaps[i].map[x][y].color = (SDL_Color){ 0, 0, 0, 0 };
					tilemapManager.tilemaps[i].map[x][y].mappingColor = (SDL_Color){ 0, 0, 0, 0 };
					tilemapManager.tilemaps[i].map[x][y].offsetInColumn = 0;
					tilemapManager.tilemaps[i].map[x][y].sprite = NULL;
					tilemapManager.tilemaps[i].map[x][y].tileGroup = -1;
					tilemapManager.tilemaps[i].map[x][y].tileType = -1;
					tilemapManager.tilemaps[i].map[x][y]._refCount = 1;
				}
			}

			tilemapManager.tilemaps[i].numColumns = width;
			tilemapManager.tilemaps[i].numRows = height;
			tilemapManager.tilemaps[i].mapName = NULL;
			tilemapManager.tilemaps[i].self = NewEntity(NewActorByName("Overworld"));
			tilemapManager.tilemaps[i].self->renderTarget = NULL;
			

			tilemapManager.tilemaps[i].self->Think = NULL;
			tilemapManager.tilemaps[i].self->Update = NULL;
			tilemapManager.tilemaps[i].self->Draw = DrawMap;
			tilemapManager.tilemaps[i].self->Free = NULL;

			return &tilemapManager.tilemaps[i];
		}
	}

	atexit(TileMapManagerClose);
	
	return NULL;	
}

TileMap* LoadTileMapFromFile(char* filename)
{
	FILE * file;
	char buf[512];
	char colorMap[512];
	TileMap *map = NULL;
	Uint32 x, y, ite, col, row;
	Bool flag = false;
	int i, j;
	SDL_Color *colors;
	Tile* tileTypes = NULL;
	

	file = fopen(filename, "r");

	if (!file)
	{
		return NULL;
	}

	//while (!flag)
	//{
		while (fscanf(file, "%s", buf) != EOF)
		{
			if (strcmp(buf, "width:") == 0)
			{
				fscanf(file, "%i", &col);
				continue;
			}
			if (strcmp(buf, "height:") == 0)
			{
				fscanf(file, "%i", &row);

				map = NewTileMap(col, row);
				continue;
			}
			if (strcmp(buf, "numImages:") == 0)
			{
				fscanf(file, "%i", &map->numCells);

				tileTypes = (Tile*)malloc(sizeof(Tile)* map->numCells);
				continue;
			}
			if (strcmp(buf, "cellWidth:") == 0)
			{
				fscanf(file, "%i", &map->cellWidth);
				continue;
			}
			if (strcmp(buf, "cellHeight:") == 0)
			{
				fscanf(file, "%i", &map->cellHeight);
				continue;
			}
			if (strcmp(buf, "groundTiles:") == 0)
			{
				fscanf(file, "%64s", &map->mapName);

				map->mapSpriteSheet = LoadImageToTexture(&map->mapName, GetRenderer());

				map->self->renderTarget = CreateBlankTexture(map->cellWidth * map->numColumns,
					map->cellHeight * map->numRows,
					map->mapSpriteSheet->pixelFmt);

				continue;
			}
			if (strcmp(buf, "shelter:") == 0)
			{
				//fscanf(file, "%64s", &map->shelterName);

				//map->shelter = LoadImageToTexture(&map->shelterName, GetRenderer());
				continue;
			}
			if (strcmp(buf, "boss:") == 0)
			{
				//fscanf(file, "%64s", &map->bossAreaName);

				//map->bossArea = LoadImageToTexture(&map->bossAreaName, GetRenderer());
				continue;
			}
			if (strcmp(buf, "tilemap:") == 0)
			{
				fscanf(file, "%64s", &colorMap);
				colors = GetPixelDataFromFile(&colorMap);

				//flag = true;
				break;
			}

			fgets(buf, sizeof(buf), file);
		}

		//rewind(file);
		//memset(buf, 0, sizeof(buf));
	//}

	j = 0;

	while (!flag)
	{
		if (fscanf(file, "%s", buf) == EOF)
		{
			rewind(file);
		}

		//3 tile types
		for (i = 0; i < TILE_TYPES; ++i)
		{
			if (strncmp(buf, tileNames[i], strlen(tileNames[i])) != 0)
			{
				continue;
			}
			if (j >= map->numCells)
			{
				flag = true;
				break;
			}

			fscanf(file, "%i,%i,%i",
				&tileTypes[j].mappingColor.r,
				&tileTypes[j].mappingColor.g,
				&tileTypes[j].mappingColor.b);

			for (y = 0; y < map->numRows; ++y)
			{
				for (x = 0; x < map->numColumns; ++x)
				{
					for (ite = 0; ite < map->numCells; ++ite)
					{
						if ((colors + (y*map->numColumns) + x)->r == 0
							&& (colors + (y*map->numColumns) + x)->g == 0
							&& (colors + (y*map->numColumns) + x)->b == 0)
						{
							map->map[x][y].active = false;

							continue;
						}
						else if (colors[(y*map->numColumns) + x].r == tileTypes[ite + 1].mappingColor.r
							&& colors[(y*map->numColumns) + x].g == tileTypes[ite + 1].mappingColor.g
							&& colors[(y*map->numColumns) + x].b == tileTypes[ite + 1].mappingColor.b)
						{
							map->map[x][y] = *NewTile(ite + 1, map->mapSpriteSheet);
							map->map[x][y].active = true;

							break;
						}
					}

					map->currentTileFilled.x++;
					map->currentTileFilled.y++;
				}
			}

			++j;
		}
	}

	map->self->boundingBox.h = map->numRows * map->cellHeight;
	map->self->boundingBox.w = map->numColumns * map->cellWidth;
	map->self->boundingBox.x = 0;
	map->self->boundingBox.y = 0;

	map->self->srcRect.x = 0;
	map->self->srcRect.y = 0;
	map->self->srcRect.w = GetRenderDimensions().x;
	map->self->srcRect.h = GetRenderDimensions().y;

	RenderMapToTexture(map);

	fclose(file);

	return map;
}

Tile* NewTile(Uint32 offsetInColumn, Sprite *sprite)
{
	int i;
	
	if (!tileManager.maxTiles)
	{
		return NULL;
	}

	//look for unused address
	for (i = 0; i < tileManager.maxTiles; ++i)
	{
		if (tileManager.tiles[i]._refCount == 0 && &tileManager.tiles[i] == NULL)
		{
			tileManager.tiles[i].sprite = sprite;
			tileManager.tiles[i].offsetInColumn = offsetInColumn;
			tileManager.tiles[i]._refCount = 1;

			tileManager.tiles[i].active = true;

			return &tileManager.tiles[i];
		}
	}

	//look for an unreferenced sprite
	for (i = 0; i < tileManager.maxTiles; ++i)
	{
		if (tileManager.tiles[i]._refCount == 0 || !tileManager.tiles[i].active)
		{
			memset(&tileManager.tiles[i], 0, sizeof(Tile));

			tileManager.tiles[i].active = true;

			tileManager.tiles[i].sprite = sprite;
			tileManager.tiles[i].offsetInColumn = offsetInColumn;
			tileManager.tiles[i]._refCount = 1;
			return &tileManager.tiles[i];
		}
	}

	slog("Out of tile address space");

	return NULL;
}

void TileMapManagerClose()
{
	int i;

	for (i = 0; i < tilemapManager.maxMaps; ++i)
	{
		TileMapDelete(&tilemapManager.tilemaps[i]);
	}
}

void TileDelete(Tile* tile)
{
	if (!tile)
	{
		slog("cannot delete a null tile");
	}

	if (!tile->sprite)
	{
		slog("no tile sprite to free");
	}

	tile->_refCount--;

	SpriteDelete(tile->sprite);

	free(tile);
}

void TileMapDelete(TileMap *map)
{
	int x, y;

	for (y = 0; y < map->numRows; ++y)
	{
		for (x = 0; x < map->numColumns; +x)
		{
			if (&map->map[y][x] != NULL)
			{
				TileDelete(&map->map[x][y]);
			}
		}
	}
}

void RenderMapToTexture(TileMap *map)
{
	Vector2D scale;
	Vector2D scaleCenter;
	Vector2D flip;
	Vector3D rot = { 0, 0, 0 };
	int x, y;
	
	scale = vector2d(1, 1);
	scaleCenter =  vector2d(map->cellWidth / 2, map->cellHeight / 2);
	flip = vector2d(0, 0);

	SDL_SetRenderTarget(GetRenderer(), map->self->renderTarget);

	for (y = 0; y < map->numRows; ++y)
	{
		for (x = 0; x < map->numColumns; ++x)
		{			
			if (!map->map[x][y].active)
			{
				continue;
			}

			DrawSprite(map->mapSpriteSheet,
				vector2d(x * map->cellWidth, y * map->cellHeight ),
				&scale,
				&scaleCenter,
				&rot,
				&flip,
				NULL,
				map->map[x][y].offsetInColumn,
				0,
				map->cellWidth,
				map->cellHeight);
		}
	}



	SDL_SetRenderTarget(GetRenderer(), NULL);
}


void DrawMap(Entity *mapEntity)
{
	Vector2D camPos = GetCameraPosition();
	Vector2D resultPos = { 0 };
	Vector2D one = { 1, 1 };
	Sprite s = { 0 };
	vector2d_sub(resultPos, mapEntity->position, camPos);
	
	s.texture = mapEntity->renderTarget;

	DrawSprite(&s, resultPos, &one, NULL, NULL, NULL, NULL, 0, 0, mapEntity->boundingBox.w, mapEntity->boundingBox.h);


}

Bool AddEntityToTileMap(Entity *ent, TileMap* map)
{

}

void TileInitEntities(Uint32 max, TileMap* map)
{
	if (!max)
	{
		slog("Cannot init 0 tilemap  entities");
	}

	map->entityList = (Entity*)malloc(sizeof(Entity) * max);
	memset(map->entityList, 0, sizeof(Entity) * max);
}

Vector2D GetCurrentTileMapDimensions(Entity *self)
{
	Vector2D dim = { self->boundingBox.w, self->boundingBox.h };

	return dim;
}