#ifndef FEEDBACKCENTERSCREEN_H_
#define FEEDBACKCENTERSCREEN_H_

#include "../libs/chesto/src/Element.hpp"

class FeedbackCenter : public Element
{
public:
	FeedbackCenter();

	void render(Element* parent);
	// bool process(InputEvents* event);
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
