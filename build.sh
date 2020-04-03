#!/bin/bash

if [[ -z "${PLATFORM}" ]]; then
  echo "No PLATFORM env variable specified, select platform:"
  platforms=("Switch" "Wii U" "3ds" "PC")
  PS3="Enter your choice (1-4): "
  select opt in "${platforms[@]}"
  do
    case $opt in
      Switch)	PLATFORM="switch"; break;;
      "Wii U")	PLATFORM="wiiu"; break;;
      "3ds")	PLATFORM="3ds"; break;;
      PC)	PLATFORM="pc"; break;;
      *)	echo "Invalid choice, enter 1-4"
    esac
  done
fi

make -f Makefile.${PLATFORM}
