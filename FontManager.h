#ifndef __FONTMANAGER_H__
#define __FONTMANAGER_H__

#include <string>
#include <list>
#include <vector>

class Font;

using namespace::std;

class FontManager {

public:
	FontManager();
	~FontManager();

	bool Init();
	const Font* Allocate(const string font, const string fileName, const string lineNumber);
	const bool Release(const string font);
	void End();

	//Dictionary of fonts
	list<string> fontNames;
	list<Font*> fontPointers;

	//Register of fonts allocated
	list<string> reservedFontName;
	list<string> fileLine;
};

#endif

