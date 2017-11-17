#ifndef __ModulePlayer_H__
#define __ModulePlayer_H__

#include "Module.h"
#include "Animation.h"

struct SDL_Texture;

class ModulePlayer : public Module
{
public:
	ModulePlayer(bool active = true);
	~ModulePlayer();

	bool Start();
	update_status Update();
	bool CleanUp();

public:
	SDL_Texture* car = nullptr;
	Animation* currentCar = nullptr;
	Animation idle;
	Animation forward;
	Animation forwardUp;
	Animation forwardDown;
	Animation idleLeft;
	Animation left;
	Animation leftUp;
	Animation leftDown;
	Animation idleRight;
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
	float position = 0.0f;
	float playerSpeed = 0.01f;
	float curveSpeed = 0.0f;
};

#endif