#include <sstream>

#include "../libs/get/src/Get.hpp"
#include "../libs/get/src/Utils.hpp"

#include "../libs/chesto/src/Button.hpp"
#include "../libs/chesto/src/NetImageElement.hpp"
#include "../libs/chesto/src/RootDisplay.hpp"

#include "rapidjson/document.h"

#include "AboutScreen.hpp"
#include "Feedback.hpp"
#include "main.hpp"
#include "ThemeManager.hpp"

#define AVATAR_URL "https://avatars.githubusercontent.com/u/"

using namespace rapidjson;

AboutScreen::AboutScreen(Get* get)
	: get(get)
{
	rebuildUI();
}

void AboutScreen::rebuildUI()
{
	removeAll();
	creditCount = 0;

	// create a full-screen scrollable list to hold all credits content
	// TODO: common way to have scrollable Screen's
	auto scrollContainer = std::make_unique<ListElement>();
	scrollContainer->width = SCREEN_WIDTH;
	scrollContainer->height = SCREEN_HEIGHT;
	scrollList = scrollContainer.get(); // Keep pointer for scrolling

	auto cancel = std::make_unique<Button>(i18n("credits.goback"), B_BUTTON, false, 29);
	cancel->position(30, 30);
	cancel->action = std::bind(&AboutScreen::back, this);
	scrollContainer->addNode(std::move(cancel));

	int MARGIN = 550;

	auto feedback = std::make_unique<Button>(i18n("credits.feedback"), A_BUTTON, false, 17);
	feedback->position(MARGIN + 500, 30);
	feedback->action = std::bind(&AboutScreen::launchFeedback, this);
	scrollContainer->addNode(std::move(feedback));

	auto title = std::make_unique<TextElement>(i18n("credits.title"), 35, &HBAS::ThemeManager::textPrimary);
	title->position(MARGIN, 40);
	scrollContainer->addNode(std::move(title));

	auto subtitle = std::make_unique<TextElement>(i18n("credits.subtitle"), 25, &HBAS::ThemeManager::textPrimary);
	subtitle->position(MARGIN, 80);
	scrollContainer->addNode(std::move(subtitle));

	auto ftuLogo = std::make_unique<NetImageElement>(AVATAR_URL "40721862", []
		  { return new ImageElement(RAMFS "res/4TU.png"); });
	ftuLogo->position(375, 15);
	ftuLogo->resize(140, 140);
	scrollContainer->addNode(std::move(ftuLogo));

	auto creds = std::make_unique<TextElement>((i18n("credits.license") + "\n\n" + i18n("credits.cta")), 20, &HBAS::ThemeManager::textPrimary, false, 1240);
	creds->position(100, 170);
	scrollContainer->addNode(std::move(creds));

	loadCreditsFromJSON();
	
	addNode(std::move(scrollContainer));
}


void AboutScreen::credHead(const std::string& header, const std::string& blurb)
{
	creditCount += (4 - creditCount % 4) % 4;
	
	auto text = std::make_unique<TextElement>(header, 30, &HBAS::ThemeManager::textPrimary);
	text->position(40, 250 + 60 + creditCount / 4 * 160);
	if (scrollList) scrollList->addNode(std::move(text));

	auto desc = std::make_unique<TextElement>(blurb, 23, &HBAS::ThemeManager::textSecondary, false, 1200);
	desc->position(40, 250 + 105 + creditCount / 4 * 160);
	if (scrollList) scrollList->addNode(std::move(desc));

	creditCount += 4;
}

void AboutScreen::credit(const std::string& username,
	const std::string& githubId,
	const std::string& bsky,
	const std::string& github,
	const std::string& gitlab,
	const std::string& patreon,
	const std::string& url,
	const std::string& discord,
	const std::string& directAvatarUrl,
	const std::string& youtube,
	const std::string& mastodon)
{
	int X = 40;
	int Y = 310;

	int myX = creditCount % 4 * 300 + X;
	int myY = creditCount / 4 * 160 + Y;

	std::string avatarUrl = !directAvatarUrl.empty() ? directAvatarUrl
	                                                  : (std::string(AVATAR_URL) + githubId + "?s=100");
	auto userLogo = std::make_unique<NetImageElement>(avatarUrl.c_str());
	userLogo->position(myX, myY);
	userLogo->resize(100, 100);
	if (scrollList) scrollList->addNode(std::move(userLogo));

	auto name = std::make_unique<TextElement>(username, 27, &HBAS::ThemeManager::textPrimary);
	name->position(myX + 110, myY);
	if (scrollList) scrollList->addNode(std::move(name));

	int socialCount = 0;
	std::string handles[8] = { bsky, github, gitlab, patreon, url, discord, youtube, mastodon };
	std::string icons[8] = { "bsky", "github", "gitlab", "patreon", "url", "discord", "youtube", "mastodon" };

	for (int x = 0; x < 8 && socialCount < 2; x++)
	{
		if (handles[x].length() == 0) continue;

		// if the text starts and ends with ( and ), don't show an icon (used for locale indication)
		bool isLocaleIndicator = (handles[x].length() >= 2 && 
		                          handles[x][0] == '(' && 
		                          handles[x][handles[x].length() - 1] == ')');
		
		if (!isLocaleIndicator)
		{
			auto icon = std::make_unique<ImageElement>(((std::string(RAMFS "res/") + icons[x]) + ".png").c_str());
			icon->resize(20, 20);
			icon->position(myX + 110, myY + 45 + socialCount * 25);
			if (scrollList) scrollList->addNode(std::move(icon));
		}

		auto link = std::make_unique<TextElement>(handles[x], 14, &HBAS::ThemeManager::textSecondary);
		link->position(myX + 140, myY + 45 + socialCount * 25);
		if (scrollList) scrollList->addNode(std::move(link));

		socialCount++;
	}

	creditCount++;
}

void AboutScreen::loadCreditsFromJSON()
{
	std::string jsonContent;
	std::string creditsUrl = std::string(META_REPO_1) + "/credits.json";
	
	bool success = downloadFileToMemory(creditsUrl, &jsonContent);
	
	if (success && !jsonContent.empty())
	{
		parseCreditsJSON(jsonContent);
	}
	else
	{
		printf("--> Failed to load credits from %s\n", creditsUrl.c_str());
	}
}

const char* AboutScreen::getJsonString(const Value& obj, const char* key)
{
	if (obj.HasMember(key) && obj[key].IsString())
	{
		return obj[key].GetString();
	}
	return nullptr;
}

void AboutScreen::parseCreditsJSON(const std::string& jsonContent)
{
	Document doc;
	doc.Parse(jsonContent.c_str());
	
	if (doc.HasParseError() || !doc.IsObject() || !doc.HasMember("credits"))
	{
		printf("--> Error parsing credits JSON\n");
		return;
	}
	
	const Value& creditsArray = doc["credits"];
	if (!creditsArray.IsArray())
	{
		printf("--> Credits is not an array\n");
		return;
	}
	
	for (SizeType i = 0; i < creditsArray.Size(); i++)
	{
		const Value& section = creditsArray[i];
		
		if (!section.IsObject())
			continue;
		
		// header and details
		const char* nameStr = getJsonString(section, "section");
		std::string sectionName =  nameStr ? nameStr : "";
		const char* detailsStr = getJsonString(section, "details");
		std::string sectionDetails =  detailsStr ? detailsStr : "";
		
		if (!sectionName.empty())
		{
			credHead(sectionName, sectionDetails);
		}
		
		// for each user under this header
		if (section.HasMember("users") && section["users"].IsArray())
		{
			const Value& users = section["users"];
			
			for (SizeType j = 0; j < users.Size(); j++)
			{
				const Value& user = users[j];
				
				if (!user.IsObject())
					continue;
				
				// most social/url fields are optional
				const char* name = getJsonString(user, "name");
				const char* githubId = getJsonString(user, "githubId");
				const char* bsky = getJsonString(user, "bsky");
				const char* github = getJsonString(user, "github");
				const char* gitlab = getJsonString(user, "gitlab");
				const char* patreon = getJsonString(user, "patreon");
				const char* url = getJsonString(user, "url");
				const char* discord = getJsonString(user, "discord");
				const char* directAvatarUrl = getJsonString(user, "directAvatarURL");
				const char* youtube = getJsonString(user, "youtube");
				const char* mastodon = getJsonString(user, "mastodon");
				
				if (name && (githubId || directAvatarUrl)) // username and (githubId or directAvatarUrl) required
				{
					credit(
						name ? name : "",
						githubId ? githubId : "",
						bsky ? bsky : "",
						github ? github : "",
						gitlab ? gitlab : "",
						patreon ? patreon : "",
						url ? url : "",
						discord ? discord : "",
						directAvatarUrl ? directAvatarUrl : "",
						youtube ? youtube : "",
						mastodon ? mastodon : ""
					);
				}
			}
		}
	}
}

void AboutScreen::render(Element* parent)
{
	if (this->parent == NULL)
		this->parent = parent;

	// draw a white background, width of the screen
	CST_Rect dimens = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

	CST_SetDrawColor(RootDisplay::renderer, HBAS::ThemeManager::background);
	CST_FillRect(RootDisplay::renderer, &dimens);

	super::render(parent);
}

bool AboutScreen::process(InputEvents* event)
{
	return Screen::process(event) || event->isTouchDrag();
}

void AboutScreen::back()
{
	RootDisplay::popScreen(); // stow this subscreen
}

void AboutScreen::launchFeedback()
{
	// find the package corresponding to us
	for (auto& package : this->get->getPackages())
	{
		if (package->getPackageName() == APP_SHORTNAME)
		{
			RootDisplay::pushScreen(std::make_unique<Feedback>(*package));
			break;
		}
	}
}
