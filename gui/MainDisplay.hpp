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
#elif defined(_3DS)
#define PLATFORM "3DS"
#elif defined(WII)
#define PLATFORM "Wii"
#else
#define PLATFORM "Generic"
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