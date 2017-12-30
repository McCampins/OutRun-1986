#include "Font.h"
#include "Application.h"
#include "ModuleTextures.h"
#include "Globals.h"

#include <stdio.h>

Font::Font()
{
}

Font::~Font()
{
}

bool Font::Init(const string str, const string trad)
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
		rect->x = fontWidth * i;
		rect->y = 0;
		rect->w = fontWidth;
		rect->h = fontHeight;
		traductionTable[c] = rect;
	}

	delete rect;

	return ret;
}

void Font::End()
{
	App->textures->Unload(fontSurface);
	fontSurface = nullptr;
}

