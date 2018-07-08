#ifndef APPPOPUP_H_
#define APPPOPUP_H_

#include "TextElement.hpp"
#include "ImageElement.hpp"
#include "ProgressBar.hpp"
#include "../libs/get/src/Package.hpp"

class AppPopup : public Element
{
public:
	AppPopup(Package* package);
	bool process(InputEvents* event);
	void render(Element* parent);

	bool operating = false;
	Package* package;
	ProgressBar* pbar = NULL;
	int highlighted = -1;

	// the callback method to update the currently displayed pop up (and variables it needs)
	static void updateCurrentlyDisplayedPopup(float amount);
	static AppPopup* frontmostPopup;
};

#endif
