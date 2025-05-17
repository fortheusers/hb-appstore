#include "../libs/chesto/src/ImageElement.hpp"
#include "../libs/chesto/src/ListElement.hpp"
#include "../libs/chesto/src/TextElement.hpp"

class AppList;

#if defined(WII) || defined(WII_MOCK)
#define USE_OSC_BRANDING 1
#endif

#if defined(USE_OSC_BRANDING)
#define TOTAL_CATS 6
#else
#define TOTAL_CATS 8
#endif
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
#ifdef USE_OSC_BRANDING
	const char* cat_names[TOTAL_CATS] = { "sidebar.search", "sidebar.all", "sidebar.utilities", "sidebar.emulators", "sidebar.games", "sidebar.media" };
	const char* cat_value[TOTAL_CATS] = { "_search", "_all", "utilities", "emulators", "games", "media" };
#else
	const char* cat_names[TOTAL_CATS] = { "sidebar.search", "sidebar.all", "sidebar.games", "sidebar.emulators", "sidebar.tools", "sidebar.media", "sidebar.advanced", "sidebar.misc" };
	const char* cat_value[TOTAL_CATS] = { "_search", "_all", "game", "emu", "tool", "media", "advanced", "_misc" };
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

#if defined(USE_OSC_BRANDING)
	rgb getOSCCategoryColor(std::string category);
#endif