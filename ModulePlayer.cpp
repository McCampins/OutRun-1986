#include "Globals.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleInput.h"
#include "ModuleParticles.h"
#include "ModuleRender.h"
#include "ModuleCollision.h"
#include "ModuleFadeToBlack.h"
#include "ModulePlayer.h"
#include "ModuleSceneStage.h"

ModulePlayer::ModulePlayer(bool active) : Module(active)
{
	//FORWARD 

	// idle animation (stopped movement with no hill)
	idle.frames.push_back({ 0, 44, 90, 41 });

	//idel (stopped uphill)
	idleUp.frames.push_back({ 0, 0, 90, 44 });

	// forward movement with no hill
	forward.frames.push_back({ 0, 44, 90, 41 });
	forward.frames.push_back({ 92, 44, 90, 41 });
	forward.speed = 0.2f;

	// move upwards (forward but uphill)
	forwardUp.frames.push_back({ 0, 0, 90, 44 });
	forwardUp.frames.push_back({ 92, 0, 90, 44 });
	forwardUp.speed = 0.2f;

	//move downwards (forward but downhill)
	forwardDown.frames.push_back({ 0, 85, 90, 36 });
	//forwardDown.frames.push_back({ 92, 0, 90, 44 });
	forwardDown.speed = 0.2f;

	//LEFT

	//idle left (no movement but turned left)
	idleLeft.frames.push_back({ 656, 165, 84, 41 });

	//idle left (going uphill)
	idleUpLeft.frames.push_back({ 653, 121, 86, 44 });

	//turn left (no hill)
	left.frames.push_back({ 656, 165, 84, 41 });
	left.frames.push_back({ 564, 165, 84, 41 });
	left.speed = 0.2f;

	//turn left (uphill)
	leftUp.frames.push_back({ 653, 121, 86, 44 });
	leftUp.frames.push_back({ 561, 121, 86, 44 });
	leftUp.speed = 0.2f;

	//RIGHT

	//idle right (no movement but turned right)
	idleRight.frames.push_back({ 0, 165, 84, 41 });

	//idle right (going uphill)
	idleUpRight.frames.push_back({ 0, 121, 84, 44 });

	//turn right (no hill)
	right.frames.push_back({ 0, 165, 84, 41 });
	right.frames.push_back({ 92, 165, 84, 41 });
	right.speed = 0.2f;

	//turn right (uphill)
	rightUp.frames.push_back({ 0, 121, 84, 44 });
	rightUp.frames.push_back({ 92, 121, 84, 44 });
	rightUp.speed = 0.2f;

	//BREAK

	//break while going forward
	breakCenter.frames.push_back({ 186, 44, 90, 41 });
	breakCenter.frames.push_back({ 281, 44, 90, 41 });
	breakCenter.speed = 0.2f;

	//break while turning left
	breakLeft.frames.push_back({ 375, 165, 84, 41 });
	breakLeft.frames.push_back({ 470, 165, 84, 41 });
	breakLeft.speed = 0.2f;

	//break while turning right
	breakRight.frames.push_back({ 186, 165, 84, 41 });
	breakRight.frames.push_back({ 281, 165, 84, 41 });
	breakRight.speed = 0.2f;

	//break while going uphill
	breakUp.frames.push_back({ 186, 0, 90, 44 });
	breakUp.frames.push_back({ 281, 0, 90, 44 });
	breakUp.speed = 0.2f;

	//break while turning left and going uphill
	breakUpLeft.frames.push_back({ 372, 121, 86, 44 });
	breakUpLeft.frames.push_back({ 467, 121, 86, 44 });
	breakUpLeft.speed = 0.2f;

	//break while turning right and going uphill
	breakUpRight.frames.push_back({ 186, 121, 84, 44 });
	breakUpRight.frames.push_back({ 281, 121, 84, 44 });
	breakUpRight.speed = 0.2f;

	//Passengers
	malePlayer.frames.push_back({ 389, 15, 24, 15 });
	malePlayer.frames.push_back({ 389, 29, 24, 15 });
	malePlayer.speed = 0.025f;
	femalePlayer.frames.push_back({ 419, 16, 15, 12 });
	femalePlayer.frames.push_back({ 419, 28, 14, 14 });
	femalePlayer.speed = 0.025f;

	//Dust (left tire)
	leftDust.frames.push_back({ 0, 29, 68, 33 });
	leftDust.frames.push_back({ 0, 61, 70, 33 });
	leftDust.frames.push_back({ 0, 98, 69, 33 });
	leftDust.frames.push_back({ 0, 133, 72, 33 });
	leftDust.speed = 0.25f;

	//Dust (right tire)
	rightDust.frames.push_back({ 79, 29, 68, 33 });
	rightDust.frames.push_back({ 79, 61, 70, 33 });
	rightDust.frames.push_back({ 79, 98, 69, 33 });
	rightDust.frames.push_back({ 79, 133, 72, 33 });
	rightDust.speed = 0.25f;
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");

	car = App->textures->Load("rtype/ferrari.png");
	dustTex = App->textures->Load("rtype/dust.png");

	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");

	App->textures->Unload(car);

	return true;
}

bool ModulePlayer::keyPressed(int direction, int keysPressed)
{
	switch (direction) {
		//LEFT
	case 0:
		return (keysPressed & 1) != 0;
		break;
		//RIGHT
	case 1:
		return (keysPressed & 2) != 0;
		break;
		//UP
	case 2:
		return (keysPressed & 4) != 0;
		break;
		//DOWN
	case 3:
		return (keysPressed & 8) != 0;
		break;
	}
	return false;
}

// Update: draw background
update_status ModulePlayer::Update()
{
	unsigned int keysPressed = 0;
	float normalizedSpeed = 0.0f;

	Segment s;
	if (App->scene_stage->topSegment.yMapPosition < 55) {
		s = App->scene_stage->topSegment;
	}
	else {
		s = App->scene_stage->bottomSegment;
	}

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		keysPressed += 1;
		if (playerSpeed > 0.0f) {
			normalizedSpeed = (MAX_SPEED - playerSpeed) / MAX_SPEED;
			if (normalizedSpeed < 0.5f) {
				App->renderer->camera.x += 4;
			}
			else {
				App->renderer->camera.x += 2;
			}

		}
	}
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		keysPressed += 2;
		if (playerSpeed > 0.0f) {
			normalizedSpeed = (MAX_SPEED - playerSpeed) / MAX_SPEED;
			if (normalizedSpeed < 0.5f) {
				App->renderer->camera.x -= 4;
			}
			else {
				App->renderer->camera.x -= 2;
			}
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
	{
		keysPressed += 4;
		switch (s.inc) {
		case Inclination::UP:
			if (keyPressed(0, keysPressed)) {
				currentCar = &breakUpLeft;
				playersDx = 4;
			}
			else if (keyPressed(1, keysPressed)) {
				currentCar = &breakUpRight;
				playersDx = -4;
			}
			else {
				currentCar = &breakUp;
				playersDx = 0;
			}
			break;
		case Inclination::CENTER:
		case Inclination::DOWN:
			if (keyPressed(0, keysPressed)) {
				currentCar = &breakLeft;
				playersDx = 4;
			}
			else if (keyPressed(1, keysPressed)) {
				currentCar = &breakRight;
				playersDx = -4;
			}
			else {
				currentCar = &breakCenter;
				playersDx = 0;
			}
			break;
		}
		if (playerSpeed > 0.0f) {
			playerSpeed -= ACCELERATION;
			if (playerSpeed < 0.0f) {
				playerSpeed = 0.0f;
			}
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
	{
		keysPressed += 8;
		switch (s.inc) {
		case Inclination::UP:
			if (keyPressed(0, keysPressed)) {
				currentCar = &leftUp;
				playersDx = 4;
			}
			else if (keyPressed(1, keysPressed)) {
				currentCar = &rightUp;
				playersDx = -4;
			}
			else {
				currentCar = &forwardUp;
				playersDx = 0;
			}
			break;
		case Inclination::CENTER:
		case Inclination::DOWN:
			if (keyPressed(0, keysPressed)) {
				currentCar = &left;
				playersDx = 4;
			}
			else if (keyPressed(1, keysPressed)) {
				currentCar = &right;
				playersDx = -4;
			}
			else {
				currentCar = &forward;
				playersDx = 0;
			}
			break;
		}
		if (playerSpeed < MAX_SPEED) {
			playerSpeed += ACCELERATION;
			if (playerSpeed > MAX_SPEED)
				playerSpeed = MAX_SPEED;
		}
	}

	if (keyPressed(2, keysPressed) == false && keyPressed(3, keysPressed) == false) {
		playerSpeed -= ACCELERATION / 2;
		switch (s.inc) {
		case Inclination::UP:
			if (keyPressed(0, keysPressed)) {
				currentCar = &leftUp;
				playersDx = 4;
			}
			else if (keyPressed(1, keysPressed)) {
				currentCar = &rightUp;
				playersDx = -4;
			}
			else {
				currentCar = &forwardUp;
				playersDx = 0;
			}
			break;
		case Inclination::CENTER:
		case Inclination::DOWN:
			if (keyPressed(0, keysPressed)) {
				currentCar = &left;
				playersDx = 4;
			}
			else if (keyPressed(1, keysPressed)) {
				currentCar = &right;
				playersDx = -4;
			}
			else {
				currentCar = &forward;
				playersDx = 0;
			}
			break;
		}
		if (playerSpeed < 0.0f) {
			playerSpeed = 0.0f;
		}
	}

	if (playerSpeed == 0.0f) {
		if (keyPressed(0, keysPressed)) {
			playersDx = 4;

			switch (s.inc) {
			case Inclination::UP:
				currentCar = &idleUpLeft;
				break;
			case Inclination::CENTER:
			case Inclination::DOWN:
				currentCar = &idleLeft;
				break;
			}
		}
		else if (keyPressed(1, keysPressed)) {
			playersDx = -4;

			switch (s.inc) {
			case Inclination::UP:
				currentCar = &idleUpRight;
				break;
			case Inclination::CENTER:
			case Inclination::DOWN:
				currentCar = &idleRight;
				break;
			}
		}
		else {
			playersDx = 0;

			switch (s.inc) {
			case Inclination::UP:
				currentCar = &idleUp;
				break;
			case Inclination::CENTER:
			case Inclination::DOWN:
				currentCar = &idle;
				break;
			}
		}
	}

	position += playerSpeed;
	curveSpeed = playerSpeed * 10;

	App->renderer->Blit(car, ((SCREEN_WIDTH - 92) / 2) - (App->renderer->camera.x / SCREEN_SIZE), SCREEN_HEIGHT - 48, &(currentCar->GetCurrentFrame()));
	App->renderer->Blit(car, ((SCREEN_WIDTH - 50 + playersDx) / 2) - (App->renderer->camera.x / SCREEN_SIZE), SCREEN_HEIGHT - 50, &(malePlayer.GetCurrentFrame()));
	App->renderer->Blit(car, ((SCREEN_WIDTH + 6 + playersDx) / 2) - (App->renderer->camera.x / SCREEN_SIZE), SCREEN_HEIGHT - 48, &(femalePlayer.GetCurrentFrame()));

	bool out = false;
	if (App->scene_stage->leftTireOut) {
		App->renderer->Blit(dustTex, (SCREEN_WIDTH / 2) - (App->renderer->camera.x / SCREEN_SIZE) - 75, SCREEN_HEIGHT - 40, &(leftDust.GetCurrentFrame()));
		out = true;
	}
	if (App->scene_stage->rigthTireOut) {
		App->renderer->Blit(dustTex, (SCREEN_WIDTH / 2) - (App->renderer->camera.x / SCREEN_SIZE), SCREEN_HEIGHT - 40, &(rightDust.GetCurrentFrame())); 
		out = true;
	}
	if (out) {
		if (playerSpeed > 0.05f) {
			playerSpeed -= ACCELERATION * 2;
			if (playerSpeed < 0.0f) {
				playerSpeed = 0.0f;
			}
		}
	}

	return UPDATE_CONTINUE;
}
