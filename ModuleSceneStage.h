#ifndef __MODULESCENESTAGE_H__
#define __MODULESCENESTAGE_H__

#define ROADHEIGHTWORLD -200
#define ROADHEIGHTSCREEN 200

#define SOFTLEFTCURVE -0.01f
#define SOFTRIGHTCURVE 0.01f
#define HARDLEFTCURVE -0.02f
#define HARDRIGHTCURVE 0.02f
#define UPHILL 1.0f
#define DOWNHILL -1.0f

#define STARTINGCAMERA 710

#include <cmath>
#include <unordered_map>
#include <ctime>
#include <fstream>

#include "Module.h"

struct SDL_Texture;
class FontManager;

enum class Inclination {
	UP,
	CENTER,
	DOWN
};

enum class VisualElementPosition {
	LEFT,
	CENTER,
	RIGHT,
	LEFTANDRIGHT,
	LEFTANDCENTER,
	CENTERANDRIGHT,
	ALL
};

enum class GameState {
	STARTING,
	PLAYING,
	GAMEOVER,
	ENDING
};

struct Segment {
	float dX;
	float dY;
	float roadSeparation;
	float yMapPosition;
	Inclination inc;

	Segment() {};
	Segment(float dX, float dY, float roadSeparation, float yMap, Inclination inc) : dX(dX), dY(dY), roadSeparation(roadSeparation), yMapPosition(yMap), inc(inc) {};
};

struct VisualElement {
	SDL_Texture* texture;
	SDL_Rect rect;
	Animation anim;
	int x;
	int y;
	bool overHorizon;
	float speed;
	int nConsecutiveElements;
	float world;
	VisualElementPosition position;

	VisualElement() {};
	VisualElement(SDL_Texture* texture, SDL_Rect rect, Animation anim, int x, int y, bool overHorizon, float speed, int nConsecutiveElements, float world, VisualElementPosition position) :
		texture(texture), rect(rect), anim(anim), x(x), y(y), overHorizon(overHorizon), speed(speed), nConsecutiveElements(nConsecutiveElements), world(world), position(position) {};

	bool CopyOf(VisualElement* ve) {
		if (rect.x != ve->rect.x || rect.y != ve->rect.y || rect.w != ve->rect.w || rect.h != ve->rect.h)
			return false;

		if (x != ve->x)
			return false;

		if (y != ve->y)
			return false;

		if (overHorizon != ve->overHorizon)
			return false;

		if (nConsecutiveElements != ve->nConsecutiveElements)
			return false;

		if (int(world * 10) != int(ve->world * 10))
			return false;

		if (position != ve->position)
			return false;

		return true;
	}
};

class ModuleSceneStage : public Module
{
public:
	ModuleSceneStage(bool active = true);
	~ModuleSceneStage();

	bool Start();
	update_status Update();
	bool CleanUp();

	void DrawVisualElement(VisualElement* vElem, float width, int height, float scaleFactor, float roadSeparation);
	void DrawVehicle(VisualElement* vElem, float width, int height, float scaleFactor, float roadSeparation);

	bool CheckLeftTire(float x, float scaleFactor, float roadSeparation);
	bool CheckRightTire(float x, float scaleFactor, float roadSeparation);
	unsigned int CheckLane(float x, float scaleFactor, float roadSeparation);

public:
	//textures
	SDL_Texture* background = nullptr;
	SDL_Texture* time = nullptr;
	SDL_Texture* score = nullptr;
	SDL_Texture* lap = nullptr;
	SDL_Rect back;
	SDL_Rect timeR;
	SDL_Rect scoreR;
	SDL_Rect lapR;
	FontManager* fm;
	const Font* font;
	//Road info
	std::vector<float> zMap;
	std::vector<float> factorMap;
	std::vector<Segment*> stageSegments;
	unsigned int currentSegment = 0;
	Segment* topSegment;
	Segment* bottomSegment;
	int curveCameraMove = 0;
	bool leftTireOut = false;
	bool rigthTireOut = false;
	unsigned int previousYTopRoad = 0;
	//Visual elements info
	std::unordered_map<std::string, SDL_Texture*> textures;
	std::unordered_multimap<int, VisualElement*> staticVisualElements;
	std::vector<VisualElement*> vehicles;
	//Performance log
	ofstream msLog;
	//car player info
	unsigned int currentLane = 0;
	//game info
	GameState gameState = GameState::STARTING;
	int timeLeft = 65;
	bool firstDraw = true;
	double secondsPassed = 0;
	double timeToStart = 4;
	clock_t startTimer;
	const char* startRace = nullptr;
	unsigned int semaphoreFx = 0;
};

#endif // __MODULESCENESTAGE_H__