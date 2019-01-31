#include "sprites.h"
#include "simple_logger.h"

typedef struct
{
	Uint16 maxSprites;
	Sprite *spriteList;
}SpriteManager;

static SpriteManager spriteManager;

void SpriteManagerInit(Uint16 max)
{
	if (max < 1)
	{
		slog("Cant allocate 0 sprites");
		return;
	}

	spriteManager.maxSprites = max;
	spriteManager.spriteList = (Sprite *)malloc(sizeof(Sprite)*max);
	
	//Allocate memory for maxSprites number of sprites
	memset(spriteManager.spriteList, 0, sizeof(Sprite)*max);

	if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
	{
		slog("failed to init image: %s", SDL_GetError());
	}

	slog("sprite system initialized");

	atexit(IMG_Quit);
	atexit(SpriteManagerClose);
}

void ClearAllSprites()
{
	int i;

	for (i = 0; i < spriteManager.maxSprites; ++i)
	{
		SpriteDelete(&spriteManager.spriteList[i]);
	}
}

void SpriteDelete(Sprite *sprite)
{
	if (!sprite)
	{
		return;
	}
	
	if (sprite->texture != NULL)
	{
		SDL_DestroyTexture(sprite->texture);
	}
	memset(sprite, 0, sizeof(Sprite));//clean up all other data
}

void SpriteManagerClose()
{
	ClearAllSprites();


}