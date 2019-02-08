#ifndef __SPRITES__
#define __SPRITES__
/*
Credits: https://opengameart.org/users/buch for overworld sprites
		 https://opengameart.org/users/eris for interior sprites
		 https://opengameart.org/users/balmer & Stephen Challener (Redshrike) for boss sprites
		 https://opengameart.org/users/surt for pipes
*/


#include "gf2d_text.h"
#include "gf2d_vector.h"

#include <SDL_image.h>
#include <SDL.h>
#include <stdarg.h>

/**
* @brief Sprite container struct
*/
typedef struct Sprite_S
{
	int _refCount;
	char* filepath;
	SDL_Texture *texture;
}Sprite;

typedef struct Animation_S
{
	Uint8		length;
	Uint32		frameWidth, frameHeight, offset;

	float		curFrame;
	float		timeLeft, timeLeftValue;

	Sprite		*spriteList;
}Animation;

/**
* @brief Initialize the sprite manager
* @param maxSprites the maximum number of sprites to be be simultaneously loaded
*/
void SpriteManagerInit(Uint32 max);

/**
* @brief Load an image file
* @param path of the file to load
* @param cellWidth Width of animation cell
* @param cellHeight Height of the cell
* @param xOffset The offset between cells
*/
Sprite* LoadImageToTexture(const char* filepath, 
	SDL_Renderer *ren,
	Uint32 cellWidth,
	Uint32 cellHeight,
	Uint32 xOffset);

/**
* @brief Load an image file into an animation
* @param path of the file to load
* @param ren The renderer to use
* @param cellWidth Width of the animation cell
* @param cellHeight Height of the animation cell
* @param xOffset The offset in pixels between cells
* @param frames The number of frames
* @param msToNext Milliseconds between frames
* @returns The animation with all the required data
*/
Animation* LoadAnimation(const char *filepath,
	SDL_Renderer *ren,
	Uint32 cellWidth,
	Uint32 cellHeight,
	Uint32 xOffset,
	Uint32 frames,
	Uint32 msToNext);

/**
* @brief Free all sprites tracked by manager
*/
void ClearAllSprites();

/**
* @brief Free a specific sprite
* @param sprite The sprite address to free
*/
void SpriteDelete(Sprite *sprite);

/**
* @brief Close and free all sprites
*/
void SpriteManagerClose();

/**
* @brief Copy texture of static, single sprite object
* @param tex The texture to copy
* @param r The renderer to copy to
* @param pos Position on-screen to render at
*/
void DrawSprite(SDL_Texture *tex, SDL_Renderer *r, Vector2D pos);

/**
* @brief Render an animated sprite
* @param sprite The sprite info
* @param position Where to draw on-screen
* @param scale The x/y scale value
* @param pivot Pivot for the scale/rotation
* @param flip The axis in which to flip the sprite over
* @param colorShift The color to shift the sprite to
*/
void DrawAnimatedSprite(
	Animation *anim,
	Vector2D position,
	Vector2D *scale,
	Vector2D *pivot,
	Vector2D *flip,
	Vector4D *colorShift,
	Uint32 frameDelay,
	float deltaTime);

/**
* @brief Create a new sprite if not in use,
* otherwise increment the reference count
* @returns A new or currently loaded sprite to reference
*/
Sprite* NewSprite();

/**
* @brief Clean up all sprites in an animation
* @param anim The animation to clean up
*/
void DeleteAnimation(Animation *anim);


#endif