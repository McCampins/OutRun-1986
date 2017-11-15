#ifndef __MODULESCENESTAGE_H__
#define __MODULESCENESTAGE_H__

#define roadHeightWorld -200
#define roadHeightScreen 200

#define roadWidth 250
#define rumbleWidth 50
#define lineWidth 24
#define terrainWidth 4000

#include <cmath>
#include "Module.h"

struct SDL_Texture;

struct Segment {
	float dX;
	float dY;
	float roadSeparation;
	float yMapPosition;
	float axisModifier;

	Segment() {};
	Segment(float dX, float dY, float roadSeparation, float yMap, float axisModifier) : dX(dX), dY(dY), roadSeparation(roadSeparation), yMapPosition(yMap), axisModifier(axisModifier) {};
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
	std::vector<float> zMap;
	std::vector<Segment> stageSegments;
	unsigned int currentSegment = 0;
	Segment topSegment;
	Segment bottomSegment;
};

#endif // __MODULESCENESTAGE_H__