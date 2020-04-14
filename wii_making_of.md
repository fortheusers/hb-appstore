# Making of HBAS/Wii, of sorts, idk, maybe a blog?

Big parts:
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
  - After trawling gbatemp, I found this (https://www.youtube.com/watch?v=6IPMo7tGUGc) YT video, where conanac mentioned grabbing libcurl from "WiiBrowser"
  - Google led me to a WiiBrew page (https://wiibrew.org/wiki/WiiBrowser) and a website (http://wiibrowser.altervista.org/mainsite/about.html), both with source. Apparently, this was a browser developed by gave92
  - Googling that revealed that gave92 is Marco Gavelli (https://www.linkedin.com/in/marco-gavelli/), an engineer at Maserati of all places
