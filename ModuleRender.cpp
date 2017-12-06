#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "SDL/include/SDL.h"

ModuleRender::ModuleRender()
{
	camera.x = camera.y = 0;
	camera.w = SCREEN_WIDTH * SCREEN_SIZE;
	camera.h = SCREEN_HEIGHT* SCREEN_SIZE;
}

// Destructor
ModuleRender::~ModuleRender()
{
}

// Called before render is available
bool ModuleRender::Init()
{
	LOG("Creating Renderer context");
	bool ret = true;
	Uint32 flags = 0;

	if (VSYNC == true)
	{
		flags |= SDL_RENDERER_PRESENTVSYNC;
	}

	renderer = SDL_CreateRenderer(App->window->window, -1, flags);

	if (renderer == nullptr)
	{
		LOG("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	camera.x = initCameraX;

	return ret;
}

update_status ModuleRender::PreUpdate()
{	
	SDL_SetRenderDrawColor(renderer, 0, 191, 255, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);
	return UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleRender::Update()
{
	// debug camera
	int curveSpeed = 3;

	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		App->renderer->camera.y += curveSpeed;

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		App->renderer->camera.y -= curveSpeed;

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		App->renderer->camera.x += curveSpeed;

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		App->renderer->camera.x -= curveSpeed;

	return UPDATE_CONTINUE;
}

update_status ModuleRender::PostUpdate()
{
	SDL_RenderPresent(renderer);
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRender::CleanUp()
{
	LOG("Destroying renderer");

	//Destroy window
	if (renderer != nullptr)
	{
		SDL_DestroyRenderer(renderer);
	}

	return true;
}

// Blit to screen
bool ModuleRender::Blit(SDL_Texture* texture, int x, int y, SDL_Rect* section, float curveSpeed, float scale)
{
	bool ret = true;
	SDL_Rect rect;
	rect.x = (int)(camera.x * curveSpeed) + x * SCREEN_SIZE;
	rect.y = (int)(camera.y * curveSpeed) + y * SCREEN_SIZE;

	if (section != NULL)
	{
		rect.w = (int) ceil(section->w * scale);
		rect.h = (int) ceil(section->h * scale);
	}
	else
	{
		SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
	}

	rect.w *= SCREEN_SIZE;
	rect.h *= SCREEN_SIZE;

	if (SDL_RenderCopy(renderer, texture, section, &rect) != 0)
	{
		LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool ModuleRender::DrawRect(const SDL_Rect & rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera)
{
	bool ret = true;

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	SDL_Rect rec(rect);
	if (use_camera)
	{
		rec.x = (int)(camera.x + rect.x * SCREEN_SIZE);
		rec.y = (int)(camera.y + rect.y * SCREEN_SIZE);
		rec.w *= SCREEN_SIZE;
		rec.h *= SCREEN_SIZE;
	}

	if (SDL_RenderFillRect(renderer, &rec) != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool ModuleRender::DrawQuad(int x1, int y1, int w1, int x2, int y2, int w2, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	bool ret = true;

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	for (int i = 0; i < (w1 - w2) / 2; i++)
	{
		if (SDL_RenderDrawLine(renderer, x1 - (w1 / 2) + i, y1 - i, x1 + (w1 / 2) - i, y1 - i) != 0)
		{
			LOG("Cannot draw quad to screen. SDL_RenderDrawLine error: %s", SDL_GetError());
			ret = false;
		}
	}

	return ret;
}

bool ModuleRender::DrawHorizontalLine(float x, int y, float w, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	bool ret = true;

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	if (SDL_RenderDrawLine(renderer, static_cast<int>(x - (w / 2)), y, static_cast<int>(x + (w / 2)), y) != 0)
	{
		LOG("Cannot draw line to screen. SDL_RenderDrawLine error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}
