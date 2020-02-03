#include "sprites.h"
#include "graphics.h"
#include "simple_logger.h"

typedef struct
{
	Uint32 maxSprites;
	Sprite *spriteList;
}SpriteManager;

static SpriteManager spriteManager = { 0 };

void SpriteManagerInit(Uint32 max)
{
	int i = 0;

	if (max < 1)
	{
		slog("Cant allocate 0 sprites");
		return;
	}

	spriteManager.maxSprites = max;
	spriteManager.spriteList = malloc(max * sizeof(Sprite));//Allocate memory for 'maxSprites' number of sprites

	memset(spriteManager.spriteList, 0, sizeof(Sprite)*max);

	for (; i < spriteManager.maxSprites; i++)
	{
		spriteManager.spriteList[i].filepath = malloc(sizeof(char)* GF2DLINELEN);
		spriteManager.spriteList[i].surface = NULL;
		spriteManager.spriteList[i].texture = NULL;
		spriteManager.spriteList[i]._refCount = 0;
	}

	if (!(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) & IMG_INIT_PNG | IMG_INIT_JPG))
	{
		slog("failed to init image: %s", SDL_GetError());
	}

	slog("sprite system initialized");

	atexit(IMG_Quit);
	//atexit(SpriteManagerClose);
}

Sprite* LoadImageToTexture(char *filepath, SDL_Renderer *ren)
{
	slog("Attempting to load %s...", filepath);

	Sprite *sprite = NULL;
	SDL_Surface *s = NULL;

	sprite = NewSprite();

	if (!sprite)
	{
		return NULL;
	}

	s = IMG_Load(filepath);

	if (!s)
	{
		slog("Failed to load image: %s, %s", filepath, IMG_GetError());
		SpriteDelete(sprite);
		return NULL;
	}

	s = gf2d_graphics_screen_convert(&s);

	if (!s)
	{
		slog("Failed to convert image: %s, %s", filepath, IMG_GetError());
		SpriteDelete(sprite);
		return NULL;
	}

	sprite->texture = SDL_CreateTextureFromSurface(GetRenderer(), s);

	if (!sprite->texture)
	{
		slog("Failed to convert to texture: %s, %s", filepath, IMG_GetError());
		SpriteDelete(sprite);
		return NULL;
	}

	SDL_SetTextureBlendMode(sprite->texture, SDL_BLENDMODE_BLEND);
	SDL_UpdateTexture(sprite->texture,
		NULL,
		s->pixels,
		s->pitch);

	sprite->filepath = filepath;
	sprite->_refCount++;
	sprite->surface = s;


	if (SDL_QueryTexture(sprite->texture,
		&sprite->pixelFmt,
		NULL,
		&sprite->width,
		&sprite->height) != 0)
	{
		slog("Error querying %s", sprite->filepath);

		SpriteDelete(sprite);
	}
	else
	{
		slog("%s loaded successfully!", sprite->filepath);
	}


	return sprite;
}

Sprite* LoadImageToTextureWithDimensions(char *filepath, SDL_Renderer *ren, Vector3D dim)
{
	Sprite * sprite = NULL;

	sprite = LoadImageToTexture(filepath, ren);

	sprite->width = dim.x;
	sprite->height = dim.y;
	sprite->bodyRadius = sqrt(SQUARE(dim.y) + SQUARE(dim.x));

	return sprite;
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
	int i;
	
	if (sprite == NULL)
	{
		return;
	}

	for (i = 0; i < spriteManager.maxSprites; ++i)
	{
		if (&spriteManager.spriteList[i] == sprite)
		{
			if (sprite->surface != NULL)
			{
				SDL_FreeSurface(sprite->surface);
				sprite->surface = NULL;
			}

			if (sprite->texture != NULL)
			{
				SDL_DestroyTexture(sprite->texture);
				sprite->texture = NULL;
			}

			if (sprite->filepath)
			{
				free(sprite->filepath);
			}

			return;
		}
	}
}

void SpriteManagerClose()
{
	ClearAllSprites();

	free(spriteManager.spriteList);
}

void DrawSprite(Sprite *sprite,
	Vector2D drawPosition,
	Vector2D *scale,
	Vector2D *scaleCenter,
	Vector3D *rotation,
	Vector2D *flip,
	Vector4D *colorShift,
	Uint32 frame,
	Uint32 offset,
	Uint32 frameWidth,
	Uint32 frameHeight)
{
	SDL_Rect cell, target;
	SDL_RendererFlip flipFlags = SDL_FLIP_NONE;
	SDL_Point r;
	Vector2D scaleFactor = { 1, 1 };
	Vector2D scaleOffset = { 0, 0 };

	if (!sprite)
	{
		return;
	}
	if (scale != NULL)
	{
		vector2d_copy(scaleFactor, (*scale));
	}
	else
	{
		vector2d_copy(scaleFactor, (vector2d(1, 1)));
	}
	if (scaleCenter != NULL)
	{
		vector2d_copy(scaleOffset, (*scaleCenter));
	}
	else
	{
		vector2d_copy(scaleOffset, (vector2d(0.5f, 0.5f)));
	}
	if (rotation != NULL)
	{
		vector2d_copy(r, (*rotation));
		r.x *= scaleFactor.x;
		r.y *= scaleFactor.y;
	}
	else
	{
		vector2d_copy(r, vector2d(0, 1));
	}

	if (flip)
	{
		if (flip->x)flipFlags = (SDL_RendererFlip)(flipFlags | SDL_FLIP_HORIZONTAL);
		if (flip->y)flipFlags = (SDL_RendererFlip)(flipFlags | SDL_FLIP_VERTICAL);
	}
	else
	{
		flipFlags = SDL_FLIP_NONE;
	}

	if (colorShift)
	{
		SDL_SetTextureColorMod(
			sprite->texture,
			colorShift->x,
			colorShift->y,
			colorShift->z);
		SDL_SetTextureAlphaMod(
			sprite->texture,
			colorShift->w);
	}

	gf2d_rect_set(
		cell,
		0,
		(frameHeight * (int)frame),
		frameWidth,
		frameHeight);
	gf2d_rect_set(
		target,
		drawPosition.x - (scaleFactor.x * scaleOffset.x),
		drawPosition.y - (scaleFactor.y * scaleOffset.y),
		frameWidth * scaleFactor.x,
		frameHeight * scaleFactor.y);

	SDL_RenderCopyEx(GetRenderer(),
		sprite->texture,
		&cell,
		&target,
		rotation ? rotation->z : 0,
		rotation ? &r : NULL,
		flipFlags);
	if (colorShift)
	{
		SDL_SetTextureColorMod(
			sprite->texture,
			255,
			255,
			255);
		SDL_SetTextureAlphaMod(
			sprite->texture,
			255);
	}
}

void DrawSpriteImage(Sprite *image, Vector2D position, Uint32 width, Uint32 height)
{
	DrawSprite(
		image,
		position,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		0,
		0,
		width,
		height);
}

Sprite* NewSprite()
{
	int i;

	if (!spriteManager.maxSprites)
	{
		slog("Cannot create new sprites with a max of 0");
		
		return NULL;
	}

	//look for unused address
	for (i = 0; i < spriteManager.maxSprites; ++i)
	{
		if (spriteManager.spriteList[i]._refCount == 0 && spriteManager.spriteList[i].texture == NULL)
		{
			spriteManager.spriteList[i]._refCount = 1;
			spriteManager.spriteList[i].surface = NULL;
			spriteManager.spriteList[i].texture = NULL;
			spriteManager.spriteList[i].pixelFmt = 0;
			spriteManager.spriteList[i].filepath = malloc(sizeof(char)* GF2DLINELEN);

			return (spriteManager.spriteList + i);
		}
	}

	//look for an unreferenced sprite
	for (i = 0; i < spriteManager.maxSprites; ++i)
	{
		if (spriteManager.spriteList[i]._refCount == 0)
		{
			SpriteDelete(&spriteManager.spriteList[i]);

			spriteManager.spriteList[i]._refCount = 1;
			spriteManager.spriteList[i].surface = NULL;
			spriteManager.spriteList[i].texture = NULL;
			spriteManager.spriteList[i].pixelFmt = 0;
			spriteManager.spriteList[i].filepath = malloc(sizeof(char)* GF2DLINELEN);

			return (spriteManager.spriteList + i);
		}
	}

	slog("Out of sprite address space");

	return NULL;
}

SDL_Color* GetPixelDataFromFile(const char* file)
{
	int x, y;
	SDL_PixelFormat *fmt;
	SDL_Surface *surf = NULL;
	SDL_Color *pixels = NULL;
	int bpp;

	surf = IMG_Load(file);

	SDL_LockSurface(surf);

	fmt = surf->format;
	bpp = surf->format->BytesPerPixel;

	pixels = (SDL_Color*)malloc(sizeof(SDL_Color)* surf->pitch * surf->h);
	memset(pixels, 0, sizeof(SDL_Color)* surf->pitch * surf->h);

	for (y = 0; y < surf->h; y++)
	{
		for (x = 0; x < surf->w; x++)
		{
			SDL_GetRGB(GetPixel(surf, x, y),
				fmt,
				&pixels[(y * surf->w + x)].r,
				&pixels[(y * surf->w + x)].g,
				&pixels[(y * surf->w + x)].b);
		}
	}

	SDL_UnlockSurface(surf);
	SDL_FreeSurface(surf);

	return pixels;
}

//from SDL docs
Uint32 GetPixel(SDL_Surface *surface, int x, int y)
{
	int bpp = surface->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to retrieve */
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

	switch (bpp) {
	case 1:
		return *p;
		break;

	case 2:
		return *(Uint16 *)p;
		break;

	case 3:
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
			return p[0] << 16 | p[1] << 8 | p[2];
		else
			return p[0] | p[1] << 8 | p[2] << 16;
		break;

	case 4:
		return *(Uint32 *)p;
		break;

	default:
		return 0;       /* shouldn't happen, but avoids warnings */
	}
}

SDL_Texture* CreateBlankTexture(int width, int height, Uint32 fmt)
{
	SDL_Texture *tex = NULL;

	tex = SDL_CreateTexture(GetRenderer(), fmt, SDL_TEXTUREACCESS_TARGET, width, height);

	if (!tex)
	{
		slog("Unable to create blank texture! SDL Error: %s\n", SDL_GetError());

		return NULL;
	}

	slog("Created a blank target texture");
	return tex;
}

Sprite* LoadImageToTextureWithAlpha(const char* filepath, SDL_Renderer *ren)
{
	Sprite *s = LoadImageToTexture(filepath, ren);

	SDL_SetTextureBlendMode(s->texture, SDL_BLENDMODE_BLEND);

	return s;
}