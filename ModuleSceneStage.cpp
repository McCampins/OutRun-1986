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

	bottomSegment = { 0, 0 };
	topSegment = { 0.075f, zMap.back() };

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
	float x = SCREEN_WIDTH * SCREEN_SIZE / 2;
	int y = SCREEN_HEIGHT * SCREEN_SIZE;
	int minY = SCREEN_HEIGHT * SCREEN_SIZE - roadHeightScreen;
	int maxY = y;
	float scaleFactor;
	float roadShrink, rumbleShrink, totalShrink;
	float z;

	float dX = 0;
	float ddX = 0;
	float curveFactor = 0.1f;

	for (vector<float>::const_iterator it = zMap.begin(); it != zMap.cend(); ++it)
	{
		z = *it;
		scaleFactor = (float)(y - minY) / (maxY - minY);
		roadShrink = roadWidth - (roadWidth * scaleFactor);
		rumbleShrink = rumbleWidth - (rumbleWidth * scaleFactor);
		totalShrink = roadShrink + rumbleShrink;

		if (z < topSegment.zMapPosition) {
			dX = bottomSegment.dX;
		}
		else {
			dX = topSegment.dX;
		}
		ddX += dX * curveFactor;
		x += ddX;

		float worldPosition = z + App->player->position;

		if ((int)(worldPosition * 10) % 2 == 0)
		{
			App->renderer->DrawHorizontalLine(x - (roadWidth / 2) - (rumbleWidth / 2) - (terrainWidth / 2) + (totalShrink / 2), y, terrainWidth, 219, 209, 180, 255);
			App->renderer->DrawHorizontalLine(x - (roadWidth / 2) - (rumbleWidth / 2) + (totalShrink / 2), y, rumbleWidth * scaleFactor, 255, 0, 0, 255);
			App->renderer->DrawHorizontalLine(x, y, roadWidth * scaleFactor, 105, 105, 105, 255);
			App->renderer->DrawHorizontalLine(x + (roadWidth / 2) + (rumbleWidth / 2) - (totalShrink / 2), y, rumbleWidth * scaleFactor, 255, 0, 0, 255);
			App->renderer->DrawHorizontalLine(x + (roadWidth / 2) + (rumbleWidth / 2) + (terrainWidth / 2) - (totalShrink / 2), y, terrainWidth, 219, 209, 180, 255);
		}
		else
		{
			App->renderer->DrawHorizontalLine(x - (roadWidth / 2) - (rumbleWidth / 2) - (terrainWidth / 2) + (totalShrink / 2), y, terrainWidth, 194, 178, 128, 255);
			App->renderer->DrawHorizontalLine(x - (roadWidth / 2) - (rumbleWidth / 2) + (totalShrink / 2), y, rumbleWidth * scaleFactor, 255, 255, 255, 255);
			App->renderer->DrawHorizontalLine(x, y, roadWidth * scaleFactor, 115, 115, 115, 255);
			App->renderer->DrawHorizontalLine(x + (roadWidth / 2) + (rumbleWidth / 2) - (totalShrink / 2), y, rumbleWidth * scaleFactor, 255, 255, 255, 255);
			App->renderer->DrawHorizontalLine(x + (roadWidth / 2) + (rumbleWidth / 2) + (terrainWidth / 2) - (totalShrink / 2), y, terrainWidth, 194, 178, 128, 255);
		}
		y--;
	}

	topSegment.zMapPosition -= App->player->speed;
	if (topSegment.zMapPosition < 0) {
		bottomSegment = topSegment;
		topSegment.zMapPosition = zMap.back();
		topSegment.dX = 0;
	}

	return UPDATE_CONTINUE;
}