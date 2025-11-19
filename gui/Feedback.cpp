#include "Feedback.hpp"
#include "MainDisplay.hpp"
#include "ThemeManager.hpp"
#include "main.hpp"
#include <sstream>

#include "../libs/chesto/src/RootDisplay.hpp"

#ifndef NETWORK_MOCK
#include <curl/curl.h>
#include <curl/easy.h>
#endif

Feedback::Feedback(Package& package)
	: package(&package)
	, title((std::string(i18n("feedback.leaving") + " \"") + package.getTitle() + "\""), 25)
	, icon(package.getIconUrl().c_str(), []{ return new ImageElement(RAMFS "res/default.png"); })
	, quit(i18n("feedback.discard"), Y_BUTTON, isDark, 20)
	, send(i18n("feedback.submit"), X_BUTTON, isDark, 20)
	, backspaceBtn(i18n("feedback.delete"), B_BUTTON, isDark, 15)
	, capsBtn(i18n("feedback.caps"), L_BUTTON, isDark, 15)
	, response(i18n("feedback.help"), 20, NULL, false, 460)
{
	title.position(50, 30);
	super::append(&title);

	icon.setScaleMode(SCALE_PROPORTIONAL_WITH_BG);
	icon.position(50, 100);
#if defined(_3DS) || defined(_3DS_MOCK)
  icon.resize(ICON_SIZE, ICON_SIZE);
#else
	icon.resize(256/SCALER, ICON_SIZE/SCALER);
#endif
	super::append(&icon);

	// keyboard.hasRoundedKeys = true;

	keyboard.typeAction = std::bind(&Feedback::keyboardInputCallback, this);
	keyboard.preventEnterAndTab = true;
	keyboard.updateSize();
	super::append(&keyboard);

	quit.position(895, 240);
	quit.action = std::bind(&Feedback::back, this);
	super::append(&quit);

	send.position(quit.x + quit.width + 15, quit.y);
	send.action = std::bind(&Feedback::submit, this);
	super::append(&send);

	backspaceBtn.position(quit.x - 15 - backspaceBtn.width, send.y);
	backspaceBtn.action = [this](void) {
		this->keyboard.backspace();
	};
	super::append(&backspaceBtn);

	capsBtn.position(backspaceBtn.x - 15 - capsBtn.width, send.y);
	capsBtn.action = [this](void) {
		this->keyboard.capsOn = !this->keyboard.capsOn;
		this->keyboard.updateSize();
	};
	super::append(&capsBtn);

	response.position(860, 20);
	super::append(&response);

	feedback.setSize(23);
	feedback.setWrappedWidth(730);
	feedback.position(390, 100);
	super::append(&feedback);
}

void Feedback::render(Element* parent)
{
	// draw a white background, 870 wiz
	CST_Color white = { 0xff, 0xff, 0xff, 0xff };

	  if (parent != NULL) {
    CST_SetDrawColor(RootDisplay::renderer, white);
  }

	return super::render(parent);
}

bool Feedback::process(InputEvents* event)
{
	bool ret = super::process(event);

	if (needsRefresh)
	{
		feedback.setText(keyboard.getTextInput());
		feedback.update();
		needsRefresh = false;
	}

	return ret;
}

void Feedback::keyboardInputCallback()
{
	needsRefresh = true;
}

void Feedback::submit()
{
#ifndef NETWORK_MOCK
	CURL* curl;
	CURLcode res;

// TODO: store a user specific key after first feed
#if defined(__WIIU__)
	const char* userKey = "wiiu_user";
#elif defined(WII)
	const char* userKey = "wii_user";
#else
	const char* userKey = "switch_user";
#endif

	curl = curl_easy_init();
	if (curl)
	{
		curl_easy_setopt(curl, CURLOPT_URL, "http://switchbru.com/appstore/feedback");

		auto mainDisplay = (MainDisplay*)RootDisplay::mainDisplay;

		int installedPackageCount = 0;
		for (auto& package : mainDisplay->get->getPackages())
		{
			if (package->getStatus() != GET) {
				installedPackageCount++;
			}
		}

		std::ostringstream oss;
		oss << "name=" << userKey
			<< "&package=" << package->getPackageName()
			<< "&message=" << keyboard.getTextInput()
			<< "&platform=" << PLATFORM
			<< "&package_version=" << package->getVersion()
			<< "&hbas_version=" << APP_VERSION
			<< "&is_low_memory=" << mainDisplay->isLowMemoryMode()
			<< "&installed_packages=" << installedPackageCount
			<< "&is_this_pkg_installed=" << (package->getStatus() != GET);
			// TODO: support these fields in the UI
			// << "&rating=" << getRating()
			// << "&is_anonymous=" << isAnonymous()

		std::string fields = oss.str();
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, fields.c_str());

		res = curl_easy_perform(curl);

		/* always cleanup */
		curl_easy_cleanup(curl);
	}
#endif

	// close this window
	this->back();
}

void Feedback::back()
{
	RootDisplay::switchSubscreen(nullptr);
}
