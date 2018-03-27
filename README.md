## Switch App Store
A graphical frontend to the [get package manager](https://github.com/vgmoose/get) for downloading and managing homebrew on the Nintendo Switch. This is a successor to the [Wii U Homebrew App Store](https://github.com/vgmoose/hbas).

### Usage
To run this program, a 3.0.0 firmware Nintendo Switch with access to hbmenu is required (currently requires an sd card).

You should already have setup and ran HBL once before, [according to these instructions](https://switchbrew.github.io/nx-hbl/).

#### Quick summary
- download latest [appstore.nro](https://github.com/vgmoose/appstorenx/releases) to `sd:/switch/appstore/appstore.nro`
- download latest [hbmenu.nro](https://github.com/switchbrew/nx-hbmenu/releases/latest) to `sd:/hbmenu.nro`
- put the SD card (recommended formatted FAT32) in the Switch
- set the DNS server as `104.236.106.125` and try to connect to Internet
   - click "Install" if you've never ran HBL before (only have to do it once, should crash with error `2000-1337`)
   - click "Run" to activate the HBL exploit (will prompt a popup to exit when done)
- go to the home menu, and then go to "Album" to start hbmenu
- run "hb App Store" from within hbmenu
   - when you're done hit home to exit (and album again to go back to hbmenu)

### Building
Setup devkitA64 and libnx [using the installer](http://switchbrew.org/index.php?title=Setting_up_Development_Environment), and then set up SDL1.2 according to the [instructions here](https://gbatemp.net/threads/sdl-1-2-15-for-switch-libnx-based.497412/)* Currently, this project also uses the zlib and freetype portlibs, which can be installed from [dkp-pacman](https://devkitpro.org/viewtopic.php?f=13&t=8702).

Once it's all setup, recursively clone the repo and run make:
```
git clone --recursive https://github.com/vgmoose/appstorenx.git
cd appstorenx
make
```

If all goes well, `appstore.nro` should be sitting in the current directory.

### Maintaining a repo
See [get's instructions](https://github.com/vgmoose/get#setting-up-repos) for setting up a repository. Everything is designed to be statically hosted. If there's no repo provided in the `repos.json` config file, then it will generate a default one pointing to [switchbru.com/appstore](http://switchbru.com/appstore/).

### Building for PC
There's a separate makefile for building the SDL1.2 app for PC. It requires libsdl, libcurl, and zlib installed.
```
make -f Makefile.pc
```

### Building the text-only version
The GUI version of this app is in progress, see [#1](https://github.com/vgmoose/appstorenx/issues/1) for details. In the mean time, editing either of the makefiles to include a `-DNOGUI` argument should build the old text-version that was used in the preview.

### License
This software is licensed under the GPLv3.
