# Guest's Game Store
[![GPLv3 License](https://img.shields.io/badge/license-GPLv3-blue.svg?style=flat-square)](https://opensource.org/licenses/GPL-3.0)
[![gitlab ci](https://gitlab.com/4TU/hb-appstore/badges/master/pipeline.svg?style=flat-square)](https://gitlab.com/4TU/hb-appstore/pipelines)
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome!-tomato.svg?style=flat-square)](http://makeapullrequest.com)

A [Chesto](https://gitlab.com/4TU/chesto)-based graphical frontend to the [get package manager](https://gitlab.com/4TU/libget) for downloading and managing homebrew on The Wii u And 3DS This is a replacement to the the soon to be shutdown Nintendo eshop for wii u and 3ds

## Supported Platforms

### 3DS
The 3DS Family of systems do not have a port as of right now one day it will As of right now only the wii u has a replacement
### Nintendo Wii U
The Game Store Is not out yet! it will be soon 
Please refer to the hb-appstore games section for now https://github.com/fortheusers/hb-appstore/releases

### Custom Music
Place a mp3 file named `background.mp3` in the following location:
- Wii U: `sd:/wiiu/apps/gamestore/background.mp3`




### 3DS Build
There Is not a port to 3DS As of now These can be compiled but are not suggested due to alot of bugs

If you would like to help speed up development for this  or put games on the store please feel free to make a PR For or contact us on the [Guest Game Store Discord](https://discord.gg/g6RXjBuyEQ)!

## Submitting A Game
To Submit a game to the store please Contact us in The Discord Server Link Above Please note that we only speak english so please use machine translating if possible(https://discord.gg/g6RXjBuyEQ)


## Compilation instructions
This program is written using [chesto](https://gitlab.com/4TU/sealo) and has dependencies on libcurl, libget, and zlib. The chesto and libget libraries are included in this repo as submodules. SDL2 or SDL1 is also required depending on the target platform.

### Building with Docker
The easiest way to build is using the [Spheal](https://gitlab.com/4TU/spheal) x86_64 docker container. It uses this [dependency helper script](https://gitlab.com/4TU/spheal/-/blob/master/dependency_helper.sh) to be able to build for all supported platforms. This is how the pre-compiled binaries are built.

1. Install [Docker](https://www.docker.com)
2. Run the following, replacing `wiiu` with the target platform these are the options: pc 3ds wiiu switch pc-sdl1 wii
```
git clone --recursive https://github.com/GuestDreemurr/game-appstore.git
cd game-appstore
export PLATFORM=wiiu
docker run -v $(pwd):/code -it ghcr.io/fortheusers/sealeo:latest /bin/bash -c "cd /code && make $PLATFORM"
```

Depending on which platform you chose, `gamestore.nro` or `gamestore.rpx` should now be sitting in the cloned directory. When building for Wii U, an `gamestore.wuhb` file will also be present.

If you are using an M1 Mac, you may have more luck running [dependency_helper.sh](https://gitlab.com/4TU/spheal/-/blob/master/dependency_helper.sh) inside of an arm64 ubuntu container, or trying the platform-specific instructions below.

### Building for Specific Platforms
Compilation instructions for specific supported platforms (Switch, Wii U, 3DS, Wii) can be found in [Compiling.md](https://github.com/GuestDreemurr/game-appstore/blob/main/docs/Compiling.md)

## License
This software is licensed under the GPLv3.

> Free software is software that gives you the user the freedom to share, study and modify it. We call this free software because the user is free. - [Free Software Foundation](https://www.fsf.org/about/what-is-free-software)

### Maintainers
- [GuestDreemurr](https://github.com/GuestDreemurr)

## Contributing
If you have some functionality that you'd like to see feel free to discuss it on an [issues page](https://github.com/GuestDreemurr/game-appstore/issues), or if you already have an implementation or desire that you'd like to see, feel free to fork and make a [pull request](https://github.com/GuestDreemurr/game-appstore/pulls)!

For code contributions, it's not required, but running a clang-format before making a PR helps to clean up styling issues:
```
find . \( -name "*.cpp" -or -name "*.hpp" \) -not -path "./libs/*" -exec clang-format -i {} \;
```

### Contributors

#### Code Contributors

This project exists thanks to all the people who love the Wii U Thanks FOSS People And The People Who Break The Laws of Physics on The Wii U!
Thank 4TU For Existing And all homebrew developers

### FAQ

Q: Will This Brick My Wii U

A: No As We Are Not Replacing the eshop applet and have no plans to we would say for home menu boot that would be the .wuhb build

Q: When Will This Release

A: There Is No ETA As Of Now But Probably november 2022 Or early 2023

Q: Does This Provide Nintendo Games

A: No For Legal Reasons We Can't Do That We Only Provide Homebrew games and no SDK Usage allowed

Q:Can I use this with pretendo network

A: Not Right now as pretendo bugs up hb-appstore/gamestore and shows a black screen so as of now No Pretendo Usage Works

Q: How Do I get this on my home menu

A: Aroma Please use the .wuhb build put it in sd/wiiu/apps
