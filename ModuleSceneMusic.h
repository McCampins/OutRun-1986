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
	Animation radioFrequency;
	Animation arm;
};

#endif // __MODULESCENEMUSIC_H__
