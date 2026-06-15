#!/usr/bin/env python3

# This tool scans all i18n language .ini files to find characters that don't exist
# in the basic ASCII 8x8 font, then it extracts those glyphs from GNU Unifont to build a C header
#  
# The unifont .hex format is:
# UNICODE_CODEPOINT:HEX_BITMAP_DATA
# 
# Where the bitmap data is either:
# - 32 hex digits for 8x16 glyphs (narrow)
# - 64 hex digits for 16x16 glyphs (wide)
# 
# This allows tthe HBAS binary to save storage space by only including the pixel glpyths that will actually be displayed

import os
import sys
from pathlib import Path

def read_i18n_files(i18n_dir):
    chars = set()
    
    for ini_file in Path(i18n_dir).glob("*.ini"):
        try:
            with open(ini_file, 'r', encoding='utf-8') as f:
                for line in f:
                    if line.startswith(';') or line.startswith('['):
                        continue
                    if '=' in line:
                        value = line.split('=', 1)[1].strip()
                        chars.update(value)
        except Exception as e:
            print(f"Warning: Error reading {ini_file}: {e}", file=sys.stderr)
    
    return chars

def read_unifont_hex(hex_file):
    # returns a dict of codepoint -> hex bitmap
    glyphs = {}
    
    with open(hex_file, 'r') as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            
            try:
                codepoint_str, bitmap_hex = line.split(':', 1)
                codepoint = int(codepoint_str, 16)
                glyphs[codepoint] = bitmap_hex
            except Exception as e:
                print(f"Warning: Error parsing line '{line}': {e}", file=sys.stderr)
    
    return glyphs

def hex_to_bytes(hex_str):
    return [int(hex_str[i:i+2], 16) for i in range(0, len(hex_str), 2)]

def generate_glyph(hex_bitmap):
    bitmap_bytes = hex_to_bytes(hex_bitmap)
    
    if len(bitmap_bytes) == 16: # 8x16 narrow glyph
        return (bitmap_bytes, 8)
    elif len(bitmap_bytes) == 32: # 16x16 wide glyph
        return (bitmap_bytes, 16)
    else:
        print(f"Warning: Unexpected bitmap size {len(bitmap_bytes)}", file=sys.stderr)
        return ([0x00] * 16, 8)

def main():
    script_dir = Path(__file__).parent
    repo_root = script_dir.parent
    i18n_dir = repo_root / "resin" / "res" / "i18n"
    hex_file = repo_root / "assets" / "unifont_sample-17.0.03.hex"
    output_file = repo_root / "console" / "font_extended.h"
    
    print(f"Scanning i18n files in: {i18n_dir}")
    print(f"Reading unifont from: {hex_file}")
    
    # read all characters from i18n files
    all_chars = read_i18n_files(i18n_dir)
    print(f"Found {len(all_chars)} unique characters in i18n files")
    
    # convert to codepoints
    codepoints = sorted([ord(c) for c in all_chars])
    
    # read unifont glyphs
    print(f"Reading unifont glyphs...")
    unifont_glyphs = read_unifont_hex(hex_file)
    print(f"Loaded {len(unifont_glyphs)} glyphs from unifont")
    
    # extract glyphs only for our needed characters
    extended_glyphs = {}
    missing_glyphs = []
    
    for codepoint in codepoints:
        if codepoint in unifont_glyphs:
            hex_bitmap = unifont_glyphs[codepoint]
            glyph_data, width = generate_glyph(hex_bitmap)
            extended_glyphs[codepoint] = (glyph_data, width)
        else:
            missing_glyphs.append(codepoint)
            # Use a placeholder (empty 8x16 glyph)
            extended_glyphs[codepoint] = ([0x00] * 16, 8)
    
    if missing_glyphs:
        print(f"Warning: {len(missing_glyphs)} glyphs not found in unifont:")
        for cp in missing_glyphs[:20]:  # Show first 20
            print(f"  U+{cp:04X} ('{chr(cp)}')")
        if len(missing_glyphs) > 20:
            print(f"  ... and {len(missing_glyphs) - 20} more")
    
    # generate output header file
    print(f"Generating {output_file}...")
    glyphs_8x16 = {cp: data for cp, (data, width) in extended_glyphs.items() if width == 8}
    glyphs_16x16 = {cp: data for cp, (data, width) in extended_glyphs.items() if width == 16}
    
    with open(output_file, 'w', encoding='utf-8') as f:
        f.write("""/**
 * Extended Font for Recovery Mode
 * Auto-generated from GNU Unifont by generate_extended_font.py
 */

#include <map>

""")
        
        f.write(f"// Extended font with {len(extended_glyphs)} glyphs total\n")
        f.write(f"// - {len(glyphs_8x16)} glyphs at 8x16 (narrow)\n")
        f.write(f"// - {len(glyphs_16x16)} glyphs at 16x16 (wide/CJK)\n")
        f.write("// Unicode range: ")
        if codepoints:
            f.write(f"U+{min(codepoints):04X} - U+{max(codepoints):04X}\n")
        else:
            f.write("(empty)\n")
        f.write("\n")
        
        f.write("// 8x16 glyphs (16 bytes each)\n")
        f.write("static const std::map<int, const unsigned char*> font_8x16 = {\n")
        
        sorted_8x16 = sorted(glyphs_8x16.keys())
        for i, codepoint in enumerate(sorted_8x16):
            glyph = glyphs_8x16[codepoint]
            char_str = chr(codepoint)
            if char_str in ['"', '\\']:
                char_str = '\\' + char_str
            
            hex_bytes = ', '.join(f'0x{b:02X}' for b in glyph)
            f.write(f'    {{0x{codepoint:04X}, (const unsigned char[]){{{hex_bytes}}}}}')
            
            if i < len(sorted_8x16) - 1:
                f.write(',')
            
            try:
                f.write(f'  // U+{codepoint:04X} {char_str}\n')
            except:
                f.write(f'  // U+{codepoint:04X}\n')
        
        f.write("};\n\n")

        f.write("// 16x16 glyphs (32 bytes each, 2 bytes per row)\n")
        f.write("static const std::map<int, const unsigned char*> font_16x16 = {\n")
        
        sorted_16x16 = sorted(glyphs_16x16.keys())
        for i, codepoint in enumerate(sorted_16x16):
            glyph = glyphs_16x16[codepoint]
            char_str = chr(codepoint)
            if char_str in ['"', '\\']:
                char_str = '\\' + char_str
            
            hex_bytes = ', '.join(f'0x{b:02X}' for b in glyph)
            f.write(f'    {{0x{codepoint:04X}, (const unsigned char[]){{{hex_bytes}}}}}')
            
            if i < len(sorted_16x16) - 1:
                f.write(',')
            
            try:
                f.write(f'  // U+{codepoint:04X} {char_str}\n')
            except:
                f.write(f'  // U+{codepoint:04X}\n')
        
        f.write("};\n\n")
        
        f.write("""
// Returns bitmap and sets width (8 or 16) via reference parameter, or nullptr if not found
inline const unsigned char* fontExtendedLookup(int codepoint, int& width) {
    auto it8 = font_8x16.find(codepoint);
    if (it8 != font_8x16.end()) {
        width = 8;
        return it8->second;
    }
    auto it16 = font_16x16.find(codepoint);
    if (it16 != font_16x16.end()) {
        width = 16;
        return it16->second;
    }
    
    width = 0;
    return nullptr;
}
""")
    print("Done!")

if __name__ == "__main__":
    main()
