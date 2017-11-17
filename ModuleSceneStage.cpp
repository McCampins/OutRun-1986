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
	LOG("Loading stage scene");

	//background = App->textures->Load("rtype/background.png");

	App->player->Enable();
	App->particles->Enable();
	App->collision->Enable();

	for (int i = 0; i < roadHeightScreen; i++)
	{
		float z = (float)roadHeightWorld / (i - ((SCREEN_HEIGHT * SCREEN_SIZE) / 2));
		zMap.push_back(z);
	}

	//STAGE 1
	Segment s = { 0.0f, 0.0f, 900.0f, 0.0f, CameraMove::LEFTTOLEFT, Inclination::CENTER };
	stageSegments.push_back(s);
	s = { 0.0f, 0.0f, 800.0f, (float)zMap.size(), CameraMove::LEFTTOLEFT, Inclination::CENTER };
	stageSegments.push_back(s);
	s = { 0.0f, UPHILL, 800.0f, (float)zMap.size(), CameraMove::LEFTTOLEFT, Inclination::UP };
	stageSegments.push_back(s);
	s = { 0.0f, DOWNHILL, 400.0f, (float)zMap.size(), CameraMove::LEFTTOLEFT, Inclination::DOWN };
	stageSegments.push_back(s);
	s = { SOFTLEFTCURVE, 0.0f, 400.0f, (float)zMap.size(), CameraMove::LEFTTOLEFT, Inclination::CENTER };
	stageSegments.push_back(s);
	s = { SOFTLEFTCURVE, 0.0f, 400.0f, (float)zMap.size(), CameraMove::LEFTTOLEFT, Inclination::CENTER };
	stageSegments.push_back(s);
	s = { 0.0f, 0.0f, 400.0f, (float)zMap.size(), CameraMove::LEFTTOLEFT, Inclination::CENTER };
	stageSegments.push_back(s);
	s = { SOFTRIGHTCURVE, DOWNHILL, 300.0f, (float)zMap.size(), CameraMove::LEFTTOLEFT, Inclination::DOWN };
	stageSegments.push_back(s);
	s = { 0.0f, DOWNHILL, 100.0f, (float)zMap.size(), CameraMove::LEFTTOCENTER, Inclination::DOWN };
	stageSegments.push_back(s);
	s = { 0.0f, 0.0f, 0.0f, (float)zMap.size(), CameraMove::CENTERTOCENTER, Inclination::CENTER };
	stageSegments.push_back(s);
	s = { 0.0f, 0.0f, 0.0f, (float)zMap.size(), CameraMove::CENTERTOCENTER, Inclination::CENTER };
	stageSegments.push_back(s);
	s = { SOFTLEFTCURVE, 0.0f, 0.0f, (float)zMap.size(), CameraMove::CENTERTOCENTER, Inclination::CENTER };
	stageSegments.push_back(s);
	s = { SOFTLEFTCURVE, 0.0f, 0.0f, (float)zMap.size(), CameraMove::CENTERTOCENTER, Inclination::CENTER };
	stageSegments.push_back(s);
	s = { 0.0f, 0.0f, 0.0f, (float)zMap.size(), CameraMove::CENTERTOCENTER, Inclination::CENTER };
	stageSegments.push_back(s);
	s = { 0.0f, 0.0f, 0.0f, (float)zMap.size(), CameraMove::CENTERTOCENTER, Inclination::CENTER };
	stageSegments.push_back(s);
	s = { 0.0f, 0.0f, 0.0f, (float)zMap.size(), CameraMove::CENTERTOCENTER, Inclination::CENTER };
	stageSegments.push_back(s);
	s = { HARDLEFTCURVE, 0.0f, 0.0f, (float)zMap.size(), CameraMove::CENTERTOCENTER, Inclination::CENTER };
	stageSegments.push_back(s);
	s = { HARDLEFTCURVE, 0.0f, 0.0f, (float)zMap.size(), CameraMove::CENTERTOCENTER, Inclination::CENTER };
	stageSegments.push_back(s);
	s = { HARDLEFTCURVE, 0.0f, 0.0f, (float)zMap.size(), CameraMove::CENTERTOCENTER, Inclination::CENTER };
	stageSegments.push_back(s);
	s = { SOFTRIGHTCURVE, 0.0f, 0.0f, (float)zMap.size(), CameraMove::CENTERTOCENTER, Inclination::CENTER };
	stageSegments.push_back(s);
	s = { 0.0f, 0.0f, 0.0f, (float)zMap.size(), CameraMove::CENTERTOCENTER, Inclination::CENTER };
	stageSegments.push_back(s);
	s = { HARDLEFTCURVE, 0.0f, 0.0f, (float)zMap.size(), CameraMove::CENTERTOCENTER, Inclination::CENTER };
	stageSegments.push_back(s);
	s = { HARDLEFTCURVE, UPHILL, 0.0f, (float)zMap.size(), CameraMove::CENTERTOCENTER, Inclination::UP };
	stageSegments.push_back(s);
	s = { HARDLEFTCURVE, DOWNHILL, 0.0f, (float)zMap.size(), CameraMove::CENTERTOCENTER, Inclination::DOWN };
	stageSegments.push_back(s);
	s = { 0.0f, 0.0f, 0.0f, (float)zMap.size(), CameraMove::CENTERTOCENTER, Inclination::CENTER };
	stageSegments.push_back(s);
	s = { 0.0f, 0.0f, 0.0f, (float)zMap.size(), CameraMove::CENTERTOCENTER, Inclination::CENTER };
	stageSegments.push_back(s);
	s = { 0.0f, 0.0f, 0.0f, (float)zMap.size(), CameraMove::CENTERTOCENTER, Inclination::CENTER };
	stageSegments.push_back(s);
	s = { 0.0f, 0.0f, 0.0f, (float)zMap.size(), CameraMove::CENTERTOCENTER, Inclination::CENTER };
	stageSegments.push_back(s);
	s = { 0.0f, 0.0f, 0.0f, (float)zMap.size(), CameraMove::CENTERTOCENTER, Inclination::CENTER };
	stageSegments.push_back(s);
	s = { HARDLEFTCURVE, 0.0f, 0.0f, (float)zMap.size(), CameraMove::CENTERTOCENTER, Inclination::CENTER };
	stageSegments.push_back(s);
	s = { HARDLEFTCURVE, 0.0f, 0.0f, (float)zMap.size(), CameraMove::CENTERTOCENTER, Inclination::CENTER };
	stageSegments.push_back(s);
	s = { HARDLEFTCURVE, 0.0f, 0.0f, (float)zMap.size(), CameraMove::CENTERTOCENTER, Inclination::CENTER };
	stageSegments.push_back(s);
	s = { 0.0f, 0.0f, 0.0f, (float)zMap.size(), CameraMove::CENTERTOCENTER, Inclination::CENTER };
	stageSegments.push_back(s);
	s = { 0.0f, 0.0f, 0.0f, (float)zMap.size(), CameraMove::CENTERTOCENTER, Inclination::CENTER };
	stageSegments.push_back(s);
	s = { HARDLEFTCURVE, 0.0f, 0.0f, (float)zMap.size(), CameraMove::CENTERTOCENTER, Inclination::CENTER };
	stageSegments.push_back(s);
	s = { HARDLEFTCURVE, 0.0f, 0.0f, (float)zMap.size(), CameraMove::CENTERTOCENTER, Inclination::CENTER };
	stageSegments.push_back(s);
	s = { HARDRIGHTCURVE, 0.0f, 0.0f, (float)zMap.size(), CameraMove::CENTERTOCENTER, Inclination::CENTER };
	stageSegments.push_back(s);
	s = { HARDRIGHTCURVE, 0.0f, 0.0f, (float)zMap.size(), CameraMove::CENTERTOCENTER, Inclination::CENTER };
	stageSegments.push_back(s);
	s = { HARDRIGHTCURVE, 0.0f, 0.0f, (float)zMap.size(), CameraMove::CENTERTOCENTER, Inclination::CENTER };
	stageSegments.push_back(s);
	s = { HARDLEFTCURVE, 0.0f, 0.0f, (float)zMap.size(), CameraMove::CENTERTOCENTER, Inclination::CENTER };
	stageSegments.push_back(s);
	s = { 0.0f, 0.0f, 0.0f, (float)zMap.size(), CameraMove::CENTERTOCENTER, Inclination::CENTER };
	stageSegments.push_back(s);
	s = { 0.0f, 0.0f, 0.0f, (float)zMap.size(), CameraMove::CENTERTOCENTER, Inclination::CENTER };
	stageSegments.push_back(s);
	s = { 0.0f, 0.0f, 0.0f, (float)zMap.size(), CameraMove::CENTERTOCENTER, Inclination::CENTER };
	stageSegments.push_back(s);
	s = { 0.0f, 0.0f, 0.0f, (float)zMap.size(), CameraMove::CENTERTOCENTER, Inclination::CENTER };
	stageSegments.push_back(s);
	s = { 0.0f, 0.0f, 0.0f, (float)zMap.size(), CameraMove::CENTERTOCENTER, Inclination::CENTER };
	stageSegments.push_back(s);
	s = { 0.0f, 0.0f, 0.0f, (float)zMap.size(), CameraMove::CENTERTOCENTER, Inclination::CENTER };
	stageSegments.push_back(s);
	s = { 0.0f, 0.0f, 0.0f, (float)zMap.size(), CameraMove::CENTERTOCENTER, Inclination::CENTER };
	stageSegments.push_back(s);
	s = { 0.0f, UPHILL, 0.0f, (float)zMap.size(), CameraMove::CENTERTOCENTER, Inclination::UP };
	stageSegments.push_back(s);
	s = { 0.0f, DOWNHILL, 0.0f, (float)zMap.size(), CameraMove::CENTERTOCENTER, Inclination::DOWN };
	stageSegments.push_back(s);
	s = { 0.0f, 0.0f, 0.0f, (float)zMap.size(), CameraMove::CENTERTOCENTER, Inclination::CENTER };
	stageSegments.push_back(s);
	s = { 0.0f, UPHILL, 0.0f, (float)zMap.size(), CameraMove::CENTERTOCENTER, Inclination::UP };
	stageSegments.push_back(s);
	s = { 0.0f, UPHILL, 0.0f, (float)zMap.size(), CameraMove::CENTERTOCENTER, Inclination::UP };
	stageSegments.push_back(s);
	s = { 0.0f, 0.0f, 0.0f, (float)zMap.size(), CameraMove::CENTERTOCENTER, Inclination::CENTER };
	stageSegments.push_back(s);
	s = { 0.0f, 0.0f, 0.0f, (float)zMap.size(), CameraMove::CENTERTOCENTER, Inclination::CENTER };
	stageSegments.push_back(s);
	//END OF STAGE 1

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
	//Road
	float x = SCREEN_WIDTH * SCREEN_SIZE / 2;
	float centerScreen = x;
	int y = SCREEN_HEIGHT * SCREEN_SIZE;
	int screenY = y;
	int minY = SCREEN_HEIGHT * SCREEN_SIZE - roadHeightScreen;
	int maxY = y;
	float scaleFactor;
	float z;

	float dX = 0;
	float ddX = 0;
	float dY = 0;

	float initialRoadSeparation = bottomSegment.roadSeparation;
	float finalRoadSeparation = topSegment.roadSeparation;
	float separationInterval = abs(finalRoadSeparation - initialRoadSeparation);
	float roadSeparation;

	if (topSegment.yMapPosition < 1.0f) {
		topSegment.yMapPosition = 0.0f;
	}
	float segmentFactor = topSegment.yMapPosition / zMap.size();
	float axisModifier;
	float previousAxis = 0;

	for (unsigned int i = 0; i < zMap.size(); i++) {
		z = zMap.at(i);
		scaleFactor = (float)(y - minY) / (maxY - minY);
		scaleFactor = (scaleFactor * 0.95f) + 0.05f;

		switch (bottomSegment.pos) {
		case CameraMove::LEFTTOLEFT:
			axisModifier = (-(roadWidth * scaleFactor * 1.5f) - (lineWidth * scaleFactor * 1.5f));
			break;
		case CameraMove::LEFTTOCENTER:
			axisModifier = (-(roadWidth * scaleFactor * 1.5f) - (lineWidth * scaleFactor * 1.5f)) * segmentFactor;
			break;
		case CameraMove::CENTERTOCENTER:
			axisModifier = 0;
			break;
		case CameraMove::CENTERTOLEFT:
			axisModifier = (-(roadWidth * scaleFactor * 1.5f) - (lineWidth * scaleFactor * 1.5f)) * -(1 - segmentFactor);
			break;
		case CameraMove::CENTERTORIGHT:
			axisModifier = ((roadWidth * scaleFactor * 1.5f) + (lineWidth * scaleFactor * 1.5f)) * -(1 - segmentFactor);
			break;
		case CameraMove::RIGHTTORIGHT:
			axisModifier = ((roadWidth * scaleFactor * 1.5f) + (lineWidth * scaleFactor * 1.5f));
			break;
		case CameraMove::RIGHTTOCENTER:
			axisModifier = ((roadWidth * scaleFactor * 1.5f) + (lineWidth * scaleFactor * 1.5f)) * segmentFactor;
			break;
		}
		x = x + previousAxis - axisModifier;
		previousAxis = axisModifier;

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

		roadSeparation = initialRoadSeparation - (separationInterval * -(-1 + segmentFactor));

		float worldPosition = z + App->player->position;

		if (dY < 0) {
			if (y > 575
				) {
				screenY = DrawRoads(screenY, worldPosition, scaleFactor, x, roadSeparation);
				screenY = DrawRoads(screenY, worldPosition, scaleFactor, x, roadSeparation);
				y--;
			}
		}
		else if (dY > 0) {
			screenY = DrawRoads(screenY, worldPosition, scaleFactor, x, roadSeparation);
			if (screenY < 650) {
				screenY = DrawRoads(screenY, worldPosition, scaleFactor, x, roadSeparation);
				if (screenY < 500) {
					screenY = DrawRoads(screenY, worldPosition, scaleFactor, x, roadSeparation);
				}
			}
			y--;
		}
		else {
			screenY = DrawRoads(screenY, worldPosition, scaleFactor, x, roadSeparation);
			y--;
		}
	}

	topSegment.yMapPosition -= App->player->curveSpeed;
	if (topSegment.yMapPosition < 0) {
		bottomSegment = topSegment;
		if (currentSegment < stageSegments.size()) {
			topSegment = stageSegments.at(currentSegment);
			currentSegment++;
		}
		else {
			topSegment = { 0.0f, 0.0f, 0.0f, (float)zMap.size(), CameraMove::CENTERTOCENTER, Inclination::CENTER };
		}
	}



	return UPDATE_CONTINUE;
}

int ModuleSceneStage::DrawRoads(int screenY, float worldPosition, float scaleFactor, float x, float roadSeparation) {
	screenY--;
	if ((int)(worldPosition * 20) % 2 == 0)
	{
		//Terrain
		App->renderer->DrawHorizontalLine(x + App->renderer->camera.x * scaleFactor * scaleFactor, screenY, terrainWidth, 219, 209, 180, 255);
		//Rumble
		App->renderer->DrawHorizontalLine(x - (roadWidth * scaleFactor * 3) - (rumbleWidth * scaleFactor / 2) - (lineWidth * scaleFactor * 3.5f) + App->renderer->camera.x * scaleFactor, screenY, rumbleWidth * scaleFactor, 161, 160, 161, 255);
		//1st Road Line
		App->renderer->DrawHorizontalLine(x - (roadWidth * scaleFactor * 3) - (lineWidth * scaleFactor * 3) + App->renderer->camera.x * scaleFactor, screenY, lineWidth * scaleFactor, 255, 255, 255, 255);
		//1st Road
		App->renderer->DrawHorizontalLine(x - (roadWidth * scaleFactor * 2.5f) - (lineWidth * scaleFactor * 2.5f) + App->renderer->camera.x * scaleFactor, screenY, roadWidth * scaleFactor, 161, 160, 161, 255);
		//1st Road - 2nd Road Line
		App->renderer->DrawHorizontalLine(x - (roadWidth * scaleFactor * 2) - (lineWidth * scaleFactor * 2) + App->renderer->camera.x * scaleFactor, screenY, lineWidth * scaleFactor, 255, 255, 255, 255);
		//2nd Road
		App->renderer->DrawHorizontalLine(x - (roadWidth * scaleFactor * 1.5f) - (lineWidth * scaleFactor * 1.5f) + App->renderer->camera.x * scaleFactor, screenY, roadWidth * scaleFactor, 161, 160, 161, 255);
		float test = -(roadWidth * scaleFactor * 1.5f) - (lineWidth * scaleFactor * 1.5f);
		//2nd Road - 3rd Road Line
		App->renderer->DrawHorizontalLine(x - (roadWidth * scaleFactor) - (lineWidth * scaleFactor) + App->renderer->camera.x * scaleFactor, screenY, lineWidth * scaleFactor, 255, 255, 255, 255);
		//Rigth Road Rumble (before 3rd Road in case both roads intersect)
		App->renderer->DrawHorizontalLine(x - (rumbleWidth * scaleFactor / 2) - (lineWidth * scaleFactor / 2) + App->renderer->camera.x * scaleFactor + (roadSeparation * scaleFactor), screenY, rumbleWidth * scaleFactor, 161, 160, 161, 255);
		//3rd Road
		App->renderer->DrawHorizontalLine(x - (roadWidth * scaleFactor / 2) - (lineWidth * scaleFactor / 2) + App->renderer->camera.x * scaleFactor, screenY, roadWidth * scaleFactor, 161, 160, 161, 255);
		//3rd Road Line
		App->renderer->DrawHorizontalLine(x + App->renderer->camera.x * scaleFactor, screenY, lineWidth * scaleFactor, 255, 255, 255, 255);
		//4th Road Line (same coordinates as they might intersect)
		App->renderer->DrawHorizontalLine(x + App->renderer->camera.x * scaleFactor + (roadSeparation * scaleFactor), screenY, lineWidth * scaleFactor, 255, 255, 255, 255);
		//Left Road Rumble (before 4th Road in case both roads intersect)
		App->renderer->DrawHorizontalLine(x + (rumbleWidth * scaleFactor / 2) + (lineWidth * scaleFactor / 2) + App->renderer->camera.x * scaleFactor, screenY, rumbleWidth * scaleFactor, 161, 160, 161, 255);
		//4th Road
		App->renderer->DrawHorizontalLine(x + (roadWidth * scaleFactor / 2) + (lineWidth * scaleFactor / 2) + App->renderer->camera.x * scaleFactor + (roadSeparation * scaleFactor), screenY, roadWidth * scaleFactor, 161, 160, 161, 255);
		//4th Road - 5th Road Line
		App->renderer->DrawHorizontalLine(x + (roadWidth * scaleFactor) + (lineWidth * scaleFactor) + App->renderer->camera.x * scaleFactor + (roadSeparation * scaleFactor), screenY, lineWidth * scaleFactor, 255, 255, 255, 255);
		//5th Road
		App->renderer->DrawHorizontalLine(x + (roadWidth * scaleFactor * 1.5f) + (lineWidth * scaleFactor * 1.5f) + App->renderer->camera.x * scaleFactor + (roadSeparation * scaleFactor), screenY, roadWidth * scaleFactor, 161, 160, 161, 255);
		//5th Road - 6th Road Line
		App->renderer->DrawHorizontalLine(x + (roadWidth * scaleFactor * 2) + (lineWidth * scaleFactor * 2) + App->renderer->camera.x * scaleFactor + (roadSeparation * scaleFactor), screenY, lineWidth * scaleFactor, 255, 255, 255, 255);
		//6th Road
		App->renderer->DrawHorizontalLine(x + (roadWidth * scaleFactor * 2.5f) + (lineWidth * scaleFactor * 2.5f) + App->renderer->camera.x * scaleFactor + (roadSeparation * scaleFactor), screenY, roadWidth * scaleFactor, 161, 160, 161, 255);
		//6th Road Line
		App->renderer->DrawHorizontalLine(x + (roadWidth * scaleFactor * 3) + (lineWidth * scaleFactor * 3) + App->renderer->camera.x * scaleFactor + (roadSeparation * scaleFactor), screenY, lineWidth * scaleFactor, 255, 255, 255, 255);
		//Rumble
		App->renderer->DrawHorizontalLine(x + (roadWidth * scaleFactor * 3) + (rumbleWidth * scaleFactor / 2) + (lineWidth *  (roadSeparation * scaleFactor) * 3.5f) + App->renderer->camera.x * scaleFactor + roadSeparation, screenY, rumbleWidth * scaleFactor, 161, 160, 161, 255);
	}
	else
	{
		//Terrain
		App->renderer->DrawHorizontalLine(x + App->renderer->camera.x * scaleFactor, screenY, terrainWidth, 194, 178, 128, 255);
		//Rumble
		App->renderer->DrawHorizontalLine(x - (roadWidth * scaleFactor * 3) - (rumbleWidth * scaleFactor / 2) - (lineWidth * scaleFactor * 3.5f) + App->renderer->camera.x * scaleFactor, screenY, rumbleWidth * scaleFactor, 255, 255, 255, 255);
		//1st Road Line
		App->renderer->DrawHorizontalLine(x - (roadWidth * scaleFactor * 3) - (lineWidth * scaleFactor * 3) + App->renderer->camera.x * scaleFactor, screenY, lineWidth * scaleFactor, 170, 170, 170, 255);
		//1st Road
		App->renderer->DrawHorizontalLine(x - (roadWidth * scaleFactor * 2.5f) - (lineWidth * scaleFactor * 2.5f) + App->renderer->camera.x * scaleFactor, screenY, roadWidth * scaleFactor, 170, 170, 170, 255);
		//1st Road - 2nd Road Line
		App->renderer->DrawHorizontalLine(x - (roadWidth * scaleFactor * 2) - (lineWidth * scaleFactor * 2) + App->renderer->camera.x * scaleFactor, screenY, lineWidth * scaleFactor, 170, 170, 170, 255);
		//2nd Road
		App->renderer->DrawHorizontalLine(x - (roadWidth * scaleFactor * 1.5f) - (lineWidth * scaleFactor * 1.5f) + App->renderer->camera.x * scaleFactor, screenY, roadWidth * scaleFactor, 170, 170, 170, 255);
		//2nd Road - 3rd Road Line
		App->renderer->DrawHorizontalLine(x - (roadWidth * scaleFactor) - (lineWidth * scaleFactor) + App->renderer->camera.x * scaleFactor, screenY, lineWidth * scaleFactor, 170, 170, 170, 255);
		//Rigth Road Rumble (before 3rd Road in case both roads intersect)
		App->renderer->DrawHorizontalLine(x - (rumbleWidth * scaleFactor / 2) - (lineWidth * scaleFactor / 2) + App->renderer->camera.x * scaleFactor + (roadSeparation * scaleFactor), screenY, rumbleWidth * scaleFactor, 255, 255, 255, 255);
		//3rd Road
		App->renderer->DrawHorizontalLine(x - (roadWidth * scaleFactor / 2) - (lineWidth * scaleFactor / 2) + App->renderer->camera.x * scaleFactor, screenY, roadWidth * scaleFactor, 170, 170, 170, 255);
		//3rd Road Line
		App->renderer->DrawHorizontalLine(x + App->renderer->camera.x * scaleFactor, screenY, lineWidth * scaleFactor, 170, 170, 170, 255);
		//4th Road Line (same coordinates as they might intersect)
		App->renderer->DrawHorizontalLine(x + App->renderer->camera.x * scaleFactor + (roadSeparation * scaleFactor), screenY, lineWidth * scaleFactor, 170, 170, 170, 255);
		//Left Road Rumble (before 4th Road in case they intersect)
		App->renderer->DrawHorizontalLine(x + (rumbleWidth * scaleFactor / 2) + (lineWidth * scaleFactor / 2) + App->renderer->camera.x * scaleFactor, screenY, rumbleWidth * scaleFactor, 255, 255, 255, 255);
		//4th Road
		App->renderer->DrawHorizontalLine(x + (roadWidth * scaleFactor / 2) + (lineWidth * scaleFactor / 2) + App->renderer->camera.x * scaleFactor + (roadSeparation * scaleFactor), screenY, roadWidth * scaleFactor, 170, 170, 170, 255);
		//4th Road - 5th Road Line
		App->renderer->DrawHorizontalLine(x + (roadWidth * scaleFactor) + (lineWidth * scaleFactor) + App->renderer->camera.x * scaleFactor + (roadSeparation * scaleFactor), screenY, lineWidth * scaleFactor, 170, 170, 170, 255);
		//5th Road
		App->renderer->DrawHorizontalLine(x + (roadWidth * scaleFactor * 1.5f) + (lineWidth * scaleFactor * 1.5f) + App->renderer->camera.x * scaleFactor + (roadSeparation * scaleFactor), screenY, roadWidth * scaleFactor, 170, 170, 170, 255);
		//5th Road - 6th Road Line
		App->renderer->DrawHorizontalLine(x + (roadWidth * scaleFactor * 2) + (lineWidth * scaleFactor * 2) + App->renderer->camera.x * scaleFactor + (roadSeparation * scaleFactor), screenY, lineWidth * scaleFactor, 170, 170, 170, 255);
		//6th Road
		App->renderer->DrawHorizontalLine(x + (roadWidth * scaleFactor * 2.5f) + (lineWidth * scaleFactor * 2.5f) + App->renderer->camera.x * scaleFactor + (roadSeparation * scaleFactor), screenY, roadWidth * scaleFactor, 170, 170, 170, 255);
		//6th Road Line
		App->renderer->DrawHorizontalLine(x + (roadWidth * scaleFactor * 3) + (lineWidth * scaleFactor * 3) + App->renderer->camera.x * scaleFactor + (roadSeparation * scaleFactor), screenY, lineWidth * scaleFactor, 170, 170, 170, 255);
		//Rumble
		App->renderer->DrawHorizontalLine(x + (roadWidth * scaleFactor * 3) + (rumbleWidth * scaleFactor / 2) + (lineWidth * scaleFactor * 3.5f) + App->renderer->camera.x * scaleFactor + (roadSeparation * scaleFactor), screenY, rumbleWidth * scaleFactor, 170, 170, 170, 255);
	}
	return screenY;
}

