#include "physicsbodies.h"
#include "simple_logger.h"
#include <string.h>

typedef struct
{
	Uint16		maxBodies;
	Uint16		numBodies;

	BodyInfo	*bodyList;
}BodyManager;

//local global
static BodyManager bodyManager = { 0 };

void BodyManagerClose(void)
{
	ClearAllBodies();

	if (bodyManager.bodyList != NULL)
	{
		free(bodyManager.bodyList);
	}
	bodyManager.bodyList = NULL;
	bodyManager.maxBodies = 0;

	slog("body list system closed");
}

void BodyManagerInit(Uint16 max)
{
	if (!max)
	{
		slog("cannot init body manager for zero bodies");
		return;
	}

	bodyManager.maxBodies = max;

	bodyManager.bodyList = (BodyInfo*)malloc(sizeof(BodyInfo)*max);

	memset(bodyManager.bodyList, 0, sizeof(BodyInfo)* max);

	atexit(BodyManagerClose);
}

void ClearAllBodies()
{
	int i;

	for (i = 0; i < bodyManager.maxBodies; ++i)
	{
		DeleteBodyInfo(&bodyManager.bodyList[i]);
	}
}

void DeleteBodyInfo(BodyInfo *body)
{
	int i;

	if (!body)
	{
		return;
	}

	cpBodyDestroy(body->body);
	cpBodyFree(body->body);

	if (!body->shape)
	{
		return;
	}

	cpShapeFree(body->shape);
	cpShapeFree(body->shape);
}

BodyInfo* NewBodyInfo(Vector3D dimAndRadius, cpSpace *world, Bool jumpPhysics)
{
	int i;
	BodyInfo* bodyInfo = NULL;

	for (i = 0; i < bodyManager.maxBodies; i++)
	{
		if (bodyManager.bodyList[i]._inUse != 1)
		{
			memset(&bodyManager.bodyList[i], 0, sizeof(BodyInfo));

			bodyInfo = &bodyManager.bodyList[i];

			slog("Creating a new physics body");

			bodyManager.bodyList[i].body = cpBodyNew(0.0f, 0.0f);

			bodyManager.bodyList[i].shape = cpBoxShapeNew(bodyManager.bodyList[i].body, dimAndRadius.x, dimAndRadius.y, dimAndRadius.z);

			bodyManager.bodyList[i]._inUse = 1;

			return bodyInfo;
		}
	}
	slog("error: out of body manager's addresses");
	return NULL;
}