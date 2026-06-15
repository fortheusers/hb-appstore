#include "FeedbackCenter.hpp"
#include "ThemeManager.hpp"
#include "Feedback.hpp"
#include "main.hpp"
#include "MainDisplay.hpp"

#include "rapidjson/document.h"
#include "rapidjson/rapidjson.h"

#include "../libs/get/src/Utils.hpp"
#include "../libs/chesto/src/NetImageElement.hpp"
#include "../libs/chesto/src/Button.hpp"
#include "../libs/chesto/src/ImageElement.hpp"
#include "../libs/chesto/src/TextElement.hpp"
#include "../libs/chesto/src/ListElement.hpp"
#include "../libs/chesto/src/Container.hpp"
#include "../libs/chesto/src/DrawUtils.hpp"
#include "../libs/chesto/src/Constraint.hpp"


#ifndef NETWORK_MOCK
#include <curl/curl.h>
#include <curl/easy.h>
#endif

using namespace rapidjson;

#define MESSAGES_URL "https://wiiubru.com/feedback/messages"

class RectangleElement : public Element
{
public:
    RectangleElement(int width, int height, rgb color)
    {
        this->backgroundColor = color;

        this->width = width;
        this->height = height;
    }

    void render(Element*)
    {
        CST_Rect rect = {
            this->xAbs,
            this->yAbs,
            this->xAbs + this->width,
            this->yAbs + this->height
        };
        CST_SetDrawColorRGBA(RootDisplay::mainDisplay->renderer,
            static_cast<Uint8>(backgroundColor.r * 0xFF),
            static_cast<Uint8>(backgroundColor.g * 0xFF),
            static_cast<Uint8>(backgroundColor.b * 0xFF),
            0xFF
        );
        CST_FillRect(RootDisplay::mainDisplay->renderer, &rect);
    }
};

FeedbackMessage::FeedbackMessage()
{

}

void FeedbackMessage::build()
{
    // build icon url from package name and repo
    auto repos = ((MainDisplay*)RootDisplay::mainDisplay)->get->getRepos();
    auto firstEnabled = std::find_if(repos.begin(), repos.end(), [](const std::shared_ptr<Repo>& repo){
        return repo->isEnabled();
    });
    // if we can't find one, there's nothing we can do!
    if (firstEnabled == repos.end()) {
        return;
    }
    std::string url = firstEnabled->get()->getUrl();
    url += "/packages/";
    url += package;
    url += "/icon.png";

    auto img = std::make_unique<NetImageElement>(url.c_str(), []{
        return new ImageElement(RAMFS "res/default.png");
    });
    img->setScaleMode(SCALE_PROPORTIONAL_WITH_BG);
    img->resize(256 / 2, ICON_SIZE / 2);
    img->setPosition(20, 0);

    Container* container = createNode<Container>(ROW_LAYOUT, 30);
    container->addNode(std::move(img));

    auto contentText = std::make_unique<TextElement>(content.c_str(), 20, &HBAS::ThemeManager::textPrimary, NORMAL, 600);
    contentText->position(200, 0);
    contentText->update();
    auto* contentPtr = contentText.get();
    container->addNode(std::move(contentText));

    auto replyText = std::make_unique<TextElement>(reply.c_str(), 20, &HBAS::ThemeManager::textPrimary, NORMAL, 400);
    replyText->position(800, 0);
    replyText->update();
    auto* replyPtr = replyText.get();
    container->addNode(std::move(replyText));

    this->height = std::max(100, std::max(contentPtr->height, replyPtr->height)) + 20;
}

FeedbackCenter::FeedbackCenter()
{
	rebuildUI();
}

void FeedbackCenter::rebuildUI()
{
	removeAll();
	
	ListElement* list = createNode<ListElement>();

#ifndef NETWORK_MOCK
	CURL* curl;
	curl = curl_easy_init();
	if (curl)
	{
        // TODO: get previously submitted IDs from local store
		std::string resp;
        downloadFileToMemory(MESSAGES_URL "?ids=11152,11126,11094,11170,11163", &resp);
        
		Document doc;
        ParseResult ok = doc.Parse(resp.c_str());

        if (!ok || !doc.HasMember("messages"))
        {
            printf("--> Invalid response from feedback center");
            return;
        }

        const Value& msgs_doc = doc["messages"];

        // for every message
        int curHeight = 0; // sum of all heights of previous messages
        for (Value::ConstValueIterator it = msgs_doc.Begin(); it != msgs_doc.End(); it++)
        {
            auto msg = std::make_unique<FeedbackMessage>();
            msg->package = (*it)["package"].GetString();
            msg->content = (*it)["content"].GetString();
            msg->reply   = (*it)["reply"].GetString();
            msg->build();
            msg->position(0, 130 + curHeight);
            curHeight += msg->height;

            // add as a child to this view
            list->addNode(std::move(msg));
        }

		curl_easy_cleanup(curl);
	}

#endif

    auto wroteLabel = std::make_unique<TextElement>("You wrote:", 15, &HBAS::ThemeManager::textSecondary);
    wroteLabel->setPosition(200, 100);
    list->addNode(std::move(wroteLabel));
    
    auto responseLabel = std::make_unique<TextElement>("Our response:", 15, &HBAS::ThemeManager::textSecondary);
    responseLabel->setPosition(800, 100);
    list->addNode(std::move(responseLabel));

    createNode<RectangleElement>(
        SCREEN_WIDTH,
        85,
        RootDisplay::mainDisplay->backgroundColor
    );

    auto header = createNode<TextElement>("Feedback Center", 35);
	header->position(0, 15);
	header->constrain(ALIGN_CENTER_HORIZONTAL, 0);

    // back button
    createNode<Button>(i18n("details.back"), B_BUTTON, true)
        ->setPosition(35, 15)
        ->setAction([]{ RootDisplay::popScreen(); });

    // credits button
    auto feedback = createNode<Button>(i18n("credits.feedback"), Y_BUTTON, true)
        ->setAction([]{
            // find the package corresponding to us
            for (auto& package : ((MainDisplay*)RootDisplay::mainDisplay)->get->getPackages())
            {
                std::cout << "Checking package: " << package->getPackageName() << std::endl;
                if (package->getPackageName() == APP_SHORTNAME)
                {
                    RootDisplay::pushScreen(std::make_unique<Feedback>(*package));
                    break;
                }
            }
        });
    feedback->constrain(ALIGN_TOP | ALIGN_RIGHT, 10);

    
}

void FeedbackCenter::render(Element* parent)
{
	if (this->parent == NULL)
		this->parent = parent;

	// draw a white background, width of the screen
	CST_Rect dimens = { 0, 85, SCREEN_WIDTH, SCREEN_HEIGHT };

	CST_SetDrawColor(RootDisplay::renderer, HBAS::ThemeManager::background);
	CST_FillRect(RootDisplay::renderer, &dimens);

	super::render(parent);
}