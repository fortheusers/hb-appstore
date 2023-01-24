#pragma once

#if defined(__WIIU__)
#define DEFAULT_REPO "https://wiiu.cdn.fortheusers.org"
#elif defined(_3DS) || defined(_3DS_MOCK)
#define DEFAULT_REPO "https://3ds.apps.fortheusers.org"
#else
#define DEFAULT_REPO "https://switch.cdn.fortheusers.org"
#endif

#define SOUND_PATH "./.toggle_sound"
#define DEFAULT_GET_HOME "./.get/"

void quit();
