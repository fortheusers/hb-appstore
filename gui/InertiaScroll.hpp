#include "Element.hpp"

class InertiaScroll : public Element
{
public:
	static void handle(Element* element, InputEvents* event);
};
