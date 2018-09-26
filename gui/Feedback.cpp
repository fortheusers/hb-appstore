#include "Feedback.hpp"
#include "TextElement.hpp"
#include "ImageElement.hpp"
#include "Button.hpp"
#include "MainDisplay.hpp"

#include <curl/curl.h>
#include <curl/easy.h>

Feedback::Feedback(Package* package)
{
    this->package = package;

    TextElement* elem = new TextElement((std::string("Leaving feedback for: \"") + package->title + "\"").c_str(), 25);
    elem->position(20, 20);
    elements.push_back(elem);

    ImageElement* icon = new ImageElement((ImageCache::cache_path + package->pkg_name + "/icon.png").c_str());
    icon->position(20, 120);
    elements.push_back(icon);

    TextElement* feedback = new TextElement(this->message.c_str(), 17, NULL, false, 500);
    feedback->position(140, 140);
    elements.push_back(feedback);

    this->keyboard = new Keyboard(NULL, &this->message);
    elements.push_back(keyboard);

    Button* send = new Button("Submit", X_BUTTON, 24);
    Button* quit = new Button("Discard", Y_BUTTON, 24);
    send->position(20, 330);
    quit->position(20, 400);
    elements.push_back(send);
    elements.push_back(quit);

    send->action = std::bind(&Feedback::submit, this);
    quit->action = std::bind(&Feedback::back, this);
}

void Feedback::submit()
{
    CURL *curl;
    CURLcode res;

#if defined(__WIIU__)
    const char* userKey = "wiiu_user";
#else
    const char* userKey = "switch_user";
#endif

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "http://switchbru.com/appstore/feedback");
        std::string fields = std::string("name=") + userKey + "&package=" + package->pkg_name + "&message=" + this->message;
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, fields.c_str());

        res = curl_easy_perform(curl);

        /* always cleanup */
        curl_easy_cleanup(curl);
    }

    // close this window
    this->back();
}

void Feedback::back()
{
    MainDisplay::subscreen = NULL;  // todo: clean up memory
}
