#ifndef FEEDBACKSCREEN_H_
#define FEEDBACKSCREEN_H_

#include "../libs/get/src/Get.hpp"

#include "../libs/chesto/src/Button.hpp"
#include "../libs/chesto/src/EKeyboard.hpp"
#include "../libs/chesto/src/ImageElement.hpp"
#include "../libs/chesto/src/NetImageElement.hpp"
#include "../libs/chesto/src/TextElement.hpp"
#include "../libs/chesto/src/Screen.hpp"

using namespace Chesto;

class Feedback : public Screen
{
public:
	Feedback(Package& package);

	void rebuildUI() override;
	void render(Element* parent) override;
	bool process(InputEvents* event) override;
	bool needsRefresh = false;

	Package* package = NULL;

	void submit();
	void back();

private:
	void keyboardInputCallback();

	TextElement* title;
	NetImageElement* icon;
	EKeyboard* keyboard;
	Button* quit;
	Button* send;
	Button* backspaceBtn;
	Button* capsBtn;
	TextElement* response;
	TextElement* feedback;
};

#endif
