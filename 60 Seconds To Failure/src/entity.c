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
		return;
	}

	memset(&entityManager, 0, sizeof(EntityManager));

	entityManager.entityList = (Entity*)malloc(sizeof(Entity)*maxEnt);
	if (!entityManager.entityList)
	{
		slog("failed to allocate entity list");
		EntityManagerClose();
		return;
	}
	memset(entityManager.entityList, 0, sizeof(Entity)*maxEnt);
	entityManager.maxEntities = maxEnt;
	atexit(EntityManagerClose);
	slog("entity system initialized");

	atexit(EntityManagerClose);
}

void EntityManagerClose()
{
	int i = 0;

	if (entityManager.entityList != NULL)
	{
		for (i = 0; i < entityManager.maxEntities; ++i)
		{		
			FreeEntity(&entityManager.entityList[i]);
		}

		free(entityManager.entityList);
	}

	memset(&entityManager, 0, sizeof(EntityManager));
	slog("Entities freed and manager closed");
}

Entity* NewEntity(Actor* act)
{
	int i;
	Entity* ent = NULL;

	for (i = 0; i < entityManager.maxEntities; i++)
	{
		if (entityManager.entityList[i]._inUse == 0)
		{
			memset(&entityManager.entityList[i], 0, sizeof(Entity));

			ent = &entityManager.entityList[i];

			ent->id = entityManager.numEntities++;
			ent->_inUse = 1;

			vector2d_set(entityManager.entityList[i].scale, 1, 1);
			
			ent->actor = act;

			ent->scale = vector2d(1,1);
			
			ent->id = entityManager.numEntities;

			ent->Draw = EntityDraw;

			ent->Think = EntityThinkAll;

			ent->Update = EntityUpdate;

			ent->Free = EntityDelete;

			ent->renderTarget = NULL;
			
			++entityManager.numEntities;

			ent->actor->color = vector4d(1, 1, 1, 1);// no color shift, opaque
			return ent;
		}
	}
	return NULL;
}

void FreeEntity(Entity *self)
{
	//int i;

	if (!self)return;

	if (self->Free)
	{
		self->Free(self);
	}

	/*
	for (i = 0; i < EntitySoundMax; i++)
	{
		gf2d_sound_free(self->sound[i]);
	}*/

	FreeActor(self->actor);
	
	//gf2d_particle_emitter_free(self->pe);
	
	memset(self, 0, sizeof(Entity));
}

void EntityDraw(Entity *self)
{
	if (!self)
	{
		return;
	}

	if (!self->_inUse)
	{
		return;
	}

	//draw entity's particle emitter
	DrawSpriteImage(self->actor->currentSprite, self->position, self->actor->currentAnimation->cellWidth, self->actor->currentAnimation->cellHeight);

}

void EntityDrawAll()
{
	int i;

	for (i = 0; i < entityManager.maxEntities; ++i)
	{	
		if (entityManager.entityList[i]._inUse == 0)
		{
			continue;
		}

		if (entityManager.entityList[i].Draw != NULL)
		{
			//slog("Entity name: %s", entityManager.entityList[i].actor->name);
			entityManager.entityList[i].Draw(&entityManager.entityList[i]);
		}
	}
}

void EntityUpdate(Entity *self)
{
	if (!self)
	{
		return;
	}

	if (!self->_inUse)
	{
		return;
	}

	if (self->dead != 0)
	{
		FreeEntity(self);
	}

	//update position logic

	if (self->Update != NULL)
	{
		self->Update(self);
	}
}

void EntityUpdateAll()
{
	int i;

	for (i = 0; i < entityManager.maxEntities; ++i)
	{
		if (entityManager.entityList[i]._inUse == 0)
		{
			continue;
		}

		if (entityManager.entityList[i].Update != NULL)
		{
			entityManager.entityList[i].Update(&entityManager.entityList[i]);
		}
	}
}

void EntityThinkAll()
{
	int i;
	for (i = 0; i < entityManager.maxEntities; i++)
	{
		if (entityManager.entityList[i]._inUse == 0)continue;
		if (entityManager.entityList[i].Think != NULL)
		{
			entityManager.entityList[i].Think(&entityManager.entityList[i]);
		}
	}
}

void EntityThink(Entity *self)
{
	if (!self)
	{
		return;
	}

	if (self->Think != NULL)
	{
		self->Think(self);
	}
}

void EntityDelete(Entity *self)
{
	if (!self)
	{
		return;
	}

	if (!self->actor)
		return;

	DeleteActor(self->actor);

	memset(self, 0, sizeof(Entity));
}

void EntityDeleteAll()
{
	int i;
	for (i = 0; i < entityManager.maxEntities; i++)
	{
		EntityDelete(&entityManager.entityList[i]);
	}
}