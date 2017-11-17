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

	// idle animation (forward movement with no hill)
	idle.frames.push_back({ 0, 44, 90, 41 });

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

	//turn right (no hill)
	right.frames.push_back({ 0, 165, 84, 41 });
	right.frames.push_back({ 92, 165, 84, 41 });
	right.speed = 0.2f;

	//turn right (uphill)
	rightUp.frames.push_back({ 0, 121, 84, 44 });
	rightUp.frames.push_back({ 92, 121, 84, 44 });
	rightUp.speed = 0.2f;

	//BREAK
	breakCenter.frames.push_back({ 186, 44, 90, 41 });
	breakCenter.frames.push_back({ 281, 44, 90, 41 });
	breakCenter.speed = 0.2f;

	//Passengers
	malePlayer.frames.push_back({ 389, 15, 24, 15 });
	malePlayer.frames.push_back({ 389, 29, 24, 15 });
	malePlayer.speed = 0.025f;
	femalePlayer.frames.push_back({ 419, 16, 15, 12 });
	femalePlayer.frames.push_back({ 419, 28, 14, 14 });
	femalePlayer.speed = 0.025f;
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");

	car = App->textures->Load("rtype/ferrari.png");

	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");

	App->textures->Unload(car);

	return true;
}

// Update: draw background
update_status ModulePlayer::Update()
{
	bool moving = false;
	bool turnLeft = false;
	bool turnRight = false;

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		turnLeft = true;
	}
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		turnRight = true;
	}

	if (turnLeft && turnRight) {
		turnLeft = false;
		turnRight = false;
	}

	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
	{
		Segment s;
		if (App->scene_stage->topSegment.yMapPosition < 55) {
			s = App->scene_stage->topSegment;
		}
		else {
			s = App->scene_stage->bottomSegment;
		}
		switch (s.inc) {
		case Inclination::UP:
			if (turnLeft) {
				currentCar = &leftUp;
				playersDx = 4;
			}
			else if (turnRight) {
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
			if (turnLeft) {
				currentCar = &left;
				playersDx = 4;
			}
			else if (turnRight) {
				currentCar = &right;
				playersDx = -4;
			}
			else {
				currentCar = &breakCenter;
				playersDx = 0;
			}
			break;
		}
		if (playerSpeed > 0.0f) {
			playerSpeed -= 0.1f;
			if (playerSpeed < 0.0f)
				playerSpeed = 0.0f;
		}
		if (playerSpeed )
		position += playerSpeed;
		curveSpeed = 0.0f;
		moving = true;
	}

	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
	{
		Segment s;
		if (App->scene_stage->topSegment.yMapPosition < 55) {
			s = App->scene_stage->topSegment;
		}
		else {
			s = App->scene_stage->bottomSegment;
		}
		switch (s.inc) {
		case Inclination::UP:
			if (turnLeft) {
				currentCar = &leftUp;
				playersDx = 4;
			}
			else if (turnRight) {
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
			if (turnLeft) {
				currentCar = &left;
				playersDx = 4;
			}
			else if (turnRight) {
				currentCar = &right;
				playersDx = -4;
			}
			else {
				currentCar = &forward;
				playersDx = 0;
			}
			break;
		}
		position += playerSpeed;
		curveSpeed = 1.5f;
		moving = true;
	}

	if (moving == false) {
		if (turnLeft) {
			currentCar = &idleLeft;
			playersDx = 4;
		}
		else if (turnRight) {
			currentCar = &idleRight;
			playersDx = -4;
		}
		else {
			playersDx = 0;
			currentCar = &idle;
		}
		curveSpeed = 0.0f;
	}

	App->renderer->Blit(car, (SCREEN_WIDTH - 92) / 2, SCREEN_HEIGHT - 48, &(currentCar->GetCurrentFrame()));
	App->renderer->Blit(car, (SCREEN_WIDTH - 50 + playersDx) / 2, SCREEN_HEIGHT - 50, &(malePlayer.GetCurrentFrame()));
	App->renderer->Blit(car, (SCREEN_WIDTH + 6 + playersDx) / 2, SCREEN_HEIGHT - 48, &(femalePlayer.GetCurrentFrame()));

	return UPDATE_CONTINUE;
}
