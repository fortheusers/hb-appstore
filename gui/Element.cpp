#include "Element.hpp"

bool Element::process(InputEvents* event)
{
	// call process on subelements
	for (int x=0; x<this->elements.size(); x++)
		this->elements[x]->process(event);

	return false;
}

void Element::render(Element* parent)
{
	for (int x=0; x<this->elements.size(); x++)
	{
		// go through every subelement and run render
		this->elements[x]->render(parent);
	}
}

void Element::position(int x, int y)
{
	this->x = x;
	this->y = y;
}

void Element::hide()
{
	this->hidden = true;
}

void Element::wipeElements()
{
	for (int x=0; x<this->elements.size(); x++)
	{
		Element* elem = this->elements[x];
		delete elem;
	}

	this->elements.clear();
}
