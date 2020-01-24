#include <string.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>

#include "actor.h"
#include "simple_logger.h"
#include "graphics.h"
#include "gf2d_list.h"

#define ANIMATION_DATA_KEY_NUM 9

const int COLOR_VAL_LENGTH = 4;


Actor* NewActor(Uint32 numAnim)
{
	int j;
	Actor* act = NULL;

	act = (Actor*)malloc(sizeof(Actor));
	memset(act, 0, sizeof(Actor));
	act->animations = NewAnimation();
	slog("using new actor");
	act->_inUse = 1;
	act->animState = 0;
	act->name = malloc(sizeof(char)* GF2DWORDLEN);
	act->currentSprite = NULL;
	act->color = vector4d(0, 0, 0, 0);
	act->numAnimations = numAnim;
	act->scale = vector2d(0, 0);
	act->animState = ART_LOOPING;

	for (j = 0; j < numAnim; ++j)
	{
		act->animations[j].animType = 0;
		act->animations[j].cellHeight = 0;
		act->animations[j].cellWidth = 0;
		act->animations[j].colorSpecial = vector4d(0, 0, 0, 0);
		act->animations[j].currentFrame = 0;
		act->animations[j].filepath = NULL;
		act->animations[j].frameRate = 0;
		act->animations[j].name = NULL;
		act->animations[j].yOffset = 0;
	}

	return act;
}

Actor* NewActorByName(const char *name)
{
	Actor* act = NULL;

	act = malloc(sizeof(Actor));
	memset(act, 0, sizeof(Actor));
	act->animations = NULL;
	slog("using new actor");
	act->_inUse = 1;
	act->animState = 0;
	act->name = name;
	act->currentSprite = NULL;
	act->color = vector4d(0, 0, 0, 0);
	act->numAnimations = 0;

	return act;
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

Actor* LoadActor(const char *filename)
{
	FILE * file;
	char buf[GF2DTEXTLEN];
	char tempBuf[GF2DTEXTLEN];
	char c;
	int i = 0, n = 0, bufLen = 0;
	Actor *actor = NULL;
	int numAnim = 0;
	char *fileContents = NULL;
	Uint32 fileSize = 0;

	
	file = fopen(filename, "r");

	if (!file)
	{
		slog("Failed to open file %", filename);

		return NULL;
	}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         

	fseek(file, 0L, SEEK_END);
	fileSize = ftell(file);
	rewind(file);
	
	fclose(file);

	fileContents = malloc((sizeof(char)* fileSize) + 1);
	memset(fileContents, 0, (sizeof(char)* fileSize) + 1);

	file = fopen(filename, "r");

	c = fgetc(file);

	while (c != '~')
	{
		fileContents[i++] = c;
		c = fgetc(file);
	}

	fclose(file);

	fileContents[i] = '\0';
	
	while (sscanf(fileContents, " %s\n%n", buf, &n) == 1)
	{
		if (buf[0] == '~')
		{
			break;
		}

		fileContents += n;
		bufLen += n;

		if (strcmp(buf, "numAnimations:") == 0)
		{
			sscanf(fileContents, " %s\n%n", &tempBuf, &n);
			
			numAnim = atoi(tempBuf);

			if (numAnim)
			{
				break;
			}
		}
	} 

	actor = NewActor(numAnim);

	while (sscanf(fileContents, " %s\n%n", buf, &n) == 1)
	{
		if (buf[0] == '~')
		{
			break;
		}

		fileContents += n;
		bufLen += n;

		if (strcmp(buf, "actorName:") == 0)
		{
			sscanf(fileContents, " %s\n%n", tempBuf, &n);

			fileContents += n;
			bufLen += n;

			strncpy(actor->name, tempBuf, n);

			continue;
		}
		if (strcmp(buf, "color:") == 0)
		{
			sscanf(fileContents, " %lf,%lf,%lf,%lf\n%n", &actor->color.x, &actor->color.y, &actor->color.z, &actor->color.w, &n);

			fileContents += n;
			bufLen += n;

			continue;
		}
		if (strcmp(buf, "scale:") == 0)
		{
			sscanf(fileContents, " %lf,%lf", &actor->scale.x, &actor->scale.y, &n);

			fileContents += n;
			bufLen += n;

			break;

		}
	}

	for (i = 0; i < numAnim; ++i)
	{
		while (sscanf(fileContents, " %s\n%n", buf, &n) == 1)
		{
			if (buf[0] == '~')
			{
				break;
			}

			fileContents += n;
			bufLen += n;

			if (strcmp(buf, "Animation:") == 0)
			{
				sscanf(fileContents, " %s\n%n", tempBuf, &n);

				actor->animations[i] = *ParseAnimation(tempBuf);
				
				fileContents += n;
				bufLen += n;
				break;
			}
		}
	}

	for (i = 0; i < actor->numAnimations; ++i)
	{
		(actor->animations + i)->sprite = LoadImageToTexture(actor->animations[i].filepath, GetRenderer());
	}

	slog("Loaded actor %s", actor->name);

	fileContents -= bufLen;

	free(fileContents);

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
		DeleteAnimation(&actor->animations[i]);
	}

	SpriteDelete(actor->currentSprite);
	DeleteAnimation(actor->currentAnimation);
	DeleteAnimation(actor->animations);
	free(actor->name);
	free(actor);
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

Animation* ParseAnimation(const char* filename)
{
	FILE * file = NULL;
	char buf[GF2DTEXTLEN];
	char *tempBuf[GF2DTEXTLEN];
	char *tempData[GF2DTEXTLEN];

	int i = 0, n = 0, bufLen = 0;
	Uint32 iteFlag = 0;
	Animation *anim = NULL;
	char c;
	char *fC = NULL;
	Uint32 fileSize = 0;

	anim = NewAnimation();

	file = fopen(filename, "r");

	if (!file)
	{
		slog("Failed to open file %", filename);

		return NULL;
	}

	fseek(file, 0L, SEEK_END);
	fileSize = ftell(file);
	rewind(file);

	fclose(file);

	fC = malloc((sizeof(char)* fileSize) + 1);
	memset(fC, 0, (sizeof(char)* fileSize) + 1);

	file = fopen(filename, "r");

	c = fgetc(file);

	while (c != '~')
	{
		fC[i++] = c;
		c = fgetc(file);
	}

	fclose(file);

	fC[i] = '\0';



	while (sscanf(fC, " %s\n%n", buf, &n) == 1)
	{
		if (buf[0] == "~")
		{
			break;
		}
		
		fC += n;
		bufLen += n;

		if (strcmp(buf, "name:") == 0)
		{
			sscanf(fC, " %s\n%n", tempBuf, &n);

			strncpy(anim->name, tempBuf, n);

			fC += n;
			bufLen += n;

			continue;
		}
		else if (strcmp(buf, "filename:") == 0)
		{
			sscanf(fC, " %s\n%n", tempBuf, &n);

			strncpy(anim->filepath, tempBuf, n);

			fC += n;
			bufLen += n;

			continue;
		}
		else if (strcmp(buf, "colorSpecial:") == 0)
		{
			sscanf(fC, " %lf,%lf,%lf,%lf\n%n", &anim->colorSpecial.x, &anim->colorSpecial.y, &anim->colorSpecial.z, &anim->colorSpecial.w, &n);

			fC += n;
			bufLen += n;

			continue;
		}
		else if (strcmp(buf, "yOffset:") == 0)
		{
			sscanf(fC, " %i\n%n", &anim->yOffset, &n);

			fC += n;
			bufLen += n;

			continue;
		}
		else if (strcmp(buf, "length:") == 0)
		{
			sscanf(fC, " %i\n%n", &anim->length, &n);

			fC += n;
			bufLen += n;

			continue;
		}
		else if (strcmp(buf, "frameWidth:") == 0)
		{
			sscanf(fC, " %i\n%n", &anim->cellWidth, &n);

			fC += n;
			bufLen += n;

			continue;
		}
		else if (strcmp(buf, "frameHeight:") == 0)
		{
			sscanf(fC, " %i\n%n", &anim->cellHeight, &n);

			fC += n;
			bufLen += n;

			continue;
		}
		else if (strcmp(buf, "frameRate:") == 0)
		{
			sscanf(fC, " %f\n%n", &anim->frameRate, &n);

			fC += n;
			bufLen += n;

			continue;
		}
		else if (strcmp(buf, "type:") == 0)
		{
			sscanf(fC, " %s\n%n", tempBuf, &n);

			if (strcmp(tempBuf, "loop") == 0)
			{
				anim->animType = AT_LOOP;
			}
			else if (strcmp(tempBuf, "once") == 0)
			{
				anim->animType = AT_ONCE;
			}

			if (anim->animType)
			{
				
				fC += n;
				bufLen += n;
				break;
			}
		}
	}

	fC -= bufLen;

	free(fC);

	return anim;
}
