#pragma once
#include <switch.h>
#include <string>
#include <vector>

class MarioMakerLevel
{
    public:
    MarioMakerLevel(std::string levelName, u32 index,
    char thumb[0x1C000],
    char course[0x5C000],
    char replay[0x68000])
    {
        this->levelName = levelName;
        this->index = index;
        this->thumb = thumb;
        this->course = course;
        this->replay = replay;
    }
    std::string levelName;
    u32 index;
    char *thumb;
    char *course;
    char *replay;
};

class MarioMaker
{
    FsFileSystem smm2fs;
    char profileIcon[64000];
    size_t profileSize;
    void dumpLevels();
    public:
    std::vector<MarioMakerLevel*> levels;
    std::string username;
    MarioMaker();
    Result failResult = 0;
    ~MarioMaker();
};