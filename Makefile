BINARY      := appstore

APP_TITLE   := HB App Store
APP_AUTHOR  := 4TU Team

SOURCES      += gui
SDL2_SOURCES += console

include libs/chesto/Makefile
include libs/get/Makefile
