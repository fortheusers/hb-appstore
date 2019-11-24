#ifndef FEEDBACKSCREEN_H_
#define FEEDBACKSCREEN_H_

#include "../libs/get/src/Get.hpp"
#include "Keyboard.hpp"

#include "../libs/chesto/src/ImageElement.hpp"
#include "../libs/chesto/src/TextElement.hpp"

class Feedback : public Element
{
public:
	Feedback(Package* package);
	void refresh();
	Keyboard* keyboard = NULL;
	Package* package = NULL;

	std::string message = "";
	bool touchMode = false;

	void submit();
	void back();
};

#endif
