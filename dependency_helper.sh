#!/bin/bash
# This is a dependency helper script to help set up requirements for running make
# primarly this script is used by the CI, but is also useful when setting up a new env

# Before running it, you should export the desired PLATFORM environment
# variable ( one of pc, pc-sdl1, switch, 3ds, wii, wiiu )
#  eg. PLATFORM=switch ./dependency_helper.sh

# this script should work when ran on these OSes:
#   - archlinux (uses native pacman)
#   - ubuntu:18.04 w/ dkp-pacman (flakey)
# (aka, if you have either pacman or apt-get already)
# TODO: add macOS, WSL (windows), and fedora builds using $OSTYPE checks

# It's probably better to follow the README.md instructions for the
# desired target and platform that you want to build, but this may
# help if you are interested in seeing how the dependencies come together
# on various platforms, or how the CI works

export HAS_PACMAN="$(command -v pacman)"
export HAS_SUDO="$(command -v sudo)"

main_platform_logic () {
  case "${PLATFORM}" in
    pc)
        setup_deb_sdl_deps || sudo pacman --noconfirm -S sdl2 sdl2_image sdl2_gfx sdl2_ttf
      ;;
    pc-sdl1)
        setup_deb_sdl_deps || sudo pacman --noconfirm -S sdl sdl_image sdl_gfx sdl_ttf
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

        # one day replace with pacman: https://github.com/dborth/sdl-wii/issues/54
        sudo apt-get -y install wget git
        git clone https://gitlab.com/4TU/sdl-wii.git
        export DEVKITPRO=/opt/devkitpro
        export DEVKITPPC=$DEVKITPRO/devkitPPC
        cd sdl-wii && make && make install
      ;;
    wiiu)   # uses wut
        setup_fling_repo
        sudo ${DKP}pacman --noconfirm -S wut-linux wiiu-sdl2 devkitPPC wiiu-libromfs wiiu-sdl2_gfx wiiu-sdl2_image wiiu-sdl2_ttf wiiu-sdl2_mixer ppc-zlib ppc-bzip2 ppc-freetype ppc-mpg123 ppc-libpng wiiu-curl-headers ppc-pkg-config wiiu-pkg-config
      ;;
  esac
}

install_container_deps () {
  if [ ! -z $HAS_PACMAN ]; then
    pacman --noconfirm -Syuu && pacman --noconfirm -Sy wget sudo base-devel jq git strongswan
    echo "keyserver keys.gnupg.net" >> /etc/pacman.d/gnupg/gpg.conf
    pacman-key --init
  else
    apt-get update && apt-get install -y wget sudo libxml2 xz-utils lzma build-essential haveged
    haveged &
    touch /trustdb.gpg
  fi
}

setup_deb_sdl_deps () {
  # will return positive exit code if apt-get fails, also just grabs both sdl1 and sdl2
  sudo apt-get -y install libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev libsdl2-gfx-dev zlib1g-dev gcc g++ libcurl4-openssl-dev wget git libsdl1.2-dev libsdl-ttf2.0-dev libsdl-image1.2-dev libsdl-gfx1.2-dev
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

retry_pacman_sync () {
  # some continuous integration IPs are blocked by dkP servers, not sure what other verbiage to use to describe that move other than
  # user-hostile! We'll get a new IP for travis to workaround this (Gitlab CI is ok due to using our own runners)
  # currently this workaround is only for archlinux OSes, since those are the containers we use on travis
  
  # load VPN info from environment secret
  declare -a INFO=($VPN_INFO)
  VPN_DATA=${INFO[0]}; VPN_CERT=${INFO[1]}; VPN_USER=${INFO[2]}; VPN_AUTH=${INFO[3]}
  VPN_SERVER=$(curl -s $VPN_DATA | jq -r -c "map(select(.features.ikev2) | .domain) | .[]" | sort -R | head -1)

  echo "$VPN_USER : EAP \"$VPN_AUTH\"" >> /etc/ipsec.secrets 
  echo "conn VPN
          keyexchange=ikev2
          dpdaction=clear
          dpddelay=300s
          eap_identity=\"$VPN_USER\"
          leftauth=eap-mschapv2
          left=%defaultroute
          leftsourceip=%config
          right=${VPN_SERVER}
          rightauth=pubkey
          rightsubnet=0.0.0.0/0
          rightid=%${VPN_SERVER}
          rightca=/etc/ipsec.d/cacerts/VPN.pem
          type=tunnel
          auto=add
  " >> /etc/ipsec.conf 

  wget $VPN_CERT -O /etc/ipsec.d/cacerts/VPN.der >/dev/null 2>&1
  openssl x509 -inform der -in /etc/ipsec.d/cacerts/VPN.der -out /etc/ipsec.d/cacerts/VPN.pem

  ipsec restart; sleep 5; ipsec up VPN >/dev/null 2>&1
  pacman --noconfirm -Syu

  # To any dkP staff that may be reading this: Why don't you want developers to use their own scripts for their own CI?
  # if it's a bandwidth issue, let's talk, because hosting static resources (like pacman repos) should not be incurring bandwidth charges
  # For instance, hb-appstore is statically hosted, and processes several hundred GB a month with no concern about how much bandwidth we use
  # (in fact, we're happy to see more downloads!) so I really can't understand the motive behind this move other than to control CI scripts

  # In my opinion, this absolutely contradicts the usefulness provided by the pacman repos, and pushes devs towards alternative solutions
  # such as using leseratte's historical archives of your tools, or wanting to host their own mirrors
  # Instance of dkP staff disdain for build scripts: https://github.com/diasurgical/devilutionX/issues/739

  # We build for many different platforms and want to have control our own CI images that we build in (a very reasonable desire!)
  # I like shell scripts, they are effective and frequently used in CI across the board, and work on most operating systems (including windows!)
  # It's not dkP staff's concern if they don't like the way that we're building our own packages, happy to talk more about this

  # Also happy to offer assistance mirroring or rehosting your packages under unconstrained bandwidth!
}

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
    Server = http://downloads.devkitpro.org/packages/linux/\$arch/
  " | sudo tee --append /etc/pacman.conf
  
  pacman --noconfirm -Syu || retry_pacman_sync
}

setup_dkp_pacman () {
  # WARNING: this URL for dkp's pacman will become out of date, check https://github.com/devkitPro/pacman/releases/ for the latest URL
  # dkP team is aware of this issue and chooses to remove older download links specifically to not support using build scripts like this one
  wget https://github.com/devkitPro/pacman/releases/download/v1.0.2/devkitpro-pacman.amd64.deb
  sudo dpkg -i devkitpro-pacman.deb

  DKP="dkp-"
  PACMAN_ROOT="/opt/devkitpro/pacman/"

  dkp-pacman --noconfirm -Syu || retry_pacman_sync
}

if [ -z $HAS_SUDO ]; then
  install_container_deps
fi

main_platform_logic
