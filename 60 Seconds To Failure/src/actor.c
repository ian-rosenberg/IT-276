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

	slog("animation list system closed");
}

void ActorManagerInit(Uint32 max)
{
	if (!max)
	{
		slog("cannot init animation manager for zero animations");
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
	slog("error: out of animation manager's addresses");
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
			}
			if (strcmp(buf, "colorSpecial:") == 0)
			{
				fscanf(file, "%lf,%lf,%lf,%lf", &animations->colorSpecial.x, &animations->colorSpecial.y, &animations->colorSpecial.z, &animations->colorSpecial.w);
			}
			if (strcmp(buf, "yOffset:") == 0)
			{
				fscanf(file, "%i", &animations->yOffset);
			}
			if (strcmp(buf, "name:") == 0)
			{
				fscanf(file, "%s", &animations->name);
			}
			if (strcmp(buf, "filename:") == 0)
			{
				fscanf(file, "%s", &animations->filepath);
			}
			if (strcmp(buf, "length:") == 0)
			{
				fscanf(file, "%i", &animations->length);
			}
			if (strcmp(buf, "frameWidth:") == 0)
			{
				fscanf(file, "%i", &animations->cellWidth);
			}
			if (strcmp(buf, "frameHeight:") == 0)
			{
				fscanf(file, "%i", &animations->cellHeight);
			}
			if (strcmp(buf, "frameRate:") == 0)
			{
				fscanf(file, "%f", &animations->frameRate);
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

