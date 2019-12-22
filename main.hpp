#pragma once

#if defined(__WIIU__)
#define DEFAULT_REPO "https://wiiubru.com/appstore"
#else
#define DEFAULT_REPO "https://switchbru.com/appstore"
#endif

#define DEFAULT_GET_HOME "./.get/"

void quit();
