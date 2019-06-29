#pragma once
#include <switch.h>
#include <string>
#include <vector>

class MarioMakerLevel
{
    public:
    std::string levelName;
    u32 index;
    char thumb[0x1C000];
    char course[0x5C000];
    char replay[0x68000];
};

class MarioMaker
{
    FsFileSystem smm2fs;
    char profileIcon[64000];
    size_t profileSize;
    void dumpLevels();
    public:
    std::vector<MarioMakerLevel> levels;
    std::string username;
    MarioMaker();
    std::vector<MarioMakerLevel> listLevels();
    Result failResult = 0;
    ~MarioMaker();
};