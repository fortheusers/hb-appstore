#ifndef ABOUTSCREEN_H_
#define ABOUTSCREEN_H_

#include "../libs/get/src/Get.hpp"

#include "../libs/chesto/src/ImageElement.hpp"
#include "../libs/chesto/src/ListElement.hpp"
#include "../libs/chesto/src/Screen.hpp"
#include "../libs/chesto/src/TextElement.hpp"

#include "rapidjson/document.h"

using namespace Chesto;

class AboutScreen : public Screen
{
public:
	AboutScreen(Get* get);

	void render(Element* parent) override;
	bool process(InputEvents* event) override;
	void rebuildUI() override;

	// button bindings
	void back();
	void launchFeedback();
	
	void credHead(const std::string& header, const std::string& blurb);
	void credit(const std::string& username,
				const std::string& githubId,
				const std::string& bsky = "",
				const std::string& github = "",
				const std::string& gitlab = "",
				const std::string& patreon = "",
				const std::string& url = "",
				const std::string& discord = "",
				const std::string& directAvatarUrl = "",
				const std::string& youtube = "",
				const std::string& mastodon = "");

	// JSON loading methods
	void loadCreditsFromJSON();
	void parseCreditsJSON(const std::string& jsonContent);
	const char* getJsonString(const rapidjson::Value& obj, const char* key);

	int creditCount = 0;
	ListElement* scrollList = nullptr;

private:
	Get* get = nullptr;
};

#endif
