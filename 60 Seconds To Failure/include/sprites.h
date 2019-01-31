#ifndef __SPRITES__
#define __SPRITES__

#include "gf2d_text.h"

#include <SDL_image.h>
#include <SDL.h>

typedef struct Sprite_S
{
	int refCount;
	TextLine filepath;
	SDL_Texture *texture;
	Uint32 framesPerLine;
	Uint32 frameWidth, frameHeight;
}Sprite;

/**
* @brief Initialize the sprite manager
* @param maxSprites the maximum number of sprites to be be simultaneously loaded
*/
void SpriteManagerInit(Uint16 max);

/**
* @brief Free all sprites tracked by manager
*/
void ClearAllSprites();

/**
* @brief Free a specific sprite
*/
void SpriteDelete(Sprite *sprite);

/**
* @brief Close and free all sprites
*/
void SpriteManagerClose();

#endif