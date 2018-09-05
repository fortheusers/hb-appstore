#include "TextElement.hpp"
#include "ImageElement.hpp"
#include "../libs/get/src/Package.hpp"

class AppCard: public Element
{
public:
	AppCard(Package* package);
	void update();
	bool process(InputEvents* event);
	void render(Element* parent);

	Package* package;

	// the number of which package this is in the list
	int index;
};
