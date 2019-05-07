#include "actor.h"
#include "simple_logger.h"
#include "graphics.h"

#include <string.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>

#define ANIMATION_DATA_KEY_NUM 9

const int COLOR_VAL_LENGTH = 4;


Actor* NewActor(Uint32 numAnim)
{
	int j;
	Actor* act = NULL;

	act = malloc(sizeof(Actor));
	memset(act, 0, sizeof(Actor));
	act->animations = NewAnimation(numAnim);
	slog("using new actor");
	act->_inUse = 1;
	act->animState = 0;
	act->name = NULL;
	act->currentSprite = NULL;
	act->color = vector4d(0, 0, 0, 0);
	act->numAnimations = numAnim;
	act->scale = vector2d(0, 0);

	act->name = malloc(sizeof(char) * GF2DTEXTLEN);
	memset(act->name, 0, sizeof(char) * GF2DTEXTLEN);

	for (j = 0; j < numAnim; ++j)
	{
		act->animations[j].animType = 0;
		act->animations[j].cellHeight = 0;
		act->animations[j].cellWidth = 0;
		act->animations[j].colorSpecial = vector4d(0, 0, 0, 0);
		act->animations[j].currentFrame = 0;
		act->animations[j].filepath = malloc(sizeof(char) * GF2DTEXTLEN);
		memset(act->animations[j].filepath, 0, sizeof(char) * GF2DTEXTLEN);
		act->animations[j].frameRate = 0;
		act->animations[j].name = malloc(sizeof(char) * GF2DTEXTLEN);
		memset(act->animations[j].name, 0, sizeof(char) * GF2DTEXTLEN);
		act->animations[j].yOffset = 0;
	}

	return act;
}

Actor* NewActorByName(char *name)
{
	Actor* act = NULL;

	act = malloc(sizeof(Actor));
	memset(act, 0, sizeof(Actor));
	act->animations = NULL;
	slog("using new actor");
	act->_inUse = 1;
	act->animState = 0;
	act->name = malloc(sizeof(char) * GF2DTEXTLEN);
	memset(act->name, 0, sizeof(char) * GF2DTEXTLEN);
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

Actor* LoadActor(char *filename)
{
	FILE * file;
	char buf[GF2DTEXTLEN];
	char tempBuf[GF2DTEXTLEN];
	char c;
	int i = 0, n = 0;
	Actor *actor = NULL;
	int numAnim = 0;
	char **filenames = NULL;
	char *fileContents = "";
	size_t fileSize = 0;

	
	file = fopen(filename, "r");

	if (!file)
	{
		slog("Failed to open file %", filename);

		return NULL;
	}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         

	fseek(file, 0L, SEEK_END);
	fileSize = ftell(file);
	rewind(file);
	fileContents = malloc(sizeof(char) * fileSize);
	memset(fileContents, 0, sizeof(char) * fileSize);

	c = fgetc(file);

	while (c != EOF)
	{
		fileContents[i++] = c;
		c = fgetc(file);
	}

	//rewind(file);

	fclose(file);

	fileContents[i] = '\0';
	
	while (sscanf(fileContents, " %s\n%n", buf, &n) == 1)
	{
		if (buf[0] == '~')
		{
			break;
		}

		fileContents += n;

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

		if (strcmp(buf, "actorName:") == 0)
		{
			sscanf(fileContents, " %s\n%n", tempBuf, &n);

			fileContents += n;

			strncpy(actor->name, tempBuf, GF2DTEXTLEN);

			continue;
		}
		if (strcmp(buf, "color:") == 0)
		{
			sscanf(fileContents, " %lf,%lf,%lf,%lf\n%n", &actor->color.x, &actor->color.y, &actor->color.z, &actor->color.w, &n);

			fileContents += n;

			continue;
		}
		if (strcmp(buf, "scale:") == 0)
		{
			sscanf(fileContents, " %lf,%lf", &actor->scale.x, &actor->scale.y, &n);

			if (actor->scale.x >= 0 && actor->scale.x <= 255)
			{

				fileContents += n;

				break;
			}

		}
	}

	filenames = malloc(numAnim * sizeof(char));
	memset(filenames, 0, sizeof(char) * numAnim);

	for (i = 0; i < numAnim; ++i)
	{
		filenames[i] = malloc(GF2DTEXTLEN * sizeof(char));
		memset(filenames[i], 0, GF2DTEXTLEN * sizeof(char));
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

			if (strcmp(buf, "Animation:") == 0)
			{
				sscanf(fileContents, " %s\n%n", tempBuf, &n);

				strncpy(filenames[i], tempBuf, sizeof(char) * GF2DTEXTLEN);

				fileContents += n;

				break;
			}
		}
	}

	for (i = 0; i < numAnim; ++i)
	{
		*(actor->animations + i) = *ParseAnimation(filenames[i]);
	}

	for (i = 0; i < actor->numAnimations; ++i)
	{
		(actor->animations + i)->sprite = LoadImageToTexture(actor->animations[i].filepath, GetRenderer());
	}

	slog("Loaded actor %s", actor->name);

	for (i = 0; i < numAnim; ++i)
	{
		if (filenames[i])
		{
			memset(filenames[i], 0, sizeof(char) * GF2DTEXTLEN);
			free(filenames[i]);
		}
	}

	if (filenames)
	{
		memset(filenames, 0, numAnim * sizeof(char));
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

	if (!actor->animations)
	{
		slog("No animations to delete for empty actor");
		return;
	}
	
	for (i = 0; i < actor->numAnimations; ++i)
	{
		DeleteAnimation(&actor->animations[i]);
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

Animation* ParseAnimation(char* filename)
{
	FILE * file = NULL;
	char *buf = NULL;
	char *tempBuf = NULL;
	char *tempData = NULL;
	int i = 0, n = 0;
	Uint32 iteFlag = 0;
	Animation *anim = NULL; 
	char c;
	char *fileContents = "";
	size_t fileSize = 0;

	anim = NewAnimation(1);

	file = fopen(filename, "r");

	if (!file)
	{
		slog("Failed to open file %", filename);

		return NULL;
	}

	fseek(file, 0L, SEEK_END);
	fileSize = ftell(file);
	rewind(file);
	fileContents = malloc(sizeof(char) * fileSize);
	memset(fileContents, 0, sizeof(char) * fileSize);

	c = fgetc(file);

	while (c != EOF)
	{
		fileContents[i++] = c;
		c = fgetc(file);
	}
	fileContents[i] = '\0';

	fclose(file);

	buf = malloc(GF2DTEXTLEN * sizeof(char));
	memset(buf, 0, GF2DTEXTLEN * sizeof(char));

	tempBuf = malloc(GF2DTEXTLEN * sizeof(char));
	memset(tempBuf, 0, GF2DTEXTLEN * sizeof(char));

	tempData = malloc(GF2DTEXTLEN * sizeof(char));
	memset(tempData, 0, GF2DTEXTLEN * sizeof(char));
	
	while (sscanf(fileContents, " %s\n%n", buf, &n) == 1)
	{
		if (buf[0] == "~")
		{
			break;
		}
		
		fileContents += n;

		if (strcmp(buf, "name:") == 0)
		{
			sscanf(fileContents, " %s\n%n", tempBuf, &n);

			strncpy(anim->name, tempBuf, GF2DTEXTLEN);

			fileContents += n;

			continue;
		}
		else if (strcmp(buf, "filename:") == 0)
		{
			sscanf(fileContents, " %s\n%n", tempBuf, &n);

			strncpy(anim->filepath, tempBuf, GF2DTEXTLEN);

			fileContents += n;

			continue;
		}
		else if (strcmp(buf, "colorSpecial:") == 0)
		{
			sscanf(fileContents, " %lf,%lf,%lf,%lf\n%n", &anim->colorSpecial.x, &anim->colorSpecial.y, &anim->colorSpecial.z, &anim->colorSpecial.w, &n);

			fileContents += n;

			continue;
		}
		else if (strcmp(buf, "yOffset:") == 0)
		{
			sscanf(fileContents, " %i\n%n", &anim->yOffset, &n);

			fileContents += n;

			continue;
		}
		else if (strcmp(buf, "length:") == 0)
		{
			sscanf(fileContents, " %i\n%n", &anim->length, &n);

			fileContents += n;

			continue;
		}
		else if (strcmp(buf, "frameWidth:") == 0)
		{
			sscanf(fileContents, " %i\n%n", &anim->cellWidth, &n);

			fileContents += n;

			continue;
		}
		else if (strcmp(buf, "frameHeight:") == 0)
		{
			sscanf(fileContents, " %i\n%n", &anim->cellHeight, &n);

			fileContents += n;

			continue;
		}
		else if (strcmp(buf, "frameRate:") == 0)
		{
			sscanf(fileContents, " %f\n%n", &anim->frameRate, &n);

			fileContents += n;

			continue;
		}
		else if (strcmp(buf, "type:") == 0)
		{
			sscanf(fileContents, " %s\n%n", tempBuf, &n);

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
				break;
			}
			slog("TempBuffer: %s", tempBuf);
		}
	}


	free(buf);
	free(tempBuf);
	free(tempData);

	return anim;
}
