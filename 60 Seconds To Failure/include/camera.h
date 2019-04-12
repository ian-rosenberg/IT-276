#ifndef __CAMERA__
#define __CAMERA__

#include <SDL.h>
#include "gf2d_shape.h"
#include "gf2d_vector.h"

/**
* @brief Initialize the camera's dimensions
* @param dimensions A rect with the x,y coords
* and the width + height of the camera
*/
void SetCameraDimensions(SDL_Rect dimensions);

/**
* @brief Set the camera position, respecting bounds
* @param pos The position to move to
*/
void SetCameraPosition(Vector2D pos);

/**
* @brief Setup the boundsd for the camera
* bounds The rectangle with the bounds info
*/
void SetCameraBounds(SDL_Rect bounds);

/**
* @brief Get the current camera position
* @returns 2D coordinates of the camera
*/
Vector2D GetCameraPosition();

/**
* @brief Get the current camera offset
* @returns 2D coordinates of the camera
*/
Vector2D GetCameraOffset();

/**
* @brief Get camera's dimensions
* @returns Rect containing x,y pos and width + height
*/
Rect GetCameraDimensions();

#endif