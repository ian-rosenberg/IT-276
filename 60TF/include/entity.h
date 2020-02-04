#ifndef __ENTITY__
#define	__ENTITY__

#include "actor.h"
#include "gf2d_shape.h"
#include "camera.h"

typedef struct Entity_S
{
	Uint8					_inUse;
	Uint8					team;
	Uint16					gravityEnabled;
	Uint64					id;	

	State					prevState; // Same enum as used for animations, used here for logic of those states
	State					curState;

	AnimationReturnType		artState;

	Actor					*actor;

	Rect					boundingBox;

	float					speed;
	float					maxSpeed;

	Vector2D				velocity;
	Vector2D				position;
	Vector2D				drawPos;
	Vector2D				scale;												/**<scale to draw sprite at*/
	Vector3D				rotation;											/**<how to rotate the sprite*/
	Vector2D				facing;												/**<direction the entity is facing*/
	Vector2D				spawnPos;
		
	void(*Draw)(struct Entity_S *self);											/**<called after system entity drawing for custom effects*/
	void(*Think)(struct Entity_S *self);										/**<called before system updates to make decisions / hand input*/
	void(*Update)(struct Entity_S *self);										/**<called after system entity update*/
	int(*Touch)(struct Entity_S *self, struct Entity_S *other);					/**<when this entity touches another entity*/
	void(*Activate)(struct Entity_S *self, struct Entity_S *activator);			/**<some entities can be activated by others, doors opened, levels, etc*/
	int(*Damage)(struct Entity_S *self, int amount, struct Entity_S *source);	/**<when this entity takes damage*/
	void(*Die)(struct Entity_S *self);											/**<when this entity dies*/
	void(*Free)(struct TileMap *map, struct Entity_S *self);					/**<called when the entity is freed for any custom cleanup*/
	
	int						dead;												/**<when true, the entity system will delete the entity on the next update*/

	float					health;
	int						maxHealth;

	int						grounded;

	float					energy;
	int						maxEnergy;
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
* @brief Render an entity to screen
* @param self The entity to draw
*/
void EntityDraw(Entity *self);

/**
* @brief free an entity
*/
void EntityFree(Entity *self);

/*
* @brief Handle collisions between entities
* @param self The lhs entity
* @param other The rhs entity
*/
Bool EntityEntityTouch(Entity *self, Entity *other);

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
* @brief Delete all entities
*/
void EntityFreeAll();

/**
 * @brief search active entities for the one with the provided id
 * @param id the id to search for
 * @return NULL if not found, or the first entity with the id specified otherwise
 */
Entity* GetEntityByID(Uint32 id);


/**
 * @brief step through the entity list from the starting point provided
 * @note if start is NULL, it will return the first active entity in the list
 * @param start the starting position to iterate from
 * @return the next entity in the list that is active or NULL when there are not more
 */
Entity* EntityIterate(Entity *start);

/*
* @brief Handle entity animation transitions
* @param self The entity to animate
* @param newState The state to transition to
*/
void EntityNextAnimation(Entity *self, State newState);

/*
* @brief Set an entity's current animation
* @param self The entity to animate
* @param anim The name of the animation to switch to 
*/
void SetEntityAnimation(Entity *self, const char* anim);

/**
 * @brief check to see if a given pointer points to an entity in the entity system
 * @param p a pointer to data to check
 * @return 0 if the pointer is out of the range of the internal entity manager's list, 1 otherwise
 */
int IsEntityPtrValid(void *p);
#endif