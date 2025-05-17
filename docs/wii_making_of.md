# Making of HBAS/Wii, of sorts, idk, maybe a blog?

## The beginning
- Somebody suggests "new platforms!"
- Big issue: Wii/3DS/PS2 all need SDL1, Chesto is SDL2 (check timeline vs. chestotesto here)
- CompuCat writes ChestoTesto as a minimal Chesto test app (https://discordapp.com/channels/339118412414582786/601836954874544148/615718137777881088)
- Months pass
- vgmoose overnight-speedcodes an almost-finished SDL1 port then passes out, exhausted (https://discordapp.com/channels/339118412414582786/601836954874544148/665745535344443402)
- The next day, CompuCat finishes the port, then speedcodes enough to get ChestoTesto working on Wii...then passes out, exhausted, unable to post anything until the following morning (https://discordapp.com/channels/339118412414582786/601836954874544148/668567358046076947)
- Time passes
- CompuCat starts talking with OpenShopChannel to use their repositories
- Lyfe and crc32 writes some brilliant python-scriptery to migrate the repos over
- More time passes
- CrafterPika takes charge of doing a vWii repository
- CompuCat finally fixes resinfs in ChestoTesto
- CompuCat starts abstracting some SDL2 calls in HBAS...then forgets about the branch...then vgmoose accidentally redoes it all from scratch in master for 3DS! Whoops
- Wii build is halted by a search for cURL - dkP doesn't have a portlib
- vgmoose finds WiiLauncher by conanac which has a mystery binary
- CompuCat does more digging (copypasta from discord logs)
  - WiiLauncher by conanac uses libcurl from Mysterious Source™️, as you found (easier git source: https://github.com/conanac/wiilauncher)
  - After trawling gbatemp, CompuCat found this (https://www.youtube.com/watch?v=6IPMo7tGUGc) YT video, where conanac mentioned grabbing libcurl from "WiiBrowser"

## A short time later
- rw creates several very important ports for getting modern libcurl and mbedtls working on Wii:
  - pacman packages page: https://gitlab.com/4TU/wii-packages
  - libwiisocket port: https://gitlab.com/4TU/libwiisocket
- This was the last "technical" hurdle to making network requests!
- The Wii port itself is still plauged by SDL1 making development difficult
  - Long developer hiatuses as well, with a focus on WiiU/Switch support as needed

## Sometime in between
- Gary helps get SSL working directly on WiiU, which allows hb-appstore to provide its own cert store
- hb-appstore and chesto source code gets a little cleaner (A little)
  - better cross platform code (fewer ifdefs) and resolution/font support in SDL2
- better wiimote controller support planned on wiiu

## A long time later
- Support for the new OpenShopChannel API is added directly into libget
  - initially intended for adding vWii support to the WiiU port
  - changes were being planned to support multiple repos in the UI
- Suddenly, in 2024, light at the end of the tunnel!
  - Developer mardy posts a port of sdl2 to wii! https://gbatemp.net/threads/sdl-2-port-ongoing.645576/
  - This is so good and highly desired!
- vgmoose and Whovian get wii curl packages to build: https://gitlab.com/4TU/wii-packages
  - After these packages are installed, `make wii` in hb-appstore works without `-DNETWORK_MOCK`
  - SD card writing not working, but networking is!
- CompuCat, pwsincd, vgmoose, and others back in active discussion in dusted off discord channels regarding layout changes!

## Little bit later still
- Fancy2209 enters with widescreen patches and other Wii-specific fixes!
  - Downloading packages now actually works! And they install!
- More changes to the UI discussed, Gabubu and vgmoose make mockups
  - Glossy/two-toned Wii-style buttons are added, along with Wii remote input refactoring
- OSC branding added with their category icon and colors
