#include <SDL2/SDL2_gfxPrimitives.h>
#include "DetailsPopup.hpp"
#include "InertiaScroll.hpp"

DetailsPopup::DetailsPopup(std::string* details_text)
{
	// popup card (element 1)
	ImageElement* popup = new ImageElement("res/popup.png");
	popup->position(469, 109);
	popup->resize(727, 422);
	this->elements.push_back(popup);

  SDL_Color black = {0x00, 0x00, 0x00, 0xff};

	// download/update/remove button (2)
	TextElement* details = new TextElement(details_text->c_str(), 20, &black, false, 700);
	details->position(485, 115);
	this->elements.push_back(details);
}

bool DetailsPopup::process(InputEvents* event)
{
	InertiaScroll::handle(this->elements[1], event);
	return false;
}

void DetailsPopup::render(Element* parent)
{
	if (this->renderer == NULL)
		this->renderer = parent->renderer;

	if (this->parent == NULL)
		this->parent = parent;

  // background (element 0)
  SDL_Rect fullscreen = {0, 0, 1280, 720};
  SDL_SetRenderDrawColor(parent->renderer, 0x00, 0x00, 0x00, 0xff);
  SDL_RenderFillRect(parent->renderer, &fullscreen);

	for (int x=0; x<this->elements.size(); x++)
	{
		// go through every subelement and run render
		// (use "this" instead of "parent" to be absolute)
		this->elements[x]->render(this);
	}
}
