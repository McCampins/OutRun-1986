#ifndef __FONTMANAGER_H__
#define __FONTMANAGER_H__

#include <string>
#include <list>
#include <vector>

class ModuleFont;

using namespace::std;

class ModuleFontManager {

public:
	ModuleFontManager();
	~ModuleFontManager();

	bool Init();
	const ModuleFont* Allocate(const string font, const string fileName, const string lineNumber);
	const bool Release(const string font);
	void End();

	//Dictionary of fonts
	list<string> fontNames;
	list<ModuleFont*> fontPointers;

	//Register of fonts allocated
	list<string> reservedFontName;
	list<string> fileLine;
};

#endif

