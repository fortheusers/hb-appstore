#include "libget/src/Package.hpp"

#include "ImageElement.hpp"
#include "TextElement.hpp"

class AppCard : public Element
{
public:
	AppCard(Package* package);
	void update();
	bool process(InputEvents* event);
	void render(Element* parent);
	void displaySubscreen();

	Package* package;

	// the number of which package this is in the list
	int index;
};
