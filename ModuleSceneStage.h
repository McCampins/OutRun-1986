#ifndef __MODULESCENESTAGE_H__
#define __MODULESCENESTAGE_H__

#define ROADHEIGHTWORLD -200
#define ROADHEIGHTSCREEN 200

#define ROADWIDTH 450
#define RUMBLEWIDTH 50
#define LINEWIDTH 24
#define TERRAINWIDTH 6000

#define SOFTLEFTCURVE -0.01f
#define SOFTRIGHTCURVE 0.01f
#define HARDLEFTCURVE -0.02f
#define HARDRIGHTCURVE 0.02f
#define UPHILL 1.0f
#define DOWNHILL -1.0f

#define VISUALELEMENTDISTANCE 2.0f

#include <cmath>
#include <unordered_map>
#include "Module.h"

struct SDL_Texture;

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
	int x;
	int y;
	float worldPosition;
	int nConsecutiveElements; 
	VisualElementPosition position;

	VisualElement() {};
	VisualElement(SDL_Texture* texture, SDL_Rect rect, int x, int y, float worldPosition, int nConsecutiveElements, VisualElementPosition position) : 
		texture(texture), rect(rect), x(x), y(y),	worldPosition(worldPosition), nConsecutiveElements(nConsecutiveElements), position(position) {};
};

class ModuleSceneStage : public Module
{
public:
	ModuleSceneStage(bool active = true);
	~ModuleSceneStage();

	bool Start();
	update_status Update();
	bool CleanUp();

	int DrawRoads(int screenY, float worldPosition, float scaleFactor, float x, float roadSeparation);
	bool CheckLeftTire(float x, float scaleFactor, float roadSeparation);
	bool CheckRightTire(float x, float scaleFactor, float roadSeparation);

public:
	SDL_Texture* background = nullptr; 
	SDL_Texture* startFlag = nullptr;
	std::vector<float> zMap;
	std::vector<float> factorMap;
	std::vector<Segment> stageSegments;
	unsigned int currentSegment = 0;
	Segment topSegment;
	Segment bottomSegment;
	int curveCameraMove = 0;
	bool leftTireOut = false;
	bool rigthTireOut = false;
	std::unordered_map<const char*, std::pair<SDL_Texture*, SDL_Rect>> textures;
	std::vector<VisualElement> elements;
};

#endif // __MODULESCENESTAGE_H__