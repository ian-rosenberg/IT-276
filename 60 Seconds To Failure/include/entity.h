#ifndef __ENTITY__
#define	__ENTITY__

#include "actor.h"
#include "gf2d_collision.h"
#include "tilemap.h"

typedef struct Entity_S
{
	Uint8					_inUse;
	Uint64					id;	

	State					logicalState; // Same enum as used for animations, used here for logic of those states

	Actor					*actor;

	SDL_Rect			    srcRect;

	TileMap					*currentMap;

	Body					body;
	Shape					shape;

	Vector2D				velocity;
	Vector2D				position;
	Vector2D				scale;												/**<scale to draw sprite at*/
	Vector2D				scaleCenter;										/**<where to scale sprite from*/
	Vector3D				rotation;											/**<how to rotate the sprite*/
	Vector2D				flip;												/**<if to flip the sprite*/
	Vector2D				facing;												/**<direction the entity is facing*/

	void(*Draw)(struct Entity_S *self);											/**<called after system entity drawing for custom effects*/
	void(*Think)(struct Entity_S *self);										/**<called before system updates to make decisions / hand input*/
	void(*Update)(struct Entity_S *self);										/**<called after system entity update*/
	int(*touch)(struct Entity_S *self, struct Entity_S *other);					/**<when this entity touches another entity*/
	void(*activate)(struct Entity_S *self, struct Entity_S *activator);    /**<some entities can be activated by others, doors opened, levels, etc*/
	int(*damage)(struct Entity_S *self, int amount, struct Entity_S *source);	/**<when this entity takes damage*/
	void(*die)(struct Entity_S *self);											/**<when this entity dies*/
	void(*Free)(struct TileMap *map, struct Entity_S *self);											/**<called when the entity is freed for any custom cleanup*/
	int dead;																	/**<when true, the entity system will delete the entity on the next update*/

	float					health;
	int						maxHealth;

	int						cooldown;
	int						grounded;

	float					jumpCool;

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
void EntityDeleteAll();

/**
 * @brief search active entities for the one with the provided id
 * @param id the id to search for
 * @return NULL if not found, or the first entity with the id specified otherwise
 */
Entity *gf2d_entity_get_by_id(Uint32 id);

/**
 * @brief call before call to collision space update to prep all bodies with their entities
 */
void gf2d_entity_pre_sync_all();

/**
 * @brief call after call to collision space update to get all entities in sync with what happened
 */
void gf2d_entity_post_sync_all();

/**
* @brief prep body for update
*/
void gf2d_entity_pre_sync_body(Entity *self);

/**
* @brief sync body info back to ent
*/
void gf2d_entity_post_sync_body(Entity *self);

/**
* @brief handle body body collisions
*/
int body_body_touch(Body *self, List *collisionList);

/**
 * @brief step through the entity list from the starting point provided
 * @note if start is NULL, it will return the first active entity in the list
 * @param start the starting position to iterate from
 * @return the next entity in the list that is active or NULL when there are not more
 */
Entity *gf2d_entity_iterate(Entity *start);

/**
 * @brief check to see if a given pointer points to an entity in the entity system
 * @param p a pointer to data to check
 * @return 0 if the pointer is out of the range of the internal entity manager's list, 1 otherwise
 */
int gf2d_entity_validate_entity_pointer(void *p);
#endif