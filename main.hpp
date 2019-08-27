#pragma once

#if defined(__WIIU__)
#define DEFAULT_REPO "https://wiiu.apps.fortheusers.org"
#else
#define DEFAULT_REPO "https://switch.apps.fortheusers.org"
#endif

void quit();
