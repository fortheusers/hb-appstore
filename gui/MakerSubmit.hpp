#pragma once
#include "ImageElement.hpp"
#include "ListElement.hpp"
#include "TextElement.hpp"
#include "libget/src/Package.hpp"
#include "MarioMaker.hpp"
#include "AppList.hpp"

class MakerSubmit : public Element
{
    AppList *applist;
    MarioMaker *mario;
    TextElement *descContent;

    SDL_Color red = { 0xFF, 0x00, 0x00, 0xff };
	SDL_Color gray = { 0x50, 0x50, 0x50, 0xff };
	SDL_Color black = { 0x00, 0x00, 0x00, 0xff };
	SDL_Color white = { 0xFF, 0xFF, 0xFF, 0xff };

    SDL_Texture *grid;
    int MARGIN;
    int SBSTART;
    MarioMakerLevel *level;
    void cSubmit();
    public:
    Package *package;
    MakerSubmit(Package *package, AppList *appList, MarioMaker *mario);
    bool process(InputEvents* event);
	void render(Element* parent);
    void update();
};

class TextElementTitle : public TextElement
{
    using TextElement::TextElement;

    void render(Element* parent)
    {
        
        if (this->renderer == NULL)
		    this->renderer = parent->renderer;
	    if (this->parent == NULL)
		    this->parent = parent;
        
        SDL_Rect dimens = { 0, 0, 1280-336, 110 };

	    SDL_SetRenderDrawColor(parent->renderer, 0xF8, 0xB9, 0x00, 0xff);
	    SDL_RenderFillRect(parent->renderer, &dimens);
	    SDL_SetRenderDrawColor(parent->renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        TextElement::render(this);
    }
};

class ScrollContent : public ListElement
{
	bool process(InputEvents* event);
	void render(Element* parent);

public:
	int MARGIN = 60;
};