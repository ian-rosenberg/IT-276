#ifndef __TILEMAP__
#define __TILEMAP__

#include <SDL.h>
#include "sprites.h"
#include "gf2d_types.h"
#include "gf2d_vector.h"
#include "physicsbodies.h"


typedef enum {
	Ground = 0,
	Flooring = 1,
	Building = 2
}TileType;

typedef enum {
	TopLeft = 0,
	TopRight = 1,
	BottomLeft = 2,
	BottomRight = 3,
	Right = 4,
	Left = 5,
	Bottom = 6,
	Top = 7,
	Center1 = 8,
	Cneter2 = 9
}TileAlignmentType;

typedef struct Tile_S
{
	int					_refCount;
	
	TileType			tileType;
	TileAlignmentType	tileGroup;
	Sprite				*sprite;
	Vector2D			tilePos;

	SDL_Color			color;
	SDL_Color			mappingColor;

	Uint32				offsetInColumn;

	Bool				active;
}Tile;

typedef struct
{
	Uint32			numColumns, numRows, cellWidth, cellHeight, numCells;

	Uint8			_inUse;

	//cpSpace			*levelSpace;

	//BodyInfo		*levelBody;
	
	Vector2D		spawnPos, bossPos;
	Vector2D		*shelterPositions;

	Vector2D		currentTileFilled;

	Tile			**overworld;
	Sprite			*overworldSpriteSheet;

	Tile			*sideView;
	Sprite			*sideViewSprite;

	Sprite			*shelter;
	
	Sprite			*bossArea;

	char			*overworldName;
	char			*shelterName;
	char			*bossAreaName;
}TileMap;

/**
* @brief Intialize the tilemap system
* @param max The number of tilemaps to support
*/
void TileMapInit(Uint32 max);

/**
* @brief Init the tile system
* @param max The number of tiles to support
*/
void TileInit(Uint32 max);

/**
* @brief Find and add an unused/referenced tilemap to use
* @param width The width in cells of the tilemap
* @param height The height in cells of the tilemap
* @returns A new tile map
*/
TileMap* NewTileMap(Uint32 width, Uint32 height);

/**
* @brief Return a new/unreferenced tile from the manager
* @param offsetInColumn The offset in the column of sprites
* @param sprite Sprite to assign
* @returns the new tile
*/
Tile* NewTile(Uint32 offsetInColumn, Sprite *sprite);

/**
* @brief Close the tilemanager system and clean up all tiles
*/
void TileMapManagerClose();

/**
* @brief Generate a tilemap from config file
* @param filename The file to load
* @returns a tilemap loaded from the specified file
*/
TileMap* LoadOverworldTileMapFromFile(char* filename);

/**
* @brief Memset and free a tilemap
* @param map The tilemap to delete
*/
void TileMapDelete(TileMap *map);

/**
* @brief Delete a tile
* @param tile The tile to delete
*/
void TileDelete(Tile* tile);

/**
* @brief Draw a tilemap
* @param the map to draw
*/
void DrawOverworldTileMap(TileMap *map);

/**
* @brief Retrieve the current tilemap
* @returns the current overworld's tilemap
*/
TileMap* GetCurrentTileMap();
#endif