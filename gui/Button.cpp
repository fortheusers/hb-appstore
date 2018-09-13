#include "Button.hpp"

Button::Button(const char* message, char button, bool dark, int size, int width)
{
    int PADDING = 10;
    SDL_Color color;
    
    if (dark)
        color = {0xff, 0xff, 0xff, 0xff};
    else
        color = {0x00, 0x00, 0x00, 0xff};
    
    this->dark = dark;
    
    TextElement* text = new TextElement(message, size, &color);
    text->position(PADDING, PADDING);
    this->elements.push_back(text);
    
    this->width = (width > 0)? width : text->width + PADDING*2;
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
    
    if (dark)
        SDL_SetRenderDrawColor(parent->renderer, 0x67, 0x6a, 0x6d, 0xFF);
    else
        SDL_SetRenderDrawColor(parent->renderer, 0xee, 0xee, 0xee, 0xFF);
    
    SDL_RenderFillRect(parent->renderer, &dimens);
    
    super::render(this);
}
