#ifndef __ModulePlayer_H__
#define __ModulePlayer_H__

#include "Module.h"
#include "Animation.h"

#define MAX_SPEED 0.2f
#define ACCELERATION 0.001f

struct SDL_Texture;

class ModulePlayer : public Module
{
public:
	ModulePlayer(bool active = true);
	~ModulePlayer();

	bool Start();
	update_status Update();
	bool CleanUp();

	bool keyPressed(int direction, int keysPressed);

public:
	SDL_Texture* car = nullptr;
	SDL_Texture* dustTex = nullptr;
	Animation* currentCar = nullptr;
	Animation idle;
	Animation idleUp;
	Animation forward;
	Animation forwardUp;
	Animation forwardDown;
	Animation idleLeft;
	Animation idleUpLeft;
	Animation left;
	Animation leftUp;
	Animation leftDown;
	Animation idleRight;
	Animation idleUpRight;
	Animation right;
	Animation rightUp;
	Animation rightDown;
	Animation breakCenter;
	Animation breakLeft;
	Animation breakRight;
	Animation breakUpLeft;
	Animation breakUp;
	Animation breakUpRight;
	Animation malePlayer;
	Animation femalePlayer;
	int playersDx = 0;
	Animation leftDust;
	Animation rightDust;

	float position = 0.0f;
	float playerSpeed = 0.0f;
	float curveSpeed = 0.0f;
};

#endif