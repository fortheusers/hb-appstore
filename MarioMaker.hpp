#pragma once
#include <switch.h>
#include <string.h>
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
        memcpy(this->thumb, thumb, 0x1C000);
        memcpy(this->course, course, 0x5C000);
        this->levelName = levelName;
        this->index = index;
    }
    std::string levelName;
    u32 index;
    char thumb[0x1C000] = {0};
    char course[0x5C000] = {0};
    char replay[0x68000] = {0};
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