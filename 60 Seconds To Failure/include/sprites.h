#ifndef __SPRITES__
#define __SPRITES__
/*
Credits: https://opengameart.org/users/buch for overworld sprites
		 https://opengameart.org/users/bart for interior sprites
		 https://opengameart.org/users/redshrike for robot sprite
*/


#include "gf2d_text.h"
#include "gf2d_vector.h"

#include <SDL_image.h>
#include <SDL.h>
#include <stdarg.h>
#include <malloc.h>

/**
* @brief Sprite container struct
*/
typedef struct Sprite_S
{
	int _refCount;
	Uint32 width, height;
	float bodyRadius;
	const char* filepath;
	SDL_Texture *texture;
	SDL_Surface *surface;
	Uint32 pixelFmt;
}Sprite;

/**
* @brief Initialize the sprite manager
* @param maxSprites the maximum number of sprites to be be simultaneously loaded
*/
void SpriteManagerInit(Uint32 max);

/**
* @brief Load an image file
* @param path of the file to load
* @param dimData The dimensions data to load a specific rect
* @returns A sprite containing the texture and its dimensions
*/
Sprite* LoadImageToTextureWithDimensions(char* filepath, SDL_Renderer *ren, Vector3D dimData);

/**
* @brief Load an image file
* @param path of the file to load
* @returns A sprite containing the texture
*/
Sprite* LoadImageToTexture(char* filepath, SDL_Renderer *ren);

/**
* @brief Load an image file
* @param path of the file to load
* @returns A sprite containing the texture with alpha blending
*/
Sprite* LoadImageToTextureWithAlpha(char* filepath, SDL_Renderer *ren);


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
* @brief Draw a sprite given it's spritesheet
* @param sprite The sprite to draw
* @param drawPosition The position to draw at
* @param scale For scaling the sprite's current image
* @param scaleCenter The pivot for scaling
* @param rotation The x,y and z rotations
* @param flip Whether or not to flip the sprite horizontally/vertically
* @param colorShift Change the color of a sprite
* @param frame The frame we are currently on
* @param yOffset The offset in the x axis between sprites of an animation
* @param frameWidth The cell/frame width per sprite
* @param frameHeight The cell/frame height per sprite
*/
void DrawSprite(Sprite *sprite,
	Vector2D drawPosition,
	Vector2D *scale,
	Vector2D *scaleCenter,
	Vector3D *rotation,
	Vector2D *flip,
	Vector4D *colorShift,
	Uint32 frame,
	Uint32 yOffset,
	Uint32 frameWidth,
	Uint32 frameHeight);

/**
* @brief Draw a single image to screen
* @param image The image to draw
* @param position The position to draw at
* @param width The width of the image
* @param height The image's height
*/
void DrawSpriteImage(Sprite *image, Vector2D position, Uint32 width, Uint32 height);

/**
* @brief Create a new sprite if not in use,
* otherwise increment the reference count
* @returns A new or currently loaded sprite to reference
*/
Sprite* NewSprite();

/**
* @brief Get a sprite's surface for creating tilemap from pixel data
* @param sprite The sprite to load via it's filepath into a surface
* @returns The colors to work with
*/
SDL_Color* GetPixelDataFromFile(char* file);
/**
* @brief Get pixel data given coordinates
* @param surface The surface to get pixel from
* @param x The x pos
* @param y The y pos
*/
Uint32 GetPixel(SDL_Surface *surface, int x, int y);

/**
* @brief Create a blank texture to render to
* @param width Width of the tilemap to bake
* @param height Height of the tilemap to bake
* @param fmt The format of the file to write to this texture
* @returns An SDL_Texture that is ready to target
*/
SDL_Texture* CreateBlankTexture(int width, int height, Uint32 fmt);
#endif