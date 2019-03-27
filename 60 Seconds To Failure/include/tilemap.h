#ifndef __TILEMAP__
#define __TILEMAP__

#include <SDL.h>
#include "sprites.h"
#include "gf2d_types.h"
#include "gf2d_vector.h"
#include "entity.h"

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

	Uint32			maxEntities;
	Entity			*entityList;

	Entity			*self;
	
	Vector2D		spawnPos, bossPos;
	Vector2D		*shelterPositions;

	Vector2D		currentTileFilled;

	Tile			**map;
	Sprite			*mapSpriteSheet;

	//Tile			*sideView;
	//Sprite			*sideViewSprite;

	//Sprite			*shelter;
	
	//Sprite			*bossArea;

	char			*mapName;
	//char			*shelterName;
	//char			*bossAreaName;

	SDL_Rect		srcRect;
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
* @brief Init the tilemap's entities
* @param map The map to add enemies to
*/
void TileInitEntities(Uint32 max, TileMap* map);

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
TileMap* LoadTileMapFromFile(char* filename);

/**
* @brief Memset and free a tilemap
* @param mapEntity The tilemap to delete
*/
void TileMapDelete(Entity *mapEntity);

/**
* @brief Delete a tile
* @param tile The tile to delete
*/
void TileDelete(Tile* tile);

/**
* @brief Draw a tilemap
* @param the map to draw
*/
void DrawMap(TileMap *map);

/**
* @brief Retrieve the current tilemap
* @returns the current map's tilemap
*/
TileMap* GetCurrentTileMap();

/**
* @brief Render the entire overworld to a render target texture
* @param map The map to use
*/
void RenderMapToTexture(TileMap *map);

/**
* @brief Add to the entity list of the current tilemap
* @param ent The entity to add
* @returns true on success false on failure
*/
Bool AddEntityToTileMap(Entity *ent, TileMap* map);

/**
* @brief Retrieve the current map's dimensions
* @param owner The map to get dimensions of
*/
Vector2D GetCurrentTileMapDimensions(Entity *owner);
#endif