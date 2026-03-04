#if defined(SWITCH)
#include <SDL2/SDL_mixer.h>
#include <switch.h>
#endif
#if defined(WII)
#include <ogc/conf.h>
#endif
#include "../libs/chesto/src/Constraint.hpp"
#include "../libs/get/src/Get.hpp"
#include "../libs/get/src/Utils.hpp"
#include "../libs/get/src/repos/GetRepo.hpp"
#include <filesystem>
#include <unordered_set>

#include "MainDisplay.hpp"
#include "ThemeManager.hpp"
#include "main.hpp"

using namespace std::string_literals; // for ""s

MainDisplay::MainDisplay()
	: RootDisplay()
{
	// add in the sidebar, footer, and main app listing
	sidebar = createNode<Sidebar>();
	appList = createNode<AppList>(nullptr, sidebar);
	sidebar->appList = appList;

	needsRedraw = true;

	updateSidebarColor();

#if defined(WII)
	if (CONF_GetAspectRatio() == CONF_ASPECT_16_9)
		setScreenResolution(854, 480);
#endif
	// use HD resolution for hb-appstore
	// setScreenResolution(1920, 1080);
	// setScreenResolution(3840, 2160); // 4k
}

void MainDisplay::updateSidebarColor()
{
	// set the background color (used as sidebar color)
	auto color = HBAS::ThemeManager::sidebarColor;
	backgroundColor = fromRGB(color.r, color.g, color.b);
}

void MainDisplay::rebuildUI()
{
	// rebuild the UI (for theme changes)
	appList->rebuildUI();
	// sidebar.rebuildUI();
	updateSidebarColor();
	needsRedraw = true;
}

void MainDisplay::setupMusic()
{
	// initialize music (only if MUSIC defined)
	this->initMusic();

#ifdef MUSIC
	// load the music state from a config file
	this->startMusic();

	bool allowSound = getDefaultAudioStateForPlatform();

	if (std::filesystem::exists(SOUND_PATH))
	{
		// invert our sound allowing setting, due to the existence of this file
		allowSound = !allowSound;
	}

	if (!allowSound)
	{
		// muted, so pause the music that we started earlier
		Mix_PauseMusic();
	}

	// load the sfx noise
	click_sfx = Mix_LoadWAV(RAMFS "res/click.wav");

#endif
}

bool MainDisplay::getDefaultAudioStateForPlatform()
{
#ifdef __WIIU__
	// default to true, only for wiiu
	return true;
#endif
	return false;
}

// plays an sfx interface-moving-noise, if sound isn't muted
void MainDisplay::playSFX()
{
#ifdef MUSIC
	if (this->music && !Mix_PausedMusic())
	{
		Mix_PlayChannel(-1, this->click_sfx, 0);
	}
#endif
}

MainDisplay::~MainDisplay()
{
}

void MainDisplay::beginInitialLoad()
{
	networking_callback = nullptr;

	if (spinner)
	{
		// remove spinner
		super::remove(spinner);
		spinner = nullptr;
	}

	// set get instance to our applist
	appList->get = get.get();
	appList->update();
	appList->sidebar->addHints();
}

void MainDisplay::updateGetLocale()
{
	localePackages.clear();

	if (TextElement::curLang != "en-us")
	{
		// create a lightweight get instance for the locale repo
		auto localUrl = META_REPO_1 "/locales/" + TextElement::curLang;
		GetRepo localeRepo("locale", localUrl, true);
		auto localeList = localeRepo.loadPackages();
		for (auto& pkg : localeList)
		{
			localePackages[pkg->getPackageName()] = std::move(pkg);
		}
	}
}

bool MainDisplay::checkMetaRepoForUpdates(Get* get)
{
	// download the metarepo (+1 network call)
	std::string data("");
	bool success = downloadFileToMemory(META_REPO_1 "/index.json", &data);

	if (!success)
	{
		// couldn't download the metarepo, so just return
		// TODO: surface some error notification to the user
		std::cout << "couldn't download metarepo" << std::endl;
		return false;
	}

	// parse the metarepo
	rapidjson::Document d;
	d.Parse(data.c_str());

	// check for parse success
	if (d.HasParseError())
	{
		// couldn't parse metarepo
		std::cout << "couldn't parse metarepo" << std::endl;
		return false;
	}

	// the repos that we're interested in, which is based on our platform
	std::vector<std::string> platformsToCheck;
	// TODO: Use a RepoManager to get which platform types are enabled
#if defined(__WIIU__) || defined(PC)
	platformsToCheck.push_back("wiiu"); // TOOD: also use vwii, if enabled
#endif
#if defined(SWITCH) || defined(PC)
	platformsToCheck.push_back("switch");
#endif
#if defined(WII) || defined(WII_MOCK)
	platformsToCheck.push_back("wii"); // osc
#endif
#if defined(_3DS) || defined(_3DS_MOCK)
	platformsToCheck.push_back("3ds"); // uu
#endif

	// set of repos to remove (exclude)
	std::unordered_set<std::string> reposToRemove;

	// set of repos to add (include)
	std::unordered_map<std::string, std::string> reposToAdd;

	// grab the "suggestions" key
	if (d.HasMember("suggestions"))
	{
		// check the repo platforms that we're interested in
		for (auto& platform : platformsToCheck)
		{
			if (d["suggestions"].HasMember(platform.c_str()))
			{
				// operations for this platform
				auto& ops = d["suggestions"][platform.c_str()];

				// iterate through the operations
				for (auto& op : ops.GetArray())
				{
					if (!op.HasMember("op")) continue;
					if (!op.HasMember("url")) continue;

					std::string opName = op["op"].GetString();
					std::string repoUrl = op["url"].GetString();

					if ("remove" == opName)
					{
						// remove this repo
						reposToRemove.insert(repoUrl);
					}
					else if ("add" == opName)
					{
						// check/get the type
						auto repoType = "get"; // default to get
						if (op.HasMember("type"))
						{
							repoType = op["type"].GetString();
						}
						// add this repo
						reposToAdd[repoUrl] = repoType;
					}
				}
			}
		}
	}

	get->addAndRemoveReposByURL(reposToAdd, reposToRemove);
	return true;
}

void MainDisplay::render(Element* parent)
{
	if (showingSplash)
		renderedSplash = true;

	renderBackground(true);
	RootDisplay::render(parent);
}

bool MainDisplay::process(InputEvents* event)
{
	if (!hasScreens() && showingSplash && renderedSplash && event->noop)
	{
		showingSplash = false;

		// initial loading spinner
		auto spinnerPath = RAMFS "res/spinner.png";
#ifdef SWITCH
		// switch gets a red spinner
		spinnerPath = RAMFS "res/spinner_red.png";
#endif

		if (isEarthDay())
		{
			backgroundColor = fromRGB(12, 156, 91);
			spinnerPath = RAMFS "res/spinner_green.png";
		}

		spinner = createNode<ImageElement>(spinnerPath);
		spinner->resize(90, 90);
		spinner->constrain(ALIGN_TOP, 90)->constrain(ALIGN_CENTER_HORIZONTAL, 0)->constrain(OFFSET_LEFT, 45);

#if defined(_3DS) || defined(_3DS_MOCK)
		spinner->resize(40, 40);
		spinner->position(SCREEN_WIDTH / 2 - spinner->width / 2, 70);
#endif

		networking_callback = MainDisplay::updateLoader;

		// fetch repositories metadata
#if defined(WII)
		// default the repo type to OSC for wii
		get = std::make_unique<Get>(DEFAULT_GET_HOME, DEFAULT_REPO, false, "osc");
#else
		get = std::make_unique<Get>(DEFAULT_GET_HOME, DEFAULT_REPO, false);
#endif

		// update active repos according to the metarepo
		bool isOnline = checkMetaRepoForUpdates(get.get());

		// actually download the repos
		get->update();

		// update our get locale (will do a network call, if in a non-english language)
		updateGetLocale();

		// go through all repos and if one has an error, set the error flag
		for (auto repo : get->getRepos())
		{
			error = error || !repo->isLoaded();
			atLeastOneEnabled = atLeastOneEnabled || repo->isEnabled();
		}

		if (!isOnline)
		{
			std::string connTestMsg = replaceAll(i18n("errors.conntest"), "PLATFORM", PLATFORM);
			RootDisplay::pushScreen(std::make_unique<ErrorScreen>(i18n("errors.nowifi"), connTestMsg + "\n" + i18n("errors.dnsmsg") + " " + META_REPO_1));
			return true;
		}

		if (!atLeastOneEnabled)
		{
			RootDisplay::pushScreen(std::make_unique<ErrorScreen>(i18n("errors.noserver"), i18n("errors.norepos") + "\n" + i18n("errors.onepkg")));
			return true;
		}

		// sd card write test, try to open a file on the sd root
		std::string tmp_dir = get->mTmp_path;
		std::string tmp_file = tmp_dir + "write_test.txt";

		bool writeFailed = false;
		std::string magic = "Whosoever holds this hammer, if they be worthy, shall possess the power of Thor.";

		// try to write to the file (no append)
		std::ofstream file(tmp_file);
		if (file.is_open())
		{
			file << magic;
			file.close();
		}
		else
			writeFailed = true;

		// try to read from the file
		std::ifstream read_file(tmp_file);
		if (!writeFailed && read_file.is_open())
		{
			std::string line;
			std::getline(read_file, line);
			read_file.close();

			if (line != magic) writeFailed = true;

			// delete the file
			std::remove(tmp_file.c_str());
		}
		else
			writeFailed = true;

		if (writeFailed)
		{
			std::string cardText = replaceAll(i18n("errors.writetestfail"), "PATH", tmp_file) + "\n";
#if defined(__WIIU__)
			cardText = i18n("errors.sdlock") + "\n"s + cardText;
#elif defined(SWITCH)
			cardText = i18n("errors.exfat") + "\n"s + cardText;
#endif

			RootDisplay::pushScreen(std::make_unique<ErrorScreen>(i18n("errors.sdaccess"), cardText));
			return true;
		}

		beginInitialLoad();

		return true;
	}

	// if we need a redraw, also update the app list (for resizing events)
	// TODO: have a more generalized way to have a view describe what needs redrawing
	if (needsRedraw)
		appList->update();

	return RootDisplay::process(event);
}

int MainDisplay::updateLoader(void*, double progress)
{
	int now = CST_GetTicks();
	int diff = now - AppDetails::lastFrameTime;

	// don't update the GUI too frequently here, it slows down downloading
	// (never return early if it's 100% done)
	if (diff < 32 && progress != 1.0)
		return 0;

	MainDisplay* display = (MainDisplay*)RootDisplay::mainDisplay;
	if (display->spinner)
		display->spinner->angle += 10;
	display->render(NULL);

	AppDetails::lastFrameTime = CST_GetTicks();

	return 0;
}

ErrorScreen::ErrorScreen(std::string mainErrorText, std::string troubleshootingText)
	: mainErrorText(std::move(mainErrorText)), troubleshootText(std::move(troubleshootingText))
{
	rebuildUI();
}

void ErrorScreen::rebuildUI()
{
	removeAll();

	Container* logoCon = createNode<Container>(ROW_LAYOUT, 10);

	auto iconPtr = std::make_unique<ImageElement>(LOGO_PATH);
	auto* icon = iconPtr.get();
	icon->resize(35, 35);
	logoCon->add(std::move(iconPtr));

	auto titlePtr = std::make_unique<TextElement>(i18n("credits.title"), 50 - 25);

#if defined(USE_OSC_BRANDING)
	// make the icon larger
	auto* title = titlePtr.get();
	title->setText("HBAS + OSC Wii");
	title->update();
	icon->setScaleMode(SCALE_PROPORTIONAL_NO_BG);
	icon->resize(80, 80);
#endif

	logoCon->add(std::move(titlePtr));

	// constraints
	logoCon->constrain(ALIGN_TOP | ALIGN_CENTER_HORIZONTAL, 25);

	auto errorMsg = createNode<TextElement>(mainErrorText.c_str(), 40);
	errorMsg->constrain(ALIGN_CENTER_BOTH);

	auto troubleshoot = createNode<TextElement>((std::string(i18n("errors.troubleshooting") + "\n") + troubleshootText).c_str(), 20, nullptr, false, 600);
	troubleshoot->constrain(ALIGN_BOTTOM | ALIGN_CENTER_HORIZONTAL, 40);

	auto btnQuit = createNode<Button>(i18n("listing.quit"), SELECT_BUTTON, false, 15);
	btnQuit->constrain(ALIGN_LEFT | ALIGN_BOTTOM, 100);
	btnQuit->action = []()
	{
		RootDisplay::mainDisplay->requestQuit();
	};

	auto ignoreBtn = createNode<Button>(i18n("errors.ignorethis"), X_BUTTON, false, 15);
	ignoreBtn->constrain(ALIGN_RIGHT | ALIGN_BOTTOM, 100);
	ignoreBtn->action = []()
	{
		auto mainDisplay = (MainDisplay*)RootDisplay::mainDisplay;
		mainDisplay->get->addLocalRepo();
		mainDisplay->needsRedraw = true;
		mainDisplay->beginInitialLoad();
		RootDisplay::popScreen();
	};
}

bool isEarthDay()
{
	time_t now = time(0);
	tm* ltm = localtime(&now);

	return ltm->tm_mon == 3 && ltm->tm_mday == 22;
}

bool MainDisplay::isLowMemoryMode()
{
#if defined(SWITCH)
	AppletType at = appletGetAppletType();
	// in switch applet mode, we're in a low memory device
	return at == AppletType_Application || at == AppletType_SystemApplication;
#endif
	return false;
}

std::string getOSVersion()
{
#if defined(SWITCH)
	auto version = hosversionGet();
	return std::to_string(version);
#elif defined(__WIIU__)
	// TODO: hook up this wut function
	// MCP_GetSystemVersion(int32_t handle, MCPSystemVersion *systemVersion)
#elif defined(_3DS)
	// TODO: Check libctru
#elif defined(WII)
	// TODO: Check libogc
#elif defined(__APPLE__)
	return "macOS";
#elif defined(_WIN32)
	return "Windows";
#endif
	return "Unknown";
}