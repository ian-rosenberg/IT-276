#include "player.h"
#include "graphics.h"

#define PLAYER_SPEED 5
#define PLAYER_ACCELERATION 0.25

static Player player = { 0 };

PlayerInput HandleInput(const Uint8 *keys)
{
	SDL_PumpEvents();

	keys = SDL_GetKeyboardState(NULL);

	if (keys[SDL_SCANCODE_ESCAPE])
	{
		return Quit;
	}
	if (keys[SDL_SCANCODE_W])
	{
		return MoveNorth;
	}
	if (keys[SDL_SCANCODE_A])
	{
		return MoveWest;
	}
	if (keys[SDL_SCANCODE_S])
	{
		return MoveSouth;
	}
	if (keys[SDL_SCANCODE_D])
	{
		return MoveEast;
	}
	if (keys[SDL_SCANCODE_Q])
	{
		return Cancel;
	}
	if (keys[SDL_SCANCODE_E])
	{
		return Interact;
	}
	if (keys[SDL_SCANCODE_F])
	{
		return Menu;
	}

	return None;
}

void PlayerInit()
{
	Actor *pActor = LoadActor("data/player.actor");

	player.self = NewEntity(pActor);
	player.self->actor->animState = State_Idle;

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
	player.self->Update = PlayerUpdate;
}

void PlayerThink(Entity *self)
{
	if (player.curInputState == player.prevInputState)
	{
		return;
	}
	
	player.plannedMovement.x = 0;
	player.plannedMovement.y = 0;

	if (player.curInputState == Interact)
	{

	}
	if (player.curInputState == Cancel)
	{

	}
	if (player.curInputState == Menu)
	{

	}
	if (player.curInputState == MoveNorth)
	{
		player.plannedMovement.y = -1;
	}
	if (player.curInputState == MoveSouth)
	{
		player.plannedMovement.y = 1;
	}
	if (player.curInputState == MoveEast)
	{
		player.plannedMovement.x = 1;
	}
	if (player.curInputState == MoveWest)
	{
		player.plannedMovement.x = -1;
	}
}

void PlayerUpdate(Entity *self)
{
	if (player.self->gravityEnabled)
	{
		MovePlayerGravity();
	}
	else
	{
		MovePlayerTopDown();
		
		if (player.prevInputState != player.curInputState)
		{
			if (player.plannedMovement.x > 0)
			{
				player.self->facing.x = 0;
			}
			else if (player.plannedMovement.x < 0)
			{
				player.self->facing.x = 1;
			}
			
			if (player.plannedMovement.x != 0 || player.plannedMovement.y != 0)
			{	
				EntityNextAnimation(player.self, State_Walking);
			}
			else
			{
				EntityNextAnimation(player.self, State_Idle);
			}
		}
	}	
}

void SetInputState(PlayerInput state)
{
	player.prevInputState = player.curInputState;

	player.curInputState = state;
}

void MovePlayerGravity()
{
	/*if (player.plannedMovement.x == 1.0f || player.plannedMovement.x == -1.0f)
	{
		player.self->velocity.x += player.self->speed * player.plannedMovement.x;
		player.self->speed += PLAYER_ACCELERATION * player.plannedMovement.x;
	}
	else
	{
		if (player.self->velocity.x < -0.75f)
		{
			player.self->velocity.x += PLAYER_ACCELERATION;
		}
		else if (player.self->velocity.x > 0.75f)
		{
			player.self->velocity.x -= PLAYER_ACCELERATION;
		}
		else
		{
			player.self->velocity.x = 0;
		}
	}

	player.self->position.x += player.self->velocity.x * GetFrameDelay() * PLAYER_ACCELERATION;	*/
}

void MovePlayerTopDown()
{
	if (player.plannedMovement.x != 0)
	{
		player.self->position.x += PLAYER_ACCELERATION * GetFrameDelay() * player.plannedMovement.x;
	}
	if (player.plannedMovement.y != 0)
	{
		player.self->position.y += PLAYER_ACCELERATION * GetFrameDelay() * player.plannedMovement.y;
	}
}