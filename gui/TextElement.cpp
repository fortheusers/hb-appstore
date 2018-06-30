#include "MainDisplay.hpp"

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
	if (this->hidden)
		return;

	SDL_Rect textLocation;
	textLocation.x = this->x + parent->x;
	textLocation.y = this->y + parent->y;
	textLocation.w = this->width;
	textLocation.h = this->height;

	// std::cout << this->text->c_str() << " [" << this->x << " " << parent->x << " " <<
	// this->y << " " << parent->y << " " <<
	// textLocation.w << " " << textLocation.h << std::endl;

	SDL_RenderCopy(MainDisplay::mainRenderer, this->textSurface, NULL, &textLocation);

}

SDL_Texture* TextElement::renderText(std::string& message, int size)
{
	std::string key = message + std::to_string(size);

	// try to find it in the cache first
	if (ImageCache::cache.count(key))
		return ImageCache::cache[key];

	// not found, make/render it

	TTF_Font *font = TTF_OpenFont("./res/opensans.ttf", size);

	// font couldn't load, don't render anything
	if (!font)
		return NULL;

	SDL_Surface* surf = TTF_RenderText_Blended(font, message.c_str(), this->color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(MainDisplay::mainRenderer, surf);

	this->width = surf->w;
	this->height = surf->h;
	SDL_FreeSurface(surf);

	//	SDL_FreeSurface(surf);
	TTF_CloseFont(font);

	// save it to the cache for later
	ImageCache::cache[key] = texture;

	return texture;
}
