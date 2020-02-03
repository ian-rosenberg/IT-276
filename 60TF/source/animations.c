#include <stdio.h>

#include "animations.h"
#include "simple_logger.h"
#include "graphics.h"


typedef struct
{
	Uint32 maxAnim;
	Animation *animList;
}AnimationManager;

static AnimationManager animationManager = { 0 };

void AnimationManagerInit(Uint32 max)
{
	int i = 0;

	if (max < 1)
	{
		slog("Cant allocate 0 animations");
		return;
	}

	animationManager.maxAnim = max;
	animationManager.animList = malloc(max * sizeof(Animation));

	memset(animationManager.animList, 0, sizeof(Animation)*max);

	for (; i < animationManager.maxAnim; i++)
	{
		animationManager.animList[i].name = malloc(sizeof(char)* GF2DWORDLEN);
		animationManager.animList[i].filepath = malloc(sizeof(char)* GF2DLINELEN);
		animationManager.animList[i].sprite = NewSprite();
		animationManager.animList[i]._refCount = 0;
		animationManager.animList[i]._inUse = 0;
		animationManager.animList[i].length = 0;
		animationManager.animList[i].currentFrame = 0;
		animationManager.animList[i].cellWidth = 0;
		animationManager.animList[i].cellHeight = 0;
		animationManager.animList[i].yOffset = 0;
		animationManager.animList[i].colorSpecial = vector4d(0, 0, 0, 0);
		animationManager.animList[i].frameRate = 0;
		animationManager.animList[i].animType = 0;
	}

	slog("animation system initialized");
}

void DeleteAnimation(Animation *animList)
{
	int i = 0;
	Animation *anim = NULL;

	for (; i < animationManager.maxAnim; ++i)
	{
		if (&animationManager.animList[i] == animList)
		{
			anim = &animationManager.animList[i];
		}
	}
	
	if (anim == NULL)
	{
		return;
	}

	if (anim->sprite != NULL)
	{
		SpriteDelete(anim->sprite);
	}

	if (animList->filepath != NULL)
	{
		memset(anim->filepath, 0, sizeof(char)* GF2DLINELEN);
	}
	if (animList->name != NULL)
	{
		memset(anim->name, 0, sizeof(char) * GF2DWORDLEN);
	}
}

Animation* NewAnimation()
{
	int i = 0;
	Animation *anim;

	for (; i < animationManager.maxAnim; ++i)
	{
		if ((animationManager.animList[i]._inUse == 0) ||
			animationManager.animList[i]._inUse == 0 && &animationManager.animList[i].sprite->_refCount == 0)
		{
			anim = &animationManager.animList[i];
		}
	}

	if (!anim)
	{
		slog("Failed to allocate a new animation");

		return;
	}

	anim->sprite = NewSprite();
	memset(anim->sprite, 0, sizeof(Sprite));

	anim->filepath = malloc(sizeof(char) * GF2DLINELEN);
	memset(anim->filepath, 0, sizeof(char) * GF2DLINELEN);

	anim->name = malloc(sizeof(char)* GF2DWORDLEN);
	memset(anim->filepath, 0, sizeof(char)* GF2DWORDLEN);

	anim->animType = 0;
	anim->cellHeight = 0;
	anim->cellWidth = 0;
	anim->colorSpecial = vector4d(0, 0, 0, 0);
	anim->currentFrame = 0.0f;
	anim->frameRate = 0.0f;
	anim->length = 0;
	anim->_refCount++;
	anim->_inUse = 1;


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

void ClearAllAnimations()
{
	int i = 0; 

	for (; i < animationManager.maxAnim; ++i)
	{
		if (animationManager.animList[i].name != NULL)
		{
			free(animationManager.animList[i].name);
		}
		if (animationManager.animList[i].filepath != NULL)
		{
			free(animationManager.animList[i].filepath);
		}
		if (animationManager.animList[i].sprite != NULL)
		{
			SpriteDelete(animationManager.animList[i].sprite);
		}
		
		free(&animationManager.animList[i]);
	}
}

void AnimationManagerClose()
{
	ClearAllAnimations();

	free(animationManager.animList);
}