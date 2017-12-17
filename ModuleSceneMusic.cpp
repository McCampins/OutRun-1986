#include "Globals.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleFadeToBlack.h"
#include "ModuleSceneMusic.h"

ModuleSceneMusic::ModuleSceneMusic(bool active) : Module(active)
{
}

ModuleSceneMusic::~ModuleSceneMusic()
{
}

bool ModuleSceneMusic::Start()
{
	LOG("Loading select music scene");

	background = App->textures->Load("rtype/musicScene.png");

	App->renderer->camera.x = App->renderer->camera.y = 0;

	return true;
}

update_status ModuleSceneMusic::Update()
{
	App->renderer->Blit(background, 0, 0, NULL, 1.0f, 0.1f);
	//App->renderer->Blit(background, (SCREEN_WIDTH / 2) - 71, 179, &(insertCoin.GetCurrentFrame()), 1.0f, 0.31f);
	//App->renderer->Blit(background, (SCREEN_WIDTH / 2) - 87, 30, &(logo.GetCurrentFrame()));

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && App->fade->isFading() == false)
	{
		App->fade->FadeToBlack((Module*)App->scene_stage, this);
	}

	return UPDATE_CONTINUE;
}

bool ModuleSceneMusic::CleanUp()
{
	LOG("Unloading select music scene");

	App->textures->Unload(background);

	return true;
}
