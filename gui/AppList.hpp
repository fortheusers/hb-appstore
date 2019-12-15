#pragma once

#include "../libs/get/src/Get.hpp"

#include "../libs/chesto/src/Button.hpp"
#include "../libs/chesto/src/ImageElement.hpp"
#include "../libs/chesto/src/ListElement.hpp"
#include "../libs/chesto/src/TextElement.hpp"

#include "AppCard.hpp"
#include "AppDetails.hpp"
#include "Keyboard.hpp"
#include "Sidebar.hpp"

#include <random>
#include <list>

#define TOTAL_SORTS 5 // alphabetical (with updates at top), downloads, last updated, size, shuffled
#define RECENT 0
#define POPULARITY 1
#define ALPHABETICAL 2
#define SIZE 3
#define RANDOM 4

class AppList : public ListElement
{
public:
	AppList(Get* get, Sidebar* sidebar);
	~AppList();

	bool process(InputEvents* event);
	void render(Element* parent);
	void update();

	Get* get = NULL;
	Sidebar* sidebar = NULL;

	void toggleKeyboard();
	void cycleSort();
	void reorient();
	void toggleAudio();

	bool touchMode = true;

	// the total number of apps displayed in this list
	int totalCount = 0;

	// default number of items per row TODO: save this value as config
	int R = 3;

	int sortMode = RECENT;

	void launchSettings();

	Keyboard keyboard;
	ImageElement* spinner = nullptr;

private:
	bool sortCompare(const Package* left, const Package* right);
	std::random_device randDevice;

	// the title of this category (from the sidebar)
	static SDL_Color black, gray;
	static const char* sortingDescriptions[TOTAL_SORTS];

	TextElement* sortBlurb = nullptr;
	TextElement* category = nullptr;
	Button quitBtn;
	Button creditsBtn;
	Button sortBtn;
	Button keyboardBtn;

#if defined(MUSIC)
	Button muteBtn;
	ImageElement muteIcon;
#endif

	// list of visible app cards
	std::list<AppCard> appCards;
};
