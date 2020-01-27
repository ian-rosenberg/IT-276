#include "camera.h"
#include "graphics.h"
#include "player.h"

#define TOLERANCE 32
#define BOUNDS_PERCENT 0.30

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

Vector2D CameraMove(Entity *pEnt)
{
	Vector2D pDim = GetAverageActorDimensions(pEnt->actor);

	cam.position.x = (pEnt->position.x + pDim.x / 2) - cam.viewport.x / 2;
	cam.position.y = (pEnt->position.y + pDim.y / 2) - cam.viewport.y / 2;

	cam.distToPlayer = vector2d_magnitude_between(pEnt->position, cam.position);

	if (cam.distToPlayer > vector2d_magnitude(cam.viewport))
	{
		cam.velocity = pEnt->velocity;
		
		vector2d_scale(cam.velocity, cam.velocity, GetFrameDelay() * TOLERANCE);
	}

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

	if (cam.velocity.x != 0 || cam.velocity.y != 0)
	{
		vector2d_add(cam.position, cam.position, cam.velocity);
	}

	return cam.position;
}

Vector2D GetCameraOffset(Vector2D pPos)
{
	Vector2D result;
	
	vector2d_sub(result, pPos, cam.position);

	return result;
}