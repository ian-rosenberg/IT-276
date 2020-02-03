#ifndef __CAMERA__
#define __CAMERA__

#include "gf2d_shape.h"
#include "gf2d_vector.h"
#include "entity.h"

typedef struct Camera_S
{
	Vector2D	position,
				velocity,
				viewport,
				levelBounds;

	Rect		trackingBounds;

	float		distToPlayer;

}Camera;

void CameraInit(Vector2D pPos, Vector2D viewport, Vector2D tilemapBounds);

Vector2D GetCameraPosition();

Vector2D CameraMove(Entity *pEnt);

#endif