#ifndef DETAILSPOPUP_H_
#define DETAILSPOPUP_H_

#include "TextElement.hpp"
#include "ImageElement.hpp"

class DetailsPopup : public Element
{
public:
	DetailsPopup(std::string* package);
	bool process(InputEvents* event);
	void render(Element* parent);
};

#endif
