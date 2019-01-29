#include <SDL.h>
#include <stdio.h>

#include "simple_logger.h"
#include "graphics.h"

int main(int agrc, char* arg[])
{
	init_logger("60TF.log");
	slog("---==== BEGIN ====---");
	
	Graphics_Init();

	slog("---==== END ====---");

	return 0;
}