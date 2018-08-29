#include "Element.hpp"
#include "TextElement.hpp"
#include "ImageElement.hpp"
#include "AppList.hpp"
#include "ImageCache.hpp"
#include <unordered_map>

class MainDisplay : public Element
{
public:
	MainDisplay(Get* get);
	bool process(InputEvents* event);
	void render(Element* parent);
	void background(int r, int g, int b);
	void update();

	static SDL_Renderer* mainRenderer;

	Get* get = NULL;
	ImageCache* imageCache = NULL;
    bool error = false;

private:
	bool showingSplash = true;
	int count = 0;
};
