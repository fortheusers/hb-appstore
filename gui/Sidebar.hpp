#include "../libs/chesto/src/ImageElement.hpp"
#include "../libs/chesto/src/ListElement.hpp"
#include "../libs/chesto/src/TextElement.hpp"

class AppList;

#define TOTAL_CATS 8
#pragma once

class Sidebar : public ListElement
{
public:
	Sidebar();
	~Sidebar();

	std::string currentCatName();
	std::string currentCatValue();

  void addHints();
  int getWidth();

	std::string searchQuery = "";

	AppList* appList = NULL;

	void render(Element* parent);
	bool process(InputEvents* event);

	int currentSelection = -1;

  bool showCurrentCategory = false;

	// the currently selected category index
	int curCategory = 1; // 1 is all apps

	// list of human-readable category titles and short names from the json

#if defined(__WIIU__)
	const char* cat_names[TOTAL_CATS] = { "sidebar.search", "sidebar.all", "Aroma", "sidebar.games", "sidebar.emulators", "sidebar.tools", "sidebar.advanced", "sidebar.misc" };
	const char* cat_value[TOTAL_CATS] = { "_search", "_all", "aroma", "game", "emu", "tool", "advanced", "_misc" };
#else
	const char* cat_names[TOTAL_CATS] = { "sidebar.search", "sidebar.all", "sidebar.games", "sidebar.emulators", "sidebar.tools", "sidebar.advanced", "sidebar.themes", "sidebar.misc" };
	const char* cat_value[TOTAL_CATS] = { "_search", "_all", "game", "emu", "tool", "advanced", "theme", "_misc" };
#endif

private:
	struct
	{
		ImageElement* icon;
		TextElement* name;
	} category[TOTAL_CATS];

	ImageElement logo;
	TextElement title;
	TextElement subtitle;

	ImageElement* hider = nullptr;
	TextElement* hint = nullptr;
};
