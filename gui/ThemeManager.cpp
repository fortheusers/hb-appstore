#include "ThemeManager.hpp"
#ifdef SWITCH
#include <switch.h>
#endif

namespace HBAS::ThemeManager
{
    void themeManagerInit()
    {
        // Detect if Switch is using dark theme
        #ifdef SWITCH
        setsysInitialize();
        static ColorSetId sysTheme = ColorSetId_Light;
        setsysGetColorSetId(&sysTheme);
        isDarkMode = (sysTheme == ColorSetId_Dark);
        setsysExit();
        #endif
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