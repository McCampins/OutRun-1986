#ifndef __FONT_H__
#define __FONT_H__

#define asciiBegin 33
#define asciiEnd 126
#define fontWidth 9
#define fontHeight 15

#include "SDL/include/SDL.h"
#include <map>
#include <string>

using namespace std;

class Font
{
public:
	Font();
	~Font();

	bool Init(const string str, const string trad);
	void End();

public:
	SDL_Surface* fontSurface = nullptr;
	string traductionString;
	std::map<char, SDL_Rect> traductionTable;
};

#endif
