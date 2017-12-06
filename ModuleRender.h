#ifndef __MODULERENDER_H__
#define __MODULERENDER_H__

#include "Module.h"
#include "Globals.h"

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
	bool DrawRect(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera = true);
	bool DrawQuad(int x1, int y1, int w1, int x2, int y2, int w2, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
	bool DrawHorizontalLine(float x, int y, float w, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

public:
	SDL_Renderer* renderer = nullptr;
	SDL_Rect camera;
	int initCameraX = 710;
};

#endif // __MODULERENDER_H__