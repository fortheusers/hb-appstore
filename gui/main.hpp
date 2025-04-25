#pragma once

// the meta-repo can be used to change or add more repos
// #define   META_REPO "https://meta.fortheusers.org"
#define   META_REPO "https://fortheusers.github.io/meta-repo"

// hardcoded known repos as of this release
#define   WIIU_REPO "https://wiiu.cdn.fortheusers.org"
#define SWITCH_REPO "https://switch.cdn.fortheusers.org"
#define   _3DS_REPO "https://3ds.apps.fortheusers.org"
#define    WII_REPO "https://hbb1.oscwii.org"

// older DEFAULT_REPO variable used at first time launch or reset
// TODO: do this programmatically not at compile time
#if defined(SWITCH)
#define DEFAULT_REPO SWITCH_REPO
#elif defined(WII)
#define DEFAULT_REPO WII_REPO
#elif defined(_3DS)
#define DEFAULT_REPO _3DS_REPO
#else
#define DEFAULT_REPO WIIU_REPO
#endif

// preference paths
#define SOUND_PATH "./.toggle_sound"
#define DEFAULT_GET_HOME "./.get/"