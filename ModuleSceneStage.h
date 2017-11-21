#ifndef __MODULESCENESTAGE_H__
#define __MODULESCENESTAGE_H__

#define roadHeightWorld -200
#define roadHeightScreen 200

#define roadWidth 450
#define rumbleWidth 50
#define lineWidth 24
#define terrainWidth 4000

#define SOFTLEFTCURVE -0.01f
#define SOFTRIGHTCURVE 0.01f
#define HARDLEFTCURVE -0.02f
#define HARDRIGHTCURVE 0.02f
#define UPHILL 1.0f
#define DOWNHILL -1.0f

#include <cmath>
#include "Module.h"

struct SDL_Texture;

enum class CameraMove {
	LEFTTOLEFT,
	LEFTTOCENTER,
	CENTERTOLEFT,
	CENTERTOCENTER,
	CENTERTORIGHT,
	RIGHTTORIGHT,
	RIGHTTOCENTER
};

enum class Inclination {
	UP,
	CENTER,
	DOWN
};

struct Segment {
	float dX;
	float dY;
	float roadSeparation;
	float yMapPosition;
	CameraMove pos;
	Inclination inc;

	Segment() {};
	Segment(float dX, float dY, float roadSeparation, float yMap, CameraMove pos, Inclination inc) : dX(dX), dY(dY), roadSeparation(roadSeparation), yMapPosition(yMap), pos(pos), inc(inc) {};
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

public:
	SDL_Texture* background = nullptr;
	SDL_Texture* player = nullptr;
	std::vector<float> zMap;
	std::vector<float> factorMap;
	std::vector<Segment> stageSegments;
	unsigned int currentSegment = 0;
	Segment topSegment;
	Segment bottomSegment;
};

#endif // __MODULESCENESTAGE_H__