#pragma once

// the meta-repo can be used to change or add more repos
#define META_REPO_1 "https://fortheusers.github.io/meta-repo"
#define META_REPO_2 "https://meta.fortheusers.org"

// 4tu hardcoded repos as of this release, for wiiu/switch
#define   WIIU_REPO "https://wiiu.cdn.fortheusers.org"
#define SWITCH_REPO "https://switch.cdn.fortheusers.org"

// third party repos maintained by other teams for 3ds/wii
#define   _3DS_REPO "https://db.universal-team.net/data/full.json"
#define    WII_REPO "https://hbb1.oscwii.org/api/v3/contents"

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