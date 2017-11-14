#include "Globals.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleFadeToBlack.h"
#include "ModuleSceneIntro.h"

ModuleSceneIntro::ModuleSceneIntro(bool active) : Module(active)
{}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{	
	return true;
}

// UnLoad assets
bool ModuleSceneIntro::CleanUp()
{
	
	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{
	return UPDATE_CONTINUE;
}