#ifndef FEEDBACKSCREEN_H_
#define FEEDBACKSCREEN_H_

#include "TextElement.hpp"
#include "ImageElement.hpp"
#include "Keyboard.hpp"
#include "../libs/get/src/Get.hpp"

class Feedback : public Element
{
public:
    Feedback(Package* package);
    Keyboard* keyboard = NULL;
    Package* package = NULL;
    
    std::string message = "";
    
    void submit();
    void back();
};

#endif
