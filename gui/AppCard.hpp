#include "libget/src/Package.hpp"

#include "ImageElement.hpp"
#include "TextElement.hpp"
#include "MarioMaker.hpp"

class AppCard : public Element
{
	MarioMaker* mario = NULL;
public:
	AppCard(Package* package);
	AppCard(Package* package, MarioMaker *mario);
	void update();
	bool process(InputEvents* event);
	void render(Element* parent);
	void displaySubscreen();

	Package* package;

	// the number of which package this is in the list
	int index;
};
