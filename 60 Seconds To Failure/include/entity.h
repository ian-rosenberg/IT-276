#ifndef __ENTITY__
#define	__ENTITY__

#include "actor.h"

typedef struct Entity_S
{
	Uint8					_inUse;
	Uint64					id;	

	State					logicalState; // Same enum as used for animations, used here for logic of those states

	Actor					*actor;

	SDL_Rect				boundingBox, srcRect;

	Vector2D				velocity;
	Vector2D				position;
	Vector2D				scale;												/**<scale to draw sprite at*/
	Vector2D				scaleCenter;										/**<where to scale sprite from*/
	Vector3D				rotation;											/**<how to rotate the sprite*/
	Vector2D				flip;												/**<if to flip the sprite*/

	void(*Draw)(struct Entity_S *self);											/**<called after system entity drawing for custom effects*/
	void(*Think)(struct Entity_S *self);										/**<called before system updates to make decisions / hand input*/
	void(*Update)(struct Entity_S *self);										/**<called after system entity update*/
	//int(*touch)(struct Entity_S *self, struct Entity_S *other);					/**<when this entity touches another entity*/
	//int(*damage)(struct Entity_S *self, int amount, struct Entity_S *source);	/**<when this entity takes damage*/
	//void(*die)(struct Entity_S *self);											/**<when this entity dies*/
	void(*Free)(struct Entity_S *self);											/**<called when the entity is freed for any custom cleanup*/
	int dead;																	/**<when true, the entity system will delete the entity on the next update*/

	float					health;
	int						maxHealth;

	SDL_Texture				*renderTarget;
}Entity;

/**
* @brief Intialize the entity system
* @param maxEnt The max number of entities for the manager
*/
void EntityManagerInit(Uint32 maxEnt);

/**
* @brief Close and free the entity system
*/
void EntityManagerClose();

/**
* @brief Create a new entity given an actor
* @param act The actor to provide
* @returns The new entity to reference
*/
Entity* NewEntity(Actor* act);

/**
* @brief Free an entity
* @param self Entity to free
*/
void FreeEntity(Entity* self);

/**
* @brief Render an entity to screen
* @param self The entity to draw
*/
void EntityDraw(Entity *self);

/**
* @brief Render all entites to screen
*/
void EntityDrawAll();

/**
* @brief Update an individual entity
* @param self The entity to update
*/
void EntityUpdate(Entity *self);

/**
* @brief Update all entites
*/
void EntityUpdateAll();

/**
* @brief Let an entity think
* @param self The entity to allow to think
*/
void EntityThink(Entity *self);

/**
* @brief Let all managed entities think
*/
void EntityThinkAll();

/**
* @brief Delete an entity
* @param self The entity to delete
*/
void EntityDelete(Entity *self);

/**
* @brief Delete all entities
*/
void EntityDeleteAll();
#endif