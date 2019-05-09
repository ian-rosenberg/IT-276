#include "worlds.h"
#include "graphics.h"
#include "camera.h"
#include "simple_logger.h"

static World gameWorld = { 0 };

void WorldInit(char *oFilename, char *sFilename)
{
	gameWorld.overworld = LoadTileMapFromFile(oFilename, false);

	gameWorld.faded = 0;
	gameWorld.fadeActive = 0;
	//gameWorld.sideView = LoadTileMapFromFile(sFilename, true);

	gameWorld.overworldPixelWidth = gameWorld.overworld->numColumns * gameWorld.overworld->cellWidth;
	gameWorld.overworldPixelHeight = gameWorld.overworld->numRows * gameWorld.overworld->cellHeight;

	gameWorld.transitionActor = NewActor(0);
	gameWorld.transitionActor->currentSprite = LoadImageToTextureWithAlpha("images/transitionImage.png", GetRenderer());
	gameWorld.transitionActor->color = vector4d( 0,0,0,255 );
	SDL_SetTextureAlphaMod(gameWorld.transitionActor->currentSprite->texture,0);

	//gameWorld.sidePixelWidth = gameWorld.sideView->numColumns * gameWorld.sideView->cellWidth;
	//gameWorld.sidePixelHeight = gameWorld.sideView->numRows * gameWorld.sideView->cellHeight;
}

Vector2D GetWorldDimensions(World *world, Bool gravity)
{
	Vector2D dim = { 0 };
	if (!gravity)
	{
		dim.x = world->overworldPixelWidth;
		dim.y = world->overworldPixelHeight;
	}
	else
	{
		dim.x = world->sidePixelWidth;
		dim.y = world->sidePixelHeight;
	}

	return dim;
}

void CleanUpWorld(World *world)
{
	TileMapDelete(world->overworld);

	TileMapDelete(world->sideView);

	memset(world, 0, sizeof(World));
	
	free(world);
}

World* GetGameWorld()
{
	return &gameWorld;
}

void Fade()
{
	SDL_Rect renderQuad = { GetCameraPosition().x, GetCameraPosition().y, GetRenderDimensions().x, GetRenderDimensions().y };
	Uint8 alpha = 0;
	
	SDL_GetTextureAlphaMod(gameWorld.transitionActor->currentSprite->texture, &alpha);

	if (alpha < 255 && !gameWorld.faded)
	{
		alpha += 1;
		
		SDL_SetTextureAlphaMod(gameWorld.transitionActor->currentSprite->texture, alpha);

		if (alpha >= 255 && !gameWorld.faded)
		{
			gameWorld.faded = true;
		}
	}
	else if (alpha > 0 && gameWorld.faded)
	{
		alpha -= 1;
		
		SDL_SetTextureAlphaMod(gameWorld.transitionActor->currentSprite->texture, alpha);

		if (alpha <= 0 && gameWorld.faded)
		{
			gameWorld.fadeActive = false;
			gameWorld.faded = false;
		}
	}

	SDL_RenderCopy(GetRenderer(), gameWorld.transitionActor->currentSprite->texture, NULL, &renderQuad);
}