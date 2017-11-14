#ifndef __MODULESCENESTAGE_H__
#define __MODULESCENESTAGE_H__

#define roadHeightWorld -200
#define roadHeightScreen 300

#define roadWidth 1000
#define rumbleWidth 50
#define terrainWidth 2000
#define segmentLength 200

#include <cmath>
#include "Module.h"

struct SDL_Texture;

struct Segment {
	float dX;
	float yMapPosition;

	Segment() {};
	Segment(float dX, float yMap) : dX(dX), yMapPosition(yMap) {};
};



class ModuleSceneStage : public Module
{
public:
	ModuleSceneStage(bool active = true);
	~ModuleSceneStage();

	bool Start();
	update_status Update();
	bool CleanUp();

public:
	SDL_Texture* background = nullptr;
	std::vector<float> zMap;
	std::vector<Segment> stageSegments;
	int currentSegment = 0;
	Segment topSegment;
	Segment bottomSegment;
};

#endif // __MODULESCENESTAGE_H__