#pragma once
#include <switch.h>
#include <string>
#include <vector>

class MarioMakerLevel
{
    public:
    MarioMakerLevel(u32 index)
    {
        this->index = index;
    }
    void setName()
    {
        u16 *txt16 = (u16*)&this->course[0xf4];
        char lname[0x20 + 1] = {0};
        utf16_to_utf8((u8*)lname, txt16, 0x20);
        this->levelName = std::string(lname);
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