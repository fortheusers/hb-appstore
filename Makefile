BINARY      := appstore

APP_TITLE   := HB App Store
APP_AUTHOR  := 4TU Team
APP_VERSION := 2.3.2
# APP_VERSION := 2.3.2-$(shell git rev-parse --short HEAD)

SOURCES     += gui console

ifeq (wiiu,$(MAKECMDGOALS))
SOURCES   += libs/librpxloader/source
INCLUDES  += ../libs/librpxloader/include
endif

include libs/get/Makefile
include libs/chesto/Makefile
