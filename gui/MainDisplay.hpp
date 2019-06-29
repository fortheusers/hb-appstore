#include "AppList.hpp"
#include "Element.hpp"
#include "ImageCache.hpp"
#include "ImageElement.hpp"
#include "TextElement.hpp"
#include <unordered_map>

#if defined(MUSIC)
#include <SDL2/SDL_mixer.h>
#endif

#if defined(__WIIU__)
#define ICON_SIZE 90
#else
#define ICON_SIZE 150
#endif

class MainDisplay : public Element
{
public:
	MainDisplay(Get* get);
	bool process(InputEvents* event);
	void render(Element* parent);
	void background(int r, int g, int b);
	void update();
	void exit();

	TextElement* notice = NULL;

	static SDL_Renderer* mainRenderer;
	static Element* subscreen;
	static MainDisplay* mainDisplay;

	Get* get = NULL;
	ImageCache* imageCache = NULL;
	bool error = false;

	int lastFrameTime = 99;
	SDL_Event needsRender;

#if defined(MUSIC)
	Mix_Music* music;
#endif

	bool showingSplash = true;
	int count = 0;
};

void quit();
