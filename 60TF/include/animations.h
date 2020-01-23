#ifndef __ANIMATIONS__
#define __ANIMATIONS__

#include <stdio.h>

#include "sprites.h"
#include "gf2d_text.h"
#include "gf2d_vector.h"

typedef enum
{
	AT_ONCE,
	AT_LOOP,
}AnimationType;

typedef enum
{
	ART_ERROR,
	ART_LOOPING,
	ART_INPROGRESS,
	ART_END
}AnimationReturnType;

/**
* @brief Animation data
*/
typedef struct Animation_S
{
	const char		*name;
	const char		*filepath;

	Sprite			*sprite;

	Uint32			length;
	float			currentFrame;
	Uint32			cellWidth, cellHeight, yOffset;

	Vector4D		colorSpecial;

	float			frameRate;

	AnimationType	animType;
}Animation;

/*
* @brief Delete an animation
* @param animList The animation to delete
*/
void DeleteAnimation(Animation *animList);

/**
* @brief Get a new animation list
* @returns an unused/unreferenced animation list
*/
Animation* NewAnimation();

/**
* @brief Retrieve an animation by its name
* @param name The animation to get
* @returns The desired animation
*/
Animation* GetAnimationByName(Animation *list, Uint32 numAnim, char *name);

/*
* @brief Count the animations in a given config file
* @param file The file to process
* @returns An int describing the number of animations in an actor's config
*/
int GetAnimationCount(FILE *file);

/**
* @brief Proceed to the next frame
* @param animList The animation list to pull an animation from
* @param frame For timing purposes
* @returns A flag determining the status of the animation
*/
AnimationReturnType AnimationNextFrame(
	Animation *anim,
	float *frame);

#endif