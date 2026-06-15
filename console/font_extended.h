/**
 * Extended Font for Recovery Mode (Placeholder)
 * 
 * To create the actual file, run: python3 generate_extended_font.py
 */

#include <map>

static const std::map<int, const unsigned char*> font_8x16 = { };
static const std::map<int, const unsigned char*> font_16x16 = { };

inline const unsigned char* fontExtendedLookup(int, int) {
    return nullptr;
}
