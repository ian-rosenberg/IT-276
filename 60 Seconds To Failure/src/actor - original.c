#include "actor.h"
#include "simple_logger.h"
#include "graphics.h"

#include <string.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>

#define ANIMATION_DATA_KEY_NUM 10

const int COLOR_VAL_LENGTH = 7;

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
		if (actorManager.actors[i]._inUse != 1)
		{			
			act = &actorManager.actors[i];
			act->animations = NewAnimation(numAnim);
			slog("using new actor");
			act->_inUse = 1;


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
	char buf[GF2DLINELEN];
	char tempBuf[GF2DLINELEN];
	int i = 0;
	int j = 0;
	int flag = -1;
	Actor *actor = NULL;
	AnimationData *animData = NULL;
	ActorData *actorData = malloc(sizeof(ActorData));
	Uint32 iteFlag = 0;
	Uint32 numInts = 0;
	int parseArray[4];
	char *pos = NULL;
	
	file = fopen(filename, "r");

	if (!file)
	{
		return NULL;
	}

	while (fscanf(file, "%s", buf) != EOF)
	{
		if (strcmp(buf, "numAnimations:") == 0)
		{
			fscanf(file, "%s", tempBuf);

			actorData->numAnim = atoi(tempBuf);

			if (actorData->numAnim)
			{
				break;
			}
			else
			{
				rewind(file);
			}
		}
	} 

	fclose(file);

	animData = malloc(sizeof(AnimationData) * actorData->numAnim);

	file = fopen(filename, "r");

	if (!file)
	{
		return NULL;
	}

	rewind(file);

	while (flag < 0)
	{
		while (fscanf(file, "%s", buf) != EOF )
		{
			if (strcmp(buf, "actorName:") == 0)
			{
				fscanf(file, "%s", tempBuf);

				strncpy(actorData->name, tempBuf, GF2DLINELEN);

				if (actorData->name == NULL)
				{
					rewind(file);
				}

				continue;
			}
			if (strcmp(buf, "color:") == 0)
			{
				fscanf(file, "%s", tempBuf);
				
				strncpy(actorData->tempData, tempBuf, GF2DLINELEN);
				
				pos = actorData->tempData;


				while ((pos = strpbrk(pos, "0123456789")) != NULL) // strpbrk() prototype in string.h
				{
					sscanf(pos, "%u", &(parseArray[numInts]));

					if (parseArray[numInts] == 0)
						pos++;
					else
						pos += (int)log10(parseArray[numInts]) + 1;        // requires math.h

					numInts++;
				}

				actorData->actorColor.x = parseArray[0];
				actorData->actorColor.y = parseArray[1];
				actorData->actorColor.z = parseArray[2];
				actorData->actorColor.w = parseArray[3];

				if (actorData->actorColor.x >= 0 && actorData->actorColor.x <= 255)
				{
					flag = true;

					break;
				}
			}

			getchar(file);
		}
	}

	flag = -1;

	rewind(file);

	for (j = 0; j < actorData->numAnim; ++j)
	{
		while (fscanf(file, "%s", buf) != EOF && iteFlag < ANIMATION_DATA_KEY_NUM)
		{
			slog("Buffer: %s", buf);

			if (strcmp(buf, "name:") == 0)
			{
				fscanf(file, "%s", tempBuf);

				strncpy(animData[j].tempData, tempBuf, GF2DLINELEN);

				if (animData[j].tempData != NULL)
				{
					strncpy(animData[j].name, animData[j].tempData, GF2DLINELEN);

					iteFlag++;
				}
				continue;
			}
			if (strcmp(buf, "scale:") == 0)
			{
				fscanf(file, "%s", tempBuf);

				strncpy(animData[j].tempData, tempBuf, GF2DLINELEN);

				pos = animData[j].tempData;


				while ((pos = strpbrk(pos, "0123456789")) != NULL) // strpbrk() prototype in string.h
				{
					sscanf(pos, "%u", &(parseArray[numInts]));

					if (parseArray[numInts] == 0)
						pos++;
					else
						pos += (int)log10(parseArray[numInts]) + 1;        // requires math.h

					numInts++;
				}

				animData[j].scale.x = parseArray[0];
				animData[j].scale.y = parseArray[1];


				if (animData[j].scale.x >= 0 && animData[j].scale.x <= 255)
				{
					iteFlag++;
				}
				continue;
			}
			if (strcmp(buf, "filename:") == 0)
			{
				fscanf(file, "%s", tempBuf);

				strncpy(animData[j].tempData, tempBuf, GF2DLINELEN);

				if (animData[j].tempData != NULL)
				{
					strncpy(animData[j].filepath, animData[j].tempData, GF2DLINELEN);

					iteFlag++;
				}
				continue;
			}
			if (strcmp(buf, "colorSpecial:") == 0)
			{
				fscanf(file, "%s", tempBuf);

				strncpy(actorData->tempData, tempBuf, GF2DLINELEN);

				pos = actorData->tempData;


				while ((pos = strpbrk(pos, "0123456789")) != NULL) // strpbrk() prototype in string.h
				{
					sscanf(pos, "%u", &(parseArray[numInts]));

					if (parseArray[numInts] == 0)
						pos++;
					else
						pos += (int)log10(parseArray[numInts]) + 1;        // requires math.h

					numInts++;
				}

				animData[j].colorSpecial.x = parseArray[0];
				animData[j].colorSpecial.y = parseArray[1];
				animData[j].colorSpecial.z = parseArray[2];
				animData[j].colorSpecial.w = parseArray[3];

				if (actorData->actorColor.x >= 0 && actorData->actorColor.x <= 255)
				{
					iteFlag++;
				}

				continue;
			}
			if (strcmp(buf, "yOffset:") == 0)
			{
				fscanf(file, "%s", tempBuf);

				animData[j].yOffset = atoi(tempBuf);

				if (animData[j].yOffset)
				{
					iteFlag++;
				}
				continue;
			}
			if (strcmp(buf, "length:") == 0)
			{
				fscanf(file, "%s", tempBuf);

				animData[j].length = atoi(tempBuf);

				if (animData[j].length != NULL)
				{
					iteFlag++;
				}
				continue;
			}
			if (strcmp(buf, "frameWidth:") == 0)
			{
				fscanf(file, "%s", tempBuf);

				animData[j].cellWidth = atoi(tempBuf);

				if (animData[j].cellWidth != NULL)
				{
					iteFlag++;
				}
				continue;
			}
			if (strcmp(buf, "frameHeight:") == 0)
			{
				fscanf(file, "%s", tempBuf);

				animData[j].cellHeight = atoi(tempBuf);

				if (animData[j].cellHeight != NULL)
				{
					iteFlag++;
				}
				continue;
			}
			if (strcmp(buf, "frameRate:") == 0)
			{
				fscanf(file, "%s", tempBuf);

				animData[j].yOffset = atoi(tempBuf);

				if (animData[j].frameRate)
				{
					iteFlag++;
				}
				continue;
			}
			if (strcmp(buf, "type:") == 0)
			{
				fscanf(file, "%s", tempBuf);

				if (strcmp(tempBuf, "loop") == 0)
				{
					animData[j].animType = AT_LOOP;
				}
				else if (strcmp(tempBuf, "once") == 0)
				{
					animData[j].animType = AT_ONCE;
				}

				if (animData[j].animType != NULL)
				{
					iteFlag++;
				}
				slog("TempBuffer: %s", tempBuf);

				fgets(buf, sizeof(buf), file);
				memset(tempBuf, 0, sizeof(tempBuf));
			}
		}			
		
		iteFlag = 0;
	}

	fclose(file);

	actor = NewActor(actorData->numAnim);

	actor->color = actorData->actorColor;
	gf2d_word_cpy(actor->name, actorData->name);
	actor->numAnimations = actorData->numAnim;

	slog("%s actor loading...", actor->name);

	for (i = 0; i < actorData->numAnim; ++i)
	{
		strncpy(actor->animations[j].filepath, animData[j].filepath, GF2DLINELEN);
		strncpy(actor->animations[j].name, animData[j].name, GF2DLINELEN);

		actor->animations[j].animType = animData[j].animType;
		actor->animations[j].cellHeight = animData[j].cellHeight;
		actor->animations[j].cellWidth = animData[j].cellWidth;

		vector4d_copy(actor->animations[j].colorSpecial, animData[j].colorSpecial);

		actor->animations[j].currentFrame = 0.0f;

		vector2d_copy(actor->animations[j].scale, animData[j].scale);

		actor->animations[j].frameRate = animData[j].frameRate;
		actor->animations[j].yOffset = animData[j].yOffset;
		actor->animations[j].length = animData[j].length;

		actor->animations[j].sprite = LoadImageToTexture(actor->animations[j].filepath, GetRenderer());
		actor->animations[j].currentFrame = 0.0f;
		slog("Loaded animation: %s for actor: %s", actor->animations[j].name, actor->name);
	}

	slog("Loaded actor %s", actor->name);

	free(animData);
	free(actorData);

	return actor;
}


void FreeActor(Actor *actor)
{
	int i;

	if (!actor)
	{
		return;
	}

	if (!actor->animations)
	{
		slog("No animations to delete for empty actor");
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

