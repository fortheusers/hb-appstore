#include "ThemeManager.hpp"
#include <ctime>
#ifdef SWITCH
#include <switch.h>
#endif

namespace HBAS::ThemeManager
{
    void themeManagerInit()
    {
        bool canDetectDarkMode = false;
        
        // Detect if Switch is using dark theme
#ifdef SWITCH
        setsysInitialize();
        static ColorSetId sysTheme = ColorSetId_Light;
        setsysGetColorSetId(&sysTheme);
        isDarkMode = (sysTheme == ColorSetId_Dark);
        setsysExit();
        canDetectDarkMode = true;
#endif

#ifdef __WIIU__
        // TODO: Check if a custom dark theme is being used
#endif

#ifdef __APPLE__
        if (system("defaults read -g AppleInterfaceStyle 2>/dev/null") == 0) {
            isDarkMode = true;
        }
        canDetectDarkMode = true;
#endif

#ifdef _WIN32
        HKEY hKey;
        DWORD dwRegValue, dwRegType, dwRegSize = sizeof(DWORD);
        if (RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize"), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
        {
            if (RegQueryValueEx(hKey, TEXT("AppsUseLightTheme"), NULL, &dwRegType, (LPBYTE)&dwRegValue, &dwRegSize) == ERROR_SUCCESS)
            {
                isDarkMode = !dwRegValue;
            }
            RegCloseKey(hKey);
        }
        canDetectDarkMode = true;
#endif

        if (!canDetectDarkMode) {
            // we can't detect dark mode on this platform, so let's check the time of day
            time_t t = time(NULL);
            struct tm *tm = localtime(&t);
            isDarkMode = (tm->tm_hour < 5 || tm->tm_hour > 20);
        }

        // TODO: check some preference to override the automatic theme (system or time detection)

        // Set colors for dark mode
        if (isDarkMode)
        {
            background = {0x2d, 0x2c, 0x31, 0xff};
            textPrimary = {0xff, 0xff, 0xff, 0xff};
            textSecondary = {0xd0, 0xd0, 0xd0, 0xff};
        }
        // Set colors for light mode
        else
        {
            background = {0xff, 0xff, 0xff, 0xff};
            textPrimary = {0x00, 0x00, 0x00, 0xff};
            textSecondary = {0x50, 0x50, 0x50, 0xff};
        }
    }
}