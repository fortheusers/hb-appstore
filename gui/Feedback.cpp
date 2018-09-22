#include "Feedback.hpp"
#include "TextElement.hpp"
#include "ImageElement.hpp"
#include "ImageCache.hpp"
#include "Button.hpp"

Feedback::Feedback(Package* package)
{
    TextElement* elem = new TextElement((std::string("Leaving feedback for: \"") + package->title + "\"").c_str(), 25);
    elem->position(20, 20);
    elements.push_back(elem);
    
    ImageElement* icon = new ImageElement((ImageCache::cache_path + package->pkg_name + "/icon.png").c_str());
    icon->position(20, 120);
    elements.push_back(icon);
    
    TextElement* feedback = new TextElement("hi", 17, NULL, false, 500);
    feedback->position(140, 140);
    elements.push_back(feedback);
    
    this->keyboard = new Keyboard(NULL, feedback->text);
    elements.push_back(keyboard);
    
    Button* send = new Button("Submit", 'x', 24);
    Button* quit = new Button("Discard", 'y', 24);
    send->position(20, 330);
    quit->position(20, 400);
    elements.push_back(send);
    elements.push_back(quit);
}
