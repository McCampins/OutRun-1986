#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <algorithm>

#include "Globals.h"
#include "ModuleFontManager.h"
#include "ModuleFont.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModuleRender.h"
#include "ModulePlayer.h"
#include "ModuleInput.h"
#include "ModuleSceneMusic.h"
#include "ModuleSceneStage.h"
#include "ModuleFadeToBlack.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/error/en.h"

using namespace rapidjson;

ModuleSceneStage::ModuleSceneStage(bool active) : Module(active)
{
	back = { 0, 0, 1200, 266 };
	timeR = { 0, 0, 67, 35 };
}

ModuleSceneStage::~ModuleSceneStage()
{
}

/* All ms counters to know and improve performance have been commented
double clockToMilliseconds(clock_t ticks) {
	// units/(units/time) => time (seconds) * 1000 = milliseconds
	return (ticks / (double)CLOCKS_PER_SEC)*1000.0;
}
*/

// Load assets
bool ModuleSceneStage::Start()
{
	LOG("Loading stage scene");

	background = App->textures->Load("rtype/background.png");
	time = App->textures->Load("rtype/time.png");

	startRace = "rtype/Music/startingRace.wav";

	fm = new ModuleFontManager();

	fm->Init();
	greenFont = fm->Allocate("greenfont.bmp", __FILE__, to_string(__LINE__));
	pinkFont = fm->Allocate("pinkfont.bmp", __FILE__, to_string(__LINE__));
	redFont = fm->Allocate("redfont.bmp", __FILE__, to_string(__LINE__));

	App->player->Enable();

	App->renderer->camera.x = STARTINGCAMERA;

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

		Segment* s;
		if (i == 0) {
			s = new Segment(dX, dY, separation, 0.0f, inc);
		}
		else {
			s = new Segment(dX, dY, separation, (float)zMap.size(), inc);
		}
		stageSegments.push_back(s);
	}

	ifstream inVisual("config//visualElements.config");

	if (inVisual.is_open() == false) {
		LOG("File not opened --------------");
		return false;
	}

	//Save file on string
	string jsonVisual((std::istreambuf_iterator<char>(inVisual)), std::istreambuf_iterator<char>());
	str = jsonVisual.c_str();

	VisualElement* v;

	const char* texPath;
	int elemX, elemY, overHorizon, nConsElem;
	float world, speed, increment;
	bool horizon;
	string pos;
	VisualElementPosition vpos;
	std::unordered_map<std::string, SDL_Texture*>::iterator it;
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
		if (it == textures.end()) {
			tex = App->textures->Load(texPath);
			std::pair<const char*, SDL_Texture*> pair(texPath, tex);
			textures.insert(pair);
		}
		else {
			tex = it->second;
		}
		assert(valElem[i].HasMember("rect"));
		assert(valElem[i]["rect"].IsArray());
		const Value& arrRect = valElem[i]["rect"];
		Animation anim;
		if (arrRect.Size() > 1) {
			for (rapidjson::SizeType i = 0; i < arrRect.Size(); i++)
			{
				const rapidjson::Value& animationFrame = arrRect[i];
				if (animationFrame.Size() != 4) {
					LOG("Error reading config file. Check rect array parameters-----------");
					return false;
				}
				anim.frames.push_back({ animationFrame[0].GetInt(), animationFrame[1].GetInt(), animationFrame[2].GetInt(), animationFrame[3].GetInt() });
			}
			anim.speed = 0.15f;
		}
		else {
			const rapidjson::Value& rectInfor = arrRect[0];
			if (rectInfor.Size() != 4) {
				LOG("Error reading config file. Check rect array parameters-----------");
				return false;
			}
			r = { rectInfor[0].GetInt(), rectInfor[1].GetInt(), rectInfor[2].GetInt(), rectInfor[3].GetInt() };
		}
		assert(valElem[i].HasMember("x"));
		assert(valElem[i]["x"].IsInt());
		elemX = valElem[i]["x"].GetInt();
		assert(valElem[i].HasMember("y"));
		assert(valElem[i]["y"].IsInt());
		elemY = valElem[i]["y"].GetInt();
		assert(valElem[i].HasMember("overHorizon"));
		assert(valElem[i]["overHorizon"].IsInt());
		overHorizon = valElem[i]["overHorizon"].GetInt();
		switch (overHorizon)
		{
		case 0:
			horizon = false;
			break;
		case 1:
			horizon = true;
			break;
		default:
			LOG("Error reading config file -----------");
			return false;
		}
		assert(valElem[i].HasMember("worldPosition"));
		assert(valElem[i]["worldPosition"].IsFloat());
		world = valElem[i]["worldPosition"].GetFloat();
		assert(valElem[i].HasMember("speed"));
		assert(valElem[i]["speed"].IsFloat());
		speed = valElem[i]["speed"].GetFloat();
		assert(valElem[i].HasMember("increment"));
		assert(valElem[i]["increment"].IsFloat());
		increment = valElem[i]["increment"].GetFloat();
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

		float newWorld;
		for (int i = 0; i <= nConsElem; i++) {
			newWorld = world + (increment * i);
			v = new VisualElement(tex, r, anim, elemX, elemY, horizon, speed, nConsElem, newWorld, vpos);
			if (speed != 0.0f) {
				vehicles.push_back(v);
			}
			else {
				std::pair<int, VisualElement*> pair(int(newWorld * 10), v);
				staticVisualElements.insert(pair);
			}
		}
	}

	bottomSegment = stageSegments.at(currentSegment);
	currentSegment++;
	topSegment = stageSegments.at(currentSegment);
	currentSegment++;

	semaphoreFx = App->audio->LoadFx(startRace);

	startTimer = clock(); //Start timer
	App->audio->PlayFx(semaphoreFx);

	//msLog.open("log.txt");

	return true;
}

// UnLoad assets
bool ModuleSceneStage::CleanUp()
{
	LOG("Unloading stage scene");

	for (std::vector<Segment*>::iterator it = stageSegments.begin(); it != stageSegments.end(); ++it)
		delete *it;

	stageSegments.clear();

	topSegment = nullptr;
	bottomSegment = nullptr;

	for (unordered_map<std::string, SDL_Texture*>::iterator it = textures.begin(); it != textures.end(); ++it)
		App->textures->Unload(it->second);

	textures.clear();

	for (unordered_multimap<int, VisualElement*>::iterator it = staticVisualElements.begin(); it != staticVisualElements.end(); ++it)
		delete it->second;

	staticVisualElements.clear();

	App->textures->Unload(background);
	App->textures->Unload(time);

	background = nullptr;

	App->player->Disable();

	currentSegment = 0;
	curveCameraMove = 0;
	semaphoreFx = 0;
	currentLane = 0;
	previousYTopRoad = 0;
	gameState = GameState::STARTING;
	leftTireOut = false;
	rigthTireOut = false;
	firstDraw = true;
	timeToFinish = 55;
	secondsPassed = 0;
	timeToStart = 4;
	startRace = nullptr;

	zMap.clear();
	factorMap.clear();

	fm->Release("greenfont.bmp");
	fm->Release("pinkfont.bmp");
	fm->Release("redfont.bmp");
	fm->End();
	fm = nullptr;
	greenFont = nullptr;
	pinkFont = nullptr;
	redFont = nullptr;

	//msLog.close();
	return true;
}

bool compareVisualElements(VisualElement* i, VisualElement* j) { return (i->world < j->world); }

// Update: draw background
update_status ModuleSceneStage::Update()
{
	secondsPassed = (clock() - startTimer) / CLOCKS_PER_SEC;

	//clock_t beginFrame = clock();
	//int typeOfRoad = 0;

	//Background
	int diff = (previousYTopRoad - 520);
	int adj = int(diff * 0.335f);
	App->renderer->Blit(background, -600, -11 + adj, &back, 0.1f, 0.7f);
	App->renderer->Blit(background, 200, -11 + adj, &back, 0.1f, 0.7f);

	//Road
	float x = SCREEN_WIDTH * SCREEN_SIZE / 2;
	int screenY = SCREEN_HEIGHT * SCREEN_SIZE;
	float scaleFactor;
	float z;

	float dX = 0;
	float ddX = 0;
	float dY = 0;

	float initialRoadSeparation = bottomSegment->roadSeparation;
	float finalRoadSeparation = topSegment->roadSeparation;
	float separationInterval = abs(finalRoadSeparation - initialRoadSeparation);
	float roadSeparation;

	float worldPosition;

	if (topSegment->yMapPosition < 1.0f) {
		topSegment->yMapPosition = 0.0f;
	}

	float segmentFactor = topSegment->yMapPosition / zMap.size();

	bool inTopSegment = false;

	//Draw lines
	std::vector<int> screenYPerWorldPosition;
	std::vector<float> screenXPerWorldPosition;

	//clock_t endDraw = clock();
	//double msDrawRoad = 0;
	//double msChecks = 0;
	for (unsigned int i = 0; i < zMap.size(); i++) {
		z = zMap.at(i);

		scaleFactor = factorMap.at(i);

		if (i < topSegment->yMapPosition) {
			dX = bottomSegment->dX;
			dY = bottomSegment->dY;
		}
		else {
			inTopSegment = true;
			dX = topSegment->dX;
			dY = topSegment->dY;
		}
		ddX += dX;
		x += ddX;
		screenXPerWorldPosition.push_back(x);

		roadSeparation = initialRoadSeparation - (separationInterval * -(-1 + segmentFactor));

		worldPosition = (z * 10) + App->player->position;

		//clock_t roadInit = clock();

		//Check if uphill, downhill or no hill
		if (dY < 0) {
			//typeOfRoad = -1;

			float percentage = 0.0f;
			if (inTopSegment == true) {
				percentage = (float)(i - topSegment->yMapPosition) / (zMap.size() - topSegment->yMapPosition);
			}
			else {
				percentage = (float)i / topSegment->yMapPosition;
			}

			if (percentage < 0.05f) {
				screenY = App->renderer->DrawRoads(screenY, worldPosition, scaleFactor, x, roadSeparation, 3);
			}
			else if (percentage < 0.25f) {
				screenY = App->renderer->DrawRoads(screenY, worldPosition, scaleFactor, x, roadSeparation, 2);
			}
			else if (percentage < 0.5f) {
				screenY = App->renderer->DrawRoads(screenY, worldPosition, scaleFactor, x, roadSeparation);
			}
		}
		else if (dY > 0) {
			//typeOfRoad = 1;

			float percentage = 0.0f;
			if (inTopSegment == true) {
				percentage = (float)(i - topSegment->yMapPosition) / (zMap.size() - topSegment->yMapPosition);
			}
			else {
				percentage = (float)i / topSegment->yMapPosition;
			}

			if (percentage < 0.2f) {
				screenY = App->renderer->DrawRoads(screenY, worldPosition, scaleFactor, x, roadSeparation);
			}
			else if (percentage < 0.7f) {
				screenY = App->renderer->DrawRoads(screenY, worldPosition, scaleFactor, x, roadSeparation, 2);
			}
			else {
				screenY = App->renderer->DrawRoads(screenY, worldPosition, scaleFactor, x, roadSeparation, 3);
			}
		}
		else {
			//typeOfRoad = 0;
			screenY = App->renderer->DrawRoads(screenY, worldPosition, scaleFactor, x, roadSeparation);
		}

		//clock_t roadDraw = clock();

		//If the line we are drawing is the one the tires are placed, check if they are out of the road
		if (screenY == (SCREEN_HEIGHT - 8) * SCREEN_SIZE) {
			leftTireOut = CheckLeftTire(x, scaleFactor, roadSeparation);
			rigthTireOut = CheckRightTire(x, scaleFactor, roadSeparation);
			currentLane = CheckLane(x, scaleFactor, roadSeparation);
		}
		screenYPerWorldPosition.push_back(screenY);

		clock_t endCheck = clock();

		//msDrawRoad += clockToMilliseconds(roadDraw - roadInit);
		//msChecks += clockToMilliseconds(endCheck - roadDraw);
	}
	//msLog << "ms: " << msDrawRoad << "---" << msChecks << endl;

	previousYTopRoad = screenY;

	//clock_t endRoad = clock();

	int height;
	float width;
	VisualElement* vElem;
	std::vector<VisualElement*> elementsDrawn;

	for (int i = zMap.size() - 1; i >= 0; i--) {
		z = zMap.at(i);
		scaleFactor = factorMap.at(i);
		worldPosition = (z * 10) + App->player->position;
		roadSeparation = initialRoadSeparation - (separationInterval * -(-1 + segmentFactor));
		height = screenYPerWorldPosition.at(i);
		width = screenXPerWorldPosition.at(i);
		int key = int(worldPosition * 10);
		auto range = staticVisualElements.equal_range(key);

		for (auto it = range.first; it != range.second; ++it) {
			bool drawn = false;
			vElem = it->second;
			for (std::vector<VisualElement*>::iterator elemIt = elementsDrawn.begin(); elemIt != elementsDrawn.end(); ++elemIt) {
				if (vElem->CopyOf(*elemIt))
					drawn = true;
			}
			if (drawn == false) {
				DrawVisualElement(vElem, width, height, scaleFactor, roadSeparation);
				elementsDrawn.push_back(vElem);
			}
		}

		if (vehicles.size() > 0) {
			unsigned int idx = 0;
			vElem = vehicles.at(idx);
			while (int(vElem->world * 10) <= int(worldPosition * 10)) {
				if (int(vElem->world * 10) == int(worldPosition * 10)) {
					bool drawn = false;
					for (std::vector<VisualElement*>::iterator elemIt = elementsDrawn.begin(); elemIt != elementsDrawn.end(); ++elemIt) {
						if (vElem->CopyOf(*elemIt))
							drawn = true;
					}
					if (drawn == false) {
						DrawVehicle(vElem, width, height, scaleFactor, roadSeparation);
						elementsDrawn.push_back(vElem);
					}
				}
				idx++;
				if (idx < vehicles.size()) {
					vElem = vehicles.at(idx);
				}
				else {
					break;
				}

			}
		}
	}

	//clock_t endVisual = clock();

	//UI
	App->renderer->Blit(time, 10, 5, &timeR, 0.0f, 0.75f);

	if (gameState != GameState::GAMEOVER && gameState != GameState::ENDING && gameState != GameState::BROKEN) {
		double timeLeft = timeToFinish - secondsPassed;
		if (timeLeft < 0)
			timeLeft = 0;

		if (timeLeft == 0)
			gameState = GameState::GAMEOVER;

		timeRemaining = int(timeLeft);
	}

	double kmh;
	std::string aux;
	switch (gameState) {
	case GameState::BROKEN:
		App->renderer->Print(greenFont, 220, 20, to_string(timeRemaining));

		App->renderer->Print(redFont, 750, 20, aux + "0 KMH");
		break;
	case GameState::STARTING:
		if (secondsPassed >= timeToStart)
		{
			startTimer = clock();
			gameState = GameState::PLAYING;
		}

		App->renderer->Print(greenFont, 220, 20, to_string(timeToFinish));

		App->renderer->Print(redFont, 750, 20, aux + "0 KMH");
		break;
	case GameState::PLAYING:
		App->renderer->Print(greenFont, 220, 20, to_string(timeRemaining));

		kmh = App->player->playerSpeed * 1425;
		aux = to_string(int(kmh));
		if (aux.length() > 1) {
			if (aux.length() > 2) {
				App->renderer->Print(redFont, 600, 20, aux + " KMH");
			}
			else {
				App->renderer->Print(redFont, 675, 20, aux + " KMH");
			}
		}
		else {
			App->renderer->Print(redFont, 750, 20, aux + " KMH");
		}
		break;
	case GameState::ENDING:
	case GameState::GAMEOVER:
		App->renderer->Print(greenFont, 220, 20, to_string(timeRemaining));
		break;
	}

	//If still playing or on game over keep updating road and vehicles
	if (gameState == GameState::PLAYING || gameState == GameState::GAMEOVER || gameState == GameState::BROKEN) {
		//Update segments
		App->renderer->camera.x += curveCameraMove;
		topSegment->yMapPosition -= App->player->curveSpeed;
		if (topSegment->yMapPosition < 0) {
			bottomSegment = topSegment;
			if (currentSegment < stageSegments.size()) {
				topSegment = stageSegments.at(currentSegment);
				currentSegment++;
			}
			else {
				if (gameState != GameState::GAMEOVER)
					gameState = GameState::ENDING;

				for (std::vector<VisualElement*>::iterator it = vehicles.begin(); it != vehicles.end(); ++it)
					delete *it;

				vehicles.clear();

				topSegment = new Segment(0.0f, 0.0f, 0.0f, (float)zMap.size(), Inclination::CENTER);
			}
		}

		//Update the camera according to the curve and the speed of the player
		curveCameraMove = 0;
		if (App->player->playerSpeed != 0.0f) {
			if (bottomSegment->dX > 0.0f) {
				float normalize = App->player->playerSpeed / MAX_SPEED;
				if (normalize < 0.2f) {
					curveCameraMove = 0;
				}
				else if (normalize < 0.33f) {
					curveCameraMove = 3;
				}
				else if (normalize < 0.66f) {
					curveCameraMove = 6;
				}
				else {
					curveCameraMove = 9;
				}
			}
			else if (bottomSegment->dX < 0.0f) {
				float normalize = App->player->playerSpeed / MAX_SPEED;
				if (normalize < 0.2f) {
					curveCameraMove = 0;
				}
				else if (normalize < 0.33f) {
					curveCameraMove = -3;
				}
				else if (normalize < 0.66f) {
					curveCameraMove = -6;
				}
				else {
					curveCameraMove = -9;
				}
			}
		}

		//Delete all visual elements that have been surpassed by the player
		if (staticVisualElements.size() > 0) {
			int minWorld = int(((zMap.at(0) * 10) + App->player->position) * 10); //minimum world position seen on screen

			unordered_multimap<int, VisualElement*>::iterator it = staticVisualElements.begin();
			while (it != staticVisualElements.end()) {
				if (it->first < minWorld) {
					delete it->second;
					it = staticVisualElements.erase(it);
				}
				else {
					++it;
				}
			}
		}

		//Update visual elements world position
		for (std::vector<VisualElement*>::iterator it = vehicles.begin(); it != vehicles.end(); ++it) {
			VisualElement* aux = *it;
			aux->world += aux->speed;
		}
		std::sort(vehicles.begin(), vehicles.end(), compareVisualElements);

		//clock_t endFrame = clock();

		/* DEBUGGING 
		double msTotalPassed = clockToMilliseconds(endFrame - beginFrame);
		double msDraw = clockToMilliseconds(endDraw - beginFrame);
		double msRoad = clockToMilliseconds(endRoad - endDraw);
		double msVisual = clockToMilliseconds(endVisual - endRoad);
		double msAdjustments = clockToMilliseconds(endFrame - endVisual);
		msLog << "ms: " << msDraw << " + " << msRoad << " + " << msVisual << " + " << msAdjustments << " = " << msTotalPassed << " - " << typeOfRoad << " - " << endl;
		*/
	}

	if (gameState == GameState::GAMEOVER) {
		if (int(secondsPassed) % 2 == 0)
			App->renderer->Print(pinkFont, 225, 225, "GAME OVER");
	}

	if (gameState == GameState::ENDING) {
		if (int(secondsPassed) % 2 == 0)
			App->renderer->Print(pinkFont, 300, 225, "VICTORY");
	}

	if (gameState == GameState::ENDING || gameState == GameState::GAMEOVER) {
		if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && App->fade->isFading() == false) {
			App->fade->FadeToBlack((Module*)App->scene_intro, this);
		}
	}

	return UPDATE_CONTINUE;
}




void ModuleSceneStage::DrawVisualElement(VisualElement* vElem, float width, int height, float scaleFactor, float roadSeparation)
{
	SDL_Rect rect;
	switch (vElem->position) {
	case VisualElementPosition::LEFT:
		if (vElem->overHorizon == true) {
			if (vElem->anim.frames.size() > 0) {
				for (std::unordered_map<int, VisualElement*>::iterator it = staticVisualElements.begin(); it != staticVisualElements.end(); ++it) {
					VisualElement* aux = it->second;
					if (vElem->CopyOf(it->second)) {
						App->renderer->Blit(vElem->texture, int((width + (vElem->x * scaleFactor)) / SCREEN_SIZE), int(SCREEN_HEIGHT - ((vElem->rect.h * scaleFactor) * 2) - vElem->y), &(vElem->anim.GetCurrentFrame()), scaleFactor, scaleFactor);
					}
				}
			}
			else {
				App->renderer->Blit(vElem->texture, int((width + (vElem->x * scaleFactor)) / SCREEN_SIZE), int(SCREEN_HEIGHT - ((vElem->rect.h * scaleFactor) * 2) - vElem->y), &(vElem->rect), scaleFactor, scaleFactor);
			}
		}
		else {
			if (vElem->anim.frames.size() > 0) {
				for (std::unordered_map<int, VisualElement*>::iterator it = staticVisualElements.begin(); it != staticVisualElements.end(); ++it) {
					VisualElement* aux = it->second;
					if (vElem->CopyOf(it->second)) {
						App->renderer->Blit(aux->texture, int((width + (aux->x * scaleFactor)) / SCREEN_SIZE), int((height / SCREEN_SIZE) - (aux->rect.h * scaleFactor) - aux->y), &(aux->anim.GetCurrentFrame()), scaleFactor, scaleFactor);
					}
				}
			}
			else {
				App->renderer->Blit(vElem->texture, int((width + (vElem->x * scaleFactor)) / SCREEN_SIZE), int((height / SCREEN_SIZE) - (vElem->rect.h * scaleFactor) - vElem->y), &(vElem->rect), scaleFactor, scaleFactor);
			}
		}
		break;
	case VisualElementPosition::CENTER:
		if (vElem->overHorizon == true) {
			if (vElem->anim.frames.size() > 0) {
				for (std::unordered_map<int, VisualElement*>::iterator it = staticVisualElements.begin(); it != staticVisualElements.end(); ++it) {
					VisualElement* aux = it->second;
					if (vElem->CopyOf(it->second)) {
						App->renderer->Blit(aux->texture, int(width / SCREEN_SIZE), int(SCREEN_HEIGHT - (aux->rect.h * scaleFactor) - aux->y), &(aux->rect), scaleFactor, scaleFactor);
					}
				}
			}
			else {
				App->renderer->Blit(vElem->texture, int(width / SCREEN_SIZE), int(SCREEN_HEIGHT - (vElem->rect.h * scaleFactor) - vElem->y), &(vElem->rect), scaleFactor, scaleFactor);
			}
		}
		else {
			if (vElem->anim.frames.size() > 0) {

			}
			else {
				App->renderer->Blit(vElem->texture, int(width / SCREEN_SIZE), int((height / SCREEN_SIZE) - (vElem->rect.h * scaleFactor) - vElem->y), &(vElem->rect), scaleFactor, scaleFactor);
			}
		}
		break;
	case VisualElementPosition::RIGHT:
		if (vElem->overHorizon == true) {
			if (vElem->anim.frames.size() > 0) {

			}
			else {
				App->renderer->Blit(vElem->texture, int((width - (vElem->x * scaleFactor)) / SCREEN_SIZE), int(SCREEN_HEIGHT - (vElem->rect.h * scaleFactor) - vElem->y), &(vElem->rect), scaleFactor, scaleFactor);
			}
		}
		else {
			if (vElem->anim.frames.size() > 0) {

			}
			else {
				App->renderer->Blit(vElem->texture, int((width - (vElem->x * scaleFactor)) / SCREEN_SIZE), int((height / SCREEN_SIZE) - (vElem->rect.h * scaleFactor) - vElem->y), &(vElem->rect), scaleFactor, scaleFactor);
			}
		}
		break;
	case VisualElementPosition::LEFTANDCENTER:
		if (vElem->overHorizon == true) {
			if (vElem->anim.frames.size() > 0) {
				for (std::unordered_map<int, VisualElement*>::iterator it = staticVisualElements.begin(); it != staticVisualElements.end(); ++it) {
					VisualElement* aux = it->second;
					if (vElem->CopyOf(it->second)) {
						App->renderer->Blit(vElem->texture, int((width + (vElem->x * scaleFactor)) / SCREEN_SIZE), int(SCREEN_HEIGHT - ((vElem->rect.h * scaleFactor) * 2) - vElem->y), &(vElem->anim.GetCurrentFrame()), scaleFactor, scaleFactor);
					}
				}
			}
			else {
				App->renderer->Blit(vElem->texture, int((width + (vElem->x * scaleFactor)) / SCREEN_SIZE), int(SCREEN_HEIGHT - (vElem->rect.h * scaleFactor) - vElem->y), &(vElem->rect), scaleFactor, scaleFactor);
			}
		}
		else {
			if (vElem->anim.frames.size() > 0) {
				for (std::unordered_map<int, VisualElement*>::iterator it = staticVisualElements.begin(); it != staticVisualElements.end(); ++it) {
					VisualElement* aux = it->second;
					if (vElem->CopyOf(it->second)) {
						App->renderer->Blit(aux->texture, int((width + (aux->x * scaleFactor)) / SCREEN_SIZE), int((height / SCREEN_SIZE) - (aux->rect.h * scaleFactor) - aux->y), &(aux->anim.GetCurrentFrame()), scaleFactor, scaleFactor);
					}
				}
			}
			else {
				App->renderer->Blit(vElem->texture, int((width + (vElem->x * scaleFactor)) / SCREEN_SIZE), int((height / SCREEN_SIZE) - (vElem->rect.h * scaleFactor) - vElem->y), &(vElem->rect), scaleFactor, scaleFactor);
			}
		}
		rect = vElem->rect;
		rect.x += rect.w;
		if (vElem->overHorizon == true) {
			if (vElem->anim.frames.size() > 0) {
				for (std::unordered_map<int, VisualElement*>::iterator it = staticVisualElements.begin(); it != staticVisualElements.end(); ++it) {
					VisualElement* aux = it->second;
					if (vElem->CopyOf(it->second)) {
						App->renderer->Blit(aux->texture, int(width / SCREEN_SIZE), int(SCREEN_HEIGHT - (aux->rect.h * scaleFactor) - aux->y), &(aux->rect), scaleFactor, scaleFactor);
					}
				}
			}
			else {
				App->renderer->Blit(vElem->texture, int(width / SCREEN_SIZE), int(SCREEN_HEIGHT - (vElem->rect.h * scaleFactor) - vElem->y), &(rect), scaleFactor, scaleFactor);
			}
		}
		else {
			if (vElem->anim.frames.size() > 0) {

			}
			else {
				App->renderer->Blit(vElem->texture, int(width / SCREEN_SIZE), int((height / SCREEN_SIZE) - (vElem->rect.h * scaleFactor) - vElem->y), &(rect), scaleFactor, scaleFactor);
			}
		}
		break;
	case VisualElementPosition::LEFTANDRIGHT:
		if (vElem->overHorizon == true) {
			if (vElem->anim.frames.size() > 0) {
				for (std::unordered_map<int, VisualElement*>::iterator it = staticVisualElements.begin(); it != staticVisualElements.end(); ++it) {
					VisualElement* aux = it->second;
					if (vElem->CopyOf(it->second)) {
						App->renderer->Blit(vElem->texture, int((width + (vElem->x * scaleFactor)) / SCREEN_SIZE), int(SCREEN_HEIGHT - ((vElem->rect.h * scaleFactor) * 2) - vElem->y), &(vElem->anim.GetCurrentFrame()), scaleFactor, scaleFactor);
					}
				}
			}
			else {
				App->renderer->Blit(vElem->texture, int((width + (vElem->x * scaleFactor)) / SCREEN_SIZE), int(SCREEN_HEIGHT - (vElem->rect.h * scaleFactor) - vElem->y), &(vElem->rect), scaleFactor, scaleFactor);
			}
		}
		else {
			if (vElem->anim.frames.size() > 0) {
				for (std::unordered_map<int, VisualElement*>::iterator it = staticVisualElements.begin(); it != staticVisualElements.end(); ++it) {
					VisualElement* aux = it->second;
					if (vElem->CopyOf(it->second)) {
						App->renderer->Blit(aux->texture, int((width + (aux->x * scaleFactor)) / SCREEN_SIZE), int((height / SCREEN_SIZE) - (aux->rect.h * scaleFactor) - aux->y), &(aux->anim.GetCurrentFrame()), scaleFactor, scaleFactor);
					}
				}
			}
			else {
				App->renderer->Blit(vElem->texture, int((width + (vElem->x * scaleFactor)) / SCREEN_SIZE), int((height / SCREEN_SIZE) - (vElem->rect.h * scaleFactor) - vElem->y), &(vElem->rect), scaleFactor, scaleFactor);
			}
		}
		rect = vElem->rect;
		rect.x += rect.w;
		if (vElem->overHorizon == true) {
			if (vElem->anim.frames.size() > 0) {

			}
			else {
				App->renderer->Blit(vElem->texture, int((width - (vElem->x * scaleFactor)) / SCREEN_SIZE), int(SCREEN_HEIGHT - (vElem->rect.h * scaleFactor) - vElem->y), &(vElem->rect), scaleFactor, scaleFactor);
			}
		}
		else {
			if (vElem->anim.frames.size() > 0) {

			}
			else {
				App->renderer->Blit(vElem->texture, int((width - (vElem->x * scaleFactor)) / SCREEN_SIZE), int((height / SCREEN_SIZE) - (vElem->rect.h * scaleFactor) - vElem->y), &(vElem->rect), scaleFactor, scaleFactor);
			}
		}
		break;
	case VisualElementPosition::CENTERANDRIGHT:
		rect = vElem->rect;
		rect.x += rect.w;
		if (vElem->overHorizon == true) {
			if (vElem->anim.frames.size() > 0) {
				for (std::unordered_map<int, VisualElement*>::iterator it = staticVisualElements.begin(); it != staticVisualElements.end(); ++it) {
					VisualElement* aux = it->second;
					if (vElem->CopyOf(it->second)) {
						App->renderer->Blit(aux->texture, int(width / SCREEN_SIZE), int(SCREEN_HEIGHT - (aux->rect.h * scaleFactor) - aux->y), &(aux->rect), scaleFactor, scaleFactor);
					}
				}
			}
			else {
				App->renderer->Blit(vElem->texture, int(width / (SCREEN_SIZE * SCREEN_SIZE)), int(SCREEN_HEIGHT - (vElem->rect.h * scaleFactor) - vElem->y), &(rect), scaleFactor, scaleFactor);
			}
		}
		else {
			if (vElem->anim.frames.size() > 0) {

			}
			else {
				App->renderer->Blit(vElem->texture, int((width + (roadSeparation * scaleFactor / 2)) / SCREEN_SIZE), int((height / SCREEN_SIZE) - (vElem->rect.h * scaleFactor) - vElem->y), &(rect), scaleFactor, scaleFactor);
			}
		}
		App->renderer->Blit(vElem->texture, int(-vElem->x + (SCREEN_WIDTH * scaleFactor / 2)), int(vElem->y * (2 - scaleFactor)), &(vElem->rect), 0.5f, scaleFactor);
		break;
	case VisualElementPosition::ALL:
		break;
	}
}

void ModuleSceneStage::DrawVehicle(VisualElement* vElem, float width, int height, float scaleFactor, float roadSeparation)
{
	unsigned int vehicleLane = 0;
	switch (vElem->x) {
	case -1300:
		vehicleLane = 1;
		break;
	case -825:
		vehicleLane = 2;
		break;
	case -350:
		vehicleLane = 3;
		break;
	case 125:
		vehicleLane = 4;
		break;
	case 600:
		vehicleLane = 5;
		break;
	case 1075:
		vehicleLane = 6;
	}

	int carIdx = 0;
	int difBetweenLanes = currentLane - vehicleLane;
	switch (difBetweenLanes) {
	case -1:
		carIdx = 3;
		break;
	case -2:
		carIdx = 4;
		break;
	case -3:
	case -4:
	case -5:
		carIdx = 5;
		break;
	case 0:
	case 1:
		carIdx = 0;
		break;
	case 2:
		carIdx = 1;
		break;
	case 3:
	case 4:
	case 5:
		carIdx = 2;
		break;
	}

	SDL_Rect rect = vElem->anim.frames.at(carIdx);
	if (vElem->x < 125) {
		App->renderer->Blit(vElem->texture, int((width + (vElem->x * scaleFactor)) / SCREEN_SIZE), int((height / SCREEN_SIZE) - (rect.h * scaleFactor) - vElem->y), &(rect), scaleFactor, scaleFactor);
	}
	else {
		App->renderer->Blit(vElem->texture, int(((width + ((vElem->x * scaleFactor))) + (roadSeparation * scaleFactor)) / SCREEN_SIZE), int((height / SCREEN_SIZE) - (rect.h * scaleFactor) - vElem->y), &(rect), scaleFactor, scaleFactor);
	}
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

unsigned int ModuleSceneStage::CheckLane(float x, float scaleFactor, float roadSeparation)
{
	int carX = (App->player->carX * SCREEN_SIZE) + App->renderer->camera.x + 95;
	float drawX = x + App->renderer->camera.x * scaleFactor;

	if (carX < ((drawX + (App->renderer->secondRoadX * scaleFactor)) - (ROADWIDTH / 2))) {
		return 1;
	}
	else if (carX < ((drawX + (App->renderer->thirdRoadX * scaleFactor)) - (ROADWIDTH / 2))) {
		return 2;
	}
	else if (carX < ((drawX + ((App->renderer->fourthRoadX + roadSeparation) * scaleFactor)) - (ROADWIDTH / 2))) {
		return 3;
	}
	else if (carX < ((drawX + ((App->renderer->fifthRoadX + roadSeparation) * scaleFactor)) - (ROADWIDTH / 2))) {
		return 4;
	}
	else if (carX < ((drawX + ((App->renderer->sixthRoadX + roadSeparation)  * scaleFactor)) - (ROADWIDTH / 2))) {
		return 5;
	}
	else {
		return 6;
	}
}

