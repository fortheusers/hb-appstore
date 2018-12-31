#!/bin/bash
# This is a dependency helper script to help set up dependencies
# for an Ubuntu Xenial target (used by travis)

# It's probably better to follow the README.md instructions for the
# desired target and platform that you want to build, but this may
# help if you are interested in seeing how the dependencies come together
# on various platforms

setup_dkp_repo () {
  # todo: containerize build in fedora, and use real pacman instead of dkp fork
  wget https://github.com/devkitPro/pacman/releases/download/devkitpro-pacman-1.0.1/devkitpro-pacman.deb
  sudo dpkg -i devkitpro-pacman.deb

  # sudo pacman-key --recv F7FD5492264BB9D0
  # sudo pacman-key --lsign F7FD5492264BB9D0
}

case "${PLATFORM}" in
  pc)
    sudo apt-get -y install libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev libsdl2-gfx-dev zlib1g-dev gcc g++
    ;;
  switch)   # currently libnx
      setup_dkp_repo
      sudo dkp-pacman --noconfirm -S devkitA64 libnx switch-tools switch-curl switch-bzip2 switch-freetype switch-libjpeg-turbo switch-sdl2 switch-sdl2_gfx switch-sdl2_image switch-sdl2_ttf switch-zlib switch-libpng switch-mesa
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

      sudo dkp-pacman --noconfirm -S wut-linux wiiu-sdl2 devkitPPC wiiu-libromfs ppc-zlib ppc-bzip2 ppc-freetype ppc-libpng wiiu-curl-headers
    ;;
esac

