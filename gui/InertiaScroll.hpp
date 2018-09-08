#include "Element.hpp"

class InertiaScroll : public Element
{
public:
	static bool handle(Element* element, InputEvents* event);
};
