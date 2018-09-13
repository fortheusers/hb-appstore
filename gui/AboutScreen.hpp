#ifndef ABOUTSCREEN_H_
#define ABOUTSCREEN_H_

#include "TextElement.hpp"
#include "ImageElement.hpp"
#include "ListElement.hpp"
#include "../libs/get/src/Get.hpp"

class AboutScreen : public Element
{
public:
    AboutScreen(Get* get);
    Get* get = NULL;
    void back();
    void render(Element* parent);
};

#endif
