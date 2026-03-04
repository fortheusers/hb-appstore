#include "MainDisplay.hpp"
#include "../libs/chesto/src/TextElement.hpp"
#include "../libs/chesto/src/Container.hpp"
#include "../libs/chesto/src/Button.hpp"
#include "../libs/chesto/src/Constraint.hpp"
#include "SettingsScreen.hpp"
#include "ThemeManager.hpp"
#include "AboutScreen.hpp"
#include "FeedbackCenter.hpp"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "main.hpp"

#include <vector>
#include <utility>
#include <fstream>
#include <sstream>

// put it in the .get folder
#define CONFIG_PATH DEFAULT_GET_HOME "settings.json"

void appendRow(Container* parent, std::unique_ptr<Element> left, std::unique_ptr<Element> right) {
    // TODO: is the double std::move fine here? (these were already moved right before calling)
    auto row = std::make_unique<Container>(ROW_LAYOUT, 20);
    row->add(std::move(left));
    row->add(std::move(right));
    parent->add(std::move(row));
}

std::string SettingsScreen::getSettingsPath() {
    return CONFIG_PATH;
}

void SettingsScreen::saveSettings() {
    using namespace rapidjson;
    
    // safeguard: don't save empty strings, store defaults instead
    if (currentTheme.empty()) currentTheme = "auto";
    if (currentLanguage.empty()) currentLanguage = "en-us";
    if (currentAccentColor.empty()) currentAccentColor = "darkgray";
    
    Document doc;
    doc.SetObject();
    Document::AllocatorType& allocator = doc.GetAllocator();
    
    // Save current settings
    Value themeVal;
    themeVal.SetString(currentTheme.c_str(), currentTheme.length(), allocator);
    doc.AddMember("theme", themeVal, allocator);
    
    Value resVal;
    resVal.SetString(currentResolution.c_str(), currentResolution.length(), allocator);
    doc.AddMember("resolution", resVal, allocator);
    
    Value langVal;
    langVal.SetString(currentLanguage.c_str(), currentLanguage.length(), allocator);
    doc.AddMember("language", langVal, allocator);
    
    Value accentVal;
    accentVal.SetString(currentAccentColor.c_str(), currentAccentColor.length(), allocator);
    doc.AddMember("accentColor", accentVal, allocator);
    
    // write to output file
    StringBuffer buffer;
    PrettyWriter<StringBuffer> writer(buffer);
    doc.Accept(writer);
    
    std::ofstream ofs(getSettingsPath());
    if (ofs.is_open()) {
        ofs << buffer.GetString();
        ofs.close();
        printf("Settings saved to %s\n", getSettingsPath().c_str());
    } else {
        printf("Failed to save settings to %s\n", getSettingsPath().c_str());
    }

    // now reload settings (all setting changes go through disk first)
    loadSettings();
}

void SettingsScreen::loadSettings() {
    using namespace rapidjson;
    
    // defaults
    currentTheme = "auto";
    currentResolution = std::to_string(SCREEN_HEIGHT) + "p";
    currentLanguage = "en-us";
    currentAccentColor = "darkgray";
    
    // try to load from file
    std::ifstream ifs(getSettingsPath());
    if (!ifs.is_open()) {
        printf("No settings file found at %s, using defaults\n", getSettingsPath().c_str());
        return;
    }
    
    std::stringstream buffer;
    buffer << ifs.rdbuf();
    std::string content = buffer.str();
    ifs.close();
    
    Document doc;
    doc.Parse(content.c_str());
    
    if (doc.HasParseError()) {
        printf("Error parsing settings file, using defaults\n");
        return;
    }
    
    // Load settings
    if (doc.HasMember("theme") && doc["theme"].IsString()) {
        std::string loadedTheme = doc["theme"].GetString();
        if (!loadedTheme.empty()) {
            currentTheme = loadedTheme;
        }
    }
    
    if (doc.HasMember("resolution") && doc["resolution"].IsString()) {
        currentResolution = doc["resolution"].GetString();
    }
    
    if (doc.HasMember("language") && doc["language"].IsString()) {
        std::string loadedLang = doc["language"].GetString();
        if (!loadedLang.empty()) {
            currentLanguage = loadedLang;
        }
    }
    
    if (doc.HasMember("accentColor") && doc["accentColor"].IsString()) {
        std::string loadedAccent = doc["accentColor"].GetString();
        if (!loadedAccent.empty()) {
            currentAccentColor = loadedAccent;
        }
    }
    
    printf("Settings loaded from %s\n", getSettingsPath().c_str());
    
    // Apply loaded settings
    if (currentTheme == "auto") {
        HBAS::ThemeManager::preferredTheme = THEME_AUTO;
    } else if (currentTheme == "light") {
        HBAS::ThemeManager::preferredTheme = THEME_LIGHT;
    } else if (currentTheme == "dark") {
        HBAS::ThemeManager::preferredTheme = THEME_DARK;
    }
    
    // Apply accent color
    if (currentAccentColor == "red")
        HBAS::ThemeManager::setAccentColor(fromRGB(255, 0, 0));
    else if (currentAccentColor == "orange")
        HBAS::ThemeManager::setAccentColor(fromRGB(255, 165, 0));
    else if (currentAccentColor == "yellow")
        HBAS::ThemeManager::setAccentColor(fromRGB(255, 255, 0));
    else if (currentAccentColor == "green")
        HBAS::ThemeManager::setAccentColor(fromRGB(0, 255, 0));
    else if (currentAccentColor == "blue")
        HBAS::ThemeManager::setAccentColor(fromRGB(0, 0, 255));
    else if (currentAccentColor == "purple")
        HBAS::ThemeManager::setAccentColor(fromRGB(128, 0, 128));
    else if (currentAccentColor == "pink")
        HBAS::ThemeManager::setAccentColor(fromRGB(255, 192, 203));
    else if (currentAccentColor == "lightgray")
        HBAS::ThemeManager::setAccentColor(fromRGB(200, 200, 200));
    else if (currentAccentColor == "darkgray")
        HBAS::ThemeManager::setAccentColor(fromRGB(30, 30, 30));
}

void SettingsScreen::loadSettingsStatic() {
    using namespace rapidjson;
    
    // Try to load from file
    std::ifstream ifs(CONFIG_PATH);
    if (!ifs.is_open()) {
        printf("No settings file found at %s, using defaults\n", CONFIG_PATH);
        return;
    }
    
    std::stringstream buffer;
    buffer << ifs.rdbuf();
    std::string content = buffer.str();
    ifs.close();
    
    Document doc;
    doc.Parse(content.c_str());
    
    if (doc.HasParseError()) {
        printf("Error parsing settings file, using defaults\n");
        return;
    }
    
    // Load and apply theme
    if (doc.HasMember("theme") && doc["theme"].IsString()) {
        std::string theme = doc["theme"].GetString();
        if (!theme.empty()) {
            if (theme == "auto") {
                HBAS::ThemeManager::preferredTheme = THEME_AUTO;
            } else if (theme == "light") {
                HBAS::ThemeManager::preferredTheme = THEME_LIGHT;
            } else if (theme == "dark") {
                HBAS::ThemeManager::preferredTheme = THEME_DARK;
            }
        }
    }
    
    if (doc.HasMember("language") && doc["language"].IsString()) {
        std::string language = doc["language"].GetString();
        if (!language.empty()) {
            TextElement::loadI18nCache(language);
        }
    }
    
    if (doc.HasMember("accentColor") && doc["accentColor"].IsString()) {
        std::string accent = doc["accentColor"].GetString();
        if (!accent.empty()) {
            if (accent == "red")
                HBAS::ThemeManager::setAccentColor(fromRGB(255, 0, 0));
            else if (accent == "orange")
                HBAS::ThemeManager::setAccentColor(fromRGB(255, 165, 0));
            else if (accent == "yellow")
                HBAS::ThemeManager::setAccentColor(fromRGB(255, 255, 0));
            else if (accent == "green")
                HBAS::ThemeManager::setAccentColor(fromRGB(0, 255, 0));
            else if (accent == "blue")
                HBAS::ThemeManager::setAccentColor(fromRGB(0, 0, 255));
            else if (accent == "purple")
                HBAS::ThemeManager::setAccentColor(fromRGB(128, 0, 128));
            else if (accent == "pink")
                HBAS::ThemeManager::setAccentColor(fromRGB(255, 192, 203));
            else if (accent == "lightgray")
                HBAS::ThemeManager::setAccentColor(fromRGB(200, 200, 200));
            else if (accent == "darkgray")
                HBAS::ThemeManager::setAccentColor(fromRGB(30, 30, 30));
        }
    }
    
    printf("Settings loaded and applied from %s\n", CONFIG_PATH);
}

SettingsScreen::SettingsScreen()
{
    loadSettings();
    rebuildUI();
}

void SettingsScreen::rebuildUI() {
    removeAll();

    // Main settings container - centered vertically and horizontally
    auto rows = createNode<Container>(COL_LAYOUT, 40);

    width = SCREEN_WIDTH; // pane is a fourth of the screen width
    height = SCREEN_HEIGHT;

    // Back button in the upper left corner
    auto backButton = std::make_unique<Button>(i18n("settings.back"), B_BUTTON, true, 20);
    backButton->setAction([]() {
        RootDisplay::popScreen();
    });
    backButton->constrain(ALIGN_TOP | ALIGN_CENTER_HORIZONTAL, 20);
    rows->add(std::move(backButton));

    // Header centered at top
    auto header = std::make_unique<TextElement>(i18n("settings.title"), 30, &HBAS::ThemeManager::textSecondary);
    header->position(0, 20);
    header->constrain(ALIGN_CENTER_HORIZONTAL, 0);
    rows->add(std::move(header));

    // Theme selection
    auto themeLabel = std::make_unique<TextElement>(i18n("settings.theme.title"), 24, &HBAS::ThemeManager::textPrimary);
    auto choices = std::vector<std::pair<std::string, std::string>> {
        { "auto", i18n("settings.auto") },
        { "light", i18n("settings.theme.light") },
        { "dark", i18n("settings.theme.dark") }
    };

    auto themeDropDown = std::make_unique<DropDown>(R_BUTTON, choices, [this](std::string choice) {
        currentTheme = choice;
        saveSettings();
        loadSettings();
        HBAS::ThemeManager::themeManagerInit();
        Texture::wipeTextCache();

        if (auto* mainDisplay = dynamic_cast<MainDisplay*>(RootDisplay::mainDisplay)) {
            mainDisplay->rebuildUI();
        }
        needsRebuild = true;
    }, 20, currentTheme, HBAS::ThemeManager::isDarkMode);
    appendRow(rows, std::move(themeLabel), std::move(themeDropDown));

    // Resolution selection
    auto resLabel = std::make_unique<TextElement>(i18n("settings.res.title"), 24, &HBAS::ThemeManager::textPrimary);
    auto resChoices = std::vector<std::pair<std::string, std::string>> {
        { "480p", i18n("settings.res.480p") },
        { "720p", i18n("settings.res.720p") },
        { "1080p", i18n("settings.res.1080p") },
    };

    auto resDropDown = std::make_unique<DropDown>(X_BUTTON, resChoices, [this](std::string choice) {
        currentResolution = choice;
        saveSettings();
        Texture::wipeTextCache();
        if (choice == "480p") {
            RootDisplay::mainDisplay->setScreenResolution(854, 480);
            RootDisplay::globalScale = 0.75f;
        } else if (choice == "720p") {
            RootDisplay::mainDisplay->setScreenResolution(1280, 720);
            RootDisplay::globalScale = 1.0f;
        } else if (choice == "1080p") {
            RootDisplay::mainDisplay->setScreenResolution(1920, 1080);
            RootDisplay::globalScale = 1.5f;
        }
        // we don't have to rebuild UI, as setScreenResolution does that for us
    }, 20, currentResolution, isDark);
    appendRow(rows, std::move(resLabel), std::move(resDropDown));

    // Language selection
    auto langLabel = std::make_unique<TextElement>(i18n("settings.language"), 24, &HBAS::ThemeManager::textPrimary);
    auto langDropdown = std::make_unique<DropDown>(L_BUTTON, TextElement::getAvailableLanguages(), [this](std::string choice) {
        currentLanguage = choice;
        saveSettings();

        TextElement::loadI18nCache(choice);
        Texture::wipeTextCache();
        
        RootDisplay::deferAction([this]() {
            if (auto* mainDisplay = dynamic_cast<MainDisplay*>(RootDisplay::mainDisplay)) {
                mainDisplay->updateGetLocale();
                mainDisplay->rebuildUI();
            }
            needsRebuild = true;
        });
    }, 20, currentLanguage, HBAS::ThemeManager::isDarkMode);
    appendRow(rows, std::move(langLabel), std::move(langDropdown));
    
    // Accent color selection
    auto accentLabel = std::make_unique<TextElement>(i18n("settings.accentcolor"), 24, &HBAS::ThemeManager::textPrimary);
    auto accentChoices = std::vector<std::pair<std::string, std::string>> {
        { "red", i18n("settings.accent.red") },
        { "orange", i18n("settings.accent.orange") },
        { "yellow", i18n("settings.accent.yellow") },
        { "green", i18n("settings.accent.green") },
        { "blue", i18n("settings.accent.blue") },
        { "purple", i18n("settings.accent.purple") },
        { "pink", i18n("settings.accent.pink") },
        { "lightgray", i18n("settings.accent.lightgray") },
        { "darkgray", i18n("settings.accent.darkgray") }
    };

    appendRow(rows, std::move(accentLabel), std::make_unique<DropDown>(Y_BUTTON, accentChoices, [this](std::string choice) {
        currentAccentColor = choice;
        saveSettings();
        loadSettings();
        Texture::wipeTextCache();
        if (auto* mainDisplay = dynamic_cast<MainDisplay*>(RootDisplay::mainDisplay)) {
            mainDisplay->rebuildUI();
        }
        needsRebuild = true;
    }, 20, currentAccentColor, HBAS::ThemeManager::isDarkMode));

    // // button color selection
    // auto buttonColorLabel = std::make_unique<TextElement>(i18n("settings.buttoncolor"), 24, &HBAS::ThemeManager::textPrimary);
    // auto buttonColorChoices = std::vector<std::pair<std::string, std::string>> {
    //     { "red", i18n("settings.accent.red") },
    //     { "orange", i18n("settings.accent.orange") },
    //     { "yellow", i18n("settings.accent.yellow") },
    //     { "green", i18n("settings.accent.green") },
    //     { "blue", i18n("settings.accent.blue") },
    //     { "purple", i18n("settings.accent.purple") },
    //     { "pink", i18n("settings.accent.pink") },
    //     { "lightgray", i18n("settings.accent.lightgray") },
    //     { "darkgray", i18n("settings.accent.darkgray") }
    // };
    // appendRow(rows, std::move(buttonColorLabel), std::make_unique<DropDown>(ZL_BUTTON, buttonColorChoices, [](std::string) {
    //     // Dropdown callback unused in this context
    // }, 20, "blue", HBAS::ThemeManager::isDarkMode));


    // Feedback button at bottom center
    auto feedbackCenter = std::make_unique<Button>(i18n("listing.feedbackcenter"), 0, HBAS::ThemeManager::isDarkMode, 20);
    feedbackCenter->setAction([]() {
        RootDisplay::pushScreen(std::make_unique<FeedbackCenter>());
    });
    feedbackCenter->constrain(ALIGN_CENTER_HORIZONTAL)->constrain(ALIGN_BOTTOM, 80);
    rows->add(std::move(feedbackCenter));

    // Credits button at bottom center (above feedback)
    auto credits = std::make_unique<Button>(i18n("listing.credits"), 0, HBAS::ThemeManager::isDarkMode, 20);
    credits->setAction([]() {
        RootDisplay::pushScreen(std::make_unique<AboutScreen>(((MainDisplay*)RootDisplay::mainDisplay)->get.get()));
    });
    credits->constrain(ALIGN_BOTTOM, 20)->constrain(ALIGN_CENTER_HORIZONTAL);
    rows->add(std::move(credits));

    // right align
    rows->height = SCREEN_HEIGHT;
    auto dest = 10; // 10 padding from the right
    // if we already transitioned in, don't redo it
    if (this->transitionAmt < 0x99) {
        rows->xAbs = rows->x = SCREEN_WIDTH*10;
        rows->animate(250, [rows, dest, this](float progress) {
            // std::cout << "Progress: " << progress << std::endl;
            rows->constraints.clear();
            rows->constrain(ALIGN_RIGHT, dest - (rows->width * (1 - progress)));
            // unless we've already faded in
            this->transitionAmt = static_cast<int>(0x99 * progress);
        }, [rows, dest, this]() {
            rows->constraints.clear();
            rows->constrain(ALIGN_RIGHT, dest);
            this->transitionAmt = 0x99;
        });
    } else {
        // just set the final constraint
        rows->constraints.clear();
        rows->constrain(ALIGN_RIGHT, dest);
    }
}

SettingsScreen::~SettingsScreen()
{
}

bool SettingsScreen::process(InputEvents* event)
{
    // TODO: check if needsRebuild is needed for this
    if (needsRebuild) {
        needsRebuild = false;
        rebuildUI();
        return true;
    }
    
    return Screen::process(event);
}

void SettingsScreen::render(Element* parent)
{
	if (this->parent == NULL)
		this->parent = parent;

    // draw a full screen dimmer background
    // TODO: this is probably literally the fourth place we do this, and we do it a different way every time
    CST_Rect fullDimens = { 0, 0, RootDisplay::screenWidth, RootDisplay::screenHeight };
    CST_SetDrawBlend(RootDisplay::renderer, true);
    CST_SetDrawColorRGBA(RootDisplay::renderer, 0, 0, 0, transitionAmt);
	CST_FillRect(RootDisplay::renderer, &fullDimens);
    CST_SetDrawBlend(RootDisplay::renderer, false);

    // draw the header bg, only over where the pane is
    auto row_x = elements[0]->xAbs;
    CST_Rect headerDimens = { row_x, 0, width, 85 };
    auto accentColor = RootDisplay::mainDisplay->backgroundColor;
    CST_Color headerColor = {
        (Uint8)(accentColor.r * 0xff),
        (Uint8)(accentColor.g * 0xff),
        (Uint8)(accentColor.b * 0xff),
        0xff
    };
    CST_SetDrawColor(RootDisplay::renderer, headerColor);
    CST_FillRect(RootDisplay::renderer, &headerDimens);

	// draw a white background, width of the screen
	CST_Rect dimens = { row_x, 85, width, height };

	CST_SetDrawColor(RootDisplay::renderer, HBAS::ThemeManager::background);
	CST_FillRect(RootDisplay::renderer, &dimens);

    // draw the secondary color along the top
    super::render(parent);
}