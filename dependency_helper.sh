#!/bin/bash
# This is a dependency helper script to help set up dependencies
# for an Ubuntu Xenial target (used by travis)

# It's probably better to follow the README.md instructions for the
# desired target and platform that you want to build, but this may
# help if you are interested in seeing how the dependencies come together
# on various platforms

install_container_deps () {
  apt-get update && apt-get install -y wget sudo libxml2 xz-utils lzma build-essential haveged
  haveged &
  touch /trustdb.gpg
}

setup_dkp_repo () {
  wget https://github.com/devkitPro/pacman/releases/download/devkitpro-pacman-1.0.1/devkitpro-pacman.deb
  sudo dpkg -i devkitpro-pacman.deb
}

install_intel_deps () {
  sudo apt-get -y install wget git libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev libsdl2-gfx-dev zlib1g-dev gcc g++ libcurl4-openssl-dev
}

if ! [ -x "$(command -v sudo)" ]; then
  install_container_deps
fi

case "${PLATFORM}" in
  pc)
      install_intel_deps
    ;;
  switch)   # currently libnx
      setup_dkp_repo
      sudo dkp-pacman --noconfirm -S devkitA64 libnx switch-tools switch-curl switch-bzip2 switch-freetype switch-libjpeg-turbo switch-libwebp switch-sdl2 switch-sdl2_gfx switch-sdl2_image switch-sdl2_ttf switch-zlib switch-libpng switch-mesa
    ;;
  wiiu)   # uses wut
      setup_dkp_repo

      sudo dkp-pacman-key --recv 6F986ED22C5B9003
      sudo dkp-pacman-key --lsign 6F986ED22C5B9003

      sudo echo "
      [wiiu-fling]
      Server = https://fling.heyquark.com
      " | sudo tee --append /opt/devkitpro/pacman/etc/pacman.conf

      sudo dkp-pacman -Syu

      sudo dkp-pacman --noconfirm -S wut-linux wiiu-sdl2 devkitPPC wiiu-libromfs wiiu-sdl2_gfx wiiu-sdl2_image wiiu-sdl2_ttf wiiu-sdl2_mixer ppc-zlib ppc-bzip2 ppc-freetype ppc-mpg123 ppc-libpng wiiu-curl-headers ppc-pkg-config wiiu-pkg-config
    ;;
esac

