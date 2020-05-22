#ifndef FEEDBACKSCREEN_H_
#define FEEDBACKSCREEN_H_

#include "../libs/get/src/Get.hpp"
#include "Keyboard.hpp"

#include "../libs/chesto/src/Button.hpp"
#include "../libs/chesto/src/ImageElement.hpp"
#include "../libs/chesto/src/TextElement.hpp"
#include "../libs/chesto/src/NetImageElement.hpp"

class Feedback : public Element
{
public:
	Feedback(Package* package);

	void render(Element* parent);
	bool process(InputEvents* event);
	bool needsRefresh = false;

	Package* package = NULL;

	void submit();
	void back();

private:
	void keyboardInputCallback();

	TextElement title;
	NetImageElement icon;
	Keyboard keyboard;
	Button quit;
	Button send;
	TextElement response;
	TextElement feedback;
#if defined(__WIIU__)
	TextElement hint;
#endif
};

#endif
