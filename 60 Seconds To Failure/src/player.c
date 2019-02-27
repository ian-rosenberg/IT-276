#include "player.h"
#include "simple_logger.h"

const int JOYSTICK_DEAD_ZONE = 8000;

static Player player = { 0 };

void PlayerInit()
{
	Vector3D dimensions = { 0 };
	Vector2D widthHeight = { 0 };

	Actor *playerActor = LoadActor("data/player.actor");

	player.self = NewEntity(playerActor);

	widthHeight = GetAverageActorDimensions(playerActor);

	dimensions = vector3d(widthHeight.x, widthHeight.y, 1.0f);

	//player.self->physBody = NewBodyInfo(dimensions, GetCurrentWorldSpace(), false);

	player.self->maxHealth = 100.0f;
	player.self->health = player.self->maxHealth;

	player.self->Draw = PlayerDraw;
	player.self->Think = PlayerThink;
	player.self->Update = PlayerUpdate;

	player.self->velocity = vector2d(0, 0);

	player.self->flip = vector2d(0, 0);
	player.self->scale = vector2d(1, 1);

	player.self->position = vector2d(0, 0);

	player.self->actor = playerActor;

	player.controller = SDL_GameControllerOpen(0);
	player.maxSpeed = 1.0f;

	player.self->actor->animState = State_Idle;
	player.self->logicalState = State_Idle;

	player.self->actor->currentAnimation = GetAnimationByName("idle");
	player.self->actor->currentSprite = player.self->actor->currentAnimation->sprite;

	player.self->actor->currentAnimation->currentFrame = 0.0f;
}

Entity* GetPlayerEntity()
{
	return player.self;
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
			self->velocity.x = 0.0f;
		}
		if (LeftStick.x < 0 && self->velocity.x > 0) {
			self->velocity.x = 0.0f;
		}
		if (LeftStick.y > 0 && self->velocity.y < 0) {
			self->velocity.y = 0.0f;
		}
		if (LeftStick.y < 0 && self->velocity.y > 0) {
			self->velocity.y = 0.0f;
		}

		//Move player according to left stick
		self->velocity.x = LeftStick.x*0.000010f;
		self->velocity.y = LeftStick.y*0.000010f;

		if (self->velocity.x > player.maxSpeed) {
			self->velocity.x = player.maxSpeed;
		}
		else if (self->velocity.x < -player.maxSpeed) {
			self->velocity.x = -player.maxSpeed;
		}
		if (self->velocity.y > player.maxSpeed) {
			self->velocity.y = player.maxSpeed;
		}
		else if (self->velocity.y < -player.maxSpeed) {
			self->velocity.y = -player.maxSpeed;
		}


		//If left stick is untouched, player stops moving
		if (LeftStick.x == 0)
			self->velocity.x = 0.0f;

		if (LeftStick.y == 0)
			self->velocity.y = 0.0f;
	}
	else {
		//Controller is not plugged in.
	}


	//Set logical state
	if (self->velocity.x != 0.0f ||
		self->velocity.y != 0.0f)
	{
		self->logicalState = State_Walking;
		self->actor->currentAnimation = GetAnimationByName("walk");
		self->actor->currentSprite = self->actor->currentAnimation->sprite;
	}
	else
	{
		self->logicalState = State_Idle;
		self->actor->currentAnimation = GetAnimationByName("idle");
		self->actor->currentSprite = self->actor->currentAnimation->sprite;
	}

}

void PlayerDraw(Entity *self)
{
	Vector2D centerScalePoint;

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

	DrawSprite(self->actor->currentSprite,
		self->position,
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

		player.self->position.x += player.self->velocity.x;
		player.self->position.y += player.self->velocity.y;
	}

	//set anim state
	if (self->logicalState != self->actor->animState)
	{
		switch (self->logicalState)
		{
			case State_Walking:
				self->logicalState = self->actor->animState;
				self->actor->currentSprite = self->actor->currentAnimation->sprite;
				self->scaleCenter = vector2d(self->actor->currentAnimation->cellWidth, self->actor->currentAnimation->cellHeight);
				break;

			case State_Idle:
				self->logicalState = self->actor->animState;
				self->actor->currentSprite = self->actor->currentAnimation->sprite; 
				self->scaleCenter = vector2d(self->actor->currentAnimation->cellWidth, self->actor->currentAnimation->cellHeight); 
				break;

			default:
				break;
		}
	}
}

void PlayerFree()
{
	SDL_GameControllerClose(player.controller);
}

