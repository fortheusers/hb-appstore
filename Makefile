BINARY      := appstore

APP_TITLE   := HB App Store
APP_AUTHOR  := 4TU Team
APP_VERSION := 2.4-pre

SOURCES     += gui console
DEBUG_BUILD := 1

# CFLAGS    += -DWII_MOCK=1
# CFLAGS += -D_3DS_MOCK

ifeq (wiiu,$(MAKECMDGOALS))
SOURCES   += libs/librpxloader/source
INCLUDES  += ../libs/librpxloader/include
endif

include libs/get/Makefile
include libs/chesto/Makefile
