#pragma once

#include "../libs/get/src/Get.hpp"

#include "../libs/chesto/src/Button.hpp"
#include "../libs/chesto/src/EKeyboard.hpp"
#include "../libs/chesto/src/ImageElement.hpp"
#include "../libs/chesto/src/ListElement.hpp"
#include "../libs/chesto/src/TextElement.hpp"

#include "AppCard.hpp"
#include "AppDetails.hpp"
#include "../libs/chesto/src/Grid.hpp"
#include "Sidebar.hpp"

using namespace Chesto;

#include <list>
#include <random>

#define TOTAL_SORTS 5 // alphabetical (with updates at top), downloads, last updated, size, shuffled
#define RECENT 0
#define POPULARITY 1
#define ALPHABETICAL 2
#define SIZE 3
#define RANDOM 4

#define PANE_WIDTH SCREEN_HEIGHT

class AppList : public ListElement
{
public:
	AppList(Get* get, Sidebar* sidebar);

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
	bool needsUpdate = false;

	// the total number of apps displayed in this list
	int totalCount = 0;

	// default number of items per row TODO: save this value as config
	int R = 3;

	int sortMode = RECENT;
	bool useBannerIcons = true;

	void launchSettings();
	bool hideSidebar = true;
	EKeyboard* keyboard;

	void rebuildUI(); // common method for managing non-appcard elements

private:
	bool sortCompare(const Package& left, const Package& right);
	std::random_device randDevice;

	void keyboardInputCallback();

	enum UIElementTag {
		TAG_APP_CARD = 1,      // AppCards get this tag
		TAG_QUIT_BTN = 1000,
		TAG_CREDITS_BTN,
		TAG_SORT_BTN,
		TAG_KEYBOARD_BTN,
		TAG_BACKSPACE_BTN,
		TAG_CATEGORY_TEXT,
		TAG_SORT_BLURB,
		TAG_NOW_PLAYING_TEXT,
		TAG_MUTE_BTN,
		TAG_MUTE_ICON,
		TAG_UNMUTE_ICON,
		TAG_NOW_PLAYING_ICON
	};

	template<typename T>
	T* findElementByTag(int tag) {
		for (auto& elem : elements) {
			if (elem->tag == tag) {
				return dynamic_cast<T*>(elem.get());
			}
		}
		return nullptr;
	}

#if defined(MUSIC)
	std::vector<std::string> musicInfo;
#endif

	// list of all the app cards
	std::vector<AppCard*> appCards;

	// grid container for organizing app card positions
	Chesto::Grid* appGrid = nullptr;

	// list of pointers to the cards that are currently being shown
	std::vector<AppCard*> visibleAppCards;

	// int selected = -1;
	// int targetY = 0;
	// int draggingY = 0;
	// bool isDragging = false;
	// int momentumY = 0;
	// bool isDark = false;

	// used for sorting
	static std::string sortingDescriptions[TOTAL_SORTS];
	static CST_Color red;
	static CST_Color lighterRed;
};
