#include "ModuleFont.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "Globals.h"

#include <stdio.h>

ModuleFont::ModuleFont()
{
}

ModuleFont::~ModuleFont()
{
}

bool ModuleFont::Init(const string str, const string trad)
{
	bool ret = true;

	traductionString = trad;

	fontSurface = App->textures->Load(str.c_str());
	if (fontSurface == NULL)
	{
		LOG("Unable to load font %s surface! SDL Error: %s\n", str.c_str(), SDL_GetError());
		ret = false;
	}

	SDL_Rect* rect;
	for (unsigned int i = 0; i < traductionString.length(); i++)
	{
		char c = traductionString.at(i);

		rect = new SDL_Rect();
		rect->x = int(fontWidth * i);
		rect->y = 0;
		rect->w = int(fontWidth);
		rect->h = fontHeight;
		traductionTable[c] = rect;
	}

	delete rect;

	return ret;
}

void ModuleFont::End()
{
	App->textures->Unload(fontSurface);
	fontSurface = nullptr;

	traductionString.clear();

	for (map<char, SDL_Rect*>::iterator it = traductionTable.begin(); it != traductionTable.end(); ++it) {
		it->second = nullptr;
		delete it->second;
	}

	traductionTable.clear();
}

