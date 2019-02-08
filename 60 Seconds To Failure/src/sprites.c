#include "sprites.h"
#include "graphics.h"
#include "simple_logger.h"

typedef struct
{
	Uint32 maxSprites;
	Sprite *spriteList;
}SpriteManager;

static SpriteManager spriteManager;

void SpriteManagerInit(Uint32 max)
{
	if (max < 1)
	{
		slog("Cant allocate 0 sprites");
		return;
	}

	spriteManager.maxSprites = max;
	spriteManager.spriteList = (Sprite *)malloc(sizeof(Sprite)*max);
	
	//Allocate memory for 'maxSprites' number of sprites
	memset(spriteManager.spriteList, 0, sizeof(Sprite)*max);

	if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
	{
		slog("failed to init image: %s", SDL_GetError());
	}

	slog("sprite system initialized");

	atexit(IMG_Quit);
	atexit(SpriteManagerClose);
}

Sprite* LoadImageToTexture(const char *filepath, SDL_Renderer *ren)
{
	Sprite *s = NewSprite();
	s->texture = IMG_LoadTexture(ren, filepath);

	if (!s->texture)
	{
		slog("Failed to load image: %s", IMG_GetError());

		return NULL;
	}

	s->filepath = filepath;
	s->_refCount++;

	return s;
}

Animation* LoadAnimation(const char* filepath, 
	SDL_Renderer *ren, 
	Uint32 cellW, 
	Uint32 cellH, 
	Uint32 xOffset, 
	Uint32 frames, 
	Uint32 msToNext)
{
	Animation* newAnim = (Animation*)malloc(sizeof(Animation));
	newAnim->spriteList = (Sprite *)malloc(sizeof(Sprite)*frames);

	newAnim->curFrame = 0;
	newAnim->length = frames;
	newAnim->timeLeft = msToNext;
	newAnim->timeLeftValue = msToNext;
	newAnim->frameHeight = cellH;
	newAnim->frameWidth = cellW;
	newAnim->offset = xOffset;

	newAnim->spriteList = LoadImageToTexture(filepath, ren);
	
	return newAnim;
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

void DrawSprite(SDL_Texture *tex, SDL_Renderer *r, Vector2D pos, float deltaTime)
{
	SDL_Rect dest;
	dest.x = pos.x;
	dest.y = pos.y;

	SDL_RenderCopy(r, tex, NULL, &dest);
}

void DrawAnimatedSprite(
	Animation *anim,
	Vector2D position,
	Vector2D *scale,
	Vector2D *pivot,
	Vector2D *flip,
	Vector4D *colorShift,
	Uint32 frameDelay,
	float deltaTime)
{
	SDL_Rect cell, target;
	SDL_RendererFlip flipFlags = SDL_FLIP_NONE;
	SDL_Point r;
	Vector2D scaleFactor = { 1, 1 };
	Vector2D scaleOffset = { 0, 0 };

	anim->timeLeft -= deltaTime;
	if (anim->timeLeft <= 0)
	{
		anim->timeLeft = anim->timeLeftValue;
		++anim->curFrame;
		if (anim->curFrame >= anim->length)
		{
			anim->curFrame = 0;
		}
	}

	gf2d_rect_set(
		cell,
		(int)anim->curFrame * anim->frameWidth + (int)anim->curFrame,
		0,
		anim->frameWidth,
		anim->frameHeight);
	gf2d_rect_set(
		target,
		position.x - (scaleFactor.x * scaleOffset.x),
		position.y - (scaleFactor.y * scaleOffset.y),
		anim->frameWidth * scaleFactor.x,
		anim->frameHeight * scaleFactor.y);
	
	SDL_RenderCopyEx(GetRenderer(),
		anim->spriteList->texture,
		&cell,
		&target,
		0, 
		NULL,
		flipFlags);
}


Sprite* NewSprite()
{
	int i;

	if (!spriteManager.maxSprites)
	{
		return NULL;
	}

	//look for unused address
	for (i = 0; i < spriteManager.maxSprites; ++i)
	{
		if (spriteManager.spriteList[i]._refCount == 0 && spriteManager.spriteList[i].texture == NULL)
		{
			spriteManager.spriteList[i]._refCount = 1;
			return &spriteManager.spriteList[i];
		}
	}

	//look for an unreferenced sprite
	for (i = 0; i < spriteManager.maxSprites; ++i)
	{
		if (spriteManager.spriteList[i]._refCount == 0)
		{
			SpriteDelete(&spriteManager.spriteList[i]);

			spriteManager.spriteList[i]._refCount = 1;
			return &spriteManager.spriteList[i];
		}
	}

	slog("Out of sprite address space");

	return NULL;
}

void DeleteAnimation(Animation *anim)
{
	int i;

	for (i = 0; i < anim->length; ++i)
	{
		SpriteDelete(&anim->spriteList[i]);
	}

	memset(&anim->spriteList, 0, sizeof(Sprite) * anim->length);
}


