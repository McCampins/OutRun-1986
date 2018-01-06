#include "Globals.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleInput.h"
#include "ModuleAudio.h"
#include "ModuleRender.h"
#include "ModuleFadeToBlack.h"
#include "ModulePlayer.h"
#include "ModuleSceneStage.h"

ModulePlayer::ModulePlayer(bool active) : Module(active)
{
	//FORWARD 

	// idle animation (stopped movement with no hill)
	idle.frames.push_back({ 0, 44, 90, 41 });

	//idle (stopped uphill)
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
	malePlayerMoving.frames.push_back({ 389, 15, 24, 15 });
	malePlayerMoving.frames.push_back({ 389, 29, 24, 15 });
	malePlayerMoving.speed = 0.05f;
	femalePlayer.frames.push_back({ 419, 16, 15, 12 });
	femalePlayerMoving.frames.push_back({ 419, 16, 15, 12 });
	femalePlayerMoving.frames.push_back({ 419, 28, 14, 14 });
	femalePlayerMoving.speed = 0.05f;

	//Dust (left tire)
	leftDust.frames.push_back({ 0, 13, 68, 28 });
	leftDust.frames.push_back({ 0, 45, 68, 28 });
	leftDust.frames.push_back({ 0, 81, 68, 28 });
	leftDust.frames.push_back({ 0, 124, 68, 32 });
	leftDust.speed = 0.25f;

	//Dust (right tire)
	rightDust.frames.push_back({ 80, 13, 68, 28 });
	rightDust.frames.push_back({ 80, 45, 68, 28 });
	rightDust.frames.push_back({ 80, 81, 68, 28 });
	rightDust.frames.push_back({ 80, 124, 68, 32 });
	rightDust.speed = 0.25f;
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");

	car = App->textures->Load("rtype/ferrari.png");
	dust = App->textures->Load("rtype/dust.png");

	surpassCarFX = App->audio->LoadFx("rtype/Music/adelantar.wav");
	crashFX = App->audio->LoadFx("rtype/Music/crash.wav");

	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");

	App->textures->Unload(car);
	App->textures->Unload(dust);

	car = nullptr;
	dust = nullptr;
	currentCar = nullptr;

	playersDx = 0;
	position = 0.0f;
	playerSpeed = 0.0f;
	curveSpeed = 0.0f;
	currentLane = 0;
	carX = 0;

	return true;
}

bool ModulePlayer::keyPressed(int direction, int keysPressed) const
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

	switch (App->scene_stage->gameState) {
	case GameState::STARTING:
		currentCar = &idle;
		break;
	case GameState::PLAYING:
		//Get the segment to know its inclination
		Segment* s;
		if (App->scene_stage->topSegment != nullptr) {
			if (App->scene_stage->topSegment->yMapPosition < 55) {
				s = App->scene_stage->topSegment;
			}
			else {
				s = App->scene_stage->bottomSegment;
			}

			//Update camera according to left/right
			if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
			{
				keysPressed += 1;
				if (playerSpeed > 0.0f) {
					normalizedSpeed = playerSpeed / MAX_SPEED;
					if (normalizedSpeed < 0.5f) {
						if (normalizedSpeed > 0.05f) {
							if (normalizedSpeed < 0.1f) {
								App->renderer->camera.x += 12;
							}
							else {
								App->renderer->camera.x += 18;
							}
						}
					}
					else {
						if (normalizedSpeed > 0.75f) {
							App->renderer->camera.x += 12;
						}
						else {
							App->renderer->camera.x += 6;
						}
					}

				}
			}
			if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
			{
				keysPressed += 2;
				if (playerSpeed > 0.0f) {
					normalizedSpeed = playerSpeed / MAX_SPEED;
					if (normalizedSpeed < 0.5f) {
						if (normalizedSpeed > 0.05f) {
							if (normalizedSpeed < 0.1f) {
								App->renderer->camera.x -= 12;
							}
							else {
								App->renderer->camera.x -= 18;
							}
						}
					}
					else {
						if (normalizedSpeed > 0.75f) {
							App->renderer->camera.x -= 12;
						}
						else {
							App->renderer->camera.x -= 6;
						}
					}
				}
			}

			//Update player's speed and car position according to forward/backward
			if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
			{
				keysPressed += 4;
				switch (s->inc) {
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
				switch (s->inc) {
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

			//If no key is pressed, decrement speed
			if (keyPressed(2, keysPressed) == false && keyPressed(3, keysPressed) == false) {
				playerSpeed -= ACCELERATION / 2;
				switch (s->inc) {
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

			//Adjust animation of car if the car has no speed
			if (playerSpeed == 0.0f) {
				if (keyPressed(0, keysPressed)) {
					playersDx = 4;

					switch (s->inc) {
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

					switch (s->inc) {
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

					switch (s->inc) {
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

			//Check if colision
			bool collision = false;
			unsigned int vehicleLane = 0;
			unsigned int size = App->scene_stage->vehicles.size();
			float carPosition = 6.0f + position;
			if (size > 0) {
				unsigned int idx = 0;
				VisualElement* vElem = App->scene_stage->vehicles.at(idx);
				while (idx < App->scene_stage->vehicles.size()) {
					if (int(vElem->world * 10) == int(carPosition * 10)) {
						switch (vElem->x) {
						case -1300:
							vehicleLane = 1;
							break;
						case -825:
							vehicleLane = 2;
							break;
						case -350:
							vehicleLane = 3;
							break;
						case 125:
							vehicleLane = 4;
							break;
						case 600:
							vehicleLane = 5;
							break;
						case 1075:
							vehicleLane = 6;
						}
						if (vehicleLane == App->scene_stage->currentLane) {
							playerSpeed = 0;
							collision = true;
							App->audio->PlayFx(crashFX);
							break;
						}
						else {
							if (abs(int(vehicleLane - App->scene_stage->currentLane) < 2))
								App->audio->PlayFx(surpassCarFX);
						}
					}
					idx++;
					if (idx < size) {
						vElem = App->scene_stage->vehicles.at(idx);
					}
					else {
						break;
					}
				}
			}

			if (collision == true) {
				unsigned int idx = 0;
				VisualElement* vElem = App->scene_stage->vehicles.at(idx);
				while (idx < App->scene_stage->vehicles.size()) {
					if (int(vElem->world * 10) < int(carPosition * 10)) {
						unsigned int lane = 0;
						switch (vElem->x) {
						case -1300:
							lane = 1;
							break;
						case -825:
							lane = 2;
							break;
						case -350:
							lane = 3;
							break;
						case 125:
							lane = 4;
							break;
						case 600:
							lane = 5;
							break;
						case 1075:
							lane = 6;
						}
						if (vehicleLane == lane) {
							delete App->scene_stage->vehicles.at(idx);
							App->scene_stage->vehicles.erase(App->scene_stage->vehicles.begin() + idx);
							idx--;
						}
					}
					idx++;
					if (idx < App->scene_stage->vehicles.size()) {
						vElem = App->scene_stage->vehicles.at(idx);
					}
					else {
						break;
					}
				}
			}
		}

		position += playerSpeed;
		curveSpeed = playerSpeed * 10;
		break;
	case GameState::GAMEOVER:
		if (playerSpeed > 0.0f)
			playerSpeed -= ACCELERATION;

		if (playerSpeed < 0.0f)
			playerSpeed = 0.0f;

		position += playerSpeed;
		curveSpeed = playerSpeed * 10;
		break;
	case GameState::ENDING:
	case GameState::BROKEN:
		if (playerSpeed > 0.0f)
			playerSpeed -= ACCELERATION;

		if (playerSpeed < 0.0f)
			playerSpeed = 0.0f;

		position += playerSpeed;
		curveSpeed = playerSpeed * 10;

		if (App->renderer->camera.x > 0) {
			App->renderer->camera.x -= 12;
			if (App->renderer->camera.x < 0)
				App->renderer->camera.x = 0;
		}
		if (App->renderer->camera.x < 0) {
			App->renderer->camera.x += 12;
			if (App->renderer->camera.x > 0)
				App->renderer->camera.x = 0;
		}

		if (App->scene_stage->gameState == GameState::BROKEN) {
			if (App->renderer->camera.x == 0)
				App->scene_stage->gameState = GameState::PLAYING;
		}
		break;
	}

	carX = ((SCREEN_WIDTH - 92) / 2) - (App->renderer->camera.x / SCREEN_SIZE);
	//Draw car and player
	App->renderer->Blit(car, carX, SCREEN_HEIGHT - 48, &(currentCar->GetCurrentFrame()));
	if (playerSpeed > 0.0f) {
		App->renderer->Blit(car, ((SCREEN_WIDTH - 50 + playersDx) / 2) - (App->renderer->camera.x / SCREEN_SIZE), SCREEN_HEIGHT - 50, &(malePlayerMoving.GetCurrentFrame()));
		App->renderer->Blit(car, ((SCREEN_WIDTH + 6 + playersDx) / 2) - (App->renderer->camera.x / SCREEN_SIZE), SCREEN_HEIGHT - 48, &(femalePlayerMoving.GetCurrentFrame()));
	}
	else {
		App->renderer->Blit(car, ((SCREEN_WIDTH - 50 + playersDx) / 2) - (App->renderer->camera.x / SCREEN_SIZE), SCREEN_HEIGHT - 50, &(malePlayer.GetCurrentFrame()));
		App->renderer->Blit(car, ((SCREEN_WIDTH + 6 + playersDx) / 2) - (App->renderer->camera.x / SCREEN_SIZE), SCREEN_HEIGHT - 48, &(femalePlayer.GetCurrentFrame()));
	}

	//Draw out of road particles
	bool out = false;
	if (App->scene_stage->leftTireOut) {
		App->renderer->Blit(dust, (SCREEN_WIDTH / 2) - (App->renderer->camera.x / SCREEN_SIZE) - 75, SCREEN_HEIGHT - 35, &(leftDust.GetCurrentFrame()));
		out = true;
	}
	if (App->scene_stage->rigthTireOut) {
		App->renderer->Blit(dust, (SCREEN_WIDTH / 2) - (App->renderer->camera.x / SCREEN_SIZE), SCREEN_HEIGHT - 35, &(rightDust.GetCurrentFrame()));
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

	//Check if the player is too far to a side and reposition him
	if (App->renderer->camera.x > 2800 || App->renderer->camera.x < -3800) {
		App->scene_stage->gameState = GameState::BROKEN;
	}

	return UPDATE_CONTINUE;
}
