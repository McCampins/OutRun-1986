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

	Segment s = { 0.0f, 0.0f, 0.0f };
	stageSegments.push_back(s);
	s = { 0.0f, 0.0f, (float)zMap.size() };
	stageSegments.push_back(s);
	s = { 0.0f, 1.0f, (float)zMap.size() };
	stageSegments.push_back(s);
	s = { 0.0f, -1.0f, (float)zMap.size() };
	stageSegments.push_back(s);
	s = { 0.0f, 0.0f, (float)zMap.size() };
	stageSegments.push_back(s);
	s = { 0.01f, 0.0f, (float)zMap.size() };
	stageSegments.push_back(s);
	s = { 0.0075f, 0.0f, (float)zMap.size() };
	stageSegments.push_back(s);
	s = { 0.0f, 0.0f, (float)zMap.size() };
	stageSegments.push_back(s);
	s = { -0.0075f, 0.0f, (float)zMap.size() };
	stageSegments.push_back(s);
	s = { 0.01f, 0.0f, (float)zMap.size() };
	stageSegments.push_back(s);

	bottomSegment = stageSegments.at(currentSegment);
	currentSegment++;
	topSegment = stageSegments.at(currentSegment);
	currentSegment++;

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
	int screenY = y;
	int minY = SCREEN_HEIGHT * SCREEN_SIZE - roadHeightScreen;
	int maxY = y;
	float scaleFactor;
	float z;

	float dX = 0;
	float ddX = 0;
	float dY = 0;

	for (unsigned int i = 0; i < zMap.size(); i++) {
		z = zMap.at(i);
		scaleFactor = (float)(y - minY) / (maxY - minY);
		scaleFactor = (scaleFactor * 0.95f) + 0.05f;

		if (i < topSegment.yMapPosition) {
			dX = bottomSegment.dX;
			dY = bottomSegment.dY;
		}
		else {
			dX = topSegment.dX;
			dY = topSegment.dY;
		}
		ddX += dX;
		x += ddX;

		float worldPosition = z + App->player->position;

		if ((int)(worldPosition * 15) % 2 == 0)
		{
			App->renderer->DrawHorizontalLine(x - (roadWidth * scaleFactor / 2) - (rumbleWidth * scaleFactor / 2) - (terrainWidth / 2), screenY, terrainWidth, 219, 209, 180, 255);
			App->renderer->DrawHorizontalLine(x - (roadWidth * scaleFactor / 2) - (rumbleWidth * scaleFactor / 2), screenY, rumbleWidth * scaleFactor, 255, 0, 0, 255);
			App->renderer->DrawHorizontalLine(x, screenY, roadWidth * scaleFactor, 105, 105, 105, 255);
			App->renderer->DrawHorizontalLine(x + (roadWidth  * scaleFactor / 2) + (rumbleWidth  * scaleFactor / 2), screenY, rumbleWidth * scaleFactor, 255, 0, 0, 255);
			App->renderer->DrawHorizontalLine(x + (roadWidth * scaleFactor / 2) + rumbleWidth * scaleFactor + (terrainWidth / 2), screenY, terrainWidth, 219, 209, 180, 255);
		}
		else
		{
			App->renderer->DrawHorizontalLine(x - (roadWidth * scaleFactor / 2) - (rumbleWidth * scaleFactor / 2) - (terrainWidth / 2), screenY, terrainWidth, 194, 178, 128, 255);
			App->renderer->DrawHorizontalLine(x - (roadWidth * scaleFactor / 2) - (rumbleWidth * scaleFactor / 2), screenY, rumbleWidth * scaleFactor, 255, 255, 255, 255);
			App->renderer->DrawHorizontalLine(x, screenY, roadWidth * scaleFactor, 115, 115, 115, 255);
			App->renderer->DrawHorizontalLine(x + (roadWidth * scaleFactor / 2) + (rumbleWidth  * scaleFactor / 2), screenY, rumbleWidth * scaleFactor, 255, 255, 255, 255);
			App->renderer->DrawHorizontalLine(x + (roadWidth * scaleFactor / 2) + rumbleWidth * scaleFactor + (terrainWidth / 2), screenY, terrainWidth, 194, 178, 128, 255);
		}

		if (dY < 0) {
			i++;
			DrawUphill(screenY, worldPosition, scaleFactor, x);
		}
		else if (dY > 0) {
			if (screenY < 650) {
				screenY = DrawUphill(screenY, worldPosition, scaleFactor, x);
				if (screenY < 500) {
					screenY = DrawUphill(screenY, worldPosition, scaleFactor, x);
				}
			}

		}

		screenY--;
		y--;
	}

	topSegment.yMapPosition -= App->player->speed;
	if (topSegment.yMapPosition < 0) {
		bottomSegment = topSegment;
		if (currentSegment < stageSegments.size() - 1) {
			topSegment = stageSegments.at(currentSegment);
			currentSegment++;
		}
		else {
			topSegment = { 0.0f, 0.0f, (float)zMap.size() };
		}
	}

	return UPDATE_CONTINUE;
}

int ModuleSceneStage::DrawUphill(int screenY, float worldPosition, float scaleFactor, float x) {
	screenY--;
	if ((int)(worldPosition * 15) % 2 == 0)
	{
		App->renderer->DrawHorizontalLine(x - (roadWidth * scaleFactor / 2) - (rumbleWidth * scaleFactor / 2) - (terrainWidth / 2), screenY, terrainWidth, 219, 209, 180, 255);
		App->renderer->DrawHorizontalLine(x - (roadWidth * scaleFactor / 2) - (rumbleWidth * scaleFactor / 2), screenY, rumbleWidth * scaleFactor, 255, 0, 0, 255);
		App->renderer->DrawHorizontalLine(x, screenY, roadWidth * scaleFactor, 105, 105, 105, 255);
		App->renderer->DrawHorizontalLine(x + (roadWidth  * scaleFactor / 2) + (rumbleWidth  * scaleFactor / 2), screenY, rumbleWidth * scaleFactor, 255, 0, 0, 255);
		App->renderer->DrawHorizontalLine(x + (roadWidth * scaleFactor / 2) + rumbleWidth * scaleFactor + (terrainWidth / 2), screenY, terrainWidth, 219, 209, 180, 255);
	}
	else
	{
		App->renderer->DrawHorizontalLine(x - (roadWidth * scaleFactor / 2) - (rumbleWidth * scaleFactor / 2) - (terrainWidth / 2), screenY, terrainWidth, 194, 178, 128, 255);
		App->renderer->DrawHorizontalLine(x - (roadWidth * scaleFactor / 2) - (rumbleWidth * scaleFactor / 2), screenY, rumbleWidth * scaleFactor, 255, 255, 255, 255);
		App->renderer->DrawHorizontalLine(x, screenY, roadWidth * scaleFactor, 115, 115, 115, 255);
		App->renderer->DrawHorizontalLine(x + (roadWidth * scaleFactor / 2) + (rumbleWidth  * scaleFactor / 2), screenY, rumbleWidth * scaleFactor, 255, 255, 255, 255);
		App->renderer->DrawHorizontalLine(x + (roadWidth * scaleFactor / 2) + rumbleWidth * scaleFactor + (terrainWidth / 2), screenY, terrainWidth, 194, 178, 128, 255);
	}
	return screenY;
}

int ModuleSceneStage::DrawDownhill(int screenY, float worldPosition, float scaleFactor, float x)
{
	screenY++;
	if ((int)(worldPosition * 10) % 2 == 0)
	{
		App->renderer->DrawHorizontalLine(x - (roadWidth * scaleFactor / 2) - (rumbleWidth * scaleFactor / 2) - (terrainWidth / 2), screenY, terrainWidth, 219, 209, 180, 255);
		App->renderer->DrawHorizontalLine(x - (roadWidth * scaleFactor / 2) - (rumbleWidth * scaleFactor / 2), screenY, rumbleWidth * scaleFactor, 255, 0, 0, 255);
		App->renderer->DrawHorizontalLine(x, screenY, roadWidth * scaleFactor, 105, 105, 105, 255);
		App->renderer->DrawHorizontalLine(x + (roadWidth  * scaleFactor / 2) + (rumbleWidth  * scaleFactor / 2), screenY, rumbleWidth * scaleFactor, 255, 0, 0, 255);
		App->renderer->DrawHorizontalLine(x + (roadWidth * scaleFactor / 2) + rumbleWidth * scaleFactor + (terrainWidth / 2), screenY, terrainWidth, 219, 209, 180, 255);
	}
	else
	{
		App->renderer->DrawHorizontalLine(x - (roadWidth * scaleFactor / 2) - (rumbleWidth * scaleFactor / 2) - (terrainWidth / 2), screenY, terrainWidth, 194, 178, 128, 255);
		App->renderer->DrawHorizontalLine(x - (roadWidth * scaleFactor / 2) - (rumbleWidth * scaleFactor / 2), screenY, rumbleWidth * scaleFactor, 255, 255, 255, 255);
		App->renderer->DrawHorizontalLine(x, screenY, roadWidth * scaleFactor, 115, 115, 115, 255);
		App->renderer->DrawHorizontalLine(x + (roadWidth * scaleFactor / 2) + (rumbleWidth  * scaleFactor / 2), screenY, rumbleWidth * scaleFactor, 255, 255, 255, 255);
		App->renderer->DrawHorizontalLine(x + (roadWidth * scaleFactor / 2) + rumbleWidth * scaleFactor + (terrainWidth / 2), screenY, terrainWidth, 194, 178, 128, 255);
	}
	return screenY;
}
