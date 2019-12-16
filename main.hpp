#pragma once

#if defined(__WIIU__)
#define DEFAULT_REPO "http://wiiubru.com/appstore"
#else
#define DEFAULT_REPO "http://switchbru.com/appstore"
#endif

void quit();
