#ifndef __ENTITY__
#define	__ENTITY__

#include "gf2d_text.h"
#include "sprites.h" // replace
#include "gf2d_vector.h"



typedef enum
{
	State_Idle,
	State_Attacking,
	State_Hurt,
	State_Dead,
	State_Walking
}EntityState;

typedef struct
{
	EntityState entState;

}AnimationState;

typedef enum
{
	north, 
	east, 
	south, 
	west
}Direction;


typedef struct Entity_S
{
	Uint8					_inUse;
	Uint32					maxEntities;
	Uint64					id;	

	EntityState				state;

	void(*draw)(struct Entity_S *self);											/**<called after system entity drawing for custom effects*/
	void(*think)(struct Entity_S *self);										/**<called before system updates to make decisions / hand input*/
	void(*update)(struct Entity_S *self);										/**<called after system entity update*/
	int(*touch)(struct Entity_S *self, struct Entity_S *other);					/**<when this entity touches another entity*/
	int(*damage)(struct Entity_S *self, int amount, struct Entity_S *source);	/**<when this entity takes damage*/
	void(*die)(struct Entity_S *self);											/**<when this entity dies*/
	void(*free)(struct Entity_S *self);											/**<called when the entity is freed for any custom cleanup*/
	int dead;																	/**<when true, the entity system will delete the entity on the next update*/

	float					health;
	int						maxHealth;
}Entity;

/**
* @brief Intialize the entity system
*/
void EntityManagerInit();

/**
* @brief Close and free the entity system
*/
void EntityManagerClose();

#endif