#include "Element.hpp"
#include <SDL2/SDL2_gfxPrimitives.h>

bool Element::process(InputEvents* event)
{
    // whether or not we need to update the screen
    bool ret = false;
    
    // do any touch down or up events
    ret |= onTouchDown(event);
    ret |= onTouchUp(event);
    
	// call process on subelements
	for (int x=0; x<this->elements.size(); x++)
		ret |= this->elements[x]->process(event);
    
    ret |= this->needsRedraw;
    this->needsRedraw = false;

	return ret;
}

void Element::render(Element* parent)
{
	for (int x=0; x<this->elements.size(); x++)
	{
		// go through every subelement and run render
		this->elements[x]->render(parent);
	}
    
    // if we're touchable, and we have some animation counter left, draw a rectangle+overlay
    if (this->touchable && this->elasticCounter > HIGHLIGHT)
    {
        SDL_Rect d = { this->xOff + this->x, this->yOff + this->y, this->width, this->height };
        SDL_SetRenderDrawBlendMode(parent->renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(parent->renderer, 0xad, 0xd8, 0xe6, 0x90);
        SDL_RenderFillRect(parent->renderer, &d);
    }
    
    if (this->touchable && this->elasticCounter > NO_HIGHLIGHT)
    {
        SDL_Rect d = { this->xOff + this->x, this->yOff + this->y, this->width, this->height };
        rectangleRGBA(parent->renderer, d.x, d.y, d.x + d.w, d.y + d.h, 0x66, 0x7c, 0x89, 0xFF);
    }
}

void Element::position(int x, int y)
{
	this->x = x;
	this->y = y;
}

bool Element::onTouchDown(InputEvents* event)
{
    if (!event->isTouchDown())
        return false;
    
    if (!event->touchIn(this->xOff + this->x, this->yOff + this->y, this->width, this->height))
        return false;
    
    // turn on deep highlighting during a touch down
    if (this->touchable)
        this->elasticCounter = DEEP_HIGHLIGHT;
    
    return true;
}

bool Element::onTouchUp(InputEvents* event)
{
    if (!event->isTouchUp())
        return false;
    
    if (!event->touchIn(this->xOff + this->x, this->yOff + this->y, this->width, this->height))
        return false;
    
    // animation counter must be nonzero to allow click to go through
    if (this->elasticCounter <= 0)
        return false;
    
    this->elasticCounter = 0;
    
    return true;
}

void Element::hide()
{
	this->hidden = true;
}

void Element::wipeElements()
{
	for (int x=0; x<this->elements.size(); x++)
	{
		Element* elem = this->elements[x];
		delete elem;
	}

	this->elements.clear();
}
