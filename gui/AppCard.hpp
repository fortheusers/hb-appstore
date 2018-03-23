#include "TextElement.hpp"
#include "ImageElement.hpp"
#include "../libs/get/src/Package.hpp"

class AppCard: public Element
{
public:
	AppCard(Package* package);
	void update();
	
	Package* package;
};
