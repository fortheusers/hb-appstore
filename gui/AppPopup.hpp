#include "TextElement.hpp"
#include "ImageElement.hpp"
#include "../libs/get/src/Package.hpp"

class AppPopup : public Element
{
public:
	AppPopup(Package* package);
	bool process(SDL_Event* event);
	void render(Element* parent);
};
