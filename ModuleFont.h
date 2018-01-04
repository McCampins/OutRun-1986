#ifndef __MODULEFONT_H__
#define __MODULEFONT_H__

#define asciiBegin 33
#define asciiEnd 126
#define fontWidth 76.6
#define fontHeight 70

#include "SDL/include/SDL.h"
#include <map>
#include <string>

using namespace std;

class ModuleFont
{
public:
	ModuleFont();
	~ModuleFont();

	bool Init(const string str, const string trad);
	void End();

public:
	SDL_Texture* fontSurface = nullptr;
	string traductionString;
	std::map<char, SDL_Rect*> traductionTable;
};


#endif
