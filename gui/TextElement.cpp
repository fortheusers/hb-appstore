#include "TextElement.hpp"

TextElement::TextElement(const char* text, int size, SDL_Color* color)
{
	this->text = new std::string(text);
	this->size = size;
	
	if (color == NULL)
		this->color = {0xff, 0xff, 0xff};
	else
		this->color = *color;
	
	this->textSurface = this->renderText(*(this->text), size);
}

void TextElement::render(Element* parent)
{
	SDL_Rect textLocation;
	textLocation.x = this->x + parent->x;
	textLocation.y = this->y + parent->y;
	
	SDL_BlitSurface(this->textSurface, NULL, parent->window_surface, &textLocation);
}

SDL_Surface* TextElement::renderText(std::string& message, int size)
{
	TTF_Font *font = TTF_OpenFont("./res/productsans.ttf", size);
	
	SDL_Surface *surf = TTF_RenderText_Blended(font, message.c_str(), this->color);
	
	//	SDL_FreeSurface(surf);
	TTF_CloseFont(font);
	
	return surf;
}
