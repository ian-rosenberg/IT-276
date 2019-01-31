#ifndef __GRAPHICS__
#define __GRAPHICS__

#include "gf2d_types.h"

#include <SDL.h>

/**
* @brief Initialize SDL window and surface
* @param winWidth The width of the window in pixels
* @param winHeight The height of the window in pixels
* @param flags
*/
void Graphics_Init(Uint32 winWidth, Uint32 winHeight, Bool fullscreen);

/**
* @brief sets the amount of delay to aim for between frames.
* @param fDelay the amount of time, in milliseconds, that each frame should take
*/
void SetFrameDelay(Uint32 fDelay);

/**
* @brief Load an image file
* @param path of the file to load
*/
SDL_Texture* LoadImageMedia(const char* filepath);

/**
* @brief Exit SDL and close all subsystems
*/
void Graphics_Close();

#endif