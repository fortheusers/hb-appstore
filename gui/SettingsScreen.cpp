#include "../libs/chesto/src/RootDisplay.hpp"
#include "../libs/chesto/src/TextElement.hpp"
#include "../libs/chesto/src/Container.hpp"
#include "../libs/chesto/src/DropDown.hpp"
#include "SettingsScreen.hpp"
#include "ThemeManager.hpp"
#include <map>

SettingsScreen::SettingsScreen()
{
    auto rows = new Container(ROW_LAYOUT, 10);
    // main header
    // rows->add(new TextElement(i18n("settings.title"), 30, HBAS::ThemeManager::textPrimary));

    // theme selection (map of key -> value)
    auto choices = std::map<std::string, std::string> {
        { "settings.auto", "auto" },
        { "settings.theme.light", "light" },
        { "settings.theme.dark", "dark" }
    };
    auto themeDropDown = new DropDown(choices, [](std::string choice) {
        printf("Selected theme: %s\n", choice.c_str());
    });
    rows->child(themeDropDown);
    this->child(rows);

    // rows->add(new TextElement(i18n("settings.theme.title"), 24, HBAS::ThemeManager::textPrimary));
    // rows->add((new Container(COL_LAYOUT, 10))->child(
    //     new SettingsButton(i18n("settings.auto"), 20, HBAS::ThemeManager::textPrimary)->action([]() {
    //         // HBAS::ThemeManager::setTheme(HBAS::ThemeManager::autoTheme);
    //     })->child(
    //     new SettingsButton(i18n("settings.theme.light"), 20, HBAS::ThemeManager::textPrimary)->action([]() {
    //         // HBAS::ThemeManager::setTheme(HBAS::ThemeManager::lightTheme);
    //     }))->child(
    //     new SettingsButton(i18n("settings.theme.dark"), 20, HBAS::ThemeManager::textPrimary)->action([]() {
    //         // HBAS::ThemeManager::setTheme(HBAS::ThemeManager::darkTheme);
    //     }))
    // ));

    // // resolution selection
    // rows->add(new TextElement(i18n("settings.res.title")), new TextElement(header, 24, HBAS::ThemeManager::textPrimary));
    // rows->add((new Container(COL_LAYOUT, 10))->child(
    //     new SettingsButton(i18n("settings.auto"), 20, HBAS::ThemeManager::textPrimary)->action([]() {
    //         // HBAS::ThemeManager::setResolution(HBAS::ThemeManager::autoResolution);
    //     })->child(
    //     new SettingsButton(i18n("settings.res.720p"), 20, HBAS::ThemeManager::textPrimary)->action([]() {
    //         // HBAS::ThemeManager::setResolution(HBAS::ThemeManager::res720p);
    //     }))->child(
    //     new SettingsButton(i18n("settings.res.1080p"), 20, HBAS::ThemeManager::textPrimary)->action([]() {
    //         // HBAS::ThemeManager::setResolution(HBAS::ThemeManager::res1080p);
    //     }))
    // ));

    // // language selection
    // rows->add(new TextElement(i18n("settings.lang"), 24, HBAS::ThemeManager::textPrimary));
    // rows->add((new Container(COL_LAYOUT, 10))->child(
    //     (new SettingsButton(i18n("settings.auto"), 20, HBAS::ThemeManager::textPrimary)->action([]() {
    //         // HBAS::ThemeManager::setLanguage(HBAS::ThemeManager::autoLanguage);
    //     }))->child(
    //     (new SettingsButton(i18n("settings.lang.en"), 20, HBAS::ThemeManager::textPrimary)->action([]() {
    //         // HBAS::ThemeManager::setLanguage(HBAS::ThemeManager::enLanguage);
    //     })))->child(
    //     new SettingsButton(i18n("settings.lang.cn"), 20, HBAS::ThemeManager::textPrimary)->action([]() {
    //         // HBAS::ThemeManager::setLanguage(HBAS::ThemeManager::cnLanguage);
    //     }))
    // ));

    // // repo selection
    // rows->add(new TextElement(i18n("settings.repos.title"), 24, HBAS::ThemeManager::textPrimary));
    // rows->add((new Container(ROW_LAYOUT, 10))->child(
    //     new SettingsButton("Add", 20, HBAS::ThemeManager::textPrimary)->action([]() {
    //         // HBAS::RootDisplay::pushScreen(new AddRepoScreen());
    //     })->child(
    //     new SettingsButton("Remove", 20, HBAS::ThemeManager::textPrimary)->action([]() {
    //         // HBAS::RootDisplay::pushScreen(new RemoveRepoScreen());
    //     }))
    // ));

    // // some menu buttons
    // rows->add((new Container(ROW_LAYOUT, 10))->child(
    //     new SettingsButton("Back", 20, HBAS::ThemeManager::textPrimary)->action([]() {
	//         RootDisplay::switchSubscreen(nullptr);
    //     })->child(
    //     new SettingsButton("Feedback Center", 20, HBAS::ThemeManager::textPrimary)->action([]() {
    //         // HBAS::ThemeManager::saveSettings();
    //     }))
    // ));

    
}

SettingsScreen::~SettingsScreen()
{
}

bool SettingsScreen::process(InputEvents* event)
{
    return true;
}

void SettingsScreen::render(Element* parent)
{
    super::render(this);
}