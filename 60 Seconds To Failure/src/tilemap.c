#include <stdio.h>
#include <stdlib.h>
#include "tilemap.h"
#include "graphics.h"
#include "simple_logger.h"
#include "sprites.h"
#include "camera.h"
#include "entity.h"

#define TILE_TYPES 3

#define NOTHING {0,0,0}

#define GROUND_CENTER_1 8
#define GROUND_CENTER_2 9
#define INSIDE_CENTER_WALKABLE 18

char* overworldTileNames[TILE_TYPES] = { "GROUND", "INSIDE", "BUILDING" };

typedef enum{
	GROUND = 0,
	INSIDE = 1,
	BUILDING = 2
}OverworldTileNames;

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

	atexit(TileMapManagerClose);
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
					tilemapManager.tilemaps[i].map[x][y].offsetInColumn = -1;
					tilemapManager.tilemaps[i].map[x][y].sprite = NULL;
					tilemapManager.tilemaps[i].map[x][y].tileGroup = -1;
					tilemapManager.tilemaps[i].map[x][y].tileType = -1;
					tilemapManager.tilemaps[i].map[x][y]._refCount = 1;
					tilemapManager.tilemaps[i].map[x][y].active = 0;
				}
			}

			tilemapManager.tilemaps[i].numColumns = width;
			tilemapManager.tilemaps[i].numRows = height;
			tilemapManager.tilemaps[i].mapName = NULL;
			tilemapManager.tilemaps[i].renderTarget = NULL;		
			tilemapManager.tilemaps[i].numEnts = 0;
			tilemapManager.tilemaps[i].ents = NULL;

			return &tilemapManager.tilemaps[i];
		}
	}
	
	return NULL;	
}

TileMap* LoadTileMapFromFile(char* filename)
{
	FILE * file;
	char buf[1024];
	char colorMap[1024];
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

			map->renderTarget = CreateBlankTexture(map->cellWidth * map->numColumns,
				map->cellHeight * map->numRows,
				map->mapSpriteSheet->pixelFmt);

			map->mapSpace = gf2d_space_new_full(
				1,
				gf2d_rect(0, 0, map->cellWidth * map->numColumns, map->cellHeight * map->numRows),
				1,
				vector2d(0, 0),
				0.1,
				0.001);

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
			if (strncmp(buf, overworldTileNames[i], strlen(overworldTileNames[i])) != 0)
			{
				continue;
			}
			if (j >= map->numCells)
			{
				flag = true;
				break;
			}

			fscanf(file, "%d,%d,%d",
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
							&& (colors + (y*map->numColumns) + x)->b == 0
							&& map->map[x][y].filled != 1)
						{
							map->map[x][y].active = false;

							map->map[x][y].filled = 1;

							map->currentTileFilled.x++;
							map->currentTileFilled.y++;

							continue;
						}

						if (colors[(y*map->numColumns) + x].r == tileTypes[ite].mappingColor.r
							&& colors[(y*map->numColumns) + x].g == tileTypes[ite].mappingColor.g
							&& colors[(y*map->numColumns) + x].b == tileTypes[ite].mappingColor.b
							&& map->map[x][y].filled != 1)
						{
							map->map[x][y] = *NewTile(ite, map->mapSpriteSheet);
							map->map[x][y].active = true;

							if (map->map[x][y].offsetInColumn != GROUND_CENTER_1 && map->map[x][y].offsetInColumn != GROUND_CENTER_2 && map->map[x][y].offsetInColumn != INSIDE_CENTER_WALKABLE && map->map[x][y].filled != 1)
							{
								gf2d_space_add_static_shape(map->mapSpace, gf2d_shape_rect((x * map->cellWidth) - (map->cellWidth / 2), (y * map->cellHeight) - (map->cellHeight / 2), map->cellWidth, map->cellHeight));
								
								map->currentTileFilled.x++;
								map->currentTileFilled.y++;

								map->map[x][y].filled = 1;

								continue;
							}//gf2d_shape_rect((x * map->cellWidth) - (map->cellWidth / 2), (y * map->cellHeight) - (map->cellHeight / 2), map->cellWidth, map->cellHeight));
							//gf2d_shape_circle(x * map->cellWidth, y * map->cellHeight, map->cellWidth/2);
						}
					}
				}
			}

			++j;
		}
	}

	map->boundingBox.h = map->numRows * map->cellHeight;
	map->boundingBox.w = map->numColumns * map->cellWidth;
	map->boundingBox.x = 0;
	map->boundingBox.y = 0;

	map->srcRect.x = 0;
	map->srcRect.y = 0;
	map->srcRect.w = GetRenderDimensions().x;
	map->srcRect.h = GetRenderDimensions().y;

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

	for (i = 0; i < tileManager.maxTiles; ++i)
	{
		if (tileManager.tiles[i]._refCount == 0 && &tileManager.tiles[i] == NULL)
		{
			tileManager.tiles[i].sprite = sprite;
			tileManager.tiles[i].offsetInColumn = offsetInColumn;
			tileManager.tiles[i].tileGroup = offsetInColumn;
			tileManager.tiles[i]._refCount = 1;
			tileManager.tiles[i].filled = -1;

			return &tileManager.tiles[i];
		}
	}

	for (i = 0; i < tileManager.maxTiles; ++i)
	{
		if (tileManager.tiles[i]._refCount == 0 || !tileManager.tiles[i].active)
		{
			memset(&tileManager.tiles[i], 0, sizeof(Tile));

			tileManager.tiles[i].sprite = sprite;
			tileManager.tiles[i].offsetInColumn = offsetInColumn;
			tileManager.tiles[i].tileGroup = offsetInColumn;
			tileManager.tiles[i]._refCount = 1;
			tileManager.tiles[i].filled = -1;


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
void TileMapDelete(TileMap *map)
{
	int i;

	for (i = 0; i < map->numEnts; ++i)
	{
		gf2d_space_remove_body(map->mapSpace, &map->ents[i].body);
	}

	gf2d_space_free(map->mapSpace);

	memset(map, 0, sizeof(TileMap));
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

	SDL_SetRenderTarget(GetRenderer(), map->renderTarget);

	for (y = 0; y < map->numRows; ++y)
	{
		for (x = 0; x < map->numColumns; ++x)
		{			
			if (!map->map[x][y].active)
			{
				DrawSprite(map->emptyTile,
					vector2d(x * map->cellWidth, y * map->cellHeight),
					&scale,
					&scaleCenter,
					&rot,
					&flip,
					NULL,
					0,
					0,
					map->cellWidth,
					map->cellHeight);
					
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

			SpriteDelete(&map->map[x][y].sprite);
		}
	}



	SDL_SetRenderTarget(GetRenderer(), NULL);
}


void DrawMap(TileMap *map)
{
	Vector2D camPos = GetCameraPosition();
	Vector2D resultPos = { 0 };
	Vector2D one = { 1, 1 };
	Sprite s = { 0 };
	vector2d_sub(resultPos, map->position, camPos);
	
	s.texture = map->renderTarget;

	DrawSprite(&s, resultPos, &one, NULL, NULL, NULL, NULL, 0, 0, map->boundingBox.w, map->boundingBox.h);
}

Bool AddEntityToTileMap(Entity *ent, TileMap *map)
{
	if (!ent)
	{
		return false;
	}

	if (ent->_inUse != 1)
	{
		return false;
	}

	if (!map->mapSpace)
	{
		slog("no space to add ent to");

		return false;
	}

	if (map->ents == NULL)
	{
		map->ents = malloc(sizeof(Entity));
		map->ents = ent;
	}
	else
	{
		map->ents = realloc(map->ents, sizeof(map->numEnts + 1));
		map->ents[map->numEnts] = *ent;//questionable
	}

	if (!&ent->body.touch)
	{
		ent->body.touch = body_body_touch;
	}

	gf2d_space_add_body(map->mapSpace, &ent->body);

	if (gf2d_line_cmp(ent->actor->name, "Player"))
	{
		SetCurrentMapActive(map);
	}

	map->numEnts++;

	return true;
}

Vector2D GetCurrentTileMapDimensions(TileMap *map)
{
	Vector2D dim = { map->boundingBox.w, map->boundingBox.h };

	return dim;
}


void MapRemoveEntity(TileMap *map, Body *body)
{
	if (!body)return;
	if (!map->mapSpace)
	{//nothing to do
		return;
	}
	gf2d_space_remove_body(map->mapSpace, body);

	map->numEnts--;
}

void MapUpdate(TileMap *map)
{
	gf2d_entity_pre_sync_all();
	gf2d_space_update(map->mapSpace);
	gf2d_entity_post_sync_all();
}

TileMap* GetCurrentMap()
{
	int i;

	for (i = 0; i < tilemapManager.maxMaps; ++i)
	{
		if (tilemapManager.tilemaps[i].active == 1)
		{
			return &tilemapManager.tilemaps[i];
		}
	}
}

void SetCurrentMapActive(TileMap* map)
{
	int i;

	for (i = 0; i < tilemapManager.maxMaps; ++i)
	{
		if (map == &tilemapManager.tilemaps[i])
		{
			tilemapManager.tilemaps[i].active = 1;
		}
		else
		{
			tilemapManager.tilemaps[i].active = 0;
		}
	}
}