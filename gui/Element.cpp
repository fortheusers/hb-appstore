#include "Element.hpp"
#include <SDL2/SDL2_gfxPrimitives.h>

#define TOUCHANIMATION_TIMEOUT 200

bool Element::process(InputEvents* event)
{
    // do any touch down or up events
    onTouchDown(event);
    onTouchUp(event);
    
	// call process on subelements
	for (int x=0; x<this->elements.size(); x++)
		this->elements[x]->process(event);

	return false;
}

void Element::render(Element* parent)
{
	for (int x=0; x<this->elements.size(); x++)
	{
		// go through every subelement and run render
		this->elements[x]->render(parent);
	}
    
    // if we're touchable, and we have some animation counter left, draw a rectangle+overlay
    if (this->touchable && this->elasticCounter > 0)
    {
        SDL_Rect d = { this->xOff + this->x, this->yOff + this->y, this->width, this->height };
        SDL_SetRenderDrawBlendMode(parent->renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(parent->renderer, 0xad, 0xd8, 0xe6, 0x90);
        SDL_RenderFillRect(parent->renderer, &d);
        
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
    
    if (this->touchable)
    {
        // set animation counter to 20, will count down in the render phase (if touchable is true)
        this->elasticCounter = TOUCHANIMATION_TIMEOUT;
    }
    
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
