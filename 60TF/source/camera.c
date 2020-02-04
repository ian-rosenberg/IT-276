#include "camera.h"
#include "graphics.h"
#include "player.h"

#define BOUNDS_PERCENT 0.30
#define DAMPING 6
#define STIFFNESS 5
#define MASS 1
#define MAX_FOLLOW_DISTANCE 100

static Camera cam = { 0 };

void CameraInit(Vector2D pPos, Vector2D viewport, Vector2D tilemapBounds)
{
	cam.position = vector2d(0,0);

	cam.viewport = viewport;
	
	cam.levelBounds = tilemapBounds;

	cam.trackingBounds = gf2d_rect(viewport.x * BOUNDS_PERCENT,
		viewport.y * BOUNDS_PERCENT,
		(viewport.x * (1 - (2 * BOUNDS_PERCENT))),
		(viewport.y * (1 - (2 * BOUNDS_PERCENT))));
}

Vector2D GetCameraPosition()
{
	return cam.position;
}

Vector2D CameraMove(Vector2D entPos)
{
	cam.position.x = entPos.x - cam.viewport.x / 2;
	cam.position.y = entPos.y - cam.viewport.y / 2;

	//cam.distToPlayer = vector2d_magnitude_between(entPos, cam.position);

	if (cam.position.x < 0)
	{
		cam.position.x = 0;
	}
	if (cam.position.x > cam.levelBounds.x - cam.viewport.x)
	{
		cam.position.x = cam.levelBounds.x - cam.viewport.x;
	}
	if (cam.position.y < 0)
	{
		cam.position.y = 0;
	}
	if (cam.position.y > cam.levelBounds.x - cam.viewport.y)
	{
		cam.position.y = cam.levelBounds.x - cam.viewport.y;
	}

	return cam.position;
}