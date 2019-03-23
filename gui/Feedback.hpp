#ifndef FEEDBACKSCREEN_H_
#define FEEDBACKSCREEN_H_

#include "libget/src/Get.hpp"
#include "ImageElement.hpp"
#include "Keyboard.hpp"
#include "TextElement.hpp"

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
