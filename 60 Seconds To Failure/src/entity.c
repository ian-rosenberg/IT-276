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
			EntityFree(&entityManager.entityList[i]);
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

			ent->Free = EntityFree;
			
			++entityManager.numEntities;

			ent->actor->color = vector4d(1, 1, 1, 1);// no color shift, opaque
			return ent;
		}
	}
	return NULL;
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
		EntityFree(self);
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

void EntityDeleteAll()
{
	int i;
	for (i = 0; i < entityManager.maxEntities; i++)
	{
		EntityFree(&entityManager.entityList[i]);
	}
}

void gf2d_entity_pre_sync_all()
{
	int i;
	for (i = 0; i < entityManager.maxEntities; i++)
	{
		if (entityManager.entityList[i]._inUse == 0)continue;
		gf2d_entity_pre_sync_body(&entityManager.entityList[i]);
	}
}

void gf2d_entity_post_sync_all()
{
	int i;
	for (i = 0; i < entityManager.maxEntities; i++)
	{
		if (entityManager.entityList[i]._inUse == 0)continue;
		gf2d_entity_post_sync_body(&entityManager.entityList[i]);
	}
}

void gf2d_entity_pre_sync_body(Entity *self)
{
	if (!self)return;// nothin to do
	vector2d_copy(self->body.velocity, self->velocity);
	vector2d_copy(self->body.position, self->position);
}

void gf2d_entity_post_sync_body(Entity *self)
{
	if (!self)return;// nothin to do
//    slog("entity %li : %s old position(%f,%f) => new position (%f,%f)",self->id,self->name,self->position,self->body.position);
	vector2d_copy(self->position, self->body.position);
	vector2d_copy(self->velocity, self->body.velocity);
}

Entity *gf2d_entity_get_by_id(Uint32 id)
{
	int i;
	for (i = 0; i < entityManager.maxEntities; i++)
	{
		if (entityManager.entityList[i]._inUse == 0)continue;
		if (entityManager.entityList[i].id == id)
		{
			return &entityManager.entityList[i];
		}
	}
	return NULL;
}

int body_body_touch(Body *self, List *collisionList)
{
	Entity *selfEnt;
	Collision *c;
	int i, count;
	if (!self)return 0;
	selfEnt = (Entity*)self->data;
	if (!selfEnt->touch)return 0;
	count = gf2d_list_get_count(collisionList);
	for (i = 0; i < count; i++)
	{
		c = (Collision *)gf2d_list_get_nth(collisionList, i);
		if (!c)continue;
		if (!c->body)continue;
		if (!c->body->data)continue;
		selfEnt->touch(selfEnt, (Entity*)c->body->data);
	}
	return 0;
}

void EntityFree(Entity *self)
{
	int i;
	
	if (!self)return;

	memset(self, 0, sizeof(Entity));
	
	//free(self);
}

Entity *gf2d_entity_iterate(Entity *start)
{
	Entity *p = NULL;
	if (!start)p = entityManager.entityList;
	else
	{
		p = start;
		p++;
	}
	for (; p != &entityManager.entityList[entityManager.maxEntities]; p++)
	{
		if (p->_inUse)return p;
	}
	return NULL;
}

int gf2d_entity_validate_entity_pointer(void *p)
{
	Entity *ent;
	if (!p)return 0;
	ent = (Entity *)p;
	if (ent < entityManager.entityList)return 0;
	if (ent >= &entityManager.entityList[entityManager.maxEntities])return 0;
	return 1;
}