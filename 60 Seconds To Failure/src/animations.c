#include <stdio.h>

#include "animations.h"
#include "simple_logger.h"
#include "graphics.h"


void DeleteAnimation(Animation *animList)
{
	if (!animList)
	{
		return;
	}

	if (animList->sprite != NULL)
	{
		SpriteDelete(animList->sprite);
	}

	memset(animList, 0, sizeof(Animation));//clean up all other data
}

Animation* NewAnimation(Uint32 numAnim)
{
	Animation *anim = NULL;

	if (!numAnim)
	{
		slog("Cannot create 0 animations");
		return NULL;
	}

	anim = (Animation*)malloc(sizeof(Animation) * numAnim);
	memset(anim, 0, sizeof(Animation) * numAnim);
	
	anim->animType = 0;
	anim->cellHeight = 0;
	anim->cellWidth = 0;
	anim->colorSpecial = vector4d(0, 0, 0, 0);
	anim->currentFrame = 0.0f;
	anim->filepath = NULL;
	anim->filepath = malloc(sizeof(char) * GF2DTEXTLEN); 
	memset(anim->filepath, 0, sizeof(char) * GF2DTEXTLEN);
	anim->frameRate = 0.0f;
	anim->length = 0;
	anim->name = NULL;
	anim->name = malloc(sizeof(char) * GF2DTEXTLEN);
	memset(anim->name, 0, sizeof(char) * GF2DTEXTLEN);

	return anim;
}

int GetAnimationCount(FILE *file)
{
	char buf[512];
	int count = 0;
	if (!file)return 0;
	rewind(file);
	while (fscanf(file, "%s", buf) != EOF)
	{
		if (strcmp(buf, "animation:") == 0)
		{
			count++;
		}
		fgets(buf, sizeof(buf), file);
	}
	return count;
}

Animation* GetAnimationByName(Animation *list, Uint32 numAnim, char *name)
{
	int i;
	if (!list)
	{
		slog("no aniamtion list provided");
		return NULL;
	}
	if (numAnim < 1)
	{
		slog("no number of animations provided");
		return NULL;
	}
	if (!name)
	{
		slog("no filename provided");
		return NULL;
	}
	for (i = 0; i < numAnim; i++)
	{
		if (strcmp(list[i].name, name) == 0)
		{
			return &list[i];
		}
	}
	return NULL;// not found
}

AnimationReturnType AnimationNextFrame(Animation *anim, float *frame)
{
	if (!frame)
	{
		slog("missing frame data");
		return ART_ERROR;
	}
	if (!anim)
	{
		slog("no animation found by name %s", anim->name);
		return ART_ERROR;
	}
	*frame += anim->frameRate;
	if ((int)(*frame) >= anim->length - 1)
	{
		switch (anim->animType)
		{
		case AT_LOOP:
			*frame = 0;
			return ART_LOOPING;
		case AT_ONCE:
			*frame = 0;//fix this loop
			return ART_END;
		}
	}
	return ART_INPROGRESS;
}