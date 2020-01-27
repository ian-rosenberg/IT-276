#ifndef __TILEMAP__
#define __TILEMAP__

#include <SDL.h>
#include "sprites.h"
#include "entity.h"
#include "gf2d_types.h"
#include "gf2d_list.h"
#include "gf2d_vector.h"

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
	Center2 = 9
}TileAlignmentType;

typedef struct Tile_S
{
	int					_refCount;
	int					filled;

	TileType			tileType;
	TileAlignmentType	tileGroup;
	Sprite				*sprite;
	Vector2D			tilePos;

	Rect				boundingBox;

	SDL_Color			color;
	SDL_Color			mappingColor;

	Uint32				offsetInColumn;

	Bool				active;
	Bool				walkable;
}Tile;

typedef struct TileMapData_S {
	Uint32 col, row, cellWidth, cellHeight, numCells;
	const char *mapName;
	const char *emptyTileFileName;
	Bool flag;
	SDL_Color *colors;
	Tile* tileTypes;
	char *colorMap;
	const char *tileGenInfoName;
}TileMapData;

typedef struct TileMap_S
{
	Uint8			_inUse;
	Uint8			active;
	Uint8			gravityActive;
	
	Vector2D		spawnPos, bossPos;
	Vector2D		*shelterPositions;

	Vector2D		currentTileFilled;

	Vector2D		position;

	Tile			**map;
	
	Uint32			numColumns;
	Uint32			numRows;
	Uint32			numCells;
	Uint32			cellWidth;
	Uint32			cellHeight;

	char			*mapName;
	char			*emptyTileFileName;
	Sprite			*mapSpriteSheet;
	Sprite			*emptyTile;

	Uint32			numEnts;
	Entity			*ents;
	SDL_Rect		boundingBox, srcRect;

	char			*backgroundImage;    /**<background image for this level*/
	char			*backgroundMusic;    /**<background music for this level*/

	SDL_Texture		*renderTarget;
}TileMap;

/**
* @brief Intialize the tile map manager given a number of tilemaps
*/
void TileMapManagerInit(Uint32 max);

/**
* @brief Find and add an unused/referenced tilemap to use
* @param width The width in cells of the tilemap
* @param height The height in cells of the tilemap
* @returns A new tile map
*/
TileMap* NewTileMap(Uint32 width, Uint32 height, Bool gravity);

/**
* @brief Generate a tilemap from config file
* @param filename The file to load
* @returns a tilemap loaded from the specified file
*/
TileMap* LoadTileMapFromFile(char* filename);

/**
* @brief delete a tilemap
*/
void TileMapDelete(TileMap *map);

/**
* @brief Draw a tilemap
* @param the map to draw
*/
void DrawMap(TileMap *map);

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
Bool AddEntityToCurrentTileMap(Entity* ent);

/**
* @brief Retrieve the current map's dimensions
* @param owner The map to get dimensions of
*/
Vector2D GetCurrentTileMapDimensions(TileMap *map);

/**
* @brief Retrieve the current active tile map
*/
TileMap* GetCurrentTileMap();

/**
* @brief Remove entity from tilemap
* @param map The map to remove from
* @param ent The entity's body to remove
*/
void MapRemoveEntity(TileMap *map, Entity *ent);

/**
* @brief Update the map and it's entities
*/
void MapUpdate(TileMap *map);

/**
* @brief Parses the config file of the map
* @param name Of config file to parse
* @param map To create
* @param data Tilemap data to populate
* @returns A pointer to the parsed tilemap
*/
TileMap* ParseMapFile(char* filename, TileMapData *data);

/**
* @brief Parse the file with the mapping colors
* @param data The map's data to populate
*/
void ParseTileColors(TileMapData *data, Uint32 numCells);

/**
* @brief Function to mark tiles as active or not, and to 
* check if a tile is an edge or a corner for collisions
* @param data The generation data for this tilemap
* @param map Map to generate
* @param gravity If this is true, then the map is a sideView
*/
void FillMapTiles(TileMapData *data, TileMap * map);

/*
* @brief Handle collisions between entities and non-walkable tiles
* in the overworld tilemap
* @param self The tile that is being collided with
* @param other The colliding entity
*/
Bool EntityTileTouch(Tile *self, Entity *other);
#endif