#include "AppList.hpp"
#include "../libs/chesto/src/RootDisplay.hpp"
#include "../libs/chesto/src/TextElement.hpp"
#include "../libs/chesto/src/Button.hpp"
#include <unordered_map>

#if defined(MUSIC)
#include <SDL2/SDL_mixer.h>
#endif

#if defined(SWITCH)
#include <switch.h>
#define PLATFORM "Switch"
#elif defined(__WIIU__)
#define PLATFORM "Wii U"
#elif defined(_3DS) || defined(_3DS_MOCK)
#define PLATFORM "3DS"
#elif defined(WII) || defined(WII_MOCK)
#define PLATFORM "Wii"
#else
#define PLATFORM "Generic"
#endif

#ifdef USE_OSC_BRANDING
#define LOGO_PATH RAMFS "res/open-hbas-logo.png"
#else
#define LOGO_PATH RAMFS "res/icon.png"
#endif

class MainDisplay : public RootDisplay
{
public:
	MainDisplay();
	~MainDisplay();

	bool process(InputEvents* event);
	void render(Element* parent);

	void drawErrorScreen(std::string troubleshootingText);
	bool getDefaultAudioStateForPlatform();
	void setupMusic();
	void beginInitialLoad();

	bool checkMetaRepoForUpdates(Get* get);
	void updateSidebarColor();
	bool isLowMemoryMode();

	Get* get = NULL;

	bool error = false;
	bool atLeastOneEnabled = false;

	static int updateLoader(void* clientp, double dltotal, double dlnow, double ultotal, double ulnow);

	bool showingSplash = true;
	bool renderedSplash = false;
	ImageElement *spinner = nullptr;

	void playSFX();

#if defined(MUSIC)
	Mix_Chunk* click_sfx;
#endif

private:
	Sidebar sidebar;
	AppList appList;
};

class ErrorScreen : public Element
{
public:
	ErrorScreen(std::string errorMessage, std::string troubleshootingText);

private:
	ImageElement icon;
	TextElement title;
	TextElement errorMessage;
	TextElement troubleshooting;
	Button btnQuit;
};

bool isEarthDay();