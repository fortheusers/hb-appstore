#include "Button.hpp"

Button::Button(const char* message, const char* button)
{
    int PADDING = 10;
    SDL_Color color = {0x00, 0x00, 0x00, 0xff};
    
    TextElement* text = new TextElement(message, 20, &color);
    text->position(PADDING, PADDING);
    this->elements.push_back(text);
    
    this->width = text->width + PADDING*2;
    this->height = text->height + PADDING*2;
    
    this->touchable = true;
    
    // TODO: add icon and make room for it in the x, y dimens
}

void Button::position(int x, int y)
{
    ox = x;
    oy = y;
    super::position(x, y);
}

void Button::render(Element* parent)
{
    if (this->parent == NULL)
        this->parent = parent;
    
    this->renderer = parent->renderer;
    
    // update our x and y according to our parent
    this->x = ox + parent->x;
    this->y = oy + parent->y;
    
    // draw bg for button
    SDL_Rect dimens = { x, y, width, height };
    
    SDL_SetRenderDrawColor(parent->renderer, 0xee, 0xee, 0xee, 0xFF);
    SDL_RenderFillRect(parent->renderer, &dimens);
    
    super::render(this);
}
