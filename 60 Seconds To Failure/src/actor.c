#include "actor.h"
#include "simple_logger.h"
#include "graphics.h"

#include <stdio.h>

typedef struct
{
	Uint32 maxActors;

	Actor *actors;
}ActorManager;

//local global
static ActorManager actorManager = { 0 };


void ActorManagerClose(void)
{
	ClearAllActors();

	if (actorManager.actors != NULL)
	{
		free(actorManager.actors);
	}
	actorManager.actors = NULL;
	actorManager.maxActors = 0;

	slog("actor list system closed");
}

void ActorManagerInit(Uint32 max)
{
	if (!max)
	{
		slog("cannot init actor manager for zero actors");
		return;
	}

	actorManager.maxActors = max;

	actorManager.actors = (Actor*)malloc(sizeof(Actor)*max);

	memset(actorManager.actors, 0, sizeof(Actor)* max);

	atexit(ActorManagerClose);
}

Actor* NewActor(Uint32 numAnim)
{
	int i;
	Actor* act = NULL;

	for (i = 0; i < actorManager.maxActors; i++)
	{
		if (actorManager.actors[i]._inUse == 0 || actorManager.actors[i]._inUse == NULL)
		{
			memset(&actorManager.actors[i], 0, sizeof(Actor));
			actorManager.actors[i].animations = (Animation*)malloc(sizeof(Animation)* numAnim);

			act = &actorManager.actors[i];

			slog("reusing actor");
			act->_inUse = 1;

			for (i = 0; i < numAnim; ++i)
			{
				act->animations = NewAnimation();

				++act->animations;
			}

			return act;
		}
	}

	/*find an unused actor address and clean up the old data*/
	for (i = 0; i < actorManager.maxActors; i++)
	{
		if (actorManager.actors[i]._inUse == 0)
		{
			memset(&actorManager.actors[i], 0, sizeof(Actor));
			actorManager.actors[i].animations = (Animation*)malloc(sizeof(Animation)* numAnim);
			
			act = &actorManager.actors[i];
			
			act->_inUse = 1;

			for (i = 0; i < numAnim; ++i)
			{
				act->animations = NewAnimation();

				++act->animations;
			}

			slog("cleaned up old actor");

			return act;
		}
	}
	slog("error: out of actor manager's addresses");
	return NULL;
}

Actor* NewActorByName(char *name)
{
	int i;
	Actor* act = NULL;

	for (i = 0; i < actorManager.maxActors; i++)
	{
		if (actorManager.actors[i]._inUse == 0 || actorManager.actors[i]._inUse == NULL)
		{
			memset(&actorManager.actors[i], 0, sizeof(Actor));

			*actorManager.actors[i].name = name;

			act = &actorManager.actors[i];

			slog("reusing actor");
			act->_inUse = 1;

			return act;
		}
	}

	/*find an unused actor address and clean up the old data*/
	for (i = 0; i < actorManager.maxActors; i++)
	{
		if (actorManager.actors[i]._inUse == 0)
		{
			memset(&actorManager.actors[i], 0, sizeof(Actor));

			*actorManager.actors[i].name = name;

			act = &actorManager.actors[i];

			act->_inUse = 1;

			slog("cleaned up old actor");

			return act;
		}
	}
	slog("error: out of actor manager's addresses");
	return NULL;
}

void DeleteActor(Actor *actor)
{
	int i;
	
	if (!actor)
	{
		return;
	}

	if (actor->animations)
	{
		for (i = 0; i < actor->numAnimations; ++i)
		{
			DeleteAnimation(&actor->animations[i]);
		}
	}

	FreeActor(actor);
}

void ClearAllActors(void)
{
	int i;

	for (i = 0; i < actorManager.maxActors; ++i)
	{
		DeleteActor(&actorManager.actors[i]);
	}
}

Actor* LoadActor(char *filename)
{
	FILE * file;
	char buf[512];
	int i = 0;
	int j = 0;
	int flag = -1;
	Actor *actor;
	Animation *animations = NewAnimation();
	
	file = fopen(filename, "r");

	if (!file)
	{
		return NULL;
	}

	while (fscanf(file, "%s", buf) != EOF)
	{
		if (strcmp(buf, "numAnimations:") == 0)
		{
			fscanf(file, "%i", &i);

			break;
		}
	} 

	actor = NewActor(i);

	rewind(file);

	while (flag < 0)
	{
		while (fscanf(file, "%s", buf) != EOF)
		{
			if (strcmp(buf, "actorName:") == 0)
			{
				fscanf(file, "%s", &actor->name);
			}
			if (strcmp(buf, "color:") == 0)
			{
				fscanf(file, "%lf,%lf,%lf,%lf", &actor->color.x, &actor->color.y, &actor->color.z, &actor->color.w);

				flag = 1;
			}
		}
	}


	memset(buf, 0, sizeof(buf));

	flag = -1;

	rewind(file);

	for (j = 0; j < i; ++j)
	{
		while (fscanf(file, "%s", buf) != EOF)
		{
			if (strcmp(buf, "scale:") == 0)
			{
				fscanf(file, "%lf,%lf", &animations->scale.x, &animations->scale.y);
				continue;
			}
			if (strcmp(buf, "colorSpecial:") == 0)
			{
				fscanf(file, "%lf,%lf,%lf,%lf", &animations->colorSpecial.x, &animations->colorSpecial.y, &animations->colorSpecial.z, &animations->colorSpecial.w);
				continue;
			}
			if (strcmp(buf, "yOffset:") == 0)
			{
				fscanf(file, "%i", &animations->yOffset);
				continue;
			}
			if (strcmp(buf, "name:") == 0)
			{
				fscanf(file, "%s", &animations->name);
				continue;
			}
			if (strcmp(buf, "filename:") == 0)
			{
				fscanf(file, "%s", &animations->filepath);
				continue;
			}
			if (strcmp(buf, "length:") == 0)
			{
				fscanf(file, "%i", &animations->length);
				continue;
			}
			if (strcmp(buf, "frameWidth:") == 0)
			{
				fscanf(file, "%i", &animations->cellWidth);
				continue;
			}
			if (strcmp(buf, "frameHeight:") == 0)
			{
				fscanf(file, "%i", &animations->cellHeight);
				continue;
			}
			if (strcmp(buf, "frameRate:") == 0)
			{
				fscanf(file, "%f", &animations->frameRate);
				continue;
			}
			if (strcmp(buf, "type:") == 0)
			{
				fscanf(file, "%s", buf);
				if (strcmp(buf, "loop") == 0)
				{
					animations->animType = AT_LOOP;
					flag = 1;
				}
				if (strcmp(buf, "once") == 0)
				{
					animations->animType = AT_ONCE;
					flag = 1;
				}
			}

			if (flag > 0)
			{
				animations->sprite = LoadImageToTexture(animations->filepath, GetRenderer());

				slog("Loaded animation: %s for actor: %s", animations->name, actor->name);

				animations++;

				++actor->numAnimations;

				flag = -1;		
			}
		}

		memset(buf, 0, sizeof(buf));
	}

	slog("%s actor loaded!", actor->name);

	fclose(file);

	for (i = 0; i < actor->numAnimations; ++i)
	{

		actor->animations = &animations[i];

		++actor->animations;
	}

	return actor;
}


void FreeActor(Actor *actor)
{
	int i;

	if (!actor)
	{
		return;
	}
	
	for (i = 0; i < actor->numAnimations; ++i)
	{
		FreeAnimation(actor->animations);
	}

	memset(actor, 0, sizeof(Actor));
}

Vector2D GetAverageActorDimensions(Actor *actor)
{
	int i;
	Vector2D maxDim = { 0 };
	Vector2D minDim = { 0 };
	Vector2D avgDim = { 0 };

	for (i = 0; i < actor->numAnimations; ++i)
	{
		maxDim.x = MAX(actor->animations[i].cellWidth, maxDim.x);
		maxDim.y = MAX(actor->animations[i].cellHeight, maxDim.y);

		minDim.x = MIN(actor->animations[i].cellWidth, minDim.x);
		minDim.y = MIN(actor->animations[i].cellHeight, minDim.y);
	}

	avgDim.x = (maxDim.x - minDim.x) / 2;
	avgDim.y = (maxDim.y - minDim.y) / 2;

	return avgDim;
}

