#include "Feedback.hpp"
#include "ImageCache.hpp"
#include "MainDisplay.hpp"
#include "main.hpp"

#include "../libs/chesto/src/RootDisplay.hpp"

#include <curl/curl.h>
#include <curl/easy.h>

Feedback::Feedback(Package* package)
	: package(package)
	, message("")
	, title((std::string("Leaving feedback for: \"") + package->title + "\"").c_str(), 25)
	, icon(package->getIconUrl().c_str(), []{ return new ImageElement(RAMFS "res/default.png"); })
	, keyboard(NULL, &message, this)
	, quit("Discard", Y_BUTTON, true, 24)
	, send("Submit", X_BUTTON, true, 24)
	, response("If you need to send more detailed feedback, please email us at fight@fortheusers.org", 20, NULL, false, 360)
#if defined(__WIIU__)
	, hint("(btw you can press Minus to exit!)", 20, NULL)
#endif
{
	title.position(50, 30);
	super::append(&title);

	icon.position(50, 160);
	icon.resize(256, ICON_SIZE);
	super::append(&icon);

	keyboard.x = 200;
	super::append(&keyboard);

	quit.position(470, 340);
	quit.action = std::bind(&Feedback::back, this);
	super::append(&quit);

	send.position(quit.x + quit.width + 25, quit.y);
	send.action = std::bind(&Feedback::submit, this);
	super::append(&send);

	response.position(860, 20);
	super::append(&response);

#if defined(__WIIU__)
	hint.position(50, 120);
	super::append(&hint);
#endif

	this->refresh();
}

Feedback::~Feedback()
{
	if (feedback)
	{
		super::remove(feedback);
		delete feedback;
	}
}

void Feedback::refresh()
{
	if (feedback)
	{
		super::remove(feedback);
		delete feedback;
	}

	feedback = new TextElement(this->message.c_str(), 23, NULL, false, 730);
	feedback->position(390, 140);
	super::append(feedback);
}

void Feedback::submit()
{
	CURL* curl;
	CURLcode res;

#if defined(__WIIU__)
	const char* userKey = "wiiu_user";
#else
	const char* userKey = "switch_user";
#endif

	curl = curl_easy_init();
	if (curl)
	{
		curl_easy_setopt(curl, CURLOPT_URL, "https://switchbru.com/appstore/feedback");
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
	RootDisplay::switchSubscreen(nullptr);
}
