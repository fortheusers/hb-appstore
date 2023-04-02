#include "FeedbackCenter.hpp"
#include "ImageCache.hpp"
#include "main.hpp"

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


#ifndef NETWORK_MOCK
#include <curl/curl.h>
#include <curl/easy.h>
#endif

using namespace rapidjson;

#define MESSAGES_URL "https://wiiubru.com/feedback/messages"
#define REPO_URL "https://switch.cdn.fortheusers.org"

class RectangleElement : public Element
{
public:
    RectangleElement(int width, int height, rgb color)
    {
        this->backgroundColor = color;

        this->width = width;
        this->height = height;
    }

    void render(Element* parent)
    { 
        CST_Rect rect = {
            xOff + this->x,
            yOff + this->y,
            xOff + this->x + this->width,
            yOff + this->y + this->height
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
    CST_Color black = { 0, 0, 0, 0xff };

    // build icon url from package name and repo
    std::string url = REPO_URL;
    url += "/packages/";
    url += package;
    url += "/icon.png";

    NetImageElement* img = new NetImageElement(url.c_str(), []{
        return new ImageElement(RAMFS "res/default.png");
    });
    img->setScaleMode(SCALE_PROPORTIONAL_WITH_BG);
    img->resize(256 / 2, ICON_SIZE / 2);
    img->setPosition(20, 0);

    Container* container = new Container(ROW_LAYOUT, 30);
    container->child(img);

    TextElement* contentText = new TextElement(content.c_str(), 20, &black, NORMAL, 600);
    contentText->position(200, 0);
    contentText->update();
    container->child(contentText);

    TextElement* replyText = new TextElement(reply.c_str(), 20, &black, NORMAL, 400);
    replyText->position(800, 100);
    replyText->update();
    container->child(replyText);

    child(container);

    this->height = std::max(100, std::max(contentText->height, replyText->height)) + 20;
}

FeedbackCenter::FeedbackCenter(AppList* appList)
{
    TextElement* header = new TextElement("Feedback Center", 35);
    this->width = RootDisplay::mainDisplay->width; // TODO: chesto should handle this... eg. default all new full screen views to the dimensions of the main display
    this->height = RootDisplay::mainDisplay->height;

    header->position(0, 15);
    header->centerHorizontallyIn(this);
    ListElement* list = new ListElement();

#ifndef NETWORK_MOCK
	CURL* curl;
	CURLcode res;
	curl = curl_easy_init();
	if (curl)
	{
        // TODO: get previously submitted IDs from local store
		std::string resp;
        downloadFileToMemory(MESSAGES_URL "?ids=8975,8974,8969,8951,8940,8957,8956", &resp);
        
		Document doc;
        ParseResult ok = doc.Parse(resp.c_str());

        if (!ok || !doc.HasMember("messages"))
        {
            printf("--> Invalid response from feedback center");
            return;
        }

        const Value& msgs_doc = doc["messages"];

        // for every message
        int count = 0;
        int curHeight = 0; // sum of all heights of previous messages
        for (Value::ConstValueIterator it = msgs_doc.Begin(); it != msgs_doc.End(); it++)
        {
            FeedbackMessage* msg = new FeedbackMessage();
            msg->package = (*it)["package"].GetString();
            msg->content = (*it)["content"].GetString();
            msg->reply   = (*it)["reply"].GetString();
            msg->build();
            msg->position(0, 130 + curHeight);
            count++;
            curHeight += msg->height;

            // add as a child to this view
            list->child(msg);
        }

        child(list);
		curl_easy_cleanup(curl);
	}

#endif

    CST_Color gray = { 0x77, 0x77, 0x77, 0xff };

    list->child((new TextElement("You wrote:", 15, &gray))->setPosition(200, 100));
    list->child((new TextElement("Our response:", 15, &gray))->setPosition(800, 100));

    RectangleElement* rect = new RectangleElement(
        1280,
        85,
        RootDisplay::mainDisplay->backgroundColor
    );
    child(rect);
    child(header);

    // back button
    child((new Button("Back", B_BUTTON, true))->setPosition(35, 15)->setAction([]{
        RootDisplay::switchSubscreen(nullptr);
    }));

    // credits button
    child((new Button("Credits", START_BUTTON, true))->setPosition(SCREEN_WIDTH - 200, 15)->setAction([appList]{
        appList->launchSettings(true);
    }));
    
}

void FeedbackCenter::render(Element* parent)
{
	if (this->parent == NULL)
		this->parent = parent;

	// draw a white background, 1280 wide
	CST_Rect dimens = { 0, 85, 1280, 720 };

	CST_Color white = { 0xff, 0xff, 0xff, 0xff };
	CST_SetDrawColor(RootDisplay::renderer, white);
	CST_FillRect(RootDisplay::renderer, &dimens);

	super::render(parent);
}