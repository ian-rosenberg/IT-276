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

	free(animList->filepath);
	free(animList->name);
	free(animList);
}

Animation* NewAnimation()
{
	Animation *anim = NULL;

	anim = malloc(sizeof(Animation));
	memset(anim, 0, sizeof(Animation));

	if (!anim)
	{
		slog("Failed to allocate a new animation");

		return;
	}

	anim->animType = 0;
	anim->cellHeight = 0;
	anim->cellWidth = 0;
	anim->colorSpecial = vector4d(0, 0, 0, 0);
	anim->currentFrame = 0.0f;
	anim->filepath = malloc(sizeof(char) * GF2DLINELEN);
	anim->frameRate = 0.0f;
	anim->length = 0;
	anim->name = malloc(sizeof(char)* GF2DWORDLEN);;
	anim->sprite = NewSprite();

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
		slog("no animation found");
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
			*frame = 0;
			return ART_END;
		}
	}
	return ART_INPROGRESS;
}