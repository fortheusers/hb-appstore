#!/bin/bash
# This is a dependency helper script to help set up requirements for running make

# Before running it, you should export the desired PLATFORM environment
# variable ( one of pc, pc-sdl1, switch, 3ds, wii, wiiu )
#  eg. PLATFORM=switch ./dependency_helper.sh

# this script should work when ran on these OSes:
#   - archlinux (uses native pacman)
#   - ubuntu:18.04 w/ dkp-pacman (flakey)
# (aka, if you have either pacman or apt-get already)
# TODO: add macOS, WSL (windows),  andfedora builds using $OSTYPE checks

# It's probably better to follow the README.md instructions for the
# desired target and platform that you want to build, but this may
# help if you are interested in seeing how the dependencies come together
# on various platforms, or how the CI works

export HAS_PACMAN="$(command -v pacman)"
export HAS_SUDO="$(command -v sudo)"

main_platform_logic () {
  case "${PLATFORM}" in
    pc)
        sudo pacman --noconfirm -S sdl2
      ;;
    pc-sdl1)
        sudo pacman --noconfirm -S sdl
      ;;
    switch) # currently libnx
        setup_dkp_repo
        sudo ${DKP}pacman --noconfirm -S devkitA64 libnx switch-tools switch-curl switch-bzip2 switch-freetype switch-libjpeg-turbo switch-libwebp switch-sdl2 switch-sdl2_gfx switch-sdl2_image switch-sdl2_ttf switch-zlib switch-libpng switch-mesa
      ;;
    3ds)    # uses libctru
        setup_dkp_repo
        sudo ${DKP}pacman --noconfirm -S devkitARM 3ds-sdl 3ds-sdl_image 3ds-sdl_mixer 3ds-sdl_gfx 3ds-sdl_ttf libctru citro3d 3dstools 3ds-curl 3ds-mbedtls
      ;;
    wii)    # uses libogc
        setup_dkp_repo
        sudo ${DKP}pacman --noconfirm -S devkitPPC libogc gamecube-tools wii-sdl wii-sdl_gfx wii-sdl_image wii-sdl_mixer wii-sdl_ttf ppc-zlib ppc-bzip2 ppc-freetype ppc-mpg123 ppc-libpng ppc-pkg-config ppc-libvorbisidec ppc-libjpeg-turbo libfat-ogc
      ;;
    wiiu)   # uses wut
        setup_fling_repo
        sudo ${DKP}pacman --noconfirm -S wut-linux wiiu-sdl2 devkitPPC wiiu-libromfs wiiu-sdl2_gfx wiiu-sdl2_image wiiu-sdl2_ttf wiiu-sdl2_mixer ppc-zlib ppc-bzip2 ppc-freetype ppc-mpg123 ppc-libpng wiiu-curl-headers ppc-pkg-config wiiu-pkg-config
      ;;
  esac
}

install_container_deps () {
  if [ ! -z $HAS_PACMAN ]; then
    pacman --noconfirm -Syuu && pacman --noconfirm -Sy wget sudo base-devel
    pacman-key --init
  else
    apt-get update && apt-get install -y wget sudo libxml2 xz-utils lzma build-essential haveged
    haveged &
    touch /trustdb.gpg
  fi
}

setup_sdl2_deps () {
  sudo apt-get -y install libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev libsdl2-gfx-dev zlib1g-dev gcc g++ libcurl4-openssl-dev wget git
}

setup_sdl_deps () {
  sudo apt-get -y install libsdl1.2-dev libsdl-ttf2.0-dev libsdl-image1.2-dev libsdl-gfx1.2-dev zlib1g-dev gcc g++ libcurl4-openssl-dev wget git
}

setup_fling_repo () {
  setup_dkp_repo

  # trust quarky signing keys
  sudo ${DKP}pacman-key --recv 6F986ED22C5B9003
  sudo ${DKP}pacman-key --lsign 6F986ED22C5B9003

  sudo echo "
    [wiiu-fling]
    Server = https://fling.heyquark.com
    $(cat $PACMAN_ROOT/etc/pacman.conf)
  " | sudo tee $PACMAN_ROOT/etc/pacman.conf

  sudo ${DKP}pacman --noconfirm -Syu
}

export DKP=""
export PACMAN_ROOT=""

setup_dkp_repo () {
  if [ -z $HAS_PACMAN ]; then
    # we don't have a pacman command on this system, try dkP's
    setup_dkp_pacman && return
  fi

  # trust wintermute and fincs signing keys
  sudo pacman-key --recv BC26F752D25B92CE272E0F44F7FD5492264BB9D0 62C7609ADA219C60
  sudo pacman-key --lsign BC26F752D25B92CE272E0F44F7FD5492264BB9D0 62C7609ADA219C60

  sudo echo "
    [dkp-libs]
    Server = http://downloads.devkitpro.org/packages

    [dkp-linux]
    Server = http://downloads.devkitpro.org/packages/linux/$arch/
  " | sudo tee --append /etc/pacman.conf
  
  pacman --noconfirm -Syu
}

setup_dkp_pacman () {
  # WARNING: this URL for dkp's pacman will become out of date, check https://github.com/devkitPro/pacman/releases/ for the latest URL
  # dkP team is aware of this issue and chooses to remove older download links specifically to not support using build scripts like this one
  wget https://github.com/devkitPro/pacman/releases/download/v1.0.2/devkitpro-pacman.amd64.deb
  sudo dpkg -i devkitpro-pacman.deb

  DKP="dkp-"
  PACMAN_ROOT="/opt/devkitpro/pacman/"
}

if [ -z $HAS_SUDO ]; then
  install_container_deps
fi

main_platform_logic
