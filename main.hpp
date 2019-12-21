#pragma once

#if defined(__WIIU__)
#define DEFAULT_REPO "https://wiiubru.com/appstore"
#else
#define DEFAULT_REPO "https://switchbru.com/appstore"
#endif

void quit();
