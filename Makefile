BINARY      := appstore

APP_TITLE   := HB App Store
APP_AUTHOR  := 4TU Team
APP_VERSION := 2.3

SOURCES     += gui console
CFLAGS      += -DMUSIC
LDFLAGS += -lSDL2_mixer

include libs/get/Makefile
include libs/chesto/Makefile
