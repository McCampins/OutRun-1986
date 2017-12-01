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

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/error/en.h"

using namespace rapidjson;

ModuleSceneStage::ModuleSceneStage(bool active) : Module(active)
{
	startFlagRect.x = 0;
	startFlagRect.y = 100;
	startFlagRect.w = 490;
	startFlagRect.h = 75;
}

ModuleSceneStage::~ModuleSceneStage()
{
}

// Load assets
bool ModuleSceneStage::Start()
{
	LOG("Loading stage scene");

	startFlag = App->textures->Load("rtype/startflag.png");

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

	ifstream inSegment("config//segments.config");

	if (inSegment.is_open() == false) {
		LOG("File not opened --------------");
		return false;
	}

	//Save file on string
	string jsonElem((std::istreambuf_iterator<char>(inSegment)), std::istreambuf_iterator<char>());
	const char* str = jsonElem.c_str();

	Segment s;

	//JSON read values
	int sep;
	string jx, jy, inclination;

	//Segment parameters
	float dX = 0.0f;
	float dY = 0.0f;
	float separation = 0.0f;
	Inclination inc;

	//JSON Parser
	Document document;
	if (document.Parse(str).HasParseError()) {
		const char* err = GetParseError_En(document.GetParseError());
		LOG(err);
		return false;
	}
	assert(document.IsObject());
	assert(document.HasMember("segments"));
	const Value& val = document["segments"];
	assert(val.IsArray());

	for (SizeType i = 0; i < val.Size(); i++) {
		assert(val[i].HasMember("dX"));
		assert(val[i]["dX"].IsString());
		jx = val[i]["dX"].GetString();
		assert(val[i].HasMember("dY"));
		assert(val[i]["dY"].IsString());
		jy = val[i]["dY"].GetString();
		assert(val[i].HasMember("separation"));
		assert(val[i]["separation"].IsInt());
		sep = val[i]["separation"].GetInt();
		assert(val[i].HasMember("inclination"));
		assert(val[i]["inclination"].IsString());
		inclination = val[i]["inclination"].GetString();
		
		if (jx.compare("0") == 0) {
			dX = 0.0f;
		}
		else if (jx.compare("SOFTLEFTCURVE") == 0) {
			dX = SOFTLEFTCURVE;
		}
		else  if (jx.compare("SOFTRIGHTCURVE") == 0) {
			dX = SOFTRIGHTCURVE;
		}
		else  if (jx.compare("HARDLEFTCURVE") == 0) {
			dX = HARDLEFTCURVE;
		}
		else  if (jx.compare("HARDRIGHTCURVE") == 0) {
			dX = HARDRIGHTCURVE;
		}
		else {
			LOG("Error reading config file -----------");
			return false;
		}

		if (jy.compare("0") == 0) {
			dY = 0.0f;
		}
		else if (jy.compare("UPHILL") == 0) {
			dY = UPHILL;
		}
		else  if (jy.compare("DOWNHILL") == 0) {
			dY = DOWNHILL;
		}
		else {
			LOG("Error reading config file -----------");
			return false;
		}

		separation = (float)sep;

		if (inclination.compare("Inclination::CENTER") == 0) {
			inc = Inclination::CENTER;
		}
		else if (inclination.compare("Inclination::DOWN") == 0)
		{
			inc = Inclination::DOWN;
		}
		else if (inclination.compare("Inclination::UP") == 0)
		{
			inc = Inclination::UP;
		}
		else {
			LOG("Error reading config file -----------");
			return false;
		}

		if (i == 0) {
			s = { dX, dY, separation, 0.0f, inc };
		}
		else {
			s = { dX, dY, separation, (float)zMap.size(), inc };
		}
		stageSegments.push_back(s);
	}

	ifstream inVisual("config//visualelements.config");

	if (inVisual.is_open() == false) {
		LOG("File not opened --------------");
		return false;
	}

	//Save file on string
	string jsonVisual((std::istreambuf_iterator<char>(inVisual)), std::istreambuf_iterator<char>());
	str = jsonVisual.c_str();

	VisualElement v;

	int elemX, elemY, world;

	if (document.Parse(str).HasParseError()) {
		const char* err = GetParseError_En(document.GetParseError());
		LOG(err);
		return false;
	}
	assert(document.IsObject());
	assert(document.HasMember("elements"));
	const Value& valElem = document["elements"];
	assert(valElem.IsArray());

	for (SizeType i = 0; i < val.Size(); i++) {
		assert(val[i].HasMember("dX"));
		assert(val[i]["dX"].IsString());
		jx = val[i]["dX"].GetString();
		assert(val[i].HasMember("dY"));
		assert(val[i]["dY"].IsString());
		jy = val[i]["dY"].GetString();
		assert(val[i].HasMember("separation"));
		assert(val[i]["separation"].IsInt());
		sep = val[i]["separation"].GetInt();
		assert(val[i].HasMember("inclination"));
		assert(val[i]["inclination"].IsString());
		inclination = val[i]["inclination"].GetString();

		if (jx.compare("0") == 0) {
			dX = 0.0f;
		}
		else if (jx.compare("SOFTLEFTCURVE") == 0) {
			dX = SOFTLEFTCURVE;
		}
		else  if (jx.compare("SOFTRIGHTCURVE") == 0) {
			dX = SOFTRIGHTCURVE;
		}
		else  if (jx.compare("HARDLEFTCURVE") == 0) {
			dX = HARDLEFTCURVE;
		}
		else  if (jx.compare("HARDRIGHTCURVE") == 0) {
			dX = HARDRIGHTCURVE;
		}
		else {
			LOG("Error reading config file -----------");
			return false;
		}

		if (jy.compare("0") == 0) {
			dY = 0.0f;
		}
		else if (jy.compare("UPHILL") == 0) {
			dY = UPHILL;
		}
		else  if (jy.compare("DOWNHILL") == 0) {
			dY = DOWNHILL;
		}
		else {
			LOG("Error reading config file -----------");
			return false;
		}

		separation = (float)sep;

		if (inclination.compare("Inclination::CENTER") == 0) {
			inc = Inclination::CENTER;
		}
		else if (inclination.compare("Inclination::DOWN") == 0)
		{
			inc = Inclination::DOWN;
		}
		else if (inclination.compare("Inclination::UP") == 0)
		{
			inc = Inclination::UP;
		}
		else {
			LOG("Error reading config file -----------");
			return false;
		}

		if (i == 0) {
			s = { dX, dY, separation, 0.0f, inc };
		}
		else {
			s = { dX, dY, separation, (float)zMap.size(), inc };
		}
		stageSegments.push_back(s);
	}
	//STAGE 1
	/*
	s = { 0.0f, 0.0f, 900.0f, 0.0f, Inclination::CENTER };
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
	*/
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
	bool drawn = false;

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

		if (ceil(worldPosition) == 8 && !drawn) {
			App->renderer->Blit(startFlag, -50 - ((App->renderer->camera.x / SCREEN_SIZE) * scaleFactor), 35 * (2 - scaleFactor), &startFlagRect, 1.0f, scaleFactor);
			drawn = true;
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

