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

//    ImageElement* icon = new ImageElement((std::string("res/")+inverse+"button-"+button+outline+".png").c_str());
//    icon->position(PADDING, PADDING);
//    this->elements.push_back(icon);
    
    const char* unicode;
    
    switch (button)
    {
        case 'a':
            unicode = "\ue0a0";
            break;
        case 'b':
            unicode = "\ue0a1";
            break;
        case 'y':
            unicode = "\ue0a2";
            break;
        case 'x':
            unicode = "\ue0a3";
            break;
        default:
            unicode = "";
    }
    
    TextElement* icon = new TextElement(unicode, size*1.25, &color, ICON);
    this->elements.push_back(icon);
    icon->position(PADDING, PADDING);
    
    TextElement* text = new TextElement(message, size, &color);
//    icon->resize(text->height, text->height);
    
    int bWidth = PADDING*0.5*(icon->width!=0);  // gap space between button

    text->position(PADDING + bWidth + icon->width, PADDING);
    this->elements.push_back(text);
    
    this->width = (width > 0)? width : text->width + PADDING*2 + bWidth + icon->width;
    this->height = text->height + PADDING*2;
    
    icon->position(PADDING, PADDING + (text->height - icon->height)/2);

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
