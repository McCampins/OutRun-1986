#include "Globals.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModuleRender.h"
#include "ModulePlayer.h"
#include "ModuleCollision.h"
#include "ModuleParticles.h"
#include "ModuleSceneStage.h"

// Reference at https://www.youtube.com/watch?v=OEhmUuehGOA

ModuleSceneStage::ModuleSceneStage(bool active) : Module(active)
{
}

ModuleSceneStage::~ModuleSceneStage()
{
}

// Load assets
bool ModuleSceneStage::Start()
{
	LOG("Loading space scene");

	App->player->Enable();
	App->particles->Enable();
	App->collision->Enable();

	for (int i = 0; i < roadHeightScreen; i++)
	{
		float z = (float)roadHeightWorld / (i - ((SCREEN_HEIGHT * SCREEN_SIZE) / 2));
		zMap.push_back(z);
	}
	/*
	for (int i = 0; i < 2; i++)
	{
		Line* l = new Line();
		l->z = (float)(i*segmentLength);

		roadLines.push_back(l);
	}
	*/

	return true;
}

// UnLoad assets
bool ModuleSceneStage::CleanUp()
{
	LOG("Unloading space scene");

	//App->textures->Unload(background);
	App->player->Disable();
	App->collision->Disable();
	App->particles->Disable();

	return true;
}

// Update: draw background
update_status ModuleSceneStage::Update()
{
	App->renderer->Blit(background, 0, 0, NULL);
	/*
	int linesSize = roadLines.size();

	for (int i = 0; i < 300; i++)
	{
		Line* l = nullptr;
		l = roadLines[i%linesSize];

		l->Project(0, 1500, 0);

		if ((i / 3) % 2 == 0)
		{
			sand = new SDL_Color();
			sand->r = 238;
			sand->g = 222;
			sand->b = 206;

			rumble = new SDL_Color();
			rumble->r = 255;
			rumble->g = 255;
			rumble->b = 255;

			road = new SDL_Color();
			road->r = 155;
			road->g = 155;
			road->b = 155;
		}
		else
		{
			sand = new SDL_Color();
			sand->r = 230;
			sand->g = 209;
			sand->b = 190;

			rumble = new SDL_Color();
			rumble->r = 105;
			rumble->g = 105;
			rumble->b = 105;

			road = new SDL_Color();
			road->r = 150;
			road->g = 150;
			road->b = 150;
		}

		int pi = i - 1 < 0 ? 0 : i - 1;
		Line* p = roadLines[(pi) % linesSize];

		SDL_Rect rect = { 0, 0, 50, 50 };
		App->renderer->DrawQuad(0, p->Y, roadWidth, 0, l->Y, roadWidth, sand->r, sand->g, sand->b, sand->a);
	}
	*/
	
	int y = SCREEN_HEIGHT * SCREEN_SIZE;
	int minY = SCREEN_HEIGHT * SCREEN_SIZE - roadHeightScreen;
	int maxY = y;
	float scaleFactor;
	float roadShrink, rumbleShrink, totalShrink;
	float z;

	for (vector<float>::const_iterator it = zMap.begin(); it != zMap.cend(); ++it)
	{
		z = *it;
		scaleFactor = (float) (y - minY) / (maxY - minY);
		roadShrink = roadWidth - (roadWidth * scaleFactor);
		rumbleShrink = rumbleWidth - (rumbleWidth * scaleFactor);
		totalShrink = roadShrink + rumbleShrink;

		if ((int) ((z + App->player->position) * 10) % 2 == 0)
		{
			App->renderer->DrawHorizontalLine((SCREEN_WIDTH * SCREEN_SIZE / 2) - (roadWidth / 2) - (rumbleWidth / 2) - (terrainWidth / 2) + (totalShrink / 2), y, terrainWidth, 219, 209, 180, 255);
			App->renderer->DrawHorizontalLine((SCREEN_WIDTH * SCREEN_SIZE / 2) - (roadWidth / 2) - (rumbleWidth / 2) + (totalShrink / 2), y, rumbleWidth * scaleFactor, 255, 0, 0, 255);
			App->renderer->DrawHorizontalLine(SCREEN_WIDTH * SCREEN_SIZE / 2, y, roadWidth * scaleFactor, 105, 105, 105, 255);
			App->renderer->DrawHorizontalLine((SCREEN_WIDTH * SCREEN_SIZE / 2) + (roadWidth / 2) + (rumbleWidth / 2) - (totalShrink / 2), y, rumbleWidth * scaleFactor, 255, 0, 0, 255);
			App->renderer->DrawHorizontalLine((SCREEN_WIDTH * SCREEN_SIZE / 2) + (roadWidth / 2) + (rumbleWidth / 2) + (terrainWidth / 2) - (totalShrink / 2), y, terrainWidth, 219, 209, 180, 255);
		}
		else
		{
			App->renderer->DrawHorizontalLine((SCREEN_WIDTH * SCREEN_SIZE / 2) - (roadWidth / 2) - (rumbleWidth / 2) - (terrainWidth / 2) + (totalShrink / 2), y, terrainWidth, 194, 178, 128, 255);
			App->renderer->DrawHorizontalLine((SCREEN_WIDTH * SCREEN_SIZE / 2) - (roadWidth / 2) - (rumbleWidth / 2) + (totalShrink / 2), y, rumbleWidth * scaleFactor, 255, 255, 255, 255);
			App->renderer->DrawHorizontalLine(SCREEN_WIDTH * SCREEN_SIZE / 2, y, roadWidth * scaleFactor, 115, 115, 115, 255);
			App->renderer->DrawHorizontalLine((SCREEN_WIDTH * SCREEN_SIZE / 2) + (roadWidth / 2) + (rumbleWidth / 2) - (totalShrink / 2), y, rumbleWidth * scaleFactor, 255, 255, 255, 255);
			App->renderer->DrawHorizontalLine((SCREEN_WIDTH * SCREEN_SIZE / 2) + (roadWidth / 2) + (rumbleWidth / 2) + (terrainWidth / 2) - (totalShrink / 2), y, terrainWidth, 194, 178, 128, 255);
		}
		y--;
	}

	return UPDATE_CONTINUE;
}