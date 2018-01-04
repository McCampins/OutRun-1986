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
	breeze = { 0, 0, 651, 407 };
	magical = { 0, 407, 651, 407 };
	splash = { 0, 814, 651, 407 };
}

ModuleSceneMusic::~ModuleSceneMusic()
{
}

bool ModuleSceneMusic::Start()
{
	LOG("Loading select music scene");

	background = App->textures->Load("rtype/musicScene.png");

	magicalSound = "rtype/Music/MagicalSoundShower.mp3";
	passingBreeze = "rtype/Music/PassingBreeze.mp3";
	splashWave = "rtype/Music/SplashWave.mp3";

	App->renderer->camera.x = App->renderer->camera.y = 0;

	musicPlaying = -1;

	currentScreen = 1;

	return true;
}

update_status ModuleSceneMusic::Update()
{
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN)
	{
		if (currentScreen > 0)
			currentScreen--;
	}

	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN)
	{
		if (currentScreen < 2)
			currentScreen++;
	}

	switch (currentScreen) {
	case 0:
		App->renderer->Blit(background, 0, 0, &breeze, 1.0f, 0.59f);
		if (musicPlaying != currentScreen && mute == false) {
			App->audio->PlayMusic(passingBreeze, 0.0f);
			musicPlaying = 0;
		}

		break;
	case 1:
		App->renderer->Blit(background, 0, 0, &magical, 1.0f, 0.59f);
		if (musicPlaying != currentScreen && mute == false) {
			App->audio->PlayMusic(magicalSound, 0.0f);
			musicPlaying = 1;
		}
		break;
	case 2:
		App->renderer->Blit(background, 0, 0, &splash, 1.0f, 0.59f);
		if (musicPlaying != currentScreen && mute == false) {
			App->audio->PlayMusic(splashWave, 0.0f);
			musicPlaying = 2;
		}
		break;
	}

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && App->fade->isFading() == false)
	{
		App->fade->FadeToBlack((Module*)App->scene_stage, this);
	}

	if (App->input->GetKey(SDL_SCANCODE_M) == KEY_DOWN)
	{
		App->audio->StopMusic();
		mute = !mute;
		//If unmute
		if (mute == false) {
			switch (musicPlaying) {
			case 0:
				App->audio->PlayMusic(passingBreeze, 0.0f);
				break;
			case 1:
				App->audio->PlayMusic(magicalSound, 0.0f);
				break;
			case 2:
				App->audio->PlayMusic(splashWave, 0.0f);
				break;
		}
		}
	}

	return UPDATE_CONTINUE;
}

bool ModuleSceneMusic::CleanUp()
{
	LOG("Unloading select music scene");

	App->textures->Unload(background);
	background = nullptr;

	magicalSound = nullptr;
	passingBreeze = nullptr;
	splashWave = nullptr;

	return true;
}
