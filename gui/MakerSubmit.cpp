#include "MakerSubmit.hpp"
#include "libget/src/Package.hpp"
#include "libs/get/src/ZipUtil.hpp"
#include "MarioMaker.hpp"
#include "Button.hpp"
#include "ListElement.hpp"
#include "MainDisplay.hpp"
#include <switch.h>
#include <curl/curl.h>
#include <curl/easy.h>
#include <iomanip>

MakerSubmit::MakerSubmit(Package *package, AppList *appList, MarioMaker *mario)
{
    this->mario = mario;
    this->package = package;
    this->applist = applist;
    this->MARGIN = 60;
    this->SBSTART = 1280-336;
    this->level = mario->levels[std::stoi(package->pkg_name)];

    SDL_Surface *g = IMG_Load(ROMFS "./res/grid.png");
    this->grid = SDL_CreateTextureFromSurface(MainDisplay::mainRenderer, g);

    this->update();
}

std::string openKB(std::string ok, std::string head, std::string sub, std::string guide)
{
    char tmpout[128];
    SwkbdConfig kbd;
    swkbdCreate(&kbd, 0);
    swkbdConfigMakePresetDefault(&kbd);
    swkbdConfigSetOkButtonText(&kbd, ok.c_str());
    swkbdConfigSetHeaderText(&kbd, head.c_str());
    swkbdConfigSetSubText(&kbd, sub.c_str());
    swkbdConfigSetGuideText(&kbd, guide.c_str());

    swkbdShow(&kbd, tmpout, sizeof(tmpout));
    std::string out = std::string(tmpout);
    swkbdClose(&kbd);
    return out;
}

std::string u128ToString(u128 num) {
    std::string str;
    do {
        u64 digits = num % 10000000000000000000U;
        auto digitsStr = std::to_string(digits);
        auto leading0s = (digits != num) ? std::string(19 - digitsStr.length(), '0') : "";
        str = leading0s + digitsStr + str;
        num = (num - digits) / 10000000000000000000U;
    } while (num != 0);
    return str;
}

void MakerSubmit::cSubmit()
{
    CURL* curl;
    curl = curl_easy_init();
    if (curl)
    {
        mkdir("sdmc:/switch/appstore/mario/", 0777);
        Zip *zip = new Zip("sdmc:/switch/appstore/mario/course.zip");
        
        zip->AddFile("thumb.bin", level->tpath.c_str());

        zip->AddFile("course.bin", level->cpath.c_str());
        
        zip->AddFile("replay.bin", level->rpath.c_str());

        zip->Close();

        FILE *zipIn = fopen("sdmc:/switch/appstore/mario/course.zip", "rb");

        curl_easy_setopt(curl, CURLOPT_URL, "https://dragonite.fortheusers.org/mmcourse");
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
        curl_easy_setopt(curl, CURLOPT_READDATA, zipIn);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);

        char* epc = curl_easy_escape(curl, package->title.c_str(), 0);
        std::string escpkg = std::string(epc);
        curl_free(epc);

	    std::stringstream fields;
        fields << "/" << u128ToString(mario->uuid) << "/" << package->pkg_name << "-" << escpkg;
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, fields.str().c_str());

        CURLcode res = curl_easy_perform(curl);
        fclose(zipIn);
        curl_easy_cleanup(curl);
    }
}

void setDescBtn(MakerSubmit *m)
{
    m->package->long_desc = openKB("Set", "Edit Description", "Enter a description for your course!", "Description");
    m->wipeElements();
    m->update();
}

void backBtn()
{
    MainDisplay::subscreen = NULL;
}

void MakerSubmit::update()
{
    TextElementTitle *pgTitle = new TextElementTitle("Course Bot", 40, &black, MAKER);
    pgTitle->position(MARGIN, 20);

    TextElement *dt = new TextElement("Description", 35, &black, MAKER);
    dt->position(MARGIN, 110);
    Button *desc = new Button("Edit", Y_BUTTON);
    desc->position(MARGIN, 160);
    desc->action = std::bind(setDescBtn, this);

    this->descContent = new TextElement(this->package->long_desc.c_str(), 25, &black);
    descContent->position(MARGIN, 210);

    ScrollContent *descl = new ScrollContent();
    descl->position(MARGIN, 110);
    descl->width = 1280-360;
    descl->height = 1024;

    descl->elements.push_back(dt);
    descl->elements.push_back(this->descContent);
    descl->elements.push_back(desc);

    this->elements.push_back(descl);
    this->elements.push_back(pgTitle);

    TextElement *title = new TextElement(package->title.c_str(), 35, &black, MAKER);
    title->position(SBSTART+48, 48);
    this->elements.push_back(title);

    ImageElement *thumb = new ImageElement(false, mario->levels[std::stoi(package->pkg_name)]->thumb, 0x1C000);
    thumb->position(SBSTART+48, 48*2);
    thumb->resize(48*5, 144);
    this->elements.push_back(thumb);

    Button *submit = new Button("Submit", A_BUTTON, true, 25, 48*4);
    submit->position(SBSTART+48, 48*11);
    //submit->action = std::bind(&MakerSubmit::cSubmit, this);
    this->elements.push_back(submit);

    Button *cancel = new Button("Cancel", B_BUTTON, true, 25, 48*4);
    cancel->position(SBSTART+48, (48*12)+24);
    cancel->action = std::bind(backBtn);
    this->elements.push_back(cancel);

    ImageElement *robot = new ImageElement(ROMFS "./res/robot.png", false, false);
    robot->position((SBSTART+(48*3)+24)-64, 7*48);
    robot->resize(128, 128);
    this->elements.push_back(robot);
}

bool MakerSubmit::process(InputEvents* event)
{
    if (event->pressed(B_BUTTON))
	{
		backBtn();
		return true;
	}
    if (event->pressed(A_BUTTON))
    {
        this->cSubmit();
        return true;
    }

    return super::process(event);
}

void MakerSubmit::render(Element* parent)
{
    if (this->renderer == NULL)
		this->renderer = parent->renderer;
	if (this->parent == NULL)
		this->parent = parent;

    SDL_Rect mbg = {0, 0, SBSTART, 720};
    SDL_SetRenderDrawColor(parent->renderer, 0xF8, 0xB9, 0x00, 0xff);
	SDL_RenderFillRect(parent->renderer, &mbg);

	SDL_SetRenderDrawColor(parent->renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    
    SDL_Rect dimensi = {SBSTART, 0, 48, 48};
    for (int i = SBSTART; i <= 1280; i+=48)
    {
        dimensi.x = i;
        for (int y = 0; y <= 720; y+=48)
        {
            dimensi.y = y;
            SDL_RenderCopy(this->renderer, this->grid, NULL, &dimensi);
        }
    }
	// draw all elements
	super::render(this);
}

void ScrollContent::render(Element* parent)
{
	if (this->parent == NULL)
		this->parent = parent;

	this->renderer = parent->renderer;

	super::render(this);
}

bool ScrollContent::process(InputEvents* event)
{
	int SPEED = 60;
	bool ret = false;

	// handle up and down for the scroll view
	if (event->isKeyDown())
	{
		// scroll the view
		this->y += (SPEED * event->held(UP_BUTTON) - SPEED * event->held(DOWN_BUTTON));
		if (this->y > 0)
			this->y = 0;
		ret |= event->held(UP_BUTTON) || event->held(DOWN_BUTTON);
	}

	return ret || ListElement::process(event);
}