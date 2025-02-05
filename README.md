# Homebrew App Store

[![GPLv3 License](https://img.shields.io/badge/license-GPLv3-blue.svg?style=flat-square)](https://opensource.org/licenses/GPL-3.0)
[![gh actions](https://img.shields.io/github/actions/workflow/status/fortheusers/hb-appstore/main.yml?style=flat-square)](https://github.com/fortheusers/hb-appstore/actions/workflows/main.yml)
[![gitlab ci](https://gitlab.com/4TU/hb-appstore/badges/master/pipeline.svg?style=flat-square)](https://gitlab.com/4TU/hb-appstore/pipelines)
[![discord](https://img.shields.io/discord/339118412414582786.svg?color=blueviolet&label=discord&style=flat-square)](https://discordapp.com/invite/F2PKpEj)
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome!-tomato.svg?style=flat-square)](http://makeapullrequest.com)

The Homebrew App Store is a GUI that can be used to access an online repository of open-source [homebrew apps](https://en.wikipedia.org/wiki/Homebrew_(video_games)). It's written in [SDL2 with Chesto](https://github.com/fortheusers/chesto) and uses the [get package manager](https://github.com/fortheusers/libget) to download and manage open-source apps.

## Supported Platforms

### Web Browsers
A web interface to download files from the Homebrew App Store directly is available at [apps.fortheusers.org](https://apps.fortheusers.org). Source code: [hbas-frontend](https://github.com/fortheusers/hbas-frontend).

### Wii U
The ability to run homebrew apps is required. Download and extract the latest [hb-appstore](https://github.com/fortheusers/hb-appstore/releases) to `sd:/wiiu/apps/appstore/`, and run "HB App Store" either from the home menu or the Homebrew Launcher.

### Switch
Access to the Homebrew Menu is required. Download and extract the latest [hb-appstore](https://github.com/fortheusers/hb-appstore/releases) to `sd:/switch/appstore/`, and run "HB App Store" from within hbmenu.

### Linux, Mac, Windows
Standalone PC executables are available [here](https://nightly.link/fortheusers/hb-appstore/workflows/pc-builds/main). These PC clients download and extract files to an `sdroot` folder relative to the app's path.

In the future, direct support for selecting an SD card different repos is planned.

### Wii and 3DS
There are work-in-progress ports to Wii and 3DS, with varying degrees of feature support. These platforms are not yet ready to be used by most users.

**Note**: The Wii/3DS ports will utilize existing community repos (such as [Wii/OSC](https://oscwii.org) and [3ds/universal-db](https://db.universal-team.net/3ds/)).

## Configuration
### Custom Music
Place a mp3 file named `background.mp3` in one of the following locations:
- PC: `./background.mp3` (in the current directory)
- Wii U: `sd:/wiiu/apps/appstore/background.mp3`
- Switch: `sd:/switch/appstore/background.mp3`

### Submitting an App
See [hb-app.store/submit-or-request](https://hb-app.store/submit-or-request) for App Store guidelines and instructions on how to submit or request homebrew apps for to the main repositories.

### Maintaining a repo
See [get's instructions](https://github.com/fortheusers/get#setting-up-repos) for setting up a repository. All repo JSON data and package zips are designed to be statically hosted as files, with no explicit backend logic.

For more information on the API and self-hosting, see: [hb-app.store/api-info](https://hb-app.store/api-info)

## License
This software is licensed under the GPLv3.

### Credits
For an up-to-date list of repo maintainers, app authors, and special thanks, see [hb-app.store/about](https://hb-app.store/about)!

### Contributing
If you have some functionality that you'd like to see feel free to discuss it on an [issues page](https://github.com/fortheusers/hb-appstore/issues), or if you already have an implementation or desire that you'd like to see, please send in a [pull request](https://github.com/fortheusers/hb-appstore/pulls)!

We're also currently working on localization and translation of the app into other languages. Please see the [current languages here](https://github.com/fortheusers/hb-appstore/tree/main/resin/res/i18n), and if you are inclined to submit a new one, thanks!

## Compiling
This program is written using [Chesto](https://github.com/fortheusers/chesto) and has dependencies on libcurl, libget, and zlib. The chesto and libget libraries are included in this repo as submodules.

Pre-compiled builds off the main branch can be downloaded [here](https://nightly.link/fortheusers/hb-appstore/workflows/main/main) without signing in.

### Instructions
Detailed build instructions for specific platforms can be found in [Compiling.md](https://github.com/fortheusers/hb-appstore/blob/main/docs/Compiling.md).

In general, clone the repository recursively (with `git clone --recursive`) and run `make <platform>`, after installing the required platform dependencies.
