#include "player.h"
#include "graphics.h"

#define PLAYER_SPEED 40
#define PLAYER_ACCELERATION 0.1667

static Player player = { 0 };

void PlayerInit()
{
	Actor *pActor = LoadActor("data/player.actor");

	player.self = NewEntity(pActor);
	player.self->actor->animState = State_Idle;
	player.self->logicalState = State_Idle;

	player.controller = SDL_GameControllerOpen(0);

	player.plannedMovement = vector2d(0, 0);
	player.self->velocity = vector2d(0, 0);

	player.self->position = vector2d(200, 200);

	player.self->maxSpeed = PLAYER_SPEED;
	player.self->speed = 0;

	player.self->actor->currentAnimation = GetAnimationByName(pActor->animations, 
		pActor->numAnimations, 
		"idle");
	player.self->actor->currentSprite = player.self->actor->currentAnimation->sprite;

	player.self->Draw = EntityDraw;
	player.self->Think = PlayerThink;
	//player.self->Update = PlayerUpdate;
}

void PlayerThink(Entity *self)
{
	PlayerInput inputState = HandleInput(player.keys);

	player.plannedMovement.x = 0;
	player.plannedMovement.y = 0;

	if ((inputState & Interact) == Interact)
	{

	}
	if ((inputState & Cancel) == Cancel)
	{

	}
	if ((inputState & Menu) == Menu)
	{

	}
	if ((inputState & MoveNorth) == MoveNorth)
	{
		player.plannedMovement.y = -1;
	}
	if ((inputState & MoveSouth) == MoveSouth)
	{
		player.plannedMovement.y = 1;
	}
	if ((inputState & MoveEast) == MoveEast)
	{
		player.plannedMovement.x = 1;
	}
	if ((inputState & MoveWest) == MoveWest)
	{
		player.plannedMovement.x = -1;
	}
}

void PlayerUpdate(Entity *self)
{
	MovePlayer();

	if (player.self->gravityEnabled)
	{
		//ApplyGravity();
	}

	
}

void MovePlayer()
{
	if (player.plannedMovement.x != 0)
	{
		player.self->velocity.x += player.self->speed * player.plannedMovement.x;
		player.self->speed += PLAYER_ACCELERATION * player.plannedMovement.x;

		if (player.self->speed > player.self->maxSpeed)
		{
			if (player.self->velocity.x < 0)
			{
				player.self->speed = -player.self->maxSpeed;
			}
			else
			{
				player.self->speed = player.self->maxSpeed;
			}
		}
	}
	else
	{
		if (player.self->velocity.x < -0.75f)
		{
			player.self->velocity.x += 1.5f;
		}
		else if (player.self->velocity.x > 0.75f)
		{
			player.self->velocity.x -= 1.5f;
		}
		else
		{
			player.self->velocity.x = 0;
		}
	}

	player.self->position.x += player.self->velocity.x * GetFrameDelay();
}