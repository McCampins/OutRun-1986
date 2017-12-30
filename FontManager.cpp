#include "FontManager.h"
#include "Font.h"

#include <iostream>
#include <vector>
#include <string>
#include <direct.h>
#include <experimental/filesystem>

using namespace::std;

FontManager::FontManager() {}

FontManager::~FontManager() {}


vector<string> getFilenames(experimental::filesystem::path path) {
	namespace stdfs = std::experimental::filesystem;

	vector<string> filenames;

	const stdfs::directory_iterator end{};

	for (stdfs::directory_iterator iter{ path }; iter != end; ++iter) {
		if (stdfs::is_regular_file(*iter))
			filenames.push_back(iter->path().string());
	}

	return filenames;
}

string replaceBackslashes(string s)
{
	size_t found = s.find("\\");
	while (found != string::npos)
	{
		s.replace(found, 1, "/");
		found = s.find("\\");
	}
	return s;
}

bool FontManager::Init() {
	bool ret = true;

	string path = "config/fonts/";
	vector<string> filenames = getFilenames(path);
	Font* font;
	for (vector<string>::iterator it = filenames.begin(); it != filenames.end() && ret; ++it)
	{
		font = new Font();
		string fontName = replaceBackslashes(*it);
		ret = font->Init(fontName, "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890.,@");
		if (ret)
		{
			fontNames.push_back(fontName);
			fontPointers.push_back(font);
		}
	}

	return ret;
}

const Font * FontManager::Allocate(const string font, const string file, const string line)
{
	bool fontFound = false;
	int fontIndex = 0;

	string path = "config/fonts/";
	string f = path + font;

	for (list<string>::const_iterator it = fontNames.begin(); it != fontNames.cend() && fontFound == false; ++it)
	{
		if ((*it).compare(f) == 0)
		{
			fontFound = true;
		}
		else
		{
			fontIndex++;
		}
	}

	list<Font*>::const_iterator it = fontPointers.begin();
	for (int i = 0; i < fontIndex; i++)
	{
		it = next(it, 1);
	}

	reservedFontName.push_back(font);
	fileLine.push_back(file + " , " + line);

	return *it;
}

const bool FontManager::Release(const string font)
{
	bool ret = true;
	bool found = false;
	unsigned int index = 0;
	list<string>::iterator it = reservedFontName.begin();
	while (it != reservedFontName.end() && found == false)
	{
		if ((*it).compare(font) == 0)
		{
			found = true;
			it = reservedFontName.erase(it);
		}
		else
		{
			index++;
			++it;
		}
	}

	if (found)
	{
		list<string>::iterator it = fileLine.begin();
		for (unsigned int i = 0; i < index; i++)
		{
			it = next(it, 1);
		}
		fileLine.erase(it);
	}

	return found;
}

void FontManager::End()
{
	if (reservedFontName.empty() == false)
	{
		list<string>::iterator it1 = reservedFontName.begin();
		list<string>::iterator it2 = fileLine.begin();
		for (unsigned int i = 0; i < reservedFontName.size(); i++)
		{
			cout << "The font " << *it1 << " is still in use from " << *it2 << endl;
			it1 = next(it1, 1);
			it2 = next(it2, 1);
		}
	}
	else
	{
		fontNames.clear();

		for (list<Font*>::iterator it = fontPointers.begin(); it != fontPointers.end(); ++it)
		{
			(*it)->End();
		}
		fontPointers.clear();
	}
}

