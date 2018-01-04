#ifndef __MODULESCENEMUSIC_H__
#define __MODULESCENEMUSIC_H__

#include "Module.h"
#include "Animation.h"

struct SDL_Texture;

class ModuleSceneMusic : public Module
{
public:
	ModuleSceneMusic(bool active = true);
	~ModuleSceneMusic();

	bool Start();
	update_status Update();
	bool CleanUp();

public:
	SDL_Texture* background = nullptr;
	SDL_Rect splash, breeze, magical;
	int musicPlaying;
	const char* splashWave = nullptr;
	const char* passingBreeze = nullptr;
	const char* magicalSound = nullptr;
	unsigned int currentScreen;
	bool mute = false;
};

#endif // __MODULESCENEMUSIC_H__
