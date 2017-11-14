#ifndef __MODULESCENESTAGE_H__
#define __MODULESCENESTAGE_H__

#define roadHeightWorld -200
#define roadHeightScreen 300

#define roadWidth 500
#define rumbleWidth 50
#define terrainWidth 1000
#define segmentLength 200

#include <cmath>
#include "Module.h"

struct SDL_Texture;

struct Segment {
	float dX;
	float zMapPosition;

	Segment() {};
	Segment(float dX, float zMap) : dX(dX), zMapPosition(zMap) {};
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
	Segment topSegment;
	Segment bottomSegment;
};

#endif // __MODULESCENESTAGE_H__