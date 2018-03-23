#include "Element.hpp"

class ProgressBar : public Element
{
public:
	void render(Element* parent);
	float percent = 0;
};
