#include "entity.h"
#include "simple_logger.h"


typedef struct
{
	Uint32		maxEntities;
	Uint64		numEntities;

	Entity		*entityList;
}EntityManager;

//local global 
static EntityManager entityManager = { 0 };

void EntityManagerInit(Uint32 maxEnt)
{
	if (!maxEnt)
	{
		slog("Cant allocate 0 entities");
	}
}

void EntityManagerClose()
{

}