## Switch App Store
A graphical frontend to the [get package manager](https://github.com/vgmoose/get) for downloading and managing homebrew on the Nintendo Switch. This is a successor to the [Wii U Homebrew App Store](https://github.com/vgmoose/hbas).

### Building
Setup devkita64 and libnx [using the installer](http://switchbrew.org/index.php?title=Setting_up_Development_Environment), and then set up SDL1.2 and the portlibs according to the [instructions here](https://gbatemp.net/threads/sdl-1-2-15-for-switch-libnx-based.497412/). Currently, this project uses the zlib and libcurl portlibs.

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

### License
This software is licensed under the GPLv3.
