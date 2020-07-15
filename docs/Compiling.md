Specific instructions for various platforms are covered here!

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
2. Setup [wiiu-fling](https://gitlab.com/QuarkTheAwesome/wiiu-fling#wiiu-fling) according to the instructions (**NOTICE**: at this time of writing the fling repo's will need to go above the dkp ones in order to properly build with SDL)
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

### Building for 3DS (with libctru)
1. Install [dkp-pacman](https://devkitpro.org/viewtopic.php?f=13&t=8702)
2. Install devkitARM and needed 3DS dependencies via dkp-pacman:
```
sudo dkp-pacman --noconfirm -S devkitPPC libogc gamecube-tools ppc-zlib ppc-bzip2 ppc-freetype ppc-mpg123 ppc-libpng ppc-pkg-config ppc-libvorbisidec ppc-libjpeg-turbo libfat-ogc
```
3. *If on macOS*: install gtar with: `brew install gnu-tar` (needed by resinfs)
4. Once it's all setup, recursively clone the repo and run make:
```
git clone --recursive https://github.com/vgmoose/hb-appstore.git
cd hb-appstore
make -f Makefile.3ds
```

If all goes well, `appstore.3dsx` should be sitting in the current directory.

### Building for Wii (with libogc)
1. Install [dkp-pacman](https://devkitpro.org/viewtopic.php?f=13&t=8702)
2. Install devkitPPC and needed Wii dependencies via dkp-pacman:
```
sudo dkp-pacman -S 3ds-sdl 3ds-sdl_image 3ds-sdl_mixer 3ds-sdl_gfx 3ds-sdl_ttf libctru citro3d 3dstools 3ds-curl 3ds-mbedtls
```
3. *If on macOS*: install gtar with: `brew install gnu-tar` (needed by resinfs)
4. Once it's all setup, recursively clone the repo and run make:
```
git clone --recursive https://github.com/vgmoose/hb-appstore.git
cd hb-appstore
make -f Makefile.wii
```

If all goes well, `appstore.dol` should be sitting in the current directory.

### Building for PC
There's a makefile for building the SDL2 app on PC as well, primarily used for debugging and performance monitoring. Below instructions are for Ubuntu, but should be similar on other platforms:
```
sudo apt-get install libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev libsdl2-gfx-dev zlib1g-dev gcc g++ libcurl4-openssl-dev
git clone --recursive https://github.com/vgmoose/hb-appstore.git
cd hb-appstore
make -f Makefile.pc
```
