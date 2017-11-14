#ifndef __MODULESCENESTAGE_H__
#define __MODULESCENESTAGE_H__

#define roadHeightWorld -200
#define roadHeightScreen 300

#define roadWidth 500
#define rumbleWidth 50
#define terrainWidth 700
#define segmentLength 200
#define FOV 60

#include <cmath>
#include "Module.h"

struct SDL_Texture;

struct Line
{
	float x, y, z;
	float X, Y, W;
	float scale;

	Line() { x = 0; y = 0; z = 0; }
	Line(float x, float y, float z) : x(x), y(y), z(z) {}
	/*
	void Project(int cameraX, int cameraY, int cameraZ)
	{
		scale = (SCREEN_WIDTH / 2) / tan(FOV / 2);
		X = (y * scale)/z  + (SCREEN_WIDTH / 2);
		Y = (y / z) + SCREEN_HEIGHT / 2;
		W = scale * roadWidth  * SCREEN_WIDTH / 2;
	}
	*/
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
	std::vector<Line*> roadLines;
	std::vector<float> zMap;
	SDL_Color* sand = nullptr;
	SDL_Color* rumble = nullptr;
	SDL_Color* road = nullptr;

};

#endif // __MODULESCENESTAGE_H__