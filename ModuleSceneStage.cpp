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
	int minY = SCREEN_HEIGHT * SCREEN_SIZE - ROADHEIGHTSCREEN;
	int maxY = y;
	float scaleFactor;
	for (int i = 0; i < ROADHEIGHTSCREEN; i++)
	{
		float z = (float)ROADHEIGHTWORLD / (i - ((SCREEN_HEIGHT * SCREEN_SIZE) / 2));
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
	Document docSegment;
	if (docSegment.Parse(str).HasParseError()) {
		const char* err = GetParseError_En(docSegment.GetParseError());
		LOG(err);
		return false;
	}
	assert(docSegment.IsObject());
	assert(docSegment.HasMember("segments"));
	const Value& val = docSegment["segments"];
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

	const char* texPath;
	int elemX, elemY, world, nConsElem;
	string pos;
	VisualElementPosition vpos;
	unordered_map<const char*, pair<SDL_Texture*, SDL_Rect>>::const_iterator it;
	SDL_Texture* tex = nullptr;
	SDL_Rect r;

	Document docVisual;
	if (docVisual.Parse(str).HasParseError()) {
		const char* err = GetParseError_En(docVisual.GetParseError());
		LOG(err);
		return false;
	}
	assert(docVisual.IsObject());
	assert(docVisual.HasMember("elements"));
	const Value& valElem = docVisual["elements"];
	assert(valElem.IsArray());

	for (SizeType i = 0; i < valElem.Size(); i++) {
		assert(valElem[i].HasMember("texture"));
		assert(valElem[i]["texture"].IsString());
		texPath = valElem[i]["texture"].GetString();
		it = textures.find(texPath);
		if (it == textures.cend()) {
			tex = App->textures->Load(texPath);
			assert(valElem[i].HasMember("rect"));
			assert(valElem[i]["rect"].IsArray());
			const Value& arrRect = valElem[i]["rect"];
			if (arrRect.Size() != 4) {
				LOG("Error reading config file. Check rect array parameters-----------");
				return false;
			}
			r = { arrRect[0].GetInt(), arrRect[1].GetInt(), arrRect[2].GetInt(), arrRect[3].GetInt() };
			std::pair<SDL_Texture*, SDL_Rect> sdlPair(tex, r);
			std::pair<const char*, pair<SDL_Texture*, SDL_Rect>> pair(texPath, sdlPair);
			textures.insert(pair);
		}
		else {
			tex = it->second.first;
			r = it->second.second;
		}
		assert(valElem[i].HasMember("x"));
		assert(valElem[i]["x"].IsInt());
		elemX = valElem[i]["x"].GetInt();
		assert(valElem[i].HasMember("y"));
		assert(valElem[i]["y"].IsInt());
		elemY = valElem[i]["y"].GetInt();
		assert(valElem[i].HasMember("worldPosition"));
		assert(valElem[i]["worldPosition"].IsInt());
		world = valElem[i]["worldPosition"].GetInt();
		assert(valElem[i].HasMember("nConsecutiveElements"));
		assert(valElem[i]["nConsecutiveElements"].IsInt());
		nConsElem = valElem[i]["nConsecutiveElements"].GetInt();
		assert(valElem[i].HasMember("position"));
		assert(valElem[i]["position"].IsString());
		pos = valElem[i]["position"].GetString();

		if (pos.compare("LEFT") == 0) {
			vpos = VisualElementPosition::LEFT;
		}
		else if (pos.compare("CENTER") == 0) {
			vpos = VisualElementPosition::CENTER;
		}
		else if (pos.compare("RIGHT") == 0) {
			vpos = VisualElementPosition::RIGHT;
		}
		else if (pos.compare("LEFTANDRIGHT") == 0) {
			vpos = VisualElementPosition::LEFTANDRIGHT;
		}
		else if (pos.compare("LEFTANDCENTER") == 0) {
			vpos = VisualElementPosition::LEFTANDCENTER;
		}
		else if (pos.compare("CENTERANDRIGHT") == 0) {
			vpos = VisualElementPosition::CENTERANDRIGHT;
		}
		else  if (pos.compare("ALL") == 0) {
			vpos = VisualElementPosition::ALL;
		}
		else {
			LOG("Error reading config file -----------");
			return false;
		}

		v = { tex, r, elemX, elemY, world, nConsElem, vpos };
		elements.push_back(v);
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

	float worldPosition;

	if (topSegment.yMapPosition < 1.0f) {
		topSegment.yMapPosition = 0.0f;
	}
	float segmentFactor = topSegment.yMapPosition / zMap.size();

	bool inTopSegment = false;
	std::vector<int> elementsRemaining(elements.size());
	for (unsigned int i = 0; i < elementsRemaining.size(); i++) {
		elementsRemaining.at(i) = elements.at(i).nConsecutiveElements;
	}

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

		worldPosition = (z * 10) + App->player->position;

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

	std::vector<VisualElement> elementsToDraw;
	z = zMap.at(zMap.size() - 1);
	int maxPosition = int((z * 10) + App->player->position);
	VisualElement vElem;
	unsigned int n = 0;
	vElem = elements.at(n);
	while (vElem.worldPosition <= ceil(maxPosition)) {
		elementsToDraw.push_back(vElem);
		if (vElem.nConsecutiveElements > 0) {
			bool insideRange = true;
			for (int i = 1; i <= vElem.nConsecutiveElements && insideRange == true; i++) {
				vElem.worldPosition += i;
				if (vElem.worldPosition <= ceil(maxPosition)) {
					elementsToDraw.push_back(vElem);
				}
				else {
					insideRange = false;
				}
				vElem.worldPosition -= i;
			}
		}
		n++;
		if (n > elements.size() - 1)
			break;
		vElem = elements.at(n);
	}

	std::vector<bool> elementDrawn(elementsToDraw.size());
	for (std::vector<bool>::iterator it = elementDrawn.begin(); it != elementDrawn.end(); ++it)
		*it = false;

	for (int i = zMap.size() - 1; i >= 0; i--) {
		z = zMap.at(i);
		scaleFactor = factorMap.at(i);
		worldPosition = (z * 10) + App->player->position;
		roadSeparation = initialRoadSeparation - (separationInterval * -(-1 + segmentFactor));

		n = 0;
		vElem = elementsToDraw.at(n);
		while (vElem.worldPosition <= ceil(worldPosition)) {
			if (ceil(worldPosition) == vElem.worldPosition && elementDrawn.at(n) == false) {
				SDL_Rect rect;
				switch (vElem.position) {
				case VisualElementPosition::LEFT:
					App->renderer->Blit(vElem.texture, int(vElem.x + (SCREEN_WIDTH * scaleFactor / 2)), int(vElem.y * (2 - scaleFactor)), &(vElem.rect), 1.0f, scaleFactor);
					break;
				case VisualElementPosition::CENTER:
					rect = vElem.rect;
					rect.x += rect.w;
					App->renderer->Blit(vElem.texture, int(SCREEN_WIDTH * scaleFactor / 2), int(vElem.y * (2 - scaleFactor)), &(rect), 1.0f, scaleFactor);
					break;
				case VisualElementPosition::RIGHT:
					rect = vElem.rect;
					rect.x += rect.w;
					App->renderer->Blit(vElem.texture, int(-vElem.x + (SCREEN_WIDTH * scaleFactor / 2)), int(vElem.y * (2 - scaleFactor)), &(vElem.rect), 1.0f, scaleFactor);
					break;
				case VisualElementPosition::LEFTANDCENTER:
					App->renderer->Blit(vElem.texture, int(vElem.x + (SCREEN_WIDTH * scaleFactor / 2)), int(vElem.y * (2 - scaleFactor)), &(vElem.rect), 1.0f, scaleFactor);
					rect = vElem.rect;
					rect.x += rect.w;
					App->renderer->Blit(vElem.texture, int(SCREEN_WIDTH * scaleFactor / 2), int(vElem.y * (2 - scaleFactor)), &(rect), 1.0f, scaleFactor);
					break;
				case VisualElementPosition::LEFTANDRIGHT:
					App->renderer->Blit(vElem.texture, int(vElem.x + (SCREEN_WIDTH * scaleFactor / 2)), int(vElem.y * (2 - scaleFactor)), &(vElem.rect), 1.0f, scaleFactor);
					rect = vElem.rect;
					rect.x += rect.w;
					App->renderer->Blit(vElem.texture, int(-vElem.x + (SCREEN_WIDTH * scaleFactor / 2)), int(vElem.y * (2 - scaleFactor)), &(vElem.rect), 1.0f, scaleFactor);
					break;
				case VisualElementPosition::CENTERANDRIGHT:
					rect = vElem.rect;
					rect.x += rect.w;
					App->renderer->Blit(vElem.texture, int(SCREEN_WIDTH * scaleFactor / 2), int(vElem.y * (2 - scaleFactor)), &(rect), 1.0f, scaleFactor);
					App->renderer->Blit(vElem.texture, int(-vElem.x + (SCREEN_WIDTH * scaleFactor / 2)), int(vElem.y * (2 - scaleFactor)), &(vElem.rect), 1.0f, scaleFactor);
					break;
				case VisualElementPosition::ALL:
					break;
				}
				elementDrawn.at(n) = true;
			}
			n++;
			if (n > elementsToDraw.size() - 1)
				break;
			vElem = elementsToDraw.at(n);
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
		App->renderer->DrawHorizontalLine(x + App->renderer->camera.x * scaleFactor, screenY, TERRAINWIDTH, 219, 209, 180, 255);
		//Rumble
		App->renderer->DrawHorizontalLine(x - (ROADWIDTH * scaleFactor * 3) - (RUMBLEWIDTH * scaleFactor / 2) - (LINEWIDTH * scaleFactor * 3.5f) + App->renderer->camera.x * scaleFactor, screenY, RUMBLEWIDTH * scaleFactor, 161, 160, 161, 255);
		//1st Road Line
		App->renderer->DrawHorizontalLine(x - (ROADWIDTH * scaleFactor * 3) - (LINEWIDTH * scaleFactor * 3) + App->renderer->camera.x * scaleFactor, screenY, LINEWIDTH * scaleFactor, 255, 255, 255, 255);
		//1st Road
		App->renderer->DrawHorizontalLine(x - (ROADWIDTH * scaleFactor * 2.5f) - (LINEWIDTH * scaleFactor * 2.5f) + App->renderer->camera.x * scaleFactor, screenY, ROADWIDTH * scaleFactor, 161, 160, 161, 255);
		//1st Road - 2nd Road Line
		App->renderer->DrawHorizontalLine(x - (ROADWIDTH * scaleFactor * 2) - (LINEWIDTH * scaleFactor * 2) + App->renderer->camera.x * scaleFactor, screenY, LINEWIDTH * scaleFactor, 255, 255, 255, 255);
		//2nd Road
		App->renderer->DrawHorizontalLine(x - (ROADWIDTH * scaleFactor * 1.5f) - (LINEWIDTH * scaleFactor * 1.5f) + App->renderer->camera.x * scaleFactor, screenY, ROADWIDTH * scaleFactor, 161, 160, 161, 255);
		//2nd Road - 3rd Road Line
		App->renderer->DrawHorizontalLine(x - (ROADWIDTH * scaleFactor) - (LINEWIDTH * scaleFactor) + App->renderer->camera.x * scaleFactor, screenY, LINEWIDTH * scaleFactor, 255, 255, 255, 255);
		//Rigth Road Rumble (before 3rd Road in case both roads intersect)
		App->renderer->DrawHorizontalLine(x - (RUMBLEWIDTH * scaleFactor / 2) - (LINEWIDTH * scaleFactor / 2) + App->renderer->camera.x * scaleFactor + (roadSeparation * scaleFactor), screenY, RUMBLEWIDTH * scaleFactor, 161, 160, 161, 255);
		//3rd Road
		App->renderer->DrawHorizontalLine(x - (ROADWIDTH * scaleFactor / 2) - (LINEWIDTH * scaleFactor / 2) + App->renderer->camera.x * scaleFactor, screenY, ROADWIDTH * scaleFactor, 161, 160, 161, 255);
		//3rd Road Line
		App->renderer->DrawHorizontalLine(x + App->renderer->camera.x * scaleFactor, screenY, LINEWIDTH * scaleFactor, 255, 255, 255, 255);
		//4th Road Line (same coordinates as they might intersect)
		App->renderer->DrawHorizontalLine(x + App->renderer->camera.x * scaleFactor + (roadSeparation * scaleFactor), screenY, LINEWIDTH * scaleFactor, 255, 255, 255, 255);
		//Left Road Rumble (before 4th Road in case both roads intersect)
		App->renderer->DrawHorizontalLine(x + (RUMBLEWIDTH * scaleFactor / 2) + (LINEWIDTH * scaleFactor / 2) + App->renderer->camera.x * scaleFactor, screenY, RUMBLEWIDTH * scaleFactor, 161, 160, 161, 255);
		//4th Road
		App->renderer->DrawHorizontalLine(x + (ROADWIDTH * scaleFactor / 2) + (LINEWIDTH * scaleFactor / 2) + App->renderer->camera.x * scaleFactor + (roadSeparation * scaleFactor), screenY, ROADWIDTH * scaleFactor, 161, 160, 161, 255);
		//4th Road - 5th Road Line
		App->renderer->DrawHorizontalLine(x + (ROADWIDTH * scaleFactor) + (LINEWIDTH * scaleFactor) + App->renderer->camera.x * scaleFactor + (roadSeparation * scaleFactor), screenY, LINEWIDTH * scaleFactor, 255, 255, 255, 255);
		//5th Road
		App->renderer->DrawHorizontalLine(x + (ROADWIDTH * scaleFactor * 1.5f) + (LINEWIDTH * scaleFactor * 1.5f) + App->renderer->camera.x * scaleFactor + (roadSeparation * scaleFactor), screenY, ROADWIDTH * scaleFactor, 161, 160, 161, 255);
		//5th Road - 6th Road Line
		App->renderer->DrawHorizontalLine(x + (ROADWIDTH * scaleFactor * 2) + (LINEWIDTH * scaleFactor * 2) + App->renderer->camera.x * scaleFactor + (roadSeparation * scaleFactor), screenY, LINEWIDTH * scaleFactor, 255, 255, 255, 255);
		//6th Road
		App->renderer->DrawHorizontalLine(x + (ROADWIDTH * scaleFactor * 2.5f) + (LINEWIDTH * scaleFactor * 2.5f) + App->renderer->camera.x * scaleFactor + (roadSeparation * scaleFactor), screenY, ROADWIDTH * scaleFactor, 161, 160, 161, 255);
		//6th Road Line
		App->renderer->DrawHorizontalLine(x + (ROADWIDTH * scaleFactor * 3) + (LINEWIDTH * scaleFactor * 3) + App->renderer->camera.x * scaleFactor + (roadSeparation * scaleFactor), screenY, LINEWIDTH * scaleFactor, 255, 255, 255, 255);
		//Rumble
		App->renderer->DrawHorizontalLine(x + (ROADWIDTH * scaleFactor * 3) + (RUMBLEWIDTH * scaleFactor / 2) + (LINEWIDTH * scaleFactor * 3.5f) + App->renderer->camera.x * scaleFactor + (roadSeparation * scaleFactor), screenY, RUMBLEWIDTH * scaleFactor, 161, 160, 161, 255);
	}
	else
	{
		//Terrain
		App->renderer->DrawHorizontalLine(x + App->renderer->camera.x * scaleFactor, screenY, TERRAINWIDTH, 194, 178, 128, 255);
		//Rumble
		App->renderer->DrawHorizontalLine(x - (ROADWIDTH * scaleFactor * 3) - (RUMBLEWIDTH * scaleFactor / 2) - (LINEWIDTH * scaleFactor * 3.5f) + App->renderer->camera.x * scaleFactor, screenY, RUMBLEWIDTH * scaleFactor, 255, 255, 255, 255);
		//1st Road Line
		App->renderer->DrawHorizontalLine(x - (ROADWIDTH * scaleFactor * 3) - (LINEWIDTH * scaleFactor * 3) + App->renderer->camera.x * scaleFactor, screenY, LINEWIDTH * scaleFactor, 170, 170, 170, 255);
		//1st Road
		App->renderer->DrawHorizontalLine(x - (ROADWIDTH * scaleFactor * 2.5f) - (LINEWIDTH * scaleFactor * 2.5f) + App->renderer->camera.x * scaleFactor, screenY, ROADWIDTH * scaleFactor, 170, 170, 170, 255);
		//1st Road - 2nd Road Line
		App->renderer->DrawHorizontalLine(x - (ROADWIDTH * scaleFactor * 2) - (LINEWIDTH * scaleFactor * 2) + App->renderer->camera.x * scaleFactor, screenY, LINEWIDTH * scaleFactor, 170, 170, 170, 255);
		//2nd Road
		App->renderer->DrawHorizontalLine(x - (ROADWIDTH * scaleFactor * 1.5f) - (LINEWIDTH * scaleFactor * 1.5f) + App->renderer->camera.x * scaleFactor, screenY, ROADWIDTH * scaleFactor, 170, 170, 170, 255);
		//2nd Road - 3rd Road Line
		App->renderer->DrawHorizontalLine(x - (ROADWIDTH * scaleFactor) - (LINEWIDTH * scaleFactor) + App->renderer->camera.x * scaleFactor, screenY, LINEWIDTH * scaleFactor, 170, 170, 170, 255);
		//Rigth Road Rumble (before 3rd Road in case both roads intersect)
		App->renderer->DrawHorizontalLine(x - (RUMBLEWIDTH * scaleFactor / 2) - (LINEWIDTH * scaleFactor / 2) + App->renderer->camera.x * scaleFactor + (roadSeparation * scaleFactor), screenY, RUMBLEWIDTH * scaleFactor, 255, 255, 255, 255);
		//3rd Road
		App->renderer->DrawHorizontalLine(x - (ROADWIDTH * scaleFactor / 2) - (LINEWIDTH * scaleFactor / 2) + App->renderer->camera.x * scaleFactor, screenY, ROADWIDTH * scaleFactor, 170, 170, 170, 255);
		//3rd Road Line
		App->renderer->DrawHorizontalLine(x + App->renderer->camera.x * scaleFactor, screenY, LINEWIDTH * scaleFactor, 170, 170, 170, 255);
		//4th Road Line (same coordinates as they might intersect)
		App->renderer->DrawHorizontalLine(x + App->renderer->camera.x * scaleFactor + (roadSeparation * scaleFactor), screenY, LINEWIDTH * scaleFactor, 170, 170, 170, 255);
		//Left Road Rumble (before 4th Road in case they intersect)
		App->renderer->DrawHorizontalLine(x + (RUMBLEWIDTH * scaleFactor / 2) + (LINEWIDTH * scaleFactor / 2) + App->renderer->camera.x * scaleFactor, screenY, RUMBLEWIDTH * scaleFactor, 255, 255, 255, 255);
		//4th Road
		App->renderer->DrawHorizontalLine(x + (ROADWIDTH * scaleFactor / 2) + (LINEWIDTH * scaleFactor / 2) + App->renderer->camera.x * scaleFactor + (roadSeparation * scaleFactor), screenY, ROADWIDTH * scaleFactor, 170, 170, 170, 255);
		//4th Road - 5th Road Line
		App->renderer->DrawHorizontalLine(x + (ROADWIDTH * scaleFactor) + (LINEWIDTH * scaleFactor) + App->renderer->camera.x * scaleFactor + (roadSeparation * scaleFactor), screenY, LINEWIDTH * scaleFactor, 170, 170, 170, 255);
		//5th Road
		App->renderer->DrawHorizontalLine(x + (ROADWIDTH * scaleFactor * 1.5f) + (LINEWIDTH * scaleFactor * 1.5f) + App->renderer->camera.x * scaleFactor + (roadSeparation * scaleFactor), screenY, ROADWIDTH * scaleFactor, 170, 170, 170, 255);
		//5th Road - 6th Road Line
		App->renderer->DrawHorizontalLine(x + (ROADWIDTH * scaleFactor * 2) + (LINEWIDTH * scaleFactor * 2) + App->renderer->camera.x * scaleFactor + (roadSeparation * scaleFactor), screenY, LINEWIDTH * scaleFactor, 170, 170, 170, 255);
		//6th Road
		App->renderer->DrawHorizontalLine(x + (ROADWIDTH * scaleFactor * 2.5f) + (LINEWIDTH * scaleFactor * 2.5f) + App->renderer->camera.x * scaleFactor + (roadSeparation * scaleFactor), screenY, ROADWIDTH * scaleFactor, 170, 170, 170, 255);
		//6th Road Line
		App->renderer->DrawHorizontalLine(x + (ROADWIDTH * scaleFactor * 3) + (LINEWIDTH * scaleFactor * 3) + App->renderer->camera.x * scaleFactor + (roadSeparation * scaleFactor), screenY, LINEWIDTH * scaleFactor, 170, 170, 170, 255);
		//Rumble
		App->renderer->DrawHorizontalLine(x + (ROADWIDTH * scaleFactor * 3) + (RUMBLEWIDTH * scaleFactor / 2) + (LINEWIDTH * scaleFactor * 3.5f) + App->renderer->camera.x * scaleFactor + (roadSeparation * scaleFactor), screenY, RUMBLEWIDTH * scaleFactor, 255, 255, 255, 255);
	}
	return screenY;
}

bool ModuleSceneStage::CheckLeftTire(float x, float scaleFactor, float roadSeparation)
{
	bool ret = false;
	float leftTire = x - (ROADWIDTH * scaleFactor * 3) - (RUMBLEWIDTH * scaleFactor / 2) - (LINEWIDTH * scaleFactor * 3.5f) + App->renderer->camera.x * scaleFactor;
	if ((leftTire - ((SCREEN_WIDTH * SCREEN_SIZE / 2) - 45)) > 0.0f) {
		return true;
	}
	else {
		ret = false;
	}
	leftTire = x + (RUMBLEWIDTH * scaleFactor / 2) + (LINEWIDTH * scaleFactor / 2) + App->renderer->camera.x * scaleFactor;
	if ((((SCREEN_WIDTH * SCREEN_SIZE / 2) - 100) - leftTire) > 0.0f) {
		leftTire = x - (RUMBLEWIDTH * scaleFactor / 2) - (LINEWIDTH * scaleFactor / 2) + App->renderer->camera.x * scaleFactor + (roadSeparation * scaleFactor);
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
	leftTire = x + (ROADWIDTH * scaleFactor * 3) + (RUMBLEWIDTH * scaleFactor / 2) + (LINEWIDTH * scaleFactor * 3.5f) + App->renderer->camera.x * scaleFactor + (roadSeparation * scaleFactor);
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
	float rightTire = x - (ROADWIDTH * scaleFactor * 3) - (RUMBLEWIDTH * scaleFactor / 2) - (LINEWIDTH * scaleFactor * 3.5f) + App->renderer->camera.x * scaleFactor;
	if ((rightTire - ((SCREEN_WIDTH * SCREEN_SIZE / 2) + 125)) > 0.0f) {
		return true;
	}
	else {
		ret = false;
	}
	rightTire = x + (RUMBLEWIDTH * scaleFactor / 2) + (LINEWIDTH * scaleFactor / 2) + App->renderer->camera.x * scaleFactor;
	if ((((SCREEN_WIDTH * SCREEN_SIZE / 2) + 70) - rightTire) > 0.0f) {
		rightTire = x - (RUMBLEWIDTH * scaleFactor / 2) - (LINEWIDTH * scaleFactor / 2) + App->renderer->camera.x * scaleFactor + (roadSeparation * scaleFactor);
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
	rightTire = x + (ROADWIDTH * scaleFactor * 3) + (RUMBLEWIDTH * scaleFactor / 2) + (LINEWIDTH * scaleFactor * 3.5f) + App->renderer->camera.x * scaleFactor + (roadSeparation * scaleFactor);
	if ((((SCREEN_WIDTH * SCREEN_SIZE / 2) + 60) - rightTire) > 0.0f) {
		return true;
	}
	else {
		ret = false;
	}
	return ret;
}

