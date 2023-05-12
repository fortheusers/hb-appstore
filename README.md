# Homebrew App Store
[![GPLv3 License](https://img.shields.io/badge/license-GPLv3-blue.svg?style=flat-square)](https://opensource.org/licenses/GPL-3.0)
[![gh actions](https://img.shields.io/github/actions/workflow/status/fortheusers/hb-appstore/main.yml?style=flat-square)](https://github.com/fortheusers/hb-appstore/actions/workflows/main.yml)
[![gitlab ci](https://gitlab.com/4TU/hb-appstore/badges/master/pipeline.svg?style=flat-square)](https://gitlab.com/4TU/hb-appstore/pipelines)
[![discord](https://img.shields.io/discord/339118412414582786.svg?color=blueviolet&label=discord&style=flat-square)](https://discordapp.com/invite/F2PKpEj)
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome!-tomato.svg?style=flat-square)](http://makeapullrequest.com)

A [Chesto](https://github.com/fortheusers/chesto)-based graphical frontend to the [get package manager](https://github.com/fortheusers/libget) for downloading and managing homebrew on video game consoles, such as the Nintendo Switch and Wii U. This is a replacement to the older [Wii U Homebrew App Store](https://github.com/vgmoose/wiiu-hbas).

## Supported Platforms
### Nintendo Switch
To run this program, a Nintendo Switch with access to the Homebrew Menu is required. This can be done on most Switches manufactured before July 2018, for compatibility check your serial number at [ismyswitchpatched.com](https://ismyswitchpatched.com). To run hbmenu, see the tutorial [here](https://guide.sdsetup.com/).

Download and extract the latest [hb-appstore](https://github.com/fortheusers/hb-appstore/releases) to `sd:/switch/appstore/`, and run "HB App Store" from within hbmenu. When you're done, you can press the Home or Minus buttons to exit.

### Nintendo Wii U
To run this program, a modded Wii U is required. This can be done on any console or firmware, but for a compatibility check anyway, see [ismywiiupatched.com](https://ismywiiupatched.com). To run the Homebrew Launcher, see the tutorial [here](https://wiiuguide.xyz/).

Download and extract the latest [hb-appstore](https://github.com/fortheusers/hb-appstore/releases) to `sd:/wiiu/apps/appstore/`, and run "HB App Store" from within Homebrew Launcher. When you're done, you can press the Home or Minus buttons to exit.

If you're using the [Aroma beta](https://github.com/wiiu-env/Aroma), you can use the .wuhb file instead of the .rpx to launch the app directly from the Wii U home menu instead of HBL.

### Custom Music
Place a mp3 file named `background.mp3` in one of the following two locations:
- Wii U: `sd:/wiiu/apps/appstore/background.mp3`
- Switch: `sd:/switch/appstore/background.mp3`

### Web and Desktop
Web browsers can download files from the Homebrew App Store at [apps.fortheusers.org](https://apps.fortheusers.org). Source code: [hbas-frontend](https://github.com/fortheusers/hbas-frontend).

Desktop platforms (Windows, MacOS, and Linux), can use @LyfeOnEdge's [unofficial-appstore](https://github.com/LyfeOnEdge/unofficial-appstore), or their alternative program: [HBUpdater](https://github.com/LyfeOnEdge/HBUpdater).

There is a PC port of the same client source code for Linux, Mac, and Windows that can be downloaded from [the nightlies](https://nightly.link/fortheusers/hb-appstore/workflows/pc-builds/main). This will download files to an `sdroot` folder where the app is ran.

### 3DS and Wii
There are work-in-progress ports to 3DS and Wii, with varying degrees of feature support. These can be compiled using the instructions below, but aren't yet ready to be used by most users.

These clients are being developed with the intent to hook up to other existing non-libget repos (such as [Wii/OSC](https://oscwii.org) and [3ds/universal-db](https://db.universal-team.net/3ds/)) in the future. [libget has `*Repo.cpp` implementations](https://github.com/fortheusers/libget/tree/main/src) that would be extended to add this support.

## Maintaining a repo
See [get's instructions](https://github.com/vgmoose/get#setting-up-repos) for setting up a repository. Everything is designed to be statically hosted. If there's no repo provided in the `repos.json` config file, then it will generate a default one pointing to [wiiu.cdn.fortheusers.com](https://wiiu.cdn.fortheusers.com) or [switch.cdn.fortheusers.com](https://switch.cdn.fortheusers.com).

If you run into any issues and need help maintaining or setting up a libget repo, feel free to get in touch with vgmoose at me@vgmoose.com or on Discord.

## Compilation instructions
This program is written using [chesto](https://github.com/fortheusers/chesto) and has dependencies on libcurl, libget, and zlib. The chesto and libget libraries are included in this repo as submodules. SDL2 or SDL1 is also required depending on the target platform.

You can get pre-compiled binaries for each platform under [GH Actions](https://github.com/fortheusers/hb-appstore/actions/workflows/main.yml) for a given commit. [Click here](https://nightly.link/fortheusers/hb-appstore/workflows/main/main) to download the latest nightly builds without signing in.

### Building with Docker
The easiest way to build is using the [Sealeo](https://github.com/fortheusers/sealeo) docker container. It uses this [dependency helper script](https://github.com/fortheusers/sealeo/blob/main/dependency_helper.sh) to be able to build for all supported platforms. This is how the pre-compiled binaries are built.

1. Install [Docker](https://www.docker.com)
2. Run the following, replacing `switch` with the target platform (one of `switch`, `wiiu`, or `pc`):
```
git clone --recursive https://github.com/fortheusers/hb-appstore.git
cd hb-appstore
export PLATFORM=switch    # or wiiu, 3ds, wii, pc, pc-sdl1
docker run -v $(pwd):/code -it ghcr.io/fortheusers/sealeo /bin/bash -c "cd /code && make $PLATFORM"
```

Depending on which platform you chose, `appstore.nro` or `appstore.rpx` should now be sitting in the cloned directory. When building for Wii U, an `appstore.wuhb` file will also be present.

### Building for Specific Platforms
Compilation instructions for specific supported platforms (Switch, Wii U, 3DS, Wii) can be found in [Compiling.md](https://github.com/fortheusers/hb-appstore/blob/main/docs/Compiling.md)

## License
This software is licensed under the GPLv3.

> Free software is software that gives you the user the freedom to share, study and modify it. We call this free software because the user is free. - [Free Software Foundation](https://www.fsf.org/about/what-is-free-software)

### Maintainers
- [vgmoose](https://github.com/vgmoose)
- [pwsincd](https://github.com/pwsincd)
- [Nightkingale](https://github.com/Nightkingale)
- [rw-r-r_0644](https://github.com/rw-r-r-0644)
- [crc32](https://github.com/crc-32)
- [Compucat](https://github.com/compucat)

## Contributing
If you have some functionality that you'd like to see feel free to discuss it on an [issues page](https://github.com/fortheusers/hb-appstore/issues), or if you already have an implementation or desire that you'd like to see, feel free to fork and make a [pull request](https://github.com/fortheusers/hb-appstore/pulls)!

For code contributions, it's not required, but running a clang-format before making a PR helps to clean up styling issues:
```
find . \( -name "*.cpp" -or -name "*.hpp" \) -not -path "./libs/*" -exec clang-format -i {} \;
```

### Contributors

#### Code Contributors
This project exists thanks to all the people who contribute!
<a href="https://github.com/fortheusers/hb-appstore/graphs/contributors"><img src="https://opencollective.com/fortheusers/contributors.svg?width=890&button=false" /></a>

#### Financial Contributions
Special thanks to [dojafoja](https://github.com/dojafoja) for donating to the project in the past! The funds went directly torwards repository hosting and maintenance. ([Archived OpenCollective link](https://opencollective.com/fortheusers))

If you would like to monetarily support this project, we ask that you instead make a donation towards some other cause instead. Here are a few notable ones:

- [Wikipedia / Wikimedia](https://donate.wikimedia.org/)
- [Electronic Frontier Foundation](https://www.eff.org)
- [Save the Children](https://www.savethechildren.org)
- [Partners in Health](https://www.pih.org)
- [Good Food Institute](https://gfi.org)
- [Mercy for Animals](https://mercyforanimals.org)
- [Fandom Forward](https://fandomforward.org)
- [NaNoWriMo](https://nanowrimo.org)

Thank you!
