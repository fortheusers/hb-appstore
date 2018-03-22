#include "Element.hpp"
#include <SDL/SDL_image.h>

class ImageElement : public Element
{
public:
	ImageElement(const char* path);
	void render(Element* parent);
	SDL_Surface* imgSurface;
};

