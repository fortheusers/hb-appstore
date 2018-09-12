#include "TextElement.hpp"
#include "ImageElement.hpp"

class Button : public Element
{
public:
    Button(const char* text, const char* invoke);
//    bool process(InputEvents* event);
    void render(Element* parent);
    void position(int x, int y);
    
    // original x and y coordinates of this button before add in the parent
    int ox = 0, oy = 0;

    
    // TOOD: callback -> part of Element?
};

