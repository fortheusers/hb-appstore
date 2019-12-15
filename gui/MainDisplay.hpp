#include "AppList.hpp"
#include "../libs/chesto/src/RootDisplay.hpp"
#include "../libs/chesto/src/TextElement.hpp"
#include "../libs/chesto/src/Button.hpp"
#include "ImageCache.hpp"
#include <unordered_map>

#if defined(MUSIC)
#include <SDL2/SDL_mixer.h>
#endif

#if defined(__WIIU__)
#define ICON_SIZE 90
#else
#define ICON_SIZE 150
#endif

class MainDisplay : public RootDisplay
{
public:
	MainDisplay();
	~MainDisplay();

	bool process(InputEvents* event);
	void render(Element* parent);

	void drawErrorScreen(std::string troubleshootingText);

	Get* get = NULL;
	ImageCache* imageCache = NULL;

	bool error = false;
	bool atLeastOneEnabled = false;

	static int updateLoader(void* clientp, double dltotal, double dlnow, double ultotal, double ulnow);

	bool showingSplash = true;
	bool renderedSplash = false;

private:
	Sidebar sidebar;
	AppList appList;
};

class ErrorScreen : public Element
{
public:
	ErrorScreen(std::string troubleshootingText);

private:
	ImageElement icon;
	TextElement title;
	TextElement errorMessage;
	TextElement troubleshooting;
	Button btnQuit;
};
