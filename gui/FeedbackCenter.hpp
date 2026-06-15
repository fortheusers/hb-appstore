#ifndef FEEDBACKCENTERSCREEN_H_
#define FEEDBACKCENTERSCREEN_H_

#include "../libs/chesto/src/Screen.hpp"

using namespace Chesto;

class FeedbackCenter : public Screen
{
public:
	FeedbackCenter();
	void rebuildUI() override;

	void render(Element* parent) override;
};

class FeedbackMessage : public Element
{
public:
    FeedbackMessage();
    std::string content;
    std::string reply;
    std::string package;
    void build();
};

#endif
