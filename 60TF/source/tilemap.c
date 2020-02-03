#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tilemap.h"
#include "graphics.h"
#include "simple_logger.h"
#include "sprites.h"
#include "entity.h"

#define TILE_TYPES 3

#define NOTHING {0,0,0}

#define GROUND_CENTER_1 8
#define GROUND_CENTER_2 9
#define INSIDE_CENTER_WALKABLE 18

#define WALKABLE_BLOCK1 17
#define WALKABLE_BLOCK2 18
#define WALKABLE_BLOCK3 19

char* overworldTileNames[TILE_TYPES] = { "GROUND", "INSIDE", "BUILDING" };

typedef enum
{
	GROUND = 0,
	INSIDE = 1,
	STRUCTURE = 2
}OverworldTileNames;

typedef struct {
	TileMap		*tilemaps;

	Uint32		maxMaps;
}TileMapManager;

//local global
static TileMapManager tilemapManager = { 0 };

void TileMapManagerInit(Uint32 max)
{
	int i;

	if (!max)
	{
		slog("Cannot init a tilemap manager with 0 tilemaps");
	}

	tilemapManager.maxMaps = max;
	tilemapManager.tilemaps = malloc(sizeof(TileMap) * max);
	memset(tilemapManager.tilemaps, 0, sizeof(TileMap) * max);

	for (i = 0; i < tilemapManager.maxMaps; i++)
	{
		tilemapManager.tilemaps[i].active = 0;
		tilemapManager.tilemaps[i]._inUse = 0;
		tilemapManager.tilemaps[i].shelterPositions = NULL;
		tilemapManager.tilemaps[i].map = NULL;
		tilemapManager.tilemaps[i].mapName = NULL;
		tilemapManager.tilemaps[i].emptyTileFileName = NULL;
		tilemapManager.tilemaps[i].emptyTile = NULL;
		tilemapManager.tilemaps[i].ents = NULL;
		tilemapManager.tilemaps[i].backgroundImage = NULL;
		tilemapManager.tilemaps[i].backgroundMusic = NULL;
		tilemapManager.tilemaps[i].renderTarget = NULL;

	}
}

TileMap* NewTileMap(Uint32 width, Uint32 height)
{
	int i, x, y;
	TileMap *map = NULL;

	if (!width || !height)
	{
		slog("cannot create a tilemap with 0 height or width");

		return NULL;
	}

	for (i = 0; i < tilemapManager.maxMaps; ++i)
	{
		if (!tilemapManager.tilemaps[i]._inUse && !tilemapManager.tilemaps[i].active)
		{
			map = &tilemapManager.tilemaps[i];

			break;
		}
	}

	if (!map)
	{
		slog("Out of tilemap addresses");

		return NULL;
	}

	map->_inUse = 1;
	map->currentTileFilled = vector2d(-1, -1);
	map->numColumns = width;
	map->numBlockingTiles = 0;
	map->numRows = height;
	map->mapName = malloc(sizeof(char)* GF2DWORDLEN);

	map->map = malloc(sizeof(Tile*) * height);
	memset(map->map, 0, sizeof(Tile*) * height);

	for (i = 0; i < height; ++i)
	{
		map->map[i] = malloc(width * sizeof(Tile));
		memset(map->map[i], 0, width * sizeof(Tile));
	}

	for (y = 0; y < height; ++y)
	{
		for (x = 0; x < width; ++x)
		{
			map->map[y][x].offsetInColumn = -1;
			map->map[y][x].sprite = NULL;
			map->map[y][x].tileGroup = -1;
			map->map[y][x].tileType = -1;
			map->map[y][x]._refCount = 1;
			map->map[y][x].active = 0;
			map->map[y][x].walkable = 0;
			map->map[y][x].boundingBox = gf2d_rect(0, 0, 0, 0);
		}
	}

	map->renderTarget = NULL;
	map->numEnts = 0;
	map->ents = gf2d_list_new();

	map->active = 1;

	return map;
}

TileMap* LoadTileMapFromFile(char* filename, Bool gravity)
{
	FILE * file;
	TileMap *map = NULL;
	char buf[1024];
	char temp[1024];
	Uint32 x, y, ite;
	int i, j;
	TileMapData *data = NULL;
	
	data = malloc(sizeof(TileMapData));
	memset(data, 0, sizeof(TileMapData));

	data->flag = false;

	map = ParseMapFile(filename, data);

	data->colors = GetPixelDataFromFile(data->colorMap);

	data->tileTypes = (Tile*)malloc(sizeof(Tile)* map->numCells);
	memset(data->tileTypes, 0, sizeof(Tile) * map->numCells);

	map->emptyTile = LoadImageToTexture(map->emptyTileFileName, GetRenderer());

	map->mapSpriteSheet = LoadImageToTexture(map->mapName, GetRenderer());

	map->renderTarget = CreateBlankTexture(map->cellWidth * map->numColumns,
		map->cellHeight * map->numRows,
		map->mapSpriteSheet->pixelFmt);

	data->flag = false;

	ParseTileColors(data, map->numCells);

	data->flag = false;

	FillMapTiles(data, map, gravity);

	map->boundingBox.h = map->numRows * map->cellHeight;
	map->boundingBox.w = map->numColumns * map->cellWidth;
	map->boundingBox.x = 0;
	map->boundingBox.y = 0;

	map->srcRect.x = 0;
	map->srcRect.y = 0;
	map->srcRect.w = GetRenderDimensions().x;
	map->srcRect.h = GetRenderDimensions().y;

	RenderMapToTexture(map);

	free(data->tileTypes);
	free(data->colors);
	free(data->colorMap);
	free(data->tileGenInfoName);
	free(data->emptyTileFileName);
	free(data->mapName);
	free(data);

	return map;
}

void TileMapDelete(TileMap *map)
{
	int i, j;

	for (i = 0; i < map->numRows; ++i)
	{
		for (j = 0; j < map->numColumns; ++j)
		{
			SpriteDelete(map->map[i][j].sprite);
		}
		
		free(map->map[i]);
	}
	
	SpriteDelete(map->backgroundImage);
	SpriteDelete(map->renderTarget);
	SpriteDelete(map->mapSpriteSheet);
	SpriteDelete(map->emptyTile);

	for (i = 0; i < map->numEnts; ++i)
	{
		EntityFree(&map->ents[i]);
	}

	free(map->shelterPositions);
	free(map->mapName);
	free(map->emptyTileFileName);
	free(map->backgroundMusic);
	free(map->map);
	free(map);
}

void RenderMapToTexture(TileMap *map)
{
	Vector2D scale;
	Vector2D scaleCenter;
	Vector2D flip;
	Vector3D rot = { 0, 0, 0 };
	Vector4D alpha = { 0,0,0,1 };
	int x, y;
	
	scale = vector2d(1, 1);
	scaleCenter =  vector2d(map->cellWidth / 2, map->cellHeight / 2);
	flip = vector2d(0, 0);

	SDL_SetRenderTarget(GetRenderer(), map->renderTarget);

	for (y = 0; y < map->numRows; ++y)
	{
		for (x = 0; x < map->numColumns; ++x)
		{	
			if (!map->map[y][x].active)
			{
				DrawSprite(map->mapSpriteSheet,
					vector2d(x * map->cellWidth, y * map->cellHeight),
					&scale,
					&scaleCenter,
					&rot,
					&flip,
					&alpha,
					map->map[y][x].offsetInColumn,
					0,
					map->cellWidth,
					map->cellHeight);

				SpriteDelete(&map->map[y][x].sprite);
				
				continue;
			}

			DrawSprite(map->mapSpriteSheet,
				vector2d(x * map->cellWidth, y * map->cellHeight ),
				&scale,
				&scaleCenter,
				&rot,
				&flip,
				NULL,
				map->map[y][x].offsetInColumn,
				0,
				map->cellWidth,
				map->cellHeight);

			SpriteDelete(&map->map[y][x].sprite);
		}
	}



	SDL_SetRenderTarget(GetRenderer(), NULL);
}


void DrawMap(TileMap *map, Vector2D camPos)
{

	Vector2D resultPos = { 0 };
	Vector2D one = { 1, 1 };
	Sprite s = { 0 };
	vector2d_sub(resultPos, map->position, camPos);
	
	s.texture = map->renderTarget;

	slog("Camera coords (%f, %f)", camPos.x, camPos.y);

	DrawSprite(&s, resultPos, &one, NULL, NULL, NULL, NULL, 0, 0, map->boundingBox.w, map->boundingBox.h);

}

TileMap* GetCurrentTileMap()
{
	int i;

	for (i = 0; i < tilemapManager.maxMaps; ++i)
	{
		if (tilemapManager.tilemaps[i].active)
		{
			return &tilemapManager.tilemaps[i];
		}
	}
}

Bool AddEntityToCurrentTileMap(Entity *ent)
{
	TileMap *map = GetCurrentTileMap();
	
	if (!ent)
	{
		return false;
	}

	if (ent->_inUse != 1)
	{
		return false;
	}

	if (map->ents == NULL)
	{
		//map->ents = gf2d_list_new();
	}
	else
	{
		//map->ents = gf2d_list_append(map->ents, ent);
	}

	/*if (ent == GetPlayerEntity())
	{
		SetPlayerGravity();
	}*/

	map->numEnts++;

	return true;
}

Vector2D GetCurrentTileMapDimensions(TileMap *map)
{
	Vector2D dim = { map->boundingBox.w, map->boundingBox.h };

	return dim;
}

void MapRemoveEntity(TileMap *map, Entity *ent)
{
	int i = 0;

	for (; i < map->numEnts; i++)
	{
		if (&map->ents[i] == ent)
		{
			&map->ents[i] == NULL;
		}
	}

	map->numEnts--;
}

void MapUpdate(TileMap *map)
{
	int i, j, k;

	for (i = 0; i < map->numColumns; i++)
	{
		for (j = 0; j < map->numRows; i++)
		{
			if (&map->map[i][j] != NULL)
			{
				if (map->map[i][j].walkable == false)
				{
					for (k = 0; k < map->numEnts; k++)
					{

						if (EntityTileTouch(&map->map[i][j].boundingBox, &map->ents[k]))
						{
							Vector2D negV = map->ents[k].velocity;
							
							vector2d_negate(negV, map->ents[k].velocity);

							vector2d_add(map->ents[k].position, map->ents[k].position, negV);
						}
					}
				}
			}
		}
	}
}


TileMap* ParseMapFile(char* filename, TileMapData *data)
{
	FILE *file;
	char buf[GF2DTEXTLEN];
	TileMap *map = NULL;
	char *mapName = NULL;
	char *colorMap = NULL;
	char *emptyTileFileName = NULL;
	char **filenames = NULL;
	char *fileContents = "";
	size_t fileSize = 0;
	int i = 0, n = 0;
	char c;


	file = fopen(filename, "r");

	if (!file)
	{
		slog("Failed to open file %", filename);

		return NULL;
	}

	fseek(file, 0L, SEEK_END);
	fileSize = ftell(file);
	rewind(file);
	fileContents = malloc(sizeof(char) * fileSize);
	memset(fileContents, 0, sizeof(char) * fileSize);

	c = fgetc(file);

	while (c != EOF)
	{
		fileContents[i++] = c;
		c = fgetc(file);
	}

	fileContents[i] = '\0';

	fclose(file);

	data->flag = false;

	while (sscanf(fileContents, "%s\n%n", buf, &n) == 1)
	{
		if (buf[0] == '~')
		{
			//'~' denotes end of important info: text afterwards is ignored
			break;
		}

		fileContents += n;

		if (strcmp(buf, "width:") == 0)
		{
			sscanf(fileContents, " %s\n%n", buf, &n);

			fileContents += n;

			data->col = atoi(buf);
		}
		if (strcmp(buf, "height:") == 0)
		{
			sscanf(fileContents, " %s\n%n", buf, &n);
			
			fileContents += n;

			data->row = atoi(buf);
			break;
		}
	}

	map = NewTileMap(data->col, data->row);

	while (sscanf(fileContents, " %s\n%n", buf, &n) == 1)
	{
		if (buf[0] == '~')
		{
			//'~' denotes end of important info: text afterwards is ignored
			break;
		}
		fileContents += n;

		if (strcmp(buf, "numImages:") == 0)
		{
			sscanf(fileContents, " %s\n%n", buf, &n);

			fileContents += n; 
			map->numCells = atoi(buf);
			continue;
		}
		if (strcmp(buf, "cellWidth:") == 0)
		{
			sscanf(fileContents, " %s\n%n", buf, &n);

			fileContents += n; 
			map->cellWidth = atoi(buf);
			continue;
		}
		if (strcmp(buf, "cellHeight:") == 0)
		{
			sscanf(fileContents, " %s\n%n", buf, &n);

			fileContents += n; 
			map->cellHeight = atoi(buf);
			continue;
		}
		if (strcmp(buf, "groundTiles:") == 0)
		{
			sscanf(fileContents, " %s\n%n", buf, &n);

			fileContents += n;
			
			data->mapName = malloc(sizeof(char)*n);
			memset(data->mapName, 0, sizeof(char)*n);

			strncpy(data->mapName, buf, n);

			map->mapName = data->mapName;

			continue;
		}
		if (strcmp(buf, "emptyTile:") == 0)
		{
			sscanf(fileContents, " %s\n%n", buf, &n);

			fileContents += n;

			data->emptyTileFileName = malloc(sizeof(char)*n);
			memset(data->emptyTileFileName, 0, sizeof(char)*n);

			strncpy(data->emptyTileFileName, buf, n);

			map->emptyTileFileName = data->emptyTileFileName;

			continue;
		}
		if (strcmp(buf, "tilemap:") == 0)
		{
			sscanf(fileContents, " %s\n%n", buf, &n);

			fileContents += n;

			data->colorMap = malloc(sizeof(char)*n);
			memset(data->colorMap, 0, sizeof(char)*n);

			strncpy(data->colorMap, buf, n);

			continue;
		}
		if (strcmp(buf, "tileData:") == 0)
		{
			sscanf(fileContents, " %s\n%n", buf, &n);

			fileContents += n;

			data->tileGenInfoName = malloc(sizeof(char)*n);
			memset(data->tileGenInfoName, 0, sizeof(char)*n);

			strncpy(data->tileGenInfoName, buf, n);

			continue;

			break;
		}

		memset(buf, 0, GF2DTEXTLEN * sizeof(char));
	}

	return map;
}

void ParseTileColors(TileMapData *data, Uint32 numCells)
{
	FILE * file = NULL;
	int i = 0, j = 0, n = 0;
	char buf[GF2DTEXTLEN];
	char tempBuf[GF2DTEXTLEN];
	char c;
	size_t fileSize = 0;
	char *fileContents = ""; 

	file = fopen(data->tileGenInfoName, "r");

	if (!file)
	{
		slog("Failed to open file %", data->tileGenInfoName);

		return NULL;
	}

	fseek(file, 0L, SEEK_END);
	fileSize = ftell(file);
	rewind(file);
	fileContents = malloc(sizeof(char) * fileSize);
	memset(fileContents, 0, sizeof(char) * fileSize);

	c = fgetc(file);

	while (c != EOF)
	{
		fileContents[i++] = c;
		c = fgetc(file);
	}

	fclose(file);

	fileContents[i] = '\0';


	for (i = 0; i < TILE_TYPES; ++i)
	{
		while (sscanf(fileContents, " %s\n%n", buf, &n) == 1)
		{
			if (j >= numCells || buf == '~')
			{
				break;
			}

			fileContents += n;

			if (sscanf(fileContents, " %i,%i,%i\n%n", &data->tileTypes[j].mappingColor.r, &data->tileTypes[j].mappingColor.g, &data->tileTypes[j].mappingColor.b, &n) == 3)
			{
				slog("Mapping color: %i, %i, %i", data->tileTypes[j].mappingColor.r, data->tileTypes[j].mappingColor.g, data->tileTypes[j].mappingColor.b);

				fileContents += n;

				++j;
			}
		}
	}
	
	
}

void FillMapTiles(TileMapData *data, TileMap * map, Bool gravity)
{
	int x, y, ite, i;
		while (!data->flag)
		{			//3 tile types
			for (i = 0; i < TILE_TYPES; ++i)
			{
				if (map->currentTileFilled.x >= map->numColumns && map->currentTileFilled.y >= map->numRows)
				{
					data->flag = true;
					break;
				}

				for (y = 0; y < map->numRows; ++y)
				{
					for (x = 0; x < map->numColumns; ++x)
					{
						for (ite = 0; ite < map->numCells; ++ite)
						{
							if (data->colors[(y*map->numColumns) + x].r == 0
								&& data->colors[(y*map->numColumns) + x].g == 0
								&& data->colors[(y*map->numColumns) + x].b == 0
								&& map->map[y][x].filled != 1)
							{
								map->map[y][x].active = false;

								map->map[y][x].filled = 1;
								map->map[y][x].walkable = false;
								map->map[y][x].offsetInColumn = -1;

								map->map[y][x].sprite = map->mapSpriteSheet;
								
								map->map[y][x].boundingBox = gf2d_rect(x * map->cellWidth, 
									y * map->cellHeight, 
									map->cellWidth, 
									map->cellHeight);

								map->currentTileFilled.x++;
								map->currentTileFilled.y++;

								continue;
							}

							if (data->colors[(y*map->numColumns) + x].r == data->tileTypes[ite].mappingColor.r
								&& data->colors[(y*map->numColumns) + x].g == data->tileTypes[ite].mappingColor.g
								&& data->colors[(y*map->numColumns) + x].b == data->tileTypes[ite].mappingColor.b
								&& !map->map[y][x].active)
							{
								map->map[y][x].active = true;
								map->map[y][x].offsetInColumn = ite;
								map->map[y][x].sprite = map->mapSpriteSheet;

								map->currentTileFilled.x++;
								map->currentTileFilled.y++;

								map->map[y][x].filled = 1;

								map->map[y][x].walkable = true;

								if (map->map[y][x].offsetInColumn != GROUND_CENTER_1 && map->map[y][x].offsetInColumn != GROUND_CENTER_2 && map->map[y][x].offsetInColumn != INSIDE_CENTER_WALKABLE)
								{
									map->map[y][x].walkable = false;
									map->numBlockingTiles++;
									map->map[y][x].boundingBox = gf2d_rect(x * map->cellWidth,
										y * map->cellHeight,
										map->cellWidth,
										map->cellHeight);
								}

								continue;
							}
						}
					}
				}
			}
		}
}

