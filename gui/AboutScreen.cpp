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
	, cancel(i18n("credits.goback"), B_BUTTON, false, 29)
	, feedback(i18n("credits.feedback"), A_BUTTON, false, 17)
	, title(i18n("credits.title"), 35, &HBAS::ThemeManager::textPrimary)
	, subtitle(i18n("credits.subtitle"), 25, &HBAS::ThemeManager::textPrimary)
	, ftuLogo(AVATAR_URL "40721862", []
		  { return new ImageElement(RAMFS "res/4TU.png"); })
	, creds((i18n("credits.license") + "\n\n" + i18n("credits.cta")), 20, &HBAS::ThemeManager::textPrimary, false, 1240)
{

	// TODO: show current app status somewhere

	// download/update/remove button (2)

	cancel.position(30, 30);
	cancel.action = std::bind(&AboutScreen::back, this);
	super::append(&cancel);

	int MARGIN = 550;

	feedback.position(MARGIN + 500, 30);
	feedback.action = std::bind(&AboutScreen::launchFeedback, this);
	super::append(&feedback);

	title.position(MARGIN, 40);
	super::append(&title);

	subtitle.position(MARGIN, 80);
	super::append(&subtitle);

	ftuLogo.position(375, 15);
	ftuLogo.resize(140, 140);
	super::append(&ftuLogo);

	creds.position(100, 170);
	super::append(&creds);

	// credits are fetched dynamically from meta repo json
	loadCreditsFromJSON();
}

AboutScreen::~AboutScreen()
{
	super::removeAll();
	for (auto& i : creditHeads)
	{
		delete i.text;
		delete i.desc;
	}
	for (auto& i : credits)
	{
		delete i.userLogo;
		delete i.name;
		delete i.social[0].icon;
		delete i.social[0].link;
		delete i.social[1].icon;
		delete i.social[1].link;
	}
}

void AboutScreen::credHead(const std::string& header, const std::string& blurb)
{
	auto head = creditHeads.emplace(creditHeads.end());

	creditCount += (4 - creditCount % 4) % 4;
	head->text = new TextElement(header, 30, &HBAS::ThemeManager::textPrimary);
	head->text->position(40, 250 + 60 + creditCount / 4 * 160);
	super::append(head->text);

	head->desc = new TextElement(blurb, 23, &HBAS::ThemeManager::textSecondary, false, 1200);
	head->desc->position(40, 250 + 105 + creditCount / 4 * 160);
	super::append(head->desc);

	creditCount += 4;
}

void AboutScreen::credit(const char* username,
	const char* githubId,
	const char* bsky,
	const char* github,
	const char* gitlab,
	const char* patreon,
	const char* url,
	const char* discord,
	const char* directAvatarUrl,
	const char* youtube,
	const char* mastodon)
{
	int X = 40;
	int Y = 310;

	int myX = creditCount % 4 * 300 + X;
	int myY = creditCount / 4 * 160 + Y;

	auto cred = credits.emplace(credits.end());

	auto avatar = directAvatarUrl ? directAvatarUrl : (std::string(AVATAR_URL) + githubId + "?s=100").c_str();
	cred->userLogo = new NetImageElement(directAvatarUrl != NULL ? directAvatarUrl : ((std::string(AVATAR_URL) + githubId + "?s=100").c_str()));
	cred->userLogo->position(myX, myY);
	cred->userLogo->resize(100, 100);
	super::append(cred->userLogo);

	cred->name = new TextElement(username, 27, &HBAS::ThemeManager::textPrimary);
	cred->name->position(myX + 110, myY);
	super::append(cred->name);

	int socialCount = 0;

	const char* handles[8] = { bsky, github, gitlab, patreon, url, discord, youtube, mastodon };
	const char* icons[8] = { "bsky", "github", "gitlab", "patreon", "url", "discord", "youtube", "mastodon" };

	for (int x = 0; x < 8; x++)
	{
		if (handles[x] == NULL) continue;

		cred->social[socialCount].icon = new ImageElement(((std::string(RAMFS "res/") + icons[x]) + ".png").c_str());
		cred->social[socialCount].icon->resize(20, 20);
		cred->social[socialCount].icon->position(myX + 110, myY + 45 + socialCount * 25);
		super::append(cred->social[socialCount].icon);

		cred->social[socialCount].link = new TextElement(handles[x], 14, &HBAS::ThemeManager::textSecondary);
		cred->social[socialCount].link->position(myX + 140, myY + 45 + socialCount * 25);
		super::append(cred->social[socialCount].link);

		socialCount++;

		if (socialCount >= 2) break;
	}

	creditCount++;
}

void AboutScreen::loadCreditsFromJSON()
{
	std::string jsonContent;
	std::string creditsUrl = std::string(META_REPO) + "/credits.json";
	
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
						name,
						githubId, bsky, github, gitlab,
						patreon, url, discord, directAvatarUrl,
						youtube, mastodon);
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
	bool ret = false;
	ret |= ListElement::processUpDown(event);
	return ret || ListElement::process(event);
}

void AboutScreen::back()
{
	RootDisplay::switchSubscreen(nullptr);
}

void AboutScreen::launchFeedback()
{
	// find the package corresponding to us
	for (auto& package : this->get->getPackages())
	{
		if (package->getPackageName() == APP_SHORTNAME)
		{
			RootDisplay::switchSubscreen(new Feedback(*package));
			break;
		}
	}
}
