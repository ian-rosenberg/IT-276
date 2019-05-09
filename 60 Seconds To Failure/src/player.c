#include "player.h"
#include "simple_logger.h"
#include "camera.h"
#include "worlds.h"

const int JOYSTICK_DEAD_ZONE = 8000;
const Vector2D overworldSpawn = {288,128};
const Vector2D sideViewSpawn = { 32,320 };

static Player player = { 0 };

void PlayerInit()
{
	Vector3D dimensions = { 0 };
	Vector2D widthHeight = { 0 };

	Actor *playerActor = LoadActor("data/player.actor");

	player.self = NewEntity(playerActor);

	widthHeight = GetAverageActorDimensions(playerActor);

	dimensions = vector3d(widthHeight.x, widthHeight.y, 1.0f);

	player.dimensions = dimensions;

	player.self->maxHealth = 100.0f;
	player.self->health = player.self->maxHealth;

	player.self->position = overworldSpawn;

	player.self->shape = gf2d_shape_circle(0, 0,25);
	
	SetPlayerGravity();

	player.self->Draw = PlayerDraw;
	player.self->Think = PlayerThink;
	player.self->Update = PlayerUpdate;
	player.self->Free = EntityFree;
	player.self->touch = PlayerTouch;

	player.self->velocity = vector2d(0, 0);
	player.self->facing = vector2d(1, 0);
	player.self->flip = vector2d(0, 0);
	player.self->scale = vector2d(0.75f, 0.75f);

	player.self->actor = playerActor;

	player.controller = SDL_GameControllerOpen(0);
	player.maxSpeed = 2.0f;

	player.self->actor->animState = State_Idle;
	player.self->logicalState = State_Idle;

	player.self->actor->currentAnimation = GetAnimationByName(playerActor->animations, playerActor->numAnimations, "idle");
	player.self->actor->currentSprite = player.self->actor->currentAnimation->sprite;
	player.owner = NULL;

	player.self->actor->currentAnimation->currentFrame = 0.0f;
}

Entity* GetPlayerEntity()
{
	if (player.self)
	{
		return player.self;
	}

	slog("Player entity not ready!");

	return NULL;
}

void PlayerThink(Entity *self)
{
	if (player.controller != 0 && SDL_GameControllerGetAttached(player.controller)) {
		Vector2D LeftStick = vector2d(SDL_GameControllerGetAxis(player.controller, SDL_CONTROLLER_AXIS_LEFTX),
							SDL_GameControllerGetAxis(player.controller, SDL_CONTROLLER_AXIS_LEFTY));

		int16_t LeftTrigger = SDL_GameControllerGetAxis(player.controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT);
		int16_t RightTrigger = SDL_GameControllerGetAxis(player.controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT);

		Bool AButton = SDL_GameControllerGetButton(player.controller, SDL_CONTROLLER_BUTTON_A);
		Bool BButton = SDL_GameControllerGetButton(player.controller, SDL_CONTROLLER_BUTTON_B);
		Bool XButton = SDL_GameControllerGetButton(player.controller, SDL_CONTROLLER_BUTTON_X);
		Bool YButton = SDL_GameControllerGetButton(player.controller, SDL_CONTROLLER_BUTTON_Y);
		Bool Up = SDL_GameControllerGetButton(player.controller, SDL_CONTROLLER_BUTTON_DPAD_UP);
		Bool Down = SDL_GameControllerGetButton(player.controller, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
		Bool Left = SDL_GameControllerGetButton(player.controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
		Bool Right = SDL_GameControllerGetButton(player.controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
		Bool Start = SDL_GameControllerGetButton(player.controller, SDL_CONTROLLER_BUTTON_START);
		Bool Back = SDL_GameControllerGetButton(player.controller, SDL_CONTROLLER_BUTTON_BACK);

		//Right Trigger Pressed
		if (RightTrigger > 1000) {

		}
		//Left Trigger Pressed
		if (LeftTrigger > 1000) {

		}

		//Axis lock on left stick
		if (LeftStick.x < JOYSTICK_DEAD_ZONE && LeftStick.x > -JOYSTICK_DEAD_ZONE)
			LeftStick.x = 0;
		if (LeftStick.y < JOYSTICK_DEAD_ZONE && LeftStick.y > -JOYSTICK_DEAD_ZONE)
			LeftStick.y = 0;

		//Stop moving player when left stick is left alone
		if (LeftStick.x > 0 && self->velocity.x < 0) {
			player.self->velocity.x = 0.0f;
		}
		if (LeftStick.x < 0 && self->velocity.x > 0) {
			player.self->velocity.x = 0.0f;
		}
		if (LeftStick.y > 0 && self->velocity.y < 0) {
			player.self->velocity.y = 0.0f;
		}
		if (LeftStick.y < 0 && self->velocity.y > 0) {
			player.self->velocity.y = 0.0f;
		}

		//Move player according to left stick
		player.self->velocity.x = LeftStick.x*0.0010f;
		player.self->velocity.y = LeftStick.y*0.0010f;

		//If left stick is untouched, player stops moving
		if (LeftStick.x == 0)
			player.self->velocity.x = 0.0f;

		if (LeftStick.y == 0)
			player.self->velocity.y = 0.0f;
	}
	else 
	{
		player.keys = SDL_GetKeyboardState(NULL);
		
		if (player.keys[SDL_SCANCODE_A] && self->velocity.x < 0) {
			player.self->velocity.x = 0.0f;
		}
		if (player.keys[SDL_SCANCODE_D] && self->velocity.x > 0) {
			player.self->velocity.x = 0.0f;
		}
		if (player.keys[SDL_SCANCODE_S] && self->velocity.y < 0) {
			player.self->velocity.y = 0.0f;
		}
		if (player.keys[SDL_SCANCODE_W] && self->velocity.y > 0) {
			player.self->velocity.y = 0.0f;
		}

		//Move player according to  key input
		if (player.keys[SDL_SCANCODE_A])
		{
			player.self->velocity.x = -player.maxSpeed;
		}
		else if (player.keys[SDL_SCANCODE_D])
		{
			player.self->velocity.x = player.maxSpeed;
		}
		if (player.keys[SDL_SCANCODE_W])
		{
			player.self->velocity.y = -player.maxSpeed;
		}
		else if (player.keys[SDL_SCANCODE_S])
		{
			player.self->velocity.y = player.maxSpeed;
		}

		//If keys are untouched, player stops moving
		if (!player.keys[SDL_SCANCODE_A] && !player.keys[SDL_SCANCODE_D])
			player.self->velocity.x = 0.0f;

		if (!player.keys[SDL_SCANCODE_W] && !player.keys[SDL_SCANCODE_S])
			player.self->velocity.y = 0.0f;
	}


	//Set logical state
	if (player.self->velocity.x != 0.0f ||
		player.self->velocity.y != 0.0f)
	{
		self->logicalState = State_Walking;
		self->actor->currentAnimation = GetAnimationByName(player.self->actor->animations, player.self->actor->numAnimations, "walk");
		self->actor->currentSprite = self->actor->currentAnimation->sprite;
	}
	else
	{
		self->logicalState = State_Idle;
		self->actor->currentAnimation = GetAnimationByName(player.self->actor->animations, player.self->actor->numAnimations, "idle");
		self->actor->currentSprite = self->actor->currentAnimation->sprite;
	}

	if (player.self->body.gravity != 0)
	{
		entity_apply_gravity(player.self);
	}

	SetCameraPosition(self->position);
}

void PlayerDraw(Entity *self)
{
	Vector2D centerScalePoint;
	Vector2D resultPos = { 0 };
	Color debugColor = gf2d_color8(.5, 1, 0, 1);

	if (!self)
	{
		return;
	}

	if (!self->_inUse)
	{
		return;
	}

	if ((self->health / self->maxHealth) < 0.25f)
	{
		self->actor->color = vector4d(SDL_MAX_UINT8, 0, 0, SDL_MAX_UINT8);
	}
	else
	{
		self->actor->color = vector4d(SDL_MAX_UINT8, SDL_MAX_UINT8, SDL_MAX_UINT8, SDL_MAX_UINT8);
	}

	centerScalePoint = vector2d(self->actor->currentAnimation->cellWidth / 2.0f,
		self->actor->currentAnimation->cellHeight / 2.0f);

	vector2d_sub(resultPos, self->position, GetCameraPosition());

	DrawSprite(self->actor->currentSprite,
		resultPos,
		&self->scale,
		&centerScalePoint,
		&self->rotation,
		&self->flip,
		&self->actor->color,
		self->actor->currentAnimation->currentFrame,
		self->actor->currentAnimation->yOffset,
		self->actor->currentAnimation->cellWidth,
		self->actor->currentAnimation->cellHeight);


	AnimationNextFrame(self->actor->currentAnimation, &self->actor->currentAnimation->currentFrame);
}

void PlayerUpdate(Entity *self)
{
	Vector2D tilemapDimensions = vector2d(GetCurrentTileMap()->boundingBox.w, GetCurrentTileMap()->boundingBox.h);

	if (!self)
	{
		return;
	}
	
	if (self->dead != 0)
	{
		return;
	}

	//Movement
	if (player.self->velocity.x != 0 || player.self->velocity.y != 0)
	{
		if (player.self->velocity.x < 0)
		{
			player.self->flip.x = 1;
		}
		else if (player.self->velocity.x > 0)
		{
			player.self->flip.x = 0;
		}
	}

	if (player.self->velocity.x > player.maxSpeed) {
		player.self->velocity.x = player.maxSpeed;
	}
	else if (player.self->velocity.x < -player.maxSpeed) {
		player.self->velocity.x = -player.maxSpeed;
	}
	if (player.self->velocity.y > player.maxSpeed) {
		player.self->velocity.y = player.maxSpeed;
	}
	else if (player.self->velocity.y < -player.maxSpeed) {
		player.self->velocity.y = -player.maxSpeed;
	}

	//set anim state
	if (self->logicalState != self->actor->animState)
	{
		switch (self->logicalState)
		{
			case State_Walking:
				self->logicalState = self->actor->animState;
				self->actor->currentAnimation = GetAnimationByName(player.self->actor->animations, player.self->actor->numAnimations, "walk");
				self->actor->currentSprite = self->actor->currentAnimation->sprite;
				self->scaleCenter = vector2d(self->actor->currentAnimation->cellWidth/2, self->actor->currentAnimation->cellHeight/2);
				break;

			case State_Idle:
				self->logicalState = self->actor->animState;
				self->actor->currentAnimation = GetAnimationByName(player.self->actor->animations, player.self->actor->numAnimations, "idle");
				self->actor->currentSprite = self->actor->currentAnimation->sprite; 
				self->scaleCenter = vector2d(self->actor->currentAnimation->cellWidth/2, self->actor->currentAnimation->cellHeight/2); 
				break;

			default:
				break;
		}
	}
}

void PlayerFree()
{
	SDL_GameControllerClose(player.controller);

	DeleteActor(player.self->actor);

	MapRemoveEntity(GetCurrentTileMap(), &player.self->body);
}

int PlayerTouch(Entity *self, Entity *other)
{
	slog("Player touched something");
}

void SetPlayerGravity()
{
	if (player.self->body.data != NULL)
	{
		slog("Player already has a physics body");

		return;
	}
	
	if(GetGameWorld()->overworld == GetCurrentTileMap())
	{
		gf2d_body_set(&player.self->body,
			player.self->actor->name,
			1,
			PLAYER_LAYER,
			ALL_LAYERS,
			PLAYER_TEAM,
			player.self->position,
			player.self->velocity,
			10,
			0,
			10,
			&player.self->shape,
			player.self,
			player.self->body.touch);
	}
	else 
{
		gf2d_body_set(&player.self->body,
			player.self->actor->name,
			1,
			PLAYER_LAYER,
			ALL_LAYERS,
			PLAYER_TEAM,
			player.self->position,
			player.self->velocity,
			10,
			1,
			10,
			&player.self->shape,
			player.self,
			player.self->body.touch);
	}
}
