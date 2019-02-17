#ifndef __GUI__
#define __GUI__

/**
* @brief Intialize the gui/HUD
*/
void GUISetupHUD();

/**
* @brief Draw the gui/HUD
*/
void GUIDrawHUD();

/**
* @brief Set the health value
* @param hp The value in percentage to set for health
*/
void GUISetHealth(float hp);

/**
* @brief toggle the FPS display
*/
void ToggleShowFPS();

#endif