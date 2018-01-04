#ifndef __MODULERENDER_H__
#define __MODULERENDER_H__

#define ROADWIDTH 450
#define RUMBLEWIDTH 50
#define LINEWIDTH 24
#define TERRAINWIDTH 6000

#include "Module.h"
#include "Globals.h"
#include "ModuleFont.h"

#include <string>

struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;

class ModuleRender : public Module
{
public:
	ModuleRender();
	~ModuleRender();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

	bool Blit(SDL_Texture* texture, int x, int y, SDL_Rect* section, float curveSpeed = 1.0f, float scale = 1.0f);
	bool Print(const ModuleFont* font, const int x, const int y, const string text);
	bool DrawRect(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera = true);
	bool DrawQuad(int x1, int y1, int w1, int x2, int y2, int w2, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
	bool DrawHorizontalLine(int x, int y, int w, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
	int DrawRoads(int screenY, float worldPosition, float scaleFactor, float x, float roadSeparation, int times = 1);

public:
	SDL_Renderer* renderer = nullptr;
	SDL_Rect camera;
	//Road lines parameters
	float leftRoadRumbleX = 0.0f;
	float leftRoadLinesX = 0.0f;
	float rightRoadRumbleX = 0.0f;
	float rightRoadLinesX = 0.0f;
	float firstRoadX = 0.0f;
	float secondRoadX = 0.0f;
	float thirdRoadX = 0.0f;
	float fourthRoadX = 0.0f;
	float fifthRoadX = 0.0f;
	float sixthRoadX = 0.0f;
};

#endif // __MODULERENDER_H__