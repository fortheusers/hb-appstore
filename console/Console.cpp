#include "Console.hpp"
#include <cstring>
#include "font.h"
#include "font_extended.h"
#include "../libs/chesto/src/TextElement.hpp"

using namespace Chesto;

Console::Console(CST_Window* window)
{
	this->window = window;
	this->window_surface = SDL_GetWindowSurface(window);

	// make background black
	this->background(0x42, 0x45, 0x48);
}

void Console::close()
{
	SDL_Delay(1);
}

void Console::background(int r, int g, int b)
{
	SDL_FillRect(this->window_surface, NULL, SDL_MapRGBA(this->window_surface->format, r, g, b, 0xFF));
}

void Console::update()
{
	SDL_UpdateWindowSurface(this->window);
}

void Console::fillRect(int x, int y, int width, int height, int r, int g, int b)
{
	SDL_Rect rect;
	rect.x = x * 2 * 8;
	rect.y = y * 2 * 8;
	rect.w = width * 2 * 8;
	rect.h = height * 2 * 8;

	SDL_FillRect(this->window_surface, &rect, SDL_MapRGBA(this->window_surface->format, b, g, r, 0xFF));
}

// puts a 3x3 "pixel" on screen at the given x, y with the given r, g, b
// will need update() called after to display any pixels that were created
// the switch resolution is 1280x720, but the "pixels" used by this grid are 426x240
void Console::putAPixel(int x, int y, int r, int g, int b)
{
	// int num = (r << 24) | (g << 16) | (b << 8) | 0;
	x *= 2;
	y *= 2;

	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = 2;
	rect.h = 2;

	SDL_FillRect(this->window_surface, &rect, SDL_MapRGBA(this->window_surface->format, b, g, r, 0xFF));
}

// draws a string using our bitmap font (below) at the given xi, yi
// the xi, yi given are on a 53x30 grid, (8x less than the "pixel" grid)
// optional: provide r, g, b values, otherwise white
void Console::drawString(int xi, int yi, const std::string& string)
{
	this->drawColorString(xi, yi, string, 0xff, 0xff, 0xff);
}

void Console::drawColorString(int xi, int yi, const std::string& string, int r, int g, int b)
{
	// for every character in the string, if it's within range, render it at the current position
	// and move over 8 characters

	xi *= 8;
	yi *= 8;

	int xOffset = 0;
	const unsigned char* str = (const unsigned char*)string.c_str();
	
	while (*str != '\0')
	{
		int codepoint = 0;
		int bytes = 0;
		
		// decode UTF-8 character
		if ((*str & 0x80) == 0) {
			// single-byte ASCII (0xxxxxxx)
			codepoint = *str;
			bytes = 1;
		} else if ((*str & 0xE0) == 0xC0) {
			// two-byte sequence (110xxxxx 10xxxxxx)
			if ((str[1] & 0xC0) == 0x80) {
				codepoint = ((*str & 0x1F) << 6) | (str[1] & 0x3F);
				bytes = 2;
			}
		} else if ((*str & 0xF0) == 0xE0) {
			// three-byte sequence (1110xxxx 10xxxxxx 10xxxxxx)
			if ((str[1] & 0xC0) == 0x80 && (str[2] & 0xC0) == 0x80) {
				codepoint = ((*str & 0x0F) << 12) | ((str[1] & 0x3F) << 6) | (str[2] & 0x3F);
				bytes = 3;
			}
		} else if ((*str & 0xF8) == 0xF0) {
			// four-byte sequence (11110xxx 10xxxxxx 10xxxxxx 10xxxxxx)
			if ((str[1] & 0xC0) == 0x80 && (str[2] & 0xC0) == 0x80 && (str[3] & 0xC0) == 0x80) {
				codepoint = ((*str & 0x07) << 18) | ((str[1] & 0x3F) << 12) | ((str[2] & 0x3F) << 6) | (str[3] & 0x3F);
				bytes = 4;
			}
		}
		
		// if decoding failed, skip this byte and try next
		if (bytes == 0) {
			str++;
			continue;
		}
		
	// get the bitmap for this character
	const unsigned char* bitmap = nullptr;
	int glyphWidth = 8;
	int glyphHeight = 8;
	
	// use original 8x8 font for en-us only, unifont for all other languages
	bool useOriginalFont = (TextElement::curLang == "en-us");
	
	if (useOriginalFont && codepoint < 128) {
		// use the original 8x8 ASCII font for en-us
		bitmap = font[codepoint];
		glyphWidth = 8;
		glyphHeight = 8;
	} else {
		// use extended font (unifont) for all other languages
		bitmap = (const unsigned char*)fontExtendedLookup(codepoint, glyphWidth);
		if (bitmap != nullptr) {
			glyphHeight = 16;  // unifont glyphs are always 16 tall
		} else if (codepoint < 128) {
			// fallback
			bitmap = font[codepoint];
			glyphWidth = 8;
			glyphHeight = 8;
		}
	}
	
	// actually draw the character
	if (bitmap != nullptr) {
		// handle different widths
		if (glyphWidth == 8 && glyphHeight == 16) {
			for (int row = 0; row < glyphHeight; row++) {
				unsigned char rowByte = bitmap[row];
				for (int col = 0; col < 8; col++) {
					// Unifont: Bit 7 is leftmost pixel, bit 0 is rightmost
					if (rowByte & (1 << (7 - col))) {
						this->putAPixel(xi + xOffset + col, yi + row, b, g, r);
					}
				}
			}
			xOffset += 8;
		} else if (glyphWidth == 8 && glyphHeight == 8) {
			for (int row = 0; row < glyphHeight; row++) {
				unsigned char rowByte = bitmap[row];
				for (int col = 0; col < 8; col++) {
					// small font: Bit 0 is leftmost pixel, bit 7 is rightmost
					if (rowByte & (1 << col)) {
						this->putAPixel(xi + xOffset + col, yi + row, b, g, r);
					}
				}
			}
			xOffset += 8;
		} else if (glyphWidth == 16) {
			for (int row = 0; row < glyphHeight; row++) {
				unsigned char byte0 = bitmap[row * 2];
				unsigned char byte1 = bitmap[row * 2 + 1];
				unsigned short rowBits = (byte0 << 8) | byte1;
				
				for (int col = 0; col < 16; col++) {
					// bit 15 is leftmost pixel, bit 0 is rightmost
					if (rowBits & (1 << (15 - col))) {
						this->putAPixel(xi + xOffset + col, yi + row, b, g, r);
					}
				}
			}
			xOffset += 16;
		}
	}
		
		// advance
		str += bytes;
	}
}

unsigned char* fontLookup(char c)
{
	return font[((unsigned char)c) % 128];
}

void sleep(int s)
{
	SDL_Delay(s * 1000);
}
