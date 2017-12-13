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

double clockToMilliseconds(clock_t ticks) {
	// units/(units/time) => time (seconds) * 1000 = milliseconds
	return (ticks / (double)CLOCKS_PER_SEC)*1000.0;
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
	int elemX, elemY, overHorizon, nConsElem;
	float world;
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

		v = { tex, r, anim, elemX, elemY, horizon, nConsElem, world, vpos };
		elements.push_back(v);
	}

	bottomSegment = stageSegments.at(currentSegment);
	currentSegment++;
	topSegment = stageSegments.at(currentSegment);
	currentSegment++;

	msLog.open("log.txt");
	
	return true;
}

// UnLoad assets
bool ModuleSceneStage::CleanUp()
{
	LOG("Unloading space scene");

	for (unordered_map<std::string, SDL_Texture*>::iterator it = textures.begin(); it != textures.end(); ++it)
		App->textures->Unload(it->second);

	//App->textures->Unload(background);
	App->player->Disable();
	App->collision->Disable();
	App->particles->Disable();

	msLog.close();
	return true;
}

// Update: draw background
update_status ModuleSceneStage::Update()
{
	clock_t beginFrame = clock();
	int typeOfRoad = 0;

	//Road
	float x = SCREEN_WIDTH * SCREEN_SIZE / 2;
	int screenY = SCREEN_HEIGHT * SCREEN_SIZE;
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

	//Draw lines
	std::vector<int> screenYPerWorldPosition;
	std::vector<float> screenXPerWorldPosition;

	std::vector<VisualElement> elementsToDraw;
	z = zMap.at(0);
	float minPosition = (z * 10) + App->player->position;
	z = zMap.at(zMap.size() - 1);
	float maxPosition = (z * 10) + App->player->position;
	VisualElement vElem;

	unsigned int n = 0;
	vElem = elements.at(n);

	while (int(vElem.worldPosition * 10) <= int(maxPosition * 10)) {
		if (int(minPosition * 10) <= int((vElem.worldPosition + vElem.nConsecutiveElements) * 10)) {
			elementsToDraw.push_back(vElem);
			if (vElem.nConsecutiveElements > 0) {
				bool insideRange = true;
				for (int i = 1; i <= vElem.nConsecutiveElements && insideRange == true; i++) {
					vElem.worldPosition += (float)i;
					if (int((vElem.worldPosition * 10)) <= int((maxPosition * 10))) {
						elementsToDraw.push_back(vElem);
					}
					else {
						insideRange = false;
					}
					vElem.worldPosition -= (float)i;
				}
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

	clock_t endDraw = clock();
	double totalDraw = 0;
	double msInitRoad = 0;
	double msDrawRoad = 0;
	double msEndRoad = 0;
	for (unsigned int i = 0; i < zMap.size(); i++) {
		clock_t roadStart = clock();

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
		screenXPerWorldPosition.push_back(x);

		roadSeparation = initialRoadSeparation - (separationInterval * -(-1 + segmentFactor));

		worldPosition = (z * 10) + App->player->position;

		clock_t roadInit = clock();

		//Check if uphill, downhill or no hill
		if (dY < 0) {
			typeOfRoad = -1;

			float percentage = 0.0f;
			if (inTopSegment == true) {
				percentage = (float)(i - topSegment.yMapPosition) / (zMap.size() - topSegment.yMapPosition);
			}
			else {
				percentage = (float)i / topSegment.yMapPosition;
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
			typeOfRoad = 1;

			float percentage = 0.0f;
			if (inTopSegment == true) {
				percentage = (float)(i - topSegment.yMapPosition) / (zMap.size() - topSegment.yMapPosition);
			}
			else {
				percentage = (float)i / topSegment.yMapPosition;
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
			typeOfRoad = 0;
			screenY = App->renderer->DrawRoads(screenY, worldPosition, scaleFactor, x, roadSeparation);
		}

		clock_t roadDraw = clock();

		//If the line we are drawing is the one the tires are placed, check if they are out of the road
		if (screenY == (SCREEN_HEIGHT - 8) * SCREEN_SIZE) {
			leftTireOut = CheckLeftTire(x, scaleFactor, roadSeparation);
			rigthTireOut = CheckRightTire(x, scaleFactor, roadSeparation);
		}
		screenYPerWorldPosition.push_back(screenY);

		clock_t roadTire = clock();

		msInitRoad += clockToMilliseconds(roadInit - roadStart);
		msDrawRoad += clockToMilliseconds(roadDraw - roadInit);
		msEndRoad += clockToMilliseconds(roadTire - roadDraw);
	}
	//msLog << "ms: " << msInitRoad << " + " << msDrawRoad << " + " << msEndRoad << " --- " << typeOfRoad << endl;
	totalDraw = msInitRoad + msDrawRoad + msEndRoad;
	//msLog << "---------\n\t" << totalDraw << "\n------------" << endl;

	clock_t endRoad = clock();

	int height;
	float width;
	float screenFactor = 0.0f;

	for (int i = zMap.size() - 1; i >= 0; i--) {
		z = zMap.at(i);
		scaleFactor = factorMap.at(i);
		worldPosition = (z * 10) + App->player->position;
		roadSeparation = initialRoadSeparation - (separationInterval * -(-1 + segmentFactor));
		height = screenYPerWorldPosition.at(i);
		width = screenXPerWorldPosition.at(i);

		n = 0;
		if (elementsToDraw.size() > 0) {
			vElem = elementsToDraw.at(n);
			while (n < elementsToDraw.size()) {
				if (int(worldPosition * 10) == int(vElem.worldPosition * 10) && elementDrawn.at(n) == false) {
					SDL_Rect rect;
					switch (vElem.position) {
					case VisualElementPosition::LEFT:
						if (vElem.overHorizon == true) {
							if (vElem.anim.frames.size() > 0) {
								for (std::vector<VisualElement>::iterator it = elements.begin(); it != elements.end(); ++it) {
									VisualElement* aux = &(*it);
									if (vElem.CopyOf(*it)) {
										App->renderer->Blit(vElem.texture, int((width + (vElem.x * scaleFactor)) / SCREEN_SIZE), int(SCREEN_HEIGHT - ((vElem.rect.h * scaleFactor) * 2) - vElem.y), &(vElem.anim.GetCurrentFrame()), scaleFactor, scaleFactor);
									}
								}
							}
							else {
								App->renderer->Blit(vElem.texture, int((width + (vElem.x * scaleFactor)) / SCREEN_SIZE), int(SCREEN_HEIGHT - ((vElem.rect.h * scaleFactor) * 2) - vElem.y), &(vElem.rect), scaleFactor, scaleFactor);
							}
						}
						else {
							if (vElem.anim.frames.size() > 0) {
								for (std::vector<VisualElement>::iterator it = elements.begin(); it != elements.end(); ++it) {
									VisualElement* aux = &(*it);
									if (vElem.CopyOf(*it)) {
										App->renderer->Blit(aux->texture, int((width + (aux->x * scaleFactor)) / SCREEN_SIZE), int((height / SCREEN_SIZE) - (aux->rect.h * scaleFactor) - aux->y), &(aux->anim.GetCurrentFrame()), scaleFactor, scaleFactor);
									}
								}
							}
							else {
								App->renderer->Blit(vElem.texture, int((width + (vElem.x * scaleFactor)) / SCREEN_SIZE), int((height / SCREEN_SIZE) - (vElem.rect.h * scaleFactor) - vElem.y), &(vElem.rect), scaleFactor, scaleFactor);
							}
						}
						break;
					case VisualElementPosition::CENTER:
						if (vElem.overHorizon == true) {
							if (vElem.anim.frames.size() > 0) {
								for (std::vector<VisualElement>::iterator it = elements.begin(); it != elements.end(); ++it) {
									VisualElement* aux = &(*it);
									if (vElem.CopyOf(*it)) {
										App->renderer->Blit(aux->texture, int(width / SCREEN_SIZE), int(SCREEN_HEIGHT - (aux->rect.h * scaleFactor) - aux->y), &(aux->rect), scaleFactor, scaleFactor);
									}
								}
							}
							else {
								App->renderer->Blit(vElem.texture, int(width / SCREEN_SIZE), int(SCREEN_HEIGHT - (vElem.rect.h * scaleFactor) - vElem.y), &(vElem.rect), scaleFactor, scaleFactor);
							}
						}
						else {
							if (vElem.anim.frames.size() > 0) {

							}
							else {
								App->renderer->Blit(vElem.texture, int(width / SCREEN_SIZE), int((height / SCREEN_SIZE) - (vElem.rect.h * scaleFactor) - vElem.y), &(vElem.rect), scaleFactor, scaleFactor);
							}
						}
						break;
					case VisualElementPosition::RIGHT:
						rect = vElem.rect;
						rect.x += rect.w;
						App->renderer->Blit(vElem.texture, int(-vElem.x + (SCREEN_WIDTH * scaleFactor / 2)), int(vElem.y * (2 - scaleFactor)), &(vElem.rect), 0.5f, scaleFactor);
						break;
					case VisualElementPosition::LEFTANDCENTER:
						if (vElem.overHorizon == true) {
							if (vElem.anim.frames.size() > 0) {

							}
							else {
								App->renderer->Blit(vElem.texture, int((width + (vElem.x * scaleFactor)) / SCREEN_SIZE), int(SCREEN_HEIGHT - (vElem.rect.h * scaleFactor) - vElem.y), &(vElem.rect), scaleFactor, scaleFactor);
							}
						}
						else {
							if (vElem.anim.frames.size() > 0) {

							}
							else {
								App->renderer->Blit(vElem.texture, int((width + (vElem.x * scaleFactor)) / SCREEN_SIZE), int((height / SCREEN_SIZE) - (vElem.rect.h * scaleFactor) - vElem.y), &(vElem.rect), scaleFactor, scaleFactor);
							}
						}
						rect = vElem.rect;
						rect.x += rect.w;
						if (vElem.overHorizon == true) {
							if (vElem.anim.frames.size() > 0) {

							}
							else {
								App->renderer->Blit(vElem.texture, int(width / SCREEN_SIZE), int(SCREEN_HEIGHT - (vElem.rect.h * scaleFactor) - vElem.y), &(rect), scaleFactor, scaleFactor);
							}
						}
						else {
							if (vElem.anim.frames.size() > 0) {

							}
							else {
								App->renderer->Blit(vElem.texture, int(width / SCREEN_SIZE), int((height / SCREEN_SIZE) - (vElem.rect.h * scaleFactor) - vElem.y), &(rect), scaleFactor, scaleFactor);
							}
						}
						break;
					case VisualElementPosition::LEFTANDRIGHT:
						if (vElem.overHorizon == true) {
							if (vElem.anim.frames.size() > 0) {

							}
							else {
								App->renderer->Blit(vElem.texture, int((width + (vElem.x * scaleFactor)) / SCREEN_SIZE), int(SCREEN_HEIGHT - (vElem.rect.h * scaleFactor) - vElem.y), &(vElem.rect), scaleFactor, scaleFactor);
							}
						}
						else {
							App->renderer->Blit(vElem.texture, int((width + (vElem.x * scaleFactor)) / SCREEN_SIZE), int((height / SCREEN_SIZE) - (vElem.rect.h * scaleFactor) - vElem.y), &(vElem.rect), scaleFactor, scaleFactor);

						}
						rect = vElem.rect;
						rect.x += rect.w;
						App->renderer->Blit(vElem.texture, int(-vElem.x + (SCREEN_WIDTH * scaleFactor / 2)), int(vElem.y * (2 - scaleFactor)), &(vElem.rect), 0.5f, scaleFactor);
						break;
					case VisualElementPosition::CENTERANDRIGHT:
						rect = vElem.rect;
						rect.x += rect.w;
						if (vElem.overHorizon == true) {
							if (vElem.anim.frames.size() > 0) {

							}
							else {
								App->renderer->Blit(vElem.texture, int(width / (SCREEN_SIZE * SCREEN_SIZE)), int(SCREEN_HEIGHT - (vElem.rect.h * scaleFactor) - vElem.y), &(rect), scaleFactor, scaleFactor);
							}
						}
						else {
							if (vElem.anim.frames.size() > 0) {

							}
							else {
								App->renderer->Blit(vElem.texture, int((width + (roadSeparation * scaleFactor / 2)) / SCREEN_SIZE), int((height / SCREEN_SIZE) - (vElem.rect.h * scaleFactor) - vElem.y), &(rect), scaleFactor, scaleFactor);
							}
						}
						App->renderer->Blit(vElem.texture, int(-vElem.x + (SCREEN_WIDTH * scaleFactor / 2)), int(vElem.y * (2 - scaleFactor)), &(vElem.rect), 0.5f, scaleFactor);
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
	}

	clock_t endVisual = clock();

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
				curveCameraMove = 12;
			}
		}
		else if (bottomSegment.dX < 0.0f) {
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
				curveCameraMove = -12;
			}
		}
	}

	clock_t endFrame = clock();
	double msTotalPassed = clockToMilliseconds(endFrame - beginFrame);
	double msDraw = clockToMilliseconds(endDraw - beginFrame);
	double msRoad = clockToMilliseconds(endRoad - endDraw);
	double msVisual = clockToMilliseconds(endVisual - endRoad);
	double msAdjustments = clockToMilliseconds(endFrame - endVisual);
	msLog << "ms: " << msDraw << " + " << msRoad << " + " << msVisual << " + " << msAdjustments << " = " << msTotalPassed << " - " << typeOfRoad << " - " << endl;

	return UPDATE_CONTINUE;
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

