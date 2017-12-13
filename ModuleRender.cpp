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

	firstRumbleX = -(ROADWIDTH * 3) - (RUMBLEWIDTH / 2) - (LINEWIDTH * 3.5f);
	firstLineX = -(ROADWIDTH * 3) - (LINEWIDTH * 3);
	firstRoadX = -(ROADWIDTH * 2.5f) - (LINEWIDTH * 2.5f);
	firstSecondLineX = -(ROADWIDTH * 2) - (LINEWIDTH * 2);
	secondRoadX = -(ROADWIDTH * 1.5f) - (LINEWIDTH * 1.5f);
	secondThirdLineX = -(ROADWIDTH)-(LINEWIDTH);
	secondRumbleX = -(RUMBLEWIDTH / 2) - (LINEWIDTH / 2);
	thirdRoadX = -(ROADWIDTH / 2) - (LINEWIDTH / 2);
	thirdRumbleX = (RUMBLEWIDTH / 2) + (LINEWIDTH / 2);
	fourthRoadX = (ROADWIDTH / 2) + (LINEWIDTH / 2);
	fourthFifthLineX = (ROADWIDTH)+(LINEWIDTH);
	fifthRoadX = (ROADWIDTH * 1.5f) + (LINEWIDTH * 1.5f);
	fifthSixthLineX = (ROADWIDTH * 2) + (LINEWIDTH * 2);
	sixthRoadX = (ROADWIDTH * 2.5f) + (LINEWIDTH * 2.5f);
	sixthLineX = (ROADWIDTH * 3) + (LINEWIDTH * 3);
	fourthRumbleX = (ROADWIDTH * 3) + (RUMBLEWIDTH / 2) + (LINEWIDTH * 3.5f);

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

	if (SDL_RenderDrawLine(renderer, int(x - (w / 2)), y, int(x + (w / 2)), y) != 0)
	{
		LOG("Cannot draw line to screen. SDL_RenderDrawLine error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

int ModuleRender::DrawRoads(int screenY, float worldPosition, float scaleFactor, float x, float roadSeparation, int times) {
	float drawX = x + App->renderer->camera.x * scaleFactor;
	if ((int)worldPosition % 2 == 0)
	{
		for (int i = 0; i < times; i++) {
			screenY--;
			//Terrain
			App->renderer->DrawHorizontalLine(drawX, screenY, TERRAINWIDTH, 219, 209, 180, 255);
			//Rumble
			App->renderer->DrawHorizontalLine(drawX + (firstRumbleX * scaleFactor), screenY, RUMBLEWIDTH * scaleFactor, 161, 160, 161, 255);
			//1st Road Line
			App->renderer->DrawHorizontalLine(drawX + (firstLineX * scaleFactor), screenY, LINEWIDTH * scaleFactor, 255, 255, 255, 255);
			//1st Road
			App->renderer->DrawHorizontalLine(drawX + (firstRoadX * scaleFactor), screenY, ROADWIDTH * scaleFactor, 161, 160, 161, 255);
			//1st Road - 2nd Road Line
			App->renderer->DrawHorizontalLine(drawX + (firstSecondLineX * scaleFactor), screenY, LINEWIDTH * scaleFactor, 255, 255, 255, 255);
			//2nd Road
			App->renderer->DrawHorizontalLine(drawX + (secondRoadX * scaleFactor), screenY, ROADWIDTH * scaleFactor, 161, 160, 161, 255);
			//2nd Road - 3rd Road Line
			App->renderer->DrawHorizontalLine(drawX + (secondThirdLineX * scaleFactor), screenY, LINEWIDTH * scaleFactor, 255, 255, 255, 255);
			//Rigth Road Rumble (before 3rd Road in case both roads intersect)
			App->renderer->DrawHorizontalLine(drawX + ((secondRumbleX + roadSeparation) * scaleFactor), screenY, RUMBLEWIDTH * scaleFactor, 161, 160, 161, 255);
			//3rd Road
			App->renderer->DrawHorizontalLine(drawX + (thirdRoadX * scaleFactor), screenY, ROADWIDTH * scaleFactor, 161, 160, 161, 255);
			//3rd Road Line
			App->renderer->DrawHorizontalLine(drawX, screenY, LINEWIDTH * scaleFactor, 255, 255, 255, 255);
			//4th Road Line (same coordinates as they might intersect)
			App->renderer->DrawHorizontalLine(drawX + (roadSeparation * scaleFactor), screenY, LINEWIDTH * scaleFactor, 255, 255, 255, 255);
			//Left Road Rumble (before 4th Road in case both roads intersect)
			App->renderer->DrawHorizontalLine(drawX + (thirdRumbleX * scaleFactor), screenY, RUMBLEWIDTH * scaleFactor, 161, 160, 161, 255);
			//4th Road
			App->renderer->DrawHorizontalLine(drawX + ((fourthRoadX + roadSeparation) * scaleFactor), screenY, ROADWIDTH * scaleFactor, 161, 160, 161, 255);
			//4th Road - 5th Road Line
			App->renderer->DrawHorizontalLine(drawX + ((fourthFifthLineX + roadSeparation) * scaleFactor), screenY, LINEWIDTH * scaleFactor, 255, 255, 255, 255);
			//5th Road
			App->renderer->DrawHorizontalLine(drawX + ((fifthRoadX + roadSeparation) * scaleFactor), screenY, ROADWIDTH * scaleFactor, 161, 160, 161, 255);
			//5th Road - 6th Road Line
			App->renderer->DrawHorizontalLine(drawX + ((fifthSixthLineX + roadSeparation) * scaleFactor), screenY, LINEWIDTH * scaleFactor, 255, 255, 255, 255);
			//6th Road
			App->renderer->DrawHorizontalLine(drawX + ((sixthRoadX + roadSeparation)  * scaleFactor), screenY, ROADWIDTH * scaleFactor, 161, 160, 161, 255);
			//6th Road Line
			App->renderer->DrawHorizontalLine(drawX + ((sixthLineX + roadSeparation)  * scaleFactor), screenY, LINEWIDTH * scaleFactor, 255, 255, 255, 255);
			//Rumble
			App->renderer->DrawHorizontalLine(drawX + ((fourthRumbleX + roadSeparation) * scaleFactor), screenY, RUMBLEWIDTH * scaleFactor, 161, 160, 161, 255);
		}
	}
	else
	{
		for (int i = 0; i < times; i++) {
			screenY--;
			//Terrain
			App->renderer->DrawHorizontalLine(drawX, screenY, TERRAINWIDTH, 194, 178, 128, 255);
			//Rumble
			App->renderer->DrawHorizontalLine(drawX + (firstRumbleX * scaleFactor), screenY, RUMBLEWIDTH * scaleFactor, 255, 255, 255, 255);
			//1st Road Line
			App->renderer->DrawHorizontalLine(drawX + (firstLineX * scaleFactor), screenY, LINEWIDTH * scaleFactor, 170, 170, 170, 255);
			//1st Road
			App->renderer->DrawHorizontalLine(drawX + (firstRoadX * scaleFactor), screenY, ROADWIDTH * scaleFactor, 170, 170, 170, 255);
			//1st Road - 2nd Road Line
			App->renderer->DrawHorizontalLine(drawX + (firstSecondLineX * scaleFactor), screenY, LINEWIDTH * scaleFactor, 170, 170, 170, 255);
			//2nd Road
			App->renderer->DrawHorizontalLine(drawX + (secondRoadX * scaleFactor), screenY, ROADWIDTH * scaleFactor, 170, 170, 170, 255);
			//2nd Road - 3rd Road Line
			App->renderer->DrawHorizontalLine(drawX + (secondThirdLineX * scaleFactor), screenY, LINEWIDTH * scaleFactor, 170, 170, 170, 255);
			//Rigth Road Rumble (before 3rd Road in case both roads intersect)
			App->renderer->DrawHorizontalLine(drawX + ((secondRumbleX + roadSeparation) * scaleFactor), screenY, RUMBLEWIDTH * scaleFactor, 255, 255, 255, 255);
			//3rd Road
			App->renderer->DrawHorizontalLine(drawX + (thirdRoadX * scaleFactor), screenY, ROADWIDTH * scaleFactor, 170, 170, 170, 255);
			//3rd Road Line
			App->renderer->DrawHorizontalLine(drawX, screenY, LINEWIDTH * scaleFactor, 170, 170, 170, 255);
			//4th Road Line (same coordinates as they might intersect)
			App->renderer->DrawHorizontalLine(drawX + (roadSeparation * scaleFactor), screenY, LINEWIDTH * scaleFactor, 170, 170, 170, 255);
			//Left Road Rumble (before 4th Road in case they intersect)
			App->renderer->DrawHorizontalLine(drawX + (thirdRumbleX * scaleFactor), screenY, RUMBLEWIDTH * scaleFactor, 255, 255, 255, 255);
			//4th Road
			App->renderer->DrawHorizontalLine(drawX + ((fourthRoadX + roadSeparation) * scaleFactor), screenY, ROADWIDTH * scaleFactor, 170, 170, 170, 255);
			//4th Road - 5th Road Line
			App->renderer->DrawHorizontalLine(drawX + ((fourthFifthLineX + roadSeparation) * scaleFactor), screenY, LINEWIDTH * scaleFactor, 170, 170, 170, 255);
			//5th Road
			App->renderer->DrawHorizontalLine(drawX + ((fifthRoadX + roadSeparation) * scaleFactor), screenY, ROADWIDTH * scaleFactor, 170, 170, 170, 255);
			//5th Road - 6th Road Line
			App->renderer->DrawHorizontalLine(drawX + ((fifthSixthLineX + roadSeparation) * scaleFactor), screenY, LINEWIDTH * scaleFactor, 170, 170, 170, 255);
			//6th Road
			App->renderer->DrawHorizontalLine(drawX + ((sixthRoadX + roadSeparation)  * scaleFactor), screenY, ROADWIDTH * scaleFactor, 170, 170, 170, 255);
			//6th Road Line
			App->renderer->DrawHorizontalLine(drawX + ((sixthLineX + roadSeparation)  * scaleFactor), screenY, LINEWIDTH * scaleFactor, 170, 170, 170, 255);
			//Rumble
			App->renderer->DrawHorizontalLine(drawX + ((fourthRumbleX + roadSeparation) * scaleFactor), screenY, RUMBLEWIDTH * scaleFactor, 255, 255, 255, 255);
		}
	}
	return screenY;
}
