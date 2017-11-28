#include <iostream>
#include <fstream>
#include <string>

#include "Globals.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModuleRender.h"
#include "ModulePlayer.h"
#include "ModuleCollision.h"
#include "ModuleParticles.h"
#include "ModuleSceneStage.h"

#include "rapidxml.hpp"

using namespace rapidxml;

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

	int y = SCREEN_HEIGHT * SCREEN_SIZE;
	int minY = SCREEN_HEIGHT * SCREEN_SIZE - roadHeightScreen;
	int maxY = y;
	float scaleFactor;
	for (int i = 0; i < roadHeightScreen; i++)
	{
		float z = (float)roadHeightWorld / (i - ((SCREEN_HEIGHT * SCREEN_SIZE) / 2));
		zMap.push_back(z);
		scaleFactor = (float)(y - minY) / (maxY - minY);
		scaleFactor = (scaleFactor * 0.95f) + 0.05f;
		factorMap.push_back(scaleFactor);
		y--;
	}

	ifstream inFile("segments.config");

	if (inFile.is_open() == false) {
		LOG("File not opened --------------");
		return false;
	}

	Segment s;
	//string json;
	string a, b, c, d;
	const char* str = nullptr;
	float dX, dY, separation;
	Inclination inc;

	xml_document<> doc;
	xml_node<> * root_node; 
	vector<char> buffer((istreambuf_iterator<char>(inFile)), istreambuf_iterator<char>());
	buffer.push_back('\0');
	doc.parse<0>(&buffer[0]);

	root_node = doc.first_node("segment");
	for (xml_node<> * segment_node = root_node->first_node("info"); segment_node; segment_node = segment_node->next_sibling())
	{
		a = segment_node->first_attribute("dX")->value();
		b = segment_node->first_attribute("dY")->value();
		c = segment_node->first_attribute("separation")->value();
		d = segment_node->first_attribute("inclination")->value();
		
	}

	 /*
	while (std::getline(inFile, json)) {
		str = json.c_str();
		d.Parse(str);
		*/
		/*
		assert(document.HasMember("dX"));
		assert(document["dX"].IsString());
		a = document["dX"].GetString();
		assert(document.HasMember("dY"));
		assert(document["dY"].IsString());
		b = document["dY"].GetString();
		assert(document.HasMember("separation"));
		assert(document["separation"].IsFloat());
		separation = document["separation"].GetFloat();
		assert(document.HasMember("inclination"));
		assert(document["inclination"].IsString());
		c = document["inclination"].GetString();
		*/

		/*
		if (b.compare("0") == 0) {
			dY = 0.0f;
		}
		else if (b.compare("UPHILL") == 0) {
			dY = UPHILL;
		}
		else  if (b.compare("DOWNHILL") == 0) {
			dY = DOWNHILL;
		}
		else {
			LOG("Error reading config file -----------");
			return false;
		}

		if (c.compare("Inclination::CENTER") == 0) {
			inc = Inclination::CENTER;
		}
		else if (c.compare("Inclination::DOWN") == 0)
		{
			inc = Inclination::DOWN;
		}
		else if (c.compare("Inclination::UP") == 0)
		{
			inc = Inclination::UP;
		}
		else {
			LOG("Error reading config file -----------");
			return false;
		}
		s = { dX, dY, separation, 0.0f, inc };
		*/
	//}

	//STAGE 1
	//s = { 0.0f, 0.0f, 900.0f, 0.0f, Inclination::CENTER };
	stageSegments.push_back(s);
	s = { 0.0f, 0.0f, 800.0f, (float)zMap.size(), Inclination::CENTER };
	stageSegments.push_back(s);
	s = { 0.0f, UPHILL, 800.0f, (float)zMap.size(), Inclination::UP };
	stageSegments.push_back(s);
	s = { 0.0f, DOWNHILL, 400.0f, (float)zMap.size(), Inclination::DOWN };
	stageSegments.push_back(s);
	s = { SOFTLEFTCURVE, 0.0f, 400.0f, (float)zMap.size(), Inclination::CENTER };
	stageSegments.push_back(s);
	s = { SOFTLEFTCURVE, 0.0f, 400.0f, (float)zMap.size(), Inclination::CENTER };
	stageSegments.push_back(s);
	s = { 0.0f, 0.0f, 400.0f, (float)zMap.size(), Inclination::CENTER };
	stageSegments.push_back(s);
	s = { SOFTRIGHTCURVE, DOWNHILL, 300.0f, (float)zMap.size(), Inclination::DOWN };
	stageSegments.push_back(s);
	s = { 0.0f, 0.0f, 0.0f, (float)zMap.size(), Inclination::CENTER };
	stageSegments.push_back(s);
	s = { 0.0f, 0.0f, 0.0f, (float)zMap.size(), Inclination::CENTER };
	stageSegments.push_back(s);
	s = { SOFTLEFTCURVE, 0.0f, 0.0f, (float)zMap.size(), Inclination::CENTER };
	stageSegments.push_back(s);
	s = { SOFTLEFTCURVE, 0.0f, 0.0f, (float)zMap.size(), Inclination::CENTER };
	stageSegments.push_back(s);
	s = { 0.0f, 0.0f, 0.0f, (float)zMap.size(), Inclination::CENTER };
	stageSegments.push_back(s);
	s = { 0.0f, 0.0f, 0.0f, (float)zMap.size(), Inclination::CENTER };
	stageSegments.push_back(s);
	s = { 0.0f, 0.0f, 0.0f, (float)zMap.size(), Inclination::CENTER };
	stageSegments.push_back(s);
	s = { HARDLEFTCURVE, 0.0f, 0.0f, (float)zMap.size(), Inclination::CENTER };
	stageSegments.push_back(s);
	s = { HARDLEFTCURVE, 0.0f, 0.0f, (float)zMap.size(), Inclination::CENTER };
	stageSegments.push_back(s);
	s = { HARDLEFTCURVE, 0.0f, 0.0f, (float)zMap.size(), Inclination::CENTER };
	stageSegments.push_back(s);
	s = { SOFTRIGHTCURVE, 0.0f, 0.0f, (float)zMap.size(), Inclination::CENTER };
	stageSegments.push_back(s);
	s = { 0.0f, 0.0f, 0.0f, (float)zMap.size(), Inclination::CENTER };
	stageSegments.push_back(s);
	s = { HARDLEFTCURVE, 0.0f, 0.0f, (float)zMap.size(), Inclination::CENTER };
	stageSegments.push_back(s);
	s = { HARDLEFTCURVE, UPHILL, 0.0f, (float)zMap.size(), Inclination::UP };
	stageSegments.push_back(s);
	s = { HARDLEFTCURVE, DOWNHILL, 0.0f, (float)zMap.size(), Inclination::DOWN };
	stageSegments.push_back(s);
	s = { 0.0f, 0.0f, 0.0f, (float)zMap.size(), Inclination::CENTER };
	stageSegments.push_back(s);
	s = { 0.0f, 0.0f, 0.0f, (float)zMap.size(), Inclination::CENTER };
	stageSegments.push_back(s);
	s = { 0.0f, 0.0f, 0.0f, (float)zMap.size(), Inclination::CENTER };
	stageSegments.push_back(s);
	s = { 0.0f, 0.0f, 0.0f, (float)zMap.size(), Inclination::CENTER };
	stageSegments.push_back(s);
	s = { 0.0f, 0.0f, 0.0f, (float)zMap.size(), Inclination::CENTER };
	stageSegments.push_back(s);
	s = { HARDLEFTCURVE, 0.0f, 0.0f, (float)zMap.size(), Inclination::CENTER };
	stageSegments.push_back(s);
	s = { HARDLEFTCURVE, 0.0f, 0.0f, (float)zMap.size(), Inclination::CENTER };
	stageSegments.push_back(s);
	s = { HARDLEFTCURVE, 0.0f, 0.0f, (float)zMap.size(), Inclination::CENTER };
	stageSegments.push_back(s);
	s = { 0.0f, 0.0f, 0.0f, (float)zMap.size(), Inclination::CENTER };
	stageSegments.push_back(s);
	s = { 0.0f, 0.0f, 0.0f, (float)zMap.size(), Inclination::CENTER };
	stageSegments.push_back(s);
	s = { HARDLEFTCURVE, 0.0f, 0.0f, (float)zMap.size(), Inclination::CENTER };
	stageSegments.push_back(s);
	s = { HARDLEFTCURVE, 0.0f, 0.0f, (float)zMap.size(), Inclination::CENTER };
	stageSegments.push_back(s);
	s = { HARDRIGHTCURVE, 0.0f, 0.0f, (float)zMap.size(), Inclination::CENTER };
	stageSegments.push_back(s);
	s = { HARDRIGHTCURVE, 0.0f, 0.0f, (float)zMap.size(), Inclination::CENTER };
	stageSegments.push_back(s);
	s = { HARDRIGHTCURVE, 0.0f, 0.0f, (float)zMap.size(), Inclination::CENTER };
	stageSegments.push_back(s);
	s = { HARDLEFTCURVE, 0.0f, 0.0f, (float)zMap.size(), Inclination::CENTER };
	stageSegments.push_back(s);
	s = { 0.0f, 0.0f, 0.0f, (float)zMap.size(), Inclination::CENTER };
	stageSegments.push_back(s);
	s = { 0.0f, 0.0f, 0.0f, (float)zMap.size(), Inclination::CENTER };
	stageSegments.push_back(s);
	s = { 0.0f, 0.0f, 0.0f, (float)zMap.size(), Inclination::CENTER };
	stageSegments.push_back(s);
	s = { 0.0f, 0.0f, 0.0f, (float)zMap.size(), Inclination::CENTER };
	stageSegments.push_back(s);
	s = { 0.0f, 0.0f, 0.0f, (float)zMap.size(), Inclination::CENTER };
	stageSegments.push_back(s);
	s = { 0.0f, 0.0f, 0.0f, (float)zMap.size(), Inclination::CENTER };
	stageSegments.push_back(s);
	s = { 0.0f, 0.0f, 0.0f, (float)zMap.size(), Inclination::CENTER };
	stageSegments.push_back(s);
	s = { 0.0f, UPHILL, 0.0f, (float)zMap.size(), Inclination::UP };
	stageSegments.push_back(s);
	s = { 0.0f, DOWNHILL, 0.0f, (float)zMap.size(), Inclination::DOWN };
	stageSegments.push_back(s);
	s = { 0.0f, 0.0f, 0.0f, (float)zMap.size(), Inclination::CENTER };
	stageSegments.push_back(s);
	s = { 0.0f, UPHILL, 0.0f, (float)zMap.size(), Inclination::UP };
	stageSegments.push_back(s);
	s = { 0.0f, UPHILL, 0.0f, (float)zMap.size(), Inclination::UP };
	stageSegments.push_back(s);
	s = { 0.0f, 0.0f, 0.0f, (float)zMap.size(), Inclination::CENTER };
	stageSegments.push_back(s);
	s = { 0.0f, 0.0f, 0.0f, (float)zMap.size(), Inclination::CENTER };
	stageSegments.push_back(s);
	//END OF STAGE 1

	bottomSegment = stageSegments.at(currentSegment);
	currentSegment++;
	topSegment = stageSegments.at(currentSegment);
	currentSegment++;

	App->renderer->camera.x = 710;

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
	int y = SCREEN_HEIGHT * SCREEN_SIZE;
	int screenY = y;
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

	bool inTopSegment = false;

	//Draw lines
	for (unsigned int i = 0; i < zMap.size(); i++) {
		z = zMap.at(i);

		scaleFactor = factorMap.at(i);

		if (i < topSegment.yMapPosition) {
			dX = bottomSegment.dX;
			dY = bottomSegment.dY;
		}
		else {
			inTopSegment = true;
			dX = topSegment.dX;
			dY = topSegment.dY;
		}
		ddX += dX;
		x += ddX;

		roadSeparation = initialRoadSeparation - (separationInterval * -(-1 + segmentFactor));

		float worldPosition = (z * 10) + App->player->position;

		//Check if uphill, downhill or no hill
		if (dY < 0) {
			float percentage = 0.0f;
			if (inTopSegment == true) {
				percentage = (float)(i - topSegment.yMapPosition) / (zMap.size() - topSegment.yMapPosition);
			}
			else {
				percentage = (float)i / topSegment.yMapPosition;
			}
			if (percentage < 0.5f)
				screenY = DrawRoads(screenY, worldPosition, scaleFactor, x, roadSeparation);
			if (percentage < 0.25f)
				screenY = DrawRoads(screenY, worldPosition, scaleFactor, x, roadSeparation);
			if (percentage < 0.05f)
				screenY = DrawRoads(screenY, worldPosition, scaleFactor, x, roadSeparation);
		}
		else if (dY > 0) {
			float percentage = 0.0f;
			if (inTopSegment == true) {
				percentage = (float)(i - topSegment.yMapPosition) / (zMap.size() - topSegment.yMapPosition);
			}
			else {
				percentage = (float)i / topSegment.yMapPosition;
			}
			if (percentage < 0.2f) {
				screenY = DrawRoads(screenY, worldPosition, scaleFactor, x, roadSeparation);
			}
			else if (percentage < 0.7f) {
				screenY = DrawRoads(screenY, worldPosition, scaleFactor, x, roadSeparation);
				screenY = DrawRoads(screenY, worldPosition, scaleFactor, x, roadSeparation);
			}
			else {
				screenY = DrawRoads(screenY, worldPosition, scaleFactor, x, roadSeparation);
				screenY = DrawRoads(screenY, worldPosition, scaleFactor, x, roadSeparation);
				screenY = DrawRoads(screenY, worldPosition, scaleFactor, x, roadSeparation);
			}
		}
		else {
			screenY = DrawRoads(screenY, worldPosition, scaleFactor, x, roadSeparation);
		}
		//If the line we are drawing is the one the tires are placed, check if they are out of the road
		if (screenY == (SCREEN_HEIGHT - 8) * SCREEN_SIZE) {
			leftTireOut = CheckLeftTire(x, scaleFactor, roadSeparation);
			rigthTireOut = CheckRightTire(x, scaleFactor, roadSeparation);
		}
	}

	//Update segments
	App->renderer->camera.x += curveCameraMove;
	topSegment.yMapPosition -= App->player->curveSpeed;
	if (topSegment.yMapPosition < 0) {
		bottomSegment = topSegment;
		if (currentSegment < stageSegments.size()) {
			topSegment = stageSegments.at(currentSegment);
			currentSegment++;
		}
		else {
			topSegment = { 0.0f, 0.0f, 0.0f, (float)zMap.size(), Inclination::CENTER };
		}
	}

	//Update the camera according to the curve and the speed of the player
	curveCameraMove = 0;
	if (App->player->playerSpeed != 0.0f) {
		if (bottomSegment.dX > 0.0f) {
			float normalize = App->player->playerSpeed / MAX_SPEED;
			if (normalize < 0.33f) {
				curveCameraMove = 3;
			}
			else if (normalize < 0.66f) {
				curveCameraMove = 6;
			}
			else {
				curveCameraMove = 12;
			}
		}
		else if (bottomSegment.dX < 0.0f) {
			float normalize = App->player->playerSpeed / MAX_SPEED;
			if (normalize < 0.33f) {
				curveCameraMove = -3;
			}
			else if (normalize < 0.66f) {
				curveCameraMove = -6;
			}
			else {
				curveCameraMove = -12;
			}
		}
	}

	return UPDATE_CONTINUE;
}

int ModuleSceneStage::DrawRoads(int screenY, float worldPosition, float scaleFactor, float x, float roadSeparation) {
	screenY--;
	if ((int)worldPosition % 2 == 0)
	{
		//Terrain
		App->renderer->DrawHorizontalLine(x + App->renderer->camera.x * scaleFactor, screenY, terrainWidth, 219, 209, 180, 255);
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
		App->renderer->DrawHorizontalLine(x + (roadWidth * scaleFactor * 3) + (rumbleWidth * scaleFactor / 2) + (lineWidth * scaleFactor * 3.5f) + App->renderer->camera.x * scaleFactor + (roadSeparation * scaleFactor), screenY, rumbleWidth * scaleFactor, 161, 160, 161, 255);
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
		App->renderer->DrawHorizontalLine(x + (roadWidth * scaleFactor * 3) + (rumbleWidth * scaleFactor / 2) + (lineWidth * scaleFactor * 3.5f) + App->renderer->camera.x * scaleFactor + (roadSeparation * scaleFactor), screenY, rumbleWidth * scaleFactor, 255, 255, 255, 255);
	}
	return screenY;
}

bool ModuleSceneStage::CheckLeftTire(float x, float scaleFactor, float roadSeparation)
{
	bool ret = false;
	float leftTire = x - (roadWidth * scaleFactor * 3) - (rumbleWidth * scaleFactor / 2) - (lineWidth * scaleFactor * 3.5f) + App->renderer->camera.x * scaleFactor;
	if ((leftTire - ((SCREEN_WIDTH * SCREEN_SIZE / 2) - 45)) > 0.0f) {
		return true;
	}
	else {
		ret = false;
	}
	leftTire = x + (rumbleWidth * scaleFactor / 2) + (lineWidth * scaleFactor / 2) + App->renderer->camera.x * scaleFactor;
	if ((((SCREEN_WIDTH * SCREEN_SIZE / 2) - 100) - leftTire) > 0.0f) {
		leftTire = x - (rumbleWidth * scaleFactor / 2) - (lineWidth * scaleFactor / 2) + App->renderer->camera.x * scaleFactor + (roadSeparation * scaleFactor);
		if ((leftTire - ((SCREEN_WIDTH * SCREEN_SIZE / 2) - 45)) > 0.0f) {
			return true;
		}
		else {
			ret = false;
		}
	}
	else {
		ret = false;
	}
	leftTire = x + (roadWidth * scaleFactor * 3) + (rumbleWidth * scaleFactor / 2) + (lineWidth * scaleFactor * 3.5f) + App->renderer->camera.x * scaleFactor + (roadSeparation * scaleFactor);
	if ((((SCREEN_WIDTH * SCREEN_SIZE / 2) - 100) - leftTire) > 0.0f) {
		return true;
	}
	else {
		ret = false;
	}
	return ret;
}

bool ModuleSceneStage::CheckRightTire(float x, float scaleFactor, float roadSeparation)
{
	bool ret = false;
	float rightTire = x - (roadWidth * scaleFactor * 3) - (rumbleWidth * scaleFactor / 2) - (lineWidth * scaleFactor * 3.5f) + App->renderer->camera.x * scaleFactor;
	if ((rightTire - ((SCREEN_WIDTH * SCREEN_SIZE / 2) + 125)) > 0.0f) {
		return true;
	}
	else {
		ret = false;
	}
	rightTire = x + (rumbleWidth * scaleFactor / 2) + (lineWidth * scaleFactor / 2) + App->renderer->camera.x * scaleFactor;
	if ((((SCREEN_WIDTH * SCREEN_SIZE / 2) + 70) - rightTire) > 0.0f) {
		rightTire = x - (rumbleWidth * scaleFactor / 2) - (lineWidth * scaleFactor / 2) + App->renderer->camera.x * scaleFactor + (roadSeparation * scaleFactor);
		if ((rightTire - ((SCREEN_WIDTH * SCREEN_SIZE / 2) + 115)) > 0.0f) {
			return true;
		}
		else {
			ret = false;
		}
	}
	else {
		ret = false;
	}
	rightTire = x + (roadWidth * scaleFactor * 3) + (rumbleWidth * scaleFactor / 2) + (lineWidth * scaleFactor * 3.5f) + App->renderer->camera.x * scaleFactor + (roadSeparation * scaleFactor);
	if ((((SCREEN_WIDTH * SCREEN_SIZE / 2) + 60) - rightTire) > 0.0f) {
		return true;
	}
	else {
		ret = false;
	}
	return ret;
}

