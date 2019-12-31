# Homebrew App Store
[![GPLv3 License](https://img.shields.io/badge/license-GPLv3-blue.svg?style=flat-square)](https://opensource.org/licenses/GPL-3.0) [![PRs Welcome](https://img.shields.io/badge/PRs-welcome!-tomato.svg?style=flat-square)](http://makeapullrequest.com) [![travis](https://img.shields.io/travis/vgmoose/hb-appstore.svg?label=travis&style=flat-square)](https://travis-ci.org/vgmoose/hb-appstore) [![discord](https://img.shields.io/discord/339118412414582786.svg?color=blueviolet&label=discord&style=flat-square)](https://discordapp.com/invite/F2PKpEj) 	[![twitter](https://img.shields.io/twitter/follow/wiiubru.svg?label=twitter&style=flat-square)](https://twitter.com/wiiubru)

A graphical frontend to the [get package manager](https://github.com/vgmoose/libget) for downloading and managing homebrew on video game consoles, such as the Nintendo Switch and Wii U. This is a replacement to the older [Wii U Homebrew App Store](https://github.com/vgmoose/wiiu-hbas).

## Supported Platforms
### Nintendo Switch
To run this program, a Nintendo Switch with access to the Homebrew Menu is required. This can be done on most Switches manufactured before July 2018, for compatibility check your serial number at [ismyswitchpatched.com](https://ismyswitchpatched.com). To run hbmenu, see the tutorial [here](https://guide.sdsetup.com/).

Extract the latest [hb-appstore](https://github.com/vgmoose/hb-appstore/releases) to `sd:/switch/appstore/`, and run "hb App Store" from within hbmenu. When you're done, you can press the Minus (-) button to exit.

### Nintendo Wii U
To run this program, a Wii U with access to the Homebrew Launcher is required. This can be done on any firmware, for compatibility check at [ismywiiupatched.com](https://ismywiiupatched.com). To run the Homebrew Launcher, see the tutorial [here](https://wiiu.hacks.guide).

Extract the latest [hb-appstore](https://github.com/vgmoose/hb-appstore/releases) to `sd:/wiiu/apps/appstore/`, and run "hb App Store" from within Homebrew Launcher. When you're done, you can press the Minus (-) button to exit.

### Web and Desktop
Web browsers can download files from the Homebrew App Store at [apps.fortheusers.org](https://apps.fortheusers.org). Source code: [hbas-frontend](https://gitlab.com/4TU/hbas-frontend)

Desktop platforms (Windows, MacOS, and Linux), can use @LyfeOnEdge's [unofficial-appstore](https://github.com/LyfeOnEdge/unofficial-appstore), or their alternative program: [HBUpdater](https://github.com/LyfeOnEdge/HBUpdater)

## Maintaining a repo
See [get's instructions](https://github.com/vgmoose/get#setting-up-repos) for setting up a repository. Everything is designed to be statically hosted. If there's no repo provided in the `repos.json` config file, then it will generate a default one pointing to [switchbru.com/appstore](http://switchbru.com/appstore/).

If you run into any issues and need help maintaining or setting up a libget repo, feel free to get in touch with vgmoose at me@vgmoose.com or on Discord.

## Compilation instructions
This program is written using SDL2 and has dependencies on libsdl, libcurl, and zlib. It also makes use of libget which is included in this repo as a submodule.

You can get pre-compiled binaries for each platform under [Pipelines](https://gitlab.com/4TU/hb-appstore/pipelines) for a given commit. The download artifacts dropdown is to the right of the build passing status.

### Building with Docker
The easiest way to build is to use a docker container. Docker can be used to build for any of the three supported platforms, using the dependency helper scripts. This is how the pre-compiled binaries are built.

1. Install [Docker](https://www.docker.com)
2. Run the following, replacing `switch` with the target platform (one of `switch`, `wiiu`, or `pc`):
```
git clone https://github.com/vgmoose/hb-appstore.git
cd hb-appstore
docker run -e PLATFORM='switch' -v $(pwd):/code -it ubuntu:18.10 /bin/bash
```
3. You should now be in the container, run:
```
cd /code
./dependency_helper.sh
make
```

Depending on which platform you chose, `appstore.nro` or `appstore.rpx` should now be sitting in the cloned directory.

### Building for Switch (with libnx)
1. Install [dkp-pacman](https://devkitpro.org/viewtopic.php?f=13&t=8702)
2. Install devkitA64 and needed Switch dependencies via dkp-pacman:
```
sudo dkp-pacman -S devkitA64 libnx switch-tools switch-curl switch-bzip2 switch-freetype switch-libjpeg-turbo switch-libwebp switch-sdl2 switch-sdl2_gfx switch-sdl2_image switch-sdl2_ttf switch-zlib switch-libpng switch-mesa
```
3. *If on macOS*: install gtar with: `brew install gnu-tar` (needed by resinfs)
4. Once it's all setup, recursively clone the repo and run make:
```
git clone --recursive https://github.com/vgmoose/hb-appstore.git
cd hb-appstore
make -f Makefile.switch
```

If all goes well, `appstore.nro` should be sitting in the current directory.

### Building for Wii U (with WUT)
1. Install [dkp-pacman](https://devkitpro.org/viewtopic.php?f=13&t=8702)
2. Setup [wiiu-fling](https://gitlab.com/QuarkTheAwesome/wiiu-fling#wiiu-fling) according to the instructions
3. Install devkitPPC and needed Wii U dependencies via dkp-pacman:
```
sudo dkp-pacman -S wut wiiu-sdl2 devkitPPC wiiu-libromfs wiiu-sdl2_gfx wiiu-sdl2_image wiiu-sdl2_ttf wiiu-sdl2_mixer ppc-zlib ppc-bzip2 ppc-freetype ppc-mpg123 ppc-libpng wiiu-curl-headers ppc-pkg-config wiiu-pkg-config
```
4. *If on macOS*: install gtar with: `brew install gnu-tar` (needed by resinfs)
5. Once the environment is setup:
```
git clone --recursive https://github.com/vgmoose/hb-appstore.git
cd hb-appstore
make -f Makefile.wiiu
```

If all goes well, `appstore.rpx` should be sitting in the current directory.

### Building for PC
There's a makefile for building the SDL2 app on PC as well, primarily used for debugging and performance monitoring. Below instructions are for Ubuntu, but should be similar on other platforms:
```
sudo apt-get install libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev libsdl2-gfx-dev zlib1g-dev gcc g++ libcurl4-openssl-dev
git clone --recursive https://github.com/vgmoose/hb-appstore.git
cd hb-appstore
make -f Makefile.pc
```

### License
This software is licensed under the GPLv3.

#### Maintainers
- [vgmoose](https://github.com/vgmoose)
- [pwsincd](https://github.com/pwsincd)
- [rw-r-r_0644](https://github.com/rw-r-r-0644)
- [crc32](https://github.com/crc-32)

#### Contributing
If you have some functionality that you'd like to see feel free to discuss it on an [issues page](https://github.com/vgmoose/hb-appstore/issues), or if you already have an implementation or desire that you'd like to see, feel free to fork and make a [pull request](https://github.com/vgmoose/hb-appstore/pulls)!

It's not required, but running a clang-format before making a PR helps to clean up styling issues:
```
find . \( -name "*.cpp" -or -name "*.hpp" \) -not -path "./libs/*" -exec clang-format -i {} \;
```

