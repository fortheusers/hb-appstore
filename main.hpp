#pragma once

#if defined(__WIIU__)
#define DEFAULT_REPO "https://wiiubru.com/appstore"
#elif defined(_3DS) || defined(_3DS_MOCK)
#define DEFAULT_REPO "https://3ds.apps.fortheusers.org"
#else
#define DEFAULT_REPO "https://switchbru.com/appstore"
#endif

#define DEFAULT_GET_HOME "./.get/"

void quit();
