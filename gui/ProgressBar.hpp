#include "Element.hpp"

class ProgressBar : public Element
{
public:
	ProgressBar();
	void render(Element* parent);
	float percent = 0;
	
	int color;
	int width = 0;
};
