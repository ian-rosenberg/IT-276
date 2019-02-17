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
	Uint32			refCount;

	TextLine		name;
	TextLine		filepath;

	Sprite			*sprite;

	Uint32			length;
	float			currentFrame;
	Uint32			cellWidth, cellHeight, yOffset;

	Vector2D		scale;

	Vector4D		colorSpecial;

	float			frameRate;

	AnimationType	animType;//Following DJ's style, set the type at rutime
}Animation;

/**
* @brief Intialize the animation list on a per actor basis
* @param max The max number of animations to be simultaneously loaded
*/
void AnimationManagerInit(Uint32 max);

/**
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
* @brief Get an animatiob by its filename
* @param filename The file to check against
* @returns The animation in question
*/
Animation* GetAnimationByFilename(char* filename);

/**
* @brief Load the indiviudal sprites that make up the individual animations
* @param file The filepath of the config file for the corresponding actor
* @returns A list of animations
*/
Animation* LoadAnimation(char* file);

/**
* @brief Free a list of of animations' sprites
* @param animList The list to free
*/
void FreeAnimation(Animation *animList);

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
Animation* GetAnimationByName(char *name);
#endif