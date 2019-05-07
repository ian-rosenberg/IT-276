#ifndef __ANIMATIONS__
#define __ANIMATIONS__

#include "sprites.h"
#include "gf2d_text.h"
#include "gf2d_vector.h"

typedef enum
{
	State_Idle,
	State_Walking,
	State_Attacking,
	State_Hurt,
	State_Death,
	State_Dead
}State;

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
	char			*name;
	char			*filepath;

	Sprite			*sprite;

	Uint32			length;
	float			currentFrame;
	Uint32			cellWidth, cellHeight, yOffset;

	Vector4D		colorSpecial;

	float			frameRate;

	AnimationType	animType;//Following DJ's style, set the type at rutime
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
Animation* NewAnimation(Uint32 numAnim);

/**
* @brief Proceed to the next frame
* @param animList The animation list to pull an animation from
* @param frame For timing purposes
* @returns A flag determining the status of the animation
*/
AnimationReturnType AnimationNextFrame(
	Animation *animList,
	float *frame);

/**
* @brief Retrieve an animation by its name
* @param name The animation to get
* @returns The desired animation
*/
Animation* GetAnimationByName(Animation *list, Uint32 numAnim, char *name);
#endif