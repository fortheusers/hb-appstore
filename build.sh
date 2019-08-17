#!/bin/bash

if [[ -z "${PLATFORM}" ]]; then
  echo "No PLATFORM env variable specified, select platform:"
  platforms=("Switch" "Wii U" "PC" "Buck")
  PS3="Enter your choice (1-4): "
  select opt in "${platforms[@]}"
  do
    case $opt in
      Switch)	PLATFORM="switch"; break;;
      "Wii U")	PLATFORM="wiiu"; break;;
      PC)	PLATFORM="pc"; break;;
      Buck)	PLATFORM="buck"; break;;
      *)	echo "Invalid choice, enter 1-4"
    esac
  done
fi

if [[ "${PLATFORM}" == "buck" ]]; then
  buckaroo install
  buck build :hb-appstore
else
  make -f Makefile.${PLATFORM}
fi
