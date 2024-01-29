#pragma once

#include <cstdint>
#include <string>
#include <algorithm>

#include "SDL.h"

// Some utility to use SDL colors better
struct Color {
	uint8_t r = 0, g = 0, b = 0, a = 255;

	bool operator==(const Color& other) {
		return r == other.r && g == other.g && b == other.b && a == other.a;
	}

	void lighten(float amount) {
		r = static_cast<uint8_t>(std::min((int)(amount * r), 255));
		g = static_cast<uint8_t>(std::min((int)(amount * g), 255));
		b = static_cast<uint8_t>(std::min((int)(amount * b), 255));
	}

	const SDL_Color sdl() const {
		return { r, g, b, a };
	}
};

inline Color lerp(Color a, Color b, float bias) {
	return Color{
		static_cast<uint8_t>(a.r * (1.f - bias) + b.r * bias),
		static_cast<uint8_t>(a.g * (1.f - bias) + b.g * bias),
		static_cast<uint8_t>(a.b * (1.f - bias) + b.b * bias),
		static_cast<uint8_t>(a.a * (1.f - bias) + b.a * bias)
	};
}

// Let's set up some color names!
constexpr Color c_white = { 255, 255, 255 };
constexpr Color c_black = { 0, 0, 0 };
constexpr Color c_red = { 255, 0, 0 };
constexpr Color c_green = { 0, 255, 0 };
constexpr Color c_blue = { 0, 0, 255 };
constexpr Color c_cyan = { 0, 255, 255 };
constexpr Color c_magenta = { 255, 0, 255 };
constexpr Color c_yellow = { 255, 255, 0 };
constexpr Color c_orange = { 255, 150, 0 };
constexpr Color c_nothing = { 255, 255, 255, 0 };



Color get_draw_color(SDL_Renderer* renderer);
void set_draw_color(SDL_Renderer* renderer, Color c);

constexpr uint8_t hex_parse(const char byte) {
	if (byte >= '0' && byte <= '9') return byte - '0';
	if (byte >= 'A' && byte <= 'F') return (byte - 'A' + 10);
	if (byte >= 'a' && byte <= 'f') return (byte - 'a' + 10);
}

constexpr uint8_t parse_hex_pair(const char pair[2]) {
	return (hex_parse(pair[0]) << 4) | hex_parse(pair[1]);
}

constexpr const Color from_hex(const char hex[7]) {
	return Color{ parse_hex_pair(hex), parse_hex_pair(hex + 2), parse_hex_pair(hex + 4), 255};
}

#define HEX(x) from_hex(#x)

constexpr Color c_bg = HEX(1d1f21);
constexpr Color c_fg = HEX(c5c8c6);

constexpr Color c_0 = HEX(282a2e);
constexpr Color c_1 = HEX(a54242);
constexpr Color c_2 = HEX(8c9440);
constexpr Color c_3 = HEX(de935f);
constexpr Color c_4 = HEX(5f819d);
constexpr Color c_5 = HEX(85678f);
constexpr Color c_6 = HEX(5e8d87);
constexpr Color c_7 = HEX(707880);
constexpr Color c_8 = HEX(373b41);
constexpr Color c_9 = HEX(cc6666);
constexpr Color c_10 = HEX(b5bd68);
constexpr Color c_11 = HEX(f0c674);
constexpr Color c_12 = HEX(81a2be);
constexpr Color c_13 = HEX(b294bb);
constexpr Color c_14 = HEX(8abeb7);
constexpr Color c_15 = HEX(c5c8c6);

constexpr Color c_board_bg = c_8;