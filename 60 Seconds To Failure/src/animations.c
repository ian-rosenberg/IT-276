#include <stdio.h>

#include "animations.h"
#include "simple_logger.h"
#include "graphics.h"

typedef struct
{
	Uint32 maxAnimations;

	Animation *animations;
}AnimationManager;

//local global
static AnimationManager animationManager = { 0 };

void ClearAllAnimations();

void AnimationManagerClose()
{
	ClearAllAnimations();

	if (animationManager.animations != NULL)
	{
		free(animationManager.animations);
	}
	animationManager.animations = NULL;
	animationManager.maxAnimations = 0;

	slog("animation list system closed");
}

void AnimationManagerInit(Uint32 max)
{
	if (!max)
	{
		slog("cannot init animation manager for zero animations");
		return;
	}

	animationManager.maxAnimations = max;

	animationManager.animations = (Animation*)malloc(sizeof(Animation)*max);

	memset(animationManager.animations, 0, sizeof(Animation)* max);

	atexit(AnimationManagerClose);
}

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

void FreeAnimation(Animation *animList)
{
	if (!animList)
	{
		return;
	}

	animList->refCount--;
}

void ClearAllAnimations()
{
	int i;

	for (i = 0; i < animationManager.maxAnimations; ++i)
	{
		DeleteAnimation(&animationManager.animations[i]);
	}
}

Animation* NewAnimation()
{
	int i;

	for (i = 0; i < animationManager.maxAnimations; i++)
	{
		if ((animationManager.animations[i].refCount == 0) && (animationManager.animations[i].sprite == NULL))
		{
			animationManager.animations[i].refCount = 1;
			return &animationManager.animations[i];
		}
	}

	/*find an unused sprite address and clean up the old data*/
	for (i = 0; i < animationManager.maxAnimations; i++)
	{
		if (animationManager.animations[i].refCount == 0)
		{
			DeleteAnimation(&animationManager.animations[i]);
			animationManager.animations[i].refCount = 1;
			return &animationManager.animations[i];
		}
	}
	slog("error: out of animation manager's addresses");
	return NULL;
}

Animation* GetAnimationByFilename(char* filename)
{
	int i;

	for (i = 0; i < animationManager.maxAnimations; ++i)
	{
		if (gf2d_line_cmp(animationManager.animations[i].filepath, filename) == 0)
		{
			return &animationManager.animations[i];
		}
	}

	return NULL;
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

Animation* GetAnimationByName(char *name)
{
	int i;
	if (!animationManager.animations)
	{
		slog("no action list provided");
		return NULL;
	}
	if (!name)
	{
		slog("no filename provided");
		return NULL;
	}
	for (i = 0; i < animationManager.maxAnimations; i++)
	{
		if (gf2d_line_cmp(animationManager.animations[i].name, name) == 0)
		{
			return &animationManager.animations[i];
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