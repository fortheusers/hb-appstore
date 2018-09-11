## Switch Homebrew App Store ![travis](https://travis-ci.org/vgmoose/appstorenx.svg?branch=master)
A graphical frontend to the [get package manager](https://github.com/vgmoose/get) for downloading and managing homebrew on the Nintendo Switch. This is a successor to the [Wii U Homebrew App Store](https://github.com/vgmoose/hbas).

### Usage
To run this program, a Nintendo Switch with access to the Homebrew Menu is required (currently requires an sd card).
- [1.x-5.x hbmenu instructions](https://gbatemp.net/threads/switch-hacking-101-how-to-launch-the-homebrew-menu-on-4-x-5-x.504012/)
- [older 3.0.0 hbmenu instructions](https://switchbrew.github.io/nx-hbl/) -

A quick summary of how to run it, provided you can get to hbmenu, is also included below. For newer firmwares, see the link to the 4.x-5.x instructions above.

#### Quick summary
- download latest [appstore folder](https://github.com/vgmoose/appstorenx/releases) to `sd:/switch/appstore/`
   - make sure that the `res` folder is next to `appstore.nro` inside of the `appstore` folder
- download latest [hbmenu.nro](https://github.com/switchbrew/nx-hbmenu/releases/latest) to `sd:/hbmenu.nro`
- put the SD card (recommended formatted FAT32) in the Switch, and enter hbmenu (see instructions above)
- run "hb App Store" from within hbmenu
   - when you're done hit home to exit (and album again to go back to hbmenu)

### Building with libnx
1. Install [dkp-pacman](https://devkitpro.org/viewtopic.php?f=13&t=8702)
3. Install devkitA64 and needed Switch dependencies via dkp-pacman:
```
sudo dkp-pacman -S devkitA64 libnx switch-tools switch-curl switch-bzip2 switch-freetype switch-libjpeg-turbo switch-sdl2 switch-sdl2_gfx switch-sdl2_image switch-sdl2_ttf switch-zlib switch-libpng
```
3. Once it's all setup, recursively clone the repo and run make:
```
git clone --recursive https://github.com/vgmoose/appstorenx.git
cd appstorenx
make
```

If all goes well, `appstore.nro` should be sitting in the current directory.

### Maintaining a repo
See [get's instructions](https://github.com/vgmoose/get#setting-up-repos) for setting up a repository. Everything is designed to be statically hosted. If there's no repo provided in the `repos.json` config file, then it will generate a default one pointing to [switchbru.com/appstore](http://switchbru.com/appstore/).

A new project called [Barkeep](https://github.com/vgmoose/barkeep) is being worked on to allow this to be done without using a simple script.

### Building for PC
There's a separate makefile for building the SDL2 app for PC. It requires libsdl, libcurl, and zlib installed. Below instructions are for Ubuntu, but should be similar on other platforms:
```
sudo apt-get install libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev libsdl2-gfx-dev zlib1g-dev gcc g++ git
git clone --recursive https://github.com/vgmoose/appstorenx.git
cd appstorenx
make -f Makefile.pc
```

### License
This software is licensed under the GPLv3.
