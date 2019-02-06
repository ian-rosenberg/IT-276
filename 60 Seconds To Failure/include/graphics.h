#ifndef __GRAPHICS__
#define __GRAPHICS__

#include "gf2d_types.h"

#include <SDL.h>

/**
* @brief Initialize SDL window and surface
* @param winWidth The width of the window in pixels
* @param winHeight The height of the window in pixels
* @param fullscreen Trigger fullscreen mode or not
* @param fDelay the delay, in milliseconds, between frames.
*/
void GraphicsInit(Uint32 winWidth, Uint32 winHeight, Bool fullscreen, Uint32 fDelay);

/**
* @brief Exit SDL and close all subsystems
*/
void GraphicsClose();

/**
* @brief Clear the rendered screen
*/
void ClearScreen();

/**
* @brief Get the current renderer
* @returns The renderer
*/
SDL_Renderer* GetRenderer();

/**
* @brief Render the next frame and calculate the frame timing
* @param tex The SDL texture to copy to the renderer
* @param r The SDL renderer to copy to 
* @param pos The worldspace coordinate to draw at
*/
void NextFrame();

/**
* @brief Get the frame rate of the current renderer
* @returns A floating point representing the number
* of frames per second
*/
float GetFrameRate();

/**
* @brief Delay drawing to keep everything at approx 60fps
*/
void FrameDelay();

/**
* @ brief Get the frame delay value
* @ returns The current frame delay
*/
Uint32 GetFrameDelay();


#endif