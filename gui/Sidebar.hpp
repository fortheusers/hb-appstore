#include "chesto/src/ImageElement.hpp"
#include "chesto/src/ListElement.hpp"
#include "chesto/src/TextElement.hpp"

class AppList;

#define TOTAL_CATS 8
#pragma once

class Sidebar : public ListElement
{
public:
	Sidebar();

	std::string currentCatName();
	std::string currentCatValue();

	std::string searchQuery = "";

	AppList* appList = NULL;

	void render(Element* parent);
	bool process(InputEvents* event);

	// the currently selected category index
	int curCategory = 1; // 1 is all apps

	// list of human-readable category titles and short names from the json

#if defined(__WIIU__)
	const char* cat_names[TOTAL_CATS] = { "Search", "All Apps", "Games", "Emulators", "Tools", "Advanced", "Concepts", "Misc" };
	const char* cat_value[TOTAL_CATS] = { "_search", "_all", "game", "emu", "tool", "advanced", "concept", "_misc" };
#else
	const char* cat_names[TOTAL_CATS] = { "Search", "All Apps", "Games", "Emulators", "Tools", "Advanced", "Themes", "Misc" };
	const char* cat_value[TOTAL_CATS] = { "_search", "_all", "game", "emu", "tool", "advanced", "theme", "_misc" };
#endif
};
