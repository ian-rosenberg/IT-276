#ifndef __2D_KD_TREE__
#define __2D_KD_TREE__

#include <SDL.h>
#include "entity.h"


typedef struct kdNode_S
{
	Vector2D point;
	float closestDistance;

	kdNode *childLeft;
	kdNode *childRight;
}kdNode;

/**
* @brief Insert a node for a two dimensional KD tree
* @param newPoint the point for circle origin to add
* @param node, for recursive purposes, traversing down the
* tree while keeping track of the node to add
* @param closest Distance to the closest point
*/
Bool kd_Insert_Node(Vector2D newPoint, kdNode *node, float closest);

#endif