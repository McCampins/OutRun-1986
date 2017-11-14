#include "Globals.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleInput.h"
#include "ModuleParticles.h"
#include "ModuleRender.h"
#include "ModuleCollision.h"
#include "ModuleFadeToBlack.h"
#include "ModulePlayer.h"

ModulePlayer::ModulePlayer(bool active) : Module(active)
{
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	return true;
}

// Update: draw background
update_status ModulePlayer::Update()
{
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
	}

	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
	}

	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
	{
	}

	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
	{
		position += 0.01f;
	}

	return UPDATE_CONTINUE;
}
