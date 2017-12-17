#include "Globals.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleFadeToBlack.h"
#include "ModuleSceneIntro.h"

ModuleSceneIntro::ModuleSceneIntro(bool active) : Module(active)
{
	back = { 1, 315, 724, 455 };

	logo.frames.push_back({ 0, 0, 178, 88 });
	logo.frames.push_back({ 177, 0, 178, 88 });
	logo.frames.push_back({ 354, 0, 178, 88 });
	logo.frames.push_back({ 0, 89, 178, 88 });
	logo.frames.push_back({ 177, 89, 178, 88 });
	logo.frames.push_back({ 354, 89, 178, 88 });
	logo.speed = 0.1f;

	insertCoin.frames.push_back({ 0, 233, 420, 37 });
	insertCoin.frames.push_back({ 0, 275, 420, 5 });
	insertCoin.speed = 0.025f;
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{	
	LOG("Loading game intro");

	background = App->textures->Load("rtype/introScene.png");

	App->renderer->camera.x = App->renderer->camera.y = 0;
	
	return true;
}

// UnLoad assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading game intro");

	App->textures->Unload(background);

	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{
	App->renderer->Blit(background, 0, 0, &back, 1.0f, 0.53f);
	App->renderer->Blit(background, (SCREEN_WIDTH / 2) - 71, 179, &(insertCoin.GetCurrentFrame()), 1.0f, 0.31f);
	App->renderer->Blit(background, (SCREEN_WIDTH / 2) - 87, 30, &(logo.GetCurrentFrame()));

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && App->fade->isFading() == false)
	{
		App->fade->FadeToBlack((Module*)App->scene_stage, this);
	}

	return UPDATE_CONTINUE;
}