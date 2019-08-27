#include "AppList.hpp"
#include "chesto/src/RootDisplay.hpp"
#include "chesto/src/TextElement.hpp"
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
	MainDisplay(Get* get);

	TextElement* notice = NULL;

	Get* get = NULL;
	ImageCache* imageCache = NULL;
	bool error = false;
};
